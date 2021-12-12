#include "RendererManager.h"

#include <QImage>

#define BUFFER_SIZE 4096.0

RendererManager::RendererManager()
    : mSmoothIterations(20)
    , mQuality(1)
    , mInit(false)
{}

RendererManager::~RendererManager() {}

bool RendererManager::init()
{
    initializeOpenGLFunctions();

    // Define framebuffer format
    mFrambufferFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    mFrambufferFormat.setSamples(0);
    mFrambufferFormat.setTextureTarget(GL_TEXTURE_2D);
    mFrambufferFormat.setInternalTextureFormat(GL_RGBA8);

    // Create initial framebuffer
    mInitialFrameBuffer = new QOpenGLFramebufferObject(BUFFER_SIZE, BUFFER_SIZE, mFrambufferFormat);

    for (int i = 0; i < 2; ++i) {
        mBlurFramebuffers << new QOpenGLFramebufferObject(BUFFER_SIZE, BUFFER_SIZE, mFrambufferFormat);
    }

    // Create downsampled and upsampled buffers
    int bufferSize = 0.5 * BUFFER_SIZE;
    while (bufferSize > 0) {
        mDownsampledFramebuffers << new QOpenGLFramebufferObject(bufferSize, bufferSize, mFrambufferFormat);
        mUpsampledFramebuffers << new QOpenGLFramebufferObject(bufferSize, bufferSize, mFrambufferFormat);
        bufferSize = 0.5 * bufferSize;
    }

    mContourRenderer = new ContourRenderer;
    mScreenRenderer = new ScreenRenderer;
    mDiffusionRenderer = new DiffusionRenderer;

    mInit = mContourRenderer->init();
    mInit &= mScreenRenderer->init();
    mInit &= mDiffusionRenderer->init();

    return mInit;
}

void RendererManager::contours()
{
    if (!mInit)
        return;

    // Set projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.setToIdentity();
    projectionMatrix.ortho(mProjectionParameters->left, mProjectionParameters->right, mProjectionParameters->bottom, mProjectionParameters->top, -1, 1);

    // Render
    mContourRenderer->render(mCurveContainer->getCurves(), projectionMatrix);
}

