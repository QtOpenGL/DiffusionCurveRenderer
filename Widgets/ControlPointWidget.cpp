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

        mControlPointComboBox = new QComboBox;
        mControlPointComboBox->setFocusPolicy(Qt::NoFocus);
        connect(mControlPointComboBox, QOverload<int>::of(&QComboBox::activated), this, [=](int index) {
            if (mSelectedCurve) {
                ControlPoint *controlPoint = mSelectedCurve->getControlPoint(index);
                onSelectedControlPointChanged(controlPoint);
                emit selectedControlPointChanged(controlPoint);
            }
            emit dirty();
        });

        layout->addWidget(mControlPointComboBox);

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
        mControlPointX->setValidator(new QIntValidator(-1000, 1000, this));

        connect(mControlPointX, &QLineEdit::textChanged, this, [this](QString text) {
            if (mSelectedControlPoint) {
                mSelectedControlPoint->position.setX(text.toInt());
                emit dirty();
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
        mControlPointY->setValidator(new QIntValidator(-1000, 1000, this));

        connect(mControlPointY, &QLineEdit::textChanged, this, [this](QString text) {
            if (mSelectedControlPoint) {
                mSelectedControlPoint->position.setY(text.toInt());
                emit dirty();
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

        connect(mRemoveButton, &QPushButton::clicked, this, [this]() { emit action(Action::RemoveControlPoint); });

        QHBoxLayout *layout = new QHBoxLayout;
        //layout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Minimum));
        layout->addWidget(mRemoveButton);
        //layout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Minimum));

        mainLayout->addLayout(layout);
    }

    setTitle("Control Point");
    setLayout(mainLayout);

    mControlPointComboBox->clear();
    mControlPointX->clear();
    mControlPointY->clear();
    setEnabled(false);
}

void ControlPointWidget::onSelectedControlPointChanged(ControlPoint *selectedControlPoint)
{
    if (mSelectedControlPoint == selectedControlPoint)
        return;

    mSelectedControlPoint = selectedControlPoint;

    if (mSelectedCurve) {
        mControlPointComboBox->clear();
        QVector<ControlPoint *> controlPoints = mSelectedCurve->getControlPoints();
        for (int i = 0; i < controlPoints.size(); ++i)
            mControlPointComboBox->addItem(QString::number(controlPoints[i]->index));
    }

    if (mSelectedControlPoint) {
        setEnabled(true);
        mControlPointComboBox->setCurrentIndex(mSelectedControlPoint->index);
        mControlPointX->setText(QString::number(mSelectedControlPoint->position.x(), 'f', 0));
        mControlPointY->setText(QString::number(mSelectedControlPoint->position.y(), 'f', 0));
    } else {
        mControlPointComboBox->clear();
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

    mControlPointComboBox->clear();
    mControlPointX->clear();
    mControlPointY->clear();
    setEnabled(false);

    if (mSelectedCurve) {
        QVector<ControlPoint *> controlPoints = mSelectedCurve->getControlPoints();
        for (int i = 0; i < controlPoints.size(); ++i)
            mControlPointComboBox->addItem(QString::number(controlPoints[i]->index));
    }
}

void ControlPointWidget::onDirty()
{
    if (mSelectedControlPoint) {
        setEnabled(true);
        mControlPointComboBox->setCurrentIndex(mSelectedControlPoint->index);
        mControlPointX->setText(QString::number(mSelectedControlPoint->position.x(), 'f', 0));
        mControlPointY->setText(QString::number(mSelectedControlPoint->position.y(), 'f', 0));
    }
}
