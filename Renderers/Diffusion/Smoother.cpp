#include "Smoother.h"

Smoother::Smoother()
    : mShader(nullptr)
    , mQuads(nullptr)
{}

Smoother::~Smoother()
{
    if (mShader) {
        delete mShader;
        mShader = nullptr;
    }

    if (mQuads) {
        delete mQuads;
        mQuads = nullptr;
    }
}

bool Smoother::init()
{
    initializeOpenGLFunctions();
    mShader = new QOpenGLShaderProgram;
    mQuads = new Quads;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Smoother/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Smoother/FragmentShader.frag") || !mShader->link()
        || !mShader->bind()) {
        qCritical() << mShader->log();
        return false;
    }

    mConstrainedTextureLocation = mShader->uniformLocation("constrainedTexture");
    mTargetTextureLocation = mShader->uniformLocation("targetTexture");
    mTargetWidthLocation = mShader->uniformLocation("targetWidth");
    mTargetHeightLocation = mShader->uniformLocation("targetHeight");

    mShader->bindAttributeLocation("vs_Position", 0);
    mShader->bindAttributeLocation("vs_TextureCoords", 1);

    qDebug() << "Smoother:" << mConstrainedTextureLocation << mTargetTextureLocation << mTargetWidthLocation << mTargetHeightLocation;

    mQuads->create();

    mShader->release();

    return true;
}

void Smoother::smooth(GLuint constrainedTexture, GLuint targetTexture, float targetWidth, float targetHeight)
{
    mShader->bind();

    mShader->setUniformValue(mConstrainedTextureLocation, 3);
    mShader->setUniformValue(mTargetTextureLocation, 4);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, constrainedTexture);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, targetTexture);

    mShader->setUniformValue(mTargetWidthLocation, targetWidth);
    mShader->setUniformValue(mTargetHeightLocation, targetHeight);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mShader->release();
}
