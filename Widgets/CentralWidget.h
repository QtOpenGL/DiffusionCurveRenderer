#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>

#include "LeftWidget.h"
#include "OpenGLWidget.h"

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr);

    OpenGLWidget *gLWidget() const;
    void setGLWidget(OpenGLWidget *newGLWidget);

private:
    QGroupBox *mGLWidgetContainer;
    OpenGLWidget *mGLWidget;
    LeftWidget *mLeftWidget;
};

#endif // CENTRALWIDGET_H
