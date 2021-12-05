#include "ControlPointRenderer.h"

ControlPointRenderer::ControlPointRenderer()
    : mPointRenderer(nullptr)
    , mZoomRatio(1.0f)
{}

ControlPointRenderer::~ControlPointRenderer()
{
    if (mPointRenderer)
        delete mPointRenderer;

    mPointRenderer = nullptr;
}

bool ControlPointRenderer::initialize()
{
    mPointRenderer = new PointRenderer;
    return mPointRenderer->initialize();
}

void ControlPointRenderer::render(Curve *curve)
{
    if (curve == nullptr)
        return;

    QVector<ControlPoint *> controlPoints = curve->getControlPoints();

    for (int j = 0; j < controlPoints.size(); ++j) {
        PointRenderer::RenderParameters params;
        params.point = controlPoints[j]->position;
        params.innerRadius = mZoomRatio * (controlPoints[j]->selected ? 6 : 6);
        params.outerRadius = mZoomRatio * (controlPoints[j]->selected ? 12 : 10);
        params.innerColor = QVector4D(240 / 255.0f, 240 / 255.0f, 240 / 255.0f, 1);
        params.outerColor = QVector4D(120 / 255.0f, 120 / 255.0f, 120 / 255.0f, 0.75f);

        mPointRenderer->render(params);
    }
}

void ControlPointRenderer::setProjectionMatrix(const QMatrix4x4 &newMatrix)
{
    mPointRenderer->setProjectionMatrix(newMatrix);
}

void ControlPointRenderer::setZoomRatio(float newZoomRatio)
{
    mZoomRatio = newZoomRatio;
}
