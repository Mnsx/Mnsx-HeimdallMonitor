#ifndef HEIMDALLMONITOR_PARTSPERMILLION_H
#define HEIMDALLMONITOR_PARTSPERMILLION_H

#include <QWidget>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class PartsPerMillion; }
QT_END_NAMESPACE

class PartsPerMillion : public QWidget {
    Q_OBJECT

public:
    explicit PartsPerMillion(QWidget *parent = nullptr);
    ~PartsPerMillion() override;

private:
    Ui::PartsPerMillion *ui;

    void initChart();
    QStandardItemModel* ppmModel_;
};

#endif //HEIMDALLMONITOR_PARTSPERMILLION_H