#ifndef CUSTOMFLATBUTTON_H
#define CUSTOMFLATBUTTON_H

#include <QPushButton>
#include <QVector4D>

class CustomFlatButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CustomFlatButton(QWidget *parent = nullptr);

    const QVector4D &color() const;
    void setColor(const QVector4D &newColor);

    static QVector4D getVector4DColor(const QColor &color);
    static QColor getQColor(const QVector4D &color);

private:
    QVector4D mColor;
};

#endif // CUSTOMFLATBUTTON_H
