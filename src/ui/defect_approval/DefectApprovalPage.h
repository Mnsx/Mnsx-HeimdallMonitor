/** 
 * @file DefectApprovalPage.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 * @description 
 */
#ifndef MNSX_HEIMDALLMONITOR_DEFECTAPPROVALPAGE_H
#define MNSX_HEIMDALLMONITOR_DEFECTAPPROVALPAGE_H

#include <QListWidget>
#include <QWidget>
#include <QLabel>

QT_BEGIN_NAMESPACE

namespace Ui {
    class DefectApprovalPage;
}

QT_END_NAMESPACE

class DefectApprovalPage : public QWidget {
    Q_OBJECT

public:
    explicit DefectApprovalPage(QWidget *parent = nullptr);

    ~DefectApprovalPage() override;

private:
    Ui::DefectApprovalPage *ui;

    void setupUi();

    QListWidget* thumbnailList_;
    QLabel* mainImageDisplay_;
};


#endif //MNSX_HEIMDALLMONITOR_DEFECTAPPROVALPAGE_H