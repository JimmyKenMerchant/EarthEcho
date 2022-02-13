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
    : AudioProcessorEditor (&p), audioProcessor (p), processorParameters (audioProcessor.getParameters()), numSingleChannelParameters ((unsigned int) (audioProcessor.getParameters().size() / audioProcessor.getTotalNumInputChannels())), arraySlider ((unsigned int) audioProcessor.getParameters().size()), arrayLabel ((unsigned int) audioProcessor.getParameters().size()), bgColour (juce::Colours::blue), textColour (juce::Colours::yellow), thumbColour (juce::Colours::magenta), stateColourTheme (0), stateChannel (0)
{
    // Set the size of this GUI before the end of this constructor.
    setSize (600, 400);
    //juce::Logger::getCurrentLogger()->writeToLog (String (arraySlider.size()));
    for (unsigned int i = 0; i < arraySlider.size(); i++)
    {
        arraySlider[i].setSliderStyle (juce::Slider::LinearVertical);
        arraySlider[i].setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20); // No read-only = setTextBoxIsEditable (true)
        arraySlider[i].setTextValueSuffix (processorParameters[i]->getLabel());
        arraySlider[i].setRange (earthEchoSliderParameters[i].range.start, earthEchoSliderParameters[i].range.end * earthEchoSliderParameters[i].expander);
        arraySlider[i].setValue (processorParameters[i]->getValue() * earthEchoSliderParameters[i].expander);
        arraySlider[i].setNumDecimalPlacesToDisplay (2);
        arraySlider[i].addListener (this);
        addChildComponent (&arraySlider[i]);
        arrayLabel[i].setJustificationType (juce::Justification::centred);
        arrayLabel[i].setText (processorParameters[i]->getName (20), juce::dontSendNotification);
        addChildComponent (&arrayLabel[i]);
    }
    buttonChangeBgColour.setButtonText ("BGCOLOR");
    buttonChangeBgColour.addListener (this);
    addAndMakeVisible (&buttonChangeBgColour);
    buttonChangeChannel.setButtonText ("L/R");
    buttonChangeChannel.addListener (this);
    addAndMakeVisible (&buttonChangeChannel);
    audioProcessor.addListener (this);
    changeLookAndFeel();
}

EarthEchoAudioProcessorEditor::~EarthEchoAudioProcessorEditor()
{
    // audioProcessor is the reference of EarthEchoAudioProcessor that is alive even after destructing this class.
    // If you don't remove a listener. The registration of the listener in EarthEchoAudioProcessor will be left.
    // Caution that the left listener causes a segmentation fault.
    audioProcessor.removeListener (this);
}

//==============================================================================
void EarthEchoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // GUI Customizing
    g.fillAll (bgColour);

    // Unique Settings on Background of GUI
    // Set Text Colour
    g.setColour (textColour);
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
    auto leftCornerX = (int) getLocalBounds().getX();
    auto leftCornerY = (int) getLocalBounds().getY();
    auto editorWidth = (int) getLocalBounds().getWidth();
    auto editorHeight = (int) getLocalBounds().getHeight();
    auto sliderWidth = (int) ((float) editorWidth / (float) numSingleChannelParameters);
    auto sliderHeight = (int) (3.0f * ((float) editorHeight / 4.0f));
    auto labelButtonHeight =  (int) ((float) (editorHeight - sliderHeight) / 2.0f);
    for (unsigned int i = 0; i < arraySlider.size(); i++)
    {
        arraySlider[i].setBounds (leftCornerX + (sliderWidth * (int) (i % numSingleChannelParameters)), leftCornerY, sliderWidth, sliderHeight);
        arrayLabel[i].setBounds (leftCornerX + (sliderWidth * (int) (i % numSingleChannelParameters)), leftCornerY + sliderHeight, sliderWidth, labelButtonHeight);
        if (i < numSingleChannelParameters)
        {
            arraySlider[i].setVisible (true);
            arrayLabel[i].setVisible (true);
        }
        else
        {
            arraySlider[i].setVisible (false);
            arrayLabel[i].setVisible (false);
        }
    }
    buttonChangeBgColour.setBounds (leftCornerX + (sliderWidth * (int) (numSingleChannelParameters - 1)), leftCornerY + sliderHeight + labelButtonHeight, sliderWidth, labelButtonHeight);
    buttonChangeChannel.setBounds (leftCornerX + (sliderWidth * (int) (numSingleChannelParameters - 2)), leftCornerY + sliderHeight + labelButtonHeight, sliderWidth, labelButtonHeight);
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

void EarthEchoAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &buttonChangeBgColour)
    {
        if (stateColourTheme == 0)
        {
            bgColour = juce::Colours::black;
            textColour = juce::Colours::yellow;
            thumbColour = juce::Colours::magenta;
            stateColourTheme = 1;
        }
        else if (stateColourTheme == 1)
        {
            bgColour = juce::Colours::coral;
            textColour = juce::Colours::white;
            thumbColour = juce::Colours::cadetblue;
            stateColourTheme = 2;
        }
        else if (stateColourTheme == 2)
        {
            bgColour = juce::Colours::cadetblue;
            textColour = juce::Colours::darkolivegreen;
            thumbColour = juce::Colours::white;
            stateColourTheme = 3;
        }
        else if (stateColourTheme == 3)
        {
            bgColour = juce::Colours::blue;
            textColour = juce::Colours::yellow;
            thumbColour = juce::Colours::magenta;
            stateColourTheme = 0;
        }
        repaint();
        changeLookAndFeel();
    }
    else if (button == &buttonChangeChannel)
    {
        if (stateChannel == 0)
        {
            stateChannel = 1;
        }
        else if (stateChannel == 1)
        {
            stateChannel = 0;
        }
        for (unsigned int i = 0; i < arraySlider.size(); i++)
        {
            if (i < numSingleChannelParameters)
            {
                arraySlider[i].setVisible ((bool) stateChannel ^ 1);
                arrayLabel[i].setVisible ((bool) stateChannel ^ 1);
            }
            else
            {
                arraySlider[i].setVisible ((bool) stateChannel);
                arrayLabel[i].setVisible ((bool) stateChannel);
            }
        }
    }
}

void EarthEchoAudioProcessorEditor::changeLookAndFeel()
{
    // Set Colors
    lookAndFeel.setColour (juce::Slider::backgroundColourId, juce::Colour (0x00000000));
    lookAndFeel.setColour (juce::Slider::thumbColourId, thumbColour);
    lookAndFeel.setColour (juce::Slider::trackColourId, textColour);
    lookAndFeel.setColour (juce::Slider::textBoxTextColourId, textColour);
    lookAndFeel.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    lookAndFeel.setColour (juce::Slider::textBoxHighlightColourId, textColour);
    lookAndFeel.setColour (juce::Slider::textBoxOutlineColourId, textColour);
    lookAndFeel.setColour (juce::Label::backgroundColourId, juce::Colour (0x00000000));
    lookAndFeel.setColour (juce::Label::textColourId, textColour);
    lookAndFeel.setColour (juce::Label::outlineColourId, juce::Colour (0x00000000));
    lookAndFeel.setColour (juce::TextButton::buttonColourId, juce::Colour (0x00000000));
    lookAndFeel.setColour (juce::TextButton::buttonOnColourId, juce::Colours::white);
    lookAndFeel.setColour (juce::TextButton::textColourOffId, textColour);
    lookAndFeel.setColour (juce::TextButton::textColourOnId, juce::Colours::magenta);
    lookAndFeel.setColour (juce::ComboBox::outlineColourId, textColour);
    for (unsigned int i = 0; i < arraySlider.size(); i++)
    {
        arraySlider[i].setLookAndFeel(&lookAndFeel);
        arraySlider[i].lookAndFeelChanged(); // To Change Colors for Text Box of Slider
        arrayLabel[i].setLookAndFeel(&lookAndFeel);
    }
    buttonChangeBgColour.setLookAndFeel(&lookAndFeel);
    buttonChangeChannel.setLookAndFeel(&lookAndFeel);
}
