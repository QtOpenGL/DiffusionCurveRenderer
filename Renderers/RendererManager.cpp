#include "RendererManager.h"

#include <QImage>

#define BUFFER_SIZE 4096.0

RendererManager::RendererManager()
    : mInit(false)
{}

RendererManager::~RendererManager() {}

bool RendererManager::init()
{
    initializeOpenGLFunctions();

    // Define framebuffer format
    mFrambufferFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    mFrambufferFormat.setSamples(0);
    mFrambufferFormat.setMipmap(false);
    mFrambufferFormat.setTextureTarget(GL_TEXTURE_2D);
    mFrambufferFormat.setInternalTextureFormat(GL_RGBA8);

    // Create initial framebuffer
    mInitialFrameBuffer = new QOpenGLFramebufferObject(BUFFER_SIZE, BUFFER_SIZE, mFrambufferFormat);

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
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        DiffusionRenderer::Parameters parameters;
        parameters.sourceTexture = mInitialFrameBuffer->texture();
        parameters.targetWidth = mDownsampledFramebuffers[0]->width();
        parameters.targetHeight = mDownsampledFramebuffers[0]->height();

        mDiffusionRenderer->downsample(parameters);

        mInitialFrameBuffer->release();
    }

    // Downsample 1,2,3...
    {
        for (int i = 1; i < mDownsampledFramebuffers.size(); ++i) {
            mDownsampledFramebuffers[i]->bind();
            glViewport(0, 0, mDownsampledFramebuffers[i]->width(), mDownsampledFramebuffers[i]->height());
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            DiffusionRenderer::Parameters parameters;
            parameters.sourceTexture = mDownsampledFramebuffers[i - 1]->texture();
            parameters.targetWidth = mDownsampledFramebuffers[i]->width();
            parameters.targetHeight = mDownsampledFramebuffers[i]->height();

            mDiffusionRenderer->downsample(parameters);

            mDownsampledFramebuffers[i]->release();
        }
    }

    // Upsample and smooth n-2
    {
        int secondLast = mUpsampledFramebuffers.size() - 2;

        mUpsampledFramebuffers[secondLast]->bind();
        glViewport(0, 0, mUpsampledFramebuffers[secondLast]->width(), mUpsampledFramebuffers[secondLast]->height());
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Upsample
        DiffusionRenderer::Parameters parameters;
        parameters.sourceTexture = mUpsampledFramebuffers.last()->texture();
        parameters.targetTexture = mUpsampledFramebuffers[secondLast]->texture();
        parameters.targetWidth = mUpsampledFramebuffers[secondLast]->width();
        parameters.targetHeight = mUpsampledFramebuffers[secondLast]->height();

        mDiffusionRenderer->upsample(parameters);

        // Smooth
        parameters.sourceTexture = mDownsampledFramebuffers[secondLast]->texture();
        parameters.targetTexture = mUpsampledFramebuffers[secondLast]->texture();
        parameters.targetWidth = mUpsampledFramebuffers[secondLast]->width();
        parameters.targetHeight = mUpsampledFramebuffers[secondLast]->height();

        mDiffusionRenderer->smooth(parameters);

        mUpsampledFramebuffers[secondLast]->release();
    }

    // Upsample and smooth remaining n-3,...
    {
        for (int i = mUpsampledFramebuffers.size() - 3; 0 <= i; --i)

        {
            mUpsampledFramebuffers[i]->bind();
            glViewport(0, 0, mUpsampledFramebuffers[i]->width(), mUpsampledFramebuffers[i]->height());
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            // Upsample
            DiffusionRenderer::Parameters parameters;
            parameters.sourceTexture = mUpsampledFramebuffers[i + 1]->texture();
            parameters.targetTexture = mUpsampledFramebuffers[i]->texture();
            parameters.targetWidth = mUpsampledFramebuffers[i]->width();
            parameters.targetHeight = mUpsampledFramebuffers[i]->height();

            mDiffusionRenderer->upsample(parameters);

            // Smooth
            parameters.sourceTexture = mDownsampledFramebuffers[i]->texture();
            parameters.targetTexture = mUpsampledFramebuffers[i]->texture();
            parameters.targetWidth = mUpsampledFramebuffers[i]->width();
            parameters.targetHeight = mUpsampledFramebuffers[i]->height();

            mDiffusionRenderer->smooth(parameters);

            mUpsampledFramebuffers[i]->release();
        }
    }

    // Blit to default framebuffer
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

void RendererManager::onSmoothIterationsChanged(int smoothIterations)
{
    mDiffusionRenderer->setSmoothIterations(smoothIterations);
}

void RendererManager::onDiffusionWidthChanged(float diffusionWidth)
{
    mDiffusionRenderer->setDiffusionWidth(diffusionWidth);
}

void RendererManager::onContourThicknessChanged(float thickness)
{
    mContourRenderer->setContourThickness(thickness);
}

void RendererManager::onContourColorChanged(const QVector4D color)
{
    mContourRenderer->setContourColor(color);
}
