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

    if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/Diffusion/Screen/VertexShader.vert")
        || !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/Diffusion/Screen/FragmentShader.frag") || !mShader->link()
        || !mShader->bind()) {
        qCritical() << mShader->log();
        return false;
    }

    mWidthRatioLocation = mShader->uniformLocation("widthRatio");
    mHeightRatioLocation = mShader->uniformLocation("heightRatio");

    mShader->bindAttributeLocation("vs_Position", 0);
    mShader->bindAttributeLocation("vs_TextureCoords", 1);

    mQuads->create();

    mShader->release();

    return true;
}

void ScreenRenderer::render(const Parameters &parameters)
{
    mShader->bind();

    glBindTexture(GL_TEXTURE_2D, parameters.texture);

    mShader->setUniformValue(mWidthRatioLocation, parameters.widthRatio);
    mShader->setUniformValue(mHeightRatioLocation, parameters.heightRatio);

    mQuads->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mQuads->release();

    mShader->release();
}
