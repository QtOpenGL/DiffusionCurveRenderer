#include "CurveWidget.h"

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
        connect(mZLineEdit, &QLineEdit::textChanged, this, [=](QString text) {
            if (mSelectedCurve)
                emit action(Action::UpdateCurveZIndex, text.toInt());
        });

        mainLayout->addWidget(new QLabel("Z-index"), 0, 0);
        mainLayout->addWidget(mZLineEdit, 0, 2);
    }

    {
        mContourColorStateCheckBox = new QCheckBox;
        mContourColorStateCheckBox->setCheckable(true);

        mColorButton = new QPushButton;

        mainLayout->addWidget(new QLabel("Contour Color"), 1, 0);
        mainLayout->addWidget(mContourColorStateCheckBox, 1, 1);
        mainLayout->addWidget(mColorButton, 1, 2);
    }

    // Contour Thickness
    {
        mContourThicknessSlider = new QSlider(Qt::Horizontal);
        mContourThicknessSlider->setMinimum(1);
        mContourThicknessSlider->setMaximum(99);
        mContourThicknessSlider->setValue(4);
        mContourThicknessSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Contour Thickness"), 2, 0);
        mainLayout->addWidget(mContourThicknessSlider, 2, 2);
    }

    // Diffusion Width
    {
        mDiffusionWidthSlider = new QSlider(Qt::Horizontal);
        mDiffusionWidthSlider->setMinimum(1);
        mDiffusionWidthSlider->setMaximum(99);
        mDiffusionWidthSlider->setValue(4);
        mDiffusionWidthSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Diffusion Width"), 3, 0);
        mainLayout->addWidget(mDiffusionWidthSlider, 3, 2);
    }

    // Remove Button
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
    if (mSelectedCurve == selectedCurve)
        return;

    mSelectedCurve = selectedCurve;

    if (mSelectedCurve) {
        setEnabled(true);
        mZLineEdit->setText(QString::number(mSelectedCurve->z()));

    } else {
        reset();
    }
}

void CurveWidget::reset()
{
    mZLineEdit->clear();

    setEnabled(false);
}
