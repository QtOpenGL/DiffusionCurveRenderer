#ifndef LINERENDERER_H
#define LINERENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class LineRenderer : public QObject, protected QOpenGLFunctions
{

public:
    explicit LineRenderer(QObject *parent = nullptr);
    ~LineRenderer();

    enum LineStyle {
        Solid = 0,
        Dashed = 1
    };

    struct RenderParameters {
        QVector2D startingPoint, endPoint;
        QVector4D color;
        float thickness;
        LineStyle lineStyle;
        float dashLength;
        float gapLength;
    };

    bool initialize();
    void render(RenderParameters params);
    void setProjectionMatrix(QMatrix4x4 newMatrix);

private:
    LineRenderer(const LineRenderer&);
    LineRenderer& operator=(const LineRenderer&);

    int mProjectionMatrixLocation, mStartingPointLocation, mEndPointLocation, mColorLocation, mThicknessLocation, mLineStyleLocation;
    int mLineLengthLocation, mDashLengthLocation, mGapLengthLocation;
    int mRareTicksDeltaLocation, mDenseTicksDeltaLocation;

    QOpenGLShaderProgram* mShader;
    QOpenGLVertexArrayObject mRareTicksVertexArray, mDenseTicksVertexArray;
    QOpenGLBuffer mRareTicksBuffer, mDenseTicksBuffer;
    QMatrix4x4 mProjectionMatrix;

    QVector<float> mRareTicks, mDenseTicks;
    float mRareTicksDelta, mDenseTicksDelta;
};

#endif // LINERENDERER_H
