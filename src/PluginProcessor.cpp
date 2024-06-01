/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"


//==============================================================================



SimpleDelay::SimpleDelay()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
}

SimpleDelay::~SimpleDelay() {}

//==============================================================================


const juce::String SimpleDelay::getName() const {
  return JucePlugin_Name;
}

bool SimpleDelay::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool SimpleDelay::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool SimpleDelay::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double SimpleDelay::getTailLengthSeconds() const { return 0.0; }

int SimpleDelay::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int SimpleDelay::getCurrentProgram() { return 0; }

void SimpleDelay::setCurrentProgram(int index) {}

const juce::String SimpleDelay::getProgramName(int index) {
  return {};
}

void SimpleDelay::changeProgramName(int index,
                                                 const juce::String &newName) {}

//==============================================================================
void SimpleDelay::prepareToPlay(double sampleRate,
                                             int samplesPerBlock) {
  juce::dsp::ProcessSpec spec;

  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = samplesPerBlock;
  spec.numChannels = 1;

}

void SimpleDelay::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleDelay::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void SimpleDelay::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    // Clear output channels that exceed the number of input channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    auto* intervalParameter = apvts.getRawParameterValue("Intervals");
    int intervalIndex = static_cast<int>(*intervalParameter);

    // Ensure intervalIndex is within the valid range
    if (intervalIndex < 0 || intervalIndex >= 7) {
        intervalIndex = 2; // Default to 1/8 note
    }

    // Map the interval index to delay time in seconds
    float delayTimeSeconds = getDelayTimeFromIntervalIndex(intervalIndex);

    juce::dsp::AudioBlock<float> block (buffer);
    processDelay(block, delayTimeSeconds);
}

float SimpleDelay::getDelayTimeFromIntervalIndex(int intervalIndex) {
    float tempoBPM = 120.0f; // Example tempo, this should ideally be retrieved from your DAW or set dynamically
    float noteValue = 0.0f;


    switch (intervalIndex) {
        case 0: noteValue = 1.0f / 32.0f; break; // 1/32 note
        case 1: noteValue = 1.0f / 16.0f; break; // 1/16 note
        case 2: noteValue = 1.0f / 8.0f; break;  // 1/8 note
        case 3: noteValue = 1.0f / 4.0f; break;  // 1/4 note
        case 4: noteValue = 1.0f / 2.0f; break;  // 1/2 note
        case 5: noteValue = 1.0f; break;         // Whole note
        case 6: noteValue = 2.0f; break;         // Double whole note
        default: noteValue = 1.0f / 8.0f; break; // Default to 1/8 note
    }

    return 60.0f / (tempoBPM * noteValue);
}

void SimpleDelay::processDelay(juce::dsp::AudioBlock<float>& block, float delayTimeSeconds) {
  
}






//==============================================================================
bool SimpleDelay::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *SimpleDelay::createEditor() {
  return new SimpleDelayEditor(*this);
}

//==============================================================================
void SimpleDelay::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void SimpleDelay::setStateInformation(const void *data,
                                                   int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new SimpleDelay();
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleDelay::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add (std::make_unique<juce::AudioParameterChoice> ("Intervals",
                                                              "Intervals",
                                                              juce::StringArray({ "1/32","1/16", "1/8", "1/4", "1/2", "1", "2" }),
                                                              3));
    return layout;
}
