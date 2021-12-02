#include "Util.h"
#include <Curves/Bezier.h>
#include <QDebug>
#include <QDomDocument>
#include <QFile>

Util::Util() {}

QVector<Curve *> Util::readCurveDataFromXML(QString filename)
{
    QVector<Curve *> curves;
    QDomDocument document;

    // Read the file
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            qCritical() << "Error while loading file.";
            return curves;
        }

        document.setContent(&file);
        file.close();
    }

    QDomElement root = document.documentElement();
    QDomElement component = root.firstChild().toElement();

    while (!component.isNull()) {
        Curve *curve = new Bezier;
        if (component.tagName() == "curve") {
            QDomElement child = component.firstChild().toElement();
            while (!child.isNull()) {
                if (child.tagName() == "control_points_set") {
                    QDomElement element = child.firstChild().toElement();
                    while (!element.isNull()) {
                        float x = element.attribute("y").toDouble();
                        float y = element.attribute("x").toDouble();
                        ControlPoint *controlPoint = new ControlPoint(x, y);
                        curve->addControlPoint(controlPoint);
                        element = element.nextSibling().toElement();
                    }
                }
                child = child.nextSibling().toElement();
            }
        }
        curves << curve;
        component = component.nextSibling().toElement();
    }

    return curves;
}
