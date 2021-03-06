#include "Controller.h"
#include "Window.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QScreen>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QUrl>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile file(":/Resources/Style/Style.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
    } else {
        qDebug() << "Could not find style sheet. Using the default one.";
    }

    int id = QFontDatabase::addApplicationFont(":/Resources/Fonts/Ubuntu/Ubuntu-Regular.ttf");
    if (id == -1) {
        QFont font = qApp->font();
        font.setPixelSize(12);
        font.setBold(false);
        qApp->setFont(font);
        qDebug() << "Font cannot be loaded. Using deafult font:" << qApp->font();
    } else {
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont font(family);
        font.setPixelSize(12);
        qApp->setFont(font);
        qDebug() << "Font is loaded. Using" << qApp->font();
    }

    QSurfaceFormat format;
    format.setSamples(16);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    Controller *controller = new Controller;
    Window *window = new Window;
    window->setCentralWidget(controller->centralWidget());
    window->initialize();
    window->setWindowTitle("Diffusion Curve Renderer");
    window->showMaximized();

    return app.exec();
}
