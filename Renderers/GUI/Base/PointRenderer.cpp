#include "PointRenderer.h"
#include <QtMath>

PointRenderer::PointRenderer()
    : mShader(nullptr)
    , mTicks(nullptr)
{}

PointRenderer::~PointRenderer()
{
    if (mShader)
        delete mShader;

    if (mTicks)
        delete mTicks;

    mShader = nullptr;
    mTicks = nullptr;
}

bool PointRenderer::initialize()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Point/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Point/GeometryShader.geom")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Point/FragmentShader.frag") || !mShader->link() || !mShader->bind()) {
        qCritical() << this << mShader->log();
        return false;
    }

    // Uniforms
    mProjectionMatrixLocation = mShader->uniformLocation("projectionMatrix");
    mPointLocation = mShader->uniformLocation("point");
    mInnerColorLocation = mShader->uniformLocation("innerColor");
    mOuterColorLocation = mShader->uniformLocation("outerColor");
    mInnerRadiusLocation = mShader->uniformLocation("innerRadius");
    mOuterRadiusLocation = mShader->uniformLocation("outerRadius");
    mTicksDeltaLocation = mShader->uniformLocation("ticksDelta");

    //Bind locations
    mShader->bindAttributeLocation("vs_Tick", 0);

    // Ticks
    mTicks = new Ticks(0, 2 * static_cast<float>(M_PI), 100);
    mTicks->create();

    return true;
}

void PointRenderer::render(const RenderParameters &params)
{
    mShader->bind();

    mShader->setUniformValue(mProjectionMatrixLocation, mProjectionMatrix);
    mShader->setUniformValue(mPointLocation, params.point);
    mShader->setUniformValue(mInnerRadiusLocation, params.innerRadius);
    mShader->setUniformValue(mOuterRadiusLocation, params.outerRadius);
    mShader->setUniformValue(mInnerColorLocation, params.innerColor);
    mShader->setUniformValue(mOuterColorLocation, params.outerColor);
    mShader->setUniformValue(mTicksDeltaLocation, mTicks->ticksDelta());

    mTicks->bind();
    glDrawArrays(GL_POINTS, 0, mTicks->size());
    mTicks->release();

    mShader->release();
}

void PointRenderer::setProjectionMatrix(const QMatrix4x4 &newMatrix)
{
    mProjectionMatrix = newMatrix;
}
