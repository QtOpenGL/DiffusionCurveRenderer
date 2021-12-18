#ifndef RENDERSETTINGSWIDGET_H
#define RENDERSETTINGSWIDGET_H

#include "CustomFlatButton.h"

#include <CustomVariant.h>
#include <Types.h>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
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
    void colorRendererModeChanged(ColorRendererMode);

private:
    QCheckBox *mRenderContoursCheckBox;
    QCheckBox *mDiffuseCheckBox;
    QRadioButton *mLineStripRadioButton;
    QRadioButton *mTriangleStripRadioButton;
    QSlider *mSmoothIterationsSlider;
    QSlider *mDiffusionWidthSlider;
    QSlider *mContourThicknessSlider;
    QSlider *mQualitySlider;
    CustomFlatButton *mColorButton;

private:
    RenderMode mMode;
    ColorRendererMode mColorRendererMode;
};

#endif // RENDERSETTINGSWIDGET_H
