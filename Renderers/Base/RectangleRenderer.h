#ifndef RECTANGLERENDERER_H
#define RECTANGLERENDERER_H

#include <Widgets/ModeWidget.h>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QVector4D>
#include <QVector>

class RectangleRenderer : protected QOpenGLFunctions
{
public:
    RectangleRenderer();
    ~RectangleRenderer();

    struct RenderParameters
    {
        QRectF rectangle;
        QVector4D fillColor;
        QVector4D borderColor;
        bool borderEnabled;
        float borderWidth;
    };

    bool initialize();
    void render(const RenderParameters &params);
    void setProjectionMatrix(const QMatrix4x4 &newMatrix);

private:
    QOpenGLShaderProgram *mShader;

    int mProjectionMatrixLocation;
    int mFillColorLocation;
    int mBorderEnabledLocation;
    int mBorderWidthLocation;
    int mBorderColorLocation;
    int mWidthLocation;
    int mHeightLocation;

    QOpenGLVertexArrayObject mVertexArray;
    QOpenGLBuffer mBuffer;
    QMatrix4x4 mProjectionMatrix;

    bool mInitialized;
};

#endif // RECTANGLERENDERER_H
