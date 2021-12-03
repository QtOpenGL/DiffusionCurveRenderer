#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <CurveContainer.h>
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

    enum Action { RemoveCurve = 0, RemoveControlPoint = 1 };

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
    void onAction(OpenGLWidget::Action action);
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

    void zoom(float zoomRatio, QPoint mousePosition);
    void updateProjectionMatrix();
    void updateCursor();

    BezierContourRenderer *mBezierContourRenderer;
    ControlPointRenderer *mControlPointRenderer;
    BoundingBoxRenderer *mBoundingBoxRenderer;
    LineRenderer *mLineRenderer;

    CurveContainer *mCurveContainer;

    ModeWidget::Mode mMode;

    bool mMousePressed;
    bool mMousePressedOnCurve;
    QPoint mMousePosition;

    float mZoomRatio;
    QRect mCanvasRectangle;

    inline QVector2D mapFromGui(QPoint position)
    {
        QPoint result = (mCanvasRectangle.topLeft() + mZoomRatio * position);
        return QVector2D(result.x(), result.y());
    }

    inline QPoint mapToGui(QVector2D position) { return (position.toPoint() - mCanvasRectangle.topLeft()) / mZoomRatio; }

    bool mInitialized;
};

#endif // OPENGLWIDGET_H
