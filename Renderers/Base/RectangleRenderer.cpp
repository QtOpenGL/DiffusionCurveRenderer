#include "RectangleRenderer.h"

RectangleRenderer::RectangleRenderer(QObject *parent):
    QObject(parent),
    mShader(nullptr)
{

}

RectangleRenderer::~RectangleRenderer()
{
    mBuffer.destroy();
    mVertexArray.destroy();

    if(mShader)
        delete mShader;

    mShader = nullptr;
}

bool RectangleRenderer::initialize()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if(!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Rectangle/VertexShader.vert") ||
            !mShader->addShaderFromSourceFile(QOpenGLShader::Geometry, "Shaders/Rectangle/GeometryShader.geom") ||
            !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Rectangle/FragmentShader.frag") ||
            !mShader->link() ||
            !mShader->bind())
    {
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
    {
        mVertexArray.create();
        QOpenGLVertexArrayObject::Binder binder(&mVertexArray);
        mBuffer.create();
        mBuffer.bind();
        mBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
        mBuffer.allocate(nullptr, sizeof(QVector2D));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        mBuffer.release();
    }

    return true;
}

void RectangleRenderer::render(RenderParameters params)
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

    QOpenGLVertexArrayObject::Binder binder(&mVertexArray);
    mBuffer.bind();
    mBuffer.write(0, &topLeft, sizeof(QVector2D));
    mBuffer.release();
    glDrawArrays(GL_POINTS, 0, 1);


}

void RectangleRenderer::setProjectionMatrix(QMatrix4x4 newMatrix)
{
    mProjectionMatrix = newMatrix;
}
