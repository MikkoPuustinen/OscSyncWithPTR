/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OscSyncAudioProcessor::OscSyncAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, juce::Identifier("OscSync"),
                        {
                            std::make_unique<juce::AudioParameterFloat>("syncFrequency",                                                     // parameterID
                                                                        "Sync Frequency",                                                    // parameter name
                                                                        juce::NormalisableRange(0.0f, 2400.0f, 1.0f),                        // range
                                                                        0.0f),                                                               // default value
                            std::make_unique<juce::AudioParameterInt>("polynomial"   ,"Polynomial"   ,  0     ,     3 ,        0),
                        })
#endif
{
    for (int i = 0; i < 10; i++)
    {
        voices.push_back(SynthVoice());
    }

    syncFrequency = parameters.getRawParameterValue("syncFrequency");
    polynomial = parameters.getRawParameterValue("polynomial");

}

OscSyncAudioProcessor::~OscSyncAudioProcessor()
{

}

//==============================================================================
const juce::String OscSyncAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OscSyncAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OscSyncAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OscSyncAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OscSyncAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

double OscSyncAudioProcessor::getSampleRate()
{
    return sampleRate;
}

int* OscSyncAudioProcessor::getActiveMidiNotes()
{
    return activeMidiNotes;
}

std::atomic<float>* OscSyncAudioProcessor::getPolynomial()
{
    return polynomial;
}

std::atomic<float>* OscSyncAudioProcessor::getSyncFrequency()
{
    return syncFrequency;
}

int OscSyncAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OscSyncAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OscSyncAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OscSyncAudioProcessor::getProgramName (int index)
{
    return {};
}

void OscSyncAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OscSyncAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    this->sampleRate = sampleRate;

}

void OscSyncAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OscSyncAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void OscSyncAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    juce::MidiBuffer::Iterator i(midiMessages);
    juce::MidiMessage m;
    int t;
    while (i.getNextEvent(m, t))
    {
        if (m.isNoteOn()) {
            activeMidiNotes[m.getNoteNumber()] = m.getVelocity();
        }
        if (m.isNoteOff()) {
            activeMidiNotes[m.getNoteNumber()] = 0;
        }
        if (m.isAllNotesOff()) {
            for (int i = 0; i < 128; ++i)
                activeMidiNotes[i] = 0;
        }
    }
    juce::Array<juce::Array<int>> activeNotes;

    for (int i = 0; i < 128; i++) {
        if (activeMidiNotes[i] > 0) {
            activeNotes.add(juce::Array<int> {i, activeMidiNotes[i] });
        }
    }

    auto level = 0.125f;
    auto* leftBuffer  = buffer.getWritePointer(0);
    auto* rightBuffer = buffer.getWritePointer(1);


    for (int i = 0; i < activeNotes.size(); i++)
    {
        const int midiNote = activeNotes[i][0];
        int voiceIndex = -1;
        for (int j = 0; j < voices.size(); j++)
        {
            if (voices[j].note == midiNote)
            {
                voiceIndex = j;
            }
        }
        if (voiceIndex < 0)
        {
            int inc = 0;
            while (voiceIndex < 0 && inc < voices.size())
            {

                if (voices[inc].note == 0)
                {
                    voices[inc].setNote(midiNote);
                    voiceIndex = inc;
                }
                inc++;
            }
        }
        if (!(voiceIndex == -1))
        {
            const float frequency = 440 * pow(2, (midiNote - 69) / 12.0f);
            
            for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float s = frequency * pow(2, ((float)*syncFrequency / 1200));
                voices[voiceIndex].updateAngle(frequency, sampleRate, s, *polynomial);
                auto curr = voices[voiceIndex].increment();
                leftBuffer[sample]  += level * curr;
                rightBuffer[sample] += level * curr;
            }
        }
        
    }

    for (int i = 0; i < voices.size(); i++)
    {
        bool isActive = false;
        for (int j = 0; j < activeNotes.size(); j++)
        {
            if (voices[i].note == activeNotes[j][0])
            {
                isActive = true;
            }
        }
        if (!isActive)
        {
            voices[i].reset();
        }
    }
}

//==============================================================================
bool OscSyncAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OscSyncAudioProcessor::createEditor()
{
    return new OscSyncAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void OscSyncAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OscSyncAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OscSyncAudioProcessor();
}
