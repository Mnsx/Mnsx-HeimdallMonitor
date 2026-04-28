/** * @file DefectDistribution.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/24
 * @description
 */
//
// Created by Mnsx on 2026/4/24.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DefectDistribution.h" resolved

#include "DefectDistribution.h"
#include "ui_DefectDistribution.h"

#include "../../utils/ChartsToPdf.h"
#include <QPieModelMapper>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QVPieModelMapper>
#include <QToolTip>
#include <QCursor>
#include <QtGui/QPainter>
#include <QtGui/QPdfWriter>
#include <QtCharts>

#include "../../../../core/adapter/ValkyrieAdapter.h"

DefectDistribution::DefectDistribution(QWidget *parent) : QWidget(parent), ui(new Ui::DefectDistribution) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("background-color: transparent;");
    initChart();
}

DefectDistribution::~DefectDistribution() {
    delete ui;
}

void DefectDistribution::initChart() {
    QStandardItemModel* dataModel = new QStandardItemModel(4, 2, this);
    dataModel->setHorizontalHeaderLabels({"缺陷类型", "发生频次"});

    dataModel->setData(dataModel->index(0, 0), "斑点污块");
    dataModel->setData(dataModel->index(0, 1), count1_);
    dataModel->setData(dataModel->index(1, 0), "边缘断裂");
    dataModel->setData(dataModel->index(1, 1), count2_);
    dataModel->setData(dataModel->index(2, 0), "线条划痕");
    dataModel->setData(dataModel->index(2, 1), count3_);
    dataModel->setData(dataModel->index(3, 0), "尺寸超差");
    dataModel->setData(dataModel->index(3, 1), count4_);

    QPieSeries* series = new QPieSeries();
    series->setHoleSize(0.35);
    series->setPieSize(0.8);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("缺陷类型分布监测");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->setBackgroundVisible(false);
    chart->setPlotAreaBackgroundVisible(false);
    chart->setBackgroundBrush(Qt::transparent);
    chart->setPlotAreaBackgroundBrush(Qt::transparent);
    chart->setBackgroundRoundness(0);

    QFont titleFont("Consolas", 14, QFont::Bold);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(QColor("#E0E0E0")));

    QFont legendFont("Consolas", 10, QFont::Bold);
    chart->legend()->setFont(legendFont);
    chart->legend()->setLabelColor(QColor("#AAAAAA"));
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setMarkerShape(QLegend::MarkerShapeRectangle);

    QVPieModelMapper* mapper = new QVPieModelMapper(this);
    mapper->setModel(dataModel);
    mapper->setSeries(series);
    mapper->setLabelsColumn(0);
    mapper->setValuesColumn(1);

    QList<QColor> colors = {
        QColor("#00FFCC"),
        QColor("#FF007F"),
        QColor("#007ACC"),
        QColor("#FFD700")
    };

    QList<QPieSlice*> slices = series->slices();
    for (int i = 0; i < slices.size() && i < colors.size(); ++i) {
        slices.at(i)->setBrush(colors.at(i));
        slices.at(i)->setPen(QPen(Qt::NoPen));
        slices.at(i)->setLabelFont(QFont("Consolas", 9));
        slices.at(i)->setLabelColor(QColor("#E0E0E0"));
    }

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet(
        "QToolTip { color: #00FFCC; background-color: #1A1A1A; border: 1px solid #555555; font-family: Consolas; font-size: 13px; padding: 4px; }"
        "background: transparent; border: none;"
    );

    connect(series, &QPieSeries::hovered, this, [chartView](QPieSlice *slice, bool state) {
        if (state) {
            slice->setExploded(true);
            slice->setExplodeDistanceFactor(0.08);

            QString text = QString("%1\n频次: %2").arg(slice->label()).arg(slice->value());
            QToolTip::showText(QCursor::pos(), text, chartView);
        } else {
            slice->setExploded(false);
            QToolTip::hideText();
        }
    });

   QLayout* currentLayout = this->layout();
    if (currentLayout == nullptr) {
        QVBoxLayout* newLayout = new QVBoxLayout(this);
        newLayout->setContentsMargins(0, 0, 0, 0);
        currentLayout = newLayout;
    }

    // 1. 创建导出按钮（延续赛博朋克深色战术风格）
    QPushButton* exportPdfBtn = new QPushButton("导出 PDF 报告", this);
    exportPdfBtn->setStyleSheet(
        "QPushButton {"
        "   color: #00FFCC; "
        "   background-color: #1A1A1A; "
        "   border: 1px solid #555555; "
        "   border-radius: 4px; "
        "   padding: 6px 12px; "
        "   font-family: Consolas; "
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #333333; border: 1px solid #00FFCC; }"
        "QPushButton:pressed { background-color: #000000; }"
    );

    // 2. 加入布局（安全二段式写法）
    currentLayout->addWidget(chartView);
    currentLayout->addWidget(exportPdfBtn);
    currentLayout->setAlignment(exportPdfBtn, Qt::AlignRight);

    // 3. 挂载 PDF 导出信号
    connect(exportPdfBtn, &QPushButton::clicked, this, [chartView]() {
        QString dirPath = QDir::currentPath() + "/data_report";
        QDir dir(dirPath);

        // 防护：动态生成目录
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        // 独立命名为 defect_distribution.pdf 避免冲突
        QString filePath = dirPath + "/defect_distribution.pdf";

        // 调用统一渲染总线
        exportChartToPdf(chartView, filePath);
    });

    this->defectModel_ = dataModel;

    // 保持你原有的数据更新信号连接
    connect(&ValkyrieAdapter::getIntance(), &ValkyrieAdapter::typeCountAdd, this, &DefectDistribution::addCount);
}

void DefectDistribution::addCount(int count_id) {
    if (count_id == 1) {
        count1_++;
        defectModel_->setData(defectModel_->index(0, 1), count1_);
    } else if (count_id == 2) {
        count2_++;
        defectModel_->setData(defectModel_->index(1, 1), count2_);
    } else if (count_id == 3) {
        count3_++;
        defectModel_->setData(defectModel_->index(2, 1), count3_);
    } else if (count_id == 4) {
        count4_++;
        defectModel_->setData(defectModel_->index(3, 1), count4_);
    }
}