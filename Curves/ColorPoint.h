#ifndef COLORPOINT_H
#define COLORPOINT_H

#include <QVector4D>

struct ColorPoint
{
public:
    ColorPoint();
    ColorPoint(float position, QVector4D color);
    enum Type { Left = 0, Right = 1 };

    QVector4D color;
    float position;
    bool selected;
    int index;
    Type type;

    QString toString();
};

#endif // COLORPOINT_H
