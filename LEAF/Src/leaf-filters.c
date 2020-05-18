/*==============================================================================

    leaf-filter.c
    Created: 20 Jan 2017 12:01:10pm
    Author:  Michael R Mulshine

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-filters.h"
#include "..\Inc\leaf-tables.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-filters.h"
#include "../Inc/leaf-tables.h"
#include "../leaf.h"
#include "tim.h"
#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OnePole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tAllpass_init(tAllpass* const ft, float initDelay, uint32_t maxDelay)
{
    _tAllpass* f = *ft = (_tAllpass*) leaf_alloc(sizeof(_tAllpass));
    
    f->gain = 0.7f;
    
    f->lastOut = 0.0f;
    
    tLinearDelay_init(&f->delay, initDelay, maxDelay);
}

void tAllpass_free(tAllpass* const ft)
{
    _tAllpass* f = *ft;
    
    tLinearDelay_free(&f->delay);
    leaf_free(f);
}

void    tAllpass_initToPool     (tAllpass* const ft, float initDelay, uint32_t maxDelay, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tAllpass* f = *ft = (_tAllpass*) mpool_alloc(sizeof(_tAllpass), m);
    
    f->gain = 0.7f;
    
    f->lastOut = 0.0f;
    
    tLinearDelay_initToPool(&f->delay, initDelay, maxDelay, mp);
}

void    tAllpass_freeFromPool   (tAllpass* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tAllpass* f = *ft;
    
    tLinearDelay_freeFromPool(&f->delay, mp);
    mpool_free(f, m);
}

void    tAllpass_setDelay(tAllpass* const ft, float delay)
{
    _tAllpass* f = *ft;
    
    tLinearDelay_setDelay(&f->delay, delay);
}

void    tAllpass_setGain(tAllpass* const ft, float gain)
{
    _tAllpass* f = *ft;
    
    f->gain = gain;
}

float   tAllpass_tick(tAllpass* const ft, float input)
{
    _tAllpass* f = *ft;
    
    float s1 = (-f->gain) * f->lastOut + input;
    
    float s2 = tLinearDelay_tick(&f->delay, s1) + (f->gain) * input;
    
    f->lastOut = s2;
    
    return f->lastOut;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OnePole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tOnePole_init(tOnePole* const ft, float freq)
{
    _tOnePole* f = *ft = (_tOnePole*) leaf_alloc(sizeof(_tOnePole));
    
    f->gain = 1.0f;
    f->a0 = 1.0;
    
    tOnePole_setFreq(ft, freq);
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
}

void    tOnePole_free(tOnePole* const ft)
{
    _tOnePole* f = *ft;
    
    leaf_free(f);
}

void    tOnePole_initToPool     (tOnePole* const ft, float freq, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tOnePole* f = *ft = (_tOnePole*) mpool_alloc(sizeof(_tOnePole), m);
    
    f->gain = 1.0f;
    f->a0 = 1.0;
    
    tOnePole_setFreq(ft, freq);
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
}

void    tOnePole_freeFromPool   (tOnePole* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tOnePole* f = *ft;
    
    mpool_free(f, m);
}

void    tOnePole_setB0(tOnePole* const ft, float b0)
{
    _tOnePole* f = *ft;
    f->b0 = b0;
}

void    tOnePole_setA1(tOnePole* const ft, float a1)
{
    _tOnePole* f = *ft;
    if (a1 >= 1.0f)     a1 = 0.999999f;
    f->a1 = a1;
}

void    tOnePole_setPole(tOnePole* const ft, float thePole)
{
    _tOnePole* f = *ft;
    
    if (thePole >= 1.0f)    thePole = 0.999999f;
    
    // Normalize coefficients for peak unity gain.
    if (thePole > 0.0f)     f->b0 = (1.0f - thePole);
    else                    f->b0 = (1.0f + thePole);
    
    f->a1 = -thePole;
}

void        tOnePole_setFreq        (tOnePole* const ft, float freq)
{
    _tOnePole* f = *ft;
    f->b0 = freq * leaf.twoPiTimesInvSampleRate;
    f->b0 = LEAF_clip(0.0f, f->b0, 1.0f);
    f->a1 = 1.0f - f->b0;
}

void    tOnePole_setCoefficients(tOnePole* const ft, float b0, float a1)
{
    _tOnePole* f = *ft;
    if (a1 >= 1.0f)     a1 = 0.999999f;
    f->b0 = b0;
    f->a1 = a1;
}

void    tOnePole_setGain(tOnePole* const ft, float gain)
{
    _tOnePole* f = *ft;
    f->gain = gain;
}

float   tOnePole_tick(tOnePole* const ft, float input)
{
    _tOnePole* f = *ft;
    
    float in = input * f->gain;
    float out = (f->b0 * in) + (f->a1 * f->lastOut);
    
    f->lastIn = in;
    f->lastOut = out;
    
    return out;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TwoPole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tTwoPole_init(tTwoPole* const ft)
{
    _tTwoPole* f = *ft = (_tTwoPole*) leaf_alloc(sizeof(_tTwoPole));
    
    f->gain = 1.0f;
    f->a0 = 1.0;
    f->b0 = 1.0;
    
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
}

void    tTwoPole_free(tTwoPole* const ft)
{
    _tTwoPole* f = *ft;
    
    leaf_free(f);
}

void    tTwoPole_initToPool     (tTwoPole* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTwoPole* f = *ft = (_tTwoPole*) mpool_alloc(sizeof(_tTwoPole), m);
    
    f->gain = 1.0f;
    f->a0 = 1.0;
    f->b0 = 1.0;
    
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
}

void    tTwoPole_freeFromPool   (tTwoPole* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTwoPole* f = *ft;
    
    mpool_free(f, m);
}

float   tTwoPole_tick(tTwoPole* const ft, float input)
{
    _tTwoPole* f = *ft;
    
    float in = input * f->gain;
    float out = (f->b0 * in) - (f->a1 * f->lastOut[0]) - (f->a2 * f->lastOut[1]);
    
    f->lastOut[1] = f->lastOut[0];
    f->lastOut[0] = out;
    
    return out;
}

void    tTwoPole_setB0(tTwoPole* const ft, float b0)
{
    _tTwoPole* f = *ft;
    f->b0 = b0;
}

void    tTwoPole_setA1(tTwoPole* const ft, float a1)
{
    _tTwoPole* f = *ft;
    f->a1 = a1;
}

void    tTwoPole_setA2(tTwoPole* const ft, float a2)
{
    _tTwoPole* f = *ft;
    f->a2 = a2;
}


void    tTwoPole_setResonance(tTwoPole* const ft, float frequency, float radius, oBool normalize)
{
    _tTwoPole* f = *ft;
    
    if (frequency < 0.0f)   frequency = 0.0f;
    if (frequency > (leaf.sampleRate * 0.49f))   frequency = leaf.sampleRate * 0.49f;
    if (radius < 0.0f)      radius = 0.0f;
    if (radius >= 1.0f)     radius = 0.999999f;
    
    f->radius = radius;
    f->frequency = frequency;
    f->normalize = normalize;
    
    f->a2 = radius * radius;
    f->a1 =  -2.0f * radius * cosf(frequency * leaf.twoPiTimesInvSampleRate);
    
    if ( normalize )
    {
        // Normalize the filter gain ... not terribly efficient.
        float real = 1 - radius + (f->a2 - radius) * cosf(2 * frequency * leaf.twoPiTimesInvSampleRate);
        float imag = (f->a2 - radius) * sinf(2 * frequency * leaf.twoPiTimesInvSampleRate);
        f->b0 = sqrtf( powf(real, 2) + powf(imag, 2) );
    }
}

void    tTwoPole_setCoefficients(tTwoPole* const ft, float b0, float a1, float a2)
{
    _tTwoPole* f = *ft;
    f->b0 = b0;
    f->a1 = a1;
    f->a2 = a2;
}

void    tTwoPole_setGain(tTwoPole* const ft, float gain)
{
    _tTwoPole* f = *ft;
    f->gain = gain;
}

void     tTwoPoleSampleRateChanged (tTwoPole* const ft)
{
    _tTwoPole* f = *ft;
    
    f->a2 = f->radius * f->radius;
    f->a1 =  -2.0f * f->radius * cosf(f->frequency * leaf.twoPiTimesInvSampleRate);
    
    if ( f->normalize )
    {
        // Normalize the filter gain ... not terribly efficient.
        float real = 1 - f->radius + (f->a2 - f->radius) * cosf(2 * f->frequency * leaf.twoPiTimesInvSampleRate);
        float imag = (f->a2 - f->radius) * sinf(2 * f->frequency * leaf.twoPiTimesInvSampleRate);
        f->b0 = sqrtf( powf(real, 2) + powf(imag, 2) );
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OneZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tOneZero_init(tOneZero* const ft, float theZero)
{
    _tOneZero* f = *ft = (_tOneZero*) leaf_alloc(sizeof(_tOneZero));
    
    f->gain = 1.0f;
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
    tOneZero_setZero(ft, theZero);
}

void    tOneZero_free(tOneZero* const ft)
{
    _tOneZero* f = *ft;
    
    leaf_free(f);
}

void    tOneZero_initToPool     (tOneZero* const ft, float theZero, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tOneZero* f = *ft = (_tOneZero*) mpool_alloc(sizeof(_tOneZero), m);
    
    f->gain = 1.0f;
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
    tOneZero_setZero(ft, theZero);
}

void    tOneZero_freeFromPool   (tOneZero* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tOneZero* f = *ft;
    
    mpool_free(f, m);
}

float   tOneZero_tick(tOneZero* const ft, float input)
{
    _tOneZero* f = *ft;
    
    float in = input * f->gain;
    float out = f->b1 * f->lastIn + f->b0 * in;
    
    f->lastIn = in;
    
    return out;
}

void    tOneZero_setZero(tOneZero* const ft, float theZero)
{
    _tOneZero* f = *ft;
    
    if (theZero > 0.0f) f->b0 = 1.0f / (1.0f + theZero);
    else                f->b0 = 1.0f / (1.0f - theZero);
    
    f->b1 = -theZero * f->b0;
    
}

void    tOneZero_setB0(tOneZero* const ft, float b0)
{
    _tOneZero* f = *ft;
    
    f->b0 = b0;
}

void    tOneZero_setB1(tOneZero* const ft, float b1)
{
    _tOneZero* f = *ft;
    f->b1 = b1;
}

void    tOneZero_setCoefficients(tOneZero* const ft, float b0, float b1)
{
    _tOneZero* f = *ft;
    f->b0 = b0;
    f->b1 = b1;
}

void    tOneZero_setGain(tOneZero *ft, float gain)
{
    _tOneZero* f = *ft;
    f->gain = gain;
}

float   tOneZero_getPhaseDelay(tOneZero* const ft, float frequency )
{
    _tOneZero* f = *ft;
    
    if ( frequency <= 0.0f) frequency = 0.05f;
    
    f->frequency = frequency;
    
    float omegaT = 2 * PI * frequency * leaf.invSampleRate;
    float real = 0.0, imag = 0.0;
    
    real += f->b0;
    
    real += f->b1 * cosf(omegaT);
    imag -= f->b1 * sinf(omegaT);
    
    real *= f->gain;
    imag *= f->gain;
    
    float phase = atan2f( imag, real );
    
    real = 0.0; imag = 0.0;
    
    phase -= atan2f( imag, real );
    
    phase = fmodf( -phase, 2 * PI );
    
    return phase / omegaT;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TwoZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tTwoZero_init(tTwoZero* const ft)
{
    _tTwoZero* f = *ft = (_tTwoZero*) leaf_alloc(sizeof(_tTwoZero));
    
    f->gain = 1.0f;
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
}

void    tTwoZero_free(tTwoZero* const ft)
{
    _tTwoZero* f = *ft;
    
    leaf_free(f);
}

void    tTwoZero_initToPool     (tTwoZero* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTwoZero* f = *ft = (_tTwoZero*) mpool_alloc(sizeof(_tTwoZero), m);
    
    f->gain = 1.0f;
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
}

void    tTwoZero_freeFromPool   (tTwoZero* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTwoZero* f = *ft;
    
    mpool_free(f, m);
}

float   tTwoZero_tick(tTwoZero* const ft, float input)
{
    _tTwoZero* f = *ft;
    
    float in = input * f->gain;
    float out = f->b2 * f->lastIn[1] + f->b1 * f->lastIn[0] + f->b0 * in;
    
    f->lastIn[1] = f->lastIn[0];
    f->lastIn[0] = in;
    
    return out;
}

void    tTwoZero_setNotch(tTwoZero* const ft, float freq, float radius)
{
    _tTwoZero* f = *ft;
    
    // Should also deal with frequency being > half sample rate / nyquist. See STK
    if (freq < 0.0f)    freq = 0.0f;
    if (radius < 0.0f)  radius = 0.0f;
    
    f->frequency = freq;
    f->radius = radius;
    
    f->b2 = radius * radius;
    f->b1 = -2.0f * radius * cosf(freq * leaf.twoPiTimesInvSampleRate); // OPTIMIZE with LOOKUP or APPROXIMATION
    
    // Normalize the filter gain. From STK.
    if ( f->b1 > 0.0f ) // Maximum at z = 0.
        f->b0 = 1.0f / ( 1.0f + f->b1 + f->b2 );
    else            // Maximum at z = -1.
        f->b0 = 1.0f / ( 1.0f - f->b1 + f->b2 );
    f->b1 *= f->b0;
    f->b2 *= f->b0;
    
}

void    tTwoZero_setB0(tTwoZero* const ft, float b0)
{
    _tTwoZero* f = *ft;
    f->b0 = b0;
}

void    tTwoZero_setB1(tTwoZero* const ft, float b1)
{
    _tTwoZero* f = *ft;
    f->b1 = b1;
}

void    tTwoZero_setCoefficients(tTwoZero* const ft, float b0, float b1, float b2)
{
    _tTwoZero* f = *ft;
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
}

void    tTwoZero_setGain(tTwoZero* const ft, float gain)
{
    _tTwoZero* f = *ft;
    f->gain = gain;
}

void tTwoZeroSampleRateChanged(tTwoZero* const ft)
{
    _tTwoZero* f = *ft;
    
    tTwoZero_setNotch(ft, f->frequency, f->radius);
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ PoleZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void   tPoleZero_init(tPoleZero* const pzf)
{
    _tPoleZero* f = *pzf = (_tPoleZero*) leaf_alloc(sizeof(_tPoleZero));
    
    f->gain = 1.0f;
    f->b0 = 1.0;
    f->a0 = 1.0;
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
}

void   tPoleZero_free(tPoleZero* const pzf)
{
    _tPoleZero* f = *pzf;
    
    leaf_free(f);
}

void    tPoleZero_initToPool        (tPoleZero* const pzf, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPoleZero* f = *pzf = (_tPoleZero*) mpool_alloc(sizeof(_tPoleZero), m);
    
    f->gain = 1.0f;
    f->b0 = 1.0;
    f->a0 = 1.0;
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
}

void    tPoleZero_freeFromPool      (tPoleZero* const pzf, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPoleZero* f = *pzf;
    
    mpool_free(f, m);
}

void    tPoleZero_setB0(tPoleZero* const pzf, float b0)
{
    _tPoleZero* f = *pzf;
    f->b0 = b0;
}

void    tPoleZero_setB1(tPoleZero* const pzf, float b1)
{
    _tPoleZero* f = *pzf;
    f->b1 = b1;
}

void    tPoleZero_setA1(tPoleZero* const pzf, float a1)
{
    _tPoleZero* f = *pzf;
    
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }
    
    f->a1 = a1;
}

void    tPoleZero_setCoefficients(tPoleZero* const pzf, float b0, float b1, float a1)
{
    _tPoleZero* f = *pzf;
    
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }
    
    f->b0 = b0;
    f->b1 = b1;
    f->a1 = a1;
}

void    tPoleZero_setAllpass(tPoleZero* const pzf, float coeff)
{
    _tPoleZero* f = *pzf;
    
    if (coeff >= 1.0f) // allpass coefficient >= 1.0 makes filter unstable
    {
        coeff = 0.999999f;
    }
    
    f->b0 = coeff;
    f->b1 = 1.0f;
    f->a0 = 1.0f;
    f->a1 = coeff;
}

void    tPoleZero_setBlockZero(tPoleZero* const pzf, float thePole)
{
    _tPoleZero* f = *pzf;
    
    if (thePole >= 1.0f) // allpass coefficient >= 1.0 makes filter unstable
    {
        thePole = 0.999999f;
    }
    
    f->b0 = 1.0f;
    f->b1 = -1.0f;
    f->a0 = 1.0f;
    f->a1 = -thePole;
}

void    tPoleZero_setGain(tPoleZero* const pzf, float gain)
{
    _tPoleZero* f = *pzf;
    f->gain = gain;
}

float   tPoleZero_tick(tPoleZero* const pzf, float input)
{
    _tPoleZero* f = *pzf;
    
    float in = input * f->gain;
    float out = (f->b0 * in) + (f->b1 * f->lastIn) - (f->a1 * f->lastOut);
    
    f->lastIn = in;
    f->lastOut = out;
    
    return out;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BiQuad Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tBiQuad_init(tBiQuad* const ft)
{
    _tBiQuad* f = *ft = (_tBiQuad*) leaf_alloc(sizeof(_tBiQuad));
    
    f->gain = 1.0f;
    
    f->b0 = 0.0f;
    f->a0 = 0.0f;
    
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
}

void    tBiQuad_free(tBiQuad* const ft)
{
    _tBiQuad* f = *ft;
    
    leaf_free(f);
}

void    tBiQuad_initToPool     (tBiQuad* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tBiQuad* f = *ft = (_tBiQuad*) mpool_alloc(sizeof(_tBiQuad), m);
    
    f->gain = 1.0f;
    
    f->b0 = 0.0f;
    f->a0 = 0.0f;
    
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
}

void    tBiQuad_freeFromPool   (tBiQuad* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tBiQuad* f = *ft;
    
    mpool_free(f, m);
}

float   tBiQuad_tick(tBiQuad* const ft, float input)
{
    _tBiQuad* f = *ft;
    
    float in = input * f->gain;
    float out = f->b0 * in + f->b1 * f->lastIn[0] + f->b2 * f->lastIn[1];
    out -= f->a2 * f->lastOut[1] + f->a1 * f->lastOut[0];
    
    f->lastIn[1] = f->lastIn[0];
    f->lastIn[0] = in;
    
    f->lastOut[1] = f->lastOut[0];
    f->lastOut[0] = out;
    
    return out;
}

void    tBiQuad_setResonance(tBiQuad* const ft, float freq, float radius, oBool normalize)
{
    _tBiQuad* f = *ft;
    
    if (freq < 0.0f)    freq = 0.0f;
    if (freq > (leaf.sampleRate * 0.49f))   freq = leaf.sampleRate * 0.49f;
    if (radius < 0.0f)  radius = 0.0f;
    if (radius >= 1.0f)  radius = 1.0f;
    
    f->frequency = freq;
    f->radius = radius;
    f->normalize = normalize;
    
    f->a2 = radius * radius;
    f->a1 = -2.0f * radius * cosf(freq * leaf.twoPiTimesInvSampleRate);
    
    if (normalize)
    {
        f->b0 = 0.5f - 0.5f * f->a2;
        f->b1 = 0.0f;
        f->b2 = -f->b0;
    }
}

void    tBiQuad_setNotch(tBiQuad* const ft, float freq, float radius)
{
    _tBiQuad* f = *ft;
    
    if (freq < 0.0f)    freq = 0.0f;
    if (freq > (leaf.sampleRate * 0.49f))   freq = leaf.sampleRate * 0.49f;
    if (radius < 0.0f)  radius = 0.0f;
    
    f->b2 = radius * radius;
    f->b1 = -2.0f * radius * cosf(freq * leaf.twoPiTimesInvSampleRate); // OPTIMIZE with LOOKUP or APPROXIMATION
    
    // Does not attempt to normalize filter gain.
}

void tBiQuad_setEqualGainZeros(tBiQuad* const ft)
{
    _tBiQuad* f = *ft;
    f->b0 = 1.0f;
    f->b1 = 0.0f;
    f->b2 = -1.0f;
}

void    tBiQuad_setB0(tBiQuad* const ft, float b0)
{
    _tBiQuad* f = *ft;
    f->b0 = b0;
}

void    tBiQuad_setB1(tBiQuad* const ft, float b1)
{
    _tBiQuad* f = *ft;
    f->b1 = b1;
}

void    tBiQuad_setB2(tBiQuad* const ft, float b2)
{
    _tBiQuad* f = *ft;
    f->b2 = b2;
}

void    tBiQuad_setA1(tBiQuad* const ft, float a1)
{
    _tBiQuad* f = *ft;
    f->a1 = a1;
}

void    tBiQuad_setA2(tBiQuad* const ft, float a2)
{
    _tBiQuad* f = *ft;
    f->a2 = a2;
}

void    tBiQuad_setCoefficients(tBiQuad* const ft, float b0, float b1, float b2, float a1, float a2)
{
    _tBiQuad* f = *ft;
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
    f->a1 = a1;
    f->a2 = a2;
}

void    tBiQuad_setGain(tBiQuad* const ft, float gain)
{
    _tBiQuad* f = *ft;
    f->gain = gain;
}

void    tBiQuadSampleRateChanged(tBiQuad* const ft)
{
    _tBiQuad* f = *ft;
    f->a2 = f->radius * f->radius;
    f->a1 = -2.0f * f->radius * cosf(f->frequency * leaf.twoPiTimesInvSampleRate);
    
    if (f->normalize)
    {
        f->b0 = 0.5f - 0.5f * f->a2;
        f->b1 = 0.0f;
        f->b2 = -f->b0;
    }
}

// Less efficient, more accurate version of SVF, in which cutoff frequency is taken as floating point Hz value and tanf
// is calculated when frequency changes.
void tSVF_init(tSVF* const svff, SVFType type, float freq, float Q)
{
    tSVF_initToPool     (svff, type, freq, Q, &leaf.mempool);

    // or maybe this?
    /*
     * hp=1 bp=A/Q (where A is 10^(G/40) and G is gain in decibels) and lp = 1
     */

}

