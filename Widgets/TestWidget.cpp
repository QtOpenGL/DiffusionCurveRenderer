#include "TestWidget.h"

TestWidget::TestWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , mRendererManager(nullptr)
{}

void TestWidget::initializeGL()
{
    initializeOpenGLFunctions();

    mProjectionParameters = new OpenGLWidget::ProjectionParameters;
    mCurveContainer = new CurveContainer;

    mRendererManager = new RendererManager;
    mRendererManager->setProjectionParameters(mProjectionParameters);
    mRendererManager->setCurveContainer(mCurveContainer);
    mRendererManager->init();

    // Initialize projection parameters
    mProjectionParameters->left = 0;
    mProjectionParameters->right = width();
    mProjectionParameters->bottom = height(); // Flip y-axis
    mProjectionParameters->top = 0;

    Bezier *bezier = new Bezier;
    bezier->addControlPoint(new ControlPoint(-1024, 0)); // First
    bezier->addControlPoint(new ControlPoint(1024, 0));  // Last
    bezier->addControlPoint(new ControlPoint(-512, 1024));
    bezier->addControlPoint(new ControlPoint(512, -1024));
    bezier->setThickness(16);

    bezier->addColorPoint(Curve::Left, new ColorPoint(0, QVector4D(1, 0, 0, 1)));
    bezier->addColorPoint(Curve::Left, new ColorPoint(0.5, QVector4D(0, 0, 0, 1)));
    bezier->addColorPoint(Curve::Left, new ColorPoint(1, QVector4D(0, 0, 1, 1)));

    bezier->addColorPoint(Curve::Right, new ColorPoint(0, QVector4D(0, 1, 0, 1)));
    bezier->addColorPoint(Curve::Right, new ColorPoint(0.25, QVector4D(1, 1, 1, 1)));
    bezier->addColorPoint(Curve::Right, new ColorPoint(0.5, QVector4D(1, 1, 0, 1)));
    bezier->addColorPoint(Curve::Right, new ColorPoint(0.75, QVector4D(0, 0, 0, 1)));
    bezier->addColorPoint(Curve::Right, new ColorPoint(1, QVector4D(0, 1, 1, 1)));

    mCurveContainer->addCurve(bezier);
}

void TestWidget::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float pixelRatio = QPaintDevice::devicePixelRatioF();

    mProjectionParameters->canvasWidth = width() * pixelRatio;
    mProjectionParameters->canvasHeight = height() * pixelRatio; // Flip y-axis
    mRendererManager->diffuse();
}

QSize TestWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize TestWidget::sizeHint() const
{
    return QSize(1024, 768);
}