void RendererManager::diffuse()
{
    //    QMatrix4x4 projectionMatrix;
    //    projectionMatrix.setToIdentity();
    //    projectionMatrix.ortho(mProjectionParameters->left,
    //                           mProjectionParameters->left + mInitialFrameBuffer->width(),
    //                           mProjectionParameters->top + mInitialFrameBuffer->height(),
    //                           mProjectionParameters->top,
    //                           -1,
    //                           1);

    QMatrix4x4 projectionMatrix;
    projectionMatrix.setToIdentity();
    projectionMatrix.ortho(mProjectionParameters->left, mProjectionParameters->right, mProjectionParameters->bottom, mProjectionParameters->top, -1, 1);

    // Render color curves into initial framebuffer
    {
        mInitialFrameBuffer->bind();
        glViewport(0, 0, mInitialFrameBuffer->width(), mInitialFrameBuffer->height());
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        mDiffusionRenderer->renderColorCurves(mCurveContainer->getCurves(), projectionMatrix);
        mInitialFrameBuffer->release();
    }

    // First downsample
    {
        mDownsampledFramebuffers[0]->bind();
        glViewport(0, 0, mDownsampledFramebuffers[0]->width(), mDownsampledFramebuffers[0]->height());
        //glViewport(0, 0, mProjectionParameters->width, mProjectionParameters->height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        mDiffusionRenderer->downsample(mInitialFrameBuffer->texture(), mDownsampledFramebuffers[0]->width(), mDownsampledFramebuffers[0]->height());
        mInitialFrameBuffer->release();
    }

    // Downsample 1,2,3...
    {
        for (int i = 1; i < mDownsampledFramebuffers.size(); ++i) {
            mDownsampledFramebuffers[i]->bind();
            glViewport(0, 0, mDownsampledFramebuffers[i]->width(), mDownsampledFramebuffers[i]->height());
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            mDiffusionRenderer->downsample(mDownsampledFramebuffers[i - 1]->texture(),
                                           mDownsampledFramebuffers[i]->width(),
                                           mDownsampledFramebuffers[i]->height());
            mDownsampledFramebuffers[i]->release();
        }
    }

    // Upsample n-2
    {
        int secondLast = mUpsampledFramebuffers.size() - 2;

        mUpsampledFramebuffers[secondLast]->bind();
        glViewport(0, 0, mUpsampledFramebuffers[secondLast]->width(), mUpsampledFramebuffers[secondLast]->height());
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        mDiffusionRenderer->upsample(mUpsampledFramebuffers.last()->texture(),
                                     mUpsampledFramebuffers[secondLast]->texture(),
                                     mUpsampledFramebuffers[secondLast]->width(),
                                     mUpsampledFramebuffers[secondLast]->height());

        mDiffusionRenderer->smooth(mDownsampledFramebuffers[secondLast]->texture(),
                                   mUpsampledFramebuffers[secondLast]->texture(),
                                   mUpsampledFramebuffers[secondLast]->width(),
                                   mUpsampledFramebuffers[secondLast]->height(),
                                   mSmoothIterations);
        mUpsampledFramebuffers[secondLast]->release();
    }

    // Upsample n-3,...
    {
        for (int i = mUpsampledFramebuffers.size() - 3; 0 <= i; --i)

        {
            mUpsampledFramebuffers[i]->bind();
            glViewport(0, 0, mUpsampledFramebuffers[i]->width(), mUpsampledFramebuffers[i]->height());
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            mDiffusionRenderer->upsample(mUpsampledFramebuffers[i + 1]->texture(),
                                         mUpsampledFramebuffers[i]->texture(),
                                         mUpsampledFramebuffers[i]->width(),
                                         mUpsampledFramebuffers[i]->height());

            mDiffusionRenderer->smooth(mDownsampledFramebuffers[i]->texture(),
                                       mUpsampledFramebuffers[i]->texture(),
                                       mUpsampledFramebuffers[i]->width(),
                                       mUpsampledFramebuffers[i]->height(),
                                       mSmoothIterations);
            mUpsampledFramebuffers[i]->release();
        }
    }

    QOpenGLFramebufferObject::bindDefault();
    glViewport(0, 0, mProjectionParameters->width, mProjectionParameters->height);
    ScreenRenderer::Parameters parameters;
    parameters.texture = mUpsampledFramebuffers[0]->texture();
    parameters.widthRatio = 1;
    parameters.heightRatio = 1;
    mScreenRenderer->render(parameters);
}

void RendererManager::setCurveContainer(const CurveContainer *newCurveContainer)
{
    mCurveContainer = newCurveContainer;
}

void RendererManager::setProjectionParameters(const ProjectionParameters *newProjectionParameters)
{
    mProjectionParameters = newProjectionParameters;
}

int RendererManager::smoothIterations() const
{
    return mSmoothIterations;
}

void RendererManager::setSmoothIterations(int newSmoothIterations)
{
    mSmoothIterations = newSmoothIterations;
}

int RendererManager::quality() const
{
    return mQuality;
}

void RendererManager::setQuality(int newQuality)
{
    mQuality = newQuality;
}

void RendererManager::clear()
{
    mInitialFrameBuffer->bind();
    glViewport(0, 0, mInitialFrameBuffer->width(), mInitialFrameBuffer->height());

    // Clear framebuffer
    glColorMask(TRUE, TRUE, TRUE, TRUE);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mInitialFrameBuffer->release();

    for (int i = 0; i < mDownsampledFramebuffers.size(); i++) {
        mDownsampledFramebuffers[i]->bind();
        glViewport(0, 0, mDownsampledFramebuffers[i]->width(), mDownsampledFramebuffers[i]->height());

        // Clear framebuffer
        glColorMask(TRUE, TRUE, TRUE, TRUE);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mDownsampledFramebuffers[i]->release();
    }

    for (int i = 0; i < mUpsampledFramebuffers.size(); i++) {
        mUpsampledFramebuffers[i]->bind();
        glViewport(0, 0, mUpsampledFramebuffers[i]->width(), mUpsampledFramebuffers[i]->height());

        // Clear framebuffer
        glColorMask(TRUE, TRUE, TRUE, TRUE);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mUpsampledFramebuffers[i]->release();
    }
}
