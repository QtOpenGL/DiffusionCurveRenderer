#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <CurveContainer.h>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPen>
#include <QWheelEvent>

#include <Renderers/Contour/ContourRenderer.h>

#include <Renderers/GUI/BoundingBoxRenderer.h>
#include <Renderers/GUI/ControlPointRenderer.h>

#include "CurveWidget.h"
#include "ModeWidget.h"

class RendererManager;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    struct ProjectionParameters
    {
        float left;
        float right;
        float bottom;
        float top;
        float canvasWidth;
        float canvasHeight;
    };

    OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setCurveContainer(CurveContainer *newCurveContainer);
    void setRendererManager(RendererManager *newRendererManager);

signals:
    void dirty();
    void zoomRatioChanged(float zoomRatio);

public slots:
    void onDirty();
    void onSelectedControlPointChanged(ControlPoint *selectedControlPoint);
    void onModeChanged(ModeWidget::Mode mode);
    void onAction(Action action);
    void onKeyPressed(Qt::Key key);
    void onZoomRatioChanged(float zoomRatio);
    void onShowContoursStateChanged(bool state);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void update();
    void updateCursor();
    void zoom(float zoomRatio, QPointF mousePosition);

    QVector2D mapFromGui(const QPointF &position) const;
    QPointF mapToGui(const QVector2D &position) const;
    QRectF mapToGui(const QRectF &rect) const;

private:
    RendererManager *mRendererManager;
    CurveContainer *mCurveContainer;
    bool mInit;

    ModeWidget::Mode mMode;

    bool mMousePressed;
    bool mMousePressedOnCurve;
    QPointF mMousePosition;

    float mZoomRatio;
    ProjectionParameters *mProjectionParameters;

    QRectF mHandles[4];

    const float mZoomStep;
};

#endif // OPENGLWIDGET_H
