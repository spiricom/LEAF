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
	for(int i = 0; i < ((f->N)/2); ++i)		tSVF_setFreq(&f->low[i], f1);
}

void tButterworth_setF2(tButterworth* const ft, float f2)
{
    _tButterworth* f = *ft;
    
	f->f2 = f2;
	for(int i = 0; i < ((f->N)/2); ++i)		tSVF_setFreq(&f->high[i], f2);
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

// Less efficient, more accurate version of SVF, in which cutoff frequency is taken as floating point Hz value and tanf
// is calculated when frequency changes.
void tSVF_init(tSVF* const svff, SVFType type, float freq, float Q)
{
    _tSVF* svf = *svff = (_tSVF*) leaf_alloc(sizeof(_tSVF));
    
    svf->type = type;
    
    svf->ic1eq = 0;
    svf->ic2eq = 0;
    
    float a1,a2,a3,g,k;
    svf->g = tanf(PI * freq * leaf.invSampleRate);
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = g*a1;
    svf->a3 = g*a2;
}

void tSVF_free(tSVF* const svff)
{
    _tSVF* svf = *svff;
    
    leaf_free(svf);
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
    
    if (svf->type == SVFTypeLowpass)        return v2;
    else if (svf->type == SVFTypeBandpass)  return v1;
    else if (svf->type == SVFTypeHighpass)  return v0 - (svf->k * v1) - v2;
    else if (svf->type == SVFTypeNotch)     return v0 - (svf->k * v1);
    else if (svf->type == SVFTypePeak)      return v0 - (svf->k * v1) - (2.0f * v2);
    else                                    return 0.0f;
    
}

void     tSVF_setFreq(tSVF* const svff, float freq)
{
    _tSVF* svf = *svff;
    
    svf->g = tanf(PI * freq * leaf.invSampleRate);
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void     tSVF_setQ(tSVF* const svff, float Q)
{
    _tSVF* svf = *svff;
    
    svf->k = 1.0f/Q;
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
    
    float a1,a2,a3,g,k;
    svf->g = filtertan[input];
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f+g*(g+k));
    svf->a2 = g*a1;
    svf->a3 = g*a2;
}

void tEfficientSVF_free(tEfficientSVF* const svff)
{
    _tEfficientSVF* svf = *svff;
    
    leaf_free(svf);
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
    
    svf->g = filtertan[input];
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

void	tFIR_init(tFIR* const firf, float* coeffs, int numTaps)
{
    _tFIR* fir = *firf = (_tFIR*) leaf_alloc(sizeof(_tFIR));
    
    fir->numTaps = numTaps;
    fir->coeff = coeffs;
    fir->past = (float*)leaf_alloc(sizeof(float) * fir->numTaps);
    for (int i = 0; i < fir->numTaps; ++i) fir->past[i] = 0.0f;
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

void	tFIR_free(tFIR* const firf)
{
     _tFIR* fir = *firf;
    
    leaf_free(fir->past);
    leaf_free(fir);
}
