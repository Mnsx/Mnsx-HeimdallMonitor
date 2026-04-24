#include "RealtimeYield.h"
#include "ui_RealtimeYield.h"

#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QVXYModelMapper>

#include "../../../../core/adapter/ValkyrieAdapter.h"

RealtimeYield::RealtimeYield(QWidget *parent) : QWidget(parent), ui(new Ui::RealtimeYield), currentTimeStep_(0) {
    ui->setupUi(this);
    initChart();
}

RealtimeYield::~RealtimeYield() {
    delete ui;
}

void RealtimeYield::initChart() {
    QStandardItemModel* dataModel = new QStandardItemModel(0, 2, this);
    dataModel->setHorizontalHeaderLabels({"时间步", "良率(%)"});

    for (int i = 0; i < 20; ++i) {
        dataModel->insertRow(i);
        dataModel->setData(dataModel->index(i, 0), currentTimeStep_);
        double initYield = 98.0 + QRandomGenerator::global()->generateDouble() * 2.0;
        dataModel->setData(dataModel->index(i, 1), initYield);
        currentTimeStep_++;
    }

    QSplineSeries* series = new QSplineSeries();
    series->setName("实时良率波动");

    QPen pen(QColor("#00FA9A"));
    pen.setWidth(3);
    series->setPen(pen);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("产线实时良率心电图");

    chart->setAnimationOptions(QChart::NoAnimation);

    chart->setBackgroundVisible(false);
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelColor(Qt::white);

    QVXYModelMapper* mapper = new QVXYModelMapper(this);
    mapper->setModel(dataModel);
    mapper->setSeries(series);
    mapper->setXColumn(0);
    mapper->setYColumn(1);

    axisX_ = new QValueAxis();
    axisX_->setTitleText("时间推进 (s)");
    axisX_->setLabelsColor(Qt::white);
    axisX_->setLabelFormat("%d");
    axisX_->setRange(currentTimeStep_ - 20, currentTimeStep_);
    chart->addAxis(axisX_, Qt::AlignBottom);
    series->attachAxis(axisX_);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("良率 (%)");
    axisY->setLabelsColor(Qt::white);
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setRange(95.0, 100.5);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent; border: none;");

    QLayout* currentLayout = this->layout();
    if (currentLayout == nullptr) {
        QVBoxLayout* newLayout = new QVBoxLayout(this);
        newLayout->setContentsMargins(0, 0, 0, 0);
        currentLayout = newLayout;
    }
    currentLayout->addWidget(chartView);
    this->yieldModel_ = dataModel;

    updateTimer_ = new QTimer(this);
    connect(&ValkyrieAdapter::getIntance(), &ValkyrieAdapter::realtimeYieldDataReceived, this, [this](double res) {

        int rowCount = yieldModel_->rowCount();
        yieldModel_->insertRow(rowCount);
        yieldModel_->setData(yieldModel_->index(rowCount, 0), currentTimeStep_);
        yieldModel_->setData(yieldModel_->index(rowCount, 1), 100 - res);

        if (rowCount > 20) {
            yieldModel_->removeRow(0);
        }

        axisX_->setRange(currentTimeStep_ - 20, currentTimeStep_);

        currentTimeStep_++;
    });
    connect(updateTimer_, &QTimer::timeout, this, [this]() {
        QVariantMap req;
        req["method"] = "DataReport.getRealtimeYield";
        ValkyrieAdapter::getIntance().sendPayload("METHOD", req);
    });

    updateTimer_->start(1000);
}