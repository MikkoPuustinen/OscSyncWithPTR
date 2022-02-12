/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthVoice.h"


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

    double getSampleRate();

    int* getActiveMidiNotes();

    std::atomic<float>* getPolynomial();

    std::atomic<float>* getSyncFrequency();

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

    std::atomic<float>* polynomial = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscSyncAudioProcessor)
};
