#ifndef BEZIERDIFFUSIONRENDERER_H
#define BEZIERDIFFUSIONRENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

#include <Constants.h>
#include <Curves/Curve.h>

class BezierDiffusionRenderer : protected QOpenGLExtraFunctions
{
public:
    BezierDiffusionRenderer();
    ~BezierDiffusionRenderer();

    bool initialize();
    void render(QVector<Curve *> curves);

    void setProjectionMatrix(const QMatrix4x4 &newProjectionMatrix);

private:
    QOpenGLShaderProgram *mShader;
    QOpenGLVertexArrayObject mTicksVertexArray;
    QOpenGLBuffer mTicksBuffer;

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

    QMatrix4x4 mProjectionMatrix;

    QVector<float> mTicks;
    float mTicksDelta;

    bool mInitialized;
};

#endif // BEZIERDIFFUSIONRENDERER_H
