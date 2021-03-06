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
    {"wetgain_l", "Wet Gain L", {0.0f, 1.0f, 0.00125f}, 0.125f, " Gain", 8.0f},
    {"drywet_l", "Dry/Wet L", {0.0f, 1.0f, 0.01f}, 0.5f, " Wet", 1.0f},
    {"delay_l", "Delay Time L", {0.0f, 1.0f, 0.01f}, 0.5f, " Seconds", 1.0f},
    {"repeat_l", "Repeat Rate L", {0.0f, 1.0f, 0.01f}, 0.5f, " Repeats", 1.0f},
    {"lowpass_l", "LP Filter L", {0.0f, 1.0f, 0.01f}, 1.0f, " Hi-Freq", 1.0f},
    {"highpass_l", "HP Filter L", {0.0f, 1.0f, 0.01f}, 0.0f, " Hi-Freq", 1.0f},
    {"wetgain_r", "Wet Gain R", {0.0f, 1.0f, 0.00125f}, 0.125f, " Gain", 8.0f},
    {"drywet_r", "Dry/Wet R", {0.0f, 1.0f, 0.01f}, 0.5f, " Wet", 1.0f},
    {"delay_r", "Delay Time R", {0.0f, 1.0f, 0.01f}, 0.5f, " Seconds", 1.0f},
    {"repeat_r", "Repeat Rate R", {0.0f, 1.0f, 0.01f}, 0.5f, " Repeats", 1.0f},
    {"lowpass_r", "LP Filter R", {0.0f, 1.0f, 0.01f}, 1.0f, " Hi-Freq", 1.0f},
    {"highpass_r", "HP Filter R", {0.0f, 1.0f, 0.01f}, 0.0f, " Hi-Freq", 1.0f}
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
       stateColourTheme (0),
       arrayParameter (EARTHECHO_NUMBER_PARAMETERS),
       arrayMidiControllerValue (EARTHECHO_MIDICONTROLLER_NUMBER, 0)
{
    for (unsigned int i = 0; i < arrayParameter.size(); ++i)
    {
        addParameter (arrayParameter[i] = new juce::AudioParameterFloat (earthEchoSliderParameters[i].id, earthEchoSliderParameters[i].name, earthEchoSliderParameters[i].range, earthEchoSliderParameters[i].initialValue, earthEchoSliderParameters[i].label));
    }
    juce::Logger::getCurrentLogger()->writeToLog ("Size of paramters is " + juce::String (static_cast<int> (arrayParameter.size())) + " items.");
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

void EarthEchoAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const juce::String EarthEchoAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void EarthEchoAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/)
{
}

//==============================================================================
void EarthEchoAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    // Initialization before Playback
    //juce::Logger::getCurrentLogger()->writeToLog (juce::String (sampleRate, 5));
    //juce::Logger::getCurrentLogger()->writeToLog (juce::String (static_cast<int> (sampleRate)));
    playBackSampleRate = static_cast<unsigned int> (sampleRate);
    // Moving Average for Low-pass Filter and High-pass Filter for Each Channel
    for (unsigned int i = 0; i < static_cast<unsigned int> (getTotalNumInputChannels() * 2); ++i)
        arrayMovingAverage.push_back (0);
    // Set Zero to Counter of Samples for Each Channel
    for (unsigned int i = 0; i < static_cast<unsigned int> (getTotalNumInputChannels()); ++i)
        arrayPlayBackSampleCount.push_back (0);
    // Make Memory Space to Store Delay Sound for Each Channel
    for (unsigned int i = 0; i < static_cast<unsigned int> (getTotalNumInputChannels()); ++i)
    {
        std::vector<float> channel (playBackSampleRate, 0.0f);
        arrayDelay.push_back (channel);
        juce::Logger::getCurrentLogger()->writeToLog ("Sample rate of channel " + juce::String (i) + " is " + juce::String (arrayDelay[i].size()) + "Hz.");
    }
    if (wrapperType == AudioProcessor::wrapperType_Standalone)
    {
        juce::StringArray earthEchoArguments = juce::JUCEApplication::getInstance()->getCommandLineParameterArray();
        juce::File path (File::getCurrentWorkingDirectory().getFullPathName() + File::getSeparatorString() + earthEchoArguments[0]);
        if (path.existsAsFile())
        {
            juce::Logger::getCurrentLogger()->writeToLog (juce::String ("Loading Parameter File..."));
            juce::MemoryBlock savedData;
            if (path.loadFileAsData (savedData))
                setStateInformation (savedData.getData(), static_cast<int> (savedData.getSize()));
        }
    }
}

