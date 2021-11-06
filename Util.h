#ifndef UTIL_H
#define UTIL_H

#include <Curves/Curve.h>

class Util
{
public:
    Util();

    static QVector<Curve*> readCurveDataFromXML(QString filename);
};

#endif // UTIL_H
