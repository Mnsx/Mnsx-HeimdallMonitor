#include "RealtimeYield.h"
#include "ui_RealtimeYield.h"

#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QVXYModelMapper>
#include <QToolTip>

#include "../../../../core/adapter/ValkyrieAdapter.h"

RealtimeYield::RealtimeYield(QWidget *parent) : QWidget(parent), ui(new Ui::RealtimeYield), currentTimeStep_(0) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("background-color: transparent;");
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

    QPen splinePen(QColor("#00FFCC"));
    splinePen.setWidth(3);
    series->setPen(splinePen);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("产线实时良率心电图");

    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setBackgroundVisible(false);           // 隐藏外层背景
    chart->setPlotAreaBackgroundVisible(false);   // 隐藏内层网格区域背景
    chart->setBackgroundBrush(Qt::transparent);   // 强制外层笔刷为透明
    chart->setPlotAreaBackgroundBrush(Qt::transparent); // 强制内层笔刷为透明

    chart->setBackgroundVisible(false);
    chart->setBackgroundRoundness(0);
    QFont titleFont("Consolas", 14, QFont::Bold);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(QColor("#E0E0E0")));
    chart->legend()->hide();

    QVXYModelMapper* mapper = new QVXYModelMapper(this);
    mapper->setModel(dataModel);
    mapper->setSeries(series);
    mapper->setXColumn(0);
    mapper->setYColumn(1);

    QFont axisFont("Consolas", 10, QFont::Bold);
    QPen gridPen(QColor("#333333"), 1, Qt::DashLine);
    QPen axisLinePen(QColor("#555555"), 2);
    QColor labelColor("#AAAAAA");

    axisX_ = new QValueAxis();
    axisX_->setTitleText("时间推进 (s)");
    axisX_->setLabelsFont(axisFont);
    axisX_->setLabelsColor(labelColor);
    axisX_->setTitleFont(axisFont);
    axisX_->setTitleBrush(QBrush(labelColor));

    axisX_->setGridLinePen(gridPen);     // 应用虚线网格
    axisX_->setLinePen(axisLinePen);     // 应用金属主干线

    axisX_->setLabelFormat("%d");
    axisX_->setRange(currentTimeStep_ - 20, currentTimeStep_);
    chart->addAxis(axisX_, Qt::AlignBottom);
    series->attachAxis(axisX_);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("良率 (%)");
    axisY->setLabelsFont(axisFont);
    axisY->setLabelsColor(labelColor);
    axisY->setTitleFont(axisFont);
    axisY->setTitleBrush(QBrush(labelColor));
    axisY->setGridLinePen(gridPen);
    axisY->setLinePen(axisLinePen);
    axisY->setRange(95.0, 100.5);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet(
        "QToolTip { color: #00FFCC; background-color: #1A1A1A; border: 1px solid #555555; font-family: Consolas; font-size: 13px; padding: 4px; }"
        "background: transparent; border: none;"
    );
    connect(series, &QSplineSeries::hovered, this, [chartView](const QPointF &point, bool state) {
        if (state) {
            // 格式化输出：截取2位小数，展现高精度
            QString text = QString("时间: %1 s\n良率: %2 %").arg(point.x(), 0, 'f', 0).arg(point.y(), 0, 'f', 2);
            // 在鼠标全局物理坐标处弹出科技感悬浮窗
            QToolTip::showText(QCursor::pos(), text, chartView);
        } else {
            // 鼠标移开时立即隐藏
            QToolTip::hideText();
        }
    });

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
        yieldModel_->setData(yieldModel_->index(rowCount, 1), res * 100);

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