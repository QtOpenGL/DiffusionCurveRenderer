#ifndef CONTROLPOINTRENDERER_H
#define CONTROLPOINTRENDERER_H

#include "PointRenderer.h"

#include <Curves/Curve.h>
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
};

#endif // CONTROLPOINTRENDERER_H
