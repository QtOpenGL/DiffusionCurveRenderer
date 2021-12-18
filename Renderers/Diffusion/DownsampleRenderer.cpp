#include "DownsampleRenderer.h"

DownsampleRenderer::DownsampleRenderer()
    : mInit(false)
{}

DownsampleRenderer::~DownsampleRenderer()
{
    // TODO
}

bool DownsampleRenderer::init()
{
    initializeOpenGLFunctions();

    mShader = new QOpenGLShaderProgram;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Downsample/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Downsample/FragmentShader.frag") || !mShader->link()
        || !mShader->bind()) {
        qCritical() << mShader->log();
        return false;
    }

    mSourceTextureLocation = mShader->uniformLocation("sourceTexture");

    mShader->bindAttributeLocation("vs_Position", 0);
    mShader->bindAttributeLocation("vs_TextureCoords", 1);
    mShader->release();

    mQuads = new Quads;
    mQuads->create();

    return mInit = true;
}

void DownsampleRenderer::render(GLuint texture)
{
    if (!mInit)
        return;

    mShader->bind();

    mShader->setUniformValue(mSourceTextureLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mShader->release();
}
