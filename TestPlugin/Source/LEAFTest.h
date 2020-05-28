/*
  ==============================================================================

    OOPSTest.h
    Created: 4 Dec 2016 9:14:16pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LEAFTEST1_H_INCLUDED
#define LEAFTEST1_H_INCLUDED

#include "LEAFLink.h"

#include "../../leaf/leaf.h"

#include "Yin.h"

// LEAFTest API
float   LEAFTest_tick            (float input);
void    LEAFTest_init            (float sampleRate, int blocksize);
void    LEAFTest_end             (void);
void    LEAFTest_block           (void);

void    LEAFTest_noteOn          (int midiNoteNumber, float velocity);
void    LEAFTest_noteOff         (int midiNoteNumber);

#endif  // LEAFTEST1_H_INCLUDED
