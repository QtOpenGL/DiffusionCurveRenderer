#ifndef COLORPOINT_H
#define COLORPOINT_H

#include <QVector4D>

struct ColorPoint
{
public:
    ColorPoint();
    ColorPoint(float position, QVector4D color);

    QVector4D color;
    bool enabled;
    float position;
    bool selected;
    int index;

    QString toString();
};

#endif // COLORPOINT_H
