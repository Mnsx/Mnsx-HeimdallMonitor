/** 
 * @file ChartsToPdf.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/28
 * @description 
 */
#ifndef HEIMDALLMONITOR_CHARTSTOPDF_H
#define HEIMDALLMONITOR_CHARTSTOPDF_H
#include <QChartView>
#include <QtGui/QPainter>
#include <QtGui/QPdfWriter>
#include <QtCharts>

static void exportChartToPdf(QChartView *chartView, const QString &filePath) {
    if (!chartView || !chartView->chart() || !chartView->chart()->scene()) {
        return;
    }

    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize::A4);
    pdfWriter.setPageOrientation(QPageLayout::Landscape);
    pdfWriter.setResolution(300); // 保证高清矢量输出

    QPainter painter(&pdfWriter);
    painter.setRenderHint(QPainter::Antialiasing);

    // 获取 PDF 页面的绘制区域
    QRectF targetRect = pdfWriter.pageLayout().paintRectPixels(pdfWriter.resolution());

    // 【破案关键】：调用 Scene 的 render 方法，而不是 chart 的 render 方法！
    chartView->chart()->scene()->render(&painter, targetRect);

    painter.end();
}

#endif //HEIMDALLMONITOR_CHARTSTOPDF_H