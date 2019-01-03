/*
  ==============================================================================

    LEAFDelay.c
    Created: 20 Jan 2017 12:01:24pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-delay.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-delay.h"
#include "../leaf.h"

#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Delay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tDelay_init (tDelay*  const d, uint32_t delay, uint32_t maxDelay)
{
    d->maxDelay = maxDelay;
    
    d->delay = delay;
    
    d->buff = (float*) leaf_alloc(sizeof(float) * maxDelay);
    
    d->inPoint = 0;
    d->outPoint = 0;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;
    
    d->gain = 1.0f;
    
    tDelay_setDelay(d, d->delay);
}

void tDelay_free(tDelay* const d)
{
    leaf_free(d->buff);
    leaf_free(d);
}

float   tDelay_tick (tDelay* const d, float input)
{
    // Input
    d->lastIn = input;
    d->buff[d->inPoint] = input * d->gain;
    if (++(d->inPoint) == d->maxDelay)     d->inPoint = 0;
    
    // Output
    d->lastOut = d->buff[d->outPoint];
    if (++(d->outPoint) == d->maxDelay)    d->outPoint = 0;
    
    return d->lastOut;
}


int     tDelay_setDelay (tDelay* const d, uint32_t delay)
{
    if (delay >= d->maxDelay)    d->delay = d->maxDelay;
    else                         d->delay = delay;
    
    // read chases write
    if ( d->inPoint >= delay )  d->outPoint = d->inPoint - d->delay;
    else                        d->outPoint = d->maxDelay + d->inPoint - d->delay;
    
    return 0;
}

float tDelay_tapOut (tDelay* const d, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return d->buff[tap];
    
}

void tDelay_tapIn (tDelay* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tDelay_addTo (tDelay* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

uint32_t   tDelay_getDelay (tDelay* const d)
{
    return d->delay;
}

float   tDelay_getLastOut (tDelay* const d)
{
    return d->lastOut;
}

float   tDelay_getLastIn (tDelay* const d)
{
    return d->lastIn;
}

void tDelay_setGain (tDelay* const d, float gain)
{
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tDelay_getGain (tDelay* const d)
{
    return d->gain;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DelayL ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void   tDelayL_init (tDelayL* const d, float delay, uint32_t maxDelay)
{
    d->maxDelay = maxDelay;

    if (delay > maxDelay)   d->delay = maxDelay;
    else if (delay < 0.0f)  d->delay = 0.0f;
    else                    d->delay = delay;
    
    d->buff = (float*) leaf_alloc(sizeof(float) * maxDelay);
    
    d->gain = 1.0f;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;

    d->inPoint = 0;
    d->outPoint = 0;
    
    tDelayL_setDelay(d, d->delay);
}

void tDelayL_free(tDelayL* const d)
{
    leaf_free(d->buff);
    leaf_free(d);
}

float   tDelayL_tick (tDelayL* const d, float input)
{
    d->buff[d->inPoint] = input * d->gain;
    
    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;
    
    
    // First 1/2 of interpolation
    d->lastOut = d->buff[d->outPoint] * d->omAlpha;
    
    // Second 1/2 of interpolation
    if (d->outPoint + 1 < d->maxDelay)
        d->lastOut += d->buff[d->outPoint+1] * d->alpha;
    else
        d->lastOut += d->buff[0] * d->alpha;
    
    // Increment output pointer modulo length.
    if ( ++(d->outPoint) == d->maxDelay )   d->outPoint = 0;
    
    return d->lastOut;
}

int     tDelayL_setDelay (tDelayL* const d, float delay)
{
    if (delay < 0.0f)               d->delay = 0.0f;
    else if (delay <= d->maxDelay)  d->delay = delay;
    else                            d->delay = d->maxDelay;
    
    float outPointer = d->inPoint - d->delay;
    
    while ( outPointer < 0 )
        outPointer += d->maxDelay; // modulo maximum length
    
    d->outPoint = (uint32_t) outPointer;   // integer part
    
    d->alpha = outPointer - d->outPoint; // fractional part
    d->omAlpha = 1.0f - d->alpha;
    
    if ( d->outPoint == d->maxDelay ) d->outPoint = 0;
    
    return 0;
}

float tDelayL_tapOut (tDelayL* const d, float tapDelay)
{
    float tap = (float) d->inPoint - tapDelay - 1.f;
    
    // Check for wraparound.
    while ( tap < 0.f )   tap += (float)d->maxDelay;

    float alpha = tap - (int)tap;
    float omAlpha = 1.f - alpha;

    int ptx = (int) tap;
    
    // First 1/2 of interpolation
    float samp = d->buff[ptx] * omAlpha;
    
    // Second 1/2 of interpolation
    if ((ptx + 1) < d->maxDelay)
        samp += d->buff[ptx+1] * d->alpha;
    else
        samp += d->buff[0] * d->alpha;
    
    return samp;
    
}

void tDelayL_tapIn (tDelayL* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tDelayL_addTo (tDelayL* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

float   tDelayL_getDelay (tDelayL *d)
{
    return d->delay;
}

float   tDelayL_getLastOut (tDelayL* const d)
{
    return d->lastOut;
}

float   tDelayL_getLastIn (tDelayL* const d)
{
    return d->lastIn;
}

void tDelayL_setGain (tDelayL* const d, float gain)
{
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tDelayL_getGain (tDelayL* const d)
{
    return d->gain;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DelayA ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void  tDelayA_init (tDelayA* const d, float delay, uint32_t maxDelay)
{
    d->maxDelay = maxDelay;
    
    if (delay > maxDelay)   d->delay = maxDelay;
    else if (delay < 0.0f)  d->delay = 0.0f;
    else                    d->delay = delay;
    
    d->buff = (float*) leaf_alloc(sizeof(float) * maxDelay);
    
    d->gain = 1.0f;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;
    
    d->inPoint = 0;
    d->outPoint = 0;
    
    tDelayA_setDelay(d, d->delay);
    
    d->apInput = 0.0f;
}

void tDelayA_free(tDelayA* const d)
{
    leaf_free(d->buff);
    leaf_free(d);
}

float   tDelayA_tick (tDelayA* const d, float input)
{
    d->buff[d->inPoint] = input * d->gain;
    
    // Increment input pointer modulo length.
    if ( ++(d->inPoint) >= d->maxDelay )    d->inPoint = 0;
    
    // Do allpass interpolation delay.
    float out = d->lastOut * -d->coeff;
    out += d->apInput + ( d->coeff * d->buff[d->outPoint] );
    d->lastOut = out;
    
    // Save allpass input
    d->apInput = d->buff[d->outPoint];
    
    // Increment output pointer modulo length.
    if (++(d->outPoint) >= d->maxDelay )   d->outPoint = 0;
    
    return d->lastOut;
}

int     tDelayA_setDelay (tDelayA* const d, float delay)
{
    if (delay < 0.5f)               d->delay = 0.5f;
    else if (delay <= d->maxDelay)  d->delay = delay;
    else                            d->delay = d->maxDelay;
    
    // outPoint chases inPoint
    float outPointer = (float)d->inPoint - d->delay + 1.0f;
    
    while ( outPointer < 0 )    outPointer += d->maxDelay;  // mod max length
    
    d->outPoint = (uint32_t) outPointer;         // integer part
    
    if ( d->outPoint >= d->maxDelay )   d->outPoint = 0;
    
    d->alpha = 1.0f + (float)d->outPoint - outPointer; // fractional part
    
    if ( d->alpha < 0.5f )
    {
        // The optimal range for alpha is about 0.5 - 1.5 in order to
        // achieve the flattest phase delay response.
        
        d->outPoint += 1;
        
        if ( d->outPoint >= d->maxDelay ) d->outPoint -= d->maxDelay;
        
        d->alpha += 1.0f;
    }
    
    d->coeff = (1.0f - d->alpha) / (1.0f + d->alpha);  // coefficient for allpass
    
    return 0;
}

float tDelayA_tapOut (tDelayA* const d, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return d->buff[tap];
    
}

void tDelayA_tapIn (tDelayA* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tDelayA_addTo (tDelayA* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

float   tDelayA_getDelay (tDelayA* const d)
{
    return d->delay;
}

float   tDelayA_getLastOut (tDelayA* const d)
{
    return d->lastOut;
}

float   tDelayA_getLastIn (tDelayA* const d)
{
    return d->lastIn;
}

void tDelayA_setGain (tDelayA* const d, float gain)
{
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tDelayA_getGain (tDelayA* const d)
{
    return d->gain;
}

