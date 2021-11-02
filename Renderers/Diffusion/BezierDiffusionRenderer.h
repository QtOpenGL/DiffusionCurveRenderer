#ifndef BEZIERDIFFUSIONRENDERER_H
#define BEZIERDIFFUSIONRENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <Curves/Curve.h>
#include <Constants.h>

class BezierDiffusionRenderer : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit BezierDiffusionRenderer(QObject *parent = nullptr);
    ~BezierDiffusionRenderer();

    bool initialize();
    void render(QVector<Curve*> curves);

    void setProjectionMatrix(const QMatrix4x4 &newProjectionMatrix);

private:
    int mProjectionMatrixLocation, mTicksDeltaLocation, mThicknessLocation;
    int mControlPointsLocation, mControlPointsCountLocation;
    int mLeftColorsLocation, mLeftColorPositionsLocation, mLeftColorsCountLocation;
    int mRightColorsLocation, mRightColorPositionsLocation, mRightColorsCountLocation;

    QOpenGLShaderProgram* mShader;
    QOpenGLVertexArrayObject mTicksVertexArray;
    QOpenGLBuffer mTicksBuffer;

    QMatrix4x4 mProjectionMatrix;

    QVector<float> mTicks;
    float mTicksDelta;

signals:

};

#endif // BEZIERDIFFUSIONRENDERER_H
