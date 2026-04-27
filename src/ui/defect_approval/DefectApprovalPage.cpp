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
#include <QImageReader>
#include <QDebug>
#include <QPushButton>
#include <QPainter>

#include "../../core/adapter/ValkyrieAdapter.h"


DefectApprovalPage::DefectApprovalPage(QWidget *parent) : QWidget(parent) {
    setupUi();
}

DefectApprovalPage::~DefectApprovalPage() {
    delete ui;
}

void DefectApprovalPage::setupUi() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(15);

    QGroupBox *listGroup = new QGroupBox("待审批次品队列", this);
    QVBoxLayout *listLayout = new QVBoxLayout(listGroup);

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
        "QListWidget::item:selected { border: 2px solid #007ACC; background-color: #3f3f46; color: white; }"
        // 选中时的科技蓝高亮
        "QScrollBar:horizontal { background-color: #1E1E1E; height: 12px; }" // 定制滚动条
        "QScrollBar::handle:horizontal { background-color: #555; border-radius: 6px; min-width: 30px; }"
    );

    listLayout->addWidget(thumbnailList_);
    mainLayout->addWidget(listGroup);

    // 2. 下半部分：高清影像视图 (空壳占位)
    QGroupBox *displayGroup = new QGroupBox("次品影像分析视图", this);
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);

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

    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addStretch();
    QPushButton *togglePassBtn_ = new QPushButton("误报放行", this);
    QPushButton *toggleStopBtn_ = new QPushButton("确认次品", this);
    QPushButton *toggleDefectBtn_ = new QPushButton("标记缺陷", this);
    togglePassBtn_->setCursor(Qt::PointingHandCursor);
    togglePassBtn_->setStyleSheet(
        "QPushButton {"
        "   background-color: #2D2D2D;"
        "   color: #00FFCC;" // 亮青色字体，极具科技感
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 5px 15px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #3D3D3D; border-color: #00FFCC; }"
        "QPushButton:pressed { background-color: #1E1E1E; color: #00AA88; }"
    );
    toggleStopBtn_->setCursor(Qt::PointingHandCursor);
    toggleStopBtn_->setStyleSheet(
        "QPushButton {"
        "   background-color: #2D2D2D;"
        "   color: #00FFCC;" // 亮青色字体，极具科技感
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 5px 15px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #3D3D3D; border-color: #00FFCC; }"
        "QPushButton:pressed { background-color: #1E1E1E; color: #00AA88; }"
    );
    toggleDefectBtn_->setCursor(Qt::PointingHandCursor);
    toggleDefectBtn_->setStyleSheet(
        "QPushButton {"
        "   background-color: #2D2D2D;"
        "   color: #00FFCC;" // 亮青色字体，极具科技感
        "   border: 1px solid #444444;"
        "   border-radius: 4px;"
        "   padding: 5px 15px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #3D3D3D; border-color: #00FFCC; }"
        "QPushButton:pressed { background-color: #1E1E1E; color: #00AA88; }"
    );
    controlLayout->addWidget(toggleStopBtn_);
    controlLayout->addWidget(togglePassBtn_);
    controlLayout->addWidget(toggleDefectBtn_);
    displayLayout->addLayout(controlLayout);
    mainLayout->addWidget(displayGroup);
    listGroup->setFixedHeight(250);

    // 绑定通过检测
    connect(togglePassBtn_, &QPushButton::clicked, this, [this]() {
        // 次品标记
        QString original_path = current_index_->data(Qt::UserRole).toString();
        QVariantMap req;
        req["method"] = "DetectRecord.modifyRecordStatus";
        req["flag"] = "1";
        req["filePath"] = original_path;
        ValkyrieAdapter::getIntance().sendPayload("METHOD", req);

        QListWidgetItem *item = thumbnailList_->currentItem();
        if (item) {
            int row = thumbnailList_->row(item);
            QListWidgetItem *takenItem = thumbnailList_->takeItem(row);
            delete takenItem;

            mainImageDisplay_->setPixmap(QPixmap());
            mainImageDisplay_->setText("已审核通过\n等待下一个次品数据");
        }
    });
    // 绑定取消通过
    connect(toggleStopBtn_, &QPushButton::clicked, this, [this]() {
        // 误报标记
        QString original_path = current_index_->data(Qt::UserRole).toString();
        QVariantMap req;
        req["method"] = "DetectRecord.modifyRecordStatus";
        req["flag"] = "2";
        req["filePath"] = original_path;
        ValkyrieAdapter::getIntance().sendPayload("METHOD", req);

        QListWidgetItem *item = thumbnailList_->currentItem();
        if (item) {
            int row = thumbnailList_->row(item);
            QListWidgetItem *takenItem = thumbnailList_->takeItem(row);
            delete takenItem;

            mainImageDisplay_->setPixmap(QPixmap());
            mainImageDisplay_->setText("已审核通过\n等待下一个次品数据");
        }
    });
    // 绑定缺陷显示的按钮
    connect(toggleDefectBtn_, &QPushButton::clicked, this, [this]() {
        if (current_raw_pixmap_.isNull() || current_rects_str_.isEmpty()) {
            qDebug() << "⚠️ 没有可标记的图像或坐标数据！";
            return;
        }

        if (current_image_flag_ == false) {
            QPixmap canvas = current_raw_pixmap_;

            QPainter painter(&canvas);

            painter.setPen(QPen(Qt::red, 3, Qt::SolidLine));

            // painter.setBrush(QColor(255, 0, 0, 40));

            // 先用;判定
            QStringList records = current_rects_str_.split(";");
            for (int i = 0; i < records.size(); i++) {
                QStringList coords = records[i].split(",");

                for (int i = 0; i <= coords.size() - 4; i += 4) {
                    int x = coords[i].toInt();
                    int y = coords[i + 1].toInt();
                    int w = coords[i + 2].toInt();
                    int h = coords[i + 3].toInt();
                    painter.drawRect(x, y, w, h);
                }
            }

            painter.end();

            QPixmap final_pixmap = canvas.scaled(
            mainImageDisplay_->contentsRect().size(),
             Qt::KeepAspectRatio,
             Qt::SmoothTransformation
            );

            mainImageDisplay_->setPixmap(final_pixmap);
            current_image_flag_ = true;
        } else {
            QPixmap clean_pixmap = current_raw_pixmap_.scaled(
            mainImageDisplay_->contentsRect().size(),
         Qt::KeepAspectRatio,
         Qt::SmoothTransformation
            );

            mainImageDisplay_->setPixmap(clean_pixmap);
            current_image_flag_ = false;
        }
    });

    connect(&ValkyrieAdapter::getIntance(), &ValkyrieAdapter::defectDataReceived, this, [this](const QVariantMap &map) {
        QByteArray image_bytes = QByteArray::fromBase64(map.value("img").toString().toUtf8());

        QPixmap pixmap;
        if (pixmap.loadFromData(image_bytes, "JPG")) {
            QListWidgetItem *item = new QListWidgetItem();
            item->setIcon(QIcon(pixmap));
            item->setTextAlignment(Qt::AlignCenter);
            QString tooltip_html = QString(
                        "<div style='white-space: pre-wrap;'>"
                        "<b>MAC地址：</b> %1<br>"
                        "<b>抓拍时间：</b> %2<br>"
                        "<b>缺陷类型：</b> %3<br>"
                        "<b>缺陷数量：</b> %4<br>"
                        "<b>存储路径：</b> %6"
                        "</div>"
                    ).arg(map.value("mac").toString())
                    .arg(map.value("time").toString())
                    .arg(map.value("rtype").toString())
                    .arg(map.value("count").toString())
                    .arg(map.value("path").toString());

            item->setToolTip(tooltip_html);
            // 存储重要数据
            item->setData(Qt::UserRole, map.value("path").toString());
            item->setData(Qt::UserRole + 1, map.value("mac").toString());
            item->setData(Qt::UserRole + 2, map.value("rects").toString());
            item->setData(Qt::UserRole + 3, map.value("count").toString());
            item->setData(Qt::UserRole + 4, map.value("rtype").toString());

            item->setText(QString("MAC: %1").arg(map.value("mac").toString()));
            thumbnailList_->setIconSize(QSize(160, 90));

            thumbnailList_->addItem(item);
        } else {
        }
    });
    // 绑定点击事件
    connect(thumbnailList_, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        QString original_path = item->data(Qt::UserRole).toString();
        QString pure_mac = item->data(Qt::UserRole + 1).toString();
        QString rects_str = item->data(Qt::UserRole + 2).toString();
        // 存储缺陷信息
        current_rects_str_ = item->data(Qt::UserRole + 2).toString();
        current_count_ = item->data(Qt::UserRole + 3).toString();
        current_type_ = item->data(Qt::UserRole + 4).toString();
        current_index_ = item;
        QVariantMap req;
        req["method"] = "DetectRecord.getRecordFromPath";
        req["path"] = original_path;
        ValkyrieAdapter::getIntance().sendPayload("METHOD", req);
    });
    // 绑定
    connect(&ValkyrieAdapter::getIntance(), &ValkyrieAdapter::defectImageReceived, this,
            [this](const QVariantMap &map) {
                QByteArray image_bytes = QByteArray::fromBase64(map.value("img").toString().toUtf8());

                QPixmap pixmap;
                if (pixmap.loadFromData(image_bytes)) {
                    current_raw_pixmap_ = pixmap;

                    QPixmap scaled_pixmap = pixmap.scaled(
                    mainImageDisplay_->contentsRect().size(),
                 Qt::KeepAspectRatio,
                 Qt::SmoothTransformation
                    );

                    mainImageDisplay_->setPixmap(scaled_pixmap);
                    mainImageDisplay_->setAlignment(Qt::AlignCenter);
                } else {
                }
            });
}
