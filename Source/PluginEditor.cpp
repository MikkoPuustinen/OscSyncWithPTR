/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OscSyncAudioProcessorEditor::OscSyncAudioProcessorEditor (OscSyncAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
        : AudioProcessorEditor (&p), 
            audioProcessor (p),
            valueTreeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 300);

    syncFrequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    syncFrequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(syncFrequencySlider);
    syncFrequencyAttachment.reset(new SliderAttachment(valueTreeState, "syncFrequency", syncFrequencySlider));
}

OscSyncAudioProcessorEditor::~OscSyncAudioProcessorEditor()
{
}

//==============================================================================
void OscSyncAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void OscSyncAudioProcessorEditor::resized()
{
    syncFrequencySlider.setBounds(40, getHeight() - 180, 60, 80);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
