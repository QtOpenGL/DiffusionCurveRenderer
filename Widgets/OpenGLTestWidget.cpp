#include "OpenGLTestWidget.h"

#include <Curves/Bezier.h>

OpenGLTestWidget::OpenGLTestWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , mDiffusionRenderer(nullptr)
{
    Bezier *bezier = new Bezier;
    bezier->addControlPoint(new ControlPoint(0, 0));    // First
    bezier->addControlPoint(new ControlPoint(1024, 0)); // Last
    bezier->addControlPoint(new ControlPoint(512, 512));
    bezier->setThickness(60);

    bezier->addColorPoint(Curve::Left, new ColorPoint(0, QVector4D(1, 0, 0, 1)));
    bezier->addColorPoint(Curve::Left, new ColorPoint(0.5, QVector4D(0, 0, 0, 1)));
    bezier->addColorPoint(Curve::Left, new ColorPoint(1, QVector4D(0, 0, 1, 1)));

    bezier->addColorPoint(Curve::Right, new ColorPoint(0, QVector4D(0, 1, 0, 1)));
    bezier->addColorPoint(Curve::Right, new ColorPoint(0.25, QVector4D(1, 1, 1, 1)));
    bezier->addColorPoint(Curve::Right, new ColorPoint(0.5, QVector4D(1, 1, 0, 1)));
    bezier->addColorPoint(Curve::Right, new ColorPoint(0.75, QVector4D(0, 0, 0, 1)));
    bezier->addColorPoint(Curve::Right, new ColorPoint(1, QVector4D(0, 1, 1, 1)));

    bezier->translate(QVector2D(200, 200));

    mCurves << bezier;
}

OpenGLTestWidget::~OpenGLTestWidget()
{
    makeCurrent();

    if (mDiffusionRenderer)
        delete mDiffusionRenderer;

    mDiffusionRenderer = nullptr;

    doneCurrent();
}

void OpenGLTestWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);

    mDiffusionRenderer = new DiffusionRenderer(4096, 2048);
    mDiffusionRenderer->initialize();

    mScreenRenderer = new ScreenRenderer;
    mScreenRenderer->initialize();

    mControlPointRenderer = new ControlPointRenderer;
    mControlPointRenderer->initialize();
}

void OpenGLTestWidget::paintGL()
{
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float pixelRatio = QPaintDevice::devicePixelRatioF();
    float explicitWidth = width() * pixelRatio;
    float explicitHeight = height() * pixelRatio;

    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.ortho(0, explicitWidth, explicitHeight, 0, -1, 1);
    mDiffusionRenderer->setProjectionMatrix(mProjectionMatrix);

    mDiffusionRenderer->render(mCurves);

    mScreenRenderer->render(mDiffusionRenderer->getTexture(),
                            mDiffusionRenderer->bufferWidth() / explicitWidth,
                            mDiffusionRenderer->bufferHeight() / explicitHeight);

    mControlPointRenderer->render(mCurves[0]);
}

QSize OpenGLTestWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize OpenGLTestWidget::sizeHint() const
{
    return QSize(1024, 768);
}
