#include "RectangleRenderer.h"

RectangleRenderer::RectangleRenderer()
    : mShader(nullptr)
{}

RectangleRenderer::~RectangleRenderer()
{
    mBuffer.destroy();
    mVertexArray.destroy();

    if (mShader)
        delete mShader;

    mShader = nullptr;
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

    //Bind locations
    mShader->bindAttributeLocation("topLeft", 0);

    // Vertex Array and Buffer
    mVertexArray.create();
    mVertexArray.bind();

    mBuffer.create();
    mBuffer.bind();
    mBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    mBuffer.allocate(sizeof(QVector2D));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    mBuffer.release();

    mVertexArray.release();

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

    mBuffer.bind();
    mBuffer.write(0, &topLeft, sizeof(QVector2D));
    mBuffer.release();

    mVertexArray.bind();
    glDrawArrays(GL_POINTS, 0, 1);
    mVertexArray.release();
}

void RectangleRenderer::setProjectionMatrix(const QMatrix4x4 &newMatrix)
{
    mProjectionMatrix = newMatrix;
}
