#include "ControlPoint.h"

ControlPoint::ControlPoint()
    : ControlPoint(0, 0)
{}

ControlPoint::ControlPoint(QPointF point)
    : ControlPoint(point.x(), point.y())
{}

ControlPoint::ControlPoint(QVector2D point)
    : ControlPoint(point.x(), point.y())
{}

ControlPoint::ControlPoint(float x, float y)
    : position(x, y)
    , selected(false)
    , index(-1)
{}
