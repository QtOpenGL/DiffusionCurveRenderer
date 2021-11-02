#include "MainWindow.h"
#include "Widgets/CentralWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mCentralWidget = new CentralWidget;
    setCentralWidget(mCentralWidget);
}

MainWindow::~MainWindow()
{
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{

    mCentralWidget->gLWidget()->onKeyPressed(event->key());

}
