#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QWheelEvent>

#include <Renderers/Base/LineRenderer.h>
#include <Renderers/Contour/BezierContourRenderer.h>
#include <Renderers/GUI/BoundingBoxRenderer.h>
#include <Renderers/GUI/ControlPointRenderer.h>
#include <Widgets/ModeWidget.h>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    enum Action { RemoveCurve = 0, RemoveControlPoint = 1 };

signals:
    void dirty();
    void selectedCurveChanged(Curve *selectedCurve);
    void selectedControlPointChanged(ControlPoint *selectedControlPoint);
    void zoomRatioChanged(float zoomRatio);

public slots:
    void onDirty();
    void onSelectedControlPointChanged(ControlPoint *selectedControlPoint);
    void onModeChanged(ModeWidget::Mode mode);
    void onAction(OpenGLWidget::Action action);
    void onKeyPressed(int key);
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

    void deselectAllCurves();
    void setSelectedCurve(Curve *selectedCurve);
    void setSelectedControlPoint(ControlPoint *selectedControlPoint);
    Curve *selectCurve(QPointF mousePosition, float radius = 20.0f);
    void removeCurve(int index);
    void removeCurve(Curve *curve);

    ControlPoint *getClosestControlPoint(QPointF mousePosition);
    void zoom(float zoomRatio, QPointF mousePosition);
    void updateProjectionMatrix();
    void updateCursor();

    BezierContourRenderer *mBezierContourRenderer;
    ControlPointRenderer *mControlPointRenderer;
    BoundingBoxRenderer *mBoundingBoxRenderer;
    LineRenderer *mLineRenderer;

    QVector<Curve *> mCurves;

    Curve *mSelectedCurve;
    ControlPoint *mSelectedControlPoint;
    ModeWidget::Mode mMode;

    bool mMousePressed;
    bool mMousePressedOnCurve;
    QPointF mMousePosition;

    float mZoomRatio;
    QRectF mCanvasRectangle;

    inline QPointF g2c(QPointF position) { return mCanvasRectangle.topLeft() + mZoomRatio * position; }

    inline QPointF c2g(QPointF position) { return (position - mCanvasRectangle.topLeft()) / mZoomRatio; }

    bool mInitialized;
};

#endif // OPENGLWIDGET_H
