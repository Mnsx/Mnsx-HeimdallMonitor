/** 
 * @file ClusterManagePage.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 * @description
 */
#ifndef MNSX_HEIMDALLMONITOR_CLUSTERMANAGEPAGE_H
#define MNSX_HEIMDALLMONITOR_CLUSTERMANAGEPAGE_H

#include <QVBoxLayout>
#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>


QT_BEGIN_NAMESPACE

namespace Ui {
    class ClusterManagePage;
}

QT_END_NAMESPACE

class ClusterManagePage : public QWidget {
    Q_OBJECT

public:
    explicit ClusterManagePage(QWidget *parent = nullptr);

    ~ClusterManagePage() override;

    void updateServerStatus(bool isRunning);
    void addOrUpdateClient(int clientId, const QString& ip, const QString& status, const QString& lastHeartbeat);
    void removeClient(int clientId);

    void showTableContextMenu(const QPoint& pos);

signals:
    void requestStartServer(int port);
    void requestStopServer();

private:
    void setupServerControlUI(QVBoxLayout* mainLayout);
    void setupClientTableUI(QVBoxLayout* mainLayout);

    Ui::ClusterManagePage *ui;

    // 服务端控制组件
    QSpinBox* portSpinBox_;
    QPushButton* startBtn_;
    QPushButton* stopBtn_;
    QLabel* serverStatusLabel_;

    // 客户端列表组件
    QTableWidget* clientTable_;
};


#endif //MNSX_HEIMDALLMONITOR_CLUSTERMANAGEPAGE_H