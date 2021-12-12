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
        connect(mZLineEdit, &QLineEdit::textChanged, this, [=](QString text) {
            bool success = false;
            int z = text.toInt(&success);
            if (success) {
                emit action(Action::UpdateCurveZIndex, z);
            }
        });

        mainLayout->addWidget(new QLabel("Z-index"), 0, 0);
        mainLayout->addWidget(mZLineEdit, 0, 2);
    }

    // Remove Curve Button
    {
        mRemoveButton = new QPushButton("Remove Curve");
        mRemoveButton->setFlat(false);
        mRemoveButton->setAutoFillBackground(false);

        connect(mRemoveButton, &QPushButton::clicked, this, [=]() { emit action(Action::RemoveCurve); });

        mainLayout->addWidget(mRemoveButton, 1, 2);
    }

    setLayout(mainLayout);
    setTitle("Curve");
    mZLineEdit->clear();
    setEnabled(false);
}

void CurveWidget::onSelectedCurveChanged(Curve *selectedCurve)
{
    mSelectedCurve = selectedCurve;
}

void CurveWidget::onDirty(DirtType type)
{
    if (type & DirtType::GUI) {
        if (mSelectedCurve) {
            setEnabled(true);
            mZLineEdit->setText(QString::number(mSelectedCurve->z()));
        } else {
            mZLineEdit->clear();
            setEnabled(false);
        }
    }
}
