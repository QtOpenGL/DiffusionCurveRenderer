#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include <QGroupBox>

#include "Types.h"
#include <QPushButton>

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
    QVector<QPushButton *> mButtons;
    Mode mMode;
};

#endif // MODEWIDGET_H
