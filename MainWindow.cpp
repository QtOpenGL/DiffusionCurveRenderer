#include "MainWindow.h"
#include "Widgets/CentralWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{}

MainWindow::~MainWindow() {}

void MainWindow::keyPressEvent(QKeyEvent *event) {}

void MainWindow::setCentralWidget(CentralWidget *newCentralWidget) { mCentralWidget = newCentralWidget; }

void MainWindow::initialize() { QMainWindow::setCentralWidget(mCentralWidget); }
