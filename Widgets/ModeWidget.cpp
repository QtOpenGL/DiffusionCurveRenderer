#include "ModeWidget.h"

#include <QHBoxLayout>

ModeWidget::ModeWidget(QGroupBox *parent)
    : QGroupBox(parent)
    , mMode(Mode::Select)
{
    QVBoxLayout *layout = new QVBoxLayout;

    QVector<QString> labels;
    labels << "Select"
           << "Append Control Point"
           << "Insert Control Point"
           << "Add Color Point"
           << "Move Curve"
           << "Pan";

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

    setLayout(layout);
    setTitle("Action Modes");
    refresh();
}

void ModeWidget::refresh()
{
    for (int i = 0; i < mRadioButtons.size(); ++i) {
        mRadioButtons[i]->setChecked((int) mMode == i);
    }
}
