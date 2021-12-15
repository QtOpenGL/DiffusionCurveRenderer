#include "RendererManager.h"
#include <QImage>

RendererManager::RendererManager()
    : mBufferSize(2048)
    , mQuality(1)
    , mInit(false)
    , mRenderQualityChanged(false)
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

    createFramebuffers();

    mContourRenderer = new ContourRenderer;
    mScreenRenderer = new ScreenRenderer;
    mDiffusionRenderer = new DiffusionRenderer;

    mInit = mContourRenderer->init();
    mInit &= mScreenRenderer->init();
    mInit &= mDiffusionRenderer->init();

    return mInit;
}

void RendererManager::createFramebuffers()
{
    // Create initial framebuffer
    mInitialFrameBuffer = new QOpenGLFramebufferObject(mQuality * mBufferSize, mQuality * mBufferSize, mFrambufferFormat);

    // Create downsampled and upsampled buffers
    int bufferSize = mQuality * mBufferSize;
    while (bufferSize > 0) {
        mDownsampledFramebuffers << new QOpenGLFramebufferObject(bufferSize, bufferSize, mFrambufferFormat);
        mUpsampledFramebuffers << new QOpenGLFramebufferObject(bufferSize, bufferSize, mFrambufferFormat);
        bufferSize = 0.5 * bufferSize;
    }
}

void RendererManager::deleteFramebuffers()
{
    for (int i = 0; i < mDownsampledFramebuffers.size(); i++) {
        if (mDownsampledFramebuffers[i]) {
            mDownsampledFramebuffers[i]->release();
            delete mDownsampledFramebuffers[i];
        }

        mDownsampledFramebuffers[i] = nullptr;
    }

    for (int i = 0; i < mUpsampledFramebuffers.size(); i++) {
        if (mUpsampledFramebuffers[i]) {
            mUpsampledFramebuffers[i]->release();
            delete mUpsampledFramebuffers[i];
        }
        mUpsampledFramebuffers[i] = nullptr;
    }

    if (mInitialFrameBuffer) {
        mInitialFrameBuffer->release();
        delete mInitialFrameBuffer;
    }

    mInitialFrameBuffer = nullptr;

    mDownsampledFramebuffers.clear();
    mUpsampledFramebuffers.clear();
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
    if (mRenderQualityChanged) {
        mRenderQualityChanged = false;
        mDiffusionRenderer->setQuality(mQuality);
        deleteFramebuffers();
        createFramebuffers();
    }

    QMatrix4x4 projectionMatrix;
    projectionMatrix.setToIdentity();
    projectionMatrix.ortho(mQuality * mProjectionParameters->left,
                           mQuality * mProjectionParameters->left + mQuality * mProjectionParameters->width * mProjectionParameters->zoomRatio

                           ,
                           mQuality * mProjectionParameters->top + mQuality * mProjectionParameters->height * mProjectionParameters->zoomRatio,
                           mQuality * mProjectionParameters->top,
                           -1,
                           1);

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
    mScreenRenderer->render(mUpsampledFramebuffers[0]->texture());
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

void RendererManager::onContourColorChanged(const QVector4D &color)
{
    mContourRenderer->setContourColor(color);
}

void RendererManager::onQualityChanged(int quality)
{
    mRenderQualityChanged = true;
    mQuality = quality;
}
