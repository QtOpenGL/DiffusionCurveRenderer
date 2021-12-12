#include "ColorPointWidget.h"

#include <Util.h>
#include <QColorDialog>
#include <QGridLayout>

ColorPointWidget::ColorPointWidget(QWidget *parent)
    : QGroupBox(parent)
    , mSelectedCurve(nullptr)
    , mSelectedColorPoint(nullptr)

{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(8);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);

    // Type
    {
        mTypeLabel = new QLabel;

        mainLayout->addWidget(new QLabel("Type"), 0, 0);
        mainLayout->addWidget(mTypeLabel, 0, 1);
    }

    // Y
    {
        mPositionSlider = new QSlider(Qt::Horizontal);
        mPositionSlider->setMinimum(0);
        mPositionSlider->setMaximum(100);
        mPositionSlider->setValue(0);
        mPositionSlider->setTickInterval(20);
        mPositionSlider->setInvertedAppearance(false);
        mPositionSlider->setTickPosition(QSlider::TicksBelow);

        connect(mPositionSlider, &QSlider::valueChanged, this, [=](int value) { emit action(Action::UpdateColorPointPosition, value / 100.0f); });

        mainLayout->addWidget(new QLabel("Position"), 1, 0);
        mainLayout->addWidget(mPositionSlider, 1, 1);
    }

    // Color Button
    {
        mColorButton = new CustomFlatButton;
        mColorButton->setFlat(true);
        mColorButton->setAutoFillBackground(true);
        mColorButton->setColor(QColor(0, 0, 0, 0));

        connect(mColorButton, &QPushButton::clicked, this, [=]() {
            if (mSelectedColorPoint) {
                QColor initialColor = Util::convertVector4DtoColor(mSelectedColorPoint->color());
                QColor selectedColor = QColorDialog::getColor(initialColor, this);
                if (selectedColor.isValid()) {
                    QVector4D newColor = Util::convertColorToVector4D(selectedColor);
                    emit action(Action::UpdateColorPointColor, newColor);
                }
            }
        });

        mainLayout->addWidget(new QLabel("Color"), 2, 0);
        mainLayout->addWidget(mColorButton, 2, 1);
    }

    // Remove Button
    {
        mRemoveButton = new QPushButton("Remove Color Point");
        mRemoveButton->setFlat(false);
        mRemoveButton->setAutoFillBackground(false);
        connect(mRemoveButton, &QPushButton::clicked, this, [=]() { emit action(Action::RemoveColorPoint); });

        mainLayout->addWidget(mRemoveButton, 3, 1);
    }

    setTitle("Color Point");
    setLayout(mainLayout);
    setEnabled(false);
}

void ColorPointWidget::onSelectedColorPointChanged(ColorPoint *selectedColorPoint)
{
    if (mSelectedColorPoint == selectedColorPoint)
        return;

    mSelectedColorPoint = selectedColorPoint;

    if (mSelectedColorPoint) {
        setEnabled(true);
        mColorButton->setColor(Util::convertVector4DtoColor(mSelectedColorPoint->color()));
        mTypeLabel->setText(mSelectedColorPoint->type() == ColorPoint::Left ? "Left" : "Right");
        mPositionSlider->setSliderPosition(100 * mSelectedColorPoint->position());
    } else {
        mTypeLabel->clear();
        mColorButton->setColor(QColor(0, 0, 0, 0));
        setEnabled(false);
    }
}

void ColorPointWidget::onSelectedCurveChanged(Curve *selectedCurve)
{
    if (mSelectedCurve == selectedCurve)
        return;

    mSelectedCurve = selectedCurve;
    mTypeLabel->clear();
    mColorButton->setColor(QColor(0, 0, 0, 0));
    setEnabled(false);
}

void ColorPointWidget::onDirty(DirtType type)
{
    if (type & DirtType::GUI) {
        if (mSelectedColorPoint) {
            mColorButton->setColor(Util::convertVector4DtoColor(mSelectedColorPoint->color()));
            mTypeLabel->setText(mSelectedColorPoint->type() == ColorPoint::Left ? "Left" : "Right");
            mPositionSlider->setSliderPosition(100 * mSelectedColorPoint->position());
        }
    }
}
