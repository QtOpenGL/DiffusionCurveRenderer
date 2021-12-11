#include "Controller.h"
#include "Window.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QScreen>
#include <QStyleFactory>
#include <QSurfaceFormat>

#include <Widgets/TestWidget.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QFile file("Resources/StyleSheet.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
    } else {
        qDebug() << "Could not find style sheet. Using the default one.";
    }

    int id = QFontDatabase::addApplicationFont("Resources/Fonts/Segoe/Segoe UI.ttf");
    if (id == -1) {
        QFont font = qApp->font();
        font.setPixelSize(12);
        font.setBold(false);
        qApp->setFont(font);
        qDebug() << "Font cannot be loaded. Using deafult font:" << qApp->font();
    } else {
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont font(family);
        font.setPixelSize(13);
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
    window->show();

    //    TestWidget *widget = new TestWidget;
    //    widget->show();

    return app.exec();
}
