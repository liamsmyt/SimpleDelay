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
    auto delayBufferSize = static_cast<int>(2 * sampleRate); // 2 seconds of delay buffer
    delayBuffer.setSize(getTotalNumInputChannels(), delayBufferSize);
    delayBuffer.clear();
    writePosition = 0;
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

    // Clear output channels that exceed the number of input channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();

    for(int channel = 0; channel < totalNumInputChannels; ++channel){
      auto* channelData = buffer.getWritePointer(channel);
      fillCircularBuffer(channel, bufferSize, delayBufferSize, channelData);
      readFromBuffer (buffer, delayBuffer, channel, bufferSize, delayBufferSize);
    }

    writePosition += bufferSize;
    writePosition %= delayBufferSize;
}

void SimpleDelay::fillCircularBuffer(int channel, int bufferSize, int delayBufferSize, float* channelData) {
  // check if buffer fits into delayBuffer
  if(bufferSize + writePosition < delayBufferSize){
    // copy buffer to delayBuffer
    delayBuffer.copyFrom(channel, writePosition, channelData, bufferSize);
  }
  // if buffer does not fit and needs to be wrapped
  else{
    // How much space is left from the current writePosition to the end of the delayBuffer
    auto numSamplesToEnd = delayBufferSize - writePosition;

    // Paste part of buffer to the space at the end
    delayBuffer.copyFrom(channel, writePosition, channelData, numSamplesToEnd);

    // How many samples from the start need to be filled
    auto numSamplesAtStart = bufferSize - numSamplesToEnd;


    // Paste remaining part of buffer
    delayBuffer.copyFrom(channel, 0, channelData + numSamplesToEnd, numSamplesAtStart);
  }
}

void SimpleDelay::readFromBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int channel, int bufferSize, int delayBufferSize)
{
  auto readPosition = writePosition - getSampleRate();

  // if readPosition is a negative value means need to invert and go to 'end' of delayBuffer
  if(readPosition < 0)
  {
    readPosition += delayBufferSize;
  }

  auto g = 0.5f;
  //if no wrap is needed
  if(readPosition + bufferSize < delayBufferSize){
    // copy channelData from the past to present
    buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), bufferSize, g, g);
  }
  else{
    auto numSamplesToEnd = delayBufferSize - readPosition;
    buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), numSamplesToEnd, g, g);

    auto numSamplesAtStart = bufferSize - numSamplesToEnd;
    buffer.addFromWithRamp(channel, numSamplesToEnd, delayBuffer.getReadPointer(channel, 0), numSamplesAtStart, g, g);
  }
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
