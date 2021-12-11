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
    void render();
    void update();

    void diffuse();

    void setCurveContainer(const CurveContainer *newCurveContainer);
    void setProjectionParameters(const ProjectionParameters *newProjectionParameters);

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

    bool mInit;
};

#endif // RENDERERMANAGER_H
