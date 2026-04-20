/** 
 * @file SystemLogPage.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 * @description 
 */
#ifndef MNSX_HEIMDALLMONITOR_SYSTEMLOGPAGE_H
#define MNSX_HEIMDALLMONITOR_SYSTEMLOGPAGE_H

#include <QWidget>
#include <QTextBrowser>


QT_BEGIN_NAMESPACE

namespace Ui {
    class SystemLogPage;
}

QT_END_NAMESPACE

class SystemLogPage : public QWidget {
    Q_OBJECT

public:
    explicit SystemLogPage(QWidget *parent = nullptr);

    ~SystemLogPage() override;

    void appendLog(const QString& level, const QString& message);

private:
    Ui::SystemLogPage *ui;
    QTextBrowser* logBrowser_;
};


#endif //MNSX_HEIMDALLMONITOR_SYSTEMLOGPAGE_H