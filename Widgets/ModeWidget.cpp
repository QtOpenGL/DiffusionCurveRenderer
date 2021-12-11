#include "ModeWidget.h"

#include <QHBoxLayout>

ModeWidget::ModeWidget(QGroupBox *parent)
    : QGroupBox(parent)
    , mMode(Mode::Select)
    , mSelectedCurve(nullptr)
{
    QVBoxLayout *layout = new QVBoxLayout;

    QVector<QString> labels;
    labels << "Select"               //0
           << "Append Control Point" // 1
           << "Insert Control Point" // 2
           << "Add Color Point"      // 3
           << "Move Curve"           // 4
           << "Pan";                 // 5
    mAddColorPointRadioButtonIndex = 3;

    for (int i = 0; i < labels.size(); ++i) {
        QRadioButton *button = new QRadioButton(labels[i]);
        button->setCheckable(true);

        connect(button, &QRadioButton::clicked, this, [=]() {
            mMode = static_cast<Mode>(i);
            emit modeChanged(mMode);
            refresh();
        });

        mRadioButtons << button;
        layout->addWidget(button);
    }

    mRadioButtons[mAddColorPointRadioButtonIndex]->setEnabled(false);
    setLayout(layout);
    setTitle("Action Modes");
    refresh();
}

void ModeWidget::onSelectedCurveChanged(Curve *selectedCurve)
{
    mSelectedCurve = selectedCurve;
    refresh();
}

void ModeWidget::refresh()
{
    for (int i = 0; i < mRadioButtons.size(); ++i) {
        mRadioButtons[i]->setChecked((int) mMode == i);
    }

    if (mSelectedCurve) {
        mRadioButtons[mAddColorPointRadioButtonIndex]->setEnabled(mSelectedCurve->getSize() >= 2);
    } else {
        mRadioButtons[mAddColorPointRadioButtonIndex]->setEnabled(false);
    }
}
