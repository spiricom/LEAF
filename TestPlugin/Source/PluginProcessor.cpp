/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "UIComponent.h"

#include "LEAFTest.h"

//==============================================================================
OopsAudioProcessorEditor::OopsAudioProcessorEditor (OopsAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
    
    addAndMakeVisible(uicomponent);
}

OopsAudioProcessorEditor::~OopsAudioProcessorEditor()
{
}

//==============================================================================
void OopsAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void OopsAudioProcessorEditor::resized()
{
    uicomponent.setBounds(getBounds());
}
//==============================================================================
void OopsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    gain = 0.0f;
    timer = 0;
    start = false;
    ramp = false;
    
    LEAFTest_init(sampleRate, samplesPerBlock);
}



//====================================BLOCK=======================================

void OopsAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
   const int totalNumInputChannels  = getTotalNumInputChannels();
   const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    LEAFTest_block();
    
    MidiMessage m;
    for (MidiMessageMetadata metadata : midiMessages)
    {
        m = metadata.getMessage();
        int noteNumber = m.getNoteNumber();
        float velocity = m.getFloatVelocity();
        
        if (m.isNoteOn())
        {
            LEAFTest_noteOn(noteNumber, velocity);
        }
        else if (m.isNoteOff())
        {
            LEAFTest_noteOff(noteNumber);
        }
        else
        {
            
        }
    }
    

   const float* inPointerL = buffer.getReadPointer (0);
   const float* inPointerR = buffer.getReadPointer (1);
    
    float* outPointerL = buffer.getWritePointer( 0);
    float* outPointerR = buffer.getWritePointer( 1);
    
    for (int samp = 0; samp < buffer.getNumSamples(); ++samp)
    {
        outPointerL[samp] = LEAFTest_tick( (inPointerL[samp] +inPointerR[samp]) * 0.5f);
        outPointerR[samp] = outPointerL[samp];
    }
}

//==============================================================================

OopsAudioProcessor::OopsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
    #if ! JucePlugin_IsMidiEffect
        #if ! JucePlugin_IsSynth
    .withInput  ("Input",  AudioChannelSet::stereo(), true)
        #endif
    .withOutput ("Output", AudioChannelSet::stereo(), true)
    #endif
                  )
#endif
{
    
}

OopsAudioProcessor::~OopsAudioProcessor()
{

}

//==============================================================================
const String OopsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OopsAudioProcessor::acceptsMidi() const
{
    return true;
}

bool OopsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double OopsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OopsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OopsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OopsAudioProcessor::setCurrentProgram (int index)
{
}

const String OopsAudioProcessor::getProgramName (int index)
{
    return String();
}

void OopsAudioProcessor::changeProgramName (int index, const String& newName)
{
}

void OopsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    LEAFTest_end();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OopsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


//==============================================================================
bool OopsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OopsAudioProcessor::createEditor()
{
    return new OopsAudioProcessorEditor (*this);
}

//==============================================================================
void OopsAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OopsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OopsAudioProcessor();
}
