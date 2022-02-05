/*
  ==============================================================================

    EarthEcho
    Copyright (C) 2022 Kenta Ishii

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

_earthEchoSliderParameters earthEchoSliderParameters[EARTHECHO_NUMBER_PARAMETERS] =
{
    {"wetgain", "Wet Gain", {0.0f, 1.0f, 0.002f}, 0.2f, " Gain", 5.0f},
    {"drywet", "Dry/Wet", {0.0f, 1.0f, 0.01f}, 0.5f, " Wet", 1.0f},
    {"delay", "Delay Time", {0.0f, 1.0f, 0.01f}, 0.5f, " Seconds", 1.0f},
    {"repeat", "Repeat Rate", {0.0f, 1.0f, 0.01f}, 0.5f, " Repeats", 1.0f},
    {"lowpass", "LP Filter", {0.0f, 1.0f, 0.01f}, 1.0f, " High Freq.", 1.0f},
    {"highpass", "HP Filter", {0.0f, 1.0f, 0.01f}, 0.0f, " High Freq.", 1.0f}
};

//==============================================================================
EarthEchoAudioProcessor::EarthEchoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
       arrayParameter (EARTHECHO_NUMBER_PARAMETERS)
{
    for (unsigned int i = 0; i < arrayParameter.size(); i++)
    {
        addParameter (arrayParameter[i] = new juce::AudioParameterFloat (earthEchoSliderParameters[i].id, earthEchoSliderParameters[i].name, earthEchoSliderParameters[i].range, earthEchoSliderParameters[i].initialValue, earthEchoSliderParameters[i].label));
    }
    juce::Logger::getCurrentLogger()->writeToLog ("Size of paramters is " + String ((int) arrayParameter.size()) + " items.");
}

EarthEchoAudioProcessor::~EarthEchoAudioProcessor()
{
}

//==============================================================================
const juce::String EarthEchoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EarthEchoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EarthEchoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EarthEchoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EarthEchoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EarthEchoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EarthEchoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EarthEchoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EarthEchoAudioProcessor::getProgramName (int index)
{
    return {};
}

void EarthEchoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EarthEchoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initialization before Playback
    //juce::Logger::getCurrentLogger()->writeToLog (String (sampleRate, 5));
    //juce::Logger::getCurrentLogger()->writeToLog (String ((int) sampleRate));
    for (unsigned int i = 0; i < 4; i++)
        arrayMovingAverage.push_back (0);
    for (unsigned int i = 0; i < 2; i++)
        arrayPlayBackSampleCount.push_back (0);
    playBackSampleRate = (unsigned int) sampleRate;
    for (unsigned int i = 0; i < 2; i++)
    {
        std::vector<float> channel (playBackSampleRate, 0.0f);
        arrayDelay.push_back (channel);
        juce::Logger::getCurrentLogger()->writeToLog ("Sample rate of channel " + String (i) + " is " + String (arrayDelay[i].size()) + "Hz.");
    }
}

void EarthEchoAudioProcessor::releaseResources()
{
    // Free Allocated Memory after playback
    arrayDelay[0].clear();
    arrayDelay[1].clear();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EarthEchoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // Confirm the audio layout of the output is mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Confirm the audio layout of the output is the same as the layout of the input.
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void EarthEchoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = (unsigned int) getTotalNumInputChannels();

    // Channels
    for (unsigned int i = 0; i < totalNumInputChannels; ++i)
    {
        auto* inputData = buffer.getReadPointer (i);
        auto* outputData = buffer.getWritePointer (i);
        // Samples
        for (unsigned int j = 0; j < (unsigned int) buffer.getNumSamples(); ++j)
        {
            // Input
            auto currentData = inputData[j];
            auto delayIndex = (unsigned int) ((float) (playBackSampleRate - 1) * arrayParameter[EarthEchoDelay]->get());
            // Mix
            float mixData;
            if (delayIndex == 0)
            {
                mixData = currentData;
            }
            else
            {
                mixData = ((arrayDelay[i][(((unsigned int) arrayPlayBackSampleCount[i] + playBackSampleRate) - delayIndex) % (playBackSampleRate)]) * arrayParameter[EarthEchoRepeat]->get()) + (currentData * (1.0f - arrayParameter[EarthEchoRepeat]->get()));
            }
            arrayDelay[i][arrayPlayBackSampleCount[i]] = mixData;
            // Lowpass Filter
            arrayMovingAverage[i * 2] -= arrayMovingAverage[i * 2] * arrayParameter[EarthEchoLowpassFilter]->get();
            arrayMovingAverage[i * 2] += mixData * arrayParameter[EarthEchoLowpassFilter]->get();
            mixData = arrayMovingAverage[i * 2];
            // Highpass Filter
            arrayMovingAverage[i * 2 + 1] -= arrayMovingAverage[i * 2 + 1] * arrayParameter[EarthEchoHighpassFilter]->get();
            arrayMovingAverage[i * 2 + 1] += mixData * arrayParameter[EarthEchoHighpassFilter]->get();
            mixData = mixData - arrayMovingAverage[i * 2 + 1];
            // Output
            outputData[j] = ((mixData * arrayParameter[EarthEchoDryWet]->get() * arrayParameter[EarthEchoVolume]->get() * earthEchoSliderParameters[EarthEchoVolume].expander) + (currentData * (1.0f - arrayParameter[EarthEchoDryWet]->get())));
            if (++arrayPlayBackSampleCount[i] >= playBackSampleRate)
            {
                arrayPlayBackSampleCount[i] = 0;
            }
        }
    }
}

//==============================================================================
bool EarthEchoAudioProcessor::hasEditor() const
{
    return true; // False if you don't have any editor.
}

juce::AudioProcessorEditor* EarthEchoAudioProcessor::createEditor()
{
    return new EarthEchoAudioProcessorEditor (*this);
}

//==============================================================================
void EarthEchoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save Data to Memory Block
    // Consider to use XML (strings) or JUCE's unique ValueTree classes.
    // Raw Data
    MemoryOutputStream outputStream (destData, true);
    for (unsigned int i = 0; i < arrayParameter.size(); i++)
        outputStream.writeFloat (*arrayParameter[i]);
}

void EarthEchoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Load Data from Memory Block
    // Raw Data
    MemoryInputStream inputStream (data, static_cast<size_t> (sizeInBytes), false);
    for (unsigned int i = 0; i < arrayParameter.size(); i++)
        arrayParameter[i]->setValueNotifyingHost (inputStream.readFloat());
}

//==============================================================================
// Declared in JUCE/modules/juce_audio_plugin_client/utility/juce_CreatePluginFilter.h
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EarthEchoAudioProcessor();
}
