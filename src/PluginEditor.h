/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "RotarySlider.h"
#include "NameLabel.h"


//==============================================================================
/**
 */
class SimpleDelayEditor : public juce::AudioProcessorEditor {
 public:
  SimpleDelayEditor(SimpleDelay &);
  ~SimpleDelayEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

 private:
  SimpleDelay &audioProcessor;

// intantiate label and slider instances

NameLabel intervalLabel;

RotarySlider intervalSlider;

// intantiate slider attachments
juce::AudioProcessorValueTreeState::SliderAttachment intervalSliderAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDelayEditor)
};
