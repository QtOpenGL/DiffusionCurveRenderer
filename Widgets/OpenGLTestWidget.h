#ifndef OPENGLTESTWIDGET_H
#define OPENGLTESTWIDGET_H

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QWidget>

#include <Curves/Curve.h>

#include <Renderers/Diffusion/DiffusionRenderer.h>
#include <Renderers/Diffusion/ScreenRenderer.h>

#include <Renderers/GUI/ControlPointRenderer.h>

class OpenGLTestWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLTestWidget(QWidget *parent = nullptr);
    ~OpenGLTestWidget();

    void initializeGL() override;
    void paintGL() override;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
signals:

private:
    QVector<Curve *> mCurves;
    QMatrix4x4 mProjectionMatrix;
    DiffusionRenderer *mDiffusionRenderer;
    ScreenRenderer *mScreenRenderer;

    ControlPointRenderer *mControlPointRenderer;
};

#endif // OPENGLTESTWIDGET_H
