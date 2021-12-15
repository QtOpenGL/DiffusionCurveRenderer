#include "RenderSettingsWidget.h"

#include <Util.h>
#include <QColorDialog>
#include <QGridLayout>
#include <QLabel>

RenderSettingsWidget::RenderSettingsWidget(QWidget *parent)
    : QGroupBox(parent)
    , mMode(RenderMode::Diffuse)

{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(8);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);

    {
        mRenderContoursCheckBox = new QCheckBox("Render Contours");
        mRenderContoursCheckBox->setChecked(false);
        mainLayout->addWidget(mRenderContoursCheckBox, 1, 0);

        connect(mRenderContoursCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
            mMode = mMode + RenderMode::Contours;
            emit renderModeChanged(mMode);
        });
    }

    {
        mDiffuseCheckBox = new QCheckBox("Diffuse Colors");
        mDiffuseCheckBox->setChecked(true);
        mainLayout->addWidget(mDiffuseCheckBox, 2, 0);

        connect(mDiffuseCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
            mMode = mMode + RenderMode::Diffuse;
            emit renderModeChanged(mMode);
        });
    }

    // Smooth Iterations Slider
    {
        mSmoothIterationsSlider = new QSlider(Qt::Horizontal);
        mSmoothIterationsSlider->setMinimum(1);
        mSmoothIterationsSlider->setMaximum(200);
        mSmoothIterationsSlider->setValue(40);
        mSmoothIterationsSlider->setInvertedAppearance(false);
        mSmoothIterationsSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Smooth Iterations"), 3, 0);
        mainLayout->addWidget(mSmoothIterationsSlider, 3, 1);

        connect(mSmoothIterationsSlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateSmoothIterations, value); });
    }

    // Diffusion Width Slider
    {
        mDiffusionWidthSlider = new QSlider(Qt::Horizontal);
        mDiffusionWidthSlider->setMinimum(100);
        mDiffusionWidthSlider->setMaximum(1600);
        mDiffusionWidthSlider->setTickInterval(75);
        mDiffusionWidthSlider->setValue(400);
        mDiffusionWidthSlider->setInvertedAppearance(false);
        mDiffusionWidthSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Diffusion Width"), 4, 0);
        mainLayout->addWidget(mDiffusionWidthSlider, 4, 1);

        connect(mDiffusionWidthSlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateDiffusionWidth, value / 100.0f); });
    }

    // Quality Slider
    {
        mQualitySlider = new QSlider(Qt::Horizontal);
        mQualitySlider->setMinimum(1);
        mQualitySlider->setMaximum(4);
        mQualitySlider->setTickInterval(1);
        mQualitySlider->setValue(1);
        mQualitySlider->setInvertedAppearance(false);
        mQualitySlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Render Quality"), 5, 0);
        mainLayout->addWidget(mQualitySlider, 5, 1);

        connect(mQualitySlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateRenderQuality, value); });
    }

    // Contour Thickness Slider
    {
        mContourThicknessSlider = new QSlider(Qt::Horizontal);
        mContourThicknessSlider->setMinimum(1);
        mContourThicknessSlider->setMaximum(21);
        mContourThicknessSlider->setTickInterval(2);
        mContourThicknessSlider->setValue(6);
        mContourThicknessSlider->setInvertedAppearance(false);
        mContourThicknessSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Contour Thickness"), 6, 0);
        mainLayout->addWidget(mContourThicknessSlider, 6, 1);

        connect(mContourThicknessSlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateContourThickness, value); });
    }

    // Contour Color Buton
    {
        mColorButton = new CustomFlatButton;
        mColorButton->setColor(QColor(0, 0, 0));
        connect(mColorButton, &QPushButton::clicked, this, [=]() {
            QColor initialColor(0, 0, 0, 1);
            QColor selectedColor = QColorDialog::getColor(initialColor, this);
            if (selectedColor.isValid()) {
                QVector4D newColor = Util::convertColorToVector4D(selectedColor);
                mColorButton->setColor(selectedColor);
                emit action(Action::UpdateContourColor, newColor);
            }
        });

        mainLayout->addWidget(new QLabel("Contour Color"), 7, 0);
        mainLayout->addWidget(mColorButton, 7, 1);
    }

    setTitle("Render Settings");
    setLayout(mainLayout);
    setEnabled(true);

    update();
}
