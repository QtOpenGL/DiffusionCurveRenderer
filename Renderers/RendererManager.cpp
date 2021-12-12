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
    // Define multisample format
    mFrambufferFormat.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    mFrambufferFormat.setSamples(0);
    mFrambufferFormat.setTextureTarget(GL_TEXTURE_2D);
    mFrambufferFormat.setInternalTextureFormat(GL_RGBA8);

    // Create multisample framebuffer and blitted framebuffer objects
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

    // Clear framebuffer
    //    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.setToIdentity();
    projectionMatrix.ortho(mProjectionParameters->left, mProjectionParameters->right, mProjectionParameters->bottom, mProjectionParameters->top, -1, 1);
    mContourRenderer->render(mCurveContainer->getCurves(), projectionMatrix);
}

void RendererManager::diffuse()
{
    QMatrix4x4 projectionMatrix;
    projectionMatrix.setToIdentity();
    projectionMatrix.ortho(mProjectionParameters->left, mProjectionParameters->right, mProjectionParameters->bottom, mProjectionParameters->top, -1, 1);

    // Diffuse
    {
        // Bind my framebuffer
        mInitialFrameBuffer->bind();
        glViewport(0, 0, mInitialFrameBuffer->width(), mInitialFrameBuffer->height());

        // Clear framebuffer
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set projection matrix
        //        QMatrix4x4 projectionMatrix;
        //        projectionMatrix.setToIdentity();
        //        projectionMatrix.ortho(-mInitialFrameBuffer->width() / 2,
        //                               mInitialFrameBuffer->width() / 2,
        //                               -mInitialFrameBuffer->height() / 2,
        //                               mInitialFrameBuffer->height() / 2,
        //                               -1,
        //                       1);

        // Diffuse curves
        mDiffusionRenderer->renderColorCurves(mCurveContainer->getCurves(), projectionMatrix);
        //mInitialFrameBuffer->toImage().save(QString("Initial.png"));
        mInitialFrameBuffer->release();
    }

    //    // Downsample
    {
        for (int i = 0; i < mDownsampledFramebuffers.size(); i++) {
            mDownsampledFramebuffers[i]->bind();
            glViewport(0, 0, mDownsampledFramebuffers[i]->width(), mDownsampledFramebuffers[i]->height());

            GLuint sourceTexture = i == 0 ? mInitialFrameBuffer->texture() : mDownsampledFramebuffers[i - 1]->texture();
            mDiffusionRenderer->downsample(sourceTexture, 2 * mDownsampledFramebuffers[i]->width(), 2 * mDownsampledFramebuffers[i]->height());

            mDownsampledFramebuffers[i]->release();
        }
    }

    //    // Upsample and smooth
    {
        GLuint sourceTexture = mDownsampledFramebuffers.last()->texture();
        for (int i = mUpsampledFramebuffers.size() - 2; 0 <= i; i--) {
            mUpsampledFramebuffers[i]->bind();
            glViewport(0, 0, mUpsampledFramebuffers[i]->width(), mUpsampledFramebuffers[i]->height());

            GLuint targetTexture = mDownsampledFramebuffers[i]->texture();
            mDiffusionRenderer->upsample(sourceTexture, targetTexture, mUpsampledFramebuffers[i]->width(), mUpsampledFramebuffers[i]->height());

            sourceTexture = mUpsampledFramebuffers[i]->texture();
            GLuint constrainedTexture = mDownsampledFramebuffers[i]->texture();
            //mUpsampledFramebuffers[i]->toImage().save(QString("Upsampled %1.png").arg(i));
            mDiffusionRenderer->smooth(constrainedTexture,
                                       sourceTexture,
                                       mUpsampledFramebuffers[i]->width(),
                                       mUpsampledFramebuffers[i]->height(),
                                       mSmoothIterations);

            mUpsampledFramebuffers[i]->release();
        }
    }

    //To screen
    {
        QOpenGLFramebufferObject::bindDefault();

        glViewport(0, 0, mProjectionParameters->width, mProjectionParameters->height);

        ScreenRenderer::Parameters parameters;
        parameters.widthRatio = 1;
        parameters.heightRatio = 1;
        parameters.texture = mUpsampledFramebuffers[0]->texture();
        //parameters.texture = mDownsampledFramebuffers.last()->texture();
        mScreenRenderer->render(parameters);
    }
    //clear();
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
