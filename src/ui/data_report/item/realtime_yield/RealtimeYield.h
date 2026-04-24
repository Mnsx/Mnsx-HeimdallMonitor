#ifndef HEIMDALLMONITOR_REALTIMEYIELD_H
#define HEIMDALLMONITOR_REALTIMEYIELD_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTimer>
#include <QtCharts/QValueAxis>

QT_BEGIN_NAMESPACE
namespace Ui { class RealtimeYield; }
QT_END_NAMESPACE

class RealtimeYield : public QWidget {
    Q_OBJECT

public:
    explicit RealtimeYield(QWidget *parent = nullptr);
    ~RealtimeYield() override;

private:
    Ui::RealtimeYield *ui;

    void initChart();

    QStandardItemModel* yieldModel_;
    QTimer* updateTimer_;
    int currentTimeStep_; // 记录X轴的推移时间
    QValueAxis* axisX_;   // 必须保存X轴指针，以便在心跳中动态推移它
};

#endif //HEIMDALLMONITOR_REALTIMEYIELD_H