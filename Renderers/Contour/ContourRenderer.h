#ifndef BEZIERCONTOURRENDERER_H
#define BEZIERCONTOURRENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <Curves/Bezier.h>
#include <Renderers/Base/Ticks.h>
#include <Widgets/ModeWidget.h>

class ContourRenderer : protected QOpenGLExtraFunctions
{
public:
    ContourRenderer();
    ~ContourRenderer();

    bool initialize();
    void render(QVector<Curve *> curves, bool highlightSelectedCurve);
    void setProjectionMatrix(const QMatrix4x4 &newMatrix);
    void setZoomRatio(float newZoomRatio);
    void setShowContours(bool newShowContours);

private:
    void render(Bezier *curve, QVector4D color);
    QVector4D lighter(QVector4D color, float factor); // 0 < factor < 1

    QOpenGLShaderProgram *mShader;
    Ticks *mTicks;

    int mProjectionMatrixLocation;
    int mThicknessLocation;
    int mColorLocation;
    int mTicksDeltaLocation;
    int mControlPointsCountLocation;

    GLuint mControlPointsBuffer;

    QMatrix4x4 mProjectionMatrix;

    float mZoomRatio;
    bool mShowContours;
    bool mInitialized;
};

#endif // BEZIERCONTOURRENDERER_H
