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
#include "../Inc/leaf-math.h"
#include "../leaf.h"

#endif

#ifdef ARM_MATH_CM7
#include <arm_math.h>
#endif

struct tAllpass{
    tMempool* mempool;

    float gain;

    tLinearDelay* delay;

    float lastOut;
};

struct tThiranAllpassSOCascade {

    tMempool* mempool;

    int numFilts;
    tAllpassSO* filters;
    float B;
    float iKey;
    float a[2];

    float k1[2];
    float k2[2];
    float k3[2];
    float C1[2];
    float C2[2];
    int numActiveFilters;
    int numFiltsMap[2];
    int isHigh;
    float D;
} ;

struct tFIR {

    tMempool* mempool;
    float* past;
    float* coeff;
    int numTaps;
};
struct tMedianFilter
{

    tMempool* mempool;
    float* val;
    int* age;
    int m;
    int size;
    int middlePosition;
    int last;
    int pos;
};
struct tButterworth
{
    tMempool* mempool;

    float gain;
    int order;
    int numSVF;

    tSVF* svfs;

    float f1,f2;
};
/******************************************************************************/
/*                              AllPass Filter                                */
/******************************************************************************/


void tAllpass_create(tMempool** const mp, tAllpass** const ft)
{
    ALLOC_FROM_POOL(tAllpass, ft, mp);
}

void tAllpass_init(LEAF* const leaf, tAllpass* const f, float initDelay, uint32_t maxDelay)
{
    f->gain = 0.7f;
    f->lastOut = 0.0f;
    tLinearDelay_create (&f->mempool,&f->delay);
    tLinearDelay_init(leaf, f->delay, initDelay, maxDelay);
}

void tAllpass_free (tAllpass** const ft)
{
    tAllpass *f = *ft;

    tLinearDelay_free(&f->delay);
    mpool_free((char *) f, f->mempool);
    f = NULL;
}

void tAllpass_setDelay (tAllpass* const f, float delay)
{
    tLinearDelay_setDelay(f->delay, delay);
}

void tAllpass_setGain (tAllpass* const f, float gain)
{
    f->gain = gain;
}

float tAllpass_tick (tAllpass* const f, float input)
{
    float s1 = (-f->gain) * f->lastOut + input;
    float s2 = tLinearDelay_tick(f->delay, s1) + (f->gain) * input;

    f->lastOut = s2;

    return f->lastOut;
}

/******************************************************************************/
/*                         2nd Order AllPass Filter                           */
/******************************************************************************/


void tAllpassSO_create(tMempool** const mp, tAllpassSO** const ft)
{
    ALLOC_FROM_POOL(tAllpassSO, ft, mp);
}

void tAllpassSO_init(LEAF* const leaf, tAllpassSO* const f)
{
    f->prevSamp = 0.0f;
    f->prevPrevSamp = 0.0f;
    f->a1 = 0.0f;
    f->a2 = 0.0f;
}

void tAllpassSO_free (tAllpassSO** const ft)
{
    tAllpassSO *f = *ft;
    mpool_free((char *) f, f->mempool);
}

void tAllpassSO_setCoeff (tAllpassSO* const f, float a1, float a2)
{
    //float prevSum = f->a1 + f->a2;
    //float newSum = a1+a2;
    //float ratio = 1.0f;
    ////if (prevSum != 0.0f)
    //{
    //    ratio = fabsf(newSum / prevSum);
    //}

    f->a1 = a1;
    f->a2 = a2;
    //f->prevSamp *= ratio;
    //f->prevPrevSamp *= ratio;
}

float tAllpassSO_tick (tAllpassSO* const f, float input)
{
    //DFII version, efficient but causes issues with coefficient changes happening fast (due to high gain of state variables)
    /*

    float vn = input + (f->prevSamp * -f->a1) + (f->prevPrevSamp * -f->a2);

    float output = (vn * f->a2) + (f->prevSamp * f->a1) + f->prevPrevSamp;

    f->prevPrevSamp = f->prevSamp;
    f->prevSamp = vn;
    */

    //a0 = 1.0f
    //a1 = a1
    //a2 = a2
    //b0 = a2
    //b1 = a1
    //b2 = 1.0f

    //DFII Transposed version
/*
    float vn = (input * f->a2) + f->prevSamp;
    f->prevSamp = (input * f->a1) + (vn * -f->a1) + f->prevPrevSamp;
    f->prevPrevSamp = input + (vn * -f->a2);
    return vn;]

*/
    //DFI version
    float vn = input * f->a2 + (f->prevSamp * f->a1 + f->prevPrevSamp);
    float output = vn + (f->prevSamp2 * -f->a1) + (f->prevPrevSamp2 * -f->a2);
    f->prevPrevSamp = f->prevSamp;
    f->prevSamp = input;
    f->prevPrevSamp2 = f->prevSamp2;
    f->prevSamp2 = output;

    return output;
}


/******************************************************************************/
/*              Cascade of 2nd order Thiran Allpass Filters                   */
/******************************************************************************/


void tThiranAllpassSOCascade_create(tMempool** const mp, tThiranAllpassSOCascade** const ft)
{
    ALLOC_FROM_POOL(tThiranAllpassSOCascade, ft, mp);
}

void tThiranAllpassSOCascade_init(LEAF* const leaf, tThiranAllpassSOCascade* const f, int numFilts)
{


    f->numFilts = numFilts;
    f->filters = (tAllpassSO *) mpool_calloc(sizeof(tAllpassSO) * numFilts, f->mempool);
    f->k1[0] = -0.00050469f;
    f->k2[0] = -0.0064264f;
    f->k3[0] = -2.8743f;
    f->C1[0] = 0.069618f;
    f->C2[0] = 2.0427f;
    f->k1[1] = -0.0026580f;
    f->k2[1] = -0.014811f;
    f->k3[1] = -2.9018f;
    f->C1[1] = 0.071089f;
    f->C2[1] = 2.1074f;
    f->isHigh = 0;
    f->numFiltsMap[0] = numFilts;
    f->numFiltsMap[1] = 1;
    f->numActiveFilters = numFilts;
    for (int i = 0; i < numFilts; i++) {
        tAllpassSO_init(leaf,&f->filters[i]);
    
}
}

void tThiranAllpassSOCascade_free (tThiranAllpassSOCascade** const ft)
{
    tThiranAllpassSOCascade *f = *ft;
    mpool_free((char *) f->filters, f->mempool);
    mpool_free((char *) f, f->mempool);
}

volatile float binTest;

float tThiranAllpassSOCascade_setCoeff (tThiranAllpassSOCascade* const f,
                                       float dispersionCoeff, float freq,
                                       float oversampling)
{
    f->B = dispersionCoeff;
    f->iKey = (49.0f + 12.0f * log2f(freq * INV_440));
    float iKey2 = (49.0f + 12.0f * log2f(freq * oversampling * INV_440));
    //f->iKey = logf((110.0f*twelfthRootOf2) / 27.5f)/ logf(twelfthRootOf2);
    //f->isHigh = freq > 400.0f;//switch to different coefficients for higher notes
    //float howHigh = LEAF_mapToZeroToOneOutput(iKey2, 16.0f, 76.0f);
    float howHigh = (iKey2 - 16.0f) * 0.03f;
    howHigh = LEAF_clip(0.0f, howHigh, 1.0f);
    float oneMinusHowHigh = 1.0f - howHigh;

    float k1 = (f->k1[0] * oneMinusHowHigh) + (f->k1[1] * howHigh);
    float k2 = (f->k2[0] * oneMinusHowHigh) + (f->k2[1] * howHigh);
    float k3 = (f->k3[0] * oneMinusHowHigh) + (f->k3[1] * howHigh);
    float C1 = (f->C1[0] * oneMinusHowHigh) + (f->C1[1] * howHigh);
    float C2 = (f->C2[0] * oneMinusHowHigh) + (f->C2[1] * howHigh);
    float logB = logf(f->B);
    float temp = (k1 * logB * logB) + (k2 * logB) + k3;
    float kd = fastExp3(temp);
    float Cd = fastExp3((C1 * logB) + C2);
    float D = fastExp3(Cd - (f->iKey * kd));
    f->D = D;

    float a_k = -2.0f;

    a_k *= (D - 2.0f);
    a_k /= (D - 1.0f);

    a_k *= (D - 1.0f);
    a_k /= D;

    a_k *= D;
    a_k /= (D + 1.0f);

    f->a[0] = a_k;

    a_k = 1.0f;

    a_k *= (D - 2.0f);
    a_k /= D;

    a_k *= (D - 1.0f);
    a_k /= (D + 1.0f);

    a_k *= D;
    a_k /= (D + 2.0f);

    f->a[1] = a_k;

    if (f->a[0] > 0.99f) {
        f->a[0] = 0.99f;
        f->a[1] = 0.01f;
        D = 1.0f;
    }
    //f->a[0] = LEAF_clip(0.0f, f->a[0], 1.0f);
    //f->a[1] = LEAF_clip(-1.999999f, f->a[1], 2.0f);

    for (int i = 0; i < f->numActiveFilters; i++) {
        tAllpassSO_setCoeff(&f->filters[i], f->a[0], f->a[1]);
        //f->filters[i]->prevSamp = 0.0f;
        //f->filters[i]->prevPrevSamp = 0.0f;
        //probably should adjust the gain of the internal state variables
        // (prevSamp and prevPrevSamp) if the gain total of the two coefficients
        // goes //up, since the internals of the allpass boosts gain and then
        // attenuates it, so leaving huge values in there that won't be
        // attenuated enough can make it distort or nan.
    }
    return D * f->numActiveFilters;
}

float tThiranAllpassSOCascade_tick (tThiranAllpassSOCascade* const f, float input)
{
    float sample = input;
    for (int i = 0; i < f->numActiveFilters; i++) {
        sample = tAllpassSO_tick(&f->filters[i], sample);
    }
    return sample;
}

void tThiranAllpassSOCascade_clear (tThiranAllpassSOCascade* const f)
{
    for (int i = 0; i < f->numFilts; i++) {
        //tAllpassSO_setCoeff(f->filters[i], f->a[1], f->a[2]);
        f->filters[i].prevSamp = 0.0f;
        f->filters[i].prevPrevSamp = 0.0f;

    }
}


/******************************************************************************/
/*                               OnePole Filter                               */
/******************************************************************************/


void tOnePole_create(tMempool** const mp, tOnePole** const ft)
{
    ALLOC_FROM_POOL(tOnePole, ft, mp);
}

void tOnePole_init(LEAF* const leaf, tOnePole* const f, float freq)
{


    f->gain = 1.0f;
    f->a0 = 1.0;

    f->lastIn = 0.0f;
    f->lastOut = 0.0f;

    f->twoPiTimesInvSampleRate = leaf->twoPiTimesInvSampleRate;

    tOnePole_setFreq(f, freq);

}

void tOnePole_free (tOnePole** const ft)
{
    tOnePole *f = *ft;

    mpool_free((char *) f, f->mempool);
}

void tOnePole_setB0 (tOnePole* const f, float b0)
{
    f->b0 = b0;
}

void tOnePole_setA1 (tOnePole* const f, float a1)
{
    if (a1 >= 1.0f) a1 = 0.999999f;
    f->a1 = a1;
}

void tOnePole_setPole (tOnePole* const f, float thePole)
{
    if (thePole >= 1.0f) thePole = 0.999999f;

    // Normalize coefficients for peak unity gain.
    if (thePole > 0.0f) f->b0 = (1.0f - thePole);
    else f->b0 = (1.0f + thePole);

    f->a1 = -thePole;
}

void tOnePole_setFreq (tOnePole* const f, float freq)
{
    f->freq = freq;
    f->b0 = f->freq * f->twoPiTimesInvSampleRate;
    f->b0 = LEAF_clip(0.0f, f->b0, 1.0f);
    f->a1 = 1.0f - f->b0;
}

void tOnePole_setCoefficients (tOnePole* const f, float b0, float a1)
{
    if (b0 >= 1.0f) b0 = 0.999999f;
    f->b0 = b0;
    f->a1 = a1;
}

void tOnePole_setGain (tOnePole* const f, float gain)
{
    f->gain = gain;
}

float tOnePole_tick (tOnePole* const f, float input)
{
    float in = input * f->gain;
    float out = (f->b0 * in) + (f->a1 * f->lastOut);

    f->lastIn = in;
    f->lastOut = out;

    return out;
}

void tOnePole_setSampleRate (tOnePole* const f, float sr)
{
    f->twoPiTimesInvSampleRate = (1.0f / sr) * TWO_PI;
    f->b0 = f->freq * f->twoPiTimesInvSampleRate;
    f->b0 = LEAF_clip(0.0f, f->b0, 1.0f);
    f->a1 = 1.0f - f->b0;
}


/******************************************************************************/
/*                            CookOnePole Filter                              */
/******************************************************************************/


void tCookOnePole_create(tMempool** const mp, tCookOnePole** const ft)
{
    ALLOC_FROM_POOL(tCookOnePole, ft, mp);
}

void tCookOnePole_init(LEAF* const leaf, tCookOnePole* const f)
{


    f->poleCoeff = 0.9f;
    f->sgain = 0.1f;
    f->output = 0.0f;

    f->twoPiTimesInvSampleRate = leaf->twoPiTimesInvSampleRate;

}

void tCookOnePole_free (tCookOnePole** const ft)
{
    tCookOnePole *f = *ft;

    mpool_free((char *) f, f->mempool);
}


void tCookOnePole_setPole (tCookOnePole* const onepole, float aValue)
{
    onepole->poleCoeff = aValue;
    if (onepole->poleCoeff > 0.0f)                   // Normalize gain to 1.0 max
        onepole->sgain = (1.0f - onepole->poleCoeff);
    else
        onepole->sgain = (1.0f + onepole->poleCoeff);
}

