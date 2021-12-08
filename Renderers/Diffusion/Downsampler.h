#ifndef DOWNSAMPLER_H
#define DOWNSAMPLER_H

#include <QOpenGLShaderProgram>

#include <Renderers/Base/Quads.h>

class Downsampler : protected QOpenGLFunctions
{
public:
    Downsampler();
    ~Downsampler();

    bool init();
    void downsample(GLuint sourceTexture, float targetWidth, float targetHeight);

private:
    QOpenGLShaderProgram *mShader;
    Quads *mQuads;

    int mSourceTextureLocation;
    int mTargetWidthLocation;
    int mTargetHeightLocation;
};

#endif // DOWNSAMPLER_H
