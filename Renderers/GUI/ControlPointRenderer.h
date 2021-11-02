#ifndef CONTROLPOINTRENDERER_H
#define CONTROLPOINTRENDERER_H

#include <Renderers/Base/PointRenderer.h>
#include <Curves/Curve.h>
#include <Widgets/ModeWidget.h>

class ControlPointRenderer : public QObject
{
    Q_OBJECT

public:
    explicit ControlPointRenderer(QObject *parent = nullptr);
    ~ControlPointRenderer();

    bool initialize();
    void render(Curve* curve);
    void setProjectionMatrix(QMatrix4x4 newMatrix);

    ModeWidget::Mode mode() const;
    void setMode(ModeWidget::Mode newMode);

    float zoomRatio() const;
    void setZoomRatio(float newZoomRatio);

private:
    ControlPointRenderer(const ControlPointRenderer&);
    ControlPointRenderer& operator=(const ControlPointRenderer&);

    PointRenderer* mPointRenderer;
    ModeWidget::Mode mMode;

    float mZoomRatio;
};

#endif // CONTROLPOINTRENDERER_H
