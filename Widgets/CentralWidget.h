#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>

#include "LeftWidget.h"
#include "GLWidget.h"

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr);

    GLWidget *gLWidget() const;
    void setGLWidget(GLWidget *newGLWidget);

private:
    QGroupBox* mGLWidgetContainer;
    GLWidget* mGLWidget;
    LeftWidget* mLeftWidget;
};

#endif // CENTRALWIDGET_H
