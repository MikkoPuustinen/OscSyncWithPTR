/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OscSyncAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    OscSyncAudioProcessorEditor (OscSyncAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~OscSyncAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OscSyncAudioProcessor& audioProcessor;

    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Slider syncFrequencySlider;
    std::unique_ptr<SliderAttachment> syncFrequencyAttachment;

    juce::ComboBox polynomialCombo;
    std::unique_ptr<ComboBoxAttachment> polynomialComboAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscSyncAudioProcessorEditor)
};
