#ifndef RENDERSETTINGSWIDGET_H
#define RENDERSETTINGSWIDGET_H

#include <QCheckBox>
#include <QGroupBox>
#include <QSlider>
#include <QWidget>

class RenderSettingsWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit RenderSettingsWidget(QWidget *parent = nullptr);

private:
    QCheckBox *mRenderContoursCheckBox;
    QCheckBox *mDiffuseCheckBox;
    QSlider *mSmoothIterationsSlider;
    QSlider *mQualitySlider;
};

#endif // RENDERSETTINGSWIDGET_H
