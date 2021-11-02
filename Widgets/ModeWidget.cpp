#include "ModeWidget.h"

#include <QHBoxLayout>

ModeWidget::ModeWidget(QGroupBox *parent) : QGroupBox(parent)
  , mMode(Select)
{
    QHBoxLayout* layout = new QHBoxLayout;

    QVector<QString> labels;
    labels << "Select" << "Add" << "Move" << "Pan";

    for(int i = 0; i < labels.size(); ++i)
    {
        QPushButton* button = new QPushButton(labels[i]);
        button->setCheckable(true);

        connect(button, &QPushButton::clicked, this, [=](){
            mMode = static_cast<Mode>(i);
            emit modeChanged(mMode);
            refresh();
        });

        mButtons << button;
        layout->addWidget(button);
    }

    setLayout(layout);
    setTitle("Modes");
    refresh();
}

void ModeWidget::refresh()
{
    for(int i = 0; i < mButtons.size(); ++i)
    {
        mButtons[i]->setChecked(mMode == i);
    }
}
