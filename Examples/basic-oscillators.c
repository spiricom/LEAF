/*
  ==============================================================================

    basic-oscillators.c
    Created: 5 Mar 2020 2:56:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "basic-oscillators.h"

void exampleInit()
{
    LEAF leaf;
    
    LEAF_init(&leaf, 44100, 128, mempool, 1000, &exampleRandom);
    
    tCycle_init(&cycle, &leaf);
    tCycle_setFreq(&cycle, 220);
}

void exampleFrame()
{
    
}

float exampleTick(float sampleIn)
{
    float sampleOut = tCycle_tick(&cycle);
    return sampleOut;
}

float exampleRandom()
{
    return ((float)rand()/(float)(RAND_MAX));
}

