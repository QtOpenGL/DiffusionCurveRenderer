#ifndef BOUNDINGBOXRENDERER_H
#define BOUNDINGBOXRENDERER_H

#include <Renderers/Base/RectangleRenderer.h>
#include <Renderers/Base/LineRenderer.h>
#include <Curves/Curve.h>
#include <Widgets/ModeWidget.h>

class BoundingBoxRenderer : public QObject
{
    Q_OBJECT

public:
    BoundingBoxRenderer(QObject *parent = nullptr);
    ~BoundingBoxRenderer();

    bool initialize();
    void render(Curve* curve);
    void setProjectionMatrix(QMatrix4x4 newMatrix);

    ModeWidget::Mode mode() const;
    void setMode(ModeWidget::Mode newMode);

    float zoomRatio() const;
    void setZoomRatio(float newZoomRatio);

private:
    BoundingBoxRenderer(const BoundingBoxRenderer&);
    BoundingBoxRenderer& operator=(const BoundingBoxRenderer&);

    RectangleRenderer *mRectangleRenderer;
    LineRenderer* mLineRenderer;
    ModeWidget::Mode mMode;
    float mZoomRatio;
};

#endif // BOUNDINGBOXRENDERER_H
