/*==============================================================================

    leaf-filter.c
    Created: 20 Jan 2017 12:01:10pm
    Author:  Michael R Mulshine

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-filter.h"
#include "..\Inc\leaf-tables.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-filter.h"
#include "../Inc/leaf-tables.h"
#include "../leaf.h"

#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OnePole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tAllpass_init(tAllpass* const f, float initDelay, uint32_t maxDelay)
{
    f->gain = 0.7f;
    
    f->lastOut = 0.0f;
    
    tDelayL_init(&f->delay, initDelay, maxDelay);
}

void    tAllpass_setDelay(tAllpass* const f, float delay)
{
    tDelayL_setDelay(&f->delay, delay);
}

void    tAllpass_free(tAllpass* const f)
{
    leaf_free(&f->delay);
    leaf_free(f);
}

void    tAllpass_setGain(tAllpass* const f, float gain)
{
    f->gain = gain;
}

float   tAllpass_tick(tAllpass* const f, float input)
{
    float s1 = (-f->gain) * f->lastOut + input;
    
    float s2 = tDelayL_tick(&f->delay, s1) + (f->gain) * input;
    
    f->lastOut = s2;
    
    return f->lastOut;
}

void tButterworth_init(tButterworth* const f, int N, float f1, float f2)
{
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

void tButterworth_free(tButterworth* const f)
{
    for(int i = 0; i < f->N/2; ++i)
    {
        tSVF_free(&f->low[i]);
        tSVF_free(&f->high[i]);
    }
    
    leaf_free(f);
}

float tButterworth_tick(tButterworth* const f, float samp)
{
	for(int i = 0; i < ((f->N)/2); ++i)
	{
		samp = tSVF_tick(&f->low[i],samp);
		samp = tSVF_tick(&f->high[i],samp);
	}
	return samp;
}

void tButterworth_setF1(tButterworth* const f, float f1)
{
	f->f1 = f1;
	for(int i = 0; i < ((f->N)/2); ++i)		tSVF_setFreq(&f->low[i], f1);
}

void tButterworth_setF2(tButterworth* const f, float f2)
{
	f->f2 = f2;
	for(int i = 0; i < ((f->N)/2); ++i)		tSVF_setFreq(&f->high[i], f2);
}

void tButterworth_setFreqs(tButterworth* const f, float f1, float f2)
{
	f->f1 = f1;
	f->f2 = f2;
	for(int i = 0; i < ((f->N)/2); ++i)
	{
		tSVF_setFreq(&f->low[i], f1);
		tSVF_setFreq(&f->high[i], f2);
	}
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OneZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tOneZero_init(tOneZero* const f, float theZero)
{
    f->gain = 1.0f;
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
    tOneZero_setZero(f, theZero);
}

void    tOneZero_free(tOneZero* const f)
{
    leaf_free(f);
}

float   tOneZero_tick(tOneZero* const f, float input)
{
    float in = input * f->gain;
    float out = f->b1 * f->lastIn + f->b0 * in;
    
    f->lastIn = in;
    
    return out;
}

void    tOneZero_setZero(tOneZero* const f, float theZero)
{
    if (theZero > 0.0f) f->b0 = 1.0f / (1.0f + theZero);
    else                f->b0 = 1.0f / (1.0f - theZero);
    
    f->b1 = -theZero * f->b0;
    
}

void    tOneZero_setB0(tOneZero* const f, float b0)
{
    f->b0 = b0;
}

void    tOneZero_setB1(tOneZero* const f, float b1)
{
    f->b1 = b1;
}

void    tOneZero_setCoefficients(tOneZero* const f, float b0, float b1)
{
    f->b0 = b0;
    f->b1 = b1;
}

void    tOneZero_setGain(tOneZero *f, float gain)
{
    f->gain = gain;
}

float   tOneZero_getPhaseDelay(tOneZero* const f, float frequency )
{
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
void    tTwoZero_init(tTwoZero* const f)
{
    f->gain = 1.0f;
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
}

void    tTwoZero_free(tTwoZero* const f)
{
    leaf_free(f);
}

float   tTwoZero_tick(tTwoZero* const f, float input)
{
    float in = input * f->gain;
    float out = f->b2 * f->lastIn[1] + f->b1 * f->lastIn[0] + f->b0 * in;
    
    f->lastIn[1] = f->lastIn[0];
    f->lastIn[0] = in;
    
    return out;
}

void    tTwoZero_setNotch(tTwoZero* const f, float freq, float radius)
{
    // Should also deal with frequency being > half sample rate / nyquist. See STK
    if (freq < 0.0f)    freq = 0.0f;
    if (radius < 0.0f)  radius = 0.0f;
    
    f->frequency = freq;
    f->radius = radius;
    
    f->b2 = radius * radius;
    f->b1 = -2.0f * radius * cosf(TWO_PI * freq * leaf.invSampleRate); // OPTIMIZE with LOOKUP or APPROXIMATION
    
    // Normalize the filter gain. From STK.
    if ( f->b1 > 0.0f ) // Maximum at z = 0.
        f->b0 = 1.0f / ( 1.0f + f->b1 + f->b2 );
    else            // Maximum at z = -1.
        f->b0 = 1.0f / ( 1.0f - f->b1 + f->b2 );
    f->b1 *= f->b0;
    f->b2 *= f->b0;
    
}

void    tTwoZero_setB0(tTwoZero* const f, float b0)
{
    f->b0 = b0;
}

void    tTwoZero_setB1(tTwoZero* const f, float b1)
{
    f->b1 = b1;
}

void    tTwoZero_setCoefficients(tTwoZero* const f, float b0, float b1, float b2)
{
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
}

void    tTwoZero_setGain(tTwoZero* const f, float gain)
{
    f->gain = gain;
}

void tTwoZeroSampleRateChanged(tTwoZero* const f)
{
    tTwoZero_setNotch(f, f->frequency, f->radius);
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OnePole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tOnePole_init(tOnePole* const f, float freq)
{
    f->gain = 1.0f;
    f->a0 = 1.0;
    
    tOnePole_setFreq(f, freq);
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
}

void    tOnePole_free(tOnePole* const f)
{
    leaf_free(f);
}

void    tOnePole_setB0(tOnePole* const f, float b0)
{
    f->b0 = b0;
}

void    tOnePole_setA1(tOnePole* const f, float a1)
{
    if (a1 >= 1.0f)     a1 = 0.999999f;
    
    f->a1 = a1;
}

void    tOnePole_setPole(tOnePole* const f, float thePole)
{
    if (thePole >= 1.0f)    thePole = 0.999999f;
    
    // Normalize coefficients for peak unity gain.
    if (thePole > 0.0f)     f->b0 = (1.0f - thePole);
    else                    f->b0 = (1.0f + thePole);
    
    f->a1 = -thePole;
}

void        tOnePole_setFreq        (tOnePole* const f, float freq)
{
    f->b0 = freq * TWO_PI * leaf.invSampleRate;
    
    f->b0 = LEAF_clip(0.0f, f->b0, 1.0f);
    
    f->a1 = 1.0f - f->b0;
}

void    tOnePole_setCoefficients(tOnePole* const f, float b0, float a1)
{
    if (a1 >= 1.0f)     a1 = 0.999999f;
    
    f->b0 = b0;
    f->a1 = a1;
}

void    tOnePole_setGain(tOnePole* const f, float gain)
{
    f->gain = gain;
}

float   tOnePole_tick(tOnePole* const f, float input)
{
    float in = input * f->gain;
    float out = (f->b0 * in) + (f->a1 * f->lastOut);
    
    f->lastIn = in;
    f->lastOut = out;
    
    return out;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TwoPole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tTwoPole_init(tTwoPole* const f)
{
    f->gain = 1.0f;
    f->a0 = 1.0;
    f->b0 = 1.0;
    
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
}

void    tTwoPole_free(tTwoPole* const f)
{
    leaf_free(f);
}

float   tTwoPole_tick(tTwoPole* const f, float input)
{
    float in = input * f->gain;
    float out = (f->b0 * in) - (f->a1 * f->lastOut[0]) - (f->a2 * f->lastOut[1]);
    
    f->lastOut[1] = f->lastOut[0];
    f->lastOut[0] = out;
    
    return out;
}

void    tTwoPole_setB0(tTwoPole* const f, float b0)
{
    f->b0 = b0;
}

void    tTwoPole_setA1(tTwoPole* const f, float a1)
{
    f->a1 = a1;
}

void    tTwoPole_setA2(tTwoPole* const f, float a2)
{
    f->a2 = a2;
}


void    tTwoPole_setResonance(tTwoPole* const f, float frequency, float radius, oBool normalize)
{
    if (frequency < 0.0f)   frequency = 0.0f; // need to also handle when frequency > nyquist
    if (radius < 0.0f)      radius = 0.0f;
    if (radius >= 1.0f)     radius = 0.999999f;
    
    f->radius = radius;
    f->frequency = frequency;
    f->normalize = normalize;
    
    f->a2 = radius * radius;
    f->a1 =  -2.0f * radius * cos(TWO_PI * frequency * leaf.invSampleRate);
    
    if ( normalize )
    {
        // Normalize the filter gain ... not terribly efficient.
        float real = 1 - radius + (f->a2 - radius) * cos(TWO_PI * 2 * frequency * leaf.invSampleRate);
        float imag = (f->a2 - radius) * sin(TWO_PI * 2 * frequency * leaf.invSampleRate);
        f->b0 = sqrt( pow(real, 2) + pow(imag, 2) );
    }
}

void    tTwoPole_setCoefficients(tTwoPole* const f, float b0, float a1, float a2)
{
    f->b0 = b0;
    f->a1 = a1;
    f->a2 = a2;
}

void    tTwoPole_setGain(tTwoPole* const f, float gain)
{
    f->gain = gain;
}

void     tTwoPoleSampleRateChanged (tTwoPole* const f)
{
    f->a2 = f->radius * f->radius;
    f->a1 =  -2.0f * f->radius * cos(TWO_PI * f->frequency * leaf.invSampleRate);
    
    if ( f->normalize )
    {
        // Normalize the filter gain ... not terribly efficient.
        float real = 1 - f->radius + (f->a2 - f->radius) * cos(TWO_PI * 2 * f->frequency * leaf.invSampleRate);
        float imag = (f->a2 - f->radius) * sin(TWO_PI * 2 * f->frequency * leaf.invSampleRate);
        f->b0 = sqrt( pow(real, 2) + pow(imag, 2) );
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ PoleZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void   tPoleZero_init(tPoleZero* const f)
{
    f->gain = 1.0f;
    f->b0 = 1.0;
    f->a0 = 1.0;
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
}

void   tPoleZero_free(tPoleZero* const f)
{
    leaf_free(f);
}

void    tPoleZero_setB0(tPoleZero* const pzf, float b0)
{
    pzf->b0 = b0;
}

void    tPoleZero_setB1(tPoleZero* const pzf, float b1)
{
    pzf->b1 = b1;
}

void    tPoleZero_setA1(tPoleZero* const pzf, float a1)
{
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }
    
    pzf->a1 = a1;
}

void    tPoleZero_setCoefficients(tPoleZero* const pzf, float b0, float b1, float a1)
{
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }
    
    pzf->b0 = b0;
    pzf->b1 = b1;
    pzf->a1 = a1;
}

void    tPoleZero_setAllpass(tPoleZero* const pzf, float coeff)
{
    if (coeff >= 1.0f) // allpass coefficient >= 1.0 makes filter unstable
    {
        coeff = 0.999999f;
    }
    
    pzf->b0 = coeff;
    pzf->b1 = 1.0f;
    pzf->a0 = 1.0f;
    pzf->a1 = coeff;
}

void    tPoleZero_setBlockZero(tPoleZero* const pzf, float thePole)
{
    if (thePole >= 1.0f) // allpass coefficient >= 1.0 makes filter unstable
    {
        thePole = 0.999999f;
    }
    
    pzf->b0 = 1.0f;
    pzf->b1 = -1.0f;
    pzf->a0 = 1.0f;
    pzf->a1 = -thePole;
}

void    tPoleZero_setGain(tPoleZero* const pzf, float gain)
{
    pzf->gain = gain;
}

float   tPoleZero_tick(tPoleZero* const pzf, float input)
{
    float in = input * pzf->gain;
    float out = (pzf->b0 * in) + (pzf->b1 * pzf->lastIn) - (pzf->a1 * pzf->lastOut);
    
    pzf->lastIn = in;
    pzf->lastOut = out;
    
    return out;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BiQuad Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tBiQuad_init(tBiQuad* const f)
{
    f->gain = 1.0f;
    
    f->b0 = 0.0f;
    f->a0 = 0.0f;
    
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
}

void    tBiQuad_free(tBiQuad* const f)
{
    leaf_free(f);
}

float   tBiQuad_tick(tBiQuad* const f, float input)
{
    float in = input * f->gain;
    float out = f->b0 * in + f->b1 * f->lastIn[0] + f->b2 * f->lastIn[1];
    out -= f->a2 * f->lastOut[1] + f->a1 * f->lastOut[0];
    
    f->lastIn[1] = f->lastIn[0];
    f->lastIn[0] = in;
    
    f->lastOut[1] = f->lastOut[0];
    f->lastOut[0] = out;
    
    return out;
}

void    tBiQuad_setResonance(tBiQuad* const f, float freq, float radius, oBool normalize)
{
    // Should also deal with frequency being > half sample rate / nyquist. See STK
    if (freq < 0.0f)    freq = 0.0f;
    if (radius < 0.0f)  radius = 0.0f;
    if (radius >= 1.0f)  radius = 1.0f;
    
    f->frequency = freq;
    f->radius = radius;
    f->normalize = normalize;
    
    f->a2 = radius * radius;
    f->a1 = -2.0f * radius * cosf(TWO_PI * freq * leaf.invSampleRate);
    
    if (normalize)
    {
        f->b0 = 0.5f - 0.5f * f->a2;
        f->b1 = 0.0f;
        f->b2 = -f->b0;
    }
}

void    tBiQuad_setNotch(tBiQuad* const f, float freq, float radius)
{
    // Should also deal with frequency being > half sample rate / nyquist. See STK
    if (freq < 0.0f)    freq = 0.0f;
    if (radius < 0.0f)  radius = 0.0f;
    
    f->b2 = radius * radius;
    f->b1 = -2.0f * radius * cosf(TWO_PI * freq * leaf.invSampleRate); // OPTIMIZE with LOOKUP or APPROXIMATION
    
    // Does not attempt to normalize filter gain.
}

void tBiQuad_setEqualGainZeros(tBiQuad* const f)
{
    f->b0 = 1.0f;
    f->b1 = 0.0f;
    f->b2 = -1.0f;
}

void    tBiQuad_setB0(tBiQuad* const f, float b0)
{
    f->b0 = b0;
}

void    tBiQuad_setB1(tBiQuad* const f, float b1)
{
    f->b1 = b1;
}

void    tBiQuad_setB2(tBiQuad* const f, float b2)
{
    f->b2 = b2;
}

void    tBiQuad_setA1(tBiQuad* const f, float a1)
{
    f->a1 = a1;
}

void    tBiQuad_setA2(tBiQuad* const f, float a2)
{
    f->a2 = a2;
}

void    tBiQuad_setCoefficients(tBiQuad* const f, float b0, float b1, float b2, float a1, float a2)
{
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
    f->a1 = a1;
    f->a2 = a2;
}

void    tBiQuad_setGain(tBiQuad* const f, float gain)
{
    f->gain = gain;
}

void    tBiQuadSampleRateChanged(tBiQuad* const f)
{
    f->a2 = f->radius * f->radius;
    f->a1 = -2.0f * f->radius * cosf(TWO_PI * f->frequency * leaf.invSampleRate);
    
    if (f->normalize)
    {
        f->b0 = 0.5f - 0.5f * f->a2;
        f->b1 = 0.0f;
        f->b2 = -f->b0;
    }
}

/* Highpass */
void     tHighpass_setFreq(tHighpass* const f, float freq)
{
    f->frequency = freq;
    f->R = (1.0f-((freq * 2.0f * 3.14f) * leaf.invSampleRate));
    
}

