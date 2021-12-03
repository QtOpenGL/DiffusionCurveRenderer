#include "OpenGLTestWidget.h"

#include <Curves/Bezier.h>

OpenGLTestWidget::OpenGLTestWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{
    {
        Bezier *bezier = new Bezier;
        bezier->addControlPoint(new ControlPoint(50, 250));  // First
        bezier->addControlPoint(new ControlPoint(950, 250)); // Last
        bezier->addControlPoint(new ControlPoint(500, 700));
        bezier->setThickness(180);

        bezier->addColorPoint(Curve::Left, new ColorPoint(0, QVector4D(1, 0, 0, 1)));
        bezier->addColorPoint(Curve::Left, new ColorPoint(0.5, QVector4D(0, 0, 0, 1)));
        bezier->addColorPoint(Curve::Left, new ColorPoint(1, QVector4D(0, 0, 1, 1)));

        bezier->addColorPoint(Curve::Right, new ColorPoint(0, QVector4D(0, 1, 0, 1)));
        bezier->addColorPoint(Curve::Right, new ColorPoint(0.25, QVector4D(1, 1, 1, 1)));
        bezier->addColorPoint(Curve::Right, new ColorPoint(0.5, QVector4D(1, 1, 0, 1)));
        bezier->addColorPoint(Curve::Right, new ColorPoint(0.75, QVector4D(0, 0, 0, 1)));
        bezier->addColorPoint(Curve::Right, new ColorPoint(1, QVector4D(0, 1, 1, 1)));

        mCurves << bezier;
    }
}

void OpenGLTestWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 0);
    glEnable(GL_MULTISAMPLE);

    mBezierDiffusionRenderer = new BezierDiffusionRenderer;
    mBezierDiffusionRenderer->initialize();
}

void OpenGLTestWidget::paintGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mBezierDiffusionRenderer->render(mCurves);
}

void OpenGLTestWidget::resizeGL(int width, int height)
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.ortho(0, width, height, 0, -1, 1);

    mBezierDiffusionRenderer->setProjectionMatrix(mProjectionMatrix);
}

QSize OpenGLTestWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize OpenGLTestWidget::sizeHint() const
{
    return QSize(1024, 768);
}
