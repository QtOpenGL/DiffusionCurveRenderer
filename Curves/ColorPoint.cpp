#include "ColorPoint.h"
#include "Curve.h"

ColorPoint::ColorPoint(Curve *parentCurve)
    : mParentCurve(parentCurve)
{}

ColorPoint::ColorPoint(Curve *parentCurve, float position, QVector4D color, Type type)
    : mParentCurve(parentCurve)
    , mColor(color)
    , mPosition(position)
    , mType(type)
{}

QVector2D ColorPoint::getPosition2D(float gap) const
{
    QVector2D positionOnCurve = mParentCurve->valueAt(mPosition);
    QVector2D normal = mParentCurve->normalAt(mPosition);

    QVector2D gapVector = (mParentCurve->contourThickness() + gap) * normal;

    if (mType == ColorPoint::Right)
        gapVector = -gapVector;

    return positionOnCurve + gapVector;
}

float ColorPoint::position() const
{
    return mPosition;
}

void ColorPoint::setPosition(float newPosition)
{
    mPosition = newPosition;
}

bool ColorPoint::selected() const
{
    return mSelected;
}

void ColorPoint::setSelected(bool newSelected)
{
    mSelected = newSelected;
}

ColorPoint::Type ColorPoint::type() const
{
    return mType;
}

void ColorPoint::setType(Type newType)
{
    mType = newType;
}

const QVector4D &ColorPoint::color() const
{
    return mColor;
}

void ColorPoint::setColor(const QVector4D &newColor)
{
    mColor = newColor;
}
