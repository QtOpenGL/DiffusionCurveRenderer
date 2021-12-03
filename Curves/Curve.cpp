#include "Curve.h"

#include <QRectF>

Curve::Curve()
    : mThickness(4)
    , mSelected(false)
    , mZ(0)
{}

Curve::~Curve()
{
    for (int i = 0; i < mControlPoints.size(); ++i)
        if (mControlPoints[i]) {
            delete mControlPoints[i];
            mControlPoints[i] = nullptr;
        }
}

float Curve::thickness() const { return mThickness; }

void Curve::setThickness(float newThickness) { mThickness = newThickness; }

bool Curve::selected() const { return mSelected; }

void Curve::setSelected(bool newSelected) { mSelected = newSelected; }

QVector<ControlPoint *> Curve::getControlPoints() { return mControlPoints; }

ControlPoint *Curve::getControlPoint(int index)
{
    if (0 <= index && index < mControlPoints.size())
        return mControlPoints[index];
    else
        return nullptr;
}

QVector<QVector2D> Curve::getControlPointPositions()
{
    QVector<QVector2D> positions;

    for (auto &point : mControlPoints)
        positions << point->position;

    return positions;
}

void Curve::addControlPoint(ControlPoint *controlPoint)
{
    if (mControlPoints.size() >= Constants::MAX_CONTROL_POINT_COUNT)
        return;

    switch (mControlPoints.size()) {
    case 0:
    case 1: {
        mControlPoints << controlPoint;
        updateControlPointIndices();
        break;
    }
    default: {
        mControlPoints.insert(mControlPoints.size() - 1, controlPoint);
        updateControlPointIndices();
        break;
    }
    }
}

void Curve::setControlPoint(int index, ControlPoint *controlPoint)
{
    mControlPoints.replace(index, controlPoint);
    updateControlPointIndices();
}

void Curve::removeControlPoint(int index)
{
    if (0 <= index && index < mControlPoints.size()) {
        delete mControlPoints[index];
        mControlPoints.removeAt(index);
        updateControlPointIndices();
    }
}

void Curve::removeControlPoint(ControlPoint *controlPoint)
{
    for (int i = 0; i < mControlPoints.size(); ++i) {
        ControlPoint *point = mControlPoints[i];

        if (point == controlPoint) {
            removeControlPoint(i);
            return;
        }
    }
}

int Curve::getOrder() { return mControlPoints.size(); }

int Curve::getDegree() { return mControlPoints.size() - 1; }

QVector<ColorPoint *> Curve::getColorPoints(ColorDirection direction)
{
    return (direction == Left ? mLeftColorPoints : mRightColorPoints);
}

void Curve::addColorPoint(ColorDirection direction, ColorPoint *colorPoint)
{
    switch (direction) {
    case Curve::Left: {
        if (colorPoint) {
            mLeftColorPoints << colorPoint;
            orderColorPoints(Curve::Left);
        }
        break;
    }
    case Curve::Right: {
        if (colorPoint) {
            mRightColorPoints << colorPoint;
            orderColorPoints(Curve::Right);
        }
        break;
    }
    }
}

ColorPoint *Curve::getColorPoint(ColorDirection direction, int index)
{
    QVector<ColorPoint *> &colorPoints = (direction == Left ? mLeftColorPoints : mRightColorPoints);

    if (0 <= index && index < colorPoints.size())
        return colorPoints[index];
    else
        return nullptr;
}

void Curve::removeColorPoint(ColorDirection direction, int index)
{
    switch (direction) {
    case Curve::Left:
        if (0 <= index && index < mLeftColorPoints.size()) {
            delete mLeftColorPoints[index];
            mLeftColorPoints.removeAt(index);
            orderColorPoints(Curve::Left);
        }
        break;
    case Curve::Right:
        if (0 <= index && index < mRightColorPoints.size()) {
            delete mRightColorPoints[index];
            mRightColorPoints.removeAt(index);
            orderColorPoints(Curve::Right);
        }
        break;
    }
}

void Curve::removeColorPoint(ColorDirection direction, ColorPoint *controlPoint)
{
    QVector<ColorPoint *> &colorPoints = (direction == Left ? mLeftColorPoints : mRightColorPoints);

    for (int i = 0; i < colorPoints.size(); ++i) {
        ColorPoint *point = colorPoints[i];

        if (point == controlPoint) {
            removeColorPoint(direction, i);
            return;
        }
    }
}

QVector4D Curve::colorAt(ColorDirection direction, float t)
{
    QVector<ColorPoint *> &colorPoints = (direction == Left ? mLeftColorPoints : mRightColorPoints);

    if (colorPoints.size() == 0 || colorPoints.size() == 1)
        return QVector4D(0, 0, 0, 0);

    QVector<ColorPoint *> enabledColorPoints;

    for (int i = 0; i < colorPoints.size(); ++i)
        if (colorPoints[i]->enabled)
            enabledColorPoints << colorPoints[i];

    if (enabledColorPoints.size() == 0)
        return QVector4D(0, 0, 0, 0);
    else if (enabledColorPoints.size() == 1)
        return enabledColorPoints[0]->color;

    for (int i = 1; i < enabledColorPoints.size(); ++i) {
        ColorPoint *previousColorPoint = enabledColorPoints[i - 1];
        ColorPoint *nextColorPoint = enabledColorPoints[i];

        if (previousColorPoint->position <= t && t <= nextColorPoint->position) {
            float s = (t - previousColorPoint->position) / (nextColorPoint->position - previousColorPoint->position);
            return previousColorPoint->color + s * (nextColorPoint->color - previousColorPoint->color);
        }
    }

    return QVector4D(0, 0, 0, 0);
}

