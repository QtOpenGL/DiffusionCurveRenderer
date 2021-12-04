#include "Window.h"
#include "Widgets/CentralWidget.h"

Window::Window(QMainWindow *parent)
    : QMainWindow(parent)
{}

Window::~Window() {}

void Window::keyPressEvent(QKeyEvent *event) {}

void Window::setCentralWidget(CentralWidget *newCentralWidget)
{
    mCentralWidget = newCentralWidget;
}

void Window::initialize()
{
    QMainWindow::setCentralWidget(mCentralWidget);
}
