/** 
 * @file MainWindow.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 * @description 
 */
#ifndef MNSX_HEIMDALLMONITOR_MAINWINDOW_H
#define MNSX_HEIMDALLMONITOR_MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    void setupUi();

    QListWidget* navList_;
    QStackedWidget* stackedWidget_;

    Ui::MainWindow *ui;
};


#endif //MNSX_HEIMDALLMONITOR_MAINWINDOW_H