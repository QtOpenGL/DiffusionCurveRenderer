#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include "ColorPointWidget.h"
#include "RenderSettingsWidget.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QWidget>

class OpenGLWidget;
class ModeWidget;
class CurveWidget;
class ControlPointWidget;
class ZoomWidget;

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr);

    void init();

    void setOpenGLWidget(OpenGLWidget *newOpenGLWidget);
    void setModeWidget(ModeWidget *newModeWidget);
    void setControlPointWidget(ControlPointWidget *newControlPointWidget);
    void setZoomWidget(ZoomWidget *newZoomWidget);
    void setCurveWidget(CurveWidget *newCurveWidget);
    void setColorPointWidget(ColorPointWidget *newColorPointWidget);
    void setRenderSettingsWidget(RenderSettingsWidget *newRenderSettingsWidget);

signals:
    emit void keyPressed(Qt::Key);
    emit void keyReleased(Qt::Key);

public slots:
    void onKeyPressed(Qt::Key);
    void onKeyReleased(Qt::Key);

private:
    QGroupBox *mOpenGLWidgetContainer;
    OpenGLWidget *mOpenGLWidget;
    ModeWidget *mModeWidget;
    CurveWidget *mCurveWidget;
    ControlPointWidget *mControlPointWidget;
    ZoomWidget *mZoomWidget;
    ColorPointWidget *mColorPointWidget;
    RenderSettingsWidget *mRenderSettingsWidget;
};

#endif // CENTRALWIDGET_H
