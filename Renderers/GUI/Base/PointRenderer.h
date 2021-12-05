#ifndef POINTRENDERER_H
#define POINTRENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>

#include <Renderers/Base/Ticks.h>

class PointRenderer : protected QOpenGLFunctions
{
public:
    PointRenderer();
    ~PointRenderer();

    struct RenderParameters
    {
        QVector2D point;
        float innerRadius;
        float outerRadius;
        QVector4D innerColor;
        QVector4D outerColor;
    };

    bool initialize();
    void render(const RenderParameters &params);
    void setProjectionMatrix(const QMatrix4x4 &newMatrix);

private:
    QOpenGLShaderProgram *mShader;
    Ticks *mTicks;

    int mProjectionMatrixLocation;
    int mPointLocation;
    int mInnerColorLocation;
    int mOuterColorLocation;
    int mInnerRadiusLocation;
    int mOuterRadiusLocation;
    int mTicksDeltaLocation;

    QMatrix4x4 mProjectionMatrix;
};

#endif // POINTRENDERER_H
