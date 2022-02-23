/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SynthVoice.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class Visualizer : public juce::Component
    , public juce::Timer
    , public juce::KeyListener
    , public juce::Button::Listener
{
public:

    Visualizer(OscSyncAudioProcessor& p, juce::AudioProcessorValueTreeState& vts) : 
        audioProcessor(p), 
        zoomIn("zoomIn", 0.75, juce::Colours::black),
        zoomOut("zoomOut", 0.25, juce::Colours::black),
        left("left", 0.5, juce::Colours::black),
        right("right", 0.0, juce::Colours::black)
    {
        startTimer(60);
        setWantsKeyboardFocus(true);
        addKeyListener(this);
        addAndMakeVisible(left);
        addAndMakeVisible(right);

        addAndMakeVisible(zoomIn);
        addAndMakeVisible(zoomOut);

        left.addListener(this);
        right.addListener(this);
        zoomIn.addListener(this);
        zoomOut.addListener(this);

    }

    ~Visualizer()
    {
        removeKeyListener(this);
    }
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(245, 239, 237));
        int length = getWidth();

        if (zoom < 1.0f)
        {
            length *= 1 / zoom;
        }

        length += 510;
        std::vector<float> signal(length, 0);
        auto* syncFreq = audioProcessor.getSyncFrequency();
        auto* polynomial = audioProcessor.getPolynomial();

        double sampleRate = audioProcessor.getSampleRate();

        for (int i = 0; i < activeNotes.size(); i++)
        {
            SynthVoice voice = SynthVoice();

            const int midiNote = activeNotes[i][0];
            const float frequency = 440 * pow(2, (midiNote - 69) / 12.0f);
            float syncFrequency = frequency * pow(2, ((float)*syncFreq / 1200));

            voice.updateAngle(frequency, sampleRate, syncFrequency, *polynomial);
            voice.setStart(startSample);
            for (int j = 0; j < signal.size(); j++)
            {
                signal[j] += voice.increment();
            }
        }

        if ((bool)*audioProcessor.getToneGeneratorStatus())
        {
            auto* frequency = audioProcessor.getToneGeneratorFrequency();
            SynthVoice voice = SynthVoice();
            float syncFrequency = (float)*frequency * pow(2, ((float)*syncFreq / 1200));
            voice.updateAngle((float)*frequency, sampleRate, syncFrequency, *polynomial);
            voice.setStart(startSample);

            for (int j = 0; j < signal.size(); j++)
            {
                signal[j] += voice.increment();
            }
        }

        juce::Path path;
        path.startNewSubPath(0, 0.18f * ((getHeight() * -1 * signal[500]) / 2) + (getHeight() / 2));

        for (int i = 1; i < signal.size() - 500; i++)
        {
            const int index = i;
            path.lineTo(i * zoom, 0.18f * ((getHeight() * -1 * signal[index]) / 2) + (getHeight() / 2));
            if (zoom > 5.0f)
            {
                g.drawEllipse(i * zoom - 4, 0.18f * ((getHeight() * -1 * signal[index]) / 2) + (getHeight() / 2) - 4, 8, 8, 1);
            }
        }

        g.strokePath(path, juce::PathStrokeType(2));


    }

    void buttonClicked(juce::Button* button)
    {
        if (button == &left)
        {
            startSample -= 10;
            if (startSample < 1)
            {
                startSample = 1;
            }
        } 
        else if (button == &right)
        {
            startSample += 10;
        }
        else if (button == &zoomIn)
        {
            zoom *= 1.4f;
        }
        else if (button == &zoomOut)
        {
            zoom /= 1.4f;
            if (zoom <= 0.5f)
            {
                zoom = 0.5f;
            }
        }
    }

    void resized() override
    {
        const int x = getWidth();
        left.setBounds(x - 100, 35, 20, 20);
        right.setBounds(x - 60, 35, 20, 20);

        zoomIn.setBounds(x - 80, 20, 20, 20);
        zoomOut.setBounds(x - 80, 50, 20, 20);
    }

    bool keyPressed(const juce::KeyPress& key, juce::Component* c) override
    {
        int k = key.getKeyCode();

        if (k == juce::KeyPress::upKey)
        {
            zoom += 0.1f;
            return true;
        }
        else if (k == juce::KeyPress::downKey)
        {
            zoom -= 0.1f;
            if (zoom <= 0.3f)
            {
                zoom = 0.3f;
            }
            return true;
        }
        else if (k == juce::KeyPress::rightKey)
        {
            startSample += 10;
            return false;
        }
        else if (k == juce::KeyPress::leftKey)
        {
            startSample -= 10;
            if (startSample < 1)
            {
                startSample = 1;
            }
            return false;
        }
        return false;
    }

    void timerCallback() override
    {
        juce::Array<juce::Array<int>> newNotes;
        activeNotes = newNotes;
        auto notes = audioProcessor.getActiveMidiNotes();
        for (int i = 0; i < 128; i++) {
            if (notes[i] > 0) {
                activeNotes.add(juce::Array<int> { i, notes[i] });
            }
        }
        repaint();
    }

private:


    int startSample = 1;
    float zoom = 1.0f;
    juce::Array<juce::Array<int>> activeNotes;
    OscSyncAudioProcessor& audioProcessor;


    juce::ArrowButton zoomIn;
    juce::ArrowButton zoomOut;

    juce::ArrowButton left;
    juce::ArrowButton right;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Visualizer)
};





class OscSyncAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

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

    juce::Slider toneGeneratorSlider;
    std::unique_ptr<SliderAttachment> toneGeneratorAttachment;

    juce::ToggleButton toneGeneratorOnButton {"toglre"};
    std::unique_ptr<ButtonAttachment> toneGeneratorOnAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscSyncAudioProcessorEditor)
};
