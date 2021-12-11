#include "CustomFlatButton.h"

CustomFlatButton::CustomFlatButton(QWidget *parent)
    : QPushButton(parent)
{
    setAutoFillBackground(true);
    setFlat(true);
    setColor(QVector4D(0, 0, 0, 0));
}

const QVector4D &CustomFlatButton::color() const
{
    return mColor;
}

void CustomFlatButton::setColor(const QVector4D &newColor)
{
    mColor = newColor;

    QPalette palette = this->palette();
    palette.setColor(QPalette::Button, getQColor(newColor));
    setAutoFillBackground(true);
    setFlat(true);
    setPalette(palette);
    update();
}

QVector4D CustomFlatButton::getVector4DColor(const QColor &color)
{
    return QVector4D(color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f, color.alpha() / 255.0f);
}

QColor CustomFlatButton::getQColor(const QVector4D &color)
{
    return QColor(255 * color.x(), 255 * color.y(), 255 * color.z(), 255 * color.w());
}
