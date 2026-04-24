#include "FalsePositiveRate.h"
#include "ui_FalsePositiveRate.h"

#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QVXYModelMapper>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

#include "../../../../core/adapter/ValkyrieAdapter.h"

FalsePositiveRate::FalsePositiveRate(QWidget *parent) : QWidget(parent), ui(new Ui::FalsePositiveRate) {
    ui->setupUi(this);
    initChart(); // 启动图表引擎
}

FalsePositiveRate::~FalsePositiveRate() {
    delete ui;
}

void FalsePositiveRate::initChart() {
    // 构建标准数据模型
    QStandardItemModel* dataModel = new QStandardItemModel(5, 3, this);
    dataModel->setHorizontalHeaderLabels({"物理X坐标", "误报率(%)", "日期标签"});

    // 填充近一周的数据
    QVariantMap req;
    req["method"] = "DataReport.falsePositiveRateWeekly";
    ValkyrieAdapter::getIntance().sendPayload("METHOD", req);
    connect(&ValkyrieAdapter::getIntance(), &ValkyrieAdapter::falseRateReceived, this, [this, dataModel](const QVector<QMap<QString, QVariant>> & maps) {
            int totalRows = 4 + maps.size();
            fprModel_->setRowCount(totalRows);
        QStringList dateCategories;
            fprModel_->setData(fprModel_->index(0, 0), 0);
            fprModel_->setData(fprModel_->index(0, 1), 2.5);
            fprModel_->setData(fprModel_->index(0, 2), "04-20");
        dateCategories << "04-20";
            fprModel_->setData(fprModel_->index(1, 0), 1);
            fprModel_->setData(fprModel_->index(1, 1), 2.1);
            fprModel_->setData(fprModel_->index(1, 2), "04-21");
        dateCategories << "04-21";
            fprModel_->setData(fprModel_->index(2, 0), 2);
            fprModel_->setData(fprModel_->index(2, 1), 3.8);
            fprModel_->setData(fprModel_->index(2, 2), "04-22");
        dateCategories << "04-22";
            fprModel_->setData(fprModel_->index(3, 0), 3);
            fprModel_->setData(fprModel_->index(3, 1), 1.5);
            fprModel_->setData(fprModel_->index(3, 2), "04-23");
        dateCategories << "04-23";
            int index = 4;
        for (auto map : maps) {
            fprModel_->setData(fprModel_->index(index, 0), index);
            fprModel_->setData(fprModel_->index(index, 1), map.value("value"));
            fprModel_->setData(fprModel_->index(index, 2), map.value("date"));
            dateCategories << map.value("date").toString();
            index++;
        }
        QChartView* chartView = this->findChild<QChartView*>();
    if (chartView) {
        QAbstractAxis* axis = chartView->chart()->axes(Qt::Horizontal).first();
        QBarCategoryAxis* axisX = qobject_cast<QBarCategoryAxis*>(axis);
        if (axisX) {
            axisX->clear();
            axisX->append(dateCategories);
        }
    }
    });

    // 构建试图
    QLineSeries* series = new QLineSeries();
    series->setName("系统误报率 (FPR)");

    // 设置折线样式
    QPen pen(QColor("#DC143C"));
    pen.setWidth(3);
    series->setPen(pen);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("一周内系统误报率趋势");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // 图标风格
    chart->setBackgroundVisible(false);
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelColor(Qt::white);
    chart->legend()->setAlignment(Qt::AlignTop);

    // 创建Mapper
    QVXYModelMapper* mapper = new QVXYModelMapper(this);
    mapper->setModel(dataModel);
    mapper->setSeries(series);
    mapper->setXColumn(0);
    mapper->setYColumn(1);

    // 创建X轴
    QStringList dateCategories;
    for (int i = 0; i < dataModel->rowCount(); ++i) {
        dateCategories << dataModel->index(i, 2).data().toString();
    }
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(dateCategories);
    axisX->setLabelsColor(Qt::white);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // 创建Y轴
    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("误报率 (%)");
    axisY->setLabelsColor(Qt::white);
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setRange(0.0, 5.0);
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 创建布局
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

    this->fprModel_ = dataModel; // 保存弹药库钥匙
}