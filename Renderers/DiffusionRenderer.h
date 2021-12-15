#ifndef DIFFUSIONRENDERER_H
#define DIFFUSIONRENDERER_H

#include <QOpenGLShaderProgram>

#include <Renderers/Base/Quads.h>
#include <Renderers/Base/Ticks.h>

#include <Curves/Curve.h>

typedef int Location;

class DiffusionRenderer : protected QOpenGLFunctions
{
public:
    DiffusionRenderer();
    ~DiffusionRenderer();

    struct Parameters
    {
        GLuint sourceTexture;
        GLuint targetTexture;
        float targetWidth;
        float targetHeight;
    };

    bool init();
    void renderColorCurves(const QVector<Curve *> &curves, const QMatrix4x4 &projectionMatrix);
    void blur(const Parameters &parameters);
    void downsample(const Parameters &parameters);
    void upsample(const Parameters &parameters);
    void smooth(const Parameters &parameters);

    int smoothIterations() const;
    void setSmoothIterations(int newSmoothIterations);

    float diffusionWidth() const;
    void setDiffusionWidth(float newDiffusionWidth);

    int quality() const;
    void setQuality(int newQuality);

private:
    QOpenGLShaderProgram *mColorCurveShader;
    QMap<QString, Location> mColorCurveLocations;

    QOpenGLShaderProgram *mDownsamplerShader;
    QMap<QString, Location> mDownsamplerLocations;

    QOpenGLShaderProgram *mUpsamplerShader;
    QMap<QString, Location> mUpsamplerLocations;

    QOpenGLShaderProgram *mSmootherShader;
    QMap<QString, Location> mSmootherLocations;

    QOpenGLShaderProgram *mBlurShader;
    QMap<QString, Location> mBlurShaderLocations;

    Ticks *mTicks;
    Quads *mQuads;

    int mQuality;
    int mSmoothIterations;
    float mDiffusionWidth;

    bool mInit;
};

#endif // DIFFUSIONRENDERER_H
