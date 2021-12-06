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
    struct Parameters
    {
        GLuint texture;
        float widthRatio;
        float heightRatio;
    };

    ScreenRenderer();
    ~ScreenRenderer();

    bool init();
    void render(const Parameters &parameters);

private:
    QOpenGLShaderProgram *mShader;
    Quads *mQuads;

    int mWidthRatioLocation;
    int mHeightRatioLocation;
};

#endif // SCREENRENDERER_H
