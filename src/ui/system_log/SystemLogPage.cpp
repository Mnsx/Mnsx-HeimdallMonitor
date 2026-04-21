/** 
 * @file SystemLogPage.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 */
#include "SystemLogPage.h"
#include "ui_SystemLogPage.h"

#include <QVBoxLayout>
#include <QDateTime>
#include <thread>
#include <QTimer>

SystemLogPage::SystemLogPage(QWidget *parent) : QWidget(parent),
    ui(new Ui::SystemLogPage) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    logBrowser_ = new QTextBrowser(this);
    logBrowser_->setStyleSheet("QTextBrowser { background-color: #1E1E1E; color: #00FF00; font-family: Consolas; font-size: 13px; }");

    layout->addWidget(logBrowser_);

    // 开启新线程从服务端获取日志
    pollTimer_ = new QTimer(this);
    connect(pollTimer_, &QTimer::timeout, this, &SystemLogPage::onPollTimerTimeout);
    connect(&ValkyrieAdapter::getIntance(), &ValkyrieAdapter::logReceived, this, [this](const QVector<QString> logs) {
        logBrowser_->clear();
        for (auto log : logs) {
            logBrowser_->append(log);
        }
        logBrowser_->moveCursor(QTextCursor::End);
    });
    pollTimer_->start(2000);

    // appendLog("INFO", "[删除] Heimdall Monitor System Initialized.");
    ui->setupUi(this);
}

SystemLogPage::~SystemLogPage() {
    delete ui;
}

void SystemLogPage::appendLog(const QString& level, const QString& message) {
    // TODO 使用Argus传送过来的日志信息
    QString timestamp = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss.zzz]");

    QString color = "#00FF00";
    if (level == "ERROR") color = "#FF3333";
    else if (level == "WARNING") color = "#FFFF00";

    QString htmlLog = QString("<span style='color:#888888;'>%1</span> "
                              "<span style='color:%2; font-weight:bold;'>[%3]</span> "
                              "<span style='color:#FFFFFF;'>%4</span>")
                              .arg(timestamp).arg(color).arg(level).arg(message);

    logBrowser_->append(htmlLog);
}

void SystemLogPage::onPollTimerTimeout() {
    QVariantMap req;
    req["method"] = "SystemLog.getSystemLog";

    ValkyrieAdapter::getIntance().sendPayload("METHOD", req);
}
