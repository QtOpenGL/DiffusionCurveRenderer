#ifndef COLORPOINT_H
#define COLORPOINT_H

#include <QColor>
#include <QObject>
#include <QVector4D>

class Curve;
class ColorPoint : public QObject
{
public:
    enum Type { Left = 0, Right = 1 };

    ColorPoint(Curve *parentCurve);
    ColorPoint(Curve *parentCurve, float position, QVector4D color, Type type);

    QVector2D getPosition2D(float gap = 10) const;

    const QVector4D &color() const;
    void setColor(const QVector4D &newColor);

    float position() const;
    void setPosition(float newPosition);

    bool selected() const;
    void setSelected(bool newSelected);

    Type type() const;
    void setType(Type newType);

private:
    Curve *mParentCurve;
    QVector4D mColor;
    float mPosition;
    bool mSelected;
    Type mType;
};

#endif // COLORPOINT_H
