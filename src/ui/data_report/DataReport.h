/**
* @file datareport.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/24
 * @description
 */
//
// Created by Mnsx on 2026/4/24.
//

#ifndef HEIMDALLMONITOR_DATAREPORT_H
#define HEIMDALLMONITOR_DATAREPORT_H

#include <QWidget>
#include "item/realtime_yield/RealtimeYield.h"
#include "item/defect_distribution/DefectDistribution.h"
#include "item/parts_per_million/PartsPerMillion.h"
#include "item/false_positive_rate/FalsePositiveRate.h"

class DataReport : public QWidget {
    Q_OBJECT

public:
    explicit DataReport(QWidget *parent = nullptr);

    ~DataReport() override;

private:
};


#endif //HEIMDALLMONITOR_DATAREPORT_H