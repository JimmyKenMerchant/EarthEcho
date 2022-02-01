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

extern _earthEchoSliderParameters earthEchoSliderParameters[];

//==============================================================================
EarthEchoAudioProcessorEditor::EarthEchoAudioProcessorEditor (EarthEchoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), arraySlider ((unsigned int) audioProcessor.getParameters().size()), arrayLabel ((unsigned int) audioProcessor.getParameters().size())
{
    // Set the size of this GUI before the end of this constructor.
    setSize (600, 400);
    processorParameters = audioProcessor.getParameters();
    audioProcessor.addListener(this);
    //juce::Logger::getCurrentLogger()->writeToLog (String (arraySlider.size()));

    for (unsigned int i = 0; i < arraySlider.size(); i++)
    {
        addAndMakeVisible (&arraySlider[i]);
        arraySlider[i].setSliderStyle (juce::Slider::LinearVertical);
        arraySlider[i].setTextBoxStyle (juce::Slider::TextBoxBelow, true, 80, 20);
        arraySlider[i].setTextValueSuffix (processorParameters[i]->getLabel());
        arraySlider[i].setRange (earthEchoSliderParameters[i].range.start, earthEchoSliderParameters[i].range.end * earthEchoSliderParameters[i].expander);
        arraySlider[i].setValue (processorParameters[i]->getValue() * earthEchoSliderParameters[i].expander);
        arraySlider[i].setNumDecimalPlacesToDisplay(2);
        arraySlider[i].addListener (this);
        arraySlider[i].setColour (juce::Slider::backgroundColourId, juce::Colour (0x00000000));
        arraySlider[i].setColour (juce::Slider::thumbColourId, juce::Colours::magenta);
        arraySlider[i].setColour (juce::Slider::trackColourId, juce::Colours::yellow);
        arraySlider[i].setColour (juce::Slider::textBoxTextColourId, juce::Colours::yellow);
        arraySlider[i].setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
        arraySlider[i].setColour (juce::Slider::textBoxHighlightColourId, juce::Colours::yellow);
        arraySlider[i].setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::yellow);

        addAndMakeVisible (&arrayLabel[i]);
        arrayLabel[i].setJustificationType (juce::Justification::centred);
        arrayLabel[i].setText (processorParameters[i]->getName(12), juce::dontSendNotification);
        arrayLabel[i].setColour (juce::Label::backgroundColourId, juce::Colour (0x00000000));
        arrayLabel[i].setColour (juce::Label::textColourId, juce::Colours::yellow);
        arrayLabel[i].setColour (juce::Label::outlineColourId, juce::Colour (0x00000000));
    }
}

EarthEchoAudioProcessorEditor::~EarthEchoAudioProcessorEditor()
{
    arraySlider.clear();
    arrayLabel.clear();
}

//==============================================================================
void EarthEchoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // GUI Customizing
    g.fillAll (juce::Colours::blue);

    // Unique Settings on Background of GUI
    // Set Text Colour
    g.setColour (juce::Colours::yellow);
    g.setFont (16.0f);
    g.drawFittedText (String ("EarthEcho") + newLine + EARTHECHO_VERSION, getLocalBounds(), juce::Justification::centred, 1);
}

void EarthEchoAudioProcessorEditor::resized()
{
    // Subcomponents
    //juce::Logger::getCurrentLogger()->writeToLog (String (getLocalBounds().getX()));
    //juce::Logger::getCurrentLogger()->writeToLog (String (getLocalBounds().getY()));
    //juce::Logger::getCurrentLogger()->writeToLog (String (getLocalBounds().getWidth()));
    //juce::Logger::getCurrentLogger()->writeToLog (String (getLocalBounds().getHeight()));
    auto leftCornerX = (unsigned int) getLocalBounds().getX();
    auto leftCornerY = (unsigned int) getLocalBounds().getY();
    auto sliderWidth = (unsigned int) ((float) getLocalBounds().getWidth() / 6.0f);
    auto sliderHeight = (unsigned int) (3.0f * ((float) getLocalBounds().getHeight() / 4.0f));
    auto labelHeight =  (unsigned int) getLocalBounds().getHeight() - sliderHeight;
    for (unsigned int i = 0; i < arraySlider.size(); i++)
    {
        arraySlider[i].setBounds (leftCornerX + (sliderWidth * i), leftCornerY, sliderWidth, sliderHeight);
        arrayLabel[i].setBounds (leftCornerX + (sliderWidth * i), leftCornerY + sliderHeight, sliderWidth, labelHeight);
    }
}

void EarthEchoAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    for (unsigned int i = 0; i < arraySlider.size(); i++)
    {
        if (slider == &arraySlider[i])
        {
            // Notify to the host to sync with host's panel to control parameters of this plugin.
            processorParameters[i]->setValueNotifyingHost ((float) slider->getValue() / earthEchoSliderParameters[i].expander);
        }
    }
}

void EarthEchoAudioProcessorEditor::audioProcessorParameterChanged (AudioProcessor *processorParameterChanged, int parameterIndex, float newValue)
{
    //juce::Logger::getCurrentLogger()->writeToLog (String ("EarthEchoAudioProcessorEditor::audioProcessorParameterChanged"));
    arraySlider[(unsigned int) parameterIndex].setValue (newValue * earthEchoSliderParameters[(unsigned int) parameterIndex].expander);
}

void EarthEchoAudioProcessorEditor::audioProcessorChanged (AudioProcessor *processorChanged, const ChangeDetails &details)
{
    //juce::Logger::getCurrentLogger()->writeToLog (String ("EarthEchoAudioProcessorEditor::audioProcessorChanged"));
}