void tSVF_free(tSVF* const svff)
{
    tSVF_freeFromPool     (svff, &leaf.mempool);
}

void    tSVF_initToPool     (tSVF* const svff, SVFType type, float freq, float Q, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSVF* svf = *svff = (_tSVF*) mpool_alloc(sizeof(_tSVF), m);

    svf->type = type;

    svf->ic1eq = 0;
    svf->ic2eq = 0;
    svf->Q = Q;
    svf->cutoff = freq;
    svf->g = tanf(PI * freq * leaf.invSampleRate);
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g*svf->a1;
    svf->a3 = svf->g*svf->a2;
    svf->cH = 0.0f;
    svf->cB = 0.0f;
    svf->cL = 1.0f;

    if (type == SVFTypeLowpass)
    {
        svf->cH = 0.0f;
        svf->cB = 0.0f;
        svf->cBK = 0.0f;
        svf->cL = 1.0f;
    }
    else if (type == SVFTypeBandpass)
    {
        svf->cH = 0.0f;
        svf->cB = 1.0f;
        svf->cBK = 0.0f;
        svf->cL = 0.0f;
    }

    else if (type == SVFTypeHighpass)
    {
        svf->cH = 1.0f;
        svf->cB = 0.0f;
        svf->cBK = -1.0f;
        svf->cL = -1.0f;
    }

    else if (type == SVFTypeNotch)
    {
        svf->cH = 1.0f;
        svf->cB = 0.0f;
        svf->cBK = -1.0f;
        svf->cL = 0.0f;
    }


    else if (type == SVFTypePeak)
    {
        svf->cH = 1.0f;
        svf->cB = 0.0f;
        svf->cBK = -1.0f;
        svf->cL = -2.0f;
    }
}

