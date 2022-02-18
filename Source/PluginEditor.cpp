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


    juce::LookAndFeel::setDefaultLookAndFeel(&OscSyncLookAndFeel::getInstance());


    setSize (900, 500);

    addAndMakeVisible(&visualizer);
    toneGeneratorOnButton.setButtonText("toggle oscillator");
    toneGeneratorOnButton.setLookAndFeel(&OscSyncLookAndFeel::getInstance());

    addAndMakeVisible(toneGeneratorOnButton);
    toneGeneratorOnAttachment.reset(new ButtonAttachment(valueTreeState, "toneGeneratorOn", toneGeneratorOnButton));
   // toneGeneratorOnButton.onClick = [this] { updateToggleState(&toneGeneratorOnButton, "toggle oscillator");   };

    toneGeneratorSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    toneGeneratorSlider.setLookAndFeel(&OscSyncLookAndFeel::getInstance());
    toneGeneratorSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(toneGeneratorSlider);
    toneGeneratorAttachment.reset(new SliderAttachment(valueTreeState, "toneGenerator", toneGeneratorSlider));

    syncFrequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    syncFrequencySlider.setLookAndFeel(&OscSyncLookAndFeel::getInstance());
    syncFrequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(syncFrequencySlider);
    syncFrequencyAttachment.reset(new SliderAttachment(valueTreeState, "syncFrequency", syncFrequencySlider));


    polynomialCombo.setLookAndFeel(&OscSyncLookAndFeel::getInstance());
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
    g.fillAll (juce::Colour(245, 239, 237));
    //g.fillAll(juce::Colours::black);

    juce::Path path;

    g.setColour(juce::Colour(106, 112, 110));

    path.startNewSubPath(200, 180);
    path.lineTo(200, getHeight() - 50);

    g.strokePath(path, juce::PathStrokeType(3));

}

void OscSyncAudioProcessorEditor::resized()
{
    syncFrequencySlider.setBounds(250, getHeight() - 180, 80, 100);
    toneGeneratorSlider.setBounds(50 , getHeight() - 180, 80, 100);
    polynomialCombo.setBounds(400, getHeight() - 180, 150, 25);
    toneGeneratorOnButton.setBounds(50, getHeight() - 220, 100, 40);

    juce::Rectangle<int> visualizerBounds(0, 30, getWidth() - 0, 250);
    visualizer.setBounds(visualizerBounds);
}
