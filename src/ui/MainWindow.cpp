/** 
 * @file MainWindow.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 */
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QHBoxLayout>

#include "../core/adapter/ValkyrieAdapter.h"
#include "cluster_manage/ClusterManagePage.h"
#include "data_report/DataReport.h"
#include "defect_approval/DefectApprovalPage.h"
#include "system_log/SystemLogPage.h"
#include "data_report/item/defect_distribution/DefectDistribution.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    this->setWindowTitle("Heimdall Monitor - Valkyrie Control Center");
    this->resize(1280, 800);

    this->setStyleSheet("QMainWindow { background-color: #2D2D30; } "
                        "QListWidget { background-color: #1E1E1E; color: white; font-size: 14px; border: none; outline: none; } "
                        "QListWidget::item { padding: 15px; } "
                        "QListWidget::item:selected { background-color: #007ACC; } "
                        "QLabel, QGroupBox { color: white; }");

    // 连接服务端
    ValkyrieAdapter::getIntance().connectToEngine("127.0.0.1", 8888);
    while (!ValkyrieAdapter::getIntance().getSocket()->isOpen()) {}

    setupUi();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    navTree_ = new QTreeWidget(this);
    navTree_->setFixedWidth(220);
    navTree_->setHeaderHidden(true);
    navTree_->setIndentation(20);
    navTree_->setStyleSheet(
        "QTreeWidget { background-color: #1E1E1E; color: white; font-size: 14px; border: none; outline: none; } "
        "QTreeWidget::item { height: 45px; } "
        "QTreeWidget::item:selected { background-color: #007ACC; } "
        "QTreeWidget::branch:has-children:!has-siblings:closed, QTreeWidget::branch:closed:has-children:has-siblings { color: white; } "
        "QTreeWidget::branch:has-children { "
        "   border-image: none;"
        "   image: none;"
        "}"
    );
    QTreeWidgetItem* itemCluster = new QTreeWidgetItem(navTree_, QStringList() << "集群管控");
    itemCluster->setData(0, Qt::UserRole, 0); // 绑定 StackedWidget 的第 0 页

    QTreeWidgetItem* itemApproval = new QTreeWidgetItem(navTree_, QStringList() << "次品审批");
    itemApproval->setData(0, Qt::UserRole, 1); // 绑定第 1 页

    QTreeWidgetItem* itemReport = new QTreeWidgetItem(navTree_, QStringList() << "数据报表");
    itemReport->setData(0, Qt::UserRole, 2);   // 绑定第 2 页 (可以是你的四格 Dashboard)

    // --- 构建数据报表的二级菜单项 ---
    QTreeWidgetItem* subYield = new QTreeWidgetItem(itemReport, QStringList() << "实时良率");
    subYield->setData(0, Qt::UserRole, 3); // 绑定第 3 页

    QTreeWidgetItem* subDist = new QTreeWidgetItem(itemReport, QStringList() << "缺陷分布");
    subDist->setData(0, Qt::UserRole, 4); // 绑定第 4 页

    QTreeWidgetItem* subPpm = new QTreeWidgetItem(itemReport, QStringList() << "PPM 指标");
    subPpm->setData(0, Qt::UserRole, 5); // 绑定第 5 页

    QTreeWidgetItem* subFalse = new QTreeWidgetItem(itemReport, QStringList() << "误报率");
    subFalse->setData(0, Qt::UserRole, 6); // 绑定第 6 页

    QTreeWidgetItem* itemLog = new QTreeWidgetItem(navTree_, QStringList() << "系统日志");
    itemLog->setData(0, Qt::UserRole, 7); // 绑定第 7 页

    itemReport->setExpanded(true);

    stackedWidget_ = new QStackedWidget(this);

    QWidget* cluster_manage_page = new ClusterManagePage();
    QWidget* defect_approval_page = new DefectApprovalPage();
    QWidget* data_report = new DataReport();
    QWidget* placeholder_yield = new QWidget();       // 第 3 页: 良率详情
    QWidget* defectDistribution = new DefectDistribution();        // 第 4 页: 分布详情
    QWidget* partsPerMillion = new PartsPerMillion();         // 第 5 页: PPM 详情
    QWidget* placeholder_false = new QWidget();       // 第 6 页: 误报详情
    QWidget* system_log_page = new SystemLogPage();

    stackedWidget_->addWidget(cluster_manage_page);
    stackedWidget_->addWidget(defect_approval_page);
    stackedWidget_->addWidget(data_report);
    stackedWidget_->addWidget(placeholder_yield);     // 3
    stackedWidget_->addWidget(defectDistribution);      // 4
    stackedWidget_->addWidget(partsPerMillion);       // 5
    stackedWidget_->addWidget(placeholder_false);     // 6
    stackedWidget_->addWidget(system_log_page);

    mainLayout->addWidget(navTree_);
    mainLayout->addWidget(stackedWidget_);
    this->setCentralWidget(centralWidget);

    connect(navTree_, &QTreeWidget::itemClicked, this, [this, cluster_manage_page](QTreeWidgetItem *item, int column) {
        // 1. 获取这个节点绑定的页面 Index
        int targetPageIndex = item->data(0, Qt::UserRole).toInt();

        // 2. 切换主视图
        stackedWidget_->setCurrentIndex(targetPageIndex);

        // 3. 业务联动：如果是集群管控，刷新表格
        if (targetPageIndex == 0) {
            static_cast<ClusterManagePage*>(cluster_manage_page)->refreshTable();
        }

        // 4. 交互优化：如果点击的是父节点(比如数据报表)，自动折叠/展开子菜单
        if (item->childCount() > 0) {
            item->setExpanded(!item->isExpanded());
        }
    });
    navTree_->setCurrentItem(itemCluster);
    stackedWidget_->setCurrentIndex(0);
}