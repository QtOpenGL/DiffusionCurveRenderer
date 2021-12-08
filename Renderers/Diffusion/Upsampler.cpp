#include "Upsampler.h"

Upsampler::Upsampler()
    : mShader(nullptr)
    , mQuads(nullptr)
{}

Upsampler::~Upsampler()
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

bool Upsampler::init()
{
    initializeOpenGLFunctions();
    mShader = new QOpenGLShaderProgram;
    mQuads = new Quads;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Upsampler/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Upsampler/FragmentShader.frag") || !mShader->link()
        || !mShader->bind()) {
        qCritical() << mShader->log();
        return false;
    }

    mSourceTextureLocation = mShader->uniformLocation("sourceTexture");
    mTargetTextureLocation = mShader->uniformLocation("targetTexture");

    mShader->bindAttributeLocation("vs_Position", 0);
    mShader->bindAttributeLocation("vs_TextureCoords", 1);

    mQuads->create();

    mShader->release();

    return true;
}

void Upsampler::upsample(GLuint sourceTexture, GLuint targetTexture, float targetWidth, float targetHeight)
{
    mShader->bind();

    mShader->setUniformValue(mSourceTextureLocation, 0);
    mShader->setUniformValue(mTargetTextureLocation, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, targetTexture);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mShader->release();
}
