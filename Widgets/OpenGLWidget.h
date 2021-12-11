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

class Controller;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setTransformer(Transformer *newTransformer);
    void setController(Controller *newController);

signals:
    void action(Action action, QVariant value = QVariant());
    void wheelMoved(QWheelEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);

public slots:
    void onSelectedControlPointChanged(const ControlPoint *selectedControlPoint);
    void onSelectedCurveChanged(const Curve *selectedCurve);
    void onModeChanged(Mode mode);
    void refresh();

protected:
    void initializeGL() override;
    void paintGL() override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Transformer *mTransformer;
    Controller *mController;

    const Curve *mSelectedCurve;
    const ControlPoint *mSelectedControlPoint;

    Mode mMode;
    bool mInit;
    bool mMousePressed;
    QPointF mMousePosition;
    QRectF mHandles[4];
};

#endif // OPENGLWIDGET_H
