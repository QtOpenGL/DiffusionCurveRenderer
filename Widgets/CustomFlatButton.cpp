#include "CustomFlatButton.h"

CustomFlatButton::CustomFlatButton(QWidget *parent)
    : QPushButton(parent)
{}

void CustomFlatButton::setColor(const QColor &newColor)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Button, newColor);
    setAutoFillBackground(true);
    setFlat(true);
    setPalette(palette);
    update();
}
