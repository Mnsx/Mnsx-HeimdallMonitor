#include "PartsPerMillion.h"
#include "ui_PartsPerMillion.h"

#include <QVBoxLayout>
#include "../../utils/ChartsToPdf.h"
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QVBarModelMapper>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QToolTip> // 🌟 引入提示框
#include <QCursor>  // 🌟 引入光标
#include <QtGui/QPainter>
#include <QtGui/QPdfWriter>
#include <QtCharts>

PartsPerMillion::PartsPerMillion(QWidget *parent) : QWidget(parent), ui(new Ui::PartsPerMillion) {
    ui->setupUi(this);

    // 🌟 继承透明底板战术，彻底抹杀系统默认的灰白底色
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("background-color: transparent;");

    initChart();
}

PartsPerMillion::~PartsPerMillion() {
    delete ui;
}

void PartsPerMillion::initChart() {
    QStandardItemModel* dataModel = new QStandardItemModel(3, 2, this);
    dataModel->setHorizontalHeaderLabels({"视觉节点", "PPM数值"});
    dataModel->setData(dataModel->index(0, 0), "Achilles-01");
    dataModel->setData(dataModel->index(0, 1), 125);
    dataModel->setData(dataModel->index(1, 0), "Olympus-A");
    dataModel->setData(dataModel->index(1, 1), 85);
    dataModel->setData(dataModel->index(2, 0), "Olympus-B");
    dataModel->setData(dataModel->index(2, 1), 210);

    QBarSeries* series = new QBarSeries();
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("各节点PPM数据比对");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->setBackgroundVisible(false);
    chart->setBackgroundRoundness(0);

    QFont titleFont("Consolas", 14, QFont::Bold);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(QColor("#E0E0E0")));
    chart->legend()->hide();

    QVBarModelMapper* mapper = new QVBarModelMapper(this);
    mapper->setModel(dataModel);
    mapper->setSeries(series);
    mapper->setFirstBarSetColumn(1);
    mapper->setLastBarSetColumn(1);

    if (!series->barSets().isEmpty()) {
        QBarSet* barSet = series->barSets().first();
        barSet->setBrush(QColor("#00FFCC"));
        barSet->setPen(QPen(Qt::NoPen));
    }

    QFont axisFont("Consolas", 10, QFont::Bold);
    QPen gridPen(QColor("#333333"), 1, Qt::DashLine);
    QPen axisLinePen(QColor("#555555"), 2);
    QColor labelColor("#AAAAAA");

    QStringList categories;
    for (int i = 0; i < dataModel->rowCount(); ++i) {
        categories << dataModel->index(i, 0).data().toString();
    }

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsFont(axisFont);
    axisX->setLabelsColor(labelColor);
    axisX->setGridLinePen(gridPen);
    axisX->setLinePen(axisLinePen);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("PPM");
    axisY->setLabelsFont(axisFont);
    axisY->setLabelsColor(labelColor);
    axisY->setTitleFont(axisFont);
    axisY->setTitleBrush(QBrush(labelColor));
    axisY->setGridLinePen(gridPen);
    axisY->setLinePen(axisLinePen);
    axisY->setRange(0, 300);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 🎨 统一暗黑涂装，合并原有背景透明样式
    chartView->setStyleSheet(
        "QToolTip { color: #00FFCC; background-color: #1A1A1A; border: 1px solid #555555; font-family: Consolas; font-size: 13px; padding: 4px; }"
        "background: transparent; border: none;"
    );

    // ==========================================
    // 🌟 新增战术：柱状体雷达悬停侦测
    // ==========================================
    connect(series, &QBarSeries::hovered, this, [dataModel, chartView](bool status, int index, QBarSet *barset) {
        if (status) {
            // 通过 index 回查 Model 里的第 0 列获取名字，通过 barset 获取具体数值
            QString nodeName = dataModel->index(index, 0).data().toString();
            double ppmValue = barset->at(index);

            QString text = QString("节点: %1\nPPM: %2").arg(nodeName).arg(ppmValue);
            QToolTip::showText(QCursor::pos(), text, chartView);
        } else {
            QToolTip::hideText();
        }
    });

    QLayout* currentLayout = this->layout();
    if (currentLayout == nullptr) {
        QVBoxLayout* newLayout = new QVBoxLayout(this);
        newLayout->setContentsMargins(0, 0, 0, 0);
        currentLayout = newLayout;
    }

    // 1. 创建导出按钮并复用深色科技感样式
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

    // 2. 将图表和按钮加入垂直布局（安全写法，避免 3 参数报错）
    currentLayout->addWidget(chartView);
    currentLayout->addWidget(exportPdfBtn);
    currentLayout->setAlignment(exportPdfBtn, Qt::AlignRight);

    // 3. 连接按钮信号到导出 PDF 逻辑
    connect(exportPdfBtn, &QPushButton::clicked, this, [chartView]() {
        QString dirPath = QDir::currentPath() + "/data_report";
        QDir dir(dirPath);

        // 关键防护：如果文件夹不存在，必须先创建
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        // 命名为 parts_per_million.pdf 防止覆盖之前的文件
        QString filePath = dirPath + "/parts_per_million.pdf";

        // 调用你在 ChartsToPdf.h 中封装好的公共函数
        exportChartToPdf(chartView, filePath);
    });

    this->ppmModel_ = dataModel;
}