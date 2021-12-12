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
            qCritical() << "Error occured while loading the file:" << filename;
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
                        ControlPoint *controlPoint = new ControlPoint(QVector2D(x, y));
                        curve->addControlPoint(controlPoint);
                        element = element.nextSibling().toElement();
                    }
                } else if (child.tagName() == "left_colors_set" || child.tagName() == "right_colors_set") {
                    QDomElement element = child.firstChild().toElement();
                    int maxGlobalID = 0;

                    QVector<ColorPoint *> colorPoints;

                    while (!element.isNull()) {
                        uint8_t r = element.attribute("B").toUInt();
                        uint8_t g = element.attribute("G").toUInt();
                        uint8_t b = element.attribute("R").toUInt();
                        int globalID = element.attribute("globalID").toInt();

                        ColorPoint *colorPoint = new ColorPoint();
                        colorPoint->setParent(curve);
                        colorPoint->setColor(QVector4D(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f));
                        colorPoint->setType(child.tagName() == "left_colors_set" ? ColorPoint::Left : ColorPoint::Right);
                        colorPoint->setPosition(globalID);
                        colorPoints << colorPoint;

                        if (globalID >= maxGlobalID)
                            maxGlobalID = globalID;

                        element = element.nextSibling().toElement();
                    }

                    for (int i = 0; i < colorPoints.size(); ++i) {
                        colorPoints[i]->setPosition(colorPoints[i]->position() / maxGlobalID);
                        curve->addColorPoint(colorPoints[i]);
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

QColor Util::convertVector4DtoColor(const QVector4D &color)
{
    return QColor(255 * color.x(), 255 * color.y(), 255 * color.z(), 255 * color.w());
}

QVector4D Util::convertColorToVector4D(const QColor &color)
{
    return QVector4D(color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f, color.alpha() / 255.0f);
}
