/** 
 * @file DefectDistribution.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/24
 * @description 
 */
//
// Created by Mnsx on 2026/4/24.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DefectDistribution.h" resolved

#include "DefectDistribution.h"
#include "ui_DefectDistribution.h"

#include <QPieModelMapper>
#include "DefectDistribution.h"
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QVPieModelMapper>

#include "../../../../core/adapter/ValkyrieAdapter.h"

DefectDistribution::DefectDistribution(QWidget *parent) : QWidget(parent), ui(new Ui::DefectDistribution) {
    ui->setupUi(this);
    initChart();
}

DefectDistribution::~DefectDistribution() {
    delete ui;
}

void DefectDistribution::initChart() {
    // 构建model
    // 创建标准模型
    QStandardItemModel* dataModel = new QStandardItemModel(4, 2, this);
    dataModel->setHorizontalHeaderLabels({"缺陷类型", "发生频次"});
    // 填充初始模拟数据
    dataModel->setData(dataModel->index(0, 0), "斑点污块");
    dataModel->setData(dataModel->index(0, 1), count1_);
    dataModel->setData(dataModel->index(1, 0), "边缘断裂");
    dataModel->setData(dataModel->index(1, 1), count2_);
    dataModel->setData(dataModel->index(2, 0), "线条划痕");
    dataModel->setData(dataModel->index(2, 1), count3_);
    dataModel->setData(dataModel->index(3, 0), "尺寸超差");
    dataModel->setData(dataModel->index(3, 1), count4_);
    // 绘制图表
    QPieSeries* series = new QPieSeries();
    series->setHoleSize(0.35);
    series->setPieSize(0.8);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("缺陷类型分布");

    // 图标风格
    chart->setBackgroundVisible(false);
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelColor(Qt::white);
    chart->legend()->setAlignment(Qt::AlignRight);

    // 搭建Mapper
    QVPieModelMapper* mapper = new QVPieModelMapper(this);
    mapper->setModel(dataModel);    // 绑定数据源
    mapper->setSeries(series);      // 绑定显示器
    mapper->setLabelsColumn(0);     // 设置标签第零行
    mapper->setValuesColumn(1);     // 设置第一行

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent; border: none;");

    // 创建layout
    QLayout* currentLayout = this->layout();
    if (currentLayout == nullptr) {
        QVBoxLayout* newLayout = new QVBoxLayout(this);
        newLayout->setContentsMargins(0, 0, 0, 0); // 取消边距，让图表彻底撑满
        currentLayout = newLayout;
    }
    currentLayout->addWidget(chartView);
    this->layout()->addWidget(chartView);

    this->defectModel_ = dataModel;

    connect(&ValkyrieAdapter::getIntance(), &ValkyrieAdapter::typeCountAdd, this, &DefectDistribution::addCount);
}

void DefectDistribution::addCount(int count_id) {
    if (count_id == 1) {
        count1_++;
        defectModel_->setData(defectModel_->index(0, 1), count1_);
    } else if (count_id == 2) {
        count2_++;
        defectModel_->setData(defectModel_->index(1, 1), count2_);
    } else if (count_id == 3) {
        count3_++;
        defectModel_->setData(defectModel_->index(2, 1), count3_);
    } else if (count_id == 4) {
        count4_++;
        defectModel_->setData(defectModel_->index(3, 1), count4_);
    }
}
