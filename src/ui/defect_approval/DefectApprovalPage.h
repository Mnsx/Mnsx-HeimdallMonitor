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

    QString current_rects_str_;    // 暂存当前选中的缺陷坐标
    QPixmap current_raw_pixmap_;   // 暂存当前纯净的原图 (未缩放)
};


#endif //MNSX_HEIMDALLMONITOR_DEFECTAPPROVALPAGE_H