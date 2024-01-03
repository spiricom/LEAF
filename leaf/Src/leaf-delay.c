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
void    tDelay_init (tDelay* const dl, uint32_t delay, uint32_t maxDelay, LEAF* const leaf)
{
    tDelay_initToPool(dl, delay, maxDelay, &leaf->mempool);
}

void    tDelay_initToPool   (tDelay* const dl, uint32_t delay, uint32_t maxDelay, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tDelay* d = *dl = (_tDelay*) mpool_alloc(sizeof(_tDelay), m);
    d->mempool = m;

    d->maxDelay = maxDelay;

    d->delay = delay;

    d->buff = (Lfloat*) mpool_alloc(sizeof(Lfloat) * maxDelay, m);
    
    d->inPoint = 0;
    d->outPoint = 0;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;
    
    d->gain = 1.0f;
    
    tDelay_setDelay(dl, d->delay);
}

void tDelay_free (tDelay* const dl)
{
    _tDelay* d = *dl;
    
    mpool_free((char*)d->buff, d->mempool);
    mpool_free((char*)d, d->mempool);
}

void    tDelay_clear(tDelay* const dl)
{
    _tDelay* d = *dl;
    for (unsigned i = 0; i < d->maxDelay; i++)
    {
        d->buff[i] = 0;
    }
}

Lfloat   tDelay_tick (tDelay* const dl, Lfloat input)
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

void     tDelay_setDelay (tDelay* const dl, uint32_t delay)
{
    _tDelay* d = *dl;

    d->delay = LEAF_clip(0.0f, delay,  d->maxDelay);

    // read chases write
    if ( d->inPoint >= delay )  d->outPoint = d->inPoint - d->delay;
    else                        d->outPoint = d->maxDelay + d->inPoint - d->delay;
}

Lfloat tDelay_tapOut (tDelay* const dl, uint32_t tapDelay)
{
    _tDelay* d = *dl;

    int32_t tap = d->inPoint - tapDelay - 1;

    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;

    return d->buff[tap];

}

