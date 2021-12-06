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

bool RectangleRenderer::init()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Rectangle/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Rectangle/GeometryShader.geom")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Rectangle/FragmentShader.frag") || !mShader->link()
        || !mShader->bind()) {
        qCritical() << mShader->log();
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

void RectangleRenderer::render(const Parameters &parameters, const QMatrix4x4 &projectionMatrix)
{
    QVector2D topLeft = QVector2D(parameters.rectangle.topLeft());

    float width = static_cast<float>(parameters.rectangle.width());
    float height = static_cast<float>(parameters.rectangle.height());

    mShader->bind();

    mShader->setUniformValue(mProjectionMatrixLocation, projectionMatrix);
    mShader->setUniformValue(mFillColorLocation, parameters.fillColor);
    mShader->setUniformValue(mBorderEnabledLocation, parameters.borderEnabled);
    mShader->setUniformValue(mBorderWidthLocation, parameters.borderWidth);
    mShader->setUniformValue(mBorderColorLocation, parameters.borderColor);
    mShader->setUniformValue(mWidthLocation, width);
    mShader->setUniformValue(mHeightLocation, height);
    mShader->setUniformValue(mRectangleTopLeftLocation, topLeft);

    mTicks->bind();
    glDrawArrays(GL_POINTS, 0, mTicks->size());
    mTicks->release();

    mShader->release();
}
