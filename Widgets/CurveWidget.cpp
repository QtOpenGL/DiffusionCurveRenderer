#include "CurveWidget.h"

#include <Util.h>
#include <QColorDialog>
#include <QSizePolicy>
#include <QVBoxLayout>

CurveWidget::CurveWidget(QGroupBox *parent)
    : QGroupBox(parent)
    , mSelectedCurve(nullptr)
{
    // 0, 1, 2
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(8);

    // z-index
    {
        mZLineEdit = new QLineEdit;
        mZLineEdit->setReadOnly(false);
        mZLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        mZLineEdit->setValidator(new QIntValidator(-1000, 1000, this));
        connect(mZLineEdit, &QLineEdit::textChanged, this, [=](QString text) { emit action(Action::UpdateCurveZIndex, text.toInt()); });

        mainLayout->addWidget(new QLabel("Z-index"), 0, 0);
        mainLayout->addWidget(mZLineEdit, 0, 2);
    }

    // Color
    {
        mContourColorStateCheckBox = new QCheckBox;
        mContourColorStateCheckBox->setCheckable(true);
        connect(mContourColorStateCheckBox, &QCheckBox::stateChanged, this, [=](int state) { emit action(Action::EnableContourColor, state == 2); });

        mColorButton = new CustomFlatButton;
        connect(mColorButton, &QPushButton::clicked, this, [=]() {
            if (mSelectedCurve && mSelectedCurve->contourColorEnabled()) {
                QColor initialColor = Util::convertVector4DtoColor(mSelectedCurve->contourColor());
                QColor selectedColor = QColorDialog::getColor(initialColor, this);
                if (selectedColor.isValid()) {
                    QVector4D newColor = Util::convertColorToVector4D(selectedColor);
                    emit action(Action::UpdateContourColor, newColor);
                }
            }
        });

        mainLayout->addWidget(new QLabel("Contour Color"), 1, 0);
        mainLayout->addWidget(mContourColorStateCheckBox, 1, 1);
        mainLayout->addWidget(mColorButton, 1, 2);
    }

    // Contour Thickness
    {
        mContourThicknessSlider = new QSlider(Qt::Horizontal);
        mContourThicknessSlider->setMinimum(10);
        mContourThicknessSlider->setMaximum(300);
        mContourThicknessSlider->setTickPosition(QSlider::TicksBelow);
        mContourThicknessSlider->setTickInterval(58);

        connect(mContourThicknessSlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateContourThickness, value / 10.0f); });

        mainLayout->addWidget(new QLabel("Contour Thickness"), 2, 0);
        mainLayout->addWidget(mContourThicknessSlider, 2, 2);
    }

    // Diffusion Width
    {
        mDiffusionWidthSlider = new QSlider(Qt::Horizontal);
        mDiffusionWidthSlider->setMinimum(10);
        mDiffusionWidthSlider->setMaximum(300);
        mDiffusionWidthSlider->setTickPosition(QSlider::TicksBelow);
        mDiffusionWidthSlider->setTickInterval(58);

        connect(mDiffusionWidthSlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateDiffusionWidth, value / 10.0f); });

        mainLayout->addWidget(new QLabel("Diffusion Width"), 3, 0);
        mainLayout->addWidget(mDiffusionWidthSlider, 3, 2);
    }

    // Remove Curve Button
    {
        mRemoveButton = new QPushButton("Remove Curve");
        mRemoveButton->setFlat(false);
        mRemoveButton->setAutoFillBackground(false);

        connect(mRemoveButton, &QPushButton::clicked, this, [=]() { emit action(Action::RemoveCurve); });

        mainLayout->addWidget(mRemoveButton, 4, 2);
    }

    setLayout(mainLayout);
    setTitle("Curve");
    reset();
}

void CurveWidget::onSelectedCurveChanged(Curve *selectedCurve)
{
    mSelectedCurve = selectedCurve;
}

void CurveWidget::reset()
{
    mZLineEdit->clear();
    mContourColorStateCheckBox->setChecked(false);
    mColorButton->setColor(QColor(0, 0, 0, 0));
    setEnabled(false);
}

void CurveWidget::refresh()
{
    if (mSelectedCurve) {
        setEnabled(true);
        mZLineEdit->setText(QString::number(mSelectedCurve->z()));
        mContourColorStateCheckBox->setChecked(mSelectedCurve->contourColorEnabled());
        mContourThicknessSlider->setSliderPosition(10 * mSelectedCurve->contourThickness());
        mDiffusionWidthSlider->setSliderPosition(10 * mSelectedCurve->diffusionWidth());
        if (mSelectedCurve->contourColorEnabled()) {
            mColorButton->setColor(Util::convertVector4DtoColor(mSelectedCurve->contourColor()));
        } else {
            mColorButton->setColor(QColor(0, 0, 0, 0));
        }
    } else {
        reset();
    }
}
