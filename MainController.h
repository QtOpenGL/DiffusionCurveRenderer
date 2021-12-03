#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "CurveContainer.h"

#include <QObject>

#include <Curves/Curve.h>
#include <Widgets/ModeWidget.h>

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

protected:
private:
    CentralWidget *mCentralWidget;
    OpenGLWidget *mOpenGLWidget;
    ModeWidget *mModeWidget;
    CurveWidget *mCurveWidget;
    ControlPointWidget *mControlPointWidget;
    ZoomWidget *mZoomWidget;
    CurveContainer *mCurveContainer;
};

#endif // MAINCONTROLLER_H
