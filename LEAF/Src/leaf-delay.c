/*==============================================================================

    leaf-delay.c
    Created: 20 Jan 2017 12:01:24pm
    Author:  Michael R Mulshine

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-delay.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-delay.h"
#include "../leaf.h"

#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Delay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tDelay_init (tDelay*  const dl, uint32_t delay, uint32_t maxDelay)
{
    _tDelay* d = *dl = (_tDelay*) leaf_alloc(sizeof(_tDelay));
    
    d->maxDelay = maxDelay;
    
    d->delay = delay;
    
    d->buff = (float*) leaf_alloc(sizeof(float) * maxDelay);
    
    d->inPoint = 0;
    d->outPoint = 0;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;
    
    d->gain = 1.0f;
    
    tDelay_setDelay(dl, d->delay);
}

void tDelay_free(tDelay* const dl)
{
    _tDelay* d = *dl;
    
    leaf_free(d->buff);
    leaf_free(d);
}

float   tDelay_tick (tDelay* const dl, float input)
{
    _tDelay* d = *dl;
    
    // Input
    d->lastIn = input;
    d->buff[d->inPoint] = input * d->gain;
    if (++(d->inPoint) == d->maxDelay)     d->inPoint = 0;
    
    // Output
    d->lastOut = d->buff[d->outPoint];
    if (++(d->outPoint) == d->maxDelay)    d->outPoint = 0;
    
    return d->lastOut;
}

int     tDelay_setDelay (tDelay* const dl, uint32_t delay)
{
    _tDelay* d = *dl;
    
    d->delay = LEAF_clip(0.0f, delay,  d->maxDelay);
    
    // read chases write
    if ( d->inPoint >= delay )  d->outPoint = d->inPoint - d->delay;
    else                        d->outPoint = d->maxDelay + d->inPoint - d->delay;
    
    return 0;
}

float tDelay_tapOut (tDelay* const dl, uint32_t tapDelay)
{
    _tDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return d->buff[tap];
    
}

void tDelay_tapIn (tDelay* const dl, float value, uint32_t tapDelay)
{
    _tDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tDelay_addTo (tDelay* const dl, float value, uint32_t tapDelay)
{
    _tDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

uint32_t   tDelay_getDelay (tDelay* const dl)
{
    _tDelay* d = *dl;
    return d->delay;
}

float   tDelay_getLastOut (tDelay* const dl)
{
    _tDelay* d = *dl;
    return d->lastOut;
}

float   tDelay_getLastIn (tDelay* const dl)
{
    _tDelay* d = *dl;
    return d->lastIn;
}

void tDelay_setGain (tDelay* const dl, float gain)
{
    _tDelay* d = *dl;
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tDelay_getGain (tDelay* const dl)
{
    _tDelay* d = *dl;
    return d->gain;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ LinearDelay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void   tLinearDelay_init (tLinearDelay* const dl, float delay, uint32_t maxDelay)
{
    _tLinearDelay* d = *dl = (_tLinearDelay*) leaf_alloc(sizeof(_tLinearDelay));
    
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
    
    tLinearDelay_setDelay(dl, d->delay);
}

void tLinearDelay_free(tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    
    leaf_free(d->buff);
    leaf_free(d);
}

float   tLinearDelay_tick (tLinearDelay* const dl, float input)
{
    _tLinearDelay* d = *dl;
    
    d->buff[d->inPoint] = input * d->gain;
    
    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;

    
    uint32_t idx = (uint32_t) d->outPoint;
    
    d->lastOut =    LEAF_interpolate_hermite (d->buff[((idx - 1) + d->maxDelay) % d->maxDelay],
                                              d->buff[idx],
                                              d->buff[(idx + 1) % d->maxDelay],
                                              d->buff[(idx + 2) % d->maxDelay],
                                              d->alpha);
    
    // Increment output pointer modulo length
    if ( (++d->outPoint) >= d->maxDelay )   d->outPoint = 0;
    
    return d->lastOut;
}

void   tLinearDelay_tickIn (tLinearDelay* const dl, float input)
{
    _tLinearDelay* d = *dl;
    
    d->buff[d->inPoint] = input * d->gain;

    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;
}

float   tLinearDelay_tickOut (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    
    uint32_t idx = (uint32_t) d->outPoint;

    d->lastOut =    LEAF_interpolate_hermite (d->buff[((idx - 1) + d->maxDelay) % d->maxDelay],
                                              d->buff[idx],
                                              d->buff[(idx + 1) % d->maxDelay],
                                              d->buff[(idx + 2) % d->maxDelay],
                                              d->alpha);

    // Increment output pointer modulo length
    if ( (++d->outPoint) >= d->maxDelay )   d->outPoint = 0;

    return d->lastOut;
}

int     tLinearDelay_setDelay (tLinearDelay* const dl, float delay)
{
    _tLinearDelay* d = *dl;
    
    d->delay = LEAF_clip(0.0f, delay,  d->maxDelay);
    
    float outPointer = d->inPoint - d->delay;
    
    while ( outPointer < 0 )
        outPointer += d->maxDelay; // modulo maximum length
    
    d->outPoint = (uint32_t) outPointer;   // integer part
    
    d->alpha = outPointer - d->outPoint; // fractional part
    d->omAlpha = 1.0f - d->alpha;
    
    if ( d->outPoint == d->maxDelay ) d->outPoint = 0;
    
    return 0;
}

float tLinearDelay_tapOut (tLinearDelay* const dl, float tapDelay)
{
    _tLinearDelay* d = *dl;
    
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

void tLinearDelay_tapIn (tLinearDelay* const dl, float value, uint32_t tapDelay)
{
    _tLinearDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tLinearDelay_addTo (tLinearDelay* const dl, float value, uint32_t tapDelay)
{
    _tLinearDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

float   tLinearDelay_getDelay (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    return d->delay;
}

float   tLinearDelay_getLastOut (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    return d->lastOut;
}

float   tLinearDelay_getLastIn (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    return d->lastIn;
}

void tLinearDelay_setGain (tLinearDelay* const dl, float gain)
{
    _tLinearDelay* d = *dl;
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tLinearDelay_getGain (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    return d->gain;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ AllpassDelay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void  tAllpassDelay_init (tAllpassDelay* const dl, float delay, uint32_t maxDelay)
{
    _tAllpassDelay* d = *dl = (_tAllpassDelay*) leaf_alloc(sizeof(_tAllpassDelay));
    
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
    
    tAllpassDelay_setDelay(dl, d->delay);
    
    d->apInput = 0.0f;
}

void tAllpassDelay_free(tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    
    leaf_free(d->buff);
    leaf_free(d);
}

float   tAllpassDelay_tick (tAllpassDelay* const dl, float input)
{
    _tAllpassDelay* d = *dl;
    
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

int     tAllpassDelay_setDelay (tAllpassDelay* const dl, float delay)
{
    _tAllpassDelay* d = *dl;
    
    d->delay = LEAF_clip(0.5f, delay,  d->maxDelay);
    
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

float tAllpassDelay_tapOut (tAllpassDelay* const dl, uint32_t tapDelay)
{
    _tAllpassDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return d->buff[tap];
    
}

void tAllpassDelay_tapIn (tAllpassDelay* const dl, float value, uint32_t tapDelay)
{
    _tAllpassDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tAllpassDelay_addTo (tAllpassDelay* const dl, float value, uint32_t tapDelay)
{
    _tAllpassDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

float   tAllpassDelay_getDelay (tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    return d->delay;
}

float   tAllpassDelay_getLastOut (tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    return d->lastOut;
}

float   tAllpassDelay_getLastIn (tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    return d->lastIn;
}

void tAllpassDelay_setGain (tAllpassDelay* const dl, float gain)
{
    _tAllpassDelay* d = *dl;
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tAllpassDelay_getGain (tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    return d->gain;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TapeDelay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void   tTapeDelay_init (tTapeDelay* const dl, float delay, uint32_t maxDelay)
{
    _tTapeDelay* d = *dl = (_tTapeDelay*) leaf_alloc(sizeof(_tTapeDelay));
    
    d->maxDelay = maxDelay;

    d->buff = (float*) leaf_alloc(sizeof(float) * maxDelay);
    
    d->gain = 1.0f;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;
    
    d->idx = 0.0f;
    d->inc = 1.0f;
    d->inPoint = 0;
    
    tTapeDelay_setDelay(dl, delay);
}

void tTapeDelay_free(tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    
    leaf_free(d->buff);
    leaf_free(d);
}

//#define SMOOTH_FACTOR 10.f

float   tTapeDelay_tick (tTapeDelay* const dl, float input)
{
    _tTapeDelay* d = *dl;
    
    d->buff[d->inPoint] = input * d->gain;
    
    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;

    int idx =  (int) d->idx;
    float alpha = d->idx - idx;
    
    d->lastOut =    LEAF_interpolate_hermite (d->buff[((idx - 1) + d->maxDelay) % d->maxDelay],
                                              d->buff[idx],
                                              d->buff[(idx + 1) % d->maxDelay],
                                              d->buff[(idx + 2) % d->maxDelay],
                                              alpha);
    
    float diff = (d->inPoint - d->idx);
    while (diff < 0.f) diff += d->maxDelay;
    
    d->inc = 1.0f + (diff - d->delay) / d->delay; //* SMOOTH_FACTOR;

    d->idx += d->inc;
    
    if (d->idx >= d->maxDelay) d->idx = 0.0f;

    return d->lastOut;
}

void  tTapeDelay_incrementInPoint(tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;
}


void tTapeDelay_setRate(tTapeDelay* const dl, float rate)
{
    _tTapeDelay* d = *dl;
    d->inc = rate;
}

void     tTapeDelay_setDelay (tTapeDelay* const dl, float delay)
{
    _tTapeDelay* d = *dl;
    d->delay = LEAF_clip(1.f, delay,  d->maxDelay);
}

float tTapeDelay_tapOut (tTapeDelay* const dl, float tapDelay)
{
    _tTapeDelay* d = *dl;
    
    float tap = (float) d->inPoint - tapDelay - 1.f;
    
    // Check for wraparound.
    while ( tap < 0.f )   tap += (float)d->maxDelay;
    
    int idx =  (int) tap;
    
    float alpha = tap - idx;
    
    float samp =    LEAF_interpolate_hermite (d->buff[((idx - 1) + d->maxDelay) % d->maxDelay],
                                              d->buff[idx],
                                              d->buff[(idx + 1) % d->maxDelay],
                                              d->buff[(idx + 2) % d->maxDelay],
                                              alpha);
    
    return samp;
    
}

void tTapeDelay_tapIn (tTapeDelay* const dl, float value, uint32_t tapDelay)
{
    _tTapeDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tTapeDelay_addTo (tTapeDelay* const dl, float value, uint32_t tapDelay)
{
    _tTapeDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

float   tTapeDelay_getDelay (tTapeDelay *dl)
{
    _tTapeDelay* d = *dl;
    return d->delay;
}

float   tTapeDelay_getLastOut (tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    return d->lastOut;
}

float   tTapeDelay_getLastIn (tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    return d->lastIn;
}

void tTapeDelay_setGain (tTapeDelay* const dl, float gain)
{
    _tTapeDelay* d = *dl;
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tTapeDelay_getGain (tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    return d->gain;
}

