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

EarthEchoAudioProcessorEditorCustomLookAndFeel::EarthEchoAudioProcessorEditorCustomLookAndFeel (juce::Colour bgColour, juce::Colour textColour)
{
    setWindowLookAndFeel(bgColour, textColour);
}

void EarthEchoAudioProcessorEditorCustomLookAndFeel::setWindowLookAndFeel (juce::Colour bgColour, juce::Colour textColour)
{
    getCurrentColourScheme().setUIColour (ColourScheme::windowBackground, bgColour);
    getCurrentColourScheme().setUIColour (ColourScheme::widgetBackground, bgColour);
    getCurrentColourScheme().setUIColour (ColourScheme::menuBackground, bgColour);
    getCurrentColourScheme().setUIColour (ColourScheme::outline, bgColour);
    getCurrentColourScheme().setUIColour (ColourScheme::defaultText, textColour);
    getCurrentColourScheme().setUIColour (ColourScheme::defaultFill, bgColour);
    getCurrentColourScheme().setUIColour (ColourScheme::highlightedText, textColour);
    getCurrentColourScheme().setUIColour (ColourScheme::highlightedFill, bgColour);
    getCurrentColourScheme().setUIColour (ColourScheme::menuText, textColour);
    getCurrentColourScheme().setUIColour (ColourScheme::numColours, textColour);
    setColour (juce::ResizableWindow::backgroundColourId, bgColour);
    setColour (juce::DocumentWindow::textColourId, textColour);
}

/**
* Start of the modification of JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V4.cpp Line No. 109-192 (JUCE version 6.1.5)
* to override the function, Button* LookAndFeel_V4::createDocumentWindowButton (int buttonType), for changing colors of window buttons.
* The modification date is February 14, 2022.
* This code is part of the JUCE library.
* Copyright (c) 2020 - Raw Material Software Limited
* This code is used under the terms of the GNU General Public License Version 3 (www.gnu.org/licenses/gpl-3.0).
*/
class LookAndFeel_V4_DocumentWindowButton   : public Button
{
public:
// Modification: Removed "Colour c,"
    LookAndFeel_V4_DocumentWindowButton (const String& name, const Path& normal, const Path& toggled)
// Modification: Removed "colour (c),"
        : Button (name), normalShape (normal), toggledShape (toggled)
    {
    }

    void paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto background = Colours::grey;
// Modification: Added "auto colour = Colours::white;"
        auto colour = Colours::white;
        if (auto* rw = findParentComponentOfClass<ResizableWindow>())
            if (auto lf = dynamic_cast<LookAndFeel_V4*> (&rw->getLookAndFeel()))
// Modification: Added "{"
            {
                background = lf->getCurrentColourScheme().getUIColour (LookAndFeel_V4::ColourScheme::widgetBackground);
// Modification: Added "colour = lf->getCurrentColourScheme().getUIColour (LookAndFeel_V4::ColourScheme::defaultText);"
                colour = lf->getCurrentColourScheme().getUIColour (LookAndFeel_V4::ColourScheme::defaultText);
// Modification: Added "}"
            }

        g.fillAll (background);

        g.setColour ((! isEnabled() || shouldDrawButtonAsDown) ? colour.withAlpha (0.6f)
                                                     : colour);

        if (shouldDrawButtonAsHighlighted)
        {
            g.fillAll();
            g.setColour (background);
        }

        auto& p = getToggleState() ? toggledShape : normalShape;

        auto reducedRect = Justification (Justification::centred)
                              .appliedToRectangle (Rectangle<int> (getHeight(), getHeight()), getLocalBounds())
                              .toFloat()
                              .reduced ((float) getHeight() * 0.3f);

        g.fillPath (p, p.getTransformToScaleToFit (reducedRect, true));
    }

private:
// Modification: Removed "Colour colour;"
    Path normalShape, toggledShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookAndFeel_V4_DocumentWindowButton)
};

