#ifndef BEZIERCONTOURRENDERER_H
#define BEZIERCONTOURRENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <Curves/Bezier.h>
#include <Widgets/ModeWidget.h>

class BezierContourRenderer : protected QOpenGLExtraFunctions
{
public:
    BezierContourRenderer();
    ~BezierContourRenderer();

    bool initialize();
    void render(QVector<Curve *> curves, bool highlightSelectedCurve);
    void setProjectionMatrix(const QMatrix4x4 &newMatrix);

    void setMode(ModeWidget::Mode newMode);
    void setZoomRatio(float newZoomRatio);
    void setShowContours(bool newShowContours);

private:
    void render(Bezier *curve, QVector4D color);
    QVector4D lighter(QVector4D color, float factor); // 0 < factor < 1

    QOpenGLShaderProgram *mShader;

    int mProjectionMatrixLocation;
    int mThicknessLocation;
    int mColorLocation;
    int mTicksDeltaLocation;
    int mControlPointsCountLocation;

    QOpenGLVertexArrayObject mTicksVertexArray;
    QOpenGLBuffer mTicksBuffer;
    GLuint mControlPointsBuffer;

    QMatrix4x4 mProjectionMatrix;

    QVector<float> mTicks;
    float mTicksDelta;

    ModeWidget::Mode mMode;
    float mZoomRatio;
    bool mShowContours;

    bool mInitialized;
};

#endif // BEZIERCONTOURRENDERER_H
