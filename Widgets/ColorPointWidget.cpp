#include "ColorPointWidget.h"

#include <QGridLayout>

ColorPointWidget::ColorPointWidget(QWidget *parent)
    : QGroupBox{parent}
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
        mPositionSlider->setInvertedAppearance(false);
        mPositionSlider->setTickPosition(QSlider::TicksBelow);

        mainLayout->addWidget(new QLabel("Position"), 1, 0);
        mainLayout->addWidget(mPositionSlider, 1, 1);
    }

    // Color Button
    {
        mColorButton = new QPushButton;
        mColorButton->setFlat(false);
        mColorButton->setAutoFillBackground(false);

        mainLayout->addWidget(new QLabel("Color"), 2, 0);
        mainLayout->addWidget(mColorButton, 2, 1);
    }

    // Remove Button
    {
        mRemoveButton = new QPushButton("Remove Color Point");
        mRemoveButton->setFlat(false);
        mRemoveButton->setAutoFillBackground(false);

        mainLayout->addWidget(mRemoveButton, 3, 1);
    }

    setTitle("Color Point");
    setLayout(mainLayout);
    setEnabled(true);
}
