#ifndef RECTANGLERENDERER_H
#define RECTANGLERENDERER_H

#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <Widgets/ModeWidget.h>
#include <QVector4D>

class RectangleRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit RectangleRenderer(QObject *parent = nullptr);
    ~RectangleRenderer();

    struct RenderParameters {
        QRectF rectangle;
        QVector4D fillColor;
        bool borderEnabled;
        float borderWidth;
        QVector4D borderColor;

    };

    bool initialize();
    void render(RenderParameters params);
    void setProjectionMatrix(QMatrix4x4 newMatrix);

private:
    RectangleRenderer(const RectangleRenderer&);
    RectangleRenderer& operator=(const RectangleRenderer&);

    QOpenGLShaderProgram* mShader;
    int mProjectionMatrixLocation, mFillColorLocation, mBorderEnabledLocation, mBorderWidthLocation, mBorderColorLocation, mWidthLocation, mHeightLocation;
    QOpenGLVertexArrayObject mVertexArray;
    QOpenGLBuffer mBuffer;
    QMatrix4x4 mProjectionMatrix;
};

#endif // RECTANGLERENDERER_H
