/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"

//==============================================================================
SimpleDelayEditor::SimpleDelayEditor(
    SimpleDelay &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Set up the slider
    setupSlider(gainSlider, gainSliderAttachment, "gain");

  setSize(400, 300);
}

SimpleDelayEditor::~SimpleDelayEditor() {}

//==============================================================================
void SimpleDelayEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::black);
  g.setFont(30.0f);
}

void SimpleDelayEditor::resized() {
  gainSlider.setBounds (getLocalBounds().reduced (100));
}

void SimpleDelayEditor::setupSlider(juce::Slider& slider, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment, const juce::String& parameterID)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(slider);
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, parameterID, slider);
}
