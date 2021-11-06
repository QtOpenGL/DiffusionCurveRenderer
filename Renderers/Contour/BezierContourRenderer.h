#ifndef BEZIERCONTOURRENDERER_H
#define BEZIERCONTOURRENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>

#include <Curves/Bezier.h>
#include <Widgets/ModeWidget.h>

class BezierContourRenderer : public QObject,  protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    BezierContourRenderer(QObject *parent = nullptr);
    ~BezierContourRenderer();

    bool initialize();
    void render(QVector<Curve*> curves);
    void setProjectionMatrix(QMatrix4x4 newMatrix);

    void setMode(ModeWidget::Mode newMode);
    void setZoomRatio(float newZoomRatio);

    void setShowContours(bool newShowContours);

private:
    BezierContourRenderer(const BezierContourRenderer&);
    BezierContourRenderer& operator=(const BezierContourRenderer&);

    void render(Bezier* curves, QVector4D color);
    QVector4D lighter(QVector4D color, float factor); // 0 < factor < 1
    QVector<Curve*> orderCurves(QVector<Curve*> curves);

    int mProjectionMatrixLocation, mThicknessLocation, mColorLocation, mTicksDeltaLocation, mControlPointsCountLocation;
    QOpenGLShaderProgram* mShader;
    QOpenGLVertexArrayObject mTicksVertexArray;
    QOpenGLBuffer mTicksBuffer;
    GLuint mControlPointsBuffer;

    QMatrix4x4 mProjectionMatrix;

    QVector<float> mTicks;
    float mTicksDelta;

    ModeWidget::Mode mMode;
    float mZoomRatio;
    bool mShowContours;

};

#endif // BEZIERCONTOURRENDERER_H
