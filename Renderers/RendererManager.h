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

    float sharpness() const;
    void setSharpness(float newSharpness);

public slots:
    void onSmoothIterationsChanged(int smoothIterations);
    void onDiffusionWidthChanged(float diffusionWidth);

    void onContourThicknessChanged(float thickness);
    void onContourColorChanged(const QVector4D color);

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
