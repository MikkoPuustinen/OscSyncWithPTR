/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
struct SynthVoice {
    bool isActive = false;
    int note = 0;
    double currentAngle = 0;
    double angleDelta = 0;

    void updateAngle(double freq, double sampleRate) 
    {
        auto cyclesPerSample = freq / sampleRate;
        angleDelta = 1; //yclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }

    void increment() 
    {
        currentAngle += angleDelta;
    }

    void setNote(int midiNote)
    {
        note = midiNote;
    }

    void reset()
    {
        note = 0;
        currentAngle = 0;
        angleDelta = 0;
    }
};

class OscSyncAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    OscSyncAudioProcessor();
    ~OscSyncAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;

    int findNote(int note);
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    int activeMidiNotes[128] = { 0 };

    double sampleRate = 0;
    SynthVoice voice;
    std::vector<SynthVoice> voices;
    double halfStep;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscSyncAudioProcessor)
};
