#include "RenderSettingsWidget.h"

#include <QGridLayout>
#include <QLabel>

RenderSettingsWidget::RenderSettingsWidget(QWidget *parent)
    : QGroupBox{parent}
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(8);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);

    {
        mRenderContoursCheckBox = new QCheckBox("Render Contours");
        mainLayout->addWidget(mRenderContoursCheckBox, 1, 0);
    }

    {
        mDiffuseCheckBox = new QCheckBox("Diffuse Colors");
        mainLayout->addWidget(mDiffuseCheckBox, 2, 0);
    }

    {
        mSmoothIterationsSlider = new QSlider(Qt::Horizontal);
        mSmoothIterationsSlider->setMinimum(1);
        mSmoothIterationsSlider->setMaximum(100);
        mSmoothIterationsSlider->setValue(10);
        mSmoothIterationsSlider->setInvertedAppearance(false);
        mSmoothIterationsSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Smooth Iterations"), 3, 0);
        mainLayout->addWidget(mSmoothIterationsSlider, 3, 1);
    }

    {
        mQualitySlider = new QSlider(Qt::Horizontal);
        mQualitySlider->setMinimum(1);
        mQualitySlider->setMaximum(4);
        mQualitySlider->setTickInterval(1);
        mQualitySlider->setValue(2);
        mQualitySlider->setInvertedAppearance(false);
        mQualitySlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Quality"), 4, 0);
        mainLayout->addWidget(mQualitySlider, 4, 1);
    }

    setTitle("Render Settings");
    setLayout(mainLayout);
    setEnabled(true);
}
