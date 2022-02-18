#pragma once
#include <JuceHeader.h>

class OscSyncLookAndFeel : public juce::LookAndFeel_V4
{
public:
	OscSyncLookAndFeel();
	static OscSyncLookAndFeel& getInstance();

	void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&);

	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, ::juce::Slider& slider) override;

	void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos,
		float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& s) override;

	juce::Label* createSliderTextBox(juce::Slider& slider) override;

	void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
		bool, bool isButtonDown) override;

};