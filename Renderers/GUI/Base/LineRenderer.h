#ifndef LINERENDERER_H
#define LINERENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <Renderers/Base/Ticks.h>

class LineRenderer : protected QOpenGLFunctions
{
public:
    LineRenderer();
    ~LineRenderer();

    enum LineStyle { Solid = 0, Dashed = 1 };

    struct RenderParameters
    {
        QVector2D startingPoint;
        QVector2D endPoint;
        QVector4D color;
        LineStyle lineStyle;
        float thickness;
        float dashLength;
        float gapLength;
    };

    bool initialize();
    void render(const RenderParameters &params);
    void setProjectionMatrix(const QMatrix4x4 &newMatrix);

private:
    QOpenGLShaderProgram *mShader;

    Ticks *mDenseTicks;
    Ticks *mRareTicks;

    QMatrix4x4 mProjectionMatrix;

    int mProjectionMatrixLocation;
    int mStartingPointLocation;
    int mEndPointLocation;
    int mColorLocation;
    int mThicknessLocation;
    int mLineStyleLocation;
    int mLineLengthLocation;
    int mDashLengthLocation;
    int mGapLengthLocation;
    int mRareTicksDeltaLocation;
    int mDenseTicksDeltaLocation;
};

#endif // LINERENDERER_H
