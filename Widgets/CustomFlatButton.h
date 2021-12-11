#ifndef CUSTOMFLATBUTTON_H
#define CUSTOMFLATBUTTON_H

#include <QPushButton>
#include <QVector4D>

class CustomFlatButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CustomFlatButton(QWidget *parent = nullptr);
    void setColor(const QColor &newColor);
};

#endif // CUSTOMFLATBUTTON_H
