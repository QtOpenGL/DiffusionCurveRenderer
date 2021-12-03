#ifndef OPENGLTESTWIDGET_H
#define OPENGLTESTWIDGET_H

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QWidget>

#include <Curves/Curve.h>
#include <Renderers/Curve/BezierDiffusionRenderer.h>

class OpenGLTestWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLTestWidget(QWidget *parent = nullptr);

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
signals:

private:
    QVector<Curve *> mCurves;
    QMatrix4x4 mProjectionMatrix;
    BezierDiffusionRenderer *mBezierDiffusionRenderer;
};

#endif // OPENGLTESTWIDGET_H
