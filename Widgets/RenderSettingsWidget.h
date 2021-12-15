#ifndef RENDERSETTINGSWIDGET_H
#define RENDERSETTINGSWIDGET_H

#include "CustomFlatButton.h"

#include <CustomVariant.h>
#include <Types.h>
#include <QCheckBox>
#include <QGroupBox>
#include <QSlider>
#include <QWidget>

class RenderSettingsWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit RenderSettingsWidget(QWidget *parent = nullptr);

signals:
    void action(Action, CustomVariant);
    void renderModeChanged(RenderMode);

private:
    QCheckBox *mRenderContoursCheckBox;
    QCheckBox *mDiffuseCheckBox;
    QSlider *mSmoothIterationsSlider;
    QSlider *mDiffusionWidthSlider;
    QSlider *mContourThicknessSlider;
    QSlider *mQualitySlider;
    CustomFlatButton *mColorButton;

private:
    RenderMode mMode;
};

#endif // RENDERSETTINGSWIDGET_H
