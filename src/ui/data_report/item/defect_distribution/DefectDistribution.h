/** 
 * @file DefectDistribution.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/24
 * @description 
 */
//
// Created by Mnsx on 2026/4/24.
//

#ifndef HEIMDALLMONITOR_DEFECTDISTRIBUTION_H
#define HEIMDALLMONITOR_DEFECTDISTRIBUTION_H

#include <QStandardItemModel>
#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui {
    class DefectDistribution;
}

QT_END_NAMESPACE

class DefectDistribution : public QWidget {
    Q_OBJECT

public:
    explicit DefectDistribution(QWidget *parent = nullptr);

    ~DefectDistribution() override;

    void initChart();

    void addCount(int count_id);

private:
    Ui::DefectDistribution *ui;

    QStandardItemModel* defectModel_;

    int count1_ = 0;
    int count2_ = 0;
    int count3_ = 0;
    int count4_ = 0;
};


#endif //HEIMDALLMONITOR_DEFECTDISTRIBUTION_H