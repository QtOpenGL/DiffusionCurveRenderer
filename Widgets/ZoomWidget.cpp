#include "ZoomWidget.h"

#include <QLayout>

ZoomWidget::ZoomWidget(QGroupBox *parent) : QGroupBox(parent) ,
    mZoomRatio(1.0)
{
    mSlider = new QSlider(Qt::Horizontal);
    mSlider->setMinimum(8);
    mSlider->setMaximum(256);
    mSlider->setValue(16);
    mSlider->setInvertedAppearance(false);
    mSlider->setTickPosition(QSlider::TicksBelow);

    connect(mSlider, &QSlider::valueChanged, this, [=](int value){
       mZoomRatio = 16.0f / static_cast<float>(value);
       emit zoomRatioChanged(mZoomRatio);
    });

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(mSlider);
    setLayout(layout);
    setTitle("Zoom");
}

void ZoomWidget::onZoomRatioChanged(float zoomRatio)
{
    if(qFuzzyCompare(mZoomRatio, zoomRatio))
        return;

    mZoomRatio = zoomRatio;
    mSlider->setValue(16.0f / mZoomRatio);
}
