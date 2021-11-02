#ifndef POINTRENDERER_H
#define POINTRENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>

class PointRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    struct RenderParameters {
        QVector2D point;
        float innerRadius, outerRadius;
        QVector4D innerColor, outerColor;
    };

    explicit PointRenderer(QObject *parent = nullptr);
    ~PointRenderer();

    bool initialize();
    void render(RenderParameters params);
    void setProjectionMatrix(QMatrix4x4 newMatrix);

private:
    PointRenderer(const PointRenderer&);
    PointRenderer& operator=(const PointRenderer&);

    QOpenGLShaderProgram* mShader;
    int mProjectionMatrixLocation, mPointLocation, mInnerColorLocation, mOuterColorLocation, mInnerRadiusLocation, mOuterRadiusLocation, mTicksDeltaLocation;
    QOpenGLVertexArrayObject mTicksVertexArray;
    QOpenGLBuffer mTicksBuffer;
    QMatrix4x4 mProjectionMatrix;
    QVector<float> mTicks;
    float mTicksDelta;
};

#endif // POINTRENDERER_H
