#ifndef DIFFUSIONRENDERER_H
#define DIFFUSIONRENDERER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include <Curves/Curve.h>
#include <Renderers/Base/Ticks.h>

class DiffusionRenderer : protected QOpenGLFunctions
{
public:
    DiffusionRenderer();
    ~DiffusionRenderer();

    bool init();
    void render(const QVector<Curve *> &curves, const QMatrix4x4 &projectionMatrix);

private:
    QOpenGLShaderProgram *mShader;
    Ticks *mTicks;

    int mProjectionMatrixLocation;
    int mTicksDeltaLocation;
    int mThicknessLocation;
    int mControlPointsLocation;
    int mControlPointsCountLocation;
    int mLeftColorsLocation;
    int mLeftColorPositionsLocation;
    int mLeftColorsCountLocation;
    int mRightColorsLocation;
    int mRightColorPositionsLocation;
    int mRightColorsCountLocation;
};

#endif // DIFFUSIONRENDERER_H
