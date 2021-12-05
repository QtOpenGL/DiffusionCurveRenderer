#ifndef SCREENRENDERER_H
#define SCREENRENDERER_H

#include <Renderers/Base/Quads.h>

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

class ScreenRenderer : protected QOpenGLFunctions
{
public:
    ScreenRenderer();
    ~ScreenRenderer();

    bool initialize();
    void render(GLuint textureId, float framebufferToTargetWidthRatio, float framebufferToTargetHeightRatio);

private:
    QOpenGLShaderProgram *mShader;
    Quads *mQuads;

    int mScreenTextureLocation;
    int mFramebufferToTargetWidthRatioLocation;
    int mFramebufferToTargetHeightRatioLocation;
};

#endif // SCREENRENDERER_H
