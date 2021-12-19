#include "Window.h"
#include "Widgets/CentralWidget.h"

#include <QKeyEvent>

Window::Window(QMainWindow *parent)
    : QMainWindow(parent)
{}

Window::~Window() {}

void Window::keyPressEvent(QKeyEvent *event)
{
    mCentralWidget->onKeyPressed((Qt::Key) event->key());
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    mCentralWidget->onKeyReleased((Qt::Key) event->key());
}

void Window::setCentralWidget(CentralWidget *newCentralWidget)
{
    mCentralWidget = newCentralWidget;
}

void Window::initialize()
{
    QMainWindow::setCentralWidget(mCentralWidget);
}