void EarthEchoAudioProcessor::releaseResources()
{
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
    auto totalNumInputChannels  = static_cast<unsigned int> (getTotalNumInputChannels());
    // MIDI Messages
    if (! midiMessages.isEmpty())
    {
        for (const MidiMessageMetadata midiMessageMetadata : midiMessages)
        {
            if (midiMessageMetadata.numBytes == 3)
            {
                auto midiMessage = midiMessageMetadata.getMessage();
                if (midiMessage.isController())
                {
                    auto indexMidiController = static_cast<unsigned int> (midiMessage.getControllerNumber() & EARTHECHO_MIDICONTROLLER_MASK);
                    auto valueMidiController = static_cast<unsigned short> (midiMessage.getControllerValue());
                    arrayMidiControllerValue[indexMidiController] = valueMidiController > EARTHECHO_MIDICONTROLLER_MAX_VALUE ? EARTHECHO_MIDICONTROLLER_MAX_VALUE : valueMidiController;
                    //juce::Logger::getCurrentLogger()->writeToLog ("MIDI Controller Changes Value MSB on " + String (indexMidiController) + ": " + String (arrayMidiControllerValue[indexMidiController]));
                    if (indexMidiController < EARTHECHO_NUMBER_PARAMETERS)
                        arrayParameter[indexMidiController]->setValueNotifyingHost (static_cast<float> (arrayMidiControllerValue[indexMidiController]) / static_cast<float> (EARTHECHO_MIDICONTROLLER_MAX_VALUE));
                    //juce::Logger::getCurrentLogger()->writeToLog ("MIDI Message: " + midiMessage.getDescription());
                }
            }
        }
    }
    // Channels
    for (unsigned int i = 0; i < totalNumInputChannels; ++i)
    {
        auto* inputData = buffer.getReadPointer (static_cast<int> (i));
        auto* outputData = buffer.getWritePointer (static_cast<int> (i));
        // Samples
        for (unsigned int j = 0; j < static_cast<unsigned int> (buffer.getNumSamples()); ++j)
        {
            // Input
            auto currentData = inputData[j];
            auto delayIndex = static_cast<unsigned int> (static_cast<float> (playBackSampleRate - 1) * arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoDelay]->get());
            // Mix
            float mixData;
            if (delayIndex == 0)
            {
                mixData = currentData;
            }
            else
            {
                mixData = ((arrayDelay[i][((static_cast<unsigned int> (arrayPlayBackSampleCount[i]) + playBackSampleRate) - delayIndex) % (playBackSampleRate)]) * arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoRepeat]->get()) + (currentData * (1.0f - arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoRepeat]->get()));
            }
            arrayDelay[i][arrayPlayBackSampleCount[i]] = mixData;
            // Low-pass Filter
            arrayMovingAverage[i * 2] -= arrayMovingAverage[i * 2] * arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoLowpassFilter]->get();
            arrayMovingAverage[i * 2] += mixData * arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoLowpassFilter]->get();
            mixData = arrayMovingAverage[i * 2];
            // High-pass Filter
            arrayMovingAverage[i * 2 + 1] -= arrayMovingAverage[i * 2 + 1] * arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoHighpassFilter]->get();
            arrayMovingAverage[i * 2 + 1] += mixData * arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoHighpassFilter]->get();
            mixData = mixData - arrayMovingAverage[i * 2 + 1];
            // Output
            outputData[j] = ((mixData * arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoDryWet]->get() * arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoVolume]->get() * earthEchoSliderParameters[EarthEchoVolume].expander) + (currentData * (1.0f - arrayParameter[EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL * i + EarthEchoDryWet]->get())));
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
    juce::MemoryOutputStream outputStream (destData, true);
    for (unsigned int i = 0; i < arrayParameter.size(); ++i)
        outputStream.writeFloat (*arrayParameter[i]);
    outputStream.writeInt (stateColourTheme);
}

void EarthEchoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Load Data from Memory Block
    // Raw Data
    juce::MemoryInputStream inputStream (data, static_cast<size_t> (sizeInBytes), false);
    for (unsigned int i = 0; i < arrayParameter.size(); ++i)
        arrayParameter[i]->setValueNotifyingHost (inputStream.readFloat());
    stateColourTheme = inputStream.readInt();
}

//==============================================================================
// Declared in JUCE/modules/juce_audio_plugin_client/utility/juce_CreatePluginFilter.h
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EarthEchoAudioProcessor();
}
