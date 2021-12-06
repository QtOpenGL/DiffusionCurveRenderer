#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "ScreenRenderer.h"

#include <QOpenGLFramebufferObject>

#include <CurveContainer.h>
#include <Renderers/Contour/ContourRenderer.h>
#include <Widgets/OpenGLWidget.h>

class RendererManager : protected QOpenGLFunctions
{
public:
    RendererManager();
    ~RendererManager();

    bool init();
    void render();
    void update();

    void setCurveContainer(const CurveContainer *newCurveContainer);
    void setProjectionParameters(const OpenGLWidget::ProjectionParameters *newProjectionParameters);

private:
    ContourRenderer *mContourRenderer;
    ScreenRenderer *mScreenRenderer;
    const CurveContainer *mCurveContainer;

    QOpenGLFramebufferObject *mMultisampleFramebuffer;
    QOpenGLFramebufferObject *mDownsampledFramebuffer;

    QOpenGLFramebufferObjectFormat mMultisampleFrambufferFormat;
    QOpenGLFramebufferObjectFormat mDownsampledFrambufferFormat;

    const float mBufferWidth;
    const float mBufferHeight;

    const OpenGLWidget::ProjectionParameters *mProjectionParameters;

    bool mInit;
};

#endif // RENDERERMANAGER_H
