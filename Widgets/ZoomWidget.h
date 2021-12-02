#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QGroupBox>
#include <QSlider>

class ZoomWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ZoomWidget(QGroupBox *parent = nullptr);

public slots:
    void onZoomRatioChanged(float zoomRatio);

signals:
    void zoomRatioChanged(float zoomRatio);

private:
    QSlider *mSlider;
    float mZoomRatio;
};

#endif // ZOOMWIDGET_H
