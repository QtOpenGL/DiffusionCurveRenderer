#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "ScreenRenderer.h"

#include <QOpenGLFramebufferObject>

#include <CurveContainer.h>
#include <Renderers/Contour/ContourRenderer.h>
#include <Renderers/Diffusion/DiffusionRenderer.h>
#include <Widgets/OpenGLWidget.h>

class RendererManager : protected QOpenGLFunctions
{
public:
    RendererManager();
    ~RendererManager();

    bool init();
    void render();
    void update();

    void diffuse();

    void setCurveContainer(const CurveContainer *newCurveContainer);
    void setProjectionParameters(const OpenGLWidget::ProjectionParameters *newProjectionParameters);

private:
    ContourRenderer *mContourRenderer;
    ScreenRenderer *mScreenRenderer;
    DiffusionRenderer *mDiffusionRenderer;

    const CurveContainer *mCurveContainer;

    QOpenGLFramebufferObjectFormat mMultisampleFrambufferFormat;
    QOpenGLFramebufferObjectFormat mDownsampledFrambufferFormat;
    QOpenGLFramebufferObjectFormat mUpsampledFramebufferFormat;

    QOpenGLFramebufferObject *mMultisampleFramebuffer;
    QOpenGLFramebufferObject *mBlittedFrameBuffer;

    QVector<QOpenGLFramebufferObject *> mDownsampledFramebuffers;
    QVector<QOpenGLFramebufferObject *> mUpsampledFramebuffers;

    const OpenGLWidget::ProjectionParameters *mProjectionParameters;

    bool mInit;
};

#endif // RENDERERMANAGER_H
