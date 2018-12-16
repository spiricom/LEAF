/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "PluginProcessor.h"

#include "UIComponent.h"

//==============================================================================
/**
*/
class OopsAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    OopsAudioProcessorEditor (OopsAudioProcessor&);
    ~OopsAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    UIComponent uicomponent;
    OopsAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OopsAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