void    tSVF_freeFromPool   (tSVF* const svff, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSVF* svf = *svff;
    
    mpool_free(svf, m);
}

float   tSVF_tick(tSVF* const svff, float v0)
{
    _tSVF* svf = *svff;
    
    float v1,v2,v3;
    v3 = v0 - svf->ic2eq;
    v1 = (svf->a1 * svf->ic1eq) + (svf->a2 * v3);
    v2 = svf->ic2eq + (svf->a2 * svf->ic1eq) + (svf->a3 * v3);
    svf->ic1eq = (2.0f * v1) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;
    
    return (v0 * svf->cH) + (v1 * svf->cB) + (svf->k * v1 * svf->cBK) + (v2 * svf->cL);
}

void     tSVF_setFreq(tSVF* const svff, float freq)
{
    _tSVF* svf = *svff;
    svf->cutoff = freq;
    svf->g = tanf(PI * freq * leaf.invSampleRate);
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void     tSVF_setQ(tSVF* const svff, float Q)
{
    _tSVF* svf = *svff;
    svf->Q = Q;
    svf->k = 1.0f/Q;

    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void    tSVF_setFreqAndQ(tSVF* const svff, float freq, float Q)
{
    _tSVF* svf = *svff;
    svf->k = 1.0f/Q;
    svf->g = tanf(PI * freq * leaf.invSampleRate);
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

// Efficient version of tSVF where frequency is set based on 12-bit integer input for lookup in tanh wavetable.
void   tEfficientSVF_init(tEfficientSVF* const svff, SVFType type, uint16_t input, float Q)
{
    _tEfficientSVF* svf = *svff = (_tEfficientSVF*) leaf_alloc(sizeof(_tEfficientSVF));
    
    svf->type = type;
    
    svf->ic1eq = 0;
    svf->ic2eq = 0;
    
    svf->g = __leaf_table_filtertan[input];
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f+svf->g*(svf->g+svf->k));
    svf->a2 = svf->g*svf->a1;
    svf->a3 = svf->g*svf->a2;
}

void tEfficientSVF_free(tEfficientSVF* const svff)
{
    _tEfficientSVF* svf = *svff;
    
    leaf_free(svf);
}

void    tEfficientSVF_initToPool    (tEfficientSVF* const svff, SVFType type, uint16_t input, float Q, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tEfficientSVF* svf = *svff = (_tEfficientSVF*) mpool_alloc(sizeof(_tEfficientSVF), m);
    
    svf->type = type;
    
    svf->ic1eq = 0;
    svf->ic2eq = 0;
    
    svf->g = __leaf_table_filtertan[input];
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f+svf->g*(svf->g+svf->k));
    svf->a2 = svf->g*svf->a1;
    svf->a3 = svf->g*svf->a2;
}

void    tEfficientSVF_freeFromPool  (tEfficientSVF* const svff, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tEfficientSVF* svf = *svff;
    
    mpool_free(svf, m);
}

float   tEfficientSVF_tick(tEfficientSVF* const svff, float v0)
{
    _tEfficientSVF* svf = *svff;
    
    float v1,v2,v3;
    v3 = v0 - svf->ic2eq;
    v1 = (svf->a1 * svf->ic1eq) + (svf->a2 * v3);
    v2 = svf->ic2eq + (svf->a2 * svf->ic1eq) + (svf->a3 * v3);
    svf->ic1eq = (2.0f * v1) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;
    
    if (svf->type == SVFTypeLowpass)        return v2;
    else if (svf->type == SVFTypeBandpass)  return v1;
    else if (svf->type == SVFTypeHighpass)  return v0 - (svf->k * v1) - v2;
    else if (svf->type == SVFTypeNotch)     return v0 - (svf->k * v1);
    else if (svf->type == SVFTypePeak)      return v0 - (svf->k * v1) - (2.0f * v2);
    else                                    return 0.0f;
    
}

void     tEfficientSVF_setFreq(tEfficientSVF* const svff, uint16_t input)
{
    _tEfficientSVF* svf = *svff;
    
    svf->g = __leaf_table_filtertan[input];
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void     tEfficientSVF_setQ(tEfficientSVF* const svff, float Q)
{
    _tEfficientSVF* svf = *svff;
    
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

/* Highpass */
void    tHighpass_init(tHighpass* const ft, float freq)
{
    _tHighpass* f = *ft = (_tHighpass*) leaf_alloc(sizeof(_tHighpass));
    
    f->R = (1.0f - (freq * leaf.twoPiTimesInvSampleRate));
    f->ys = 0.0f;
    f->xs = 0.0f;
    
    f->frequency = freq;
}

void    tHighpass_free(tHighpass* const ft)
{
    _tHighpass* f = *ft;
    
    leaf_free(f);
}

void    tHighpass_initToPool    (tHighpass* const ft, float freq, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tHighpass* f = *ft = (_tHighpass*) mpool_calloc(sizeof(_tHighpass), m);
    
    f->R = (1.0f - (freq * leaf.twoPiTimesInvSampleRate));
    f->ys = 0.0f;
    f->xs = 0.0f;
    
    f->frequency = freq;
}

void    tHighpass_freeFromPool  (tHighpass* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tHighpass* f = *ft;
    
    mpool_free(f, m);
}

void     tHighpass_setFreq(tHighpass* const ft, float freq)
{
    _tHighpass* f = *ft;
    f->frequency = freq;
    f->R = (1.0f - (freq * leaf.twoPiTimesInvSampleRate));
    
}

float     tHighpass_getFreq(tHighpass* const ft)
{
    _tHighpass* f = *ft;
    return f->frequency;
}

// From JOS DC Blocker
float   tHighpass_tick(tHighpass* const ft, float x)
{
    _tHighpass* f = *ft;
    f->ys = x - f->xs + f->R * f->ys;
    f->xs = x;
    return f->ys;
}

void tHighpassSampleRateChanged(tHighpass* const ft)
{
    _tHighpass* f = *ft;
    f->R = (1.0f-((f->frequency * 2.0f * 3.14f) * leaf.invSampleRate));
}

void tButterworth_init(tButterworth* const ft, int N, float f1, float f2)
{
    _tButterworth* f = *ft = (_tButterworth*) leaf_alloc(sizeof(_tButterworth));
    
    f->f1 = f1;
    f->f2 = f2;
    f->gain = 1.0f;
    
    f->N = N;
    
    if (f->N > NUM_SVF_BW) f->N = NUM_SVF_BW;
    
    for(int i = 0; i < N/2; ++i)
    {
        tSVF_init(&f->low[i], SVFTypeHighpass, f1, 0.5f/cosf((1.0f+2.0f*i)*PI/(2*N)));
        tSVF_init(&f->high[i], SVFTypeLowpass, f2, 0.5f/cosf((1.0f+2.0f*i)*PI/(2*N)));
    }
}

void tButterworth_free(tButterworth* const ft)
{
    _tButterworth* f = *ft;
    
    for(int i = 0; i < f->N/2; ++i)
    {
        tSVF_free(&f->low[i]);
        tSVF_free(&f->high[i]);
    }
    
    leaf_free(f);
}

void    tButterworth_initToPool     (tButterworth* const ft, int N, float f1, float f2, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tButterworth* f = *ft = (_tButterworth*) mpool_alloc(sizeof(_tButterworth), m);
    
    f->f1 = f1;
    f->f2 = f2;
    f->gain = 1.0f;
    
    f->N = N;
    
    if (f->N > NUM_SVF_BW) f->N = NUM_SVF_BW;
    
    for(int i = 0; i < N/2; ++i)
    {
        tSVF_initToPool(&f->low[i], SVFTypeHighpass, f1, 0.5f/cosf((1.0f+2.0f*i)*PI/(2*N)), mp);
        tSVF_initToPool(&f->high[i], SVFTypeLowpass, f2, 0.5f/cosf((1.0f+2.0f*i)*PI/(2*N)), mp);
    }
}

void    tButterworth_freeFromPool   (tButterworth* const ft, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tButterworth* f = *ft;
    
    for(int i = 0; i < f->N/2; ++i)
    {
        tSVF_freeFromPool(&f->low[i], mp);
        tSVF_freeFromPool(&f->high[i], mp);
    }
    
    mpool_free(f, m);
}

float tButterworth_tick(tButterworth* const ft, float samp)
{
    _tButterworth* f = *ft;
    
    for(int i = 0; i < ((f->N)/2); ++i)
    {
        samp = tSVF_tick(&f->low[i],samp);
        samp = tSVF_tick(&f->high[i],samp);
    }
    return samp;
}

void tButterworth_setF1(tButterworth* const ft, float f1)
{
    _tButterworth* f = *ft;
    
    f->f1 = f1;
    for(int i = 0; i < ((f->N)/2); ++i)        tSVF_setFreq(&f->low[i], f1);
}

void tButterworth_setF2(tButterworth* const ft, float f2)
{
    _tButterworth* f = *ft;
    
    f->f2 = f2;
    for(int i = 0; i < ((f->N)/2); ++i)        tSVF_setFreq(&f->high[i], f2);
}

void tButterworth_setFreqs(tButterworth* const ft, float f1, float f2)
{
    _tButterworth* f = *ft;
    
    f->f1 = f1;
    f->f2 = f2;
    for(int i = 0; i < ((f->N)/2); ++i)
    {
        tSVF_setFreq(&f->low[i], f1);
        tSVF_setFreq(&f->high[i], f2);
    }
}

void	tFIR_init(tFIR* const firf, float* coeffs, int numTaps)
{
    _tFIR* fir = *firf = (_tFIR*) leaf_alloc(sizeof(_tFIR));
    
    fir->numTaps = numTaps;
    fir->coeff = coeffs;
    fir->past = (float*)leaf_alloc(sizeof(float) * fir->numTaps);
    for (int i = 0; i < fir->numTaps; ++i) fir->past[i] = 0.0f;
}

void    tFIR_free(tFIR* const firf)
{
    _tFIR* fir = *firf;
    
    leaf_free(fir->past);
    leaf_free(fir);
}

void    tFIR_initToPool     (tFIR* const firf, float* coeffs, int numTaps, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tFIR* fir = *firf = (_tFIR*) mpool_alloc(sizeof(_tFIR), m);
    
    fir->numTaps = numTaps;
    fir->coeff = coeffs;
    fir->past = (float*) mpool_alloc(sizeof(float) * fir->numTaps, m);
    for (int i = 0; i < fir->numTaps; ++i) fir->past[i] = 0.0f;
}

void    tFIR_freeFromPool   (tFIR* const firf, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tFIR* fir = *firf;
    
    mpool_free(fir->past, m);
    mpool_free(fir, m);
}

float	tFIR_tick(tFIR* const firf, float input)
{
    _tFIR* fir = *firf;
    
	fir->past[0] = input;
	float y = 0.0f;
	for (int i = 0; i < fir->numTaps; ++i) y += fir->past[i]*fir->coeff[i];
	for (int i = fir->numTaps-1; i > 0; --i) fir->past[i] = fir->past[i-1];
	return y;
}

//---------------------------------------------
////
/// Median filter implemented based on James McCartney's median filter in Supercollider,
/// translated from a Gen~ port of the Supercollider code that I believe was made by Rodrigo Costanzo and which I got from PA Tremblay - JS


void    tMedianFilter_init           (tMedianFilter* const f, int size)
{
	tMedianFilter_initToPool(f, size, &leaf.mempool);
}
void    tMedianFilter_free           (tMedianFilter* const f)
{
	tMedianFilter_freeFromPool(f, &leaf.mempool);
}
void    tMedianFilter_initToPool     (tMedianFilter* const mf, int size, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tMedianFilter* f = *mf = (_tMedianFilter*) mpool_alloc(sizeof(_tMedianFilter), m);

    f->size = size;
    f->middlePosition = size / 2;
	f->last = size - 1;
	f->pos = -1;
    f->val = (float*) mpool_alloc(sizeof(float) * size, m);
    f->age = (int*) mpool_alloc(sizeof(int) * size, m);
    for (int i = 0; i < f->size; ++i)
	{
    	f->val[i] = 0.0f;
    	f->age[i] = i;
	}

}
void    tMedianFilter_freeFromPool   (tMedianFilter* const mf, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tMedianFilter* f = *mf;

    mpool_free(f->val, m);
    mpool_free(f->age, m);
    mpool_free(f, m);
}

float   tMedianFilter_tick           (tMedianFilter* const mf, float input)
{
	_tMedianFilter* f = *mf;

	for(int i=0; i<f->size; i++) {
		int thisAge = f->age[i];
		if(thisAge == f->last) {
			f->pos = i;
		}
		else {
			thisAge++;
			f->age[i] = thisAge;
		}
	}

	while( f->pos!=0 ) {
		float test = f->val[f->pos-1];
		if(input < test) {
			f->val[f->pos]=test;
			f->age[f->pos]=f->age[f->pos-1];
			f->pos -= 1;
		} else {break;}
	}

	while(f->pos != f->last) {
		float test = f->val[f->pos+1];
		if( input > test) {
			f->val[f->pos] = test;
			f->age[f->pos] = f->age[f->pos+1];
			f->pos += 1;
		} else {break;}
	}

	f->val[f->pos] = input;
	f->age[f->pos] = 0;

	return  f->val[f->middlePosition];
}

/////

void    tVZFilter_init           (tVZFilter* const vf, VZFilterType type, float freq, float bandWidth)
{
	tVZFilter_initToPool(vf, type, freq, bandWidth, &leaf.mempool);
}

void    tVZFilter_free           (tVZFilter* const vf)
{
	tVZFilter_freeFromPool(vf, &leaf.mempool);
}
void    tVZFilter_initToPool     (tVZFilter* const vf, VZFilterType type, float freq, float bandWidth, tMempool* const mp)
{

	 _tMempool* m = *mp;
	 _tVZFilter* f = *vf = (_tVZFilter*) mpool_alloc(sizeof(_tVZFilter), m);
	f->fc   = freq;
	f->type = type;
	f->G    = ONE_OVER_SQRT2;
	f->invG    = 1.0f/ONE_OVER_SQRT2;
	f->B    = bandWidth;
	f->m    = 0.0f;
	f->s1 = 0.0f;
	f->s2 = 0.0f;
	f->sr = leaf.sampleRate;
	f->inv_sr = leaf.invSampleRate;
	tVZFilter_calcCoeffs(vf);


}
void    tVZFilter_freeFromPool   (tVZFilter* const vf, tMempool* const mp)
{
	 _tMempool* m = *mp;
		 _tVZFilter* f = *vf = (_tVZFilter*) mpool_alloc(sizeof(_tVZFilter), m);
		 mpool_free(f, m);
}

void 	tVZFilter_setSampleRate  (tVZFilter* const vf, float sampleRate)
{
	_tVZFilter* f = *vf;
	f->sr = sampleRate;
	f->inv_sr = 1.0f/sampleRate;
}

float   tVZFilter_tick           	(tVZFilter* const vf, float in)
{
	_tVZFilter* f = *vf;

	float yL, yB, yH;

	// compute highpass output via Eq. 5.1:
	yH = (in - f->R2*f->s1 - f->g*f->s1 - f->s2) * f->h;

	// compute bandpass output by applying 1st integrator to highpass output:
	yB = tanhf(f->g*yH) + f->s1;
	f->s1 = f->g*yH + yB; // state update in 1st integrator

	// compute lowpass output by applying 2nd integrator to bandpass output:
	yL = tanhf(f->g*yB) + f->s2;
	f->s2 = f->g*yB + yL; // state update in 2nd integrator

	//according to the Vadim paper, we could add saturation to this model by adding a tanh in the integration stage.
	//
	//seems like that might look like this:
    // y = tanh(g*x) + s; // output computation
    // s = g*x + y; // state update

	//instead of this:
    // y = g*x + s; // output computation
    // s = g*x + y; // state update

	return f->cL*yL + f->cB*yB + f->cH*yH;

}

float   tVZFilter_tickEfficient           	(tVZFilter* const vf, float in)
{
	_tVZFilter* f = *vf;

	float yL, yB, yH;

	// compute highpass output via Eq. 5.1:
	yH = (in - f->R2*f->s1 - f->g*f->s1 - f->s2) * f->h;

	// compute bandpass output by applying 1st integrator to highpass output:
	yB = (f->g*yH) + f->s1;
	f->s1 = f->g*yH + yB; // state update in 1st integrator

	// compute lowpass output by applying 2nd integrator to bandpass output:
	yL = (f->g*yB) + f->s2;
	f->s2 = f->g*yB + yL; // state update in 2nd integrator

	//according to the Vadim paper, we could add saturation to this model by adding a tanh in the integration stage.
	//
	//seems like that might look like this:
    // y = tanh(g*x) + s; // output computation
    // s = g*x + y; // state update

	//instead of this:
    // y = g*x + s; // output computation
    // s = g*x + y; // state update

	return f->cL*yL + f->cB*yB + f->cH*yH;

}

float   tVZFilter_tickEfficientBP           	(tVZFilter* const vf, float in)
{
	_tVZFilter* f = *vf;

	float yL, yB, yH;

	// compute highpass output via Eq. 5.1:
	yH = (in - f->R2*f->s1 - f->g*f->s1 - f->s2) * f->h;

	// compute bandpass output by applying 1st integrator to highpass output:
	yB = (f->g*yH) + f->s1;
	f->s1 = f->g*yH + yB; // state update in 1st integrator

	// compute lowpass output by applying 2nd integrator to bandpass output:
	yL = (f->g*yB) + f->s2;
	f->s2 = f->g*yB + yL; // state update in 2nd integrator

	//according to the Vadim paper, we could add saturation to this model by adding a tanh in the integration stage.
	//
	//seems like that might look like this:
    // y = tanh(g*x) + s; // output computation
    // s = g*x + y; // state update

	//instead of this:
    // y = g*x + s; // output computation
    // s = g*x + y; // state update

	return f->cL*yL + f->cB*yB + f->cH*yH;

}


void   tVZFilter_calcCoeffs           (tVZFilter* const vf)
{

	_tVZFilter* f = *vf;
	f->g = tanf(PI * f->fc * f->inv_sr);  // embedded integrator gain (Fig 3.11)

	  switch( f->type )
	  {
	  case Bypass:
		{
		  f->R2 = f->invG;  // can we use an arbitrary value here, for example R2 = 1?
		  f->cL = 1.0f;
		  f->cB = f->R2;
		  f->cH = 1.0f;
		}
		break;
	  case Lowpass:
		{
			f->R2 = f->invG;
			f->cL = 1.0f; f->cB = 0.0f; f->cH = 0.0f;
		}
		break;
	  case Highpass:
		{
			f->R2 = f->invG;
			f->cL = 0.0f; f->cB = 0.0f; f->cH = 1.0f;
		}
		break;
	  case BandpassSkirt:
		{
			f->R2 = f->invG;
			f->cL = 0.0f; f->cB = 1.0f; f->cH = 0.0f;
		}
		break;
	  case BandpassPeak:
		{
			f->R2 = 2.0f*tVZFilter_BandwidthToR(vf, f->B);
			f->cL = 0.0f; f->cB = f->R2; f->cH = 0.0f;
		}
		break;
	  case BandReject:
		{
			f->R2 = 2.0f*tVZFilter_BandwidthToR(vf, f->B);
			f->cL = 1.0f; f->cB = 0.0f; f->cH = 1.0f;
		}
		break;
	  case Bell:
		{
			float fl = f->fc*powf(2.0f, (-f->B)*0.5f); // lower bandedge frequency (in Hz)
			float wl = tanf(PI*fl*f->inv_sr);   // warped radian lower bandedge frequency /(2*fs)
			float r  = f->g/wl;
			r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
									   // warped upper bandedge, wc the center
			f->R2 = 2.0f*sqrtf(((r*r+1.0f)/r-2.0f)/(4.0f*f->G));
			f->cL = 1.0f; f->cB = f->R2*f->G; f->cH = 1.0f;
		}
		break;
	  case Lowshelf:
		{
			float A = sqrtf(f->G);
		  f->g /= sqrtf(A);               // scale SVF-cutoff frequency for shelvers
		  f->R2 = 2*sinhf(f->B*logf(2.0f)*0.5f);
		  f->cL = f->G; f->cB = f->R2*A; f->cH = 1.0f;
		}
		break;
	  case Highshelf:
		{
		  float A = sqrtf(f->G);
		  f->g *= sqrtf(A);               // scale SVF-cutoff frequency for shelvers
		  f->R2 = 2.0f*sinhf(f->B*logf(2.0f)*0.5f);
		  f->cL = 1.0f; f->cB = f->R2*A; f->cH = f->G;
		}
		break;
	  case Allpass:
		{
			f->R2 = 2.0f*tVZFilter_BandwidthToR(vf, f->B);
			f->cL = 1.0f; f->cB = -f->R2; f->cH = 1.0f;
		}
		break;

		// experimental - maybe we must find better curves for cL, cB, cH:
	  case Morph:
		{
			f->R2 = f->invG;
		  float x  = 2.0f*f->m-1.0f;

		  f->cL = maximum(-x, 0.0f); /*cL *= cL;*/
		  f->cH = minimum( x, 0.0f); /*cH *= cH;*/
		  f->cB = 1.0f-x*x;

			// bottom line: we need to test different versions for how they feel when tweaking the
			// morph parameter

		  // this scaling ensures constant magnitude at the cutoff point (we divide the coefficients by
		  // the magnitude response value at the cutoff frequency and scale back by the gain):
		  float s = f->G * sqrtf((f->R2*f->R2) / (f->cL*f->cL + f->cB*f->cB + f->cH*f->cH - 2.0f*f->cL*f->cH));
		  f->cL *= s; f->cB *= s; f->cH *= s;
		}
		break;

	  }

	  f->h = 1.0f / (1.0f + f->R2*f->g + f->g*f->g);  // factor for feedback precomputation
}


void   tVZFilter_setBandwidth           	(tVZFilter* const vf, float B)
{
	_tVZFilter* f = *vf;
	f->B = LEAF_clip(0.0f, B, 100.0f);
	tVZFilter_calcCoeffs(vf);
}
void   tVZFilter_setFreq           (tVZFilter* const vf, float freq)
{
	_tVZFilter* f = *vf;
	f->fc = LEAF_clip(0.0f, freq, 0.5f*leaf.sampleRate);
	tVZFilter_calcCoeffs(vf);
}
void   tVZFilter_setFreqAndBandwidth           (tVZFilter* const vf, float freq, float bw)
{
	_tVZFilter* f = *vf;
	f->B = LEAF_clip(0.0f,bw, 100.0f);
	f->fc = LEAF_clip(0.0f, freq, 0.5f*leaf.sampleRate);
	tVZFilter_calcCoeffs(vf);
}

void   tVZFilter_setGain          		(tVZFilter* const vf, float gain)
{
	_tVZFilter* f = *vf;
	f->G = LEAF_clip(0.000001f, gain, 100.0f);
	f->invG = 1.0f/f->G;
	tVZFilter_calcCoeffs(vf);
}

void   tVZFilter_setMorph          		(tVZFilter* const vf, float morph)
{
	_tVZFilter* f = *vf;
	f->m = LEAF_clip(0.0f, morph, 1.0f);
	tVZFilter_calcCoeffs(vf);
}

void tVZFilter_setType         		(tVZFilter* const vf, VZFilterType type)
{
	_tVZFilter* f = *vf;
	f->type = type;
	tVZFilter_calcCoeffs(vf);
}

float tVZFilter_BandwidthToR(tVZFilter* const vf, float B)
{
	_tVZFilter* f = *vf;
  float fl = f->fc*powf(2.0f, -B*0.5f); // lower bandedge frequency (in Hz)
  float gl = tanf(PI*fl*f->inv_sr);   // warped radian lower bandedge frequency /(2*fs)
  float r  = gl/f->g;            // ratio between warped lower bandedge- and center-frequencies
							   // unwarped: r = pow(2, -B/2) -> approximation for low
							   // center-frequencies
  return sqrtf((1.0f-r*r)*(1.0f-r*r)/(4.0f*r*r));
}



void    tDiodeFilter_init           (tDiodeFilter* const vf, float cutoff, float resonance)
{
	tDiodeFilter_initToPool(vf, cutoff, resonance, &leaf.mempool);
}

void    tDiodeFilter_free           (tDiodeFilter* const vf)
{
	tDiodeFilter_freeFromPool(vf, &leaf.mempool);
}
void    tDiodeFilter_initToPool     (tDiodeFilter* const vf, float cutoff, float resonance, tMempool* const mp)
{

	 _tMempool* m = *mp;
	 _tDiodeFilter* f = *vf = (_tDiodeFilter*) mpool_alloc(sizeof(_tDiodeFilter), m);
	 // initialization (the resonance factor is between 0 and 8 according to the article)
	 f->f = tan(PI * cutoff/leaf.sampleRate);
	 f->r = (7.f * resonance + 0.5f);
	 f->Vt = 0.5f;
	 f->n = 1.836f;
	 f->zi = 0.0f; //previous input value
	 f->gamma = f->Vt*f->n;
	 f->s0 = 0.01f;
	 f->s1 = 0.02f;
	 f->s2 = 0.03f;
	 f->s3 = 0.04f;
	 f->g0inv = 1.f/(2.f*f->Vt);
	 f->g1inv = 1.f/(2.f*f->gamma);
	 f->g2inv = 1.f/(6.f*f->gamma);


}
void    tDiodeFilter_freeFromPool   (tDiodeFilter* const vf, tMempool* const mp)
{
	 _tMempool* m = *mp;
	 _tDiodeFilter* f = *vf = (_tDiodeFilter*) mpool_alloc(sizeof(_tDiodeFilter), m);
	 mpool_free(f, m);
}

float tanhXdX(float x)
{
    float a = x*x;
    // IIRC I got this as Pade-approx for tanh(sqrt(x))/sqrt(x)
    return ((a + 105.0f)*a + 945.0f) / ((15.0f*a + 420.0f)*a + 945.0f);
}

float   tDiodeFilter_tick           	(tDiodeFilter* const vf, float in)
{
	_tDiodeFilter* f = *vf;

	// the input x[n+1] is given by 'in', and x[n] by zi
	// input with half delay
	float ih = 0.5f * (in + f->zi);

	// evaluate the non-linear factors
	float t0 = f->f*tanhXdX((ih - f->r * f->s3)*f->g0inv)*f->g0inv;
	float t1 = f->f*tanhXdX((f->s1-f->s0)*f->g1inv)*f->g1inv;
	float t2 = f->f*tanhXdX((f->s2-f->s1)*f->g1inv)*f->g1inv;
	float t3 = f->f*tanhXdX((f->s3-f->s2)*f->g1inv)*f->g1inv;
	float t4 = f->f*tanhXdX((f->s3)*f->g2inv)*f->g2inv;



	// This formula gives the result for y3 thanks to MATLAB
	float y3 = (f->s2 + f->s3 + t2*(f->s1 + f->s2 + f->s3 + t1*(f->s0 + f->s1 + f->s2 + f->s3 + t0*in)) + t1*(2.0f*f->s2 + 2.0f*f->s3))*t3 + f->s3 + 2.0f*f->s3*t1 + t2*(2.0f*f->s3 + 3.0f*f->s3*t1);
	if (isnan(y3))
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 400);
	}
	float tempy3denom = (t4 + t1*(2.0f*t4 + 4.0f) + t2*(t4 + t1*(t4 + f->r*t0 + 4.0f) + 3.0f) + 2.0f)*t3 + t4 + t1*(2.0f*t4 + 2.0f) + t2*(2.0f*t4 + t1*(3.0f*t4 + 3.0f) + 2.0f) + 1.0f;
	if (isnan(tempy3denom))
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 400);
	}
	if (tempy3denom == 0.0f)
	{
		tempy3denom = 0.000001f;
	}
	y3 = y3 / tempy3denom;
	if (isnan(y3))
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 400);
	}
	if (t1 == 0.0f)
	{
		t1 = 0.000001f;
	}
	if (t2 == 0.0f)
	{
		t2 = 0.000001f;
	}
	if (t3 == 0.0f)
	{
		t3 = 0.000001f;
	}
	// Other outputs
	float y2 = (f->s3 - (1+t4+t3)*y3) / (-t3);
	float y1 = (f->s2 - (1+t3+t2)*y2 + t3*y3) / (-t2);
	float y0 = (f->s1 - (1+t2+t1)*y1 + t2*y2) / (-t1);
	float xx = (in - f->r*y3);

	// update state
	f->s0 += 2.0f * (t0*xx + t1*(y1-y0));
	if (isnan(f->s0))
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 400);
	}

	if (isinf(f->s0))
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 400);
	}
	f->s1 += 2.0f * (t2*(y2-y1) - t1*(y1-y0));
	f->s2 += 2.0f * (t3*(y3-y2) - t2*(y2-y1));
	f->s3 += 2.0f * (-t4*(y3) - t3*(y3-y2));

	f->zi = in;
	return y3*f->r;

}



void    tDiodeFilter_setFreq     (tDiodeFilter* const vf, float cutoff)
{
	 _tDiodeFilter* f = *vf;
	 f->f = tanf(PI * LEAF_clip(10.0f, cutoff, 20000.0f)*leaf.invSampleRate);
}


void    tDiodeFilter_setQ     (tDiodeFilter* const vf, float resonance)
{
	 _tDiodeFilter* f = *vf;
	 f->r = LEAF_clip(0.5, (7.f * resonance + 0.5f), 8.0f);

}