void tCookOnePole_setGain (tCookOnePole* const onepole, float gain)
{
    onepole->gain = gain;
    if (onepole->poleCoeff > 0.0f)                   // Normalize gain to 1.0 max
        onepole->sgain = ((1.0f - onepole->poleCoeff) * gain);
    else
        onepole->sgain = ((1.0f + onepole->poleCoeff) * gain);
}

void tCookOnePole_setGainAndPole (tCookOnePole* const onepole, float gain, float pole)
{
    onepole->poleCoeff = pole;
    onepole->sgain = gain;
}

float tCookOnePole_tick (tCookOnePole* const onepole, float sample)
{
    onepole->output = (onepole->sgain * sample) + (onepole->poleCoeff * onepole->output);
    return onepole->output;
}

void tCookOnePole_setSampleRate (tCookOnePole* const f, float sr)
{
    f->twoPiTimesInvSampleRate = (1.0f / sr) * TWO_PI;
}


/******************************************************************************/
/*                               TwoPole Filter                               */
/******************************************************************************/


void tTwoPole_create(tMempool** const mp, tTwoPole** const ft)
{
    ALLOC_FROM_POOL(tTwoPole, ft, mp);
}

void tTwoPole_init(LEAF* const leaf, tTwoPole* const f)
{


    f->gain = 1.0f;
    f->a0 = 1.0;
    f->b0 = 1.0;

    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;

    f->sampleRate = leaf->sampleRate;
    f->twoPiTimesInvSampleRate = leaf->twoPiTimesInvSampleRate;

}

void tTwoPole_free (tTwoPole** const ft)
{
    tTwoPole *f = *ft;
    mpool_free((char *) f, f->mempool);
}

float tTwoPole_tick (tTwoPole* const f, float input)
{
    float in = input * f->gain;
    float out = (f->b0 * in) - (f->a1 * f->lastOut[0]) - (f->a2 * f->lastOut[1]);

    f->lastOut[1] = f->lastOut[0];
    f->lastOut[0] = out;

    return out;
}

void tTwoPole_setB0 (tTwoPole* const f, float b0)
{
    f->b0 = b0;
}

void tTwoPole_setA1 (tTwoPole* const f, float a1)
{
    f->a1 = a1;
}

void tTwoPole_setA2 (tTwoPole* const f, float a2)
{
    f->a2 = a2;
}

void tTwoPole_setResonance (tTwoPole* const f, float frequency, float radius,
                            int normalize)
{
    float sampleRate = f->sampleRate;
    float twoPiTimesInvSampleRate = f->twoPiTimesInvSampleRate;

    if (frequency < 0.0f) frequency = 0.0f;
    if (frequency > (sampleRate * 0.49f)) frequency = sampleRate * 0.49f;
    if (radius < 0.0f) radius = 0.0f;
    if (radius >= 1.0f) radius = 0.999999f;

    f->radius = radius;
    f->frequency = frequency;
    f->normalize = normalize;

    f->a2 = radius * radius;
    f->a1 = -2.0f * radius * cosf(frequency * twoPiTimesInvSampleRate);

    if (normalize) {
        // Normalize the filter gain ... not terribly efficient.
        float real = 1 - radius + (f->a2 - radius) * cosf(2 * frequency * twoPiTimesInvSampleRate);
        float imag = (f->a2 - radius) * sinf(2 * frequency * twoPiTimesInvSampleRate);
        f->b0 = sqrtf(powf(real, 2) + powf(imag, 2));
    }
}

void tTwoPole_setCoefficients (tTwoPole* const f, float b0, float a1, float a2)
{
    f->b0 = b0;
    f->a1 = a1;
    f->a2 = a2;
}

void tTwoPole_setGain (tTwoPole* const f, float gain)
{
    f->gain = gain;
}

void tTwoPole_setSampleRate (tTwoPole* const f, float sr)
{
    f->twoPiTimesInvSampleRate = (1.0f / sr) * TWO_PI;

    f->a2 = f->radius * f->radius;
    f->a1 = -2.0f * f->radius * cosf(f->frequency * f->twoPiTimesInvSampleRate);

    if (f->normalize) {
        // Normalize the filter gain ... not terribly efficient.
        float real = 1 - f->radius + (f->a2 - f->radius) * cosf(2 * f->frequency * f->twoPiTimesInvSampleRate);
        float imag = (f->a2 - f->radius) * sinf(2 * f->frequency * f->twoPiTimesInvSampleRate);
        f->b0 = sqrtf(powf(real, 2) + powf(imag, 2));
    }
}


/******************************************************************************/
/*                               OneZero Filter                               */
/******************************************************************************/


void tOneZero_create(tMempool** const mp, tOneZero** const ft)
{
    ALLOC_FROM_POOL(tOneZero, ft, mp);
}

void tOneZero_init(LEAF* const leaf, tOneZero* const f, float theZero)
{


    f->gain = 1.0f;
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
    f->invSampleRate = leaf->invSampleRate;
    tOneZero_setZero(f, theZero);

}

void tOneZero_free (tOneZero** const ft)
{
    tOneZero *f = *ft;
    mpool_free((char *) f, f->mempool);
}

float tOneZero_tick (tOneZero* const f, float input)
{
    float in = input * f->gain;
    float out = f->b1 * f->lastIn + f->b0 * in;

    f->lastIn = in;

    return out;
}

void tOneZero_setZero (tOneZero* const f, float theZero)
{
    if (theZero > 0.0f) f->b0 = 1.0f / (1.0f + theZero);
    else f->b0 = 1.0f / (1.0f - theZero);

    f->b1 = -theZero * f->b0;

}

void tOneZero_setB0 (tOneZero* const f, float b0)
{
    f->b0 = b0;
}

void tOneZero_setB1 (tOneZero* const f, float b1)
{
    f->b1 = b1;
}

void tOneZero_setCoefficients (tOneZero* const f, float b0, float b1)
{
    f->b0 = b0;
    f->b1 = b1;
}

void tOneZero_setGain (tOneZero* f, float gain)
{
    f->gain = gain;
}

float tOneZero_getPhaseDelay (tOneZero* const f, float frequency)
{
    if (frequency <= 0.0f) frequency = 0.05f;

    f->frequency = frequency;

    float omegaT = 2 * PI * frequency * f->invSampleRate;
    float real = 0.0, imag = 0.0;

    real += f->b0;

    real += f->b1 * cosf(omegaT);
    imag -= f->b1 * sinf(omegaT);

    real *= f->gain;
    imag *= f->gain;

    float phase = atan2f(imag, real);

    real = 0.0;
    imag = 0.0;

    phase -= atan2f(imag, real);

    phase = fmodf(-phase, 2 * PI);

    return phase / omegaT;
}

void tOneZero_setSampleRate (tOneZero* const f, float sr)
{
    f->invSampleRate = 1.0f / sr;
}


/******************************************************************************/
/*                               TwoZero Filter                               */
/******************************************************************************/


void tTwoZero_create(tMempool** const mp, tTwoZero** const ft)
{
    ALLOC_FROM_POOL(tTwoZero, ft, mp);
}

void tTwoZero_init(LEAF* const leaf, tTwoZero* const f)
{

    f->twoPiTimesInvSampleRate = leaf->twoPiTimesInvSampleRate;
    f->gain = 1.0f;
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;

}

void tTwoZero_free (tTwoZero** const ft)
{
    tTwoZero *f = *ft;
    mpool_free((char *) f, f->mempool);
}

float tTwoZero_tick (tTwoZero* const f, float input)
{
    float in = input * f->gain;
    float out = f->b2 * f->lastIn[1] + f->b1 * f->lastIn[0] + f->b0 * in;

    f->lastIn[1] = f->lastIn[0];
    f->lastIn[0] = in;

    return out;
}

void tTwoZero_setNotch (tTwoZero* const f, float freq, float radius)
{
    // Should also deal with frequency being > half sample rate / nyquist. See STK
    if (freq < 0.0f) freq = 0.0f;
    if (radius < 0.0f) radius = 0.0f;

    f->frequency = freq;
    f->radius = radius;

    f->b2 = radius * radius;
    f->b1 = -2.0f * radius * cosf(freq * f->twoPiTimesInvSampleRate); // OPTIMIZE with LOOKUP or APPROXIMATION

    // Normalize the filter gain. From STK.
    if (f->b1 > 0.0f) // Maximum at z = 0.
        f->b0 = 1.0f / (1.0f + f->b1 + f->b2);
    else            // Maximum at z = -1.
        f->b0 = 1.0f / (1.0f - f->b1 + f->b2);
    f->b1 *= f->b0;
    f->b2 *= f->b0;

}

void tTwoZero_setB0 (tTwoZero* const f, float b0)
{
    f->b0 = b0;
}

void tTwoZero_setB1 (tTwoZero* const f, float b1)
{
    f->b1 = b1;
}

void tTwoZero_setCoefficients (tTwoZero* const f, float b0, float b1, float b2)
{
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
}

void tTwoZero_setGain (tTwoZero* const f, float gain)
{
    f->gain = gain;
}

void tTwoZero_setSampleRate (tTwoZero* const f, float sr)
{
    f->twoPiTimesInvSampleRate = TWO_PI * (1.0f / sr);
    tTwoZero_setNotch(f, f->frequency, f->radius);
}

/******************************************************************************/
/*                              PoleZero Filter                               */
/******************************************************************************/


void tPoleZero_create(tMempool** const mp, tPoleZero** const pzf)
{
    ALLOC_FROM_POOL(tPoleZero, pzf, mp);
}

void tPoleZero_init(LEAF* const leaf, tPoleZero* const f)
{
    f->gain = 1.0f;
    f->b0 = 1.0f;

    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
}

void tPoleZero_free (tPoleZero** const pzf)
{
    tPoleZero *f = *pzf;
    mpool_free((char *) f, f->mempool);
}

void tPoleZero_setB0 (tPoleZero* const f, float b0)
{
    f->b0 = b0;
}

void tPoleZero_setB1 (tPoleZero* const f, float b1)
{
    f->b1 = b1;
}

void tPoleZero_setA1 (tPoleZero* const f, float a1)
{
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }

    f->a1 = a1;
}

void tPoleZero_setCoefficients (tPoleZero* const f, float b0, float b1, float a1)
{
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }

    f->b0 = b0;
    f->b1 = b1;
    f->a1 = a1;
}

void tPoleZero_setThiranAllpassSOCascade (tPoleZero* const f, float coeff)
{
    if (coeff >= 1.0f) // allpass coefficient >= 1.0 makes filter unstable
    {
        coeff = 0.999999f;
    }

    f->b0 = coeff;
    f->b1 = 1.0f;
    f->a1 = coeff;
}

void tPoleZero_setBlockZero (tPoleZero* const f, float thePole)
{
    if (thePole >= 1.0f) // allpass coefficient >= 1.0 makes filter unstable
    {
        thePole = 0.999999f;
    }

    f->b0 = 1.0f;
    f->b1 = -1.0f;
    f->a1 = -thePole;
}

void tPoleZero_setGain (tPoleZero* const f, float gain)
{
    f->gain = gain;
}

float tPoleZero_tick (tPoleZero* const f, float input)
{
    float in = input * f->gain;
    float out = (f->b0 * in) + (f->b1 * f->lastIn) - (f->a1 * f->lastOut);

    f->lastIn = in;
    f->lastOut = out;

    return out;
}

/******************************************************************************/
/*                                BiQuad Filter                               */
/******************************************************************************/


void tBiQuad_create(tMempool** const mp, tBiQuad** const ft)
{
    ALLOC_FROM_POOL(tBiQuad, ft, mp);
}

void tBiQuad_init(LEAF* const leaf, tBiQuad* const f)
{


    f->gain = 1.0f;

    f->b0 = 0.0f;
    f->a0 = 0.0f;

    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;

    f->twoPiTimesInvSampleRate = leaf->twoPiTimesInvSampleRate;

}

void tBiQuad_free (tBiQuad** const ft)
{
    tBiQuad *f = *ft;
    mpool_free((char *) f, f->mempool);
}

float tBiQuad_tick (tBiQuad* const f, float input)
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

void tBiQuad_setResonance (tBiQuad* const f, float freq, float radius, int normalize)
{
    if (freq < 0.0f) freq = 0.0f;
    if (freq > (f->sampleRate * 0.49f))
        freq = f->sampleRate * 0.49f;
    if (radius < 0.0f) radius = 0.0f;
    if (radius >= 1.0f) radius = 1.0f;

    f->frequency = freq;
    f->radius = radius;
    f->normalize = normalize;

    f->a2 = radius * radius;
    f->a1 = -2.0f * radius * cosf(freq * f->twoPiTimesInvSampleRate);

    if (normalize) {
        f->b0 = 0.5f - 0.5f * f->a2;
        f->b1 = 0.0f;
        f->b2 = -f->b0;
    }
}

void tBiQuad_setNotch (tBiQuad* const f, float freq, float radius)
{
    if (freq < 0.0f) freq = 0.0f;
    if (freq > (f->sampleRate * 0.49f))
        freq = f->sampleRate * 0.49f;
    if (radius < 0.0f) radius = 0.0f;

    f->b2 = radius * radius;
    f->b1 = -2.0f * radius * cosf(freq * f->twoPiTimesInvSampleRate); // OPTIMIZE with LOOKUP or APPROXIMATION

    // Does not attempt to normalize filter gain.
}

void tBiQuad_setEqualGainZeros (tBiQuad** const ft)
{
    tBiQuad *f = *ft;
    f->b0 = 1.0f;
    f->b1 = 0.0f;
    f->b2 = -1.0f;
}

