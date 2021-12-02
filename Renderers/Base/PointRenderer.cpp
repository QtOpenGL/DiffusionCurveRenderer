#include "PointRenderer.h"
#include <QtMath>

PointRenderer::PointRenderer()
    : mShader(nullptr)
    , mInitialized(false)
{}

PointRenderer::~PointRenderer()
{
    mTicksBuffer.destroy();
    mTicksVertexArray.destroy();

    if (mShader)
        delete mShader;

    mShader = nullptr;
}

bool PointRenderer::initialize()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Point/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Point/GeometryShader.geom")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Point/FragmentShader.frag")
        || !mShader->link() || !mShader->bind()) {
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
    mShader->bindAttributeLocation("vertex", 0);

    // Ticks
    mTicks = QVector<float>(100, 0);
    for (int i = 0; i < mTicks.size(); ++i)
        mTicks[i] = (i * 2 * M_PI) / static_cast<float>(mTicks.size());

    mTicksDelta = (2 * M_PI) / static_cast<float>(mTicks.size());

    // Vertex Array and Buffer

    mTicksVertexArray.create();
    mTicksVertexArray.bind();

    mTicksBuffer.create();
    mTicksBuffer.bind();
    mTicksBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    mTicksBuffer.allocate(mTicks.constData(), mTicks.size() * sizeof(GLfloat));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    mTicksBuffer.release();

    mTicksVertexArray.release();

    return mInitialized = true;
}

void PointRenderer::render(const RenderParameters &params)
{
    if (!mInitialized)
        return;

    mShader->bind();

    mShader->setUniformValue(mProjectionMatrixLocation, mProjectionMatrix);
    mShader->setUniformValue(mPointLocation, params.point);
    mShader->setUniformValue(mInnerRadiusLocation, params.innerRadius);
    mShader->setUniformValue(mOuterRadiusLocation, params.outerRadius);
    mShader->setUniformValue(mInnerColorLocation, params.innerColor);
    mShader->setUniformValue(mOuterColorLocation, params.outerColor);
    mShader->setUniformValue(mTicksDeltaLocation, mTicksDelta);

    mTicksVertexArray.bind();
    glDrawArrays(GL_POINTS, 0, mTicks.size());
    mTicksVertexArray.release();

    mShader->release();
}

void PointRenderer::setProjectionMatrix(const QMatrix4x4 &newMatrix) { mProjectionMatrix = newMatrix; }
