#include "FalsePositiveRate.h"
#include "ui_FalsePositiveRate.h"

#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QVXYModelMapper>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QToolTip> // 🌟 引入提示框
#include <QCursor>  // 🌟 引入光标获取位置

#include "../../../../core/adapter/ValkyrieAdapter.h"

FalsePositiveRate::FalsePositiveRate(QWidget *parent) : QWidget(parent), ui(new Ui::FalsePositiveRate) {
    ui->setupUi(this);

    // 🌟 继承透明底板战术，抹杀系统默认灰白底色
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("background-color: transparent;");

    initChart();
}

FalsePositiveRate::~FalsePositiveRate() {
    delete ui;
}

void FalsePositiveRate::initChart() {
    QStandardItemModel* dataModel = new QStandardItemModel(5, 3, this);
    dataModel->setHorizontalHeaderLabels({"物理X坐标", "误报率(%)", "日期标签"});

    QVariantMap req;
    req["method"] = "DataReport.falsePositiveRateWeekly";
    ValkyrieAdapter::getIntance().sendPayload("METHOD", req);

    connect(&ValkyrieAdapter::getIntance(), &ValkyrieAdapter::falseRateReceived, this, [this, dataModel](const QVector<QMap<QString, QVariant>> & maps) {
        int totalRows = 4 + maps.size();
        fprModel_->setRowCount(totalRows);
        QStringList dateCategories;

        fprModel_->setData(fprModel_->index(0, 0), 0);
        fprModel_->setData(fprModel_->index(0, 1), 2.5);
        fprModel_->setData(fprModel_->index(0, 2), "04-20");
        dateCategories << "04-20";

        fprModel_->setData(fprModel_->index(1, 0), 1);
        fprModel_->setData(fprModel_->index(1, 1), 2.1);
        fprModel_->setData(fprModel_->index(1, 2), "04-21");
        dateCategories << "04-21";

        fprModel_->setData(fprModel_->index(2, 0), 2);
        fprModel_->setData(fprModel_->index(2, 1), 3.8);
        fprModel_->setData(fprModel_->index(2, 2), "04-22");
        dateCategories << "04-22";

        fprModel_->setData(fprModel_->index(3, 0), 3);
        fprModel_->setData(fprModel_->index(3, 1), 1.5);
        fprModel_->setData(fprModel_->index(3, 2), "04-23");
        dateCategories << "04-23";

        int index = 4;
        for (auto map : maps) {
            fprModel_->setData(fprModel_->index(index, 0), index);
            fprModel_->setData(fprModel_->index(index, 1), map.value("value"));
            fprModel_->setData(fprModel_->index(index, 2), map.value("date"));
            dateCategories << map.value("date").toString();
            index++;
        }

        QChartView* chartView = this->findChild<QChartView*>();
        if (chartView) {
            QAbstractAxis* axis = chartView->chart()->axes(Qt::Horizontal).first();
            QBarCategoryAxis* axisX = qobject_cast<QBarCategoryAxis*>(axis);
            if (axisX) {
                axisX->clear();
                axisX->append(dateCategories);
            }
        }
    });

    QLineSeries* series = new QLineSeries();
    series->setName("系统误报率 (FPR)");

    QPen pen(QColor("#FF0033"));
    pen.setWidth(3);
    series->setPen(pen);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("一周内系统误报率趋势");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->setBackgroundVisible(false);
    chart->setBackgroundRoundness(0);

    QFont titleFont("Consolas", 14, QFont::Bold);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(QColor("#E0E0E0")));

    QFont legendFont("Consolas", 10, QFont::Bold);
    chart->legend()->setFont(legendFont);
    chart->legend()->setLabelColor(QColor("#AAAAAA"));
    chart->legend()->setAlignment(Qt::AlignTop);

    QVXYModelMapper* mapper = new QVXYModelMapper(this);
    mapper->setModel(dataModel);
    mapper->setSeries(series);
    mapper->setXColumn(0);
    mapper->setYColumn(1);

    QFont axisFont("Consolas", 10, QFont::Bold);
    QPen gridPen(QColor("#333333"), 1, Qt::DashLine);
    QPen axisLinePen(QColor("#555555"), 2);
    QColor labelColor("#AAAAAA");

    QStringList dateCategoriesInit;
    for (int i = 0; i < dataModel->rowCount(); ++i) {
        dateCategoriesInit << dataModel->index(i, 2).data().toString();
    }

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(dateCategoriesInit);
    axisX->setLabelsFont(axisFont);
    axisX->setLabelsColor(labelColor);
    axisX->setGridLinePen(gridPen);
    axisX->setLinePen(axisLinePen);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("误报率 (%)");
    axisY->setLabelsFont(axisFont);
    axisY->setLabelsColor(labelColor);
    axisY->setTitleFont(axisFont);
    axisY->setTitleBrush(QBrush(labelColor));
    axisY->setGridLinePen(gridPen);
    axisY->setLinePen(axisLinePen);
    axisY->setRange(0.0, 5.0);
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 🎨 统装暗黑悬浮窗样式
    chartView->setStyleSheet(
        "QToolTip { color: #00FFCC; background-color: #1A1A1A; border: 1px solid #555555; font-family: Consolas; font-size: 13px; padding: 4px; }"
        "background: transparent; border: none;"
    );

    // ==========================================
    // 🌟 新增战术：折线图节点雷达悬停侦测
    // ==========================================
    connect(series, &QLineSeries::hovered, this, [dataModel, chartView](const QPointF &point, bool state) {
        if (state) {
            // point.x() 返回的是底层的物理索引 (如 0, 1, 2...)
            // 转化为整型后，去 Model 的第 2 列取出日期字符串
            int rowIndex = qRound(point.x());
            QString dateLabel = "未知";
            if (rowIndex >= 0 && rowIndex < dataModel->rowCount()) {
                dateLabel = dataModel->index(rowIndex, 2).data().toString();
            }

            // 拼接提示文本，Y轴保留2位小数
            QString text = QString("日期: %1\n误报率: %2%").arg(dateLabel).arg(point.y(), 0, 'f', 2);
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
    currentLayout->addWidget(chartView);

    this->fprModel_ = dataModel;
}