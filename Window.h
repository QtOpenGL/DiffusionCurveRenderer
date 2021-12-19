#ifndef WINDOW_H
#define WINDOW_H

#include <Widgets/CentralWidget.h>

#include <QMainWindow>

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QMainWindow *parent = nullptr);
    ~Window();

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void setCentralWidget(CentralWidget *newCentralWidget);
    void initialize();

private:
    CentralWidget *mCentralWidget;
};
#endif // WINDOW_H