float     tHighpass_getFreq(tHighpass* const f)
{
    return f->frequency;
}

// From JOS DC Blocker
float   tHighpass_tick(tHighpass* const f, float x)
{
    f->ys = x - f->xs + f->R * f->ys;
    f->xs = x;
    return f->ys;
}

void    tHighpass_init(tHighpass* const f, float freq)
{
    f->R = (1.0f-((freq * 2.0f * 3.14f)* leaf.invSampleRate));
    f->ys = 0.0f;
    f->xs = 0.0f;
    
    f->frequency = freq;
}

void    tHighpass_free(tHighpass* const f)
{
    leaf_free(f);
}

void tHighpassSampleRateChanged(tHighpass* const f)
{
    f->R = (1.0f-((f->frequency * 2.0f * 3.14f) * leaf.invSampleRate));
}

float   tSVF_tick(tSVF* const svf, float v0)
{
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

// Less efficient, more accurate version of SVF, in which cutoff frequency is taken as floating point Hz value and tanh
// is calculated when frequency changes.
void tSVF_init(tSVF* const svf, SVFType type, float freq, float Q)
{
    svf->type = type;
    
    svf->ic1eq = 0;
    svf->ic2eq = 0;
    
    float a1,a2,a3,g,k;
    g = tanf(PI * freq * leaf.invSampleRate);
    k = 1.0f/Q;
    a1 = 1.0f/(1.0f+g*(g+k));
    a2 = g*a1;
    a3 = g*a2;
    
    svf->g = g;
    svf->k = k;
    svf->a1 = a1;
    svf->a2 = a2;
    svf->a3 = a3;
}

void tSVF_free(tSVF* const svf)
{
    leaf_free(svf);
}

int     tSVF_setFreq(tSVF* const svf, float freq)
{
    svf->g = tanf(PI * freq * leaf.invSampleRate);
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

int     tSVF_setQ(tSVF* const svf, float Q)
{
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

// Efficient version of tSVF where frequency is set based on 12-bit integer input for lookup in tanh wavetable.
void   tSVFE_init(tSVFE* const svf, SVFType type, uint16_t input, float Q)
{
    svf->type = type;
    
    svf->ic1eq = 0;
    svf->ic2eq = 0;
    
    float a1,a2,a3,g,k;
    g = filtertan[input];
    k = 1.0f/Q;
    a1 = 1.0f/(1.0f+g*(g+k));
    a2 = g*a1;
    a3 = g*a2;
    
    svf->g = g;
    svf->k = k;
    svf->a1 = a1;
    svf->a2 = a2;
    svf->a3 = a3;
}

float   tSVFE_tick(tSVFE* const svf, float v0)
{
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

int     tSVFE_setFreq(tSVFE* const svf, uint16_t input)
{
    svf->g = filtertan[input];
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

int     tSVFE_setQ(tSVFE* const svf, float Q)
{
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

void	tFIR_init(tFIR* const fir, float* coeffs, int numTaps){
	fir->coeff = coeffs;
    fir->numTaps = numTaps;
    fir->past = (float*)leaf_alloc(sizeof(float) * fir->numTaps);
    for (int i = 0; i < fir->numTaps; ++i) fir->past[i] = 0.0f;
}

float	tFIR_tick(tFIR* const fir, float input){
	fir->past[0] = input;
	float y = 0.0f;
	for (int i = 0; i < fir->numTaps; ++i) y += fir->past[i]*fir->coeff[i];
	for (int i = fir->numTaps-1; i > 0; --i) fir->past[i] = fir->past[i-1];
	return y;
}

void	tFIR_free(tFIR* const fir){}
