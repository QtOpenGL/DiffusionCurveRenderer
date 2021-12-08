#include "Downsampler.h"

Downsampler::Downsampler()
    : mShader(nullptr)
    , mQuads(nullptr)
{}

Downsampler::~Downsampler()
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

bool Downsampler::init()
{
    initializeOpenGLFunctions();
    mShader = new QOpenGLShaderProgram;
    mQuads = new Quads;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Downsampler/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Downsampler/FragmentShader.frag") || !mShader->link()
        || !mShader->bind()) {
        qCritical() << mShader->log();
        return false;
    }

    mSourceTextureLocation = mShader->uniformLocation("sourceTexture");
    mTargetWidthLocation = mShader->uniformLocation("targetWidth");
    mTargetHeightLocation = mShader->uniformLocation("targetHeight");

    mShader->bindAttributeLocation("vs_Position", 0);
    mShader->bindAttributeLocation("vs_TextureCoords", 1);

    mQuads->create();

    mShader->release();

    return true;
}

void Downsampler::downsample(GLuint sourceTexture, float targetWidth, float targetHeight)
{
    mShader->bind();

    mShader->setUniformValue(mSourceTextureLocation, GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceTexture);

    mShader->setUniformValue(mTargetWidthLocation, targetWidth);
    mShader->setUniformValue(mTargetHeightLocation, targetHeight);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mShader->release();
}
