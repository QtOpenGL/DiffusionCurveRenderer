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

#include "CurveWidget.h"
#include "ModeWidget.h"
#include <Renderers/Curve/BezierContourRenderer.h>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setCurveContainer(CurveContainer *newCurveContainer);

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
    bool highlightSelectedCurve();

    void zoom(float zoomRatio, QPoint mousePosition);
    void updateProjectionMatrix();
    void updateCursor();

    QVector2D mapFromGui(QPointF position);
    QPointF mapToGui(QVector2D position);
    QRectF mapToGui(const QRectF &rect);

private:
    BezierContourRenderer *mBezierContourRenderer;
    CurveContainer *mCurveContainer;

    bool mInitialized;

    ModeWidget::Mode mMode;

    bool mMousePressed;
    bool mMousePressedOnCurve;
    QPointF mMousePosition;

    float mZoomRatio;
    QRectF mCanvasRectangle;

    QRectF mHandles[4];
};

#endif // OPENGLWIDGET_H
