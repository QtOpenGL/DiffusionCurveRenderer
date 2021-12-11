#ifndef UTIL_H
#define UTIL_H

#include <Curves/Curve.h>

class Util
{
public:
    Util();

    static QVector<Curve *> readCurveDataFromXML(QString filename);
    static QColor convertVector4DtoColor(const QVector4D &color);
    static QVector4D convertColorToVector4D(const QColor &color);
};

#endif // UTIL_H
