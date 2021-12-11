#include "Curve.h"

#include <QLineF>
#include <QRectF>

Curve::Curve()
    : mContourColorEnabled(true)
    , mContourColor(0, 0, 0, 1)
    , mContourThickness(4.0f)
    , mDiffusionWidth(4.0f)
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

const bool &Curve::selected() const
{
    return mSelected;
}

void Curve::setSelected(bool newSelected)
{
    mSelected = newSelected;
}

QVector<const ControlPoint *> Curve::getControlPoints() const
{
    QVector<const ControlPoint *> points;
    for (auto &controlPoint : mControlPoints) {
        points << controlPoint;
    }

    return points;
}

const ControlPoint *Curve::getControlPoint(int index) const
{
    if (0 <= index && index < mControlPoints.size())
        return mControlPoints[index];
    else
        return nullptr;
}

QVector<QVector2D> Curve::getControlPointPositions() const
{
    QVector<QVector2D> positions;

    for (auto &point : mControlPoints)
        positions << point->position;

    return positions;
}
#include <QDebug>
void Curve::addControlPoint(ControlPoint *controlPoint, bool append)
{
    if (mControlPoints.size() >= Constants::MAX_CONTROL_POINT_COUNT)
        return;

    if (append) {
        mControlPoints << controlPoint;
    } else {
        if (mControlPoints.size() == 0 || mControlPoints.size() == 1) {
            mControlPoints << controlPoint;
        } else {
            QVector<QVector2D> positions = getControlPointPositions();
            QVector<float> parameters;
            parameters.reserve(positions.size());

            QVector2D startingPoint;
            QVector2D direction;
            findLineOfBestFit(positions, startingPoint, direction);

            for (int i = 0; i < positions.size(); i++) {
                parameters << perpendicularAt(startingPoint, direction, positions[i]);
            }

            float t = perpendicularAt(startingPoint, direction, controlPoint->position);
            bool instertionIndex = parameters.size() - 1;
            for (int i = 0; i < parameters.size() - 1; i++) {
                if (parameters[i] <= t && t <= parameters[i + 1]) {
                    instertionIndex = i + 1;
                    break;
                }
            }

            mControlPoints.insert(instertionIndex, controlPoint);
        }
    }

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

int Curve::getOrder() const
{
    return mControlPoints.size();
}

int Curve::getDegree() const
{
    return mControlPoints.size() - 1;
}

QVector<const ColorPoint *> Curve::getLeftColorPoints() const
{
    QVector<const ColorPoint *> points;

    for (auto &controlPoint : mLeftColorPoints) {
        points << controlPoint;
    }

    return points;
}

QVector<const ColorPoint *> Curve::getRightColorPoints() const
{
    QVector<const ColorPoint *> points;

    for (auto &controlPoint : mRightColorPoints) {
        points << controlPoint;
    }

    return points;
}

void Curve::addColorPoint(ColorPoint *colorPoint)
{
    switch (colorPoint->type) {
    case ColorPoint::Left: {
        if (colorPoint) {
            mLeftColorPoints << colorPoint;
            orderLeftColorPoints();
        }
        break;
    }
    case ColorPoint::Right: {
        if (colorPoint) {
            mRightColorPoints << colorPoint;
            orderRightColorPoints();
        }
        break;
    }
    }
}

const ColorPoint *Curve::getLeftColorPoint(int index) const
{
    if (0 <= index && index < mLeftColorPoints.size())
        return mLeftColorPoints[index];
    else
        return nullptr;
}

const ColorPoint *Curve::getRightColorPoint(int index) const
{
    if (0 <= index && index < mRightColorPoints.size())
        return mRightColorPoints[index];
    else
        return nullptr;
}

void Curve::removeLeftColorPoint(int index)
{
    if (0 <= index && index < mLeftColorPoints.size()) {
        delete mLeftColorPoints[index];
        mLeftColorPoints.removeAt(index);
        orderLeftColorPoints();
    }
}

void Curve::removeRightColorPoint(int index)
{
    if (0 <= index && index < mLeftColorPoints.size()) {
        delete mLeftColorPoints[index];
        mLeftColorPoints.removeAt(index);
        orderLeftColorPoints();
    }
}

void Curve::removeColorPoint(ColorPoint *colorPoint)
{
    switch (colorPoint->type) {
    case ColorPoint::Left: {
        for (int i = 0; i < mLeftColorPoints.size(); ++i) {
            ColorPoint *point = mLeftColorPoints[i];

            if (point == mLeftColorPoints[i]) {
                removeLeftColorPoint(i);
                return;
            }
        }
        break;
    }
    case ColorPoint::Right: {
        for (int i = 0; i < mRightColorPoints.size(); ++i) {
            ColorPoint *point = mRightColorPoints[i];

            if (point == mRightColorPoints[i]) {
                removeRightColorPoint(i);
                return;
            }
        }
        break;
    }
    }
}

QVector4D Curve::leftColorAt(float t) const
{
    if (mLeftColorPoints.size() == 0 || mLeftColorPoints.size() == 1)
        return QVector4D(0, 0, 0, 0);

    for (int i = 1; i < mLeftColorPoints.size(); ++i) {
        ColorPoint *previousColorPoint = mLeftColorPoints[i - 1];
        ColorPoint *nextColorPoint = mLeftColorPoints[i];

        if (previousColorPoint->position <= t && t <= nextColorPoint->position) {
            float s = (t - previousColorPoint->position) / (nextColorPoint->position - previousColorPoint->position);
            return previousColorPoint->color + s * (nextColorPoint->color - previousColorPoint->color);
        }
    }

    return QVector4D(0, 0, 0, 0);
}

QVector4D Curve::rightColorAt(float t) const
{
    if (mRightColorPoints.size() == 0 || mRightColorPoints.size() == 1)
        return QVector4D(0, 0, 0, 0);

    for (int i = 1; i < mRightColorPoints.size(); ++i) {
        ColorPoint *previousColorPoint = mRightColorPoints[i - 1];
        ColorPoint *nextColorPoint = mRightColorPoints[i];

        if (previousColorPoint->position <= t && t <= nextColorPoint->position) {
            float s = (t - previousColorPoint->position) / (nextColorPoint->position - previousColorPoint->position);
            return previousColorPoint->color + s * (nextColorPoint->color - previousColorPoint->color);
        }
    }

    return QVector4D(0, 0, 0, 0);
}

void Curve::orderLeftColorPoints()
{
    if (mLeftColorPoints.size() == 0 || mLeftColorPoints.size() == 1)
        return;

    QVector<ColorPoint *> orderedColorPoints;

    orderedColorPoints << mLeftColorPoints[0];

    for (int i = 1; i < mLeftColorPoints.size(); ++i) {
        ColorPoint *currentColorPoint = mLeftColorPoints[i];

        if (orderedColorPoints.last()->position <= currentColorPoint->position)
            orderedColorPoints << currentColorPoint;
        else
            for (int j = 0; j < orderedColorPoints.size(); j++)
                if (currentColorPoint->position < orderedColorPoints[j]->position) {
                    orderedColorPoints.insert(j, currentColorPoint);
                    break;
                }
    }

    mLeftColorPoints = orderedColorPoints;
}

void Curve::orderRightColorPoints()
{
    if (mRightColorPoints.size() == 0 || mRightColorPoints.size() == 1)
        return;

    QVector<ColorPoint *> orderedColorPoints;

    orderedColorPoints << mRightColorPoints[0];

    for (int i = 1; i < mRightColorPoints.size(); ++i) {
        ColorPoint *currentColorPoint = mRightColorPoints[i];

        if (orderedColorPoints.last()->position <= currentColorPoint->position)
            orderedColorPoints << currentColorPoint;
        else
            for (int j = 0; j < orderedColorPoints.size(); j++)
                if (currentColorPoint->position < orderedColorPoints[j]->position) {
                    orderedColorPoints.insert(j, currentColorPoint);
                    break;
                }
    }

    mRightColorPoints = orderedColorPoints;
}

QVector<QVector4D> Curve::getLeftColors() const
{
    QVector<QVector4D> leftColors;
    for (int i = 0; i < mLeftColorPoints.size(); i++) {
        leftColors << mLeftColorPoints[i]->color;
    }

    return leftColors;
}

QVector<QVector4D> Curve::getRightColors() const
{
    QVector<QVector4D> rightColors;
    for (int i = 0; i < mRightColorPoints.size(); i++) {
        rightColors << mRightColorPoints[i]->color;
    }

    return rightColors;
}

QVector<float> Curve::getLeftColorPositions() const
{
    QVector<float> colorPointPositions;

    for (int i = 0; i < mLeftColorPoints.size(); i++) {
        colorPointPositions << mLeftColorPoints[i]->position;
    }

    return colorPointPositions;
}

QVector<float> Curve::getRightColorPositions() const
{
    QVector<float> colorPointPositions;

    for (int i = 0; i < mRightColorPoints.size(); i++) {
        colorPointPositions << mRightColorPoints[i]->position;
    }

    return colorPointPositions;
}

int Curve::getSize() const
{
    return mControlPoints.size();
}

const ControlPoint *Curve::getClosestControlPoint(QVector2D point) const
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

float Curve::distanceToPoint(const QVector2D &point, int intervals) const
{
    float minDistance = std::numeric_limits<float>::infinity();
    float t = 0.0f;
    float dt = 1.0f / intervals;

    for (int i = 0; i < intervals; ++i) {
        float distance = valueAt(t).distanceToPoint(point);
        if (distance < minDistance)
            minDistance = distance;

        t += dt;
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

void Curve::translate(QPointF translation)
{
    translate(QVector2D(translation.x(), translation.y()));
}

float Curve::length(int intervals) const
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

QRectF Curve::getBoundingBox(int intervals) const
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

const int &Curve::z() const
{
    return mZ;
}

void Curve::setZ(int newZ)
{
    mZ = newZ;
}

const QVector4D &Curve::contourColor() const
{
    return mContourColor;
}

void Curve::setContourColor(const QVector4D &newContourColor)
{
    mContourColor = newContourColor;
}

bool Curve::contourColorEnabled() const
{
    return mContourColorEnabled;
}

void Curve::setContourColorEnabled(bool newContourColorEnabled)
{
    mContourColorEnabled = newContourColorEnabled;
}

float Curve::contourThickness() const
{
    return mContourThickness;
}

void Curve::setContourThickness(float newContourThickness)
{
    mContourThickness = newContourThickness;
}

float Curve::diffusionWidth() const
{
    return mDiffusionWidth;
}

void Curve::setDiffusionWidth(float newDiffusionWidth)
{
    mDiffusionWidth = newDiffusionWidth;
}

void Curve::updateControlPointIndices()
{
    for (int i = 0; i < mControlPoints.size(); ++i) {
        mControlPoints[i]->index = i;
    }
}

QVector2D Curve::findMeanCenter(const QVector<QVector2D> &points)
{
    float x = 0;
    float y = 0;

    for (int i = 0; i < points.size(); ++i) {
        x += points[i].x();
        y += points[i].y();
    }
    return QVector2D(x, y) / points.size();
}

QVector<QVector2D> Curve::translate(const QVector<QVector2D> &points, const QVector2D &translation)
{
    QVector<QVector2D> result;
    result.reserve(points.size());

    for (int i = 0; i < points.size(); ++i) {
        result << (points[i] + translation);
    }

    return result;
}

void Curve::findLineOfBestFit(const QVector<QVector2D> &points, QVector2D &startingPoint, QVector2D &direction, int segments)
{
    // Find mean center of the points
    QVector2D meanCenter = findMeanCenter(points);

    // Move mean center to the origin
    QVector<QVector2D> translatedPoints = translate(points, -meanCenter);

    // Set variables
    QVector2D origin(0, 0);
    float theta = 0;
    float dt = M_PI / segments;
    float minimumAverageDistance = averageDistanceToLine(translatedPoints, origin, QVector2D(1, 0));
    float beta = 0;

    for (int i = 0; i < segments; ++i) {
        theta += dt;
        float averageDistance = averageDistanceToLine(translatedPoints, origin, QVector2D(cos(theta), sin(theta)));
        if (averageDistance < minimumAverageDistance) {
            minimumAverageDistance = averageDistance;
            beta = theta;
        }
    }

    startingPoint = meanCenter;
    direction = QVector2D(cos(beta), sin(beta));
}

float Curve::averageDistanceToLine(const QVector<QVector2D> &points, const QVector2D &startingPoint, const QVector2D &direction)
{
    float distance = 0;

    for (int i = 0; i < points.size(); ++i) {
        distance += points[i].distanceToLine(startingPoint, direction);
    }

    return distance / points.size();
}

float Curve::perpendicularAt(const QVector2D &startingPoint, const QVector2D &direction, const QVector2D &subject)
{
    const float &sx = startingPoint.x();
    const float &sy = startingPoint.y();

    const float &dx = direction.x();
    const float &dy = direction.y();

    const float &cx = subject.x();
    const float &cy = subject.y();

    return (cy * dy + cx * dx - sx * dx - sy * dy) / (dx * dx + dy * dy);
}

float Curve::parameterAt(const QVector2D &point, int intervals) const
{
    float t = 0;
    float s = 0;
    float dt = 1.0f / intervals;
    float minimumDistance = point.distanceToPoint(valueAt(0));

    for (int i = 0; i < intervals; i++) {
        t += dt;
        float distance = point.distanceToPoint(valueAt(t));
        if (distance < minimumDistance) {
            minimumDistance = distance;
            s = t;
        }
    }

    return s;
}
