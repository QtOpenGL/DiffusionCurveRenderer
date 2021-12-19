#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "CurveContainer.h"
#include "Transformer.h"
#include "Types.h"

#include <QObject>
#include <QVariant>

#include <Curves/Curve.h>
#include <CustomVariant.h>

#include <Renderers/RendererManager.h>

#include <Widgets/CentralWidget.h>
#include <Widgets/ColorPointWidget.h>
#include <Widgets/ControlPointWidget.h>
#include <Widgets/CurveWidget.h>
#include <Widgets/ModeWidget.h>
#include <Widgets/RenderSettingsWidget.h>
#include <Widgets/ZoomWidget.h>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    CentralWidget *centralWidget() const;

public slots:
    void onAction(Action action, CustomVariant value = CustomVariant());
    void onModeChanged(Mode mode);
    void onZoomRatioChanged(float newZoomRatio);

private slots:
    void onWheelMoved(QWheelEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);

    bool cursorInsideBoundingBox(QPointF position, QMarginsF margins = QMarginsF(-20, -20, 20, 20));
    void zoom(float newZoomRatio, CustomVariant cursorPositionVariant = CustomVariant());

signals:
    void zoomRatioChanged(float newZoomRatio);
    void dirty(DirtType);

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

    Mode mModeBeforeMousePress;
    Mode mMode;

    bool mMouseLeftButtonPressed;
    bool mMouseMiddleButtonPressed;
    bool mMouseRightButtonPressed;
    bool mMousePressedOnCurve;
    QPointF mMousePosition;

    float mZoomStep;

    bool mKeyPressed;
    Qt::Key mPressedKey;
};

#endif // CONTROLLER_H
