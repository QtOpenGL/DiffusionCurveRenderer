#ifndef CONTROLPOINTRENDERER_H
#define CONTROLPOINTRENDERER_H

#include <Curves/Curve.h>
#include <Renderers/GUI/Base/PointRenderer.h>

class ControlPointRenderer
{
public:
    ControlPointRenderer();
    ~ControlPointRenderer();

    bool initialize();
    void render(Curve *curve);

    void setProjectionMatrix(const QMatrix4x4 &newMatrix);
    void setZoomRatio(float newZoomRatio);

private:
    PointRenderer *mPointRenderer;
    float mZoomRatio;
};

#endif // CONTROLPOINTRENDERER_H