void tDelay_tapIn (tDelay* const dl, Lfloat value, uint32_t tapDelay)
{
    _tDelay* d = *dl;

    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

Lfloat tDelay_addTo (tDelay* const dl, Lfloat value, uint32_t tapDelay)
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

Lfloat   tDelay_getLastOut (tDelay* const dl)
{
    _tDelay* d = *dl;
    return d->lastOut;
}

Lfloat   tDelay_getLastIn (tDelay* const dl)
{
    _tDelay* d = *dl;
    return d->lastIn;
}

void tDelay_setGain (tDelay* const dl, Lfloat gain)
{
    _tDelay* d = *dl;
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

Lfloat tDelay_getGain (tDelay* const dl)
{
    _tDelay* d = *dl;
    return d->gain;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ LinearDelay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void   tLinearDelay_init (tLinearDelay* const dl, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
{
    tLinearDelay_initToPool(dl, delay, maxDelay, &leaf->mempool);
}

void tLinearDelay_initToPool  (tLinearDelay* const dl, Lfloat delay, uint32_t maxDelay, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tLinearDelay* d = *dl = (_tLinearDelay*) mpool_alloc(sizeof(_tLinearDelay), m);
    d->mempool = m;

    d->maxDelay = maxDelay;

    if (delay > maxDelay)   d->delay = maxDelay;
    else if (delay < 0.0f)  d->delay = 0.0f;
    else                    d->delay = delay;

    d->buff = (Lfloat*) mpool_alloc(sizeof(Lfloat) * maxDelay, m);

    d->gain = 1.0f;

    d->lastIn = 0.0f;
    d->lastOut = 0.0f;
    
    d->inPoint = 0;
    d->outPoint = 0;
    
    tLinearDelay_setDelay(dl, d->delay);
}

void tLinearDelay_free (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    
    mpool_free((char*)d->buff, d->mempool);
    mpool_free((char*)d, d->mempool);
}

void    tLinearDelay_clear(tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    for (unsigned i = 0; i < d->maxDelay; i++)
    {
        d->buff[i] = 0;
    }
}

Lfloat   tLinearDelay_tick (tLinearDelay* const dl, Lfloat input)
{
    _tLinearDelay* d = *dl;

    d->buff[d->inPoint] = input * d->gain;

    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;

    uint32_t idx = (uint32_t) d->outPoint;
    // First 1/2 of interpolation
    d->lastOut = d->buff[idx] * d->omAlpha;
        // Second 1/2 of interpolation
    if ((idx + 1) < d->maxDelay)
        d->lastOut += d->buff[idx+1] * d->alpha;
    else
        d->lastOut += d->buff[0] * d->alpha;

    // Increment output pointer modulo length
    if ( (++d->outPoint) >= d->maxDelay )   d->outPoint = 0;

    return d->lastOut;
}

void   tLinearDelay_tickIn (tLinearDelay* const dl, Lfloat input)
{
    _tLinearDelay* d = *dl;

    d->buff[d->inPoint] = input * d->gain;

    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;
}

Lfloat   tLinearDelay_tickOut (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;

    uint32_t idx = (uint32_t) d->outPoint;
    // First 1/2 of interpolation
    d->lastOut = d->buff[idx] * d->omAlpha;
        // Second 1/2 of interpolation
        
    if ((idx + 1) < d->maxDelay)
        d->lastOut += d->buff[idx+1] * d->alpha;
    else
        d->lastOut += d->buff[0] * d->alpha;

    // Increment output pointer modulo length
    if ( (++d->outPoint) >= d->maxDelay )   d->outPoint = 0;

    return d->lastOut;
}

void     tLinearDelay_setDelay (tLinearDelay* const dl, Lfloat delay)
{
    _tLinearDelay* d = *dl;

    d->delay = LEAF_clip(2.0f, delay,  d->maxDelay);

    Lfloat outPointer = d->inPoint - d->delay;

    while ( outPointer < 0 )
        outPointer += d->maxDelay; // modulo maximum length

    d->outPoint = (uint32_t) outPointer;   // integer part

    d->alpha = outPointer - d->outPoint; // fractional part
    d->omAlpha = 1.0f - d->alpha;

    if ( d->outPoint == d->maxDelay ) d->outPoint = 0;
}

Lfloat tLinearDelay_tapOut (tLinearDelay* const dl, uint32_t tapDelay)
{
    _tLinearDelay* d = *dl;

    int32_t tap = d->inPoint - tapDelay - 1;
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;

    return d->buff[tap];
}

Lfloat tLinearDelay_tapOutInterpolated (tLinearDelay* const dl, Lfloat tapDelay)
{
    _tLinearDelay* d = *dl;
   
    Lfloat tap = (float)d->inPoint - tapDelay - 1.0f;
    while ( tap < 0.0f )   tap += d->maxDelay;
    uint32_t tapInt = (uint32_t)tap;
    Lfloat alpha = tap - (Lfloat)tapInt;

    uint32_t tapIntPlusOne = tapInt + 1;
    if (tapIntPlusOne > d->maxDelay)
    {
        tapIntPlusOne = 0;
    }
        // First 1/2 of interpolation
    Lfloat tapOut = d->buff[tapInt] * (1.0f - alpha);
        // Second 1/2 of interpolation
    tapOut += d->buff[tapIntPlusOne] * alpha;
   

    return tapOut;
}

void tLinearDelay_tapIn (tLinearDelay* const dl, Lfloat value, uint32_t tapDelay)
{
    _tLinearDelay* d = *dl;

    int32_t tap = d->inPoint - tapDelay - 1;

    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;

    d->buff[tap] = value;
}

Lfloat tLinearDelay_addTo (tLinearDelay* const dl, Lfloat value, uint32_t tapDelay)
{
    _tLinearDelay* d = *dl;

    int32_t tap = d->inPoint - tapDelay - 1;

    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

Lfloat   tLinearDelay_getDelay (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    return d->delay;
}

Lfloat   tLinearDelay_getLastOut (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    return d->lastOut;
}

Lfloat   tLinearDelay_getLastIn (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    return d->lastIn;
}

void tLinearDelay_setGain (tLinearDelay* const dl, Lfloat gain)
{
    _tLinearDelay* d = *dl;
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

Lfloat tLinearDelay_getGain (tLinearDelay* const dl)
{
    _tLinearDelay* d = *dl;
    return d->gain;
}


/// Hermite Interpolated Delay
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ LinearDelay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void tHermiteDelay_init (tHermiteDelay* const dl, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
{
    tHermiteDelay_initToPool(dl, delay, maxDelay, &leaf->mempool);
}

void tHermiteDelay_initToPool  (tHermiteDelay* const dl, Lfloat delay, uint32_t maxDelay, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tHermiteDelay* d = *dl = (_tHermiteDelay*) mpool_alloc(sizeof(_tHermiteDelay), m);
    d->mempool = m;

    d->maxDelay = maxDelay;

    if (delay > maxDelay)   d->delay = maxDelay;
    else if (delay < 0.0f)  d->delay = 0.0f;
    else                    d->delay = delay;

    
    if ((maxDelay != 0) && ((maxDelay & (maxDelay - 1)) == 0))
    {
        d->maxDelay = maxDelay;
        d->bufferMask = maxDelay - 1;
    }
    else
    {
        //make the delay size into a power of 2
        maxDelay--;
        maxDelay |= maxDelay >> 1;
        maxDelay |= maxDelay >> 2;
        maxDelay |= maxDelay >> 4;
        maxDelay |= maxDelay >> 8;
        maxDelay |= maxDelay >> 16;
        maxDelay++;
        d->maxDelay = maxDelay;
        d->bufferMask = maxDelay - 1;
    }
    d->buff = (Lfloat*) mpool_alloc(sizeof(Lfloat) * maxDelay, m);

    d->gain = 1.0f;

    d->lastIn = 0.0f;
    d->lastOut = 0.0f;

    d->inPoint = 0;
    d->outPoint = 0;

    tHermiteDelay_setDelay(dl, d->delay);
}

void tHermiteDelay_free (tHermiteDelay* const dl)
{
    _tHermiteDelay* d = *dl;

    mpool_free((char*)d->buff, d->mempool);
    mpool_free((char*)d, d->mempool);
}


void    tHermiteDelay_clear(tHermiteDelay* const dl)
{
    _tHermiteDelay* d = *dl;
    for (unsigned i = 0; i < d->maxDelay; i++)
    {
        d->buff[i] = 0;
    }
}

Lfloat   tHermiteDelay_tick (tHermiteDelay* const dl, Lfloat input)
{
    _tHermiteDelay* d = *dl;

    d->buff[d->inPoint] = input * d->gain;

    
    // Increment input pointer modulo length.
    d->inPoint = (d->inPoint + 1) & d->bufferMask;
    
    
    uint32_t idx = (uint32_t) d->outPoint;
    d->lastOut =    LEAF_interpolate_hermite_x (d->buff[((idx - 1) + d->maxDelay) & d->bufferMask],
                                                d->buff[idx],
                                                d->buff[(idx + 1) & d->bufferMask],
                                                d->buff[(idx + 2) & d->bufferMask],
                                                d->alpha);
    
    // Increment output pointer modulo length
    d->outPoint = (d->outPoint + 1) & d->bufferMask;

    return d->lastOut;
}

void   tHermiteDelay_tickIn (tHermiteDelay* const dl, Lfloat input)
{
    _tHermiteDelay* d = *dl;
    
    d->buff[d->inPoint] = input;
    
    // Increment input pointer modulo length.
    d->inPoint = (d->inPoint + 1) & d->bufferMask;
}

Lfloat   tHermiteDelay_tickOut (tHermiteDelay* const dl)
{
    _tHermiteDelay* d = *dl;
    
    uint32_t idx = (uint32_t) d->outPoint;
    
    
    
    d->lastOut =    LEAF_interpolate_hermite_x (d->buff[((idx - 1) + d->maxDelay) & d->bufferMask],
                                                d->buff[idx],
                                                d->buff[(idx + 1) & d->bufferMask],
                                                d->buff[(idx + 2) & d->bufferMask],
                                                d->alpha);
    
    // Increment output pointer modulo length
    d->outPoint = (d->outPoint + 1) & d->bufferMask;
    
    return d->lastOut;
}

void tHermiteDelay_setDelay (tHermiteDelay* const dl, Lfloat delay)
{
    _tHermiteDelay* d = *dl;
    //d->delay = LEAF_clip(0.0f, delay,  d->maxDelay);
    d->delay = delay; // not safe but faster
    Lfloat outPointer = d->inPoint - d->delay;
    while ( outPointer < 0 )
        outPointer += d->maxDelay; // modulo maximum length
    
    d->outPoint = (uint32_t) outPointer;   // integer part
    
    d->alpha = outPointer - d->outPoint; // fractional part
    d->omAlpha = 1.0f - d->alpha;
    
    d->outPoint &= d->bufferMask;
}

Lfloat tHermiteDelay_tapOut (tHermiteDelay* const dl, uint32_t tapDelay)
{
    _tHermiteDelay* d = *dl;
    
    int32_t tap = (d->inPoint - tapDelay - 1) & d->bufferMask;
    
    return d->buff[tap];

}

Lfloat   tHermiteDelay_tapOutInterpolated (tHermiteDelay* const dl, uint32_t tapDelay, Lfloat alpha)
{
    _tHermiteDelay* d = *dl;
    
    int32_t idx = (d->inPoint - tapDelay - 1) & d->bufferMask;
    
    return    LEAF_interpolate_hermite_x (d->buff[((idx - 1) + d->maxDelay) & d->bufferMask],
                                          d->buff[idx],
                                          d->buff[(idx + 1) & d->bufferMask],
                                          d->buff[(idx + 2) & d->bufferMask],
                                          alpha);
}

void tHermiteDelay_tapIn (tHermiteDelay* const dl, Lfloat value, uint32_t tapDelay)
{
    _tHermiteDelay* d = *dl;
    
    int32_t tap = (d->inPoint - tapDelay - 1)  & d->bufferMask;
    
    d->buff[tap] = value;
}

Lfloat tHermiteDelay_addTo (tHermiteDelay* const dl, Lfloat value, uint32_t tapDelay)
{
    _tHermiteDelay* d = *dl;
    
    int32_t tap = (d->inPoint - tapDelay - 1)  & d->bufferMask;
    
    return (d->buff[tap] += value);
}

Lfloat   tHermiteDelay_getDelay (tHermiteDelay* const dl)
{
    _tHermiteDelay* d = *dl;
    return d->delay;
}

Lfloat   tHermiteDelay_getLastOut (tHermiteDelay* const dl)
{
    _tHermiteDelay* d = *dl;
    return d->lastOut;
}

Lfloat   tHermiteDelay_getLastIn (tHermiteDelay* const dl)
{
    _tHermiteDelay* d = *dl;
    return d->lastIn;
}

void tHermiteDelay_setGain (tHermiteDelay* const dl, Lfloat gain)
{
    _tHermiteDelay* d = *dl;
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

Lfloat tHermiteDelay_getGain (tHermiteDelay* const dl)
{
    _tHermiteDelay* d = *dl;
    return d->gain;
}



/// Lagrange Interpolated Delay
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ LinearDelay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void tLagrangeDelay_init (tLagrangeDelay* const dl, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
{
	tLagrangeDelay_initToPool(dl, delay, maxDelay, &leaf->mempool);
}

void tLagrangeDelay_initToPool  (tLagrangeDelay* const dl, Lfloat delay, uint32_t maxDelay, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tLagrangeDelay* d = *dl = (_tLagrangeDelay*) mpool_alloc(sizeof(_tLagrangeDelay), m);
    d->mempool = m;

    d->maxDelay = maxDelay;

    if (delay > maxDelay)   d->delay = maxDelay;
    else if (delay < 0.0f)  d->delay = 0.0f;
    else                    d->delay = delay;


    if ((maxDelay != 0) && ((maxDelay & (maxDelay - 1)) == 0))
    {
        d->maxDelay = maxDelay;
        d->bufferMask = maxDelay - 1;
    }
    else
    {
        //make the delay size into a power of 2
        maxDelay--;
        maxDelay |= maxDelay >> 1;
        maxDelay |= maxDelay >> 2;
        maxDelay |= maxDelay >> 4;
        maxDelay |= maxDelay >> 8;
        maxDelay |= maxDelay >> 16;
        maxDelay++;
        d->maxDelay = maxDelay;
        d->bufferMask = maxDelay - 1;
    }
    d->buff = (Lfloat*) mpool_alloc(sizeof(Lfloat) * maxDelay, m);


    d->lastIn = 0.0f;
    d->lastOut = 0.0f;

    d->inPoint = 0;
    d->outPoint = 0;

    tLagrangeDelay_setDelay(dl, d->delay);
}

void tLagrangeDelay_free (tLagrangeDelay* const dl)
{
    _tLagrangeDelay* d = *dl;

    mpool_free((char*)d->buff, d->mempool);
    mpool_free((char*)d, d->mempool);
}


void    tLagrangeDelay_clear(tLagrangeDelay* const dl)
{
	_tLagrangeDelay* d = *dl;
    for (unsigned i = 0; i < d->maxDelay; i++)
    {
        d->buff[i] = 0.0f;
    }
}

Lfloat   tLagrangeDelay_tick (tLagrangeDelay* const dl, Lfloat input)
{
	_tLagrangeDelay* d = *dl;

    d->buff[d->inPoint] = input;


    // Increment input pointer modulo length.
    d->inPoint = (d->inPoint + 1) & d->bufferMask;


    uint32_t idx = (uint32_t) d->outPoint;
    uint32_t previdx =  ((idx - 1) + d->maxDelay) & d->bufferMask;
    d->lastOut =    (d->buff[previdx] * d->h0) +
    		(d->buff[idx] * d->h1) +
			(d->buff[(idx + 1) & d->bufferMask] * d->h2) +
			(d->buff[(idx + 2) & d->bufferMask] * d->h3);

    //clear the sample that is no longer used
   // d->buff[previdx] = 0.0f;
    // Increment output pointer modulo length
    d->outPoint = (d->outPoint + 1) & d->bufferMask;

    return d->lastOut;
}

void   tLagrangeDelay_tickIn (tLagrangeDelay* const dl, Lfloat input)
{
	_tLagrangeDelay* d = *dl;

    d->buff[d->inPoint] = input;

    // Increment input pointer modulo length.
    d->inPoint = (d->inPoint + 1) & d->bufferMask;
}

Lfloat   tLagrangeDelay_tickOut (tLagrangeDelay* const dl)
{
	_tLagrangeDelay* d = *dl;

    uint32_t idx = (uint32_t) d->outPoint;

   uint32_t previdx =  ((idx - 1) + d->maxDelay) & d->bufferMask;

    d->lastOut =    (d->buff[previdx] * d->h0) +
    		(d->buff[idx] * d->h1) +
			(d->buff[(idx + 1) & d->bufferMask] * d->h2) +
			(d->buff[(idx + 2) & d->bufferMask] * d->h3);

    //d->buff[previdx] = 0.0f;
    // Increment output pointer modulo length
    d->outPoint = (d->outPoint + 1) & d->bufferMask;

    return d->lastOut;
}

void tLagrangeDelay_setDelay (tLagrangeDelay* const dl, Lfloat delay)
{
	_tLagrangeDelay* d = *dl;
    //d->delay = LEAF_clip(0.0f, delay,  d->maxDelay);
    d->delay = delay; // not safe but faster
    Lfloat outPointer = d->inPoint - d->delay - 1.0f; // minus 1 because interpolation needs value from 1->2 so lagrange interp will add the 1 back
    while ( outPointer < 0 )
        outPointer += d->maxDelay; // modulo maximum length

    d->outPoint = (uint32_t) outPointer;   // integer part

    //Lfloat alpha = 1.0f - (outPointer - d->outPoint); // fractional part
    Lfloat alpha = (outPointer - d->outPoint); // fractional part

	Lfloat dp1 = alpha+1.0f;
	Lfloat dm1 = alpha-1.0f;
	Lfloat dm2 = alpha-2.0f;

	Lfloat pdp1d = dp1*alpha;
	Lfloat pddm1 = alpha*dm1;
	Lfloat pdm1dm2 = dm1*dm2;

	d->h0 = -0.166666666666667f * (pddm1 * dm2);
	d->h1 = 0.5f * (dp1 * pdm1dm2);
	d->h2 = -0.5f * (pdp1d * dm2);
	d->h3 = 0.166666666666667f * (pdp1d * dm1);

    d->outPoint &= d->bufferMask;
}

Lfloat tLagrangeDelay_tapOut (tLagrangeDelay* const dl, uint32_t tapDelay)
{
	_tLagrangeDelay* d = *dl;

    uint32_t tap = (d->inPoint - tapDelay - 1) & d->bufferMask;

    return d->buff[tap];

}

Lfloat   tLagrangeDelay_tapOutInterpolated (tLagrangeDelay* const dl, uint32_t tapDelay, Lfloat alpha)
{
	_tLagrangeDelay* d = *dl;

    uint32_t idx = ((d->inPoint - tapDelay - 2)) & d->bufferMask;

    alpha = 1.0f - alpha;
    Lfloat dp1 = alpha+1.0f;
    Lfloat dm1 = alpha-1.0f;
    Lfloat dm2 = alpha-2.0f;

    Lfloat pdp1d = dp1*alpha;
    Lfloat pddm1 = alpha*dm1;
    Lfloat pdm1dm2 = dm1*dm2;

    Lfloat h0 = -0.166666666666667f * (pddm1 * dm2);
    Lfloat h1 = 0.5f * (dp1 * pdm1dm2);
    Lfloat h2 = -0.5f * (pdp1d * dm2);
    Lfloat h3 = 0.166666666666667f * (pdp1d * dm1);
    
    return    (d->buff[((idx - 1) + d->maxDelay) & d->bufferMask] * h0) +
    		(d->buff[idx] * h1) +
			(d->buff[(idx + 1) & d->bufferMask] * h2) +
			(d->buff[(idx + 2) & d->bufferMask] * h3);

}

void tLagrangeDelay_tapIn (tLagrangeDelay* const dl, Lfloat value, uint32_t tapDelay)
{
	_tLagrangeDelay* d = *dl;

    int32_t tap = (d->inPoint - tapDelay - 1)  & d->bufferMask;

    d->buff[tap] = value;
}

Lfloat tLagrangeDelay_addTo (tLagrangeDelay* const dl, Lfloat value, uint32_t tapDelay)
{
	_tLagrangeDelay* d = *dl;

    int32_t tap = (d->inPoint - tapDelay - 1)  & d->bufferMask;

    return (d->buff[tap] += value);
}

Lfloat   tLagrangeDelay_getDelay (tLagrangeDelay* const dl)
{
	_tLagrangeDelay* d = *dl;
    return d->delay;
}


Lfloat   tLagrangeDelay_getMaxDelay (tLagrangeDelay* const dl)
{
	_tLagrangeDelay* d = *dl;
    return d->maxDelay;
}

Lfloat   tLagrangeDelay_getLastOut (tLagrangeDelay* const dl)
{
	_tLagrangeDelay* d = *dl;
    return d->lastOut;
}

Lfloat   tLagrangeDelay_getLastIn (tLagrangeDelay* const dl)
{
	_tLagrangeDelay* d = *dl;
    return d->lastIn;
}





// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ AllpassDelay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void tAllpassDelay_init (tAllpassDelay* const dl, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
{
    tAllpassDelay_initToPool(dl, delay, maxDelay, &leaf->mempool);
}

void tAllpassDelay_initToPool  (tAllpassDelay* const dl, Lfloat delay, uint32_t maxDelay, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tAllpassDelay* d = *dl = (_tAllpassDelay*) mpool_alloc(sizeof(_tAllpassDelay), m);
    d->mempool = m;

    d->maxDelay = maxDelay;

    if (delay > maxDelay)   d->delay = maxDelay;
    else if (delay < 0.0f)  d->delay = 0.0f;
    else                    d->delay = delay;

    d->buff = (Lfloat*) mpool_alloc(sizeof(Lfloat) * maxDelay, m);

    d->gain = 1.0f;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;
    
    d->inPoint = 0;
    d->outPoint = 0;
    
    tAllpassDelay_setDelay(dl, d->delay);
    
    d->apInput = 0.0f;
}

void tAllpassDelay_free (tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    
    mpool_free((char*)d->buff, d->mempool);
    mpool_free((char*)d, d->mempool);
}

void tAllpassDelay_clear(tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    for (unsigned i = 0; i < d->maxDelay; i++)
    {
        d->buff[i] = 0;
    }
}

Lfloat tAllpassDelay_tick (tAllpassDelay* const dl, Lfloat input)
{
    _tAllpassDelay* d = *dl;

    d->buff[d->inPoint] = input * d->gain;

    // Increment input pointer modulo length.
    if ( ++(d->inPoint) >= d->maxDelay )    d->inPoint = 0;

    // Do allpass interpolation delay.
    Lfloat out = d->lastOut * -d->coeff;
    out += d->apInput + ( d->coeff * d->buff[d->outPoint] );
    d->lastOut = out;

    // Save allpass input
    d->apInput = d->buff[d->outPoint];

    // Increment output pointer modulo length.
    if (++(d->outPoint) >= d->maxDelay )   d->outPoint = 0;

    return d->lastOut;
}

void     tAllpassDelay_setDelay (tAllpassDelay* const dl, Lfloat delay)
{
    _tAllpassDelay* d = *dl;

    d->delay = LEAF_clip(0.5f, delay,  d->maxDelay);

    // outPoint chases inPoint
    Lfloat outPointer = (Lfloat)d->inPoint - d->delay + 1.0f;

    while ( outPointer < 0 )    outPointer += d->maxDelay;  // mod max length

    d->outPoint = (uint32_t) outPointer;         // integer part

    if ( d->outPoint >= d->maxDelay )   d->outPoint = 0;

    d->alpha = 1.0f + (Lfloat)d->outPoint - outPointer; // fractional part

    if ( d->alpha < 0.5f )
    {
        // The optimal range for alpha is about 0.5 - 1.5 in order to
        // achieve the flattest phase delay response.
        
        d->outPoint += 1;
        
        if ( d->outPoint >= d->maxDelay ) d->outPoint -= d->maxDelay;
        
        d->alpha += 1.0f;
    }

    d->coeff = (1.0f - d->alpha) / (1.0f + d->alpha);  // coefficient for allpass
}

Lfloat tAllpassDelay_tapOut (tAllpassDelay* const dl, uint32_t tapDelay)
{
    _tAllpassDelay* d = *dl;

    int32_t tap = d->inPoint - tapDelay - 1;

    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;

    return d->buff[tap];

}

void tAllpassDelay_tapIn (tAllpassDelay* const dl, Lfloat value, uint32_t tapDelay)
{
    _tAllpassDelay* d = *dl;

    int32_t tap = d->inPoint - tapDelay - 1;

    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;

    d->buff[tap] = value;
}

Lfloat tAllpassDelay_addTo (tAllpassDelay* const dl, Lfloat value, uint32_t tapDelay)
{
    _tAllpassDelay* d = *dl;

    int32_t tap = d->inPoint - tapDelay - 1;

    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;

    return (d->buff[tap] += value);
}

Lfloat   tAllpassDelay_getDelay (tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    return d->delay;
}

Lfloat   tAllpassDelay_getLastOut (tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    return d->lastOut;
}

Lfloat   tAllpassDelay_getLastIn (tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    return d->lastIn;
}

void tAllpassDelay_setGain (tAllpassDelay* const dl, Lfloat gain)
{
    _tAllpassDelay* d = *dl;
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

Lfloat tAllpassDelay_getGain (tAllpassDelay* const dl)
{
    _tAllpassDelay* d = *dl;
    return d->gain;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TapeDelay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void tTapeDelay_init (tTapeDelay* const dl, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
{
    tTapeDelay_initToPool(dl, delay, maxDelay, &leaf->mempool);
}

void tTapeDelay_initToPool (tTapeDelay* const dl, Lfloat delay, uint32_t maxDelay, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTapeDelay* d = *dl = (_tTapeDelay*) mpool_alloc(sizeof(_tTapeDelay), m);
    d->mempool = m;

    d->maxDelay = maxDelay;

    d->buff = (Lfloat*) mpool_alloc(sizeof(Lfloat) * maxDelay, m);

    d->gain = 1.0f;

    d->lastIn = 0.0f;
    d->lastOut = 0.0f;

    d->idx = 0.0f;
    d->inc = 1.0f;
    d->inPoint = 0;

    tTapeDelay_setDelay(dl, delay);
}

void tTapeDelay_free (tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;

    mpool_free((char*)d->buff, d->mempool);
    mpool_free((char*)d, d->mempool);
}

void tTapeDelay_clear(tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    for (unsigned i = 0; i < d->maxDelay; i++)
    {
        d->buff[i] = 0;
    }
}

//#define SMOOTH_FACTOR 10.f

Lfloat   tTapeDelay_tick (tTapeDelay* const dl, Lfloat input)
{
    _tTapeDelay* d = *dl;

    d->buff[d->inPoint] = input * d->gain;

    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;

    int idx =  (int) d->idx;
    Lfloat alpha = d->idx - idx;

    d->lastOut =    LEAF_interpolate_hermite_x (d->buff[((idx - 1) + d->maxDelay) % d->maxDelay],
                                              d->buff[idx],
                                              d->buff[(idx + 1) % d->maxDelay],
                                              d->buff[(idx + 2) % d->maxDelay],
                                              alpha);

    Lfloat diff = (d->inPoint - d->idx);
    while (diff < 0.f) diff += d->maxDelay;

    d->inc = 1.0f + (diff - d->delay) / d->delay; //* SMOOTH_FACTOR;

    d->idx += d->inc;

    while (d->idx >= d->maxDelay) d->idx -= d->maxDelay;

    if (d->lastOut)
        return d->lastOut;
    return 0.0f;
}

void  tTapeDelay_incrementInPoint(tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;
}


void tTapeDelay_setRate(tTapeDelay* const dl, Lfloat rate)
{
    _tTapeDelay* d = *dl;
    d->inc = rate;
}

void     tTapeDelay_setDelay (tTapeDelay* const dl, Lfloat delay)
{
    _tTapeDelay* d = *dl;
    d->delay = LEAF_clip(1.f, delay,  d->maxDelay);
}

Lfloat tTapeDelay_tapOut (tTapeDelay* const dl, Lfloat tapDelay)
{
    _tTapeDelay* d = *dl;

    Lfloat tap = (Lfloat) d->inPoint - tapDelay - 1.f;

    // Check for wraparound.
    while ( tap < 0.f )   tap += (Lfloat)d->maxDelay;

    int idx =  (int) tap;

    Lfloat alpha = tap - idx;

    Lfloat samp =    LEAF_interpolate_hermite_x (d->buff[((idx - 1) + d->maxDelay) % d->maxDelay],
                                              d->buff[idx],
                                              d->buff[(idx + 1) % d->maxDelay],
                                              d->buff[(idx + 2) % d->maxDelay],
                                              alpha);

    return samp;

}

void tTapeDelay_tapIn (tTapeDelay* const dl, Lfloat value, uint32_t tapDelay)
{
    _tTapeDelay* d = *dl;

    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

Lfloat tTapeDelay_addTo (tTapeDelay* const dl, Lfloat value, uint32_t tapDelay)
{
    _tTapeDelay* d = *dl;
    
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

Lfloat   tTapeDelay_getDelay (tTapeDelay *dl)
{
    _tTapeDelay* d = *dl;
    return d->delay;
}

Lfloat   tTapeDelay_getLastOut (tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    return d->lastOut;
}

Lfloat   tTapeDelay_getLastIn (tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    return d->lastIn;
}

void tTapeDelay_setGain (tTapeDelay* const dl, Lfloat gain)
{
    _tTapeDelay* d = *dl;
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

Lfloat tTapeDelay_getGain (tTapeDelay* const dl)
{
    _tTapeDelay* d = *dl;
    return d->gain;
}




void    tRingBuffer_init     (tRingBuffer* const ring, int size, LEAF* const leaf)
{
    tRingBuffer_initToPool(ring, size, &leaf->mempool);
}

void    tRingBuffer_initToPool   (tRingBuffer* const ring, int size, tMempool* const mempool)
{
    _tMempool* m = *mempool;
    _tRingBuffer* r = *ring = (_tRingBuffer*) mpool_alloc(sizeof(_tRingBuffer), m);
    r->mempool = m;
    
    // Ensure size is a power of 2
    if (size <= 0) r->size = 1;
    else r->size = pow(2, ceil(log2(size)));
    r->mask = r->size - 1;
    
    r->buffer = (Lfloat*) mpool_calloc(sizeof(Lfloat) * r->size, m);
    r->pos = 0;
}

void    tRingBuffer_free     (tRingBuffer* const ring)
{
    _tRingBuffer* r = *ring;
    
    mpool_free((char*) r->buffer, r->mempool);
    mpool_free((char*) r, r->mempool);
}

void   tRingBuffer_push     (tRingBuffer* const ring, Lfloat val)
{
    _tRingBuffer* r = *ring;
    
    --r->pos;
    r->pos &= r->mask;
    r->buffer[r->pos] = val;
}

Lfloat   tRingBuffer_getNewest    (tRingBuffer* const ring)
{
    _tRingBuffer* r = *ring;
    
    return r->buffer[r->pos];
}

Lfloat   tRingBuffer_getOldest    (tRingBuffer* const ring)
{
    _tRingBuffer* r = *ring;
    
    return r->buffer[(r->pos + r->size - 1) & r->mask];
}

Lfloat   tRingBuffer_get      (tRingBuffer* const ring, int index)
{
    _tRingBuffer* r = *ring;
    
    return r->buffer[(r->pos + index) & r->mask];
}

int     tRingBuffer_getSize  (tRingBuffer* const ring)
{
    _tRingBuffer* r = *ring;
    
    return r->size;
}
