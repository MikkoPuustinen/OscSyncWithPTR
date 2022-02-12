/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SynthVoice.h"

//==============================================================================
/**
*/
class Visualizer : public juce::Component
    , public juce::Timer
{
public:

    Visualizer(OscSyncAudioProcessor& p, juce::AudioProcessorValueTreeState& vts) : audioProcessor(p)
    {
        startTimer(60);
    }

    ~Visualizer()
    {
    }
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::white);
        const int length = getWidth();


        std::vector<float> signal(length, 0);

        for (int i = 0; i < activeNotes.size(); i++)
        {
            SynthVoice voice = SynthVoice();

            auto* syncFreq = audioProcessor.getSyncFrequency();
            auto* polynomial = audioProcessor.getPolynomial();

            double sampleRate = audioProcessor.getSampleRate();

            const int midiNote = activeNotes[i][0];
            const float frequency = 440 * pow(2, (midiNote - 69) / 12.0f);
            float syncFrequency = frequency * pow(2, ((float)*syncFreq / 1200));

            voice.updateAngle(frequency, sampleRate, syncFrequency, *polynomial);
            for (int j = 0; j < signal.size(); j++)
            {
                signal[j] += voice.increment();
            }
        }

        juce::Path path;
        path.startNewSubPath(0, getHeight() / 2);

        for (int i = 0; i < signal.size(); i++)
        {
            path.lineTo(i, 0.25f * ((getHeight() * -1 * signal[i]) / 2) + (getHeight() / 2));
        }
        //path.closeSubPath();

        g.strokePath(path, juce::PathStrokeType(2));


    }
    void timerCallback() override
    {
        juce::Array<juce::Array<int>> newNotes;
        activeNotes = newNotes;
        auto notes = audioProcessor.getActiveMidiNotes();
        for (int i = 0; i < 128; i++) {
            if (notes[i] > 0) {
                activeNotes.add(juce::Array<int> {i, notes[i] });
            }
        }
        repaint();
    }

private:

    juce::Array<juce::Array<int>> activeNotes;
    OscSyncAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Visualizer)
};





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


    Visualizer visualizer;

    juce::Slider syncFrequencySlider;
    std::unique_ptr<SliderAttachment> syncFrequencyAttachment;

    juce::ComboBox polynomialCombo;
    std::unique_ptr<ComboBoxAttachment> polynomialComboAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscSyncAudioProcessorEditor)
};
