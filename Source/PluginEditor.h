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
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class EarthEchoAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                       public juce::AudioProcessorListener,
                                       public juce::Slider::Listener,
                                       public juce::Button::Listener
{
public:
    EarthEchoAudioProcessorEditor (EarthEchoAudioProcessor&);
    ~EarthEchoAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    // Needed Define All Pure Virtual Functions (Decleared with "=0")
    void audioProcessorParameterChanged (AudioProcessor *processor, int parameterIndex, float newValue) override;
    void audioProcessorChanged (AudioProcessor *processor, const ChangeDetails &details) override;
    void sliderValueChanged (juce::Slider* slider) override;
    void buttonClicked (juce::Button* button) override;

private:
    //==============================================================================
    // References the object of AudioProcessor class that created an object of this class.
    EarthEchoAudioProcessor& audioProcessor;
    juce::Array<AudioProcessorParameter*> processorParameters;
    unsigned int numSingleChannelParameters;
    std::vector<juce::Slider> arraySlider;
    std::vector<juce::Label> arrayLabel;
    juce::Colour bgColour;
    juce::Colour textColour;
    juce::Colour thumbColour;
    juce::TextButton buttonChangeBgColour;
    juce::TextButton buttonChangeChannel;
    unsigned int stateColourTheme;
    unsigned int stateChannel;

    //==============================================================================
    juce::LookAndFeel_V4 lookAndFeel;
    void changeLookAndFeel();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EarthEchoAudioProcessorEditor)
};