void Curve::orderColorPoints(ColorDirection direction)
{
    QVector<ColorPoint *> &colorPoints = (direction == Left ? mLeftColorPoints : mRightColorPoints);

    if (colorPoints.size() == 0 || colorPoints.size() == 1)
        return;

    QVector<ColorPoint *> orderedColorPoints;

    orderedColorPoints << colorPoints[0];

    for (int i = 1; i < colorPoints.size(); ++i) {
        ColorPoint *currentColorPoint = colorPoints[i];

        if (orderedColorPoints.last()->position <= currentColorPoint->position)
            orderedColorPoints << currentColorPoint;
        else
            for (int j = 0; j < orderedColorPoints.size(); j++)
                if (currentColorPoint->position < orderedColorPoints[j]->position) {
                    orderedColorPoints.insert(j, currentColorPoint);
                    break;
                }
    }

    colorPoints = orderedColorPoints;
}

QVector<QVector4D> Curve::getColors(ColorDirection direction, bool onlyEnabledColorPoints)
{
    QVector<ColorPoint *> &colorPoints = (direction == Left ? mLeftColorPoints : mRightColorPoints);
    QVector<QVector4D> colors;
    for (int i = 0; i < colorPoints.size(); i++) {
        if (onlyEnabledColorPoints) {
            if (colorPoints[i]->enabled)
                colors << colorPoints[i]->color;
        } else {
            colors << colorPoints[i]->color;
        }
    }

    return colors;
}

QVector<float> Curve::getColorPositions(ColorDirection direction, bool onlyEnabledColorPoints)
{
    QVector<ColorPoint *> &colorPoints = (direction == Left ? mLeftColorPoints : mRightColorPoints);
    QVector<float> colorPointPositions;
    for (int i = 0; i < colorPoints.size(); i++) {
        if (onlyEnabledColorPoints) {
            if (colorPoints[i]->enabled)
                colorPointPositions << colorPoints[i]->position;
        } else {
            colorPointPositions << colorPoints[i]->position;
        }
    }

    return colorPointPositions;
}

int Curve::getSize() { return mControlPoints.size(); }

ControlPoint *Curve::getClosestControlPoint(QVector2D point)
{
    if (mControlPoints.size() == 0)
        return nullptr;

    float minimumDistance = std::numeric_limits<float>::infinity();
    int index = 0;

    for (int i = 0; i < mControlPoints.size(); ++i) {
        float distance = mControlPoints[i]->position.distanceToPoint(point);

        if (distance < minimumDistance) {
            minimumDistance = distance;
            index = i;
        }
    }

    return mControlPoints[index];
}

void Curve::deselectAllControlPoints()
{
    for (int i = 0; i < mControlPoints.size(); ++i) {
        mControlPoints[i]->selected = false;
    }
}

float Curve::distanceToPoint(QVector2D point)
{
    float minDistance = std::numeric_limits<float>::infinity();
    float t = 0;
    for (int i = 0; i < 100; ++i) {
        float distance = valueAt(t).distanceToPoint(point);
        if (distance < minDistance)
            minDistance = distance;

        t += 0.01;
    }

    return minDistance;
}

void Curve::translate(QVector2D translation)
{
    for (int i = 0; i < mControlPoints.size(); ++i) {
        ControlPoint *controlPoint = mControlPoints[i];
        controlPoint->position = controlPoint->position + translation;
    }
}

void Curve::translate(QPointF translation) { translate(QVector2D(translation)); }

float Curve::length(int intervals)
{
    float length = 0.0f;
    float t = 0.0f;
    const float dt = 1.0f / static_cast<float>(intervals);

    for (int i = 0; i < intervals; ++i) {
        QVector2D v1 = valueAt(t);
        QVector2D v2 = valueAt(t + dt);
        length += v1.distanceToPoint(v2);
        t += dt;
    }

    return length;
}

QRectF Curve::getBoundingBox(int intervals)
{
    float xMin = std::numeric_limits<float>::infinity();
    float xMax = -std::numeric_limits<float>::infinity();
    float yMin = std::numeric_limits<float>::infinity();
    float yMax = -std::numeric_limits<float>::infinity();

    const float dt = 1.0f / static_cast<float>(intervals);
    float t = 0.0f;

    for (int i = 0; i < intervals; i++) {
        QVector2D value = valueAt(t);

        if (value.x() > xMax)
            xMax = value.x();
        if (value.x() < xMin)
            xMin = value.x();
        if (value.y() > yMax)
            yMax = value.y();
        if (value.y() < yMin)
            yMin = value.y();

        t += dt;
    }

    return QRectF(xMin, yMin, xMax - xMin, yMax - yMin);
}

int Curve::z() const { return mZ; }

void Curve::setZ(int newZ) { mZ = newZ; }

void Curve::updateControlPointIndices()
{
    for (int i = 0; i < mControlPoints.size(); ++i) {
        mControlPoints[i]->index = i;
    }
}
