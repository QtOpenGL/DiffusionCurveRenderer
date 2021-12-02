#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include <QObject>
#include <QVector2D>

struct ControlPoint
{
public:
    ControlPoint();
    ControlPoint(float x, float y);
    ControlPoint(QPointF point);

    QVector2D position;
    bool selected;
    int index;
};

#endif // CONTROLPOINT_H
