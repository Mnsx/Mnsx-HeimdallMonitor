#include "PartsPerMillion.h"
#include "ui_PartsPerMillion.h"

#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QVBarModelMapper>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

PartsPerMillion::PartsPerMillion(QWidget *parent) : QWidget(parent), ui(new Ui::PartsPerMillion) {
    ui->setupUi(this);
    initChart();
}

PartsPerMillion::~PartsPerMillion() {
    delete ui;
}

void PartsPerMillion::initChart() {
    // 构建标准model
    QStandardItemModel* dataModel = new QStandardItemModel(3, 2, this);
    dataModel->setHorizontalHeaderLabels({"视觉节点", "PPM数值"});
    dataModel->setData(dataModel->index(0, 0), "Achilles-01");
    dataModel->setData(dataModel->index(0, 1), 125);
    dataModel->setData(dataModel->index(1, 0), "Olympus-A");
    dataModel->setData(dataModel->index(1, 1), 85);
    dataModel->setData(dataModel->index(2, 0), "Olympus-B");
    dataModel->setData(dataModel->index(2, 1), 210);

    // 构建图表
    QBarSeries* series = new QBarSeries();
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("各节点PPM数据比对");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    // 图标风格
    chart->setBackgroundVisible(false);
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->hide();

    // 绑定数值
    QVBarModelMapper* mapper = new QVBarModelMapper(this);
    mapper->setModel(dataModel);
    mapper->setSeries(series);
    mapper->setFirstBarSetColumn(1);
    mapper->setLastBarSetColumn(1);

    // 构建X轴
    QStringList categories;
    for (int i = 0; i < dataModel->rowCount(); ++i) {
        categories << dataModel->index(i, 0).data().toString();
    }
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(Qt::white);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // 构建Y轴
    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("PPM");
    axisY->setLabelsColor(Qt::white);
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setRange(0, 300);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 布局装载
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

    this->ppmModel_ = dataModel;
}