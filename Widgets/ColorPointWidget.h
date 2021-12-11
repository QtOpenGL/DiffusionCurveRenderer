#ifndef COLORPOINTWIDGET_H
#define COLORPOINTWIDGET_H

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>

class ColorPointWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ColorPointWidget(QWidget *parent = nullptr);

private:
    QLabel *mTypeLabel;
    QSlider *mPositionSlider;
    QPushButton *mColorButton;
};

#endif // COLORPOINTWIDGET_H
