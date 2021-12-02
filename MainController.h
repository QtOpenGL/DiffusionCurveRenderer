#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>

class CentralWidget;
class OpenGLWidget;
class ModeWidget;
class CurveWidget;
class ControlPointWidget;
class ZoomWidget;

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = nullptr);

    CentralWidget *centralWidget() const;

private:
    CentralWidget *mCentralWidget;
    OpenGLWidget *mOpenGLWidget;
    ModeWidget *mModeWidget;
    CurveWidget *mCurveWidget;
    ControlPointWidget *mControlPointWidget;
    ZoomWidget *mZoomWidget;
};

#endif // MAINCONTROLLER_H
