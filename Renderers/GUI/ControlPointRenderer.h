#ifndef CONTROLPOINTRENDERER_H
#define CONTROLPOINTRENDERER_H

#include <Curves/Curve.h>
#include <Renderers/Base/PointRenderer.h>
#include <Widgets/ModeWidget.h>

class ControlPointRenderer
{
public:
    ControlPointRenderer();
    ~ControlPointRenderer();

    bool initialize();
    void render(Curve *curve);

    void setProjectionMatrix(const QMatrix4x4 &newMatrix);

    ModeWidget::Mode mode() const;
    void setMode(ModeWidget::Mode newMode);

    float zoomRatio() const;
    void setZoomRatio(float newZoomRatio);

private:
    PointRenderer *mPointRenderer;

    ModeWidget::Mode mMode;
    float mZoomRatio;
    bool mInitialized;
};

#endif // CONTROLPOINTRENDERER_H
