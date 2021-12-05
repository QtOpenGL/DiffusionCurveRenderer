#include "RectangleRenderer.h"

RectangleRenderer::RectangleRenderer()
    : mShader(nullptr)
    , mTicks(nullptr)
{}

RectangleRenderer::~RectangleRenderer()
{
    if (mShader)
        delete mShader;

    if (mTicks)
        delete mTicks;

    mShader = nullptr;
    mTicks = nullptr;
}

bool RectangleRenderer::initialize()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Rectangle/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Rectangle/GeometryShader.geom")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Rectangle/FragmentShader.frag") || !mShader->link()
        || !mShader->bind()) {
        qCritical() << this << mShader->log();
        return false;
    }

    // Uniforms
    mProjectionMatrixLocation = mShader->uniformLocation("projectionMatrix");
    mFillColorLocation = mShader->uniformLocation("fillColor");
    mBorderEnabledLocation = mShader->uniformLocation("borderEnabled");
    mBorderWidthLocation = mShader->uniformLocation("borderWidth");
    mBorderColorLocation = mShader->uniformLocation("borderColor");
    mWidthLocation = mShader->uniformLocation("width");
    mHeightLocation = mShader->uniformLocation("height");
    mRectangleTopLeftLocation = mShader->uniformLocation("topLeft");

    //Bind locations
    mShader->bindAttributeLocation("vs_Tick", 0);

    mTicks = new Ticks(0, 1, 1);
    mTicks->create();

    return true;
}

void RectangleRenderer::render(const RenderParameters &params)
{
    QVector2D topLeft = QVector2D(params.rectangle.topLeft());

    float width = static_cast<float>(params.rectangle.width());
    float height = static_cast<float>(params.rectangle.height());

    mShader->bind();
    mShader->setUniformValue(mProjectionMatrixLocation, mProjectionMatrix);
    mShader->setUniformValue(mFillColorLocation, params.fillColor);
    mShader->setUniformValue(mBorderEnabledLocation, params.borderEnabled);
    mShader->setUniformValue(mBorderWidthLocation, params.borderWidth);
    mShader->setUniformValue(mBorderColorLocation, params.borderColor);
    mShader->setUniformValue(mWidthLocation, width);
    mShader->setUniformValue(mHeightLocation, height);
    mShader->setUniformValue(mRectangleTopLeftLocation, topLeft);

    mTicks->bind();
    glDrawArrays(GL_POINTS, 0, mTicks->size());
    mTicks->release();
}

void RectangleRenderer::setProjectionMatrix(const QMatrix4x4 &newMatrix)
{
    mProjectionMatrix = newMatrix;
}
