#ifndef SMOOTHER_H
#define SMOOTHER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include <Renderers/Base/Quads.h>

class Smoother : protected QOpenGLFunctions
{
public:
    Smoother();
    ~Smoother();

    bool init();
    void smooth(GLuint constrainedTexture, GLuint targetTexture, float targetWidth, float targetHeight);

private:
    QOpenGLShaderProgram *mShader;
    Quads *mQuads;

    int mConstrainedTextureLocation;
    int mTargetTextureLocation;
    int mTargetWidthLocation;
    int mTargetHeightLocation;
};

#endif // SMOOTHER_H
