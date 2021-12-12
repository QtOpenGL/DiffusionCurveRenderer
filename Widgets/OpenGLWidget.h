#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPen>
#include <QWheelEvent>

#include "Curves/Bezier.h"
#include "Transformer.h"
#include "Types.h"

class RendererManager;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setTransformer(Transformer *newTransformer);
    void setProjectionParameters(ProjectionParameters *newProjectionParameters);
    void setRendererManager(RendererManager *newRendererManager);
    float getWidth();
    float getHeight();

signals:
    void action(Action action, QVariant value = QVariant());
    void wheelMoved(QWheelEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);

public slots:
    void onSelectedControlPointChanged(const ControlPoint *selectedControlPoint);
    void onSelectedCurveChanged(const Curve *selectedCurve);
    void onSelectedColorPointChanged(const ColorPoint *selectedColorPoint);
    void onModeChanged(Mode mode);
    void onRenderModeChanged(RenderMode renderMode);
    void onDirty(DirtType);

private slots:
    void updatePainter();
    void updateCursor();

protected:
    void initializeGL() override;
    void paintGL() override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Transformer *mTransformer;
    RendererManager *mRendererManager;

    const Curve *mSelectedCurve;
    const ControlPoint *mSelectedControlPoint;
    const ColorPoint *mSelectedColorPoint;
    ProjectionParameters *mProjectionParameters;

    Mode mMode;
    RenderMode mRenderMode;
    bool mInit;
    bool mMouseRightButtonPressed;
    bool mMouseLeftButtonPressed;
    QPointF mMousePosition;
    QRectF mHandles[4];
    QPen mDashedPen;
    QPen mSolidPen;

    bool mUpdatePainter;
};

#endif // OPENGLWIDGET_H
