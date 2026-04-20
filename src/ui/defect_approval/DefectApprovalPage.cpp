/** 
 * @file DefectApprovalPage.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 */
#include "DefectApprovalPage.h"
#include "ui_DefectApprovalPage.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QScroller>


DefectApprovalPage::DefectApprovalPage(QWidget *parent) : QWidget(parent) {
    setupUi();
}

DefectApprovalPage::~DefectApprovalPage() {
    delete ui;
}

void DefectApprovalPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(15);

    QGroupBox* listGroup = new QGroupBox("待审批次品队列", this);
    QVBoxLayout* listLayout = new QVBoxLayout(listGroup);

    thumbnailList_ = new QListWidget(this);
    thumbnailList_->setViewMode(QListView::IconMode);
    thumbnailList_->setFlow(QListView::LeftToRight);
    thumbnailList_->setWrapping(false);
    thumbnailList_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    thumbnailList_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    thumbnailList_->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    thumbnailList_->setFixedHeight(180);
    thumbnailList_->setSpacing(15);

    QScroller::grabGesture(thumbnailList_->viewport(), QScroller::LeftMouseButtonGesture);

    thumbnailList_->setStyleSheet(
        "QListWidget { background-color: #1E1E1E; border: none; outline: none; padding: 10px; }"
        "QListWidget::item { background-color: #2D2D30; border: 1px solid #444; border-radius: 6px; color: #888; }"
        "QListWidget::item:selected { border: 2px solid #007ACC; background-color: #3f3f46; color: white; }" // 选中时的科技蓝高亮
        "QScrollBar:horizontal { background-color: #1E1E1E; height: 12px; }" // 定制滚动条
        "QScrollBar::handle:horizontal { background-color: #555; border-radius: 6px; min-width: 30px; }"
    );

    listLayout->addWidget(thumbnailList_);
    mainLayout->addWidget(listGroup);

    // 2. 下半部分：高清影像视图 (空壳占位)
    QGroupBox* displayGroup = new QGroupBox("次品影像分析视图", this);
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);

    mainImageDisplay_ = new QLabel("NO SIGNAL\n等待底层 OpenCV 图像流接入", this);
    mainImageDisplay_->setAlignment(Qt::AlignCenter);

    mainImageDisplay_->setStyleSheet(
        "QLabel { "
        "   background-color: #121212; "
        "   color: #555555; "
        "   font-size: 24px; "
        "   font-weight: bold; "
        "   font-family: Consolas; "
        "   border: 3px dashed #333333; "
        "   border-radius: 8px; "
        "}"
    );
    mainImageDisplay_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    displayLayout->addWidget(mainImageDisplay_);
    mainLayout->addWidget(displayGroup);

    listGroup->setFixedHeight(250);

    // 【测试假数据】：放入几个空的缩略图方块，让你能测试鼠标滚轮滑动
    for (int i = 1; i <= 15; ++i) {
        QListWidgetItem* item = new QListWidgetItem(QString("Defect_ID: %1\n(No Image)").arg(i));
        item->setSizeHint(QSize(140, 100)); // 强行撑开方块的大小
        item->setTextAlignment(Qt::AlignCenter);
        thumbnailList_->addItem(item);
    }
}