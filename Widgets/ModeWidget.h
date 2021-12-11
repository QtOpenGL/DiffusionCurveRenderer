#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include <QGroupBox>
#include <QRadioButton>

#include "Types.h"

class ModeWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ModeWidget(QGroupBox *parent = nullptr);

signals:
    void modeChanged(Mode mode);

private slots:
    void refresh();

private:
    QVector<QRadioButton *> mRadioButtons;
    Mode mMode;
};

#endif // MODEWIDGET_H
