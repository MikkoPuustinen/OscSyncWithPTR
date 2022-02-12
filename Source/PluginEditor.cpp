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
            valueTreeState(vts),
            visualizer(p, vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 500);

    addAndMakeVisible(&visualizer);

    syncFrequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    syncFrequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(syncFrequencySlider);
    syncFrequencyAttachment.reset(new SliderAttachment(valueTreeState, "syncFrequency", syncFrequencySlider));

    polynomialCombo.addItem("no polynomial", 1);
    polynomialCombo.addItem("w = 1", 2);
    polynomialCombo.addItem("w = 2", 3);
    polynomialCombo.addItem("w = 3", 4);
    polynomialComboAttachment.reset(new ComboBoxAttachment(valueTreeState, "polynomial", polynomialCombo));
    addAndMakeVisible(polynomialCombo);
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
    polynomialCombo.setBounds(getWidth() - 250, getHeight() - 100, 150, 25);

    juce::Rectangle<int> visualizerBounds(50, 50, getWidth() - 100, 250);
    visualizer.setBounds(visualizerBounds);
}
