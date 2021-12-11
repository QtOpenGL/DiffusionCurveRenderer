#include "ControlPointWidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>

ControlPointWidget::ControlPointWidget(QGroupBox *parent)
    : QGroupBox(parent)
    , mSelectedControlPoint(nullptr)
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setVerticalSpacing(8);
    mainLayout->setColumnStretch(0, 8);
    mainLayout->setColumnStretch(1, 8);

    // Control Point Label
    {
        mControlPointLabel = new QLabel;
        mainLayout->addWidget(new QLabel("Index"), 0, 0);
        mainLayout->addWidget(mControlPointLabel, 0, 1);
    }

    // X
    {
        mControlPointX = new QLineEdit;
        mControlPointX->setReadOnly(false);
        mControlPointX->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        mControlPointX->setValidator(new QIntValidator(-10000, 10000, this));

        connect(mControlPointX, &QLineEdit::textEdited, this, [=](QString text) {
            if (mSelectedControlPoint) {
                float x = text.toFloat();
                float y = mSelectedControlPoint->position().y();
                emit action(Action::UpdateControlPointPosition, QPointF(x, y));
            }
        });

        mainLayout->addWidget(new QLabel("X"), 1, 0);
        mainLayout->addWidget(mControlPointX, 1, 1);
    }

    // Y
    {
        mControlPointY = new QLineEdit;
        mControlPointY->setReadOnly(false);
        mControlPointY->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        mControlPointY->setValidator(new QIntValidator(-10000, 10000, this));

        connect(mControlPointY, &QLineEdit::textEdited, this, [this](QString text) {
            if (mSelectedControlPoint) {
                float x = mSelectedControlPoint->position().x();
                float y = text.toFloat();
                emit action(Action::UpdateControlPointPosition, QPointF(x, y));
            }
        });

        mainLayout->addWidget(new QLabel("Y"), 2, 0);
        mainLayout->addWidget(mControlPointY, 2, 1);
    }

    // Remove Button
    {
        mRemoveButton = new QPushButton("Remove Control Point");
        mRemoveButton->setFlat(false);
        mRemoveButton->setAutoFillBackground(false);
        connect(mRemoveButton, &QPushButton::clicked, this, [=]() { emit action(Action::RemoveControlPoint); });
        mainLayout->addWidget(mRemoveButton, 3, 1);
    }

    setTitle("Control Point");
    setLayout(mainLayout);
    mControlPointLabel->clear();
    mControlPointX->clear();
    mControlPointY->clear();
    setEnabled(false);
}

void ControlPointWidget::onSelectedControlPointChanged(ControlPoint *selectedControlPoint)
{
    if (mSelectedControlPoint == selectedControlPoint)
        return;

    mSelectedControlPoint = selectedControlPoint;

    if (mSelectedControlPoint) {
        setEnabled(true);
        mControlPointLabel->setText(QString::number(mSelectedControlPoint->index()));
        mControlPointX->setText(QString::number(mSelectedControlPoint->position().x(), 'f', 0));
        mControlPointY->setText(QString::number(mSelectedControlPoint->position().y(), 'f', 0));
    } else {
        mControlPointLabel->clear();
        mControlPointX->clear();
        mControlPointY->clear();
        setEnabled(false);
    }
}

void ControlPointWidget::onSelectedCurveChanged(Curve *selectedCurve)
{
    if (mSelectedCurve == selectedCurve)
        return;

    mSelectedCurve = selectedCurve;

    mControlPointLabel->clear();
    mControlPointX->clear();
    mControlPointY->clear();
    setEnabled(false);
}

void ControlPointWidget::refresh()
{
    if (mSelectedControlPoint) {
        mControlPointLabel->setText(QString::number(mSelectedControlPoint->index()));
        mControlPointX->setText(QString::number(mSelectedControlPoint->position().x(), 'f', 0));
        mControlPointY->setText(QString::number(mSelectedControlPoint->position().y(), 'f', 0));
        setEnabled(true);
    }
}
