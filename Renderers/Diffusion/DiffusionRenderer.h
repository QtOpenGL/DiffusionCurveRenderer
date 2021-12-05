#ifndef DIFFUSIONRENDERER_H
#define DIFFUSIONRENDERER_H

#include <QImage>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <Curves/Curve.h>

#include <Renderers/Base/Ticks.h>

class DiffusionRenderer : protected QOpenGLFunctions
{
public:
    DiffusionRenderer(float bufferWidth, float bufferHeight);
    ~DiffusionRenderer();

    bool initialize();
    void render(const QVector<Curve *> &curves);
    GLuint getTexture() const;
    QImage toImage();

    void setProjectionMatrix(const QMatrix4x4 &newProjectionMatrix);

    float bufferWidth() const;
    float bufferHeight() const;

private:
    const float mBufferWidth;
    const float mBufferHeight;

    QOpenGLShaderProgram *mShader;

    QOpenGLFramebufferObject *mMultisampleFramebuffer;
    QOpenGLFramebufferObject *mDownsampledFramebuffer;

    QOpenGLFramebufferObjectFormat mMultisampleFrambufferFormat;
    QOpenGLFramebufferObjectFormat mDownsampledFrambufferFormat;

    Ticks *mTicks;

    int mProjectionMatrixLocation;
    int mTicksDeltaLocation;
    int mThicknessLocation;
    int mControlPointsLocation;
    int mControlPointsCountLocation;
    int mLeftColorsLocation;
    int mLeftColorPositionsLocation;
    int mLeftColorsCountLocation;
    int mRightColorsLocation;
    int mRightColorPositionsLocation;
    int mRightColorsCountLocation;

    QMatrix4x4 mProjectionMatrix;
};

#endif // DIFFUSIONRENDERER_H
