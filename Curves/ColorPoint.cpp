#include "ColorPoint.h"

ColorPoint::ColorPoint() :
    color(0,0,0,0),
    enabled(false),
    position(0.0f),
    selected(false),
    index(-1)
{

}

ColorPoint::ColorPoint(float position, QVector4D color) :
    color(color),
    enabled(true),
    position(position),
    selected(false),
    index(-1)
{

}

QString ColorPoint::toString()
{
    return QString("Position: %1").arg(position);
}
