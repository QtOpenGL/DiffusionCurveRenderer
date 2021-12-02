#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

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

    void initialize();

    void setOpenGLWidget(OpenGLWidget *newOpenGLWidget);
    void setModeWidget(ModeWidget *newModeWidget);
    void setControlPointWidget(ControlPointWidget *newControlPointWidget);
    void setZoomWidget(ZoomWidget *newZoomWidget);
    void setCurveWidget(CurveWidget *newCurveWidget);

private:
    QGroupBox *mOpenGLWidgetContainer;
    OpenGLWidget *mOpenGLWidget;
    ModeWidget *mModeWidget;
    CurveWidget *mCurveWidget;
    ControlPointWidget *mControlPointWidget;
    ZoomWidget *mZoomWidget;
    QCheckBox *mShowContoursCheckBox;

signals:
    void showContoursStateChanged(bool state);
};

#endif // CENTRALWIDGET_H
