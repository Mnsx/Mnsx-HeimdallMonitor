/** 
 * @file ClusterManagePage.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 */
#include "ClusterManagePage.h"
#include "../../core/adapter/ValkyrieAdapter.h"
#include "ui_ClusterManagePage.h"

#include <QGroupBox>
#include <QHeaderView>
#include <QMenu>

ClusterManagePage::ClusterManagePage(QWidget *parent) : QWidget(parent), ui(new Ui::ClusterManagePage) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(15);

    setupServerControlUI(mainLayout);
    setupClientTableUI(mainLayout);

    // 启动时获取数据库中存储的已经连接的集群
    connect(&ValkyrieAdapter::getIntance(), &ValkyrieAdapter::clustDataReceived, this, [this](const QVector<QVariantMap> datas) {
        int row_count = clientTable_->rowCount();
        for (int i = 0; i < row_count; ++i) {
            clientTable_->removeRow(i);
        }
        for (auto data : datas) {
            addOrUpdateClient(data.value("mac_address").toString(), data.value("ip_address").toString(),
                data.value("node_name").toString(), data.value("status").toString(), data.value("last_heartbeat_time").toString());
        }
    });
}

void ClusterManagePage::setupClientTableUI(QVBoxLayout* mainLayout) {
    QGroupBox* clientGroup = new QGroupBox("Valkyrie 视觉节点管控", this);
    QVBoxLayout* groupLayout = new QVBoxLayout(clientGroup);

    clientTable_ = new QTableWidget(0, 5, this);
    clientTable_->setHorizontalHeaderLabels({"节点MAC地址", "节点IP地址", "节点名称", "当前状态", "最后心跳时间"});

    clientTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    clientTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    clientTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    clientTable_->setStyleSheet("QTableWidget { background-color: #1E1E1E; color: white; gridline-color: #333333; }"
                                "QHeaderView::section { background-color: #2D2D30; color: white; padding: 5px; border: 1px solid #333333; }");
    clientTable_->verticalHeader()->setVisible(false);

    groupLayout->addWidget(clientTable_);
    mainLayout->addWidget(clientGroup);

    clientTable_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(clientTable_, &QTableWidget::customContextMenuRequested,
            this, &ClusterManagePage::showTableContextMenu);
}

void ClusterManagePage::setupServerControlUI(QVBoxLayout* mainLayout) {
    QGroupBox* serverGroup = new QGroupBox("Valkyrie 核心终端管控", this);
    QHBoxLayout* groupLayout = new QHBoxLayout(serverGroup);

    portSpinBox_ = new QSpinBox(this);
    portSpinBox_->setRange(1024, 65535);
    portSpinBox_->setValue(8080);
    portSpinBox_->setFixedWidth(100);

    startBtn_ = new QPushButton("启动服务核心", this);
    stopBtn_ = new QPushButton("切断通讯", this);
    stopBtn_->setEnabled(false);

    serverStatusLabel_ = new QLabel("⚫ 离线", this);
    serverStatusLabel_->setStyleSheet("color: gray; font-weight: bold; font-size: 14px;");

    groupLayout->addWidget(new QLabel("监听端口:"));
    groupLayout->addWidget(portSpinBox_);
    groupLayout->addSpacing(20);
    groupLayout->addWidget(startBtn_);
    groupLayout->addWidget(stopBtn_);
    groupLayout->addSpacing(30);
    groupLayout->addWidget(serverStatusLabel_);
    groupLayout->addStretch();

    mainLayout->addWidget(serverGroup);

    connect(startBtn_, &QPushButton::clicked, [this]() {
        updateServerStatus(true);
        // emit requestStartServer(portSpinBox_->value());
    });
    connect(stopBtn_, &QPushButton::clicked, [this]() {
        // emit requestStopServer();
        updateServerStatus(false);
    });
}

ClusterManagePage::~ClusterManagePage() {
    delete ui;
}

void ClusterManagePage::updateServerStatus(bool isRunning) {
    startBtn_->setEnabled(!isRunning);
    stopBtn_->setEnabled(isRunning);
    portSpinBox_->setEnabled(!isRunning);

    if (isRunning) {
        serverStatusLabel_->setText("🟢 运行中");
        serverStatusLabel_->setStyleSheet("color: #00FF00; font-weight: bold; font-size: 14px;");
    } else {
        serverStatusLabel_->setText("⚫ 离线");
        serverStatusLabel_->setStyleSheet("color: gray; font-weight: bold; font-size: 14px;");
    }
}

void ClusterManagePage::addOrUpdateClient(const QString& mac_addr, const QString& ip, const QString& node_name, const QString& status, const QString& lastHeartbeat) {
    int row = clientTable_->rowCount();
    clientTable_->insertRow(row);

    QString status_str = status == "1" ? "在线" : "离线";

    // 插入数据
    clientTable_->setItem(row, 0, new QTableWidgetItem(mac_addr));
    clientTable_->setItem(row, 1, new QTableWidgetItem(ip));
    clientTable_->setItem(row, 2, new QTableWidgetItem(node_name));
    clientTable_->setItem(row, 3, new QTableWidgetItem(status_str));
    clientTable_->setItem(row, 4, new QTableWidgetItem(lastHeartbeat));

    // 文字居中
    for(int i=0; i<4; ++i) {
        clientTable_->item(row, i)->setTextAlignment(Qt::AlignCenter);
    }
}

void ClusterManagePage::removeClient(int clientId) {
}

void ClusterManagePage::showTableContextMenu(const QPoint &pos) {
    QTableWidgetItem* item = clientTable_->itemAt(pos);
    if (!item) return;

    int row = item->row();
    QString nodeId = clientTable_->item(row, 0)->text();
    QString status = clientTable_->item(row, 3)->text();

    // 创建右键菜单
    QMenu menu(this);
    menu.setStyleSheet("QMenu { background-color: #2D2D30; color: white; border: 1px solid #555; }"
                       "QMenu::item:selected { background-color: #007ACC; }");

    QAction* removeAction = nullptr;
    if (status.contains("检测中") || status.contains("运行中")) {
        removeAction = menu.addAction("强制断开连接");
    } else {
        removeAction = menu.addAction("清理离线记录");
    }

    QAction* selectedAction = menu.exec(clientTable_->viewport()->mapToGlobal(pos));

    if (selectedAction == removeAction) {
        QVariantMap req;
        req["method"] = "ClusterManage.removeCluster";
        req["nodeMac"] = nodeId;
        ValkyrieAdapter::getIntance().sendPayload("METHOD", req);
    }
}

void ClusterManagePage::refreshTable() {
    QVariantMap req;
    req["method"] = "ClusterManage.getAllCluster";
    ValkyrieAdapter::getIntance().sendPayload("METHOD", req);
}
