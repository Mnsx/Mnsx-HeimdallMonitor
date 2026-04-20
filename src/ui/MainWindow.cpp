/** 
 * @file MainWindow.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 */
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QHBoxLayout>

#include "cluster_manage/ClusterManagePage.h"
#include "defect_approval/DefectApprovalPage.h"
#include "system_log/SystemLogPage.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    this->setWindowTitle("Heimdall Monitor - Valkyrie Control Center");
    this->resize(1280, 800);

    this->setStyleSheet("QMainWindow { background-color: #2D2D30; } "
                        "QListWidget { background-color: #1E1E1E; color: white; font-size: 14px; border: none; outline: none; } "
                        "QListWidget::item { padding: 15px; } "
                        "QListWidget::item:selected { background-color: #007ACC; } "
                        "QLabel, QGroupBox { color: white; }");

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

    navList_ = new QListWidget(this);
    navList_->setFixedWidth(220);
    navList_->addItem("集群管控");
    navList_->addItem("次品审批");
    navList_->addItem("数据报表");
    navList_->addItem("系统日志");

    stackedWidget_ = new QStackedWidget(this);

    QWidget* cluster_manage_page = new ClusterManagePage();
    QWidget* defect_approval_page = new DefectApprovalPage();
    QWidget* placeholder4 = new QWidget();
    QWidget* system_log_page = new SystemLogPage();

    stackedWidget_->addWidget(cluster_manage_page);
    stackedWidget_->addWidget(defect_approval_page);
    stackedWidget_->addWidget(placeholder4);
    stackedWidget_->addWidget(system_log_page);

    mainLayout->addWidget(navList_);
    mainLayout->addWidget(stackedWidget_);
    this->setCentralWidget(centralWidget);

    connect(navList_, &QListWidget::currentRowChanged, stackedWidget_, &QStackedWidget::setCurrentIndex);
    navList_->setCurrentRow(0);
}