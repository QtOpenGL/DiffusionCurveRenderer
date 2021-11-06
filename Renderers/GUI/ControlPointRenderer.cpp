#include "ControlPointRenderer.h"

ControlPointRenderer::ControlPointRenderer(QObject *parent) :
    QObject(parent),
    mPointRenderer(nullptr),
    mMode(ModeWidget::Select),
    mZoomRatio(1.0f)
{

}

ControlPointRenderer::~ControlPointRenderer()
{

}

bool ControlPointRenderer::initialize()
{
    mPointRenderer = new PointRenderer(this);

    return mPointRenderer->initialize();
}

void ControlPointRenderer::render(Curve* curve)
{
    if(curve == nullptr)
        return;

    switch (mMode) {
    case ModeWidget::Pan:
        break;
    case ModeWidget::Add:
    case ModeWidget::Select:
    {

        QVector<ControlPoint*> controlPoints = curve->getControlPoints();

        for(int j = 0; j < controlPoints.size(); ++j)
        {
            PointRenderer::RenderParameters params;
            params.point = controlPoints[j]->position;
            params.innerRadius = mZoomRatio * (controlPoints[j]->selected ? 6 : 6);
            params.outerRadius = mZoomRatio * (controlPoints[j]->selected ? 12 : 10);
            params.innerColor =  QVector4D(240 / 255.0, 240 / 255.0, 240 / 255.0, 1);
            params.outerColor = QVector4D(120 / 255.0, 120 / 255.0, 120 / 255.0, 0.75);

            mPointRenderer->render(params);
        }
        break;
    }
    case ModeWidget::Move:
        break;

    }
}

void ControlPointRenderer::setProjectionMatrix(QMatrix4x4 newMatrix)
{
    mPointRenderer->setProjectionMatrix(newMatrix);
}

ModeWidget::Mode ControlPointRenderer::mode() const
{
    return mMode;
}

void ControlPointRenderer::setMode(ModeWidget::Mode newMode)
{
    mMode = newMode;
}

float ControlPointRenderer::zoomRatio() const
{
    return mZoomRatio;
}
void ControlPointRenderer::setZoomRatio(float newZoomRatio)
{
    mZoomRatio = newZoomRatio;
}
