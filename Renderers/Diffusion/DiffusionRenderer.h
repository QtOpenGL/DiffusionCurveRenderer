#ifndef DIFFUSIONRENDERER_H
#define DIFFUSIONRENDERER_H

#include "ColorCurveRenderer.h"
#include "Downsampler.h"
#include "Smoother.h"
#include "Upsampler.h"

class DiffusionRenderer
{
public:
    DiffusionRenderer();
    ~DiffusionRenderer();

    bool init();
    void renderColorCurves(const QVector<Curve *> &curves, const QMatrix4x4 &projectionMatrix);
    void downsample(GLuint sourceTexture, float targetWidth, float targetHeight);
    void upsample(GLuint sourceTexture, GLuint targetTexture, float targetWidth, float targetHeight);
    void smooth(GLuint constrainedTexture, GLuint targetTexture, float targetWidth, float targetHeight);

private:
    ColorCurveRenderer *mColorCurveRenderer;
    Downsampler *mDownsampler;
    Upsampler *mUpsampler;
    Smoother *mSmoother;

    bool mInit;
};

#endif // DIFFUSIONRENDERER_H
