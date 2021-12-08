#ifndef UPSAMPLER_H
#define UPSAMPLER_H

#include <QOpenGLShaderProgram>

#include <Renderers/Base/Quads.h>

class Upsampler : protected QOpenGLFunctions
{
public:
    Upsampler();
    ~Upsampler();

    bool init();
    void upsample(GLuint sourceTexture, GLuint targetTexture, float targetWidth, float targetHeight);

private:
    QOpenGLShaderProgram *mShader;
    Quads *mQuads;

    int mTargetTextureLocation;
    int mSourceTextureLocation;
};

#endif // UPSAMPLER_H
