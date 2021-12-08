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

    bool init();
    void renderColorCurves(const QVector<Curve *> &curves, const QMatrix4x4 &projectionMatrix);
    void downsample(GLuint sourceTexture, float targetWidth, float targetHeight);
    void upsample(GLuint sourceTexture, GLuint targetTexture, float targetWidth, float targetHeight);
    void smooth(GLuint constrainedTexture, GLuint targetTexture, float targetWidth, float targetHeight, int iterations);

private:
    QOpenGLShaderProgram *mColorCurveShader;
    QMap<QString, Location> mColorCurveLocations;

    QOpenGLShaderProgram *mDownsamplerShader;
    QMap<QString, Location> mDownsamplerLocations;

    QOpenGLShaderProgram *mUpsamplerShader;
    QMap<QString, Location> mUpsamplerLocations;

    QOpenGLShaderProgram *mSmootherShader;
    QMap<QString, Location> mSmootherLocations;

    Ticks *mTicks;
    Quads *mQuads;

    bool mInit;
};

#endif // DIFFUSIONRENDERER_H
