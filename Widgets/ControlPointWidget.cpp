#include "ControlPointWidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>

ControlPointWidget::ControlPointWidget(QGroupBox *parent)
    : QGroupBox(parent)
    , mSelectedControlPoint(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    // Index
    {
        QHBoxLayout *layout = new QHBoxLayout;
        QLabel *label = new QLabel("Index");
        layout->addWidget(label);

        mControlPointLabel = new QLabel;
        mControlPointLabel->setFocusPolicy(Qt::NoFocus);
        layout->addWidget(mControlPointLabel);
        mainLayout->addLayout(layout);
    }

    // X
    {
        QHBoxLayout *layout = new QHBoxLayout;
        QLabel *label = new QLabel("X");
        layout->addWidget(label);

        mControlPointX = new QLineEdit;
        mControlPointX->setReadOnly(false);
        mControlPointX->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        mControlPointX->setValidator(new QIntValidator(-10000, 10000, this));

        connect(mControlPointX, &QLineEdit::textEdited, this, [=](QString text) {
            if (mSelectedControlPoint) {
                float x = text.toFloat();
                float y = mSelectedControlPoint->position.y();
                emit action(Action::UpdateControlPointPosition, QPointF(x, y));
            }
        });

        layout->addWidget(mControlPointX);

        mainLayout->addLayout(layout);
    }

    // Y
    {
        QHBoxLayout *layout = new QHBoxLayout;
        QLabel *label = new QLabel("Y");
        layout->addWidget(label);

        mControlPointY = new QLineEdit;
        mControlPointY->setReadOnly(false);
        mControlPointY->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        mControlPointY->setValidator(new QIntValidator(-10000, 10000, this));

        connect(mControlPointY, &QLineEdit::textEdited, this, [this](QString text) {
            if (mSelectedControlPoint) {
                float x = mSelectedControlPoint->position.x();
                float y = text.toFloat();
                emit action(Action::UpdateControlPointPosition, QPointF(x, y));
            }
        });

        layout->addWidget(mControlPointY);

        mainLayout->addLayout(layout);
    }

    // Remove Button
    {
        mRemoveButton = new QPushButton("Remove Control Point");
        mRemoveButton->setFlat(false);
        mRemoveButton->setAutoFillBackground(false);
        mRemoveButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        connect(mRemoveButton, &QPushButton::clicked, this, [=]() { emit action(Action::RemoveControlPoint); });

        QHBoxLayout *layout = new QHBoxLayout;
        //layout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Minimum));
        layout->addWidget(mRemoveButton);
        //layout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Minimum));

        mainLayout->addLayout(layout);
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
        mControlPointLabel->setText(QString::number(mSelectedControlPoint->index));
        mControlPointX->setText(QString::number(mSelectedControlPoint->position.x(), 'f', 0));
        mControlPointY->setText(QString::number(mSelectedControlPoint->position.y(), 'f', 0));
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
        setEnabled(true);
        mControlPointLabel->setText(QString::number(mSelectedControlPoint->index));
        mControlPointX->setText(QString::number(mSelectedControlPoint->position.x(), 'f', 0));
        mControlPointY->setText(QString::number(mSelectedControlPoint->position.y(), 'f', 0));
    }
}
