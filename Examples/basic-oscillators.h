/*
  ==============================================================================

    basic-oscillators.h
    Created: 5 Mar 2020 2:56:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "../leaf/leaf.h"

char mempool[1000];
tCycle cycle;

void    exampleInit(void);

void    exampleFrame(void);

float   exampleTick(float sampleIn);

float   exampleRandom(void);
