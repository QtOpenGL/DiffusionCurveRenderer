#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include <QGroupBox>

#include <QPushButton>

class ModeWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ModeWidget(QGroupBox *parent = nullptr);

    enum Mode {
        Select = 0,
        Add = 1,
        Move = 2,
        Pan = 3
    } mMode;

signals:
    void modeChanged(ModeWidget::Mode mode);

private slots:
    void refresh();

private:
    QVector<QPushButton*> mButtons;

};

#endif // MODEWIDGET_H
