#ifndef BEZIERCONTOURRENDERER_H
#define BEZIERCONTOURRENDERER_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>

#include <Curves/Bezier.h>
#include <Renderers/Base/Ticks.h>

class ContourRenderer : protected QOpenGLExtraFunctions
{
public:
    ContourRenderer();
    ~ContourRenderer();

    bool init();
    void render(QVector<Curve *> curves, const QMatrix4x4 &projectionMatrix);

private:
    QVector4D lighter(QVector4D color, float factor); // 0 < factor < 1

    QOpenGLShaderProgram *mShader;
    Ticks *mTicks;

    GLuint mControlPointsBuffer;

    int mProjectionMatrixLocation;
    int mThicknessLocation;
    int mColorLocation;
    int mTicksDeltaLocation;
    int mControlPointsCountLocation;
};

#endif // BEZIERCONTOURRENDERER_H
