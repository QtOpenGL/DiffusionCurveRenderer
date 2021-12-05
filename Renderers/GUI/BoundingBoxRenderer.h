#ifndef BOUNDINGBOXRENDERER_H
#define BOUNDINGBOXRENDERER_H

#include <Curves/Curve.h>
#include <Renderers/GUI/Base/LineRenderer.h>
#include <Renderers/GUI/Base/RectangleRenderer.h>

class BoundingBoxRenderer
{
public:
    BoundingBoxRenderer();
    ~BoundingBoxRenderer();

    bool initialize();
    void render(Curve *curve);
    void setProjectionMatrix(const QMatrix4x4 &newMatrix);

    void setZoomRatio(float newZoomRatio);

private:
    RectangleRenderer *mRectangleRenderer;
    LineRenderer *mLineRenderer;
    float mZoomRatio;
};

#endif // BOUNDINGBOXRENDERER_H
