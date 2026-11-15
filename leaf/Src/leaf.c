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

void LEAF_init(LEAF* const leaf, float sr, char* memory, size_t memorysize)
{
    leaf->_internal_mempool.leaf = leaf;
    leaf_pool_init(leaf, memory, memorysize);

    leaf->sampleRate = sr;

    leaf->invSampleRate = 1.0f/sr;

    leaf->twoPiTimesInvSampleRate = leaf->invSampleRate * TWO_PI;

    leaf->random = LEAF_defaultRandomNumber;

    leaf->clearOnAllocation = 0;

    leaf->errorCallback = &LEAF_defaultErrorCallback;

    for (int i = 0; i < LEAFErrorNil; ++i)
        leaf->errorState[i] = 0;

    leaf->allocCount = 0;

    leaf->freeCount = 0;
    leaf->uuid = 0;
    leaf->lfoRateTable = NULL;
    leaf->envTimeTable = NULL;
    leaf->resTable = NULL;

}

void LEAF_setSampleRate(LEAF* const leaf, float sampleRate)
{
    leaf->sampleRate = sampleRate;
    leaf->invSampleRate = 1.0f/sampleRate;
    leaf->twoPiTimesInvSampleRate = leaf->invSampleRate * TWO_PI;
}

float LEAF_getSampleRate(LEAF* const leaf)
{
    return leaf->sampleRate;
}

static uint32_t _leaf_lcg_state = 1664525u;
float LEAF_defaultRandomNumber(void) {
    _leaf_lcg_state = _leaf_lcg_state * 1664525u + 1013904223u;
    return (_leaf_lcg_state >> 8) * 0.00000006f;
}

void LEAF_defaultErrorCallback(LEAF* const leaf, LEAFErrorType whichone)
{
    // Not sure what this should do if anything
    // Maybe fine as a placeholder
}

void LEAF_internalErrorCallback(LEAF* const leaf, LEAFErrorType whichone)
{
    leaf->errorState[whichone] = 1;
    leaf->errorCallback(leaf, whichone);
}

void LEAF_setErrorCallback(LEAF* const leaf, void (*callback)(LEAF* const, LEAFErrorType))
{
    leaf->errorCallback = callback;
}

unsigned int getNextUuid(LEAF* leaf)
{
    return ++leaf->uuid;
}
