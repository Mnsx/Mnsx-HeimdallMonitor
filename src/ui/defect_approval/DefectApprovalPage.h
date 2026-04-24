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

    void initDashboard();

    QFrame *createReportCard(QString title, QString value, QString color);

    QListWidget* thumbnailList_;
    QLabel* mainImageDisplay_;

    QString current_rects_str_;    // 暂存当前选中的缺陷坐标
    QString current_count_; // 当前展示图片的缺陷数量
    QString current_type_; // 当前展示图片的缺陷类型集合
    QPixmap current_raw_pixmap_;   // 暂存当前纯净的原图 (未缩放)
    QListWidgetItem* current_index_; // 当前被展示的图片索引
    bool current_image_flag_ = false; // 当前图片是否为缺陷图
};


#endif //MNSX_HEIMDALLMONITOR_DEFECTAPPROVALPAGE_H