void tBiQuad_setB0 (tBiQuad* const f, float b0)
{
    f->b0 = b0;
}

void tBiQuad_setB1 (tBiQuad* const f, float b1)
{
    f->b1 = b1;
}

void tBiQuad_setB2 (tBiQuad* const f, float b2)
{
    f->b2 = b2;
}

void tBiQuad_setA1 (tBiQuad* const f, float a1)
{
    f->a1 = a1;
}

void tBiQuad_setA2 (tBiQuad* const f, float a2)
{
    f->a2 = a2;
}

void tBiQuad_setCoefficients (tBiQuad* const f, float b0, float b1, float b2,
                              float a1, float a2)
{
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
    f->a1 = a1;
    f->a2 = a2;
}

void tBiQuad_setGain (tBiQuad* const f, float gain)
{
    f->gain = gain;
}

void tBiQuad_setSampleRate (tBiQuad* const f, float sr)
{
    f->sampleRate = sr;
    f->twoPiTimesInvSampleRate = TWO_PI * (1.0f / f->sampleRate);

    f->a2 = f->radius * f->radius;
    f->a1 = -2.0f * f->radius * cosf(f->frequency * f->twoPiTimesInvSampleRate);

    if (f->normalize) {
        f->b0 = 0.5f - 0.5f * f->a2;
        f->b1 = 0.0f;
        f->b2 = -f->b0;
    }
}


/******************************************************************************/
/*                                 SVF Filter                                 */
/******************************************************************************/


// Less efficient, more accurate version of SVF, in which cutoff frequency is
// taken as floating point Hz value and tanf is calculated when frequency changes.
void tSVF_create(tMempool** const mp, tSVF** const svff)
{
    ALLOC_FROM_POOL(tSVF, svff, mp);
}

void tSVF_init(LEAF* const leaf, tSVF* const svf, SVFType type, float freq, float Q)
{


    svf->sampleRate = leaf->sampleRate;
    svf->invSampleRate = leaf->invSampleRate;
    svf->sampleRatio = 48000.0f / svf->sampleRate;
    svf->type = type;

    svf->ic1eq = 0.0f;
    svf->ic2eq = 0.0f;
    svf->Q = Q;
    svf->cutoff = freq;
    svf->g = tanf(PI * freq * svf->invSampleRate);
    svf->k = 1.0f / Q;
    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;

    svf->cH = 0.0f;
    svf->cB = 0.0f;
    svf->cBK = 0.0f;
    svf->cL = 1.0f;

    if (type == SVFTypeLowpass) {
        svf->cH = 0.0f;
        svf->cB = 0.0f;
        svf->cBK = 0.0f;
        svf->cL = 1.0f;
    
} else if (type == SVFTypeBandpass) {
        svf->cH = 0.0f;
        svf->cB = 1.0f;
        svf->cBK = 0.0f;
        svf->cL = 0.0f;
    } else if (type == SVFTypeHighpass) {
        svf->cH = 1.0f;
        svf->cB = 0.0f;
        svf->cBK = -1.0f;
        svf->cL = -1.0f;
    } else if (type == SVFTypeNotch) {
        svf->cH = 1.0f;
        svf->cB = 0.0f;
        svf->cBK = -1.0f;
        svf->cL = 0.0f;
    } else if (type == SVFTypePeak) {
        svf->cH = 1.0f;
        svf->cB = 0.0f;
        svf->cBK = -1.0f;
        svf->cL = -2.0f;
    }
    if (leaf->sampleRate > 90000) {
        svf->table = __filterTanhTable_96000;
    } else {
        svf->table = __filterTanhTable_48000;
    }
}

void tSVF_free (tSVF** const svff)
{
    tSVF *svf = *svff;
    mpool_free((char *) svf, svf->mempool);
}

float tSVF_tick (tSVF* const svf, float v0)
{
    float v1, v2, v3;
    v3 = v0 - svf->ic2eq;
    v1 = (svf->a1 * svf->ic1eq) + (svf->a2 * v3);
    v2 = svf->ic2eq + (svf->a2 * svf->ic1eq) + (svf->a3 * v3);
    svf->ic1eq = (2.0f * v1) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;

    return (v0 * svf->cH) + (v1 * svf->cB) + (svf->k * v1 * svf->cBK) +
            (v2 * svf->cL);
}

float tSVF_tickHP (tSVF* const svf, float v0)
{
    float v1, v2;
    v1 = svf->a1 * svf->ic1eq + svf->a2 * (v0 - svf->ic2eq);
    v2 = svf->ic2eq + svf->g * v1;
    svf->ic1eq = (2.0f * v1) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;
    return v0 - (svf->k * v1) - (v2);
}

float tSVF_tickBP (tSVF* const svf, float v0)
{
    float v1, v2;
    v1 = svf->a1 * svf->ic1eq + svf->a2 * (v0 - svf->ic2eq);
    v2 = svf->ic2eq + svf->g * v1;
    svf->ic1eq = (2.0f * v1) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;

    return v1;
}

float tSVF_tickLP (tSVF* const svf, float v0)
{
    float v1, v2;
    v1 = svf->a1 * svf->ic1eq + svf->a2 * (v0 - svf->ic2eq);
    v2 = svf->ic2eq + svf->g * v1;
    svf->ic1eq = (2.0f * v1) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;
    return v2;
}

