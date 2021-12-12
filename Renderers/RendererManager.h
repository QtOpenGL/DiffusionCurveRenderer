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
    void contours();
    void diffuse();

    void setCurveContainer(const CurveContainer *newCurveContainer);
    void setProjectionParameters(const ProjectionParameters *newProjectionParameters);

    int smoothIterations() const;
    void setSmoothIterations(int newSmoothIterations);

    int quality() const;
    void setQuality(int newQuality);

    void clear();

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

    int mSmoothIterations;
    int mQuality;

    bool mInit;
};

#endif // RENDERERMANAGER_H
