/*
  ==============================================================================

    Utilities.h
    Created: 5 Dec 2016 2:37:03pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "stdlib.h"

typedef enum WaveformType {
    Sine=0,
    Triangle,
    Sawtooth,
    Square,
    WaveformTypeNil
}WaveformType;

extern std::vector<juce::String> cSliderNames;

extern std::vector<juce::String> cButtonNames;

extern std::vector<juce::String> cComboBoxNames;

extern std::vector<juce::String> cWaveformTypes;

extern std::vector<float> cSliderValues;

extern std::vector<float> cSliderModelValues;

extern std::vector<bool> cButtonStates;

extern std::vector<int> cComboBoxStates;

void printSliderValues(void);
bool getButtonState(String name);
void setButtonState(String name, bool on);
int getComboBoxState(String name);
void setComboBoxState(String name, int idx);
void setSliderModelValue(String name, float val);
void setSliderValue(String name, float val);
float getSliderValue(String name);
float randomNumberGenerator(void);

#endif  // UTILITIES_H_INCLUDED
