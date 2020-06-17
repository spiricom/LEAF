/*
  ==============================================================================

    LEAFCore.c
    Created: 20 Jan 2017 12:08:14pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#if _WIN32 || _WIN64

#include "..\leaf.h"

#else

#include "../leaf.h"

#endif

LEAF leaf;

void LEAF_init(float sr, int blocksize, char* memory, size_t memorysize, float(*random)(void))

{
    leaf_pool_init(memory, memorysize);
    
    leaf.sampleRate = sr;

    leaf.blockSize = blocksize;
    
    leaf.invSampleRate = 1.0f/sr;
    
    leaf.twoPiTimesInvSampleRate = leaf.invSampleRate * TWO_PI;

    leaf.random = random;
    
    leaf.clearOnAllocation = 0;
}


#define LEAFSampleRateChanged(THIS) leaf.THIS.sampleRateChanged(&leaf.THIS)

void LEAF_setSampleRate(float sampleRate)
{
    leaf.sampleRate = sampleRate;
    leaf.invSampleRate = 1.0f/sampleRate;
}

float LEAF_getSampleRate()
{
    return leaf.sampleRate;
}


//implement a function called this in your user code to catch errors
//__attribute__((weak))
uint8_t LEAF_error(uint8_t whichone)
{
    return whichone;
}
