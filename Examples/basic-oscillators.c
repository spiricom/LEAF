/*
  ==============================================================================

    basic-oscillators.c
    Created: 5 Mar 2020 2:56:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "basic-oscillators.c"

void exampleInit()
{
    LEAF_init(44100, 64, mempool, 1000, &exampleRandom);
    
    tCycle_init(&cycle);
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