void tSVF_setFreq (tSVF* const svf, float freq)
{
    svf->cutoff = LEAF_clip(0.0f, freq, svf->sampleRate * 0.5f);
    svf->g = tanf(PI * svf->cutoff * svf->invSampleRate);
    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void tSVF_setFreqFast (tSVF* const svf, float cutoff)
{
    svf->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f; //get 0-134 midi range to 0-4095
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;

    svf->g = ((svf->table[intVer] * (1.0f - floatVer)) +
            (svf->table[intVer + 1] * floatVer)) * svf->sampleRatio;
    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void tSVF_setQ (tSVF* const svf, float Q)
{
    svf->Q = Q;
    svf->k = 1.0f / Q;

    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void tSVF_setFreqAndQ (tSVF* const svf, float freq, float Q)
{
    svf->cutoff = LEAF_clip(0.0f, freq, svf->sampleRate * 0.5f);
    svf->k = 1.0f / Q;
    svf->g = tanf(PI * svf->cutoff * svf->invSampleRate);
    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void tSVF_setFreqAndQFast (tSVF* const svf, float cutoff, float Q)
{
    svf->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f; //get 0-134 midi range to 0-4095
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    svf->Q = Q;
    svf->k = 1.0f / Q;
    svf->g = ((svf->table[intVer] * (1.0f - floatVer)) + (svf->table[intVer + 1] * floatVer)) * svf->sampleRatio;
    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void tSVF_setFilterType (tSVF* const svf, SVFType type)
{
    if (type == SVFTypeLowpass) {
        svf->cH = 0.0f;
        svf->cB = 0.0f;
        svf->cBK = 0.0f;
        svf->cL = 1.0f;
    } else if (type == SVFTypeBandpass) {
        svf->cH = 0.0f;
        svf->cB = 1.0f;
        svf->cBK = 0.0f;
        svf->cL = 0.0f;
    } else if (type == SVFTypeHighpass) {
        svf->cH = 1.0f;
        svf->cB = 0.0f;
        svf->cBK = -1.0f;
        svf->cL = -1.0f;
    } else if (type == SVFTypeNotch) {
        svf->cH = 1.0f;
        svf->cB = 0.0f;
        svf->cBK = -1.0f;
        svf->cL = 0.0f;
    } else if (type == SVFTypePeak) {
        svf->cH = 1.0f;
        svf->cB = 0.0f;
        svf->cBK = -1.0f;
        svf->cL = -2.0f;
    }
}

void tSVF_setSampleRate (tSVF* const svf, float sr)
{
    svf->sampleRate = sr;
    svf->invSampleRate = 1.0f / svf->sampleRate;
    svf->sampleRatio = 48000.0f / svf->sampleRate;
    if (sr > 80000) {
        svf->table = __filterTanhTable_96000;
    } else {
        svf->table = __filterTanhTable_48000;
    }
}

//only works for lowpass right now
//actually doesn't work at all yet!
float tSVF_getPhaseAtFrequency (tSVF* const svf, float freq)
{
    float w = svf->invSampleRate * freq * TWO_PI;
    float c = 0.0f;
    float f = 0.0f;
    float d = 0.0f;
    float num = sinf(2.0f * w) * (c - f);
    float den = (c + f) + (2.0f * d * cosf(w)) + ((c + f) * cosf(2.0f * w));
    return atan2f(num, den);
}


/******************************************************************************/
/*                          SVF Low Pass Filter 2                             */
/******************************************************************************/


// Less efficient, more accurate version of SVF, in which cutoff frequency is taken as floating point Hz value and tanf
// is calculated when frequency changes.
void tSVF_LP_create(tMempool** const mp, tSVF_LP** const svff)
{
    ALLOC_FROM_POOL(tSVF_LP, svff, mp);
}

void tSVF_LP_init(LEAF* const leaf, tSVF_LP* const svf, float freq, float Q)
{


    svf->sampleRate = leaf->sampleRate;
    svf->invSampleRate = leaf->invSampleRate;
    svf->sampleRatio = svf->sampleRate / 48000.0f;

    svf->ic1eq = 0.0f;
    svf->ic2eq = 0.0f;
    svf->g = tanf(PI * freq * svf->invSampleRate);
    if (Q > 0.99f) {
        Q = 0.99f;
    
}
    svf->k = 2.0f * Q;
    svf->onePlusg = 1.0f + svf->g;
    svf->a0 = 1.0f / ((svf->onePlusg * svf->onePlusg) - (svf->g * svf->k));
    svf->a1 = svf->k * svf->a0;
    svf->a2 = svf->onePlusg * svf->a0;
    svf->a3 = svf->g * svf->a2;
    svf->a4 = 1.0f / svf->onePlusg;
    svf->a5 = svf->g * svf->a4;
    svf->nan = 0;

    if (leaf->sampleRate > 80000) {
        svf->table = __filterTanhTable_96000;
    } else {
        svf->table = __filterTanhTable_48000;
    }
}

void tSVF_LP_free (tSVF_LP** const svff)
{
    tSVF_LP *svf = *svff;
    mpool_free((char *) svf, svf->mempool);
}

float tSVF_LP_tick (tSVF_LP* const svf, float v0)
{
    float v1, v2;
    v1 = svf->a1 * svf->ic2eq + svf->a2 * svf->ic1eq + svf->a3 * v0;
    v2 = svf->a4 * svf->ic2eq + svf->a5 * v1;
    svf->ic1eq = (2.0f * (v1 - svf->k * v2)) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;
    return v2;
}

void tSVF_LP_setFreq (tSVF_LP* const svf, float freq)
{
    float cutoff = LEAF_clip(0.0f, freq, svf->sampleRate * 0.5f);
    svf->g = tanf(PI * cutoff * svf->invSampleRate);
    svf->onePlusg = 1.0f + svf->g;
    svf->a0 = 1.0f / ((svf->onePlusg * svf->onePlusg) - (svf->g * svf->k));
    svf->a1 = svf->k * svf->a0;
    svf->a2 = svf->onePlusg * svf->a0;
    svf->a3 = svf->g * svf->a2;
    svf->a4 = 1.0f / svf->onePlusg;
    svf->a5 = svf->g * svf->a4;


}

void tSVF_LP_setFreqFast (tSVF_LP* const svf, float cutoff)
{
    //svf->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f; //get 0-134 midi range to 0-4095
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;

    svf->g = ((svf->table[intVer] * (1.0f - floatVer)) + (svf->table[intVer + 1] * floatVer)) * svf->sampleRatio;
    svf->onePlusg = 1.0f + svf->g;
    svf->a0 = 1.0f / ((svf->onePlusg * svf->onePlusg) - (svf->g * svf->k));
    svf->a1 = svf->k * svf->a0;
    svf->a2 = svf->onePlusg * svf->a0;
    svf->a3 = svf->g * svf->a2;
    svf->a4 = 1.0f / svf->onePlusg;
    svf->a5 = svf->g * svf->a4;
}

void tSVF_LP_setQ (tSVF_LP* const svf, float Q)
{
    if (Q > 0.99f) {
        Q = 0.99f;
    }
    svf->k = 2.0f * Q;
    svf->a0 = 1.0f / ((svf->onePlusg * svf->onePlusg) - (svf->g * svf->k));
    svf->a1 = svf->k * svf->a0;
    svf->a2 = svf->onePlusg * svf->a0;
    svf->a3 = svf->g * svf->a2;
    svf->a4 = 1.0f / svf->onePlusg;
    svf->a5 = svf->g * svf->a4;
}

void tSVF_LP_setFreqAndQ (tSVF_LP* const svf, float freq, float Q)
{
    float cutoff = LEAF_clip(0.0f, freq, svf->sampleRate * 0.5f);
    if (Q > 0.99f) {
        Q = 0.99f;
    }
    svf->k = 2.0f * Q;
    svf->g = tanf(PI * cutoff * svf->invSampleRate);
    svf->onePlusg = 1.0f + svf->g;
    svf->a0 = 1.0f / ((svf->onePlusg * svf->onePlusg) - (svf->g * svf->k));
    svf->a1 = svf->k * svf->a0;
    svf->a2 = svf->onePlusg * svf->a0;
    svf->a3 = svf->g * svf->a2;
    svf->a4 = 1.0f / svf->onePlusg;
    svf->a5 = svf->g * svf->a4;
}

void tSVF_LP_setFreqAndQFast (tSVF_LP* const svf, float cutoff, float Q)
{
    //  svf->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f; //get 0-134 midi range to 0-4095
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;

    svf->g = ((svf->table[intVer] * (1.0f - floatVer)) + (svf->table[intVer + 1] * floatVer)) * svf->sampleRatio;
    if (Q > 0.99f) {
        Q = 0.99f;
    }
    svf->k = 2.0f * Q;
    svf->onePlusg = 1.0f + svf->g;
    svf->a0 = 1.0f / ((svf->onePlusg * svf->onePlusg) - (svf->g * svf->k));
    svf->a1 = svf->k * svf->a0;
    svf->a2 = svf->onePlusg * svf->a0;
    svf->a3 = svf->g * svf->a2;
    svf->a4 = 1.0f / svf->onePlusg;
    svf->a5 = svf->g * svf->a4;
}

void tSVF_LP_setSampleRate (tSVF_LP* const svf, float sr)
{
    svf->sampleRate = sr;
    svf->invSampleRate = 1.0f / svf->sampleRate;
    svf->sampleRatio = 48000.0f / sr;
    if (sr > 80000) {
        svf->table = __filterTanhTable_96000;
    } else {
        svf->table = __filterTanhTable_48000;
    }
}

//only works for lowpass right now
//actually doesn't work at all yet!
float tSVF_LP_getPhaseAtFrequency (tSVF_LP* const svf, float freq)
{
    float w = svf->invSampleRate * freq * TWO_PI;
    float c = 0.0f;
    float f = 0.0f;
    float d = 0.0f;
    float num = sinf(2.0f * w) * (c - f);
    float den = (c + f) + (2.0f * d * cosf(w)) + ((c + f) * cosf(2.0f * w));
    return atan2f(num, den);
}

#if LEAF_INCLUDE_FILTERTAN_TABLE


/******************************************************************************/
/*                           SVF Filter (Efficient)                           */
/******************************************************************************/


// Efficient version of tSVF where frequency is set based on 12-bit integer input for lookup in tanh wavetable.
void tEfficientSVF_create(tMempool** const mp, tEfficientSVF** const svff)
{
    ALLOC_FROM_POOL(tEfficientSVF, svff, mp);
}

void tEfficientSVF_init(LEAF* const leaf, tEfficientSVF* const svf, SVFType type, uint16_t input, float Q)
{


    svf->type = type;

    svf->ic1eq = 0.0f;
    svf->ic2eq = 0.0f;
    if (leaf->sampleRate > 80000) {
        svf->table = __filterTanhTable_96000;
    
} else {
        svf->table = __filterTanhTable_48000;
    }

    svf->g = svf->table[input];
    svf->k = 1.0f / Q;
    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void tEfficientSVF_free (tEfficientSVF** const svff)
{
    tEfficientSVF *svf = *svff;
    mpool_free((char *) svf, svf->mempool);
}

float tEfficientSVF_tick (tEfficientSVF* const svf, float v0)
{
    float v1, v2, v3;
    v3 = v0 - svf->ic2eq;
    v1 = (svf->a1 * svf->ic1eq) + (svf->a2 * v3);
    v2 = svf->ic2eq + (svf->a2 * svf->ic1eq) + (svf->a3 * v3);
    svf->ic1eq = (2.0f * v1) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;

    if (svf->type == SVFTypeLowpass) return v2;
    else if (svf->type == SVFTypeBandpass) return v1;
    else if (svf->type == SVFTypeHighpass) return v0 - (svf->k * v1) - v2;
    else if (svf->type == SVFTypeNotch) return v0 - (svf->k * v1);
    else if (svf->type == SVFTypePeak) return v0 - (svf->k * v1) - (2.0f * v2);
    else return 0.0f;

}

void tEfficientSVF_setFreq (tEfficientSVF* const svf, float cutoff)
{
    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    svf->g = svf->table[intVer];
    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void tEfficientSVF_setQ (tEfficientSVF* const svf, float Q)
{
    svf->k = 1.0f / Q;
    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void tEfficientSVF_setFreqAndQ (tEfficientSVF* const svf, uint16_t input, float Q)
{
    svf->g = svf->table[input];
    svf->k = 1.0f / Q;
    svf->a1 = 1.0f / (1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
}

void tEfficientSVF_setSampleRate (tEfficientSVF* const svf, float sampleRate)
{
    if (sampleRate > 80000) {
        svf->table = __filterTanhTable_96000;
    } else {
        svf->table = __filterTanhTable_48000;
    }
}

#endif // LEAF_INCLUDE_FILTERTAN_TABLE


/******************************************************************************/
/*                              Highpass Filter                               */
/******************************************************************************/


void tHighpass_create(tMempool** const mp, tHighpass** const ft)
{
    ALLOC_FROM_POOL(tHighpass, ft, mp);
}

void tHighpass_init(LEAF* const leaf, tHighpass* const f, float freq)
{


    f->twoPiTimesInvSampleRate = leaf->twoPiTimesInvSampleRate;
    f->R = (1.0f - (freq * f->twoPiTimesInvSampleRate));
    f->ys = 0.0f;
    f->xs = 0.0f;

    f->frequency = freq;

}

void tHighpass_free (tHighpass** const ft)
{
    tHighpass *f = *ft;
    mpool_free((char *) f, f->mempool);
}

void tHighpass_setFreq (tHighpass* const f, float freq)
{
    f->frequency = freq;
    f->R = (1.0f - (freq * f->twoPiTimesInvSampleRate));
}

float tHighpass_getFreq (tHighpass* const f)
{
    return f->frequency;
}

// From JOS DC Blocker
float tHighpass_tick (tHighpass* const f, float x)
{
    f->ys = x - f->xs + f->R * f->ys;
    f->xs = x;
    return f->ys;
}

void tHighpass_setSampleRate (tHighpass* const f, float sr)
{
    f->twoPiTimesInvSampleRate = TWO_PI * (1.0f / sr);
    f->R = (1.0f - (f->frequency * f->twoPiTimesInvSampleRate));
}


/******************************************************************************/
/*                            Butterworth Filter                              */
/******************************************************************************/


void tButterworth_create(tMempool** const mp, tButterworth** const ft)
{
    ALLOC_FROM_POOL(tButterworth, ft, mp);
}

void tButterworth_init(LEAF* const leaf, tButterworth* const f, int order, float f1, float f2)
{


    f->f1 = f1;
    f->f2 = f2;
    f->gain = 1.0f;

    f->numSVF = f->order = order;
    if (f1 >= 0.0f && f2 >= 0.0f) f->numSVF *= 2;

    f->svfs = (tSVF *) mpool_alloc(sizeof(tSVF) * f->numSVF, f->mempool);

    int o = 0;
    if (f1 >= 0.0f) o = f->order;
    for (int i = 0; i < f->order; ++i) {
        if (f1 >= 0.0f)
            tSVF_init(leaf,&f->svfs[i], SVFTypeHighpass, f1, 0.5f / cosf((1.0f + 2.0f * i) * PI / (4 * f->order)) );
        if (f2 >= 0.0f)
            tSVF_init(leaf,&f->svfs[i + o], SVFTypeLowpass, f2, 0.5f / cosf((1.0f + 2.0f * i) * PI / (4 * f->order)));
    
}
}

void tButterworth_free (tButterworth** const ft)
{
    tButterworth *f = *ft;
    mpool_free((char *) f->svfs, f->mempool);
    mpool_free((char *) f, f->mempool);
    f = NULL;
}

float tButterworth_tick (tButterworth* const f, float samp)
{
    for (int i = 0; i < f->numSVF; ++i)
        samp = tSVF_tick(&f->svfs[i], samp);

    return samp;
}

void tButterworth_setF1 (tButterworth* const f, float f1)
{
    if (f->f1 < 0.0f || f1 < 0.0f) return;

    f->f1 = f1;
    for (int i = 0; i < f->order; ++i) tSVF_setFreq(&f->svfs[i], f1);
}

void tButterworth_setF2 (tButterworth* const f, float f2)
{
    if (f->f2 < 0.0f || f2 < 0.0f) return;

    int o = 0;
    if (f->f1 >= 0.0f) o = f->order;
    f->f2 = f2;
    for (int i = 0; i < f->order; ++i) tSVF_setFreq(&f->svfs[i + o], f2);
}

void tButterworth_setFreqs (tButterworth* const f, float f1, float f2)
{
    tButterworth_setF1(f, f1);
    tButterworth_setF2(f, f2);
}

void tButterworth_setSampleRate (tButterworth* const f, float sr)
{
    for (int i = 0; i < f->numSVF; ++i) tSVF_setSampleRate(&f->svfs[i], sr);
}


/******************************************************************************/
/*                                FIR Filter                                  */
/******************************************************************************/


void tFIR_create(tMempool** const mp, tFIR** const firf)
{
    ALLOC_FROM_POOL(tFIR, firf, mp);
}

void tFIR_init(LEAF* const leaf, tFIR* const fir, float *coeffs, int numTaps)
{


    fir->numTaps = numTaps;
    fir->coeff = coeffs;
    fir->past = (float *) mpool_alloc(sizeof(float) * fir->numTaps, fir->mempool);
    for (int i = 0; i < fir->numTaps; ++i) fir->past[i] = 0.0f;

}

void tFIR_free (tFIR** const firf)
{
    tFIR *fir = *firf;

    mpool_free((char *) fir->past, fir->mempool);
    mpool_free((char *) fir, fir->mempool);
}

float tFIR_tick (tFIR* const fir, float input)
{
    fir->past[0] = input;
    float y = 0.0f;
    for (int i = 0; i < fir->numTaps; ++i) y += fir->past[i] * fir->coeff[i];
    for (int i = fir->numTaps - 1; i > 0; --i) fir->past[i] = fir->past[i - 1];
    return y;
}


/******************************************************************************/
/*                               Median Filter                                */
/******************************************************************************/


//---------------------------------------------
////
/// Median filter implemented based on James McCartney's median filter in Supercollider,
/// translated from a Gen~ port of the Supercollider code that I believe was made by Rodrigo Costanzo and which I got from PA Tremblay - JS


void tMedianFilter_create(tMempool** const mp, tMedianFilter** const f)
{
    ALLOC_FROM_POOL(tMedianFilter, f, mp);
}

void tMedianFilter_init(LEAF* const leaf, tMedianFilter* const f, int size)
{


    f->size = size;
    f->middlePosition = size / 2;
    f->last = size - 1;
    f->pos = -1;
    f->val = (float *) mpool_alloc(sizeof(float) * size, f->mempool);
    f->age = (int *) mpool_alloc(sizeof(int) * size, f->mempool);
    for (int i = 0; i < f->size; ++i) {
        f->val[i] = 0.0f;
        f->age[i] = i;
    
}

}

void tMedianFilter_free (tMedianFilter** const mf)
{
    tMedianFilter *f = *mf;

    mpool_free((char *) f->val, f->mempool);
    mpool_free((char *) f->age, f->mempool);
    mpool_free((char *) f, f->mempool);
}

float tMedianFilter_tick (tMedianFilter* const f, float input)
{
    for (int i = 0; i < f->size; i++) {
        int thisAge = f->age[i];
        if (thisAge == f->last) {
            f->pos = i;
        } else {
            thisAge++;
            f->age[i] = thisAge;
        }
    }

    while (f->pos != 0) {
        float test = f->val[f->pos - 1];
        if (input < test) {
            f->val[f->pos] = test;
            f->age[f->pos] = f->age[f->pos - 1];
            f->pos -= 1;
        } else { break; }
    }

    while (f->pos != f->last) {
        float test = f->val[f->pos + 1];
        if (input > test) {
            f->val[f->pos] = test;
            f->age[f->pos] = f->age[f->pos + 1];
            f->pos += 1;
        } else { break; }
    }

    f->val[f->pos] = input;
    f->age[f->pos] = 0;

    return f->val[f->middlePosition];
}


/******************************************************************************/
/*                                  VZ Filter                                 */
/******************************************************************************/


void tVZFilter_create(tMempool** const mp, tVZFilter** const vf)
{
    ALLOC_FROM_POOL(tVZFilter, vf, mp);
}

void tVZFilter_init(LEAF* const leaf, tVZFilter* const f, VZFilterType type, float freq, float bandWidth)
{

    f->sampleRate = leaf->sampleRate;
    f->invSampleRate = leaf->invSampleRate;
    f->sampRatio = 48000.0f / f->sampleRate;
    f->fc = LEAF_clip(0.0f, freq, 0.5f * f->sampleRate);
    f->cutoffMIDI = ftom(f->fc);
    f->type = type;
    f->G = INV_SQRT2;

    f->invG = 1.414213562373095f;
    f->B = bandWidth;
    f->m = 0.0f;
    f->Q = 0.5f;
    f->s1 = 0.0f;
    f->s2 = 0.0f;
    f->R2 = f->invG;
    f->R2Plusg = f->R2 + f->g;
    f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
    tVZFilter_setBandwidth(f, f->B);
    tVZFilter_calcCoeffs(f);
    if (leaf->sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    
} else {
        f->table = __filterTanhTable_48000;
    }
}

void tVZFilter_free (tVZFilter** const vf)
{
    tVZFilter *f = *vf;
    mpool_free((char *) f, f->mempool);
}

float tVZFilter_tick (tVZFilter* const f, float in)
{
    float yL, yB, yH, v1, v2;

    // compute highpass output via Eq. 5.1:
    //yH = (in - f->R2*f->s1 - f->g*f->s1 - f->s2) * f->h;
    yH = (in - (f->R2Plusg * f->s1) - f->s2) * f->h;
    // compute bandpass output by applying 1st integrator to highpass output:
    v1 = f->g * yH;
    yB = tanhf(v1) + f->s1;
    f->s1 = v1 + yB; // state update in 1st integrator

    // compute lowpass output by applying 2nd integrator to bandpass output:
    v2 = f->g * yB;
    yL = tanhf(v2) + f->s2;
    f->s2 = v2 + yL; // state update in 2nd integrator

    //according to the Vadim paper, we could add saturation to this model by adding a tanh in the integration stage.
    //
    //seems like that might look like this:
    // y = tanh(g*x) + s; // output computation
    // s = g*x + y; // state update

    //instead of this:
    // y = g*x + s; // output computation
    // s = g*x + y; // state update

    // if we wanted to go into self-oscillation with stabiity we would need to use an anti-saturator on the feedback loop, haven't figured that out yet. -JS

    return f->cL * yL + f->cB * yB + f->cH * yH;
}

float tVZFilter_tickEfficient (tVZFilter* const f, float in)
{
    float yL, yB, yH, v1, v2;

    // compute highpass output via Eq. 5.1:
    //yH = (in - f->R2*f->s1 - f->g*f->s1 - f->s2) * f->h;
    yH = (in - (f->R2Plusg * f->s1) - f->s2) * f->h;
    // compute bandpass output by applying 1st integrator to highpass output:
    v1 = f->g * yH;
    yB = v1 + f->s1;
    f->s1 = v1 + yB; // state update in 1st integrator

    // compute lowpass output by applying 2nd integrator to bandpass output:
    v2 = f->g * yB;
    yL = v2 + f->s2;
    f->s2 = v2 + yL; // state update in 2nd integrator

    return f->cL * yL + f->cB * yB + f->cH * yH;
}

void tVZFilter_calcCoeffs (tVZFilter* const f)
{
    f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)

    switch (f->type) {
        case Bypass: {
            f->R2 = f->invG;
            f->cL = 1.0f;
            f->cB = f->R2;
            f->cH = 1.0f;

        }
            break;
        case Lowpass: {
            //f->R2 = f->invG;
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            f->cL = 1.0f;
            f->cB = 0.0f;
            f->cH = 0.0f;

        }
            break;
        case Highpass: {
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            //f->R2 = f->invG;
            f->cL = 0.0f;
            f->cB = 0.0f;
            f->cH = 1.0f;
        }
            break;
        case BandpassSkirt: {
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            //f->R2 = f->invG;
            f->cL = 0.0f;
            f->cB = 1.0f;
            f->cH = 0.0f;
        }
            break;
        case BandpassPeak: {
            //f->R2 = 2.0f*tVZFilter_BandwidthToR(vf, f->B);
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            f->cL = 0.0f;
            f->cB = f->G * f->R2;
            f->cH = 0.0f;
            //f->cL = 0.0f; f->cB = f->R2; f->cH = 0.0f;
        }
            break;
        case BandReject: {
            //f->R2 = 2.0f*tVZFilter_BandwidthToR(vf, f->B);
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            f->cL = 1.0f;
            f->cB = 0.0f;
            f->cH = 1.0f;
        }
            break;
        case Bell: {
            float fl = f->fc * powf(2.0f, (-f->B) * 0.5f); // lower bandedge frequency (in Hz)
            float wl = tanf(PI * fl * f->invSampleRate);   // warped radian lower bandedge frequency /(2*fs)
            float r = f->g / wl;
            r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
            // warped upper bandedge, wc the center
            f->R2 = 2.0f * sqrtf(((r * r + 1.0f) / r - 2.0f) / (4.0f * f->G));
            f->cL = 1.0f;
            f->cB = f->R2 * f->G;
            f->cH = 1.0f;
        }
            break;
        case Lowshelf: {
            float A = sqrtf(f->G);
            f->g /= sqrtf(A);               // scale SVF-cutoff frequency for shelvers
            //f->R2 = 2*sinhf(f->B*logf(2.0f)*0.5f); if using bandwidth instead of Q
            //f->R2 = f->invQ;
            f->cL = f->G;
            f->cB = f->R2 * f->G;
            f->cH = 1.0f;

        }
            break;
        case Highshelf: {
            float A = sqrtf(f->G);
            f->g *= sqrtf(A);               // scale SVF-cutoff frequency for shelvers
            //f->R2 = 2.0f*sinhf(f->B*logf(2.0f)*0.5f); if using bandwidth instead of Q
            f->cL = 1.0f;
            f->cB = f->R2 * f->G;
            f->cH = f->G;
        }
            break;
        case Allpass: {
            //f->R2 = 2.0f*tVZFilter_BandwidthToR(vf, f->B); if using bandwidth instead of Q
            f->cL = 1.0f;
            f->cB = -f->R2;
            f->cH = 1.0f;
        }
            break;

            // experimental - maybe we must find better curves for cL, cB, cH:
        case Morph: {
            //f->R2 = 2.0f*tVZFilter_BandwidthToREfficientBP(vf, f->B);

            //f->R2 = f->invG;
            //float x = f->m-1.0f;

            float x = (2.0f * f->m - 1.0f);
            //f->cL = maximum(0.0f,(1.0f - (f->m * 2.0f)));
            //f->cH = maximum(0.0f,(f->m * 2.0f) - 1.0f);
            //f->cB = maximum(0.0f, 1.0f - f->cH - f->cL);
            //f->cL = minimum(0.0f,(1.0f - (f->m * 2.0f))) ;
            f->cL = maximum(-x, 0.0f); /*cL *= cL;*/
            f->cH = minimum(x, 0.0f); /*cH *= cH;*/
            f->cB = 1.0f - x * x;

            // bottom line: we need to test different versions for how they feel when tweaking the
            // morph parameter

            // this scaling ensures constant magnitude at the cutoff point (we divide the coefficients by
            // the magnitude response value at the cutoff frequency and scale back by the gain):
            float s = f->G * fastsqrtf(
                    (f->R2 * f->R2) / (f->cL * f->cL + f->cB * f->cB + f->cH * f->cH - 2.0f * f->cL * f->cH)) * 2.0f;
            f->cL *= s;
            f->cB *= s;
            f->cH *= s;
        }
            break;

    }
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback precomputation
}

void tVZFilter_calcCoeffsEfficientBP (tVZFilter* const f)
{
    f->g = LEAF_clip(0.001f, fabsf(fastertanf(PI * f->fc * f->invSampleRate)),
                     1000.0f);  // embedded integrator gain (Fig 3.11) // added absolute value because g can't be <=0
    f->R2 = 2.0f * tVZFilter_BandwidthToREfficientBP(f, f->B); //JS- this will ignore resonance...
    f->cB = f->R2;
    f->h = 1.0f / (1.0f + f->R2 * f->g + f->g * f->g);  // factor for feedback precomputation
}

void tVZFilter_setBandwidth (tVZFilter* const f, float B)
{
    f->B = LEAF_clip(0.0f, B, 100.0f);
    f->R2 = 2.0f * tVZFilter_BandwidthToR(f, f->B);
    tVZFilter_calcCoeffs(f);
}

void tVZFilter_setFreq (tVZFilter* const f, float freq)
{
    f->fc = LEAF_clip(1.0f, freq, 0.5f * f->sampleRate);
    tVZFilter_calcCoeffs(f);
}

void tVZFilter_setFreqFast (tVZFilter* const f, float cutoff)
{
    f->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->g = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;

    switch (f->type) {
        case Bypass: {
            f->R2 = f->invG;
            f->cL = 1.0f;
            f->cB = f->R2;
            f->cH = 1.0f;

        }
            break;
        case Lowpass: {
            //f->R2 = f->invG;
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            f->cL = 1.0f;
            f->cB = 0.0f;
            f->cH = 0.0f;

        }
            break;
        case Highpass: {
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            //f->R2 = f->invG;
            f->cL = 0.0f;
            f->cB = 0.0f;
            f->cH = 1.0f;
        }
            break;
        case BandpassSkirt: {
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            //f->R2 = f->invG;
            f->cL = 0.0f;
            f->cB = 1.0f;
            f->cH = 0.0f;
        }
            break;
        case BandpassPeak: {
            //f->R2 = 2.0f*tVZFilter_BandwidthToR(vf, f->B);
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            f->cL = 0.0f;
            f->cB = f->G * f->R2;
            f->cH = 0.0f;
            //f->cL = 0.0f; f->cB = f->R2; f->cH = 0.0f;
        }
            break;
        case BandReject: {
            //f->R2 = 2.0f*tVZFilter_BandwidthToR(vf, f->B);
            //f->g = tanf(PI * f->fc * f->invSampleRate);  // embedded integrator gain (Fig 3.11)
            f->cL = 1.0f;
            f->cB = 0.0f;
            f->cH = 1.0f;
        }
            break;
        case Bell: {
            float fl = f->fc * fastPowf(2.0f, (-f->B) * 0.5f); // lower bandedge frequency (in Hz)
            float wl = fastertanf(PI * fl * f->invSampleRate);   // warped radian lower bandedge frequency /(2*fs)
            float r = f->g / wl;
            r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
            // warped upper bandedge, wc the center
            f->R2 = 2.0f * fastsqrtf(((r * r + 1.0f) / r - 2.0f) / (4.0f * f->G));
            f->cL = 1.0f;
            f->cB = f->R2 * f->G;
            f->cH = 1.0f;
        }
            break;
        case Lowshelf: {
            float A = fastsqrtf(f->G);
            f->g /= fastsqrtf(A);               // scale SVF-cutoff frequency for shelvers
            //f->R2 = 2*sinhf(f->B*logf(2.0f)*0.5f); if using bandwidth instead of Q
            //f->R2 = f->invQ;
            f->cL = f->G;
            f->cB = f->R2 * f->G;
            f->cH = 1.0f;

        }
            break;
        case Highshelf: {
            float A = fastsqrtf(f->G);
            f->g *= fastsqrtf(A);               // scale SVF-cutoff frequency for shelvers
            //f->R2 = 2.0f*sinhf(f->B*logf(2.0f)*0.5f); if using bandwidth instead of Q
            f->cL = 1.0f;
            f->cB = f->R2 * f->G;
            f->cH = f->G;
        }
            break;
        case Allpass: {
            //f->R2 = 2.0f*tVZFilter_BandwidthToR(vf, f->B); if using bandwidth instead of Q
            f->cL = 1.0f;
            f->cB = -f->R2;
            f->cH = 1.0f;
        }
            break;

            // experimental - maybe we must find better curves for cL, cB, cH:
        case Morph: {
            //f->R2 = 2.0f*tVZFilter_BandwidthToREfficientBP(vf, f->B);

            //f->R2 = f->invG;
            //float x = f->m-1.0f;

            float x = (2.0f * f->m - 1.0f);
            //f->cL = maximum(0.0f,(1.0f - (f->m * 2.0f)));
            //f->cH = maximum(0.0f,(f->m * 2.0f) - 1.0f);
            //f->cB = maximum(0.0f, 1.0f - f->cH - f->cL);
            //f->cL = minimum(0.0f,(1.0f - (f->m * 2.0f))) ;
            f->cL = maximum(-x, 0.0f); /*cL *= cL;*/
            f->cH = minimum(x, 0.0f); /*cH *= cH;*/
            f->cB = 1.0f - x * x;

            // bottom line: we need to test different versions for how they feel when tweaking the
            // morph parameter

            // this scaling ensures constant magnitude at the cutoff point (we divide the coefficients by
            // the magnitude response value at the cutoff frequency and scale back by the gain):
            float s = f->G * fastsqrtf(
                    (f->R2 * f->R2) / (f->cL * f->cL + f->cB * f->cB + f->cH * f->cH - 2.0f * f->cL * f->cH)) * 2.0f;
            f->cL *= s;
            f->cB *= s;
            f->cH *= s;
        }
            break;

    }
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback precomputation
}


void tVZFilter_setFreqAndBandwidth (tVZFilter* const f, float freq, float bw)
{
    f->B = LEAF_clip(0.0f, bw, 100.0f);
    f->fc = LEAF_clip(0.0f, freq, 0.5f * f->sampleRate);
    tVZFilter_calcCoeffs(f);
}

void tVZFilter_setFreqAndBandwidthEfficientBP (tVZFilter* const f, float freq, float bw)
{
    f->B = LEAF_clip(0.0f, bw, 100.0f);
    f->fc = LEAF_clip(0.0f, freq, 0.5f * f->sampleRate);
    tVZFilter_calcCoeffsEfficientBP(f);
}

void tVZFilter_setGain (tVZFilter* const f, float gain)
{
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->invG = 1.0f / f->G;
    tVZFilter_calcCoeffs(f);
}

void tVZFilter_setResonance (tVZFilter* const f, float res)
{
    f->Q = LEAF_clip(0.01f, res, 100.0f);
    f->R2 = 1.0f / f->Q;
    tVZFilter_calcCoeffs(f);
}

void tVZFilter_setFrequencyAndResonance (tVZFilter* const f, float freq, float res)
{
    f->fc = LEAF_clip(0.1f, freq, 0.4f * f->sampleRate);
    f->Q = LEAF_clip(0.01f, res, 100.0f);
    f->R2 = 1.0f / f->Q;
    tVZFilter_calcCoeffs(f);
}

void tVZFilter_setFrequencyAndResonanceAndGain (tVZFilter* const f, float freq,
                                                float res, float gain)
{
    f->fc = LEAF_clip(0.1f, freq, 0.4f * f->sampleRate);
    f->Q = LEAF_clip(0.01f, res, 100.0f);
    f->R2 = 1.0f / f->Q;
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->invG = 1.0f / f->G;
    tVZFilter_calcCoeffs(f);
}

void tVZFilter_setFastFrequencyAndResonanceAndGain (tVZFilter* const f, float freq,
                                                    float res, float gain)
{
    f->Q = LEAF_clip(0.01f, res, 100.0f);
    f->R2 = 1.0f / f->Q;
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->invG = 1.0f / f->G;
    f->cutoffMIDI = freq;
    tVZFilter_setFreqFast(f, freq);
}

void tVZFilter_setFrequencyAndBandwidthAndGain (tVZFilter* const f, float freq,
                                                float BW, float gain)
{
    f->fc = LEAF_clip(0.1f, freq, 0.4f * f->sampleRate);
    //f->Q = LEAF_clip(0.01f, res, 100.0f);
    f->B = LEAF_clip(0.01, BW, 100.f);
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->invG = 1.0f / f->G;
    tVZFilter_calcCoeffs(f);
}


void tVZFilter_setFrequencyAndResonanceAndMorph (tVZFilter* const f, float freq,
                                                 float res, float morph)
                                                 {
    f->fc = LEAF_clip(0.1f, freq, 0.4f * f->sampleRate);
    f->Q = LEAF_clip(0.01f, res, 100.0f);
    f->R2 = 1.0f / f->Q;
    f->m = LEAF_clip(0.0f, morph, 1.0f);
    tVZFilter_calcCoeffs(f);
}

void tVZFilter_setMorph (tVZFilter* const f, float morph)
{
    f->m = LEAF_clip(0.0f, morph, 1.0f);
    tVZFilter_calcCoeffs(f);
}

void tVZFilter_setMorphOnly (tVZFilter* const f, float morph)
{
    f->m = LEAF_clip(0.0f, morph, 1.0f);
    //tVZFilter_calcCoeffs(vf);
}

void tVZFilter_setType (tVZFilter* const f, VZFilterType type)
{
    f->type = type;
    tVZFilter_calcCoeffs(f);
}

float tVZFilter_BandwidthToR (tVZFilter* const f, float B)
{
    float fl = f->fc * powf(2.0f, -B * 0.5f); // lower bandedge frequency (in Hz)
    float gl = tanf(PI * fl * f->invSampleRate);   // warped radian lower bandedge frequency /(2*fs)
    float r = gl / f->g;            // ratio between warped lower bandedge- and center-frequencies
    // unwarped: r = pow(2, -B/2) -> approximation for low
    // center-frequencies
    return sqrtf((1.0f - r * r) * (1.0f - r * r) / (4.0f * r * r));
}

float tVZFilter_BandwidthToREfficientBP (tVZFilter* const f, float B)
{
    float fl = f->fc * fastPowf(2.0f, -B * 0.5f); // lower bandedge frequency (in Hz)
    float gl = fastertanf(PI * fl * f->invSampleRate);   // warped radian lower bandedge frequency /(2*fs)
    float r = gl / f->g;            // ratio between warped lower bandedge- and center-frequencies
    // unwarped: r = pow(2, -B/2) -> approximation for low
    // center-frequencies
    return fastsqrtf((1.0f - r * r) * (1.0f - r * r) / (4.0f * r * r));
}

void tVZFilter_setSampleRate (tVZFilter* const f, float sr)
{
    f->sampleRate = sr;
    f->invSampleRate = 1.0f / f->sampleRate;
    f->sampRatio = 48000.0f / sr;
    if (sr > 80000) {
        f->table = __filterTanhTable_96000;
    } else {
        f->table = __filterTanhTable_48000;
    }
}


/******************************************************************************/
/*                            VZ Low Shelf Filter                             */
/******************************************************************************/


void tVZFilterLS_create(tMempool** const mp, tVZFilterLS** const vf)
{
    ALLOC_FROM_POOL(tVZFilterLS, vf, mp);
}

void tVZFilterLS_init(LEAF* const leaf, tVZFilterLS* const f, float freq, float Q, float gain)
{


    f->sampleRate = leaf->sampleRate;
    f->sampRatio = 48000.0f / f->sampleRate;
    f->invSampleRate = leaf->invSampleRate;
    f->fc = LEAF_clip(0.0f, freq, 0.5f * f->sampleRate);
    f->cutoffMIDI = ftom(f->fc);
    f->Q = Q;
    f->R2 = 1.0f / Q;
    f->s1 = 0.0f;
    f->s2 = 0.0f;
    f->gPreDiv = tanf(PI * f->fc * f->invSampleRate);
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->invSqrtA = 1.0f / (fastsqrtf(fastsqrtf(f->G)));
    f->g = f->gPreDiv * f->invSqrtA;               // scale SVF-cutoff frequency for shelvers
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
    if (leaf->sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    
} else {
        f->table = __filterTanhTable_48000;
    }
}

void tVZFilterLS_free (tVZFilterLS** const vf)
{
    tVZFilterLS *f = *vf;
    mpool_free((char *) f, f->mempool);
}

void tVZFilterLS_setSampleRate (tVZFilterLS* const f, float sampleRate)
{
    f->sampleRate = sampleRate;
    f->invSampleRate = 1.0f / sampleRate;
    f->sampRatio = 48000.0f / f->sampleRate;
    if (sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    } else {
        f->table = __filterTanhTable_48000;
    }
}

float tVZFilterLS_tick (tVZFilterLS* const f, float input)
{
    float yL, yB, yH, v1, v2;

    // compute highpass output via Eq. 5.1:
    //yH = (in - f->R2*f->s1 - f->g*f->s1 - f->s2) * f->h;
    yH = (input - (f->R2Plusg * f->s1) - f->s2) * f->h;
    // compute bandpass output by applying 1st integrator to highpass output:
    v1 = f->g * yH;
    yB = v1 + f->s1;
    f->s1 = v1 + yB; // state update in 1st integrator

    // compute lowpass output by applying 2nd integrator to bandpass output:
    v2 = f->g * yB;
    yL = v2 + f->s2;
    f->s2 = v2 + yL; // state update in 2nd integrator

    return f->G * yL + f->R2 * f->G * yB + yH;
}

void tVZFilterLS_setFreqFast (tVZFilterLS* const f, float cutoff)
{
    f->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->gPreDiv = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;

    f->g = f->gPreDiv * f->invSqrtA;               // scale SVF-cutoff frequency for shelvers
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterLS_setBandwidthSlow (tVZFilterLS* const f, float bandWidth)
{
    f->R2 = 2.0f * sinhf(bandWidth * logf(2.0f) * 0.5f);
}

void tVZFilterLS_setFreq (tVZFilterLS* const f, float freq)
{
    f->g = tanf(PI * freq * f->invSampleRate);
    f->g = f->gPreDiv * f->invSqrtA;               // scale SVF-cutoff frequency for shelvers
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterLS_setGain (tVZFilterLS* const f, float gain)
{
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->invSqrtA = 1.0f / (fastsqrtf(fastsqrtf(f->G)));
    f->g = f->gPreDiv * f->invSqrtA;               // scale SVF-cutoff frequency for shelvers
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterLS_setResonance (tVZFilterLS* const f, float res)
{
    f->Q = res;
    f->R2 = 1.0f / res;
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterLS_setFreqFastAndResonanceAndGain (tVZFilterLS* const f, float cutoff,
                                                 float res, float gain)
{
    f->cutoffMIDI = cutoff;

    f->Q = res;
    f->R2 = 1.0f / res;

    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->invSqrtA = 1.0f / (fastsqrtf(fastsqrtf(f->G)));

    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->gPreDiv = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;

    f->g = f->gPreDiv * f->invSqrtA;               // scale SVF-cutoff frequency for shelvers

    f->R2Plusg = f->R2 + f->g;

    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}


/******************************************************************************/
/*                             VZ High Shelf Filter                             */
/******************************************************************************/


void tVZFilterHS_create(tMempool** const mp, tVZFilterHS** const vf)
{
    ALLOC_FROM_POOL(tVZFilterHS, vf, mp);
}

void tVZFilterHS_init(LEAF* const leaf, tVZFilterHS* const f, float freq, float Q, float gain)
{


    f->sampleRate = leaf->sampleRate;
    f->invSampleRate = leaf->invSampleRate;
    f->sampRatio = 48000.0f / f->sampleRate;
    f->fc = LEAF_clip(0.0f, freq, 0.5f * f->sampleRate);
    f->cutoffMIDI = ftom(f->fc);
    f->Q = Q;
    f->R2 = 1.0f / Q;
    f->s1 = 0.0f;
    f->s2 = 0.0f;
    f->gPreDiv = tanf(PI * f->fc * f->invSampleRate);
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->sqrtA = (fastsqrtf(fastsqrtf(f->G)));
    f->g = f->gPreDiv * f->sqrtA;               // scale SVF-cutoff frequency for shelvers
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
    if (leaf->sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    
} else {
        f->table = __filterTanhTable_48000;
    }
}

void tVZFilterHS_free (tVZFilterHS** const vf)
{
    tVZFilterHS *f = *vf;
    mpool_free((char *) f, f->mempool);
}

void tVZFilterHS_setSampleRate (tVZFilterHS* const f, float sampleRate)
{
    f->sampleRate = sampleRate;
    f->invSampleRate = 1.0f / sampleRate;
    f->sampRatio = 48000.0f / f->sampleRate;
    if (sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    } else {
        f->table = __filterTanhTable_48000;
    }
}

float tVZFilterHS_tick (tVZFilterHS* const f, float input)
{
    float yL, yB, yH, v1, v2;

    // compute highpass output via Eq. 5.1:
    //yH = (in - f->R2*f->s1 - f->g*f->s1 - f->s2) * f->h;
    yH = (input - (f->R2Plusg * f->s1) - f->s2) * f->h;
    // compute bandpass output by applying 1st integrator to highpass output:
    v1 = f->g * yH;
    yB = v1 + f->s1;
    f->s1 = v1 + yB; // state update in 1st integrator

    // compute lowpass output by applying 2nd integrator to bandpass output:
    v2 = f->g * yB;
    yL = v2 + f->s2;
    f->s2 = v2 + yL; // state update in 2nd integrator

    return yL + f->R2 * f->G * yB + f->G * yH;
}

void tVZFilterHS_setFreqFast (tVZFilterHS* const f, float cutoff)
{
    f->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->gPreDiv = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;

    f->g = f->gPreDiv * f->sqrtA;               // scale SVF-cutoff frequency for shelvers

    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterHS_setBandwidthSlow (tVZFilterHS* const f, float bandWidth)
{
    f->R2 = 2.0f * sinhf(bandWidth * logf(2.0f) * 0.5f);
}

void tVZFilterHS_setFreq (tVZFilterHS* const f, float freq)
{
    f->gPreDiv = tanf(PI * freq * f->invSampleRate);
    f->g = f->gPreDiv * f->sqrtA;               // scale SVF-cutoff frequency for shelvers

    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterHS_setGain (tVZFilterHS* const f, float gain)
{
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->sqrtA = fastsqrtf(fastsqrtf(f->G));
    f->g = f->gPreDiv * f->sqrtA;               // scale SVF-cutoff frequency for shelvers
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterHS_setResonance (tVZFilterHS* const f, float res)
{
    f->Q = res;
    f->R2 = 1.0f / res;
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}


void tVZFilterHS_setFreqFastAndResonanceAndGain (tVZFilterHS* const f, float cutoff,
                                                 float res, float gain)
{
    f->cutoffMIDI = cutoff;

    f->Q = res;
    f->R2 = 1.0f / res;

    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->sqrtA = fastsqrtf(fastsqrtf(f->G));

    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->gPreDiv = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;

    f->g = f->gPreDiv * f->sqrtA;               // scale SVF-cutoff frequency for shelvers

    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback


}


/******************************************************************************/
/*                                VZ Bell Filter                              */
/******************************************************************************/


void tVZFilterBell_create(tMempool** const mp, tVZFilterBell** const vf)
{
    ALLOC_FROM_POOL(tVZFilterBell, vf, mp);
}

void tVZFilterBell_init(LEAF* const leaf, tVZFilterBell* const f, float freq, float BW, float gain)
{


    f->sampleRate = leaf->sampleRate;
    f->invSampleRate = leaf->invSampleRate;
    f->sampRatio = 48000.0f / f->sampleRate;
    f->fc = LEAF_clip(0.0f, freq, 0.5f * f->sampleRate);
    f->cutoffMIDI = ftom(f->fc);
    f->B = BW;
    f->s1 = 0.0f;
    f->s2 = 0.0f;
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->g = tanf(PI * freq * f->invSampleRate);
    float fl = f->fc * fastPowf(2.0f, (-f->B) * 0.5f); // lower bandedge frequency (in Hz)
    float wl = fastertanf(PI * fl * f->invSampleRate);   // warped radian lower bandedge frequency /(2*fs)
    float r = f->g / wl;
    r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
    // warped upper bandedge, wc the center
    f->rToUse = r;
    f->R2 = 2.0f * fastsqrtf(((r * r + 1.0f) / r - 2.0f) / (4.0f * f->G));
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
    if (leaf->sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    
} else {
        f->table = __filterTanhTable_48000;
    }
}

void tVZFilterBell_free (tVZFilterBell** const vf)
{
    tVZFilterBell *f = *vf;
    mpool_free((char *) f, f->mempool);
}

void tVZFilterBell_setSampleRate (tVZFilterBell* const f, float sampleRate)
{
    f->sampleRate = sampleRate;
    f->invSampleRate = 1.0f / sampleRate;
    f->sampRatio = 48000.0f / f->sampleRate;
    if (sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    } else {
        f->table = __filterTanhTable_48000;
    }
}

float tVZFilterBell_tick (tVZFilterBell* const f, float input)
{
    float yL, yB, yH, v1, v2;

    // compute highpass output via Eq. 5.1:
    //yH = (in - f->R2*f->s1 - f->g*f->s1 - f->s2) * f->h;
    yH = (input - (f->R2Plusg * f->s1) - f->s2) * f->h;
    // compute bandpass output by applying 1st integrator to highpass output:
    v1 = f->g * yH;
    yB = v1 + f->s1;
    f->s1 = v1 + yB; // state update in 1st integrator

    // compute lowpass output by applying 2nd integrator to bandpass output:
    v2 = f->g * yB;
    yL = v2 + f->s2;
    f->s2 = v2 + yL; // state update in 2nd integrator

    return yL + f->R2 * f->G * yB + yH;
}


//relies on a call to set Freq after
void tVZFilterBell_setBandwidth (tVZFilterBell* const f, float bandWidth)
{
    f->B = bandWidth;
    //float fl = f->fc*fastPowf(2.0f, (-f->B)*0.5f); // lower bandedge frequency (in Hz)
    //float wl =  fastertanf(PI*fl*f->invSampleRate);   // warped radian lower bandedge frequency /(2*fs)
    //float r  = f->g/wl;
    //r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
    // warped upper bandedge, wc the center
    //f->rToUse = r;
    //f->R2 = 2.0f*fastsqrtf(((r*r+1.0f)/r-2.0f)/(4.0f*f->G));
}

void tVZFilterBell_setFreq (tVZFilterBell* const f, float freq)
{
    f->fc = freq;
    f->g = tanf(PI * freq * f->invSampleRate);
    float fl = f->fc * fastPowf(2.0f, (-f->B) * 0.5f); // lower bandedge frequency (in Hz)
    float wl = fastertanf(PI * fl * f->invSampleRate);   // warped radian lower bandedge frequency /(2*fs)
    float r = f->g / wl;
    r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
    // warped upper bandedge, wc the center
    f->rToUse = r;
    f->R2 = 2.0f * fastsqrtf(((r * r + 1.0f) / r - 2.0f) / (4.0f * f->G));
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterBell_setFreqFast (tVZFilterBell* const f, float cutoff)
{
    f->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->g = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;
    float fl = cutoff + (12.0f * (-f->B) * 0.5f); // lower bandedge frequency (in MIDI)
    intVer = (int32_t) fl;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    floatVer = fl - (float) intVer;
    float gLower = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;

    float r = f->g / gLower;
    r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
    // warped upper bandedge, wc the center
    f->rToUse = r;
    f->R2 = 2.0f * fastsqrtf(((r * r + 1.0f) / r - 2.0f) / (4.0f * f->G));
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterBell_setFreqAndGainFast (tVZFilterBell* const f, float cutoff,
                                       float gain)
{
    f->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->g = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;
    float fl = cutoff + (12.0f * (-f->B) * 0.5f); // lower bandedge frequency (in MIDI)
    intVer = (int32_t) fl;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    floatVer = fl - (float) intVer;
    float gLower = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;

    float r = f->g / gLower;
    r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
    // warped upper bandedge, wc the center
    f->rToUse = r;
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->R2 = 2.0f * fastsqrtf(((f->rToUse * f->rToUse + 1.0f) / f->rToUse - 2.0f) / (4.0f * f->G));
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback

}

void tVZFilterBell_setGain (tVZFilterBell* const f, float gain)
{
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->R2 = 2.0f * fastsqrtf(((f->rToUse * f->rToUse + 1.0f) / f->rToUse - 2.0f) / (4.0f * f->G));
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterBell_setFrequencyAndGain (tVZFilterBell* const f, float freq, float gain)
{
    f->fc = freq;
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->g = fastertanf(PI * freq * f->invSampleRate);
    float fl = f->fc * fastPowf(2.0f, (-f->B) * 0.5f); // lower bandedge frequency (in Hz)
    float wl = fastertanf(PI * fl * f->invSampleRate);   // warped radian lower bandedge frequency /(2*fs)
    float r = f->g / wl;
    r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
    // warped upper bandedge, wc the center
    f->rToUse = r;
    f->R2 = 2.0f * fastsqrtf(((r * r + 1.0f) / r - 2.0f) / (4.0f * f->G));
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterBell_setFrequencyAndBandwidthAndGain (tVZFilterBell* const f, float freq, float bandwidth, float gain)
{
    f->fc = freq;
    f->B = bandwidth;
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->g = fastertanf(PI * freq * f->invSampleRate);
    float fl = f->fc * fastPowf(2.0f, (-f->B) * 0.5f); // lower bandedge frequency (in Hz)
    float wl = fastertanf(PI * fl * f->invSampleRate);   // warped radian lower bandedge frequency /(2*fs)
    float r = f->g / wl;
    r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
    // warped upper bandedge, wc the center
    f->rToUse = r;
    f->R2 = 2.0f * fastsqrtf(((r * r + 1.0f) / r - 2.0f) / (4.0f * f->G));
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterBell_setFreqAndBWAndGainFast (tVZFilterBell* const f, float cutoff, float BW, float gain)
{
    f->cutoffMIDI = cutoff;
    f->B = BW;
    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->g = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;
    float fl = cutoff + (12.0f * (-f->B) * 0.5f); // lower bandedge frequency (in MIDI)
    intVer = (int32_t) fl;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    floatVer = fl - (float) intVer;
    float gLower = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;

    float r = f->g / gLower;
    r *= r;    // warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
    // warped upper bandedge, wc the center
    f->rToUse = r;
    f->G = LEAF_clip(0.000001f, gain, 4000.0f);
    f->R2 = 2.0f * fastsqrtf(((f->rToUse * f->rToUse + 1.0f) / f->rToUse - 2.0f) / (4.0f * f->G));
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback

}


/******************************************************************************/
/*                                VZ BR Filter                                */
/******************************************************************************/


void tVZFilterBR_create(tMempool** const mp, tVZFilterBR** const vf)
{
    ALLOC_FROM_POOL(tVZFilterBR, vf, mp);
}

void tVZFilterBR_init(LEAF* const leaf, tVZFilterBR* const f, float freq, float Q)
{


    f->sampleRate = leaf->sampleRate;
    f->sampRatio = 48000.0f / f->sampleRate;
    f->invSampleRate = leaf->invSampleRate;
    float fc = LEAF_clip(0.0f, freq, 0.5f * f->sampleRate);
    f->cutoffMIDI = ftom(freq);
    f->s1 = 0.0f;
    f->s2 = 0.0f;
    f->R2 = 1.0f / Q;
    f->G = 1.0f;
    f->g = tanf(PI * fc * f->invSampleRate);
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback precomputation
    if (leaf->sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    
} else {
        f->table = __filterTanhTable_48000;
    }
}

void tVZFilterBR_free (tVZFilterBR** const vf)
{
    tVZFilterBR *f = *vf;
    mpool_free((char *) f, f->mempool);
}

void tVZFilterBR_setSampleRate (tVZFilterBR* const f, float sampleRate)
{
    f->sampleRate = sampleRate;
    f->invSampleRate = 1.0f / sampleRate;
    f->sampRatio = 48000.0f / f->sampleRate;
    if (sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    } else {
        f->table = __filterTanhTable_48000;
    }
}

float tVZFilterBR_tick (tVZFilterBR* const f, float input)
{
    float yL, yB, yH, v1, v2;

    // compute highpass output via Eq. 5.1:
    //yH = (in - f->R2*f->s1 - f->g*f->s1 - f->s2) * f->h;
    yH = (input - (f->R2Plusg * f->s1) - f->s2) * f->h;
    // compute bandpass output by applying 1st integrator to highpass output:
    v1 = f->g * yH;
    yB = v1 + f->s1;
    f->s1 = v1 + yB; // state update in 1st integrator

    // compute lowpass output by applying 2nd integrator to bandpass output:
    v2 = f->g * yB;
    yL = v2 + f->s2;
    f->s2 = v2 + yL; // state update in 2nd integrator

    return (yH + yL) * f->G;
}

void tVZFilterBR_setFreqFast (tVZFilterBR* const f, float cutoff)
{
    f->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->g = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback precomputation

}

void tVZFilterBR_setGain (tVZFilterBR* const f, float gain)
{
    f->G = gain;
}

void tVZFilterBR_setFreq (tVZFilterBR* const f, float freq)
{
    f->g = tanf(PI * freq * f->invSampleRate);
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}


void tVZFilterBR_setResonance (tVZFilterBR* const f, float res)
{
    f->R2 = 1.0f / res;
    f->R2Plusg = f->R2 + f->g;
    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}

void tVZFilterBR_setFreqAndResonanceFast (tVZFilterBR* const f, float cutoff, float res)
{
    f->R2 = 1.0f / res;
    f->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f;
    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->g = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;

    f->R2Plusg = f->R2 + f->g;

    f->h = 1.0f / (1.0f + (f->R2 * f->g) + (f->g * f->g));  // factor for feedback
}


/******************************************************************************/
/*                                Diode Filter                                */
/******************************************************************************/


//taken from Ivan C's model of the EMS diode ladder, based on mystran's code from KVR forums
//https://www.kvraudio.com/forum/viewtopic.php?f=33&t=349859&start=255

void tDiodeFilter_create(tMempool** const mp, tDiodeFilter** const vf)
{
    ALLOC_FROM_POOL(tDiodeFilter, vf, mp);
}

void tDiodeFilter_init(LEAF* const leaf, tDiodeFilter* const f, float cutoff, float resonance)
{


    f->invSampleRate = leaf->invSampleRate;
    f->cutoff = cutoff;
    // initialization (the resonance factor is between 0 and 8 according to the article)
    f->f = (float) tan((double) (PI * cutoff * f->invSampleRate));
    f->cutoffMIDI = ftom(cutoff);
    f->r = (7.f * resonance + 0.5f);
    f->Vt = 0.5f;
    f->n = 1.836f;
    f->zi = 0.0f; //previous input value
    f->gamma = f->Vt * f->n;
    f->s0 = 0.01f;
    f->s1 = 0.02f;
    f->s2 = 0.03f;
    f->s3 = 0.04f;
    f->g0inv = 1.f / (2.f * f->Vt);
    f->g1inv = 1.f / (2.f * f->gamma);
    f->g2inv = 1.f / (6.f * f->gamma);
    f->sampRatio = 48000.0f / leaf->sampleRate;
    if (leaf->sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    
} else {
        f->table = __filterTanhTable_48000;
    }
}

void tDiodeFilter_free (tDiodeFilter**const vf)
{
    tDiodeFilter *f = *vf;
    mpool_free((char *) f, f->mempool);
}

float tanhXdX (float x)
{
    float a = x * x;
    // IIRC I got this as Pade-approx for tanh(sqrt(x))/sqrt(x)

    float testVal = ((15.0f * a + 420.0f) * a + 945.0f);

    float output = 1.0f;

    if (testVal > 0.000001f) {
        output = testVal;

    }
    return ((a + 105.0f) * a + 945.0f) / output;
}


float tDiodeFilter_tick (tDiodeFilter* const f, float in)
{
    // the input x[n+1] is given by 'in', and x[n] by zi
    // input with half delay
    float ih = 0.5f * (in + f->zi);

    // evaluate the non-linear factors
    float t0 = f->f * tanhXdX((ih - f->r * f->s3) * f->g0inv) * f->g0inv;
    float t1 = f->f * tanhXdX((f->s1 - f->s0) * f->g1inv) * f->g1inv;
    float t2 = f->f * tanhXdX((f->s2 - f->s1) * f->g1inv) * f->g1inv;
    float t3 = f->f * tanhXdX((f->s3 - f->s2) * f->g1inv) * f->g1inv;
    float t4 = f->f * tanhXdX((f->s3) * f->g2inv) * f->g2inv;

    // This formula gives the result for y3 thanks to MATLAB
    float y3 = (f->s2 + f->s3 + t2 * (f->s1 + f->s2 + f->s3 + t1 * (f->s0 + f->s1 + f->s2 + f->s3 + t0 * in)) +
                 t1 * (2.0f * f->s2 + 2.0f * f->s3)) * t3 + f->s3 + 2.0f * f->s3 * t1 +
                t2 * (2.0f * f->s3 + 3.0f * f->s3 * t1);

    float tempy3denom =
            (t4 + t1 * (2.0f * t4 + 4.0f) + t2 * (t4 + t1 * (t4 + f->r * t0 + 4.0f) + 3.0f) + 2.0f) * t3 + t4 +
            t1 * (2.0f * t4 + 2.0f) + t2 * (2.0f * t4 + t1 * (3.0f * t4 + 3.0f) + 2.0f) + 1.0f;

    if (tempy3denom < 0.000001f) {
        tempy3denom = 0.000001f;
    }
    y3 = y3 / tempy3denom;

    if (t1 < 0.000001f) {
        t1 = 0.000001f;
    }
    if (t2 < 0.000001f) {
        t2 = 0.000001f;
    }
    if (t3 < 0.000001f) {
        t3 = 0.000001f;
    }
    // Other outputs
    float y2 = (f->s3 - (1 + t4 + t3) * y3) / (-t3);
    float y1 = (f->s2 - (1 + t3 + t2) * y2 + t3 * y3) / (-t2);
    float y0 = (f->s1 - (1 + t2 + t1) * y1 + t2 * y2) / (-t1);
    float xx = (in - f->r * y3);

    // update state
    f->s0 += 2.0f * (t0 * xx + t1 * (y1 - y0));

    f->s1 += 2.0f * (t2 * (y2 - y1) - t1 * (y1 - y0));
    f->s2 += 2.0f * (t3 * (y3 - y2) - t2 * (y2 - y1));
    f->s3 += 2.0f * (-t4 * (y3) - t3 * (y3 - y2));

    f->s0 = 1000.0f * tanhf(f->s0 * 0.001f);
    f->s1 = 1000.0f * tanhf(f->s1 * 0.001f);
    f->s2 = 1000.0f * tanhf(f->s2 * 0.001f);
    f->s3 = 1000.0f * tanhf(f->s3 * 0.001f);
    f->zi = in;
    return tanhf(y3 * f->r);
}

float tDiodeFilter_tickEfficient (tDiodeFilter* const f, float in)
{
    // the input x[n+1] is given by 'in', and x[n] by zi
    // input with half delay
    float ih = 0.5f * (in + f->zi);

    // evaluate the non-linear factors
    float t0 = f->f * tanhXdX((ih - f->r * f->s3) * f->g0inv) * f->g0inv;
    float t1 = f->f * tanhXdX((f->s1 - f->s0) * f->g1inv) * f->g1inv;
    float t2 = f->f * tanhXdX((f->s2 - f->s1) * f->g1inv) * f->g1inv;
    float t3 = f->f * tanhXdX((f->s3 - f->s2) * f->g1inv) * f->g1inv;
    float t4 = f->f * tanhXdX((f->s3) * f->g2inv) * f->g2inv;

    // This formula gives the result for y3 thanks to MATLAB
    float y3 = (f->s2 + f->s3 + t2 * (f->s1 + f->s2 + f->s3 + t1 * (f->s0 + f->s1 + f->s2 + f->s3 + t0 * in)) +
                 t1 * (2.0f * f->s2 + 2.0f * f->s3)) * t3 + f->s3 + 2.0f * f->s3 * t1 +
                t2 * (2.0f * f->s3 + 3.0f * f->s3 * t1);

    float tempy3denom =
            (t4 + t1 * (2.0f * t4 + 4.0f) + t2 * (t4 + t1 * (t4 + f->r * t0 + 4.0f) + 3.0f) + 2.0f) * t3 + t4 +
            t1 * (2.0f * t4 + 2.0f) + t2 * (2.0f * t4 + t1 * (3.0f * t4 + 3.0f) + 2.0f) + 1.0f;

    if (tempy3denom < 0.000001f) {
        tempy3denom = 0.000001f;
    }

    y3 = y3 / tempy3denom;

    if (t1 < 0.000001f) {
        t1 = 0.000001f;
    }
    if (t2 < 0.000001f) {
        t2 = 0.000001f;
    }
    if (t3 < 0.000001f) {
        t3 = 0.000001f;
    }

    // Other outputs
    float y2 = (f->s3 - (1 + t4 + t3) * y3) / (-t3);
    float y1 = (f->s2 - (1 + t3 + t2) * y2 + t3 * y3) / (-t2);
    float y0 = (f->s1 - (1 + t2 + t1) * y1 + t2 * y2) / (-t1);
    float xx = (in - f->r * y3);

    // update state
    f->s0 += 2.0f * (t0 * xx + t1 * (y1 - y0));
    f->s1 += 2.0f * (t2 * (y2 - y1) - t1 * (y1 - y0));
    f->s2 += 2.0f * (t3 * (y3 - y2) - t2 * (y2 - y1));
    f->s3 += 2.0f * (-t4 * (y3) - t3 * (y3 - y2));

    f->s0 = 1000.0f * fast_tanh(f->s0 * 0.001f);
    f->s1 = 1000.0f * fast_tanh(f->s1 * 0.001f);
    f->s2 = 1000.0f * fast_tanh(f->s2 * 0.001f);
    f->s3 = 1000.0f * fast_tanh(f->s3 * 0.001f);

    f->zi = in;
    return fast_tanh5(y3 * f->r);
}

void tDiodeFilter_setFreq (tDiodeFilter* const f, float cutoff)
{
    f->cutoff = LEAF_clip(40.0f, cutoff, 18000.0f);
    f->f = tanf(TWO_PI * f->cutoff * f->invSampleRate);
}

void tDiodeFilter_setFreqFast (tDiodeFilter* const f, float cutoff)
{
    cutoff = LEAF_clip(10.0f, cutoff + 11.13f, 140.0f);//compensate for tuning error
    f->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f; // span of midinotes 0-134.0f (frequency range up to around 19000.0f)

    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;
    f->f = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampRatio;
}

void tDiodeFilter_setQ (tDiodeFilter* const f, float resonance)
{
    //f->r = LEAF_clip(0.5f, (7.0f * resonance + 0.5f), 20.0f);
    f->r = LEAF_clip(0.5f, resonance * 2.0f, 20.0f);
}

void tDiodeFilter_setSampleRate(tDiodeFilter* const f, float sr) {
    f->invSampleRate = 1.0f / sr;
    f->sampRatio = 48000.0f / sr;
    if (sr > 80000) {
        f->table = __filterTanhTable_96000;
    } else {
        f->table = __filterTanhTable_48000;
    }
}


/******************************************************************************/
/*                               Ladder Filter                                */
/******************************************************************************/


void tLadderFilter_create(tMempool** const mp, tLadderFilter** const vf)
{
    ALLOC_FROM_POOL(tLadderFilter, vf, mp);
}

void tLadderFilter_init(LEAF* const leaf, tLadderFilter* const f, float cutoff, float resonance)
{


    f->invSampleRate = leaf->invSampleRate;
    f->sampleRatio = 48000.0f / leaf->sampleRate;
    f->cutoff = cutoff;
    f->cutoffMIDI = ftom(cutoff);
    f->oversampling = 1;
    f->invOS = 1.0f;
    f->c = (float) tan((double) (PI * (cutoff / (float) f->oversampling) * f->invSampleRate));
    f->c2 = 2.0f * f->c;
    //resonance / feedback is from 0 to 4 for 100%, further "drives" feedback
    f->fb = (resonance * 8.0f);

    // shaper coefficients, offset, scale, shape
    // very quick approximation, close enough for me to tanh
    // yet far more flexible
    f->a = 2.0f;
    f->s = 0.1f;
    f->d = 1.0f;


    f->b[0] = 0.01f;
    f->b[0] = 0.02f;
    f->b[0] = 0.03f;
    f->b[0] = 0.04f;
    if (leaf->sampleRate > 80000) {
        f->table = __filterTanhTable_96000;
    
} else {
        f->table = __filterTanhTable_48000;
    }
}

void tLadderFilter_free (tLadderFilter** const vf)
{
    tLadderFilter *f = *vf;
    mpool_free((char *) f, f->mempool);
}

float smoothABS (float x, const float y) // y controls 'smoothness' usually between 0.002 -> 0.04
{
    //possible speedup with sqrt CMSIS-DSP approximation? seems to resolve to just a normal call to sqrt. Maybe the vector version since there are two square roots to determine? -JS
//	#ifdef ARM_MATH_CM7
//		float output1;
//		float output2;
//		arm_sqrt_f32((x * x)  + y, &output1);
//		arm_sqrt_f32(y, &output2);
//		return output1 - output2;
//	#else
    return (sqrtf((x * x) + y)) - sqrtf(y);
//	#endif
}

float smoothclip (float x, const float a, const float b) // assuming symmetrical clipping
{
    float x1 = smoothABS(x - a, 0.01f);
    float x2 = smoothABS(x - b, 0.01f);
    x = x1 + (a + b);
    x = x - x2;
    x = x * 0.5f;
    return (x);
}

float tanhd (const float x, const float d, const float s)
{
    return 1.0f - s * (d + 1.0f) * x * x / (d + x * x);
}

float tLadderFilter_tick (tLadderFilter* const f, float in)
{
    float y3 = 0.0f;
    in += 0.015f;
    // per-sample computation
    for (int i = 0; i < f->oversampling; i++) {
        float t0 = tanhd(f->b[0] + f->a, f->d, f->s);
        float t1 = tanhd(f->b[1] + f->a, f->d, f->s);
        float t2 = tanhd(f->b[2] + f->a, f->d, f->s);
        float t3 = tanhd(f->b[3] + f->a, f->d, f->s);

        float den0 = 1.0f / (1.0f + f->c * t0);
        float den1 = 1.0f / (1.0f + f->c * t1);
        float den2 = 1.0f / (1.0f + f->c * t2);
        float den3 = 1.0f / (1.0f + f->c * t3);
        float g0 = 1.0f * den0;
        float g1 = 1.0f * den1;
        float g2 = 1.0f * den2;
        float g3 = 1.0f * den3;

        float z0 = f->c * t0 * den0;
        float z1 = f->c * t1 * den1;
        float z2 = f->c * t2 * den2;
        float z3 = f->c * t3 * den3;

        float f3 = f->c * t2 * g3;
        float f2 = f->c * f->c * t1 * g2 * t2 * g3;
        float f1 = f->c * f->c * f->c * t0 * g1 * t1 * g2 * t2 * g3;
        float f0 = f->c * f->c * f->c * f->c * g0 * t0 * g1 * t1 * g2 * t2 * g3;

        float estimate =
                g3 * f->b[3] +
                f3 * g2 * f->b[2] +
                f2 * g1 * f->b[1] +
                f1 * g0 * f->b[0] +
                f0 * in;

        // feedback gain coefficient, absolutely critical to get this correct
        // i believe in the original this is computed incorrectly?
        float cgfbr = 1.0f / (1.0f + f->fb * z0 * z1 * z2 * z3);

        // clamp can be a hard clip, a diode + highpass is better
        // if you implement a highpass do not forget to include it in the computation of the gain coefficients!
        float xx = in - smoothclip(f->fb * estimate, -1.0f, 1.0f) * cgfbr;
        float y0 = t0 * g0 * (f->b[0] + f->c * xx);
        float y1 = t1 * g1 * (f->b[1] + f->c * y0);
        float y2 = t2 * g2 * (f->b[2] + f->c * y1);
        y3 = t3 * g3 * (f->b[3] + f->c * y2);

        // update the stored state
        f->b[0] += f->c2 * (xx - y0);
        f->b[1] += f->c2 * (y0 - y1);
        f->b[2] += f->c2 * (y1 - y2);
        f->b[3] += f->c2 * (y2 - y3);
    }

    // you must limit the compensation if feedback is clamped
    float compensation = 1.0f + smoothclip(f->fb, 0.0f, 4.0f);
    return fast_tanh5(y3 * compensation);
}

void tLadderFilter_setFreq (tLadderFilter* const f, float cutoff)
{
    f->cutoff = LEAF_clip(40.0f, cutoff, 18000.0f);
    f->c = tanf(PI * (f->cutoff * f->invOS) * f->invSampleRate);
    f->c2 = 2.0f * f->c;
}

void tLadderFilter_setFreqFast (tLadderFilter* const f, float cutoff)
{
    cutoff += 3.0f;
    f->cutoffMIDI = cutoff;
    cutoff *= 30.567164179104478f;

    int32_t intVer = (int32_t) cutoff;
    if (intVer > 4094) {
        intVer = 4094;
    }
    if (intVer < 0) {
        intVer = 0;
    }
    float floatVer = cutoff - (float) intVer;

    f->c = ((f->table[intVer] * (1.0f - floatVer)) + (f->table[intVer + 1] * floatVer)) * f->sampleRatio;
    f->c2 = 2.0f * f->c;
}

void tLadderFilter_setQ (tLadderFilter* const f, float resonance)
{
    f->fb = LEAF_clip(0.2f, resonance, 24.0f);
}

void tLadderFilter_setSampleRate (tLadderFilter* const f, float sr)
{
    f->invSampleRate = 1.0f / sr;
    f->sampleRatio = 48000.0f / sr * f->invOS;
    if (sr > 80000) {
        f->table = __filterTanhTable_96000;
    } else {
        f->table = __filterTanhTable_48000;
    }
}


void tLadderFilter_setOversampling (tLadderFilter* const f, int os)
{
    f->oversampling = os;
    f->invOS = 1.0f / ((float) os);
    float sr = f->oversampling / f->invSampleRate;
    if (sr > 80000) {
        f->table = __filterTanhTable_96000;
    } else {
        f->table = __filterTanhTable_48000;
    }

}


/******************************************************************************/
/*                                Tilt Filter                                 */
/******************************************************************************/


void tTiltFilter_create(tMempool** const mp, tTiltFilter** const vf)
{
    ALLOC_FROM_POOL(tTiltFilter, vf, mp);
}

void tTiltFilter_init(LEAF* const leaf, tTiltFilter* const f, float cutoff)
{

    f->cutoff = cutoff;
    f->invAmp = 1.0f / (6.0f / logf(2.0f));
    f->gfactor = 5.0f;


    f->sr3 = leaf->sampleRate * 3.0f;
    float omega = TWO_PI * cutoff;
    float n = 1.0f / (f->sr3 + omega);
    f->a0 = 2.0f * omega * n;
    f->b1 = (f->sr3 - omega) * n;

}

void tTiltFilter_free (tTiltFilter** const vf)
{
    tTiltFilter *f = *vf;
    mpool_free((char *) f, f->mempool);
}


float tTiltFilter_tick (tTiltFilter* const f, float in)
{
    f->lp_out = f->a0 * in + f->b1 * f->lp_out;
    return in + f->lgain * f->lp_out + f->hgain * (in - f->lp_out);
}

void tTiltFilter_setTilt (tTiltFilter* const f, float tilt)
{
    float g1;
    float g2;
    if (tilt > 0.0f) {
        g1 = -f->gfactor * tilt;
        g2 = tilt;
    } else {
        g1 = -tilt;
        g2 = f->gfactor * tilt;
    }

    //two separate gains
    f->lgain = fasterexpf(g1 * f->invAmp) - 1.0f;
    f->hgain = fasterexpf(g2 * f->invAmp) - 1.0f;
}

void tTiltFilter_setSampleRate (tTiltFilter* const f, float sr)
{
    f->sr3 = sr * 3.0f;
    float omega = TWO_PI * f->cutoff;
    float n = 1.0f / (f->sr3 + omega);
    f->a0 = 2.0f * omega * n;
    f->b1 = (f->sr3 - omega) * n;
}
