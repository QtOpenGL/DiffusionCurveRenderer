#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "CurveContainer.h"
#include "Transformer.h"
#include "Types.h"

#include <QObject>

#include <Curves/Curve.h>
#include <Renderers/RendererManager.h>
#include <Widgets/ColorPointWidget.h>
#include <Widgets/ModeWidget.h>
#include <Widgets/RenderSettingsWidget.h>
#include <QVariant>

class CentralWidget;
class OpenGLWidget;
class ModeWidget;
class CurveWidget;
class ControlPointWidget;
class ZoomWidget;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    CentralWidget *centralWidget() const;
    void initializeOpenGL();
    void render();

public slots:
    void onAction(Action action, QVariant value = QVariant());
    void onModeChanged(Mode mode);
    void onZoomRatioChanged(float newZoomRatio);

private slots:
    void onWheelMoved(QWheelEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);

    bool cursorInsideBoundingBox(QPointF position, QMarginsF margins = QMarginsF(-20, -20, 20, 20));
    void refresh();
    void zoom(float newZoomRatio, QVariant cursorPositionVariant = QVariant());

signals:
    void zoomRatioChanged(float newZoomRatio);

private:
    CentralWidget *mCentralWidget;
    OpenGLWidget *mOpenGLWidget;
    ModeWidget *mModeWidget;
    CurveWidget *mCurveWidget;
    ControlPointWidget *mControlPointWidget;
    ZoomWidget *mZoomWidget;
    ColorPointWidget *mColorPointWidget;
    RenderSettingsWidget *mRenderSettingsWidget;

    CurveContainer *mCurveContainer;
    Transformer *mTransformer;
    RendererManager *mRendererManager;

    ProjectionParameters *mProjectionParameters;

    Mode mMode;

    bool mMousePressed;
    bool mMousePressedOnCurve;
    QPointF mMousePosition;

    float mZoomStep;
};

#endif // CONTROLLER_H
