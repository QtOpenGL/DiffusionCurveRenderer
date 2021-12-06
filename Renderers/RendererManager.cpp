#include "RendererManager.h"

#include <QImage>

RendererManager::RendererManager()
    : mBufferWidth(4096)
    , mBufferHeight(4096)
    , mInit(false)
{}

RendererManager::~RendererManager() {}

bool RendererManager::init()
{
    initializeOpenGLFunctions();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);

    mMultisampleFrambufferFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    mMultisampleFrambufferFormat.setMipmap(true);
    mMultisampleFrambufferFormat.setSamples(16);
    mMultisampleFrambufferFormat.setTextureTarget(GL_TEXTURE_2D);
    mMultisampleFrambufferFormat.setInternalTextureFormat(GL_RGBA8);
    mMultisampleFramebuffer = new QOpenGLFramebufferObject(mBufferWidth, mBufferHeight, mMultisampleFrambufferFormat);

    mDownsampledFrambufferFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    mDownsampledFrambufferFormat.setMipmap(true);
    mDownsampledFrambufferFormat.setTextureTarget(GL_TEXTURE_2D);
    mDownsampledFrambufferFormat.setInternalTextureFormat(GL_RGBA8);
    mDownsampledFramebuffer = new QOpenGLFramebufferObject(mBufferWidth, mBufferHeight, mDownsampledFrambufferFormat);

    mContourRenderer = new ContourRenderer;
    mScreenRenderer = new ScreenRenderer;

    mInit = mContourRenderer->init();
    mInit &= mScreenRenderer->init();

    return mInit;
}

void RendererManager::render()
{
    if (!mInit)
        return;

    // Clear framebuffer
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.setToIdentity();
    projectionMatrix.ortho(mProjectionParameters->left, mProjectionParameters->right, mProjectionParameters->bottom, mProjectionParameters->top, -1, 1);

    // Render curves
    mContourRenderer->render(mCurveContainer->getCurves(), projectionMatrix);
}

void RendererManager::update() {}

void RendererManager::setCurveContainer(const CurveContainer *newCurveContainer)
{
    mCurveContainer = newCurveContainer;
}

void RendererManager::setProjectionParameters(const OpenGLWidget::ProjectionParameters *newProjectionParameters)
{
    mProjectionParameters = newProjectionParameters;
}
