#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "ContourRenderer.h"
#include "DiffusionRenderer.h"
#include "ScreenRenderer.h"

#include <QOpenGLFramebufferObject>

#include <CurveContainer.h>
#include <Widgets/OpenGLWidget.h>

class RendererManager : protected QOpenGLFunctions
{
public:
    RendererManager();
    ~RendererManager();

    bool init();
    void createFramebuffers();
    void deleteFramebuffers();
    void contours();
    void diffuse();

    void setCurveContainer(const CurveContainer *newCurveContainer);
    void setProjectionParameters(const ProjectionParameters *newProjectionParameters);

public slots:
    void onSmoothIterationsChanged(int smoothIterations);
    void onDiffusionWidthChanged(float diffusionWidth);
    void onContourThicknessChanged(float thickness);
    void onContourColorChanged(const QVector4D &color);
    void onQualityChanged(int quality);

private:
    ContourRenderer *mContourRenderer;
    ScreenRenderer *mScreenRenderer;
    DiffusionRenderer *mDiffusionRenderer;

    const CurveContainer *mCurveContainer;
    const ProjectionParameters *mProjectionParameters;

    QOpenGLFramebufferObjectFormat mFrambufferFormat;
    QOpenGLFramebufferObject *mInitialFrameBuffer;
    QVector<QOpenGLFramebufferObject *> mDownsampledFramebuffers;
    QVector<QOpenGLFramebufferObject *> mUpsampledFramebuffers;

    const int mBufferSize;
    int mQuality;
    bool mInit;
    bool mRenderQualityChanged;
};

#endif // RENDERERMANAGER_H
