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
    int currentAngle = 0;

    double frequency = 0;
    double sampleRate = 0;
    float syncFrequency = 0;
    int syncAngle = 0;

    void updateAngle(double freq, double sRate, float syncFreq) 
    {
        frequency = freq;
        sampleRate = sRate;
        syncFrequency = syncFreq;
    }

    void increment() 
    {
        float f1 = 2 * (std::fmod((currentAngle * frequency / sampleRate), 1)) - 1;
        float nextSample = 2 * (std::fmod(((currentAngle + 1) * frequency / sampleRate), 1)) - 1;
        float f2 = 2 * (std::fmod((syncAngle * syncFrequency / sampleRate), 1)) - 1;
        float nextSample2 = 2 * (std::fmod(((syncAngle + 1) * syncFrequency / sampleRate), 1)) - 1;
        if (nextSample < f1)
        {
            syncAngle = 0;
        }
        else if (nextSample2 < f2) 
        {
            syncAngle = 0;
        }
        else 
        {

        }
        currentAngle++;

        syncAngle++;
        
    }

    void setNote(int midiNote)
    {
        note = midiNote;
    }

    float synthesize()
    {
        increment();
        return (2 * (syncAngle * syncFrequency / sampleRate) - 1);
    }

    void reset()
    {
        isActive = false;
        note = 0;
        currentAngle = 0;
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


    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* syncFrequency = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscSyncAudioProcessor)
};
