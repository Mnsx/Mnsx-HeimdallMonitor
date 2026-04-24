/**
* @file datareport.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/24
 */
#include <qframe.h>
#include "DataReport.h"
#include <QGridlayout>
#include <QLabel>

DataReport::DataReport(QWidget *parent) : QWidget(parent) {
    QGridLayout* gridLayout = new QGridLayout(this);

    gridLayout->setSpacing(20);
    gridLayout->setContentsMargins(20, 20, 20, 20);

    // 左上：实时良率
    RealtimeYield* chartYield = new RealtimeYield(this);
    gridLayout->addWidget(chartYield, 0, 0);

    // 右上：缺陷分布
    DefectDistribution* chartDist = new DefectDistribution(this);
    gridLayout->addWidget(chartDist, 0, 1);

    // 左下：PPM 指标
    PartsPerMillion* chartPpm = new PartsPerMillion(this);
    gridLayout->addWidget(chartPpm, 1, 0);

    // 右下：误报率
    FalsePositiveRate* chartFpr = new FalsePositiveRate(this);
    gridLayout->addWidget(chartFpr, 1, 1);

    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setRowStretch(0, 1);
    gridLayout->setRowStretch(1, 1);
}

DataReport::~DataReport() {
}