#include "ScreenRenderer.h"

#include <QOpenGLFramebufferObject>

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

bool ScreenRenderer::initialize()
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

    mFramebufferToTargetWidthRatioLocation = mShader->uniformLocation("frameBufferToTargetWidthRatio");
    mFramebufferToTargetHeightRatioLocation = mShader->uniformLocation("frameBufferToTargetHeightRatio");

    mShader->bindAttributeLocation("vs_Position", 0);
    mShader->bindAttributeLocation("vs_TextureCoords", 1);

    mQuads->create();

    mShader->release();

    return true;
}

void ScreenRenderer::render(GLuint textureId, float framebufferToTargetWidthRatio, float framebufferToTargetHeightRatio)
{
    mShader->bind();

    QOpenGLFramebufferObject::bindDefault();
    glBindTexture(GL_TEXTURE_2D, textureId);
    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mShader->setUniformValue(mFramebufferToTargetWidthRatioLocation, framebufferToTargetWidthRatio);
    mShader->setUniformValue(mFramebufferToTargetHeightRatioLocation, framebufferToTargetHeightRatio);

    mQuads->bind();
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    mQuads->release();

    mShader->release();
}
