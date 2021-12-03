#ifndef BOUNDINGBOXRENDERER_H
#define BOUNDINGBOXRENDERER_H

#include "LineRenderer.h"
#include "RectangleRenderer.h"

#include <Curves/Curve.h>
#include <Widgets/ModeWidget.h>

class BoundingBoxRenderer
{
public:
    BoundingBoxRenderer();
    ~BoundingBoxRenderer();

    bool initialize();
    void render(Curve *curve);
    void setProjectionMatrix(const QMatrix4x4 &newMatrix);

    ModeWidget::Mode mode() const;
    void setMode(ModeWidget::Mode newMode);
    float zoomRatio() const;
    void setZoomRatio(float newZoomRatio);

private:
    RectangleRenderer *mRectangleRenderer;
    LineRenderer *mLineRenderer;
    ModeWidget::Mode mMode;
    float mZoomRatio;
};

#endif // BOUNDINGBOXRENDERER_H
