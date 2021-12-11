#include "CurveWidget.h"

#include <QColorDialog>
#include <QSizePolicy>
#include <QVBoxLayout>

CurveWidget::CurveWidget(QGroupBox *parent)
    : QGroupBox(parent)
    , mSelectedCurve(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    // z-index
    {
        QHBoxLayout *layout = new QHBoxLayout;
        QLabel *label = new QLabel("Z Index");
        layout->addWidget(label);

        mZLineEdit = new QLineEdit;
        mZLineEdit->setReadOnly(false);
        mZLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        mZLineEdit->setValidator(new QIntValidator(-1000, 1000, this));
        connect(mZLineEdit, &QLineEdit::textChanged, this, [=](QString text) {
            if (mSelectedCurve)
                emit action(Action::UpdateCurveZIndex, text.toInt());
        });

        layout->addWidget(mZLineEdit);
        mainLayout->addLayout(layout);
    }

    // Thickness
    {
        QHBoxLayout *layout = new QHBoxLayout;
        QLabel *label = new QLabel("Thickness");
        layout->addWidget(label);

        mThicknessLineEdit = new QLineEdit;
        mThicknessLineEdit->setReadOnly(false);
        mThicknessLineEdit->setValidator(new QIntValidator(1, 10, this));
        mThicknessLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        connect(mThicknessLineEdit, &QLineEdit::textChanged, this, [=](QString text) {
            if (mSelectedCurve)
                emit action(Action::UpdateCurveThickness, text.toInt());
        });

        layout->addWidget(mThicknessLineEdit);

        mainLayout->addLayout(layout);
    }

    // Remove Button
    {
        mRemoveButton = new QPushButton("Remove Curve");
        mRemoveButton->setFlat(false);
        mRemoveButton->setAutoFillBackground(false);
        mRemoveButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        connect(mRemoveButton, &QPushButton::clicked, this, [=]() { emit action(Action::RemoveCurve); });

        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(mRemoveButton);

        mainLayout->addLayout(layout);
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
        mThicknessLineEdit->setText(QString::number(mSelectedCurve->thickness(), 'f', 0));
    } else {
        reset();
    }
}

void CurveWidget::reset()
{
    mZLineEdit->clear();
    mThicknessLineEdit->clear();
    setEnabled(false);
}
