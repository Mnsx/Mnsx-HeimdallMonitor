#ifndef HEIMDALLMONITOR_FALSEPOSITIVERATE_H
#define HEIMDALLMONITOR_FALSEPOSITIVERATE_H

#include <QWidget>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class FalsePositiveRate; }
QT_END_NAMESPACE

class FalsePositiveRate : public QWidget {
    Q_OBJECT

public:
    explicit FalsePositiveRate(QWidget *parent = nullptr);
    ~FalsePositiveRate() override;

private:
    Ui::FalsePositiveRate *ui;

    void initChart();
    QStandardItemModel* fprModel_;
};

#endif //HEIMDALLMONITOR_FALSEPOSITIVERATE_H