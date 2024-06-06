/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"


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
  void setupSlider(juce::Slider& slider, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment, const juce::String& parameterID);



 private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  SimpleDelay &audioProcessor;

  juce::Slider gainSlider;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDelayEditor)
};
