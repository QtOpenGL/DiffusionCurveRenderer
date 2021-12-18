#include "RenderSettingsWidget.h"

#include <Util.h>
#include <QColorDialog>
#include <QGridLayout>
#include <QLabel>

RenderSettingsWidget::RenderSettingsWidget(QWidget *parent)
    : QGroupBox(parent)
    , mMode(RenderMode::Diffuse)
    , mColorRendererMode(ColorRendererMode::TriangleStrip)

{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(0);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(2, 1);
    mainLayout->setRowMinimumHeight(0, 30);
    mainLayout->setRowMinimumHeight(1, 40);
    mainLayout->setRowMinimumHeight(2, 40);
    mainLayout->setRowMinimumHeight(3, 40);
    mainLayout->setRowMinimumHeight(4, 40);
    mainLayout->setRowMinimumHeight(5, 40);

    {
        mRenderContoursCheckBox = new QCheckBox("Contours");
        mRenderContoursCheckBox->setChecked(false);

        connect(mRenderContoursCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
            mMode = mMode + RenderMode::Contours;
            emit renderModeChanged(mMode);
        });

        mDiffuseCheckBox = new QCheckBox("Diffusion");
        mDiffuseCheckBox->setChecked(true);

        connect(mDiffuseCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
            mMode = mMode + RenderMode::Diffuse;
            emit renderModeChanged(mMode);
        });

        mainLayout->addWidget(new QLabel("Mode"), 0, 0);
        mainLayout->addWidget(mRenderContoursCheckBox, 0, 1);
        mainLayout->addWidget(mDiffuseCheckBox, 0, 2);
    }

    // Radio buttons
    {
        // Line strip
        mLineStripRadioButton = new QRadioButton("Line Strip");
        mLineStripRadioButton->setCheckable(true);

        connect(mLineStripRadioButton, &QRadioButton::clicked, this, [=]() {
            mColorRendererMode = ColorRendererMode::LineStrip;
            mTriangleStripRadioButton->setChecked(false);

            emit colorRendererModeChanged(ColorRendererMode::LineStrip);
        });

        // Triangle strip
        mTriangleStripRadioButton = new QRadioButton("Triangle Strip");
        mTriangleStripRadioButton->setCheckable(true);
        mTriangleStripRadioButton->setChecked(true);

        connect(mTriangleStripRadioButton, &QRadioButton::clicked, this, [=]() {
            mColorRendererMode = ColorRendererMode::LineStrip;
            mLineStripRadioButton->setChecked(false);

            emit colorRendererModeChanged(ColorRendererMode::TriangleStrip);
        });

        mainLayout->addWidget(new QLabel("Primitive"), 1, 0);
        mainLayout->addWidget(mLineStripRadioButton, 1, 1);
        mainLayout->addWidget(mTriangleStripRadioButton, 1, 2);
    }

    // Quality Slider
    {
        mQualitySlider = new QSlider(Qt::Horizontal);
        mQualitySlider->setMinimum(0);
        mQualitySlider->setMaximum(2);
        mQualitySlider->setTickInterval(1);
        mQualitySlider->setPageStep(1);
        mQualitySlider->setSingleStep(1);
        mQualitySlider->setValue(0);
        mQualitySlider->setInvertedAppearance(false);
        mQualitySlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Render Quality"), 2, 0, 1, 1);
        mainLayout->addWidget(mQualitySlider, 2, 1, 1, 2);

        connect(mQualitySlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateRenderQuality, pow(2, value)); });
    }

    // Smooth Iterations Slider
    {
        mSmoothIterationsSlider = new QSlider(Qt::Horizontal);
        mSmoothIterationsSlider->setMinimum(0);
        mSmoothIterationsSlider->setMaximum(100);
        mSmoothIterationsSlider->setTickInterval(20);
        mSmoothIterationsSlider->setValue(40);
        mSmoothIterationsSlider->setInvertedAppearance(false);
        mSmoothIterationsSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Smooth Iterations"), 3, 0, 1, 1);
        mainLayout->addWidget(mSmoothIterationsSlider, 3, 1, 1, 2);

        connect(mSmoothIterationsSlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateSmoothIterations, value); });
    }

    // Diffusion Width Slider
    {
        mDiffusionWidthSlider = new QSlider(Qt::Horizontal);
        mDiffusionWidthSlider->setMinimum(100);
        mDiffusionWidthSlider->setMaximum(600);
        mDiffusionWidthSlider->setTickInterval(50);
        mDiffusionWidthSlider->setValue(200);
        mDiffusionWidthSlider->setInvertedAppearance(false);
        mDiffusionWidthSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Diffusion Width"), 4, 0, 1, 1);
        mainLayout->addWidget(mDiffusionWidthSlider, 4, 1, 1, 2);

        connect(mDiffusionWidthSlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateDiffusionWidth, value / 100.0f); });
    }

    // Contour Thickness Slider
    {
        mContourThicknessSlider = new QSlider(Qt::Horizontal);
        mContourThicknessSlider->setMinimum(100);
        mContourThicknessSlider->setMaximum(1200);
        mContourThicknessSlider->setTickInterval(100);
        mContourThicknessSlider->setValue(100);
        mContourThicknessSlider->setInvertedAppearance(false);
        mContourThicknessSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Contour Thickness"), 5, 0, 1, 1);
        mainLayout->addWidget(mContourThicknessSlider, 5, 1, 1, 2);

        connect(mContourThicknessSlider, &QSlider::valueChanged, this, [=](int value) {
            emit action(Action::UpdateContourThickness, value / 100.0f);
        });
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

        //        mainLayout->addWidget(new QLabel("Contour Color"), 6, 0, 1, 1);
        //        mainLayout->addWidget(mColorButton, 6, 1, 1, 2);
    }

    setTitle("Render Settings");
    setLayout(mainLayout);
    setEnabled(true);

    update();
}
