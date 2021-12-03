#include "BoundingBoxRenderer.h"

BoundingBoxRenderer::BoundingBoxRenderer()
    : mRectangleRenderer(nullptr)
    , mLineRenderer(nullptr)
    , mZoomRatio(1.0)
{}

BoundingBoxRenderer::~BoundingBoxRenderer()
{
    if (mRectangleRenderer)
        delete mRectangleRenderer;

    if (mLineRenderer)
        delete mLineRenderer;
}

bool BoundingBoxRenderer::initialize()
{
    mRectangleRenderer = new RectangleRenderer;
    mLineRenderer = new LineRenderer;

    if (!mRectangleRenderer->initialize())
        return false;

    if (!mLineRenderer->initialize())
        return false;

    return true;
}

void BoundingBoxRenderer::render(Curve *curve)
{
    if (curve == nullptr)
        return;

    switch (mMode) {
    case ModeWidget::Pan:
        break;
    case ModeWidget::Select:
        break;
    case ModeWidget::Add:
        break;
    case ModeWidget::Move: {
        if (curve->selected()) {
            QRectF boundingBox = curve->getBoundingBox();

            // Dashed Lines
            {
                LineRenderer::RenderParameters params;
                params.color = QVector4D(0.6, 0.6, 0.6, 1);
                params.thickness = mZoomRatio * 1;
                params.dashLength = 4;
                params.gapLength = 4;
                params.lineStyle = LineRenderer::Dashed;

                params.startingPoint = QVector2D(boundingBox.topLeft()) + mZoomRatio * QVector2D(0, 5);
                params.endPoint = QVector2D(boundingBox.topRight()) + mZoomRatio * QVector2D(0, 5);
                mLineRenderer->render(params);

                params.startingPoint = QVector2D(boundingBox.topRight()) + mZoomRatio * QVector2D(5, 0);
                params.endPoint = QVector2D(boundingBox.bottomRight()) + mZoomRatio * QVector2D(5, 0);
                mLineRenderer->render(params);

                params.startingPoint = QVector2D(boundingBox.bottomRight()) + mZoomRatio * QVector2D(0, 5);
                params.endPoint = QVector2D(boundingBox.bottomLeft()) + mZoomRatio * QVector2D(0, 5);
                mLineRenderer->render(params);

                params.startingPoint = QVector2D(boundingBox.bottomLeft()) + mZoomRatio * QVector2D(5, 0);
                params.endPoint = QVector2D(boundingBox.topLeft()) + mZoomRatio * QVector2D(5, 0);
                mLineRenderer->render(params);
            }

            // Corners
            {
                RectangleRenderer::RenderParameters params;
                params.fillColor = QVector4D(0.6, 0.6, 0.6, 1);
                params.borderEnabled = false;
                params.borderWidth = 0;
                params.borderColor = QVector4D(1, 1, 1, 1);
                QSizeF size = mZoomRatio * QSizeF(10, 10);

                params.rectangle = QRectF(boundingBox.topLeft(), size);
                mRectangleRenderer->render(params);

                params.rectangle = QRectF(boundingBox.topRight(), size);
                mRectangleRenderer->render(params);

                params.rectangle = QRectF(boundingBox.bottomLeft(), size);
                mRectangleRenderer->render(params);

                params.rectangle = QRectF(boundingBox.bottomRight(), size);
                mRectangleRenderer->render(params);
            }
        }
        break;
    }
    }
}

void BoundingBoxRenderer::setProjectionMatrix(const QMatrix4x4 &newMatrix)
{
    mRectangleRenderer->setProjectionMatrix(newMatrix);
    mLineRenderer->setProjectionMatrix(newMatrix);
}

ModeWidget::Mode BoundingBoxRenderer::mode() const
{
    return mMode;
}

void BoundingBoxRenderer::setMode(ModeWidget::Mode newMode)
{
    mMode = newMode;
}

float BoundingBoxRenderer::zoomRatio() const
{
    return mZoomRatio;
}

void BoundingBoxRenderer::setZoomRatio(float newZoomRatio)
{
    mZoomRatio = newZoomRatio;
}
