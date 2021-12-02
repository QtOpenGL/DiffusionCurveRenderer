#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Widgets/CentralWidget.h>
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event) override;

private:
    CentralWidget *mCentralWidget;
};
#endif // MAINWINDOW_H
