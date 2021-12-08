#include "DiffusionRenderer.h"

DiffusionRenderer::DiffusionRenderer()
    : mColorCurveRenderer(nullptr)
    , mDownsampler(nullptr)
    , mUpsampler(nullptr)
    , mInit(false)
{}

DiffusionRenderer::~DiffusionRenderer()
{
    if (mColorCurveRenderer)
        delete mColorCurveRenderer;

    if (mDownsampler)
        delete mDownsampler;

    if (mUpsampler)
        delete mUpsampler;

    mColorCurveRenderer = nullptr;
    mDownsampler = nullptr;
    mUpsampler = nullptr;
}

bool DiffusionRenderer::init()
{
    mColorCurveRenderer = new ColorCurveRenderer;
    mDownsampler = new Downsampler;
    mUpsampler = new Upsampler;
    mSmoother = new Smoother;

    mInit = mColorCurveRenderer->init();
    mInit &= mDownsampler->init();
    mInit &= mUpsampler->init();
    mInit &= mSmoother->init();

    return mInit;
}

void DiffusionRenderer::renderColorCurves(const QVector<Curve *> &curves, const QMatrix4x4 &projectionMatrix)
{
    if (!mInit)
        return;

    mColorCurveRenderer->render(curves, projectionMatrix);
}

void DiffusionRenderer::downsample(GLuint sourceTexture, float targetWidth, float targetHeight)
{
    if (!mInit)
        return;

    mDownsampler->downsample(sourceTexture, targetWidth, targetHeight);
}

void DiffusionRenderer::upsample(GLuint sourceTexture, GLuint targetTexture, float targetWidth, float targetHeight)
{
    if (!mInit)
        return;

    mUpsampler->upsample(sourceTexture, targetTexture, targetWidth, targetHeight);
}

void DiffusionRenderer::smooth(GLuint constrainedTexture, GLuint targetTexture, float targetWidth, float targetHeight)
{
    if (!mInit)
        return;

    for (int i = 0; i < 20; ++i)
        mSmoother->smooth(constrainedTexture, targetTexture, targetWidth, targetHeight);
}
