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
    : AudioProcessorEditor(&p), 
    // attach apvts to slider and labels   
    intervalSliderAttachment(audioProcessor.apvts, "Intervals", intervalSlider),
    audioProcessor(p) {

    //setting window size
  setSize(600, 600);

  //attach labels
  intervalLabel.setText ("Intervals", juce::NotificationType::dontSendNotification);
  intervalLabel.attachToComponent (&intervalSlider, false);

  intervalSlider.setSuffix("bar");

  addAndMakeVisible(intervalSlider);
}

SimpleDelayEditor::~SimpleDelayEditor() {}


//==============================================================================
void SimpleDelayEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::black);
}

void SimpleDelayEditor::resized() {
  intervalSlider.setBounds(30, 120, 60, 60);
}