// Modification: "LookAndFeel_V4::createDocumentWindowButton" to "EarthEchoAudioProcessorEditorCustomLookAndFeel::createDocumentWindowButton"
Button* EarthEchoAudioProcessorEditorCustomLookAndFeel::createDocumentWindowButton (int buttonType)
{
    Path shape;
    auto crossThickness = 0.15f;

    if (buttonType == DocumentWindow::closeButton)
    {
        shape.addLineSegment ({ 0.0f, 0.0f, 1.0f, 1.0f }, crossThickness);
        shape.addLineSegment ({ 1.0f, 0.0f, 0.0f, 1.0f }, crossThickness);
// Modification: Removed "Colour (0xff9A131D),"
        return new LookAndFeel_V4_DocumentWindowButton ("close", shape, shape);
    }

    if (buttonType == DocumentWindow::minimiseButton)
    {
        shape.addLineSegment ({ 0.0f, 0.5f, 1.0f, 0.5f }, crossThickness);
// Modification: Removed "Colour (0xffaa8811),"
        return new LookAndFeel_V4_DocumentWindowButton ("minimise", shape, shape);
    }

    if (buttonType == DocumentWindow::maximiseButton)
    {
        shape.addLineSegment ({ 0.5f, 0.0f, 0.5f, 1.0f }, crossThickness);
        shape.addLineSegment ({ 0.0f, 0.5f, 1.0f, 0.5f }, crossThickness);

        Path fullscreenShape;
        fullscreenShape.startNewSubPath (45.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 45.0f);
        fullscreenShape.addRectangle (45.0f, 45.0f, 100.0f, 100.0f);
        PathStrokeType (30.0f).createStrokedPath (fullscreenShape, fullscreenShape);
// Modification: Removed "Colour (0xff0A830A),"
        return new LookAndFeel_V4_DocumentWindowButton ("maximise", shape, fullscreenShape);
    }

    jassertfalse;
    return nullptr;
}
/**
* End of the modification of JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V4.cpp Line No. 153-192 (JUCE version 6.1.5).
* Copyright (c) 2020 - Raw Material Software Limited
* This code is used under the terms of the GNU General Public License Version 3 (www.gnu.org/licenses/gpl-3.0).
*/

//==============================================================================
EarthEchoAudioProcessorEditor::EarthEchoAudioProcessorEditor (EarthEchoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), processorParameters (audioProcessor.getParameters()), numSingleChannelParameters ((unsigned int) (audioProcessor.getParameters().size() / audioProcessor.getTotalNumInputChannels())), arraySlider ((unsigned int) audioProcessor.getParameters().size()), arrayLabel ((unsigned int) audioProcessor.getParameters().size()), bgColour (juce::Colours::blue), textColour (juce::Colours::yellow), thumbColour (juce::Colours::magenta), stateColourTheme (0), stateChannel (0), lookAndFeel (bgColour, textColour)
{
    // Set Custom Look And Feel Overall
    LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);
    if (audioProcessor.wrapperType == AudioProcessor::wrapperType_Standalone)
        juce::TopLevelWindow::getTopLevelWindow (0)->setName(EARTHECHO_NAME + String (" ") + EARTHECHO_VERSION);
    // Set the size of this GUI before the end of this constructor.
    //setResizeLimits(600, 200, 600, 400);
    //setResizable(true, false);
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
    // Ensure to Reset Look and Feel
    LookAndFeel::setDefaultLookAndFeel (nullptr);
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
    g.drawFittedText (EARTHECHO_NAME + newLine + EARTHECHO_VERSION, getLocalBounds(), juce::Justification::centred, 1);
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
        changeLookAndFeel();
        repaint();
        if (audioProcessor.wrapperType == AudioProcessor::wrapperType_Standalone)
            juce::TopLevelWindow::getTopLevelWindow (0)->repaint(); // Update Colors for Title Bar
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
    button->giveAwayKeyboardFocus(); // Unfocus to Hide to Darken Button Color after Clicking
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
    lookAndFeel.setColour (juce::TextEditor::highlightedTextColourId, bgColour);// Also Effects Text Box of Slider
    lookAndFeel.setColour (juce::Label::backgroundColourId, juce::Colour (0x00000000));
    lookAndFeel.setColour (juce::Label::textColourId, textColour);
    lookAndFeel.setColour (juce::Label::outlineColourId, juce::Colour (0x00000000));
    lookAndFeel.setColour (juce::Label::backgroundWhenEditingColourId, bgColour); // Also Effects Text Box of Slider
    lookAndFeel.setColour (juce::Label::textWhenEditingColourId, textColour); // Also Effects Text Box of Slider
    lookAndFeel.setColour (juce::Label::outlineWhenEditingColourId, textColour); // Also Effects Text Box of Slider
    lookAndFeel.setColour (juce::TextButton::buttonColourId, bgColour);
    lookAndFeel.setColour (juce::TextButton::buttonOnColourId, bgColour); // juce::Button::setToggleState
    lookAndFeel.setColour (juce::TextButton::textColourOffId, textColour); // juce::Button::setToggleState
    lookAndFeel.setColour (juce::TextButton::textColourOnId, textColour); // juce::Button::setToggleState
    lookAndFeel.setColour (juce::ComboBox::outlineColourId, textColour); // Also Outline of juce::TextButton
    for (unsigned int i = 0; i < arraySlider.size(); i++)
    {
        arraySlider[i].lookAndFeelChanged(); // To Change Colors for Text Box of Slider
    }
    lookAndFeel.setWindowLookAndFeel (bgColour, textColour);
}
