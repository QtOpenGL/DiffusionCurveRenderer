#ifndef RECTANGLERENDERER_H
#define RECTANGLERENDERER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QVector4D>
#include <QVector>

#include <Renderers/Base/Ticks.h>

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
    Ticks *mTicks;

    int mProjectionMatrixLocation;
    int mFillColorLocation;
    int mBorderEnabledLocation;
    int mBorderWidthLocation;
    int mBorderColorLocation;
    int mWidthLocation;
    int mHeightLocation;
    int mRectangleTopLeftLocation;

    QMatrix4x4 mProjectionMatrix;
};

#endif // RECTANGLERENDERER_H
