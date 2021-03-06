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

#pragma once

#include <JuceHeader.h>

#define EARTHECHO_NUMBER_PARAMETERS 12
#define EARTHECHO_NUMBER_PARAMETERS_SINGLECHANNEL 6
#define EARTHECHO_MIDICONTROLLER_MASK 0x1F
#define EARTHECHO_MIDICONTROLLER_MAX_VALUE 100
#define EARTHECHO_MIDICONTROLLER_NUMBER 32
#define EARTHECHO_NAME ((String) "EarthEcho")
#define EARTHECHO_VERSION ((String) ProjectInfo::versionString) // In Generated JuceHeader.h
#define EARTHECHO_COMPANY ((String) ProjectInfo::companyName) // In Generated JuceHeader.h

typedef struct EarthEcho_earthEchoSliderParameters
{
    String id;
    String name;
    juce::NormalisableRange<float> range;
    float initialValue;
    String label;
    float expander;
} _earthEchoSliderParameters;

typedef enum {
    EarthEchoVolume,
    EarthEchoDryWet,
    EarthEchoDelay,
    EarthEchoRepeat,
    EarthEchoLowpassFilter,
    EarthEchoHighpassFilter,
} _earthEchoParameterIndex;

//==============================================================================
/**
*/
class EarthEchoAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    EarthEchoAudioProcessor();
    ~EarthEchoAudioProcessor() override;

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

    //==============================================================================
    int stateColourTheme;

private:
    //==============================================================================
    std::vector<juce::AudioParameterFloat*> arrayParameter;
    std::vector<unsigned short> arrayMidiControllerValue;
    std::vector<std::vector<float>> arrayDelay;
    std::vector<float> arrayMovingAverage;
    std::vector<unsigned int> arrayPlayBackSampleCount;
    unsigned int playBackSampleRate;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EarthEchoAudioProcessor)
};
