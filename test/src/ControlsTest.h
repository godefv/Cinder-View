#pragma once

#include "ui/Suite.h"

#include "cinder/gl/TextureFont.h"

class ControlsTest : public ui::SuiteView {
public:
	ControlsTest();

	void layout() override;

private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	ui::HSliderRef			mHSlider;
	ui::VSliderRef			mVSlider;
	ui::ButtonRef			mButton, mImageButton, mToggle;
	ui::CheckBoxRef			mCheckBox;
    ui::TextFieldRef        mTextField1, mTextField2, mTextField3;
};
