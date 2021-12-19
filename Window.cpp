#include "Window.h"
#include "Widgets/CentralWidget.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <QLayout>

Window::Window(QMainWindow *parent)
    : QMainWindow(parent)
{
    QMenuBar *menuBar = new QMenuBar;
    QMenu *fileMenu = new QMenu("File");

    {
        QAction *act = new QAction("Load from XML");
        connect(act, &QAction::triggered, this, [=]() {
            QString filePath = QFileDialog::getOpenFileName(this, "Load from XML", "", "*.xml");
            qDebug() << filePath;

            emit action(Action::LoadFromXML, filePath);
        });

        fileMenu->addAction(act);
    }

    {
        QAction *act = new QAction("Load from JSON");
        fileMenu->addAction(act);
    }

    {
        QAction *act = new QAction("Save as JSON");
        fileMenu->addAction(act);
    }

    {
        QAction *act = new QAction("Save as PNG");
        connect(act, &QAction::triggered, this, [=]() {
            QString filePath = QFileDialog::getSaveFileName(this, "Save File", "", "*.png");
            qDebug() << filePath;

            emit action(Action::SaveAsPNG, filePath);
        });
        fileMenu->addAction(act);
    }

    menuBar->addMenu(fileMenu);
    layout()->setMenuBar(menuBar);
}

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
