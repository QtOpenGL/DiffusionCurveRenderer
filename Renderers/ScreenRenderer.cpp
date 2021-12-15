#include "ScreenRenderer.h"

ScreenRenderer::ScreenRenderer()
    : mShader(nullptr)
    , mQuads(nullptr)
{}

ScreenRenderer::~ScreenRenderer()
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

bool ScreenRenderer::init()
{
    initializeOpenGLFunctions();
    mShader = new QOpenGLShaderProgram;
    mQuads = new Quads;

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Screen/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Screen/FragmentShader.frag") || !mShader->link() || !mShader->bind()) {
        qCritical() << mShader->log();
        return false;
    }

    mTextureLocation = mShader->uniformLocation("sourceTexture");

    qDebug() << "Screen Shader Location(s):" << mTextureLocation;

    mShader->bindAttributeLocation("vs_Position", 0);
    mShader->bindAttributeLocation("vs_TextureCoords", 1);

    mQuads->create();

    mShader->release();

    return true;
}

void ScreenRenderer::render(GLuint texture)
{
    mShader->bind();

    mShader->setUniformValue(mTextureLocation, GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mShader->release();
}
