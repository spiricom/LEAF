/*==============================================================================
 leaf-oscillators.c
 Created: 20 Jan 2017 12:00:58pm
 Author:  Michael R Mulshine
 ==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-oscillators.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-oscillators.h"
#include "../leaf.h"

#endif

#ifdef ARM_MATH_CM7
#include "arm_math.h"
#endif
#include "string.h"
struct tWaveTable{
        tMempool* mempool;

float* baseTable;
float** tables;
int size;
int sizeMask;
int numTables;
float maxFreq;
float baseFreq, invBaseFreq;
tButterworth* bl;
float sampleRate;
    } ;
struct tWaveTableS{
    tMempool* mempool;

    float* baseTable;
    float** tables;
    int numTables;
    int* sizes;
    int* sizeMasks;
    float maxFreq;
    float baseFreq, invBaseFreq;
    tButterworth* bl;
    float dsBuffer[2];
    tOversampler* ds;
    float sampleRate;
};

static inline float blamp0(float x) {
  return 1.0f / 3.0f * x * x * x;
}

static inline float blamp1(float x) {
  x = x - 1.0f;
  return -1.0f / 3.0f * x * x * x;
}

//PolyBLEP Waveform generator ported from the Jesusonic code by Tale
//http://www.taletn.com/reaper/mono_synth/
//from Martin Finke's githubb polyblep project
// Derived from blep().
static inline float blamp(float t, float dt) {
    if (t < dt) {
        t = (t / dt) - 1.0f;
        return -1.0f / 3.0f * t * t * t;
    } else if (t > (1.0f - dt)) {
        t = (t - 1.0f) / dt + 1.0f;
        return 1.0f / 3.0f * t * t * t;
    } else {
        return 0.0f;
    }
}

#if LEAF_INCLUDE_SINE_TABLE
// Cycle
void tCycle_create(tMempool** const mp, tCycle** const cy)
{
    ALLOC_FROM_POOL(tCycle, cy, mp);
}

void tCycle_init(LEAF* const leaf, tCycle* const c)
{


    c->inc      =  0;
    c->phase    =  0;
    c->invSampleRateTimesTwoTo32 = (leaf->invSampleRate * TWO_TO_32);
    c->mask = SINE_TABLE_SIZE - 1;

}

void    tCycle_free (tCycle** const cy)
{
    tCycle* c = *cy;

    mpool_free((char*)c, c->mempool);
}

//need to check bounds and wrap table properly to allow through-zero FM
float   tCycle_tick(tCycle* const c)
{
    uint32_t tempFrac;
    uint32_t idx;
    float samp0;
    float samp1;

    // Phasor increment
    c->phase += c->inc;
    // Wavetable synthesis
    idx = c->phase >> 21; //11 bit table
    tempFrac = (c->phase & 2097151u); //(2^21 - 1) all the lower bits i.e. the remainder of a division by 2^21  (2097151 is the 21 bits after the 11 bits that represent the main index)

    samp0 = __leaf_table_sinewave[idx];
    idx = (idx + 1) & c->mask;
    samp1 = __leaf_table_sinewave[idx];

    return (samp0 + (samp1 - samp0) * ((float)tempFrac * 0.000000476837386f)); // 1/2097151
}

void     tCycle_setFreq(tCycle* const c, float freq)
{

    //if (!isfinite(freq)) return;

    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tCycle_setPhase(tCycle* const c, float phase)
{

    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tCycle_setSampleRate (tCycle* const c, float sr)
{

    c->invSampleRateTimesTwoTo32 = (1.0f/sr) * TWO_TO_32;
    tCycle_setFreq(c, c->freq);
}
#endif // LEAF_INCLUDE_SINE_TABLE

#if LEAF_INCLUDE_TRIANGLE_TABLE
//========================================================================
/* Triangle */
void tTriangle_create(tMempool** const mp, tTriangle** const cy)
{
    ALLOC_FROM_POOL(tTriangle, cy, mp);
}

void tTriangle_init(LEAF* const leaf, tTriangle* const c)
{


    c->inc      =  0;
    c->phase    =  0;
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = (c->invSampleRate * TWO_TO_32);
    c->mask = TRI_TABLE_SIZE - 1;
    tTriangle_setFreq(c, 220);

}

void    tTriangle_free  (tTriangle** const cy)
{
    tTriangle* c = *cy;

    mpool_free((char*)c, c->mempool);
}

float   tTriangle_tick(tTriangle* c)
{
    uint32_t idx;
    float frac;
    float samp0;
    float samp1;

    // Phasor increment
    c->phase += c->inc;
    // Wavetable synthesis
    idx = c->phase >> 21;
    uint32_t idx2 = (idx + 1) & c->mask;
    uint32_t tempFrac = (c->phase & 2097151);
    frac = (float)tempFrac * 0.000000476837386f;// 1/2097151 (2097151 is the 21 bits after the 11 bits that represent the main index)

    samp0 = __leaf_table_triangle[c->oct][idx];
    samp1 = __leaf_table_triangle[c->oct][idx2];
    float oct0 = (samp0 + (samp1 - samp0) * frac);

    samp0 = __leaf_table_triangle[c->oct+1][idx];
    samp1 = __leaf_table_triangle[c->oct+1][idx2];
    float oct1 = (samp0 + (samp1 - samp0) * frac);

    return oct0 + (oct1 - oct0) * c->w;
}

void tTriangle_setFreq(tTriangle* c, float freq)
{
    c->freq = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;

    // abs for negative frequencies
    c->w = fabsf(c->freq * (TRI_TABLE_SIZE * c->invSampleRate));

    c->w = log2f_approx(c->w);//+ LEAF_SQRT2 - 1.0f; adding an offset here will shift our table selection upward, reducing aliasing but lower high freq fidelity. +1.0f should remove all aliasing
    if (c->w < 0.0f) c->w = 0.0f;
    c->oct = (int)c->w;
    c->w -= c->oct;
    if (c->oct >= 10) c->oct = 9;
}

void tTriangle_setPhase(tTriangle* c, float phase)
{
    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tTriangle_setSampleRate (tTriangle* c, float sr)
{

    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tTriangle_setFreq(c, c->freq);
}
#endif // LEAF_INCLUDE_TRIANGLE_TABLE

#if LEAF_INCLUDE_SQUARE_TABLE
//========================================================================
/* Square */
void tSquare_create(tMempool** const mp, tSquare** const cy)
{
    ALLOC_FROM_POOL(tSquare, cy, mp);
}

void tSquare_init(LEAF* const leaf, tSquare* const c)
{


    c->inc      =  0;
    c->phase    =  0;
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = (c->invSampleRate * TWO_TO_32);
    c->mask = SQR_TABLE_SIZE - 1;
    tSquare_setFreq(c, 220);

}

void    tSquare_free (tSquare** const cy)
{
    tSquare* c = *cy;

    mpool_free((char*)c, c->mempool);
}

float   tSquare_tick(tSquare* c)
{

    uint32_t idx;
    float frac;
    float samp0;
    float samp1;

    // Phasor increment
    c->phase += c->inc;
    // Wavetable synthesis
    idx = c->phase >> 21;
    uint32_t idx2 = (idx + 1) & c->mask;
    uint32_t tempFrac = (c->phase & 2097151);
    frac = (float)tempFrac * 0.000000476837386f;// 1/2097151 (2097151 is the 21 bits after the 11 bits that represent the main index)

    samp0 = __leaf_table_squarewave[c->oct][idx];
    samp1 = __leaf_table_squarewave[c->oct][idx2];
    float oct0 = (samp0 + (samp1 - samp0) * frac);

    samp0 = __leaf_table_squarewave[c->oct+1][idx];
    samp1 = __leaf_table_squarewave[c->oct+1][idx2];
    float oct1 = (samp0 + (samp1 - samp0) * frac);

    return oct0 + (oct1 - oct0) * c->w;
}

void    tSquare_setFreq(tSquare* c, float freq)
{

    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;

    // abs for negative frequencies
    c->w = fabsf(c->freq * (SQR_TABLE_SIZE * c->invSampleRate));

    c->w = log2f_approx(c->w);//+ LEAF_SQRT2 - 1.0f; adding an offset here will shift our table selection upward, reducing aliasing but lower high freq fidelity. +1.0f should remove all aliasing
    if (c->w < 0.0f) c->w = 0.0f;
    c->oct = (int)c->w;
    c->w -= c->oct;
    if (c->oct >= 10) c->oct = 9;
}

void    tSquare_setPhase(tSquare* c, float phase)
{

    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tSquare_setSampleRate (tSquare* c, float sr)
{

    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tSquare_setFreq(c, c->freq);
}
#endif // LEAF_INCLUDE_SQUARE_TABLE

#if LEAF_INCLUDE_SAWTOOTH_TABLE
//=====================================================================
// Sawtooth
void tSawtooth_create(tMempool** const mp, tSawtooth** const cy)
{
    ALLOC_FROM_POOL(tSawtooth, cy, mp);
}

void tSawtooth_init(LEAF* const leaf, tSawtooth* const c)
{


    c->inc      = 0;
    c->phase    = 0;
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = (c->invSampleRate * TWO_TO_32);
    c->mask = SAW_TABLE_SIZE - 1;
    tSawtooth_setFreq(c, 220);

}

void    tSawtooth_free (tSawtooth** const cy)
{
    tSawtooth* c = *cy;

    mpool_free((char*)c, c->mempool);
}

float   tSawtooth_tick(tSawtooth* c)
{

    uint32_t idx;
    float frac;
    float samp0;
    float samp1;

    // Phasor increment
    c->phase += c->inc;
    // Wavetable synthesis
    idx = c->phase >> 21;
    uint32_t idx2 = (idx + 1) & c->mask;
    uint32_t tempFrac = (c->phase & 2097151);
    frac = (float)tempFrac * 0.000000476837386f; // 1/2097151 (2097151 is the 21 bits after the 11 bits that represent the main index)

    samp0 = __leaf_table_sawtooth[c->oct][idx];
    samp1 = __leaf_table_sawtooth[c->oct][idx2];
    float oct0 = (samp0 + (samp1 - samp0) * frac);

    samp0 = __leaf_table_sawtooth[c->oct+1][idx];
    samp1 = __leaf_table_sawtooth[c->oct+1][idx2];
    float oct1 = (samp0 + (samp1 - samp0) * frac);


    return oct0 + (oct1 - oct0) * c->w;
}

void    tSawtooth_setFreq(tSawtooth* c, float freq)
{

    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;

    // abs for negative frequencies
    c->w = fabsf(c->freq * (SAW_TABLE_SIZE * c->invSampleRate));

    c->w = log2f_approx(c->w);//+ LEAF_SQRT2 - 1.0f; adding an offset here will shift our table selection upward, reducing aliasing but lower high freq fidelity. +1.0f should remove all aliasing
    if (c->w < 0.0f) c->w = 0.0f; // If c->w is < 0.0f, then freq is less than our base freq
    c->oct = (int)c->w;
    c->w -= c->oct;
    if (c->oct >= 10) c->oct = 9;
}

void tSawtooth_setPhase(tSawtooth* c, float phase)
{

    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tSawtooth_setSampleRate (tSawtooth* c, float sr)
{

    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tSawtooth_setFreq(c, c->freq);
}

#endif // LEAF_INCLUDE_SAWTOOTH_TABLE

//==============================================================================

/* tTri: Anti-aliased Triangle waveform. */
void tPBTriangle_create(tMempool** const mp, tPBTriangle** const osc)
{
    ALLOC_FROM_POOL(tPBTriangle, osc, mp);
}

void tPBTriangle_init(LEAF* const leaf, tPBTriangle* const c)
{

    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->width     =  (0.5f * TWO_TO_32);
    c->oneMinusWidth =  c->width;
    c->freq = 0.0f;

}

void    tPBTriangle_free (tPBTriangle** const cy)
{
    tPBTriangle* c = *cy;

    mpool_free((char*)c, c->mempool);
}

#ifdef ITCMRAM
    float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBTriangle_tick          (tPBTriangle* const osc)
#else
float   tPBTriangle_tick          (tPBTriangle* c)
#endif

{

    uint32_t halfWidth =(c->width >> 1);
    float floatWidth = c->width * INV_TWO_TO_32;
    uint32_t t1 = c->phase + halfWidth;

    uint32_t t2 = c->phase + (4294967296u - halfWidth);

    float t1F = t1 * INV_TWO_TO_32;
    float t2F = t2 * INV_TWO_TO_32;
    float t = c->phase * INV_TWO_TO_32;
    float incFloat = c->inc * INV_TWO_TO_32;

    float y = t * 2.0f;

    if (y >= 2.0f - floatWidth) {
        y = (y - 2.0f) / floatWidth;
    } else if (y >= floatWidth) {
        y = 1.0f - (y - floatWidth) / (1.0f - floatWidth);
    } else {
        y /= floatWidth;
    }
    float blampOne = blamp(t1F, incFloat);
    float blampTwo = blamp(t2F, incFloat);
    float scaling = incFloat / (floatWidth - floatWidth * floatWidth) ;
    y += scaling * (blampOne - blampTwo);
    c->phase += c->inc;
    return y;
}

#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBTriangle_setFreq       (tPBTriangle* const osc, float freq)
#else
void    tPBTriangle_setFreq       (tPBTriangle* c, float freq)
#endif
{

    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tPBTriangle_setSkew       (tPBTriangle* c, float width)
{
    width = width*0.5f + 0.5f;
    width = LEAF_clip(0.01f, width, 0.99f);
    c->oneMinusWidth = (1.0f - width) * TWO_TO_32;
    c->width = width * TWO_TO_32;
}

void     tPBTriangle_setSampleRate (tPBTriangle* c, float sr)
{

    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBTriangle_setFreq(c, c->freq);
}


//==============================================================================

/* tPBSineTriangle: Anti-aliased Triangle waveform with sine wave, crossfaded between them with shape param. */
void tPBSineTriangle_create(tMempool** const mp, tPBSineTriangle** const osc)
{
    ALLOC_FROM_POOL(tPBSineTriangle, osc, mp);
}

void tPBSineTriangle_init(LEAF* const leaf, tPBSineTriangle* const c)
{

    tCycle_init(leaf,&c->sine);
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->freq = 0.0f;
    c->shape     =  0.0f;
    c->oneMinusShape = 1.0f;

}

void    tPBSineTriangle_free (tPBSineTriangle** const cy)
{
    tPBSineTriangle* c = *cy;
    mpool_free((char*)c, c->mempool);
}

#ifdef ITCMRAM
    float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSineTriangle_tick          (tPBSineTriangle* const osc)
#else
float   tPBSineTriangle_tick          (tPBSineTriangle* const  c)
#endif
{

    uint32_t t1 = c->phase + TWO_TO_32_ONE_QUARTER;

    uint32_t t2 = c->phase + TWO_TO_32_THREE_QUARTERS;

    float t1F = t1 * INV_TWO_TO_32;
    float t2F = t2 * INV_TWO_TO_32;
    float t = c->phase * INV_TWO_TO_32;
    float incFloat = c->inc * INV_TWO_TO_32;

    float y = t * 4.0f;

    if (y >= 3.0f) {
        y -= 4.0f;
    } else if (y > 1.0f) {
        y = 2.0f - y;
    }
    y += 4.0f * incFloat * (blamp(t1F, incFloat) - blamp(t2F, incFloat));
    y = y * c->shape; // shape handles the inversion so it's in phase with sine (already * -1.0f)

    y = y + (tCycle_tick(&c->sine) * c->oneMinusShape);

    c->phase += c->inc;

    return y;
}

#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSineTriangle_setFreq       (tPBSineTriangle* const osc, float freq)
#else
void    tPBSineTriangle_setFreq       (tPBSineTriangle* const  c, float freq)
#endif
{
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
    tCycle_setFreq(&c->sine, freq);
}

void    tPBSineTriangle_setShape       (tPBSineTriangle* const  c, float shape)
{
    c->shape = 1.0f * shape;
    c->oneMinusShape = 1.0f - shape;
}

void    tPBSineTriangle_setSampleRate (tPBSineTriangle* const c, float sr)
{

    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBSineTriangle_setFreq(c, c->freq);
}
//==============================================================================

/* tPulse: Anti-aliased pulse waveform. */
void tPBPulse_create(tMempool** const mp, tPBPulse** const osc)
{
    ALLOC_FROM_POOL(tPBPulse, osc, mp);
}

void tPBPulse_init(LEAF* const leaf, tPBPulse* const c)
{


    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->width     =  (0.5f * TWO_TO_32);
    c->oneMinusWidth =  c->width;
    c->freq = 0.0f;

}

void    tPBPulse_free (tPBPulse** const osc)
{
    tPBPulse* c = *osc;

    mpool_free((char*)c, c->mempool);
}

#ifdef ITCMRAM
    float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBPulse_tick        (tPBPulse* const osc)
#else
float   tPBPulse_tick        (tPBPulse* const c)
#endif
{

    float phaseFloat = c->phase *  INV_TWO_TO_32;
    float incFloat = c->inc *  INV_TWO_TO_32;
    float backwardsPhaseFloat = (c->phase + c->oneMinusWidth) * INV_TWO_TO_32;
    float widthFloat =c->width *INV_TWO_TO_32;
    float out = -2.0f * widthFloat;
    if (phaseFloat < widthFloat) {
        out += 2.0f;
    }
    out += LEAF_poly_blep(phaseFloat,incFloat);
    out -= LEAF_poly_blep(backwardsPhaseFloat, incFloat);
    c->phase += c->inc;
    return out;

}

#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBPulse_setFreq     (tPBPulse* const osc, float freq)
#else
void    tPBPulse_setFreq     (tPBPulse* const c, float freq)
#endif
{

    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tPBPulse_setWidth    (tPBPulse* const c, float width)
{
    //clip width to avoid silence from pulse widths of 0 or 1
    if (width < 0.05f)
    {
        width = 0.05f;
    }
    if (width > 0.95f)
    {
        width = 0.95f;
    }
    c->oneMinusWidth = (1.0f - width) * TWO_TO_32;
    c->width = width * TWO_TO_32;
}

void    tPBPulse_setSampleRate (tPBPulse* const  c, float sr)
{
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBPulse_setFreq(c, c->freq);
}

//==============================================================================

/* tSawtooth: Anti-aliased Sawtooth waveform. */
void tPBSaw_create(tMempool** const mp, tPBSaw** const osc)
{
    ALLOC_FROM_POOL(tPBSaw, osc, mp);
}

void tPBSaw_init(LEAF* const leaf, tPBSaw* const c)
{


    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->freq = 0.0f;

}

void    tPBSaw_free  (tPBSaw** const osc)
{
    tPBSaw* c = *osc;

    mpool_free((char*)c, c->mempool);
}

#ifdef ITCMRAM
    float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSaw_tick          (tPBSaw* const osc)
#else
float   tPBSaw_tick          (tPBSaw* const c)
#endif
{
    float out = (c->phase * INV_TWO_TO_31) - 1.0f;

    float phaseFloat = c->phase * INV_TWO_TO_32;
    float incFloat = c->inc * INV_TWO_TO_32;
    out -= LEAF_poly_blep(phaseFloat, incFloat);
    c->phase += c->inc;
    return (-1.0f * out);
}

#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tPBSaw_setFreq       (tPBSaw* const osc, float freq)
#else
void    tPBSaw_setFreq       (tPBSaw* const c, float freq)
#endif
{
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tPBSaw_setSampleRate (tPBSaw* const c, float sr)
{
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBSaw_setFreq(c, c->freq);
}

//========================================================================


/* tSawtooth: Anti-aliased Sawtooth waveform. */
void tPBSawSquare_create(tMempool** const mp, tPBSawSquare** const osc)
{
    ALLOC_FROM_POOL(tPBSawSquare, osc, mp);
}

void tPBSawSquare_init(LEAF* const leaf, tPBSawSquare* const c)
{


    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->freq = 0.0f;
    c->shape = 0.0f;
    c->oneMinusShape = 1.0f;


}

void    tPBSawSquare_free  (tPBSawSquare** const osc)
{
    tPBSawSquare* c = *osc;

    mpool_free((char*)c, c->mempool);
}


#ifdef ITCMRAM
float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tPBSawSquare_tick          (tPBSawSquare* const osc)
#else
float   tPBSawSquare_tick          (tPBSawSquare* const c)
#endif
{
    //float squareOut = ((c->phase < 2147483648u) * 2.0f) - 1.0f;
    float sawOut = (c->phase * INV_TWO_TO_32 * 2.0f) - 1.0f;
    float phaseFloat = c->phase * INV_TWO_TO_32;
    float incFloat = c->inc * INV_TWO_TO_32;
    float backwardsPhaseFloat = (c->phase + 2147483648u) * INV_TWO_TO_32;
    float resetBlep = LEAF_poly_blep(phaseFloat,incFloat);
    float midBlep = LEAF_poly_blep(backwardsPhaseFloat, incFloat);

    float squareOut = -1.0f;
    if (phaseFloat < 0.5f) {
        squareOut += 2.0f;
    }
    sawOut -= resetBlep;

    squareOut += resetBlep;
    squareOut -= midBlep;

    c->phase += c->inc;

    return ((-1.0f * sawOut) * c->oneMinusShape) + (squareOut * c->shape);
}

#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tPBSawSquare_setFreq       (tPBSawSquare* const osc, float freq)
#else
void    tPBSawSquare_setFreq       (tPBSawSquare* const c, float freq)
#endif
{
    c->freq  = freq;
    c->inc = (freq * c->invSampleRateTimesTwoTo32);

}

void    tPBSawSquare_setShape      (tPBSawSquare* const c, float inputShape)
{
    c->shape  = inputShape;
    c->oneMinusShape = 1.0f - inputShape;
}

void    tPBSawSquare_setSampleRate (tPBSawSquare* const c, float sr)
{
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBSawSquare_setFreq(c, c->freq);
}

//========================================================================


//==============================================================================

/* tSawtooth: Anti-aliased Sawtooth waveform. */
void tSawOS_create(tMempool** const mp, tSawOS** const osc)
{
    ALLOC_FROM_POOL(tSawOS, osc, mp);
}

void tSawOS_init(LEAF* const leaf, tSawOS* const c, uint8_t OS_ratio, uint8_t filterOrder)
{

    c->OSratio = OS_ratio;
    c->inc      = 0;
    c->phase    = 0;
    c->invSampleRateOS = 1.0f / (leaf->sampleRate * OS_ratio);
    c->invSampleRateTimesTwoTo32OS = (c->invSampleRateOS * TWO_TO_32);
    c->filterOrder = filterOrder;
    c->aaFilters = (tSVF*) mpool_alloc(sizeof(tSVF) * filterOrder, c->mempool);

    for (int i = 0; i < filterOrder; i++)
    {
        float Qval = 0.5f/cosf((1.0f+2.0f*i)*PI/(4*filterOrder));

		tSVF_init(leaf,&c->aaFilters[i], SVFTypeLowpass, (19000.0f / OS_ratio), Qval);

}
    tSawOS_setFreq(c, 220.0f);
}

void    tSawOS_free  (tSawOS** const osc)
{
    tSawOS* c = *osc;
    mpool_free((char*)c->aaFilters, c->mempool);
    mpool_free((char*)c, c->mempool);
}

float   tSawOS_tick          (tSawOS* const c)
{
    float tempFloat = 0.0f;
    for (int i = 0; i < c->OSratio; i++)
    {
    	c->phase = (c->phase + c->inc);
        tempFloat = (c->phase * INV_TWO_TO_16)- 1.0f; // inv 2 to 32, then multiplied by 2, same as inv 2 to 16
    	for (int k = 0; k < c->filterOrder; k++)
    	{
    		tempFloat = tSVF_tick(&c->aaFilters[k], tempFloat);
    	}
    }
    return tempFloat;
}

void    tSawOS_setFreq       (tSawOS* const c, float freq)
{
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32OS;
}

void    tSawOS_setSampleRate (tSawOS* const c, float sr)
{
    c->invSampleRateOS = 1.0f/(sr * c->OSratio);
    tSawOS_setFreq(c, c->freq);
}

//========================================================================

/* Phasor */

void tPhasor_create(tMempool** const mp, tPhasor** const ph)
{
    ALLOC_FROM_POOL(tPhasor, ph, mp);
}

void tPhasor_init(LEAF* const leaf, tPhasor* const p)
{


    p->phase = 0;
    p->inc = 0;
    p->invSampleRate = leaf->invSampleRate;
    p->invSampleRateTimesTwoTo32 = p->invSampleRate * TWO_TO_32;

}

void    tPhasor_free (tPhasor** const ph)
{
    tPhasor* p = *ph;

    mpool_free((char*)p, p->mempool);
}

void    tPhasor_setFreq(tPhasor* const p, float freq)
{
    p->freq  = freq;
    p->inc = freq * p->invSampleRateTimesTwoTo32;
}

float   tPhasor_tick(tPhasor* const p)
{
    p->phase += p->inc; // no need to phase wrap, since integer overflow does it for us
    return p->phase * INV_TWO_TO_32; //smush back to 0.0-1.0 range
}

void     tPhasor_setSampleRate (tPhasor* const p, float sr)
{
    p->invSampleRate = 1.0f/sr;
    p->invSampleRateTimesTwoTo32 = p->invSampleRate * TWO_TO_32;
    tPhasor_setFreq(p, p->freq);
};

//========================================================================
/* Noise */
void tNoise_create(tMempool** const mp, tNoise** const ns)
{
    ALLOC_FROM_POOL(tNoise, ns, mp);
}

void tNoise_init(LEAF* const leaf, tNoise* const n, NoiseType type)
{


    n->type = type;
    n->rand = leaf->random;

}

void    tNoise_free (tNoise** const ns)
{
    tNoise* n = *ns;

    mpool_free((char*)n, n->mempool);
}

float   tNoise_tick(tNoise* const n)
{
    float rand = (n->rand() * 2.0f) - 1.0f;

    if (n->type == PinkNoise)
    {
        float tmp;
        n->pinkb0 = 0.99765f * n->pinkb0 + rand * 0.0990460f;
        n->pinkb1 = 0.96300f * n->pinkb1 + rand * 0.2965164f;
        n->pinkb2 = 0.57000f * n->pinkb2 + rand * 1.0526913f;
        tmp = n->pinkb0 + n->pinkb1 + n->pinkb2 + rand * 0.1848f;
        return (tmp * 0.05f);
    }
    else // WhiteNoise
    {
        return rand;
    }
}

//=================================================================================
/* Perlin Noise */
void tPerlinNoise_create(tMempool** const mp, tPerlinNoise** const ns)
{
    ALLOC_FROM_POOL(tPerlinNoise, ns, mp);
}

void tPerlinNoise_init(LEAF* const leaf, tPerlinNoise* const n, float rate, float energy)
{
    n->rateMs = rate;
    n->energy = energy;
    n->buffSize = rate/1000*leaf->sampleRate;
    n->interper = LEAF_interpolate_hermite;
    n->rand = leaf->random;
    n->x1 = 0.f;
    n->m1 = 0.f;
    n->x2 = (n->rand() * 2.0f) - 1.0f;
    n->m2 = (n->rand() * 2.0f) - 1.0f;
    n->counter = 0;
    n->scaler = 1.f;
}

void    tPerlinNoise_free (tPerlinNoise** const ns)
{
    tPerlinNoise* n = *ns;

    mpool_free((char*)n, n->mempool);
}

float   tPerlinNoise_tick(tPerlinNoise* const n)
{
    if (n->counter < n->buffSize)
    {
        n->currX = n->interper((n->x2) - 2.f*(n->m1), (n->x1), (n->x2), -2.f*(n->m2) - (n->x1), (n->counter)/(float)n->buffSize);
    } else
    {
        n->counter = 0;
        n->x1 = n->x2;
        n->m1 = n->m2;
        n->x2 = ((n->rand() * 2.0f) - 1.0f) * n->energy;
        n->m2 = ((n->rand() * 2.0f) - 1.0f) * n->energy*2.f;
        n->currX = n->interper((n->x2) - 2.f*(n->m1), (n->x1), (n->x2), -2.f*(n->m2) - (n->x1), n->counter/(float)n->buffSize);
    }
    n->counter++;
    return n->currX;

}

void    tPerlinNoise_setRate(LEAF* const leaf, tPerlinNoise* const n, float rate)
{
    n->rateMs = rate;
    n->buffSize = rate/1000.f*leaf->sampleRate;
    //n->m1 = 3.f*(-0.5f*(n->x2) - 2.f*(n->m1)+1.5f*(n->x1)-1.5f*(n->x2)+0.5f*(-2.f*(n->m2) - (n->x1)))*(n->counter/(float)n->buffSize)*(n->counter/(float)n->buffSize) + 2.f*((n->x2) - 2.f*(n->m1)-2.5f*(n->x1)+2.f*(n->x2)-0.5f*(-2.f*(n->m2) - (n->x1)))*(n->counter/(float)n->buffSize) + -0.5f*((n->x2) - 2.f*(n->m1)) + 0.5f*(n->x2);
    n->m1 = 0.f;
    n->x1 = n->currX;
    n->x2 = ((n->rand() * 2.0f) - 1.0f) * n->energy;
    n->m2 = ((n->rand() * 2.0f) - 1.0f) * n->energy*2.f;
    n->counter = 0;
}

void    tPerlinNoise_setEnergy(tPerlinNoise* const n, float energy)
{
    n->energy = energy;
    n->m1 = 0.f;
    n->x1 = n->currX;
    n->x2 = ((n->rand() * 2.0f) - 1.0f) * n->energy;
    n->m2 = ((n->rand() * 2.0f) - 1.0f) * n->energy*2.f;
    n->counter = 0;
}

//=================================================================================
/* Neuron */

void tNeuron_create(tMempool** const mp, tNeuron** const nr)
{
    ALLOC_FROM_POOL(tNeuron, nr, mp);
}

void tNeuron_init(LEAF* const leaf, tNeuron* const n)
{


    tPoleZero_init(leaf,&n->f);

    tPoleZero_setBlockZero(&n->f, 0.99f);

    n->invSampleRate = leaf->invSampleRate;
    n->timeStep = (44100.0f * n->invSampleRate) / 50.0f;

    n->current = 0.0f; // 100.0f for sound
    n->voltage = 0.0f;

    n->mode = NeuronNormal;

    n->P[0] = 0.0f;
    n->P[1] = 0.0f;
    n->P[2] = 1.0f;

    n->V[0] = -12.0f;
    n->V[1] = 115.0f;
    n->V[2] = 10.613f;

    n->gK = 36.0f;
    n->gN = 120.0f;
    n->gL = 0.3f;
    n->C = 1.0f;

    n->rate[2] = n->gL/n->C;

}

void    tNeuron_free (tNeuron** const nr)
{
    tNeuron* n = *nr;

    mpool_free((char*)n, n->mempool);
}

void   tNeuron_reset(tNeuron* const n)
{
    tPoleZero_setBlockZero(&n->f, 0.99f);

    n->timeStep = (44100.0f * n->invSampleRate) / 50.0f;

    n->current = 0.0f; // 100.0f for sound
    n->voltage = 0.0f;

    n->mode = NeuronNormal;

    n->P[0] = 0.0f;
    n->P[1] = 0.0f;
    n->P[2] = 1.0f;

    n->V[0] = -12.0f;
    n->V[1] = 115.0f;
    n->V[2] = 10.613f;

    n->gK = 36.0f;
    n->gN = 120.0f;
    n->gL = 0.3f;
    n->C = 1.0f;

    n->rate[2] = n->gL/n->C;
}

void tNeuron_setV1(tNeuron* const n, float V1)
{
    n->V[0] = V1;
}

void tNeuron_setV2(tNeuron* const n, float V2)
{
    n->V[1] = V2;
}

void tNeuron_setV3(tNeuron* const n, float V3)
{
    n->V[2] = V3;
}

void tNeuron_setTimeStep(tNeuron* const n, float timeStep)
{
    n->timeStep = (44100.0f * n->invSampleRate) * timeStep;
}

void tNeuron_setK(tNeuron* const n, float K)
{
    n->gK = K;
}

void tNeuron_setL(tNeuron* const n, float L)
{
    n->gL = L;
    n->rate[2] = n->gL/n->C;
}

void tNeuron_setN(tNeuron* const n, float N)
{
    n->gN = N;
}

void tNeuron_setC(tNeuron* const n, float C)
{
    n->C = C;
    n->rate[2] = n->gL/n->C;
}

float tNeuron_tick(tNeuron* const n)
{
    float output = 0.0f;
    float voltage = n->voltage;

    n->alpha[0] = (0.01f * (10.0f - voltage)) / (expf((10.0f - voltage)/10.0f) - 1.0f);
    n->alpha[1] = (0.1f * (25.0f-voltage)) / (expf((25.0f-voltage)/10.0f) - 1.0f);
    n->alpha[2] = (0.07f * expf((-1.0f * voltage)/20.0f));

    n->beta[0] = (0.125f * expf((-1.0f* voltage)/80.0f));
    n->beta[1] = (4.0f * expf((-1.0f * voltage)/18.0f));
    n->beta[2] = (1.0f / (expf((30.0f-voltage)/10.0f) + 1.0f));

    for (int i = 0; i < 3; i++)
    {
        n->P[i] = (n->alpha[i] * n->timeStep) + ((1.0f - ((n->alpha[i] + n->beta[i]) * n->timeStep)) * n->P[i]);

        if (n->P[i] > 1.0f)         n->P[i] = 0.0f;
        else if (n->P[i] < -1.0f)   n->P[i] = 0.0f;
    }
    // rate[0]= k ; rate[1] = Na ; rate[2] = l
    n->rate[0] = ((n->gK * powf(n->P[0], 4.0f)) / n->C);
    n->rate[1] = ((n->gN * powf(n->P[1], 3.0f) * n->P[2]) / n->C);

    //calculate the final membrane voltage based on the computed variables
    n->voltage = voltage +
    (n->timeStep * n->current / n->C) -
    (n->timeStep * ( n->rate[0] * (voltage - n->V[0]) + n->rate[1] * (voltage - n->V[1]) + n->rate[2] * (voltage - n->V[2])));

    if (n->mode == NeuronTanh)
    {
        n->voltage = 100.0f * tanhf(0.01f * n->voltage);
    }
    else if (n->mode == NeuronAaltoShaper)
    {
        float shapeVoltage = 0.01f * n->voltage;

        float w, c, xc, xc2, xc4;

        float sqrt8 = 2.82842712475f;

        float wscale = 1.30612244898f;
        float m_drive = 1.0f;

        xc = LEAF_clip(-sqrt8, shapeVoltage, sqrt8);

        xc2 = xc*xc;

        c = 0.5f * shapeVoltage * (3.0f - (xc2));

        xc4 = xc2 * xc2;

        w = (1.0f - xc2 * 0.25f + xc4 * 0.015625f) * wscale;

        shapeVoltage = w * (c + 0.05f * xc2) * (m_drive + 0.75f);

        n->voltage = 100.0f * shapeVoltage;
    }


    if (n->voltage > 100.0f)  n->voltage = 100.0f;
    else if (n->voltage < -100.) n->voltage = -100.0f;

    //(inputCurrent + (voltage - ((voltage * timeStep) / timeConstant)) + P[0] + P[1] + P[2]) => voltage;
    // now we should have a result
    //set the output voltage to the "step" ugen, which controls the DAC.
    output = n->voltage * 0.01f; // volts

    output = tPoleZero_tick(&n->f, output);

    return output;

}

void tNeuron_setMode  (tNeuron* const n, NeuronMode mode)
{
    n->mode = mode;
}

void tNeuron_setCurrent  (tNeuron* const n, float current)
{
    n->current = current;
}

void tNeuron_setSampleRate (tNeuron* const n, float sr)
{
    n->invSampleRate = 1.0f/sr;
    n->timeStep = (44100.0f * n->invSampleRate) / 50.0f;
}

//----------------------------------------------------------------------------------------------------------

void tMBPulse_create(tMempool** const mp, tMBPulse** const osc)
{
    ALLOC_FROM_POOL(tMBPulse, osc, mp);
}

void tMBPulse_init(LEAF* const leaf, tMBPulse* const c)
{


    c->invSampleRate = leaf->invSampleRate;


    c->freq = 440.f;
    c->lastsyncin = 0.0f;
    c->sync = 0.0f;
    c->syncdir = 1.0f;
    c->softsync = 0;
    c->waveform = 0.0f;
    c->_z = 0.0f;
    c->_j = 0;
    c->_p = 0.0f;  /* phase [0, 1) */
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_b = 0.5f * (1.0f + c->waveform);  /* duty cycle (0, 1) */
    c->_x = 0.5f;  /* temporary output variable */
    c->_k = 0.0f;  /* output state, 0 = high (0.5f), 1 = low (-0.5f) */
    c->_inv_w = 1.0f / c->_w;
    c->numBLEPs = 0;
    c->mostRecentBLEP = 0;
    c->maxBLEPphase = MINBLEP_PHASES * STEP_DD_PULSE_LENGTH;
    memset (c->BLEPindices, 0, 64 * sizeof (uint16_t));
    memset (c->_f, 0, 8 * sizeof (float));

}

void tMBPulse_free(tMBPulse** const osc)
{
    tMBPulse* c = *osc;
    mpool_free((char*)c, c->mempool);
}

//#ifdef ITCMRAM
//void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBPulse_place_step_dd_noBuffer(tMBPulse* const osc, int index, float phase, float inv_w, float scale)
//#else
void tMBPulse_place_step_dd_noBuffer(tMBPulse* const c, int index, float phase, float inv_w, float scale)
//#endif
{
	float r;
	long i;

	r = MINBLEP_PHASES * phase * inv_w;
	i = lrintf(r - 0.5f);
	r -= (float)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
	c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
	c->BLEPindices[c->mostRecentBLEP] = i;
    c->BLEPproperties[c->mostRecentBLEP][0] = r;
    c->BLEPproperties[c->mostRecentBLEP][1] = scale;
    c->numBLEPs = (c->numBLEPs + 1) & 63;
}


float tMBPulse_tick(tMBPulse* const c)
{
    int    j, k;
    float  sync;
    float  b, p, w, x, z, sw;

    sync = c->sync;


    p = c->_p;  /* phase [0, 1) */
    w = c->_w;  /* phase increment */
    b = c->_b;  /* duty cycle (0, 1) */
    x = c->_x;  /* temporary output variable */
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */
    k = c->_k;  /* output state, 0 = high (0.5f), 1 = low (-0.5f) */


    if (sync > 0.0f && c->softsync > 0) c->syncdir = -c->syncdir;

    sw = w * c->syncdir;
    float inv_sw = c->_inv_w * c->syncdir;
    p += sw - (int)sw;

    if (sync > 0.0f && c->softsync == 0) {  /* sync to master */
        float eof_offset = sync * sw;
        float p_at_reset = p - eof_offset;

        if (sw > 0) p = eof_offset;
        else if (sw < 0) p = 1.0f - eof_offset;

        /* place any DDs that may have occurred in subsample before reset */
        if (!k) {
            if (sw > 0)
            {
                if (p_at_reset >= b) {
                	tMBPulse_place_step_dd_noBuffer(c, j, p_at_reset - b + eof_offset, inv_sw, -1.0f);
                    k = 1;
                    x = -0.5f;
                }
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    tMBPulse_place_step_dd_noBuffer(c, j, p_at_reset + eof_offset, inv_sw, 1.0f);
                    k = 0;
                    x = 0.5f;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    tMBPulse_place_step_dd_noBuffer(c, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
                    k = 1;
                    x = -0.5f;
                }
                if (k && p_at_reset < b) {
                	tMBPulse_place_step_dd_noBuffer(c, j, b - p_at_reset - eof_offset, -inv_sw, 1.0f);
                    k = 0;
                    x = 0.5f;
                }
            }
        } else {
            if (sw > 0)
            {
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    tMBPulse_place_step_dd_noBuffer(c, j, p_at_reset + eof_offset, inv_sw, 1.0f);
                    k = 0;
                    x = 0.5f;
                }
                if (!k && p_at_reset >= b) {
                	tMBPulse_place_step_dd_noBuffer(c, j, p_at_reset - b + eof_offset, inv_sw, -1.0f);
                    k = 1;
                    x = -0.5f;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < b) {
                	tMBPulse_place_step_dd_noBuffer(c, j, b - p_at_reset - eof_offset, -inv_sw, 1.0f);
                    k = 0;
                    x = 0.5f;
                }
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    tMBPulse_place_step_dd_noBuffer(c, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
                    k = 1;
                    x = -0.5f;
                }
            }
        }

        /* now place reset DD */
        if (sw > 0)
        {
            if (k) {
            	tMBPulse_place_step_dd_noBuffer(c, j, p, inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
            if (p >= b) {
            	tMBPulse_place_step_dd_noBuffer(c, j, p - b, inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
        }
        else if (sw < 0)
        {
            if (!k) {
            	tMBPulse_place_step_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
            if (p < b) {
            	tMBPulse_place_step_dd_noBuffer(c, j, b - p, -inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
        }
    } else if (!k) {  /* normal operation, signal currently high */

        if (sw > 0)
        {
            if (p >= b) {
            	tMBPulse_place_step_dd_noBuffer(c, j, p - b, inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
            if (p >= 1.0f) {
                p -= 1.0f;
                tMBPulse_place_step_dd_noBuffer(c, j, p, inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
        }
        else if (sw < 0)
        {
            if (p < 0.0f) {
                p += 1.0f;
                tMBPulse_place_step_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
            if (k && p < b) {
            	tMBPulse_place_step_dd_noBuffer(c, j, b - p, -inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
        }

    } else {  /* normal operation, signal currently low */

        if (sw > 0)
        {
            if (p >= 1.0f) {
                p -= 1.0f;
                tMBPulse_place_step_dd_noBuffer(c, j, p, inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
            if (!k && p >= b) {
            	tMBPulse_place_step_dd_noBuffer(c, j, p - b, inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
        }
        else if (sw < 0)
        {
            if (p < b) {
            	tMBPulse_place_step_dd_noBuffer(c, j, b - p, -inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
            if (p < 0.0f) {
                p += 1.0f;
                tMBPulse_place_step_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
        }
    }

    int currentSamp = (j + DD_SAMPLE_DELAY) & 7;

    c->_f[currentSamp] = x;

    volatile uint8_t numBLEPsAtLoopStart = c->numBLEPs;
	for (int i = 0; i < numBLEPsAtLoopStart; i++)
	{
		volatile uint16_t whichBLEP = (c->mostRecentBLEP - i);
		whichBLEP &= 63;

		//use the scale and r values from the BLEPproperties array to compute the current state of each active BLEP and add it to the output value
		c->_f[j] += c->BLEPproperties[whichBLEP][1] * (step_dd_table[c->BLEPindices[whichBLEP]].value + c->BLEPproperties[whichBLEP][0] * step_dd_table[c->BLEPindices[whichBLEP]].delta);

		//increment the position in the BLEP table
		c->BLEPindices[whichBLEP] += MINBLEP_PHASES;
		//check if this BLEP is finished and if so mark it as inactive so it isn't computed anymore.
		if (c->BLEPindices[whichBLEP] >= c->maxBLEPphase)
		{
			c->numBLEPs--;
		}

	}

    z += 0.5f * (c->_f[j] - z);
    c->out = z;

    j = (j+1) & 7;

    c->_p = p;
    c->_w = w;
    c->_b = b;
    c->_x = x;
    c->_z = z;
    c->_j = j;
    c->_k = k;

    return -c->out;
}

void tMBPulse_setFreq(tMBPulse* const c, float f)
{
    c->freq = f;
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_inv_w = 1.0f / c->_w;
}

void tMBPulse_setWidth(tMBPulse* const c, float w)
{
    c->waveform = w;
    c->_b = 0.5f * (1.0f + c->waveform);
}

float tMBPulse_sync(tMBPulse* const c, float value)
{
    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    float last = c->lastsyncin;
    float delta = value - last;
    float crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;

    return value;
}

void tMBPulse_setPhase(tMBPulse* const c, float phase)
{
    c->_p = phase;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBPulse_setBufferOffset(tMBPulse* const c, uint32_t offset)
{
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBPulse_setSyncMode(tMBPulse* const c, int hardOrSoft)
{
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

void tMBPulse_setSampleRate(tMBPulse* const c, float sr)
{
    c->invSampleRate = 1.0f/sr;
}

//==========================================================================================================
//==========================================================================================================

void tMBTriangle_create(tMempool** const mp, tMBTriangle** const osc)
{
    ALLOC_FROM_POOL(tMBTriangle, osc, mp);
}

void tMBTriangle_init(LEAF* const leaf, tMBTriangle* const c)
{


    c->invSampleRate = leaf->invSampleRate;
    c->freq = 440.f;
    c->lastsyncin = 0.0f;
    c->sync = 0.0f;
    c->syncdir = 1.0f;
    c->softsync = 0;
    c->waveform = 0.0f;
    c->_z = 0.0f;
    c->_j = 0;
    c->_p = 0.25f;  /* phase [0, 1) */
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_b = 0.5f * (1.0f + c->waveform);  /* duty cycle (0, 1) */
    c->_k = 0.0f;  /* output state, 0 = high (0.5f), 1 = low (-0.5f) */
    c->_inv_w = 1.0f / c->_w;
    c->numBLEPs = 0;
    c->mostRecentBLEP = 0;
    c->maxBLEPphase = MINBLEP_PHASES * STEP_DD_PULSE_LENGTH;
    c->maxBLEPphaseSlope = MINBLEP_PHASES * SLOPE_DD_PULSE_LENGTH;
    memset (c->BLEPindices, 0, 64 * sizeof (uint16_t));
    memset (c->_f, 0, 8 * sizeof (float));

}

void tMBTriangle_free(tMBTriangle** const osc)
{
    tMBTriangle* c = *osc;
    mpool_free((char*)c, c->mempool);
}

//#ifdef ITCMRAM
//void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBTriangle_place_dd_noBuffer(tMBTriangle* const osc, int index, float phase, float inv_w, float scale, float stepOrSlope, float w)
//#else
void tMBTriangle_place_dd_noBuffer(tMBTriangle* const c, int index, float phase, float inv_w, float scale, float stepOrSlope, float w)
//#endif
{
	float r;
	long i;

	r = MINBLEP_PHASES * phase * inv_w;
	i = lrintf(r - 0.5f);
	r -= (float)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
	scale *= w;
	c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
	c->BLEPindices[c->mostRecentBLEP] = i;
    c->BLEPproperties[c->mostRecentBLEP][0] = r;
    c->BLEPproperties[c->mostRecentBLEP][1] = scale;
    c->BLEPproperties[c->mostRecentBLEP][2] = stepOrSlope;
    c->numBLEPs = (c->numBLEPs + 1) & 63;
}

float tMBTriangle_tick(tMBTriangle* const c)
{
    int    j, k;
    float  sync;
    float  b, b1, invB, invB1, p, w, sw, z;
    float  x = 0.5f;

    sync = c->sync;

    p = c->_p;  /* phase [0, 1) */
    w = c->_w;  /* phase increment */
    b = c->_b;  /* duty cycle (0, 1) */
    invB = 1.0f / b;
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */
    k = c->_k;  /* output state, 0 = positive slope, 1 = negative slope */



    b = 0.5f * (1.0f + c->waveform);
    b1 = 1.0f - b;
    invB1 = 1.0f / b1;
    if (sync > 0.0f && c->softsync > 0) c->syncdir = -c->syncdir;

    sw = w * c->syncdir;
    float inv_sw = c->_inv_w * c->syncdir;
    p += sw - (int)sw;

    if (sync > 0.0f && c->softsync == 0) {  /* sync to master */
        float eof_offset = sync * sw;
        float p_at_reset = p - eof_offset;

        if (sw > 0) p = eof_offset;
        else if (sw < 0) p = 1.0f - eof_offset;
        //
        /* place any DDs that may have occurred in subsample before reset */

        if (!k) {
            x = -0.5f + p_at_reset * invB;
            if (sw > 0)
            {
                if (p_at_reset >= b) {
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBTriangle_place_dd_noBuffer(c, j, p_at_reset - b + eof_offset, inv_sw, -invB1 - invB, 1.0f, sw);
                    k = 1;
                }
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    x = -0.5f + p_at_reset * invB;
                    tMBTriangle_place_dd_noBuffer(c, j, p_at_reset + eof_offset, inv_sw, invB + invB1, 1.0f, sw);
                    k = 0;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    x = 0.5f - (p_at_reset - b)  * invB1;
                    tMBTriangle_place_dd_noBuffer(c, j, 1.0f - p_at_reset - eof_offset, -inv_sw, invB + invB1, 1.0f, -sw);
                    k = 1;
                }
                if (k && p_at_reset < b) {
                    x = -0.5f + p_at_reset * invB;
                    tMBTriangle_place_dd_noBuffer(c, j, b - p_at_reset - eof_offset, -inv_sw, -invB1 - invB, 1.0f, -sw);
                    k = 0;
                }
            }
        } else {
            x = 0.5f - (p_at_reset - b) * invB1;
            if (sw > 0)
            {
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    x = -0.5f + p_at_reset * invB;
                    tMBTriangle_place_dd_noBuffer(c, j, p_at_reset + eof_offset, inv_sw, invB + invB1, 1.0f, sw);
                    k = 0;
                }
                if (!k && p_at_reset >= b) {
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBTriangle_place_dd_noBuffer(c, j, p_at_reset - b + eof_offset, inv_sw, -invB1 - invB, 1.0f, sw);
                    k = 1;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < b) {
                    x = -0.5f + p_at_reset * invB;
                    tMBTriangle_place_dd_noBuffer(c, j, b - p_at_reset - eof_offset, -inv_sw, -invB1 - invB, 1.0f, -sw);
                    k = 0;
                }
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBTriangle_place_dd_noBuffer(c, j, 1.0f - p_at_reset - eof_offset, -inv_sw, invB + invB1, 1.0f, -sw);
                    k = 1;
                }
            }
        }

        /* now place reset DDs */
        if (sw > 0)
        {
            if (k)
            	tMBTriangle_place_dd_noBuffer(c, j, p, inv_sw, invB + invB1, 1.0f, sw);
            tMBTriangle_place_dd_noBuffer(c, j, p, inv_sw, -0.5f - x, 0.0f, sw);
            x = -0.5f + p * invB;
            k = 0;
            if (p >= b) {
                x = 0.5f - (p - b) * invB1;
                tMBTriangle_place_dd_noBuffer(c, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
        }
        else if (sw < 0)
        {
            if (!k)
            	tMBTriangle_place_dd_noBuffer(c, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
            tMBTriangle_place_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -0.5f - x, 0.0f, -sw);
            x = 0.5f - (p - b) * invB1;
            k = 1;
            if (p < b) {
                x = -0.5f + p * invB;
                tMBTriangle_place_dd_noBuffer(c, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
        }
    } else if (!k) {  /* normal operation, slope currently up */

        x = -0.5f + p * invB;
        if (sw > 0)
        {
            if (p >= b) {
                x = 0.5f - (p - b) * invB1;;
                tMBTriangle_place_dd_noBuffer(c, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
            if (p >= 1.0f) {
                p -= 1.0f;
                x = -0.5f + p * invB;
                tMBTriangle_place_dd_noBuffer(c, j, p, inv_sw, invB + invB1, 1.0f, sw);
                k = 0;
            }
        }
        else if (sw < 0)
        {
            if (p < 0.0f) {
                p += 1.0f;
                x = 0.5f - (p - b) * invB1;
                tMBTriangle_place_dd_noBuffer(c, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
                k = 1;
            }
            if (k && p < b) {
                x = -0.5f + p * invB;
                tMBTriangle_place_dd_noBuffer(c, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
        }

    } else {  /* normal operation, slope currently down */

        x = 0.5f - (p - b) * invB1;
        if (sw > 0)
        {
            if (p >= 1.0f) {
                p -= 1.0f;
                x = -0.5f + p * invB;
                tMBTriangle_place_dd_noBuffer(c, j, p, inv_sw, invB + invB1, 1.0f, sw);
                k = 0;
            }
            if (!k && p >= b) {
                x = 0.5f - (p - b) * invB1;
                tMBTriangle_place_dd_noBuffer(c, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
        }
        else if (sw < 0)
        {
            if (p < b) {
                x = -0.5f + p * invB;
                tMBTriangle_place_dd_noBuffer(c, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
            if (p < 0.0f) {
                p += 1.0f;
                x = 0.5f - (p - b) * invB1;
                tMBTriangle_place_dd_noBuffer(c, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
                k = 1;
            }
        }
    }
    int currentSamp = (j + DD_SAMPLE_DELAY) & 7;

    c->_f[currentSamp] = x;

    volatile uint8_t numBLEPsAtLoopStart = c->numBLEPs;
    for (int i = 0; i < numBLEPsAtLoopStart; i++)
    {
    	volatile uint16_t whichBLEP = (c->mostRecentBLEP - i);
    	whichBLEP &= 63;

    	//use the scale and r values from the BLEPproperties array to compute the current state of each active BLEP and add it to the output value

    	if (c->BLEPproperties[whichBLEP][2] < 0.5f) //step blep
    	{
    		c->_f[j] += c->BLEPproperties[whichBLEP][1] * (step_dd_table[c->BLEPindices[whichBLEP]].value + c->BLEPproperties[whichBLEP][0] * step_dd_table[c->BLEPindices[whichBLEP]].delta);
        	//increment the position in the BLEP table
    		c->BLEPindices[whichBLEP] += MINBLEP_PHASES;
    		//check if this BLEP is finished and if so mark it as inactive so it isn't computed anymore.
    		if (c->BLEPindices[whichBLEP] >= c->maxBLEPphase)
    		{
    			c->numBLEPs--;
    		}
    	}
    	else  // slope blep
    	{
    		c->_f[j] += c->BLEPproperties[whichBLEP][1] * (slope_dd_table[c->BLEPindices[whichBLEP]] + c->BLEPproperties[whichBLEP][0] * (slope_dd_table[c->BLEPindices[whichBLEP]+1] - slope_dd_table[c->BLEPindices[whichBLEP]]));
        	//increment the position in the BLEP table
    		c->BLEPindices[whichBLEP] += MINBLEP_PHASES;
    		//check if this BLEP is finished and if so mark it as inactive so it isn't computed anymore.
    		if (c->BLEPindices[whichBLEP] >= (c->maxBLEPphaseSlope)) //slope bleps are 71 length instead of 72 length so subtract 1
    		{
    			c->numBLEPs--;
    		}
    	}
    }

    z += 0.5f * (c->_f[j] - z);
    c->out = z;
    j = (j+1) & 7;
    c->_p = p;
    c->_w = w;
    c->_b = b;
    c->_z = z;
    c->_j = j;
    c->_k = k;

    return -c->out;
}

void tMBTriangle_setFreq(tMBTriangle* const c, float f)
{
    c->freq = f;
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_inv_w = 1.0f / c->_w;
    //c->quarterwaveoffset = c->_w * 0.25f;
}

void tMBTriangle_setWidth(tMBTriangle* const c, float w)
{
    w = LEAF_clip(0.0f, w, 0.99f);
    c->waveform = w;
}

float tMBTriangle_sync(tMBTriangle* const c, float value)
{
    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    float last = c->lastsyncin;
    float delta = value - last;
    float crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;

    return value;
}

void tMBTriangle_setPhase(tMBTriangle* const c, float phase)
{
    c->_p = phase;
}

void tMBTriangle_setSyncMode(tMBTriangle* const c, int hardOrSoft)
{
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBTriangle_setBufferOffset(tMBTriangle* const c, uint32_t offset)
{
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBTriangle_setSampleRate(tMBTriangle* const c, float sr)
{
    c->invSampleRate = 1.0f/sr;
}



//==========================================================================================================

void tMBSineTri_create(tMempool** const mp, tMBSineTri** const osc)
{
    ALLOC_FROM_POOL(tMBSineTri, osc, mp);
}

void tMBSineTri_init(LEAF* const leaf, tMBSineTri* const c)
{


    c->invSampleRate = leaf->invSampleRate;
    c->freq = 440.f;
    c->lastsyncin = 0.0f;
    c->sync = 0.0f;
    c->syncdir = 1.0f;
    c->softsync = 0;
    c->waveform = 0.0f;
    c->shape = 0.0f;
    c->_z = 0.0f;
    c->_j = 0;
    c->_sinPhase = 0.0f;
    c->_p = 0.25f;  /* phase [0, 1) */
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_b = 0.5f * (1.0f + c->waveform);  /* duty cycle (0, 1) */
    c->_k = 0.0f;  /* output state, 0 = high (0.5f), 1 = low (-0.5f) */
    c->_inv_w = 1.0f / c->_w;
    c->numBLEPs = 0;
    c->mostRecentBLEP = 0;
    c->maxBLEPphase = MINBLEP_PHASES * STEP_DD_PULSE_LENGTH;
    c->maxBLEPphaseSlope = MINBLEP_PHASES * SLOPE_DD_PULSE_LENGTH;
    c->sineMask = 2047;
    memset (c->BLEPindices, 0, 64 * sizeof (uint16_t));
    memset (c->_f, 0, 8 * sizeof (float));

}

void tMBSineTri_free(tMBSineTri** const osc)
{
    tMBSineTri* c = *osc;
    mpool_free((char*)c, c->mempool);
}

//#ifdef ITCMRAM
//void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSineTri_place_dd_noBuffer(tMBSineTri* const osc, int index, float phase, float inv_w, float scale, float stepOrSlope, float w)
//#else
void tMBSineTri_place_dd_noBuffer(tMBSineTri* const c, int index, float phase, float inv_w, float scale, float stepOrSlope, float w)
//#endif
{
	float r;
	long i;

	r = MINBLEP_PHASES * phase * inv_w;
	i = lrintf(r - 0.5f);
	r -= (float)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
	scale *= w;
	c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
	c->BLEPindices[c->mostRecentBLEP] = i;
    c->BLEPproperties[c->mostRecentBLEP][0] = r;
    c->BLEPproperties[c->mostRecentBLEP][1] = scale;
    c->BLEPproperties[c->mostRecentBLEP][2] = stepOrSlope;
    c->numBLEPs = (c->numBLEPs + 1) & 63;
}

float tMBSineTri_tick(tMBSineTri* const c)
{
    int    j, k;
    float  sync;
    float  b, b1, invB, invB1, p, sinPhase, w, sw, z;
    float  x = 0.5f;

    sync = c->sync;

    p = c->_p;  /* phase [0, 1) */
    sinPhase = c->_sinPhase;
    w = c->_w;  /* phase increment */
    b = c->_b;  /* duty cycle (0, 1) */
    invB = 1.0f / b;
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */
    k = c->_k;  /* output state, 0 = positive slope, 1 = negative slope */



    b = 0.5f * (1.0f + c->waveform);
    b1 = 1.0f - b;
    invB1 = 1.0f / b1;
    if (sync > 0.0f && c->softsync > 0) c->syncdir = -c->syncdir;

    sw = w * c->syncdir;
    float inv_sw = c->_inv_w * c->syncdir;
    float inc_amount = sw - (int)sw;
    p += inc_amount;
    sinPhase += inc_amount;

    if (sync > 0.0f && c->softsync == 0) {  /* sync to master */
        float eof_offset = sync * sw;
        float p_at_reset = p - eof_offset;
        if (sw > 0)
        {
        	p = eof_offset + 0.25f;
        	sinPhase = eof_offset;
        }
        else if (sw < 0)
        {
        	p = (1.0f - eof_offset) + 0.25f;
        	sinPhase = (1.0f - eof_offset);
        }
        //
        /* place any DDs that may have occurred in subsample before reset */

        if (!k) {
            x = -0.5f + p_at_reset * invB;
            if (sw > 0)
            {
                if (p_at_reset >= b) {
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBSineTri_place_dd_noBuffer(c, j, p_at_reset - b + eof_offset, inv_sw, -invB1 - invB, 1.0f, sw);
                    k = 1;
                }
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    x = -0.5f + p_at_reset * invB;
                    tMBSineTri_place_dd_noBuffer(c, j, p_at_reset + eof_offset, inv_sw, invB + invB1, 1.0f, sw);
                    k = 0;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    x = 0.5f - (p_at_reset - b)  * invB1;
                    tMBSineTri_place_dd_noBuffer(c, j, 1.0f - p_at_reset - eof_offset, -inv_sw, invB + invB1, 1.0f, -sw);
                    k = 1;
                }
                if (k && p_at_reset < b) {
                    x = -0.5f + p_at_reset * invB;
                    tMBSineTri_place_dd_noBuffer(c, j, b - p_at_reset - eof_offset, -inv_sw, -invB1 - invB, 1.0f, -sw);
                    k = 0;
                }
            }
        } else {
            x = 0.5f - (p_at_reset - b) * invB1;
            if (sw > 0)
            {
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    x = -0.5f + p_at_reset * invB;
                    tMBSineTri_place_dd_noBuffer(c, j, p_at_reset + eof_offset, inv_sw, invB + invB1, 1.0f, sw);
                    k = 0;
                }
                if (!k && p_at_reset >= b) {
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBSineTri_place_dd_noBuffer(c, j, p_at_reset - b + eof_offset, inv_sw, -invB1 - invB, 1.0f, sw);
                    k = 1;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < b) {
                    x = -0.5f + p_at_reset * invB;
                    tMBSineTri_place_dd_noBuffer(c, j, b - p_at_reset - eof_offset, -inv_sw, -invB1 - invB, 1.0f, -sw);
                    k = 0;
                }
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBSineTri_place_dd_noBuffer(c, j, 1.0f - p_at_reset - eof_offset, -inv_sw, invB + invB1, 1.0f, -sw);
                    k = 1;
                }
            }
        }

        /* now place reset DDs */
        if (sw > 0)
        {
            if (k)
            	tMBSineTri_place_dd_noBuffer(c, j, p, inv_sw, invB + invB1, 1.0f, sw);
            tMBSineTri_place_dd_noBuffer(c, j, p, inv_sw, 0.0f - x, 0.0f, sw);
            x = -0.5f + p * invB;
            k = 0;
            if (p >= b) {
                x = 0.5f - (p - b) * invB1;
                tMBSineTri_place_dd_noBuffer(c, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
        }
        else if (sw < 0)
        {
            if (!k)
            	tMBSineTri_place_dd_noBuffer(c, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
            tMBSineTri_place_dd_noBuffer(c, j, 1.0f - p, -inv_sw, 0.0f - x, 0.0f, -sw);
            x = 0.5f - (p - b) * invB1;
            k = 1;
            if (p < b) {
                x = -0.5f + p * invB;
                tMBSineTri_place_dd_noBuffer(c, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
        }
    } else if (!k) {  /* normal operation, slope currently up */

        x = -0.5f + p * invB;
        if (sw > 0)
        {
            if (p >= b) {
                x = 0.5f - (p - b) * invB1;;
                tMBSineTri_place_dd_noBuffer(c, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
            if (p >= 1.0f) {
                p -= 1.0f;
                x = -0.5f + p * invB;
                tMBSineTri_place_dd_noBuffer(c, j, p, inv_sw, invB + invB1, 1.0f, sw);
                k = 0;
            }
        }
        else if (sw < 0)
        {
            if (p < 0.0f) {
                p += 1.0f;
                x = 0.5f - (p - b) * invB1;
                tMBSineTri_place_dd_noBuffer(c, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
                k = 1;
            }
            if (k && p < b) {
                x = -0.5f + p * invB;
                tMBSineTri_place_dd_noBuffer(c, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
        }

    } else {  /* normal operation, slope currently down */

        x = 0.5f - (p - b) * invB1;
        if (sw > 0)
        {
            if (p >= 1.0f) {
                p -= 1.0f;
                x = -0.5f + p * invB;
                tMBSineTri_place_dd_noBuffer(c, j, p, inv_sw, invB + invB1, 1.0f, sw);
                k = 0;
            }
            if (!k && p >= b) {
                x = 0.5f - (p - b) * invB1;
                tMBSineTri_place_dd_noBuffer(c, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
        }
        else if (sw < 0)
        {
            if (p < b) {
                x = -0.5f + p * invB;
                tMBSineTri_place_dd_noBuffer(c, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
            if (p < 0.0f) {
                p += 1.0f;
                x = 0.5f - (p - b) * invB1;
                tMBSineTri_place_dd_noBuffer(c, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
                k = 1;
            }
        }
    }
    int currentSamp = (j + DD_SAMPLE_DELAY) & 7;

    c->_f[currentSamp] = x * c->shape; //add the triangle


    float tempFrac;
    uint32_t idx;
    float samp0;
    float samp1;

    // Wavetable synthesis
    while (sinPhase >= 1.0f)
    {
    	sinPhase -= 1.0f;
    }

    while (sinPhase < 0.0f)
    {
    	sinPhase += 1.0f;
    }
    float tempPhase = (sinPhase * 2048.0f);
    idx = (uint32_t)tempPhase; //11 bit table
    tempFrac = tempPhase - idx;
    samp0 = __leaf_table_sinewave[idx];
    idx = (idx + 1) & c->sineMask;
    samp1 = __leaf_table_sinewave[idx];

    float sinOut = (samp0 + (samp1 - samp0) * tempFrac) * 0.5f; // 1/2097151

    c->_f[currentSamp] += sinOut * (1.0f - c->shape); //add the sine


    volatile uint8_t numBLEPsAtLoopStart = c->numBLEPs;
    for (int i = 0; i < numBLEPsAtLoopStart; i++)
    {
    	volatile uint16_t whichBLEP = (c->mostRecentBLEP - i);
    	whichBLEP &= 63;

    	//use the scale and r values from the BLEPproperties array to compute the current state of each active BLEP and add it to the output value

    	if (c->BLEPproperties[whichBLEP][2] < 0.5f) //step blep
    	{
    		c->_f[j] += c->BLEPproperties[whichBLEP][1] * (step_dd_table[c->BLEPindices[whichBLEP]].value + c->BLEPproperties[whichBLEP][0] * step_dd_table[c->BLEPindices[whichBLEP]].delta);
        	//increment the position in the BLEP table
    		c->BLEPindices[whichBLEP] += MINBLEP_PHASES;
    		//check if this BLEP is finished and if so mark it as inactive so it isn't computed anymore.
    		if (c->BLEPindices[whichBLEP] >= c->maxBLEPphase)
    		{
    			c->numBLEPs--;
    		}
    	}
    	else  // slope blep
    	{
    		c->_f[j] += (c->BLEPproperties[whichBLEP][1] * (slope_dd_table[c->BLEPindices[whichBLEP]] + c->BLEPproperties[whichBLEP][0] * (slope_dd_table[c->BLEPindices[whichBLEP]+1] - slope_dd_table[c->BLEPindices[whichBLEP]]))) * c->shape;
        	//increment the position in the BLEP table
    		c->BLEPindices[whichBLEP] += MINBLEP_PHASES;
    		//check if this BLEP is finished and if so mark it as inactive so it isn't computed anymore.
    		if (c->BLEPindices[whichBLEP] >= (c->maxBLEPphaseSlope)) //slope bleps are 71 length instead of 72 length so subtract 1
    		{
    			c->numBLEPs--;
    		}
    	}
    }

    z += 0.5f * (c->_f[j] - z);
    j = (j+1) & 7;
    c->out = z;
    c->_p = p;
    c->_w = w;
    c->_b = b;
    c->_z = z;
    c->_j = j;
    c->_k = k;
    c->_sinPhase = sinPhase;

    return -c->out;
}

void tMBSineTri_setFreq(tMBSineTri* const c, float f)
{
    c->freq = f;
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_inv_w = 1.0f / c->_w;
    //c->quarterwaveoffset = c->_w * 0.25f;
}

void tMBSineTri_setWidth(tMBSineTri* const c, float w)
{
    w = LEAF_clip(0.0f, w, 0.99f);
    c->waveform = w;
}

float tMBSineTri_sync(tMBSineTri* const c, float value)
{
    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    float last = c->lastsyncin;
    float delta = value - last;
    float crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;

    return value;
}

void tMBSineTri_setPhase(tMBSineTri* const c, float phase)
{
    c->_p = phase;
}

void tMBSineTri_setShape(tMBSineTri* const c, float shape)
{
    c->shape = shape;
}


void tMBSineTri_setSyncMode(tMBSineTri* const c, int hardOrSoft)
{
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBSineTri_setBufferOffset(tMBSineTri* const c, uint32_t offset)
{
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBSineTri_setSampleRate(tMBSineTri* const c, float sr)
{
    c->invSampleRate = 1.0f/sr;
}
//==================================================================================================
//==================================================================================================

void tMBSaw_create(tMempool** const mp, tMBSaw** const osc)
{
    ALLOC_FROM_POOL(tMBSaw, osc, mp);
}

void tMBSaw_init(LEAF* const leaf, tMBSaw* const c)
{


    c->invSampleRate = leaf->invSampleRate;
    c->freq = 440.f;
    c->lastsyncin = 0.0f;
    c->sync = 0.0f;
    c->syncdir = 1.0f;
    c->softsync = 0;
    c->_z = 0.0f;
    c->_j = 0;
    c->_p = 0.0f;  /* phase [0, 1) */
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_inv_w = 1.0f / c->_w;
    c->numBLEPs = 0;
    c->mostRecentBLEP = 0;
    c->maxBLEPphase = MINBLEP_PHASES * STEP_DD_PULSE_LENGTH;
    memset (c->BLEPindices, 0, 64 * sizeof (uint16_t));
    memset (c->_f, 0, 8 * sizeof (float));

}

void tMBSaw_free(tMBSaw** const osc)
{
    tMBSaw* c = *osc;
    mpool_free((char*)c, c->mempool);
}


//#ifdef ITCMRAM
//void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSaw_place_step_dd_noBuffer(tMBSaw* const osc, int index, float phase, float inv_w, float scale)
//#else
void tMBSaw_place_step_dd_noBuffer(tMBSaw* const c, int index, float phase, float inv_w, float scale)
//#endif
{
	float r;
	long i;

	r = MINBLEP_PHASES * phase * inv_w;
	i = lrintf(r - 0.5f);
	r -= (float)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
	c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
	c->BLEPindices[c->mostRecentBLEP] = i;
    c->BLEPproperties[c->mostRecentBLEP][0] = r;
    c->BLEPproperties[c->mostRecentBLEP][1] = scale;
    c->numBLEPs = (c->numBLEPs + 1) & 63;
}



float tMBSaw_tick(tMBSaw* const c)
{
    int    j;
    float  sync;
    float  p, sw, z;

    sync = c->sync;


    p = c->_p;  /* phase [0, 1) */
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */


    if (sync > 0.0f && c->softsync > 0) c->syncdir = -c->syncdir;
    sw = c->_w * c->syncdir;
    float inv_sw = c->_inv_w * c->syncdir;
    p += sw - (int)sw;

   //if (sync > 0.0f && c->softsync > 0) {
    // Should insert minblep for softsync?
	//	if (p_at_reset >= 1.0f) {
	//		p_at_reset -= (int)p_at_reset;
	//		place_slope_dd(osc, j, p_at_reset + eof_offset, inv_sw, 2.0f);
	//	}
	//	if (p_at_reset < 0.0f) {
	//		p_at_reset += 1.0f - (int)p_at_reset;
	//		place_slope_dd(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -2.0f);
	//	}
	//	if (sw > 0) place_slope_dd(osc, j, p, inv_sw, 2.0f);
	//	else if (sw < 0) place_slope_dd(osc, j, 1.0f - p, -inv_sw, -2.0f);
    //}

    if (sync > 0.0f && c->softsync == 0) {  /* sync to master */
        float eof_offset = sync * sw;
        float p_at_reset = p - eof_offset;

        if (sw > 0) p = eof_offset;
        else if (sw < 0) p = 1.0f - eof_offset;

        /* place any DD that may have occurred in subsample before reset */
        if (p_at_reset >= 1.0f) {
            p_at_reset -= 1.0f;
            tMBSaw_place_step_dd_noBuffer(c, j, p_at_reset + eof_offset, inv_sw, 1.0f);
        }
        if (p_at_reset < 0.0f) {
            p_at_reset += 1.0f;
            tMBSaw_place_step_dd_noBuffer(c, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
        }

        /* now place reset DD */
        if (sw > 0)
        	tMBSaw_place_step_dd_noBuffer(c, j, p, inv_sw, p_at_reset);
        else if (sw < 0)
        	tMBSaw_place_step_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -p_at_reset);

    } else if (p >= 1.0f) {  /* normal phase reset */
        p -= 1.0f;
        tMBSaw_place_step_dd_noBuffer(c, j, p, inv_sw, 1.0f);

    } else if (p < 0.0f) {
        p += 1.0f;
        tMBSaw_place_step_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -1.0f);
    }

    //construct the current output sample based on the state of the active BLEPs

    int currentSamp = (j + DD_SAMPLE_DELAY) & 7;

    c->_f[currentSamp] = 0.5f - p;

    volatile uint8_t numBLEPsAtLoopStart = c->numBLEPs;
    for (int i = 0; i < numBLEPsAtLoopStart; i++)
    {
    	volatile uint16_t whichBLEP = (c->mostRecentBLEP - i);
    	whichBLEP &= 63;

    	//use the scale and r values from the BLEPproperties array to compute the current state of each active BLEP and add it to the output value
    	c->_f[j] += c->BLEPproperties[whichBLEP][1] * (step_dd_table[c->BLEPindices[whichBLEP]].value + c->BLEPproperties[whichBLEP][0] * step_dd_table[c->BLEPindices[whichBLEP]].delta);

    	//increment the position in the BLEP table
		c->BLEPindices[whichBLEP] += MINBLEP_PHASES;
		//check if this BLEP is finished and if so mark it as inactive so it isn't computed anymore.
		if (c->BLEPindices[whichBLEP] >= c->maxBLEPphase)
		{
			c->numBLEPs--;
		}

    }

    z += 0.5f * (c->_f[j] - z); // LP filtering
    c->out = z;
    j = (j+1) & 7; //don't need 128 sample buffer just for lowpass, so only using the first 16 values before wrapping around (probably only need 4 or 8)

    c->_p = p;
    c->_z = z;
    c->_j = j;


    return -c->out;
}

void tMBSaw_setFreq(tMBSaw* const c, float f)
{
    c->freq = f;

    c->_w = c->freq * c->invSampleRate;
    c->_inv_w = 1.0f / c->_w;
}

float tMBSaw_sync(tMBSaw* const c, float value)
{
    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    float last = c->lastsyncin;
    float delta = value - last;
    float crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;

    return value;
}

void tMBSaw_setPhase(tMBSaw* const c, float phase)
{
    c->_p = phase;
}

void tMBSaw_setSyncMode(tMBSaw* const c, int hardOrSoft)
{
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBSaw_setBufferOffset(tMBSaw* const c, uint32_t offset)
{
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBSaw_setSampleRate(tMBSaw* const c, float sr)
{
    c->invSampleRate = 1.0f/sr;
}


//==================================================================================================

void tMBSawPulse_create(tMempool** const mp, tMBSawPulse** const osc)
{
    ALLOC_FROM_POOL(tMBSawPulse, osc, mp);
}

void tMBSawPulse_init(LEAF* const leaf, tMBSawPulse* const c)
{

    c->gain = 1.0f;
    c->active = 1;
    c->invSampleRate = leaf->invSampleRate;
    c->freq = 440.f;
    c->lastsyncin = 0.0f;
    c->sync = 0.0f;
    c->syncdir = 1.0f;
    c->softsync = 0;
    c->waveform = 0.0f;
    c->_z = 0.0f;
    c->_j = 0;
    c->_p = 0.0f;  /* phase [0, 1) */
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_b = 0.5f * (1.0f + c->waveform);  /* duty cycle (0, 1) */
    c->_x = 0.5f;  /* temporary output variable */
    c->_k = 0.0f;  /* output state, 0 = high (0.5f), 1 = low (-0.5f) */
    c->_inv_w = 1.0f / c->_w;
    c->numBLEPs = 0;
    c->mostRecentBLEP = 0;
    c->maxBLEPphase = MINBLEP_PHASES * STEP_DD_PULSE_LENGTH;
    memset (c->BLEPindices, 0, 64 * sizeof (uint16_t));
    memset (c->_f, 0, 8 * sizeof (float));


}

void tMBSawPulse_free(tMBSawPulse** const osc)
{
    tMBSawPulse* c = *osc;
    mpool_free((char*)c, c->mempool);
}


#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSawPulse_place_step_dd_noBuffer(tMBSawPulse* const osc, int index, float phase, float inv_w, float scale)
#else
void tMBSawPulse_place_step_dd_noBuffer(tMBSawPulse* const c, int index, float phase, float inv_w, float scale)
#endif
{
    if (c->active)
    {
		float r;
		long i;

		r = MINBLEP_PHASES * phase * inv_w;
		i = lrintf(r - 0.5f);
		r -= (float)i;
		i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
		c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
		c->BLEPindices[c->mostRecentBLEP] = i;
		c->BLEPproperties[c->mostRecentBLEP][0] = r;
		c->BLEPproperties[c->mostRecentBLEP][1] = scale;

		c->numBLEPs = (c->numBLEPs + 1) & 63;
    }
}



#ifdef ITCMRAM
float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSawPulse_tick(tMBSawPulse* const osc)
#else
float tMBSawPulse_tick(tMBSawPulse* const c)
#endif
{
    int    j, k;
    float  sync;
    float  b, p, w, x, z, sw;
    float shape = c->shape;
    float sawShape = 1.0f - c->shape;
    sync = c->sync;
    p = c->_p;  /* phase [0, 1) */
    w = c->_w;  /* phase increment */
    b = c->_b;  /* duty cycle (0, 1) */
    x = c->_x;  /* temporary output variable */
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */
    k = c->_k;  /* output state, 0 = high (0.5f), 1 = low (-0.5f) */

    if (sync > 0.0f && c->softsync > 0) c->syncdir = -c->syncdir;

    sw = w * c->syncdir;
    float inv_sw = c->_inv_w * c->syncdir;
    p += sw - (int)sw;
    if (sync > 0.0f && c->softsync == 0)
    {  /* sync to master */
        float eof_offset = sync * sw;
        float p_at_reset = p - eof_offset;

        if (sw > 0) p = eof_offset;
        else if (sw < 0) p = 1.0f - eof_offset;

        //pulse stuff
		 /* place any DDs that may have occurred in subsample before reset */
		 if (!k)
		 {
			 if (sw > 0)
			 {
				 if (p_at_reset >= b)
				 {
					 tMBSawPulse_place_step_dd_noBuffer(c, j, p_at_reset - b + eof_offset, inv_sw, -1.0f * shape);
					 k = 1;
					 x = -0.5f;
				 }
				 if (p_at_reset >= 1.0f)
				 {
					 p_at_reset -= 1.0f;
					 tMBSawPulse_place_step_dd_noBuffer(c, j, p_at_reset + eof_offset, inv_sw, 1.0f);
					 k = 0;
					 x = 0.5f;
				 }
			 }
			 else if (sw < 0)
			 {
				 if (p_at_reset < 0.0f)
				 {
					 p_at_reset += 1.0f;
					 tMBSawPulse_place_step_dd_noBuffer(c, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
					 k = 1;
					 x = -0.5f;
				 }
				 if (k && p_at_reset < b)
				 {
					 tMBSawPulse_place_step_dd_noBuffer(c, j, b - p_at_reset - eof_offset, -inv_sw, 1.0f * shape);
					 k = 0;
					 x = 0.5f;
				 }
			 }
		 }
		 else
		 {
			 if (sw > 0)
			 {
				 if (p_at_reset >= 1.0f)
				 {
					 p_at_reset -= 1.0f;
					 tMBSawPulse_place_step_dd_noBuffer(c, j, p_at_reset + eof_offset, inv_sw, 1.0f);
					 k = 0;
					 x = 0.5f;
				 }
				 if (!k && p_at_reset >= b)
				 {
					 tMBSawPulse_place_step_dd_noBuffer(c, j, p_at_reset - b + eof_offset, inv_sw, -1.0f * shape);
					 k = 1;
					 x = -0.5f;
				 }
			 }
			 else if (sw < 0)
			 {
				 if (p_at_reset < b)
				 {
					 tMBSawPulse_place_step_dd_noBuffer(c, j, b - p_at_reset - eof_offset, -inv_sw, 1.0f * shape);
					 k = 0;
					 x = 0.5f;
				 }
				 if (p_at_reset < 0.0f)
				 {
					 p_at_reset += 1.0f;
					 tMBSawPulse_place_step_dd_noBuffer(c, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
					 k = 1;
					 x = -0.5f;
				 }
			 }
		 }



		if (sw > 0)
		{
			/* now place reset DD for saw*/
			tMBSawPulse_place_step_dd_noBuffer(c, j, p, inv_sw, p_at_reset * sawShape);
            /* now place reset DD for pulse */
            if (k) {
            	tMBSawPulse_place_step_dd_noBuffer(c, j, p, inv_sw, 1.0f * shape);
				k = 0;
				x = 0.5f;
			}
			if (p >= b) {
				tMBSawPulse_place_step_dd_noBuffer(c, j, p - b, inv_sw, -1.0f * shape);
				k = 1;
				x = -0.5f;
			}
		}
		else if (sw < 0)
		{
	        /* now place reset DD for saw*/
			tMBSawPulse_place_step_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -p_at_reset * sawShape);
			 /* now place reset DD for pulse */
			if (!k) {
				tMBSawPulse_place_step_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -1.0f * shape);
				k = 1;
				x = -0.5f;
			}
			if (p < b) {
				tMBSawPulse_place_step_dd_noBuffer(c, j, b - p, -inv_sw, 1.0f * shape);
				k = 0;
				x = 0.5f;
			}
		}


    }


    else if (!k)
    {  /* normal operation for pulse, signal currently high */

		if (sw > 0)
		{
			if (p >= b) {
				tMBSawPulse_place_step_dd_noBuffer(c, j, p - b, inv_sw, -1.0f * shape);
				k = 1;
				x = -0.5f;
			}
			if (p >= 1.0f) {
				p -= 1.0f;
				tMBSawPulse_place_step_dd_noBuffer(c, j, p, inv_sw, 1.0f);
				k = 0;
				x = 0.5f;
			}
		}
		else if (sw < 0)
		{
			if (p < 0.0f) {
				p += 1.0f;
				tMBSawPulse_place_step_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -1.0f);
				k = 1;
				x = -0.5f;
			}
			if (k && p < b) {
				tMBSawPulse_place_step_dd_noBuffer(c, j, b - p, -inv_sw, 1.0f * shape);
				k = 0;
				x = 0.5f;
			}
		}

	} else {  /* normal operation, signal currently low */

		if (sw > 0)
		{
			if (p >= 1.0f) {
				p -= 1.0f;
				tMBSawPulse_place_step_dd_noBuffer(c, j, p, inv_sw, 1.0f);
				k = 0;
				x = 0.5f;
			}
			if (!k && p >= b) {
				tMBSawPulse_place_step_dd_noBuffer(c, j, p - b, inv_sw, -1.0f * shape);
				k = 1;
				x = -0.5f;
			}
		}
		else if (sw < 0)
		{
			if (p < b) {
				tMBSawPulse_place_step_dd_noBuffer(c, j, b - p, -inv_sw, 1.0f * shape);
				k = 0;
				x = 0.5f;
			}
			if (p < 0.0f) {
				p += 1.0f;
				tMBSawPulse_place_step_dd_noBuffer(c, j, 1.0f - p, -inv_sw, -1.0f);
				k = 1;
				x = -0.5f;
			}
		}
	}
    int currentSamp = (j + DD_SAMPLE_DELAY) & 7;
    c->_f[currentSamp] = ((0.5f - p) * sawShape); //saw

    c->_f[currentSamp] += (x * shape);//pulse

    volatile uint8_t numBLEPsAtLoopStart = c->numBLEPs;
    for (int i = 0; i < numBLEPsAtLoopStart; i++)
    {
    	volatile uint16_t whichBLEP = (c->mostRecentBLEP - i);
    	whichBLEP &= 63;

    	//use the scale and r values from the BLEPproperties array to compute the current state of each active BLEP and add it to the output value
    	c->_f[j] += c->BLEPproperties[whichBLEP][1] * (step_dd_table[c->BLEPindices[whichBLEP]].value + c->BLEPproperties[whichBLEP][0] * step_dd_table[c->BLEPindices[whichBLEP]].delta);

    	//increment the position in the BLEP table
		c->BLEPindices[whichBLEP] += MINBLEP_PHASES;
		//check if this BLEP is finished and if so mark it as inactive so it isn't computed anymore.
		if (c->BLEPindices[whichBLEP] >= c->maxBLEPphase)
		{
			c->numBLEPs--;
		}

    }


    z += 0.5f * (c->_f[j] - z); // LP filtering
    c->out = z;
    j = (j+1) & 7;

    c->_p = p;
    c->_w = w;
    c->_b = b;
    c->_x = x;
    c->_z = z;
    c->_j = j;
    c->_k = k;

    return -c->out * c->gain;
}
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tMBSawPulse_setFreq(tMBSawPulse* const osc, float f)
#else
void tMBSawPulse_setFreq(tMBSawPulse* const c, float f)
#endif
{
    c->freq = f;
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_inv_w = 1.0f / c->_w;
    if ((f > 6000.0f) || (f < -6000.0f))
    {
    	c->active = 0;
    	c->gain = 0.0f;
    }
    else
    {
    	c->active = 1;
    	c->gain = 1.0f;
    }
}

#ifdef ITCMRAM
float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tMBSawPulse_sync(tMBSawPulse* const osc, float value)
#else
float tMBSawPulse_sync(tMBSawPulse* const c, float value)
#endif
{
    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    float last = c->lastsyncin;
    float delta = value - last;
    float crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;

    return value;
}

void tMBSawPulse_setPhase(tMBSawPulse* const c, float phase)
{
    c->_p = phase;
}

void tMBSawPulse_setShape(tMBSawPulse* const c, float shape)
{
    c->shape = shape;
}

void tMBSawPulse_setSyncMode(tMBSawPulse* const c, int hardOrSoft)
{
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBSawPulse_setBufferOffset(tMBSawPulse* const c, uint32_t offset)
{
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBSawPulse_setSampleRate(tMBSawPulse* const c, float sr)
{
    c->invSampleRate = 1.0f/sr;
}


// WaveTable
void tTable_create(tMempool** const mp, tTable** const cy)
{
    ALLOC_FROM_POOL(tTable, cy, mp);
}

void tTable_init(LEAF* const leaf, tTable* const c, float* waveTable, int size)
{


    c->waveTable = waveTable;
    c->size = size;
    c->inc = 0.0f;
    c->phase = 0.0f;
    c->invSampleRate = leaf->invSampleRate;

}

void    tTable_free(tTable** const cy)
{
    tTable* c = *cy;

    mpool_free((char*)c, c->mempool);
}

void     tTable_setFreq(tTable* const c, float freq)
{
    if (!isfinite(freq)) return;

    c->freq = freq;
    c->inc = freq * c->invSampleRate;
    c->inc -= (int)c->inc;
}

float   tTable_tick(tTable* const c)
{
    float temp;
    int intPart;
    float fracPart;
    float samp0;
    float samp1;

    // Phasor increment
    c->phase += c->inc;
    if (c->phase >= 1.0f) c->phase -= 1.0f;
    if (c->phase < 0.0f) c->phase += 1.0f;

    // Wavetable synthesis

    temp = c->size * c->phase;
    intPart = (int)temp;
    fracPart = temp - (float)intPart;
    samp0 = c->waveTable[intPart];
    if (++intPart >= c->size) intPart = 0;
    samp1 = c->waveTable[intPart];

    return (samp0 + (samp1 - samp0) * fracPart);
}

void     tTable_setSampleRate(tTable* const c, float sr)
{
    c->invSampleRate = 1.0f/sr;
    tTable_setFreq(c, c->freq);
}

void tWaveTable_create(tMempool** const mp, tWaveTable** const cy)
{
    ALLOC_FROM_POOL(tWaveTable, cy, mp);
}

void tWaveTable_init(LEAF* const leaf, tWaveTable* const c, float* table, int size, float maxFreq)
{


    c->sampleRate = leaf->sampleRate;

    c->maxFreq = maxFreq;

    // Determine base frequency
    c->baseFreq = c->sampleRate / (float) size;
    c->invBaseFreq = 1.0f / c->baseFreq;

    // Determine how many tables we need
    // Assume we need at least 2, the fundamental + one to account for setting extra anti aliasing
    c->numTables = 2;
    float f = c->baseFreq;
    while (f < c->maxFreq)
    {
        c->numTables++;
        f *= 2.0f; // pass this multiplier in to set spacing of tables? would need to change setFreq too

}

    c->size = size;
    c->sizeMask = size-1;
    // Allocate memory for the tables
    c->tables = (float**) mpool_alloc(sizeof(float*) * c->numTables, c->mempool);
    c->baseTable = (float*) mpool_alloc(sizeof(float) * c->size, c->mempool);
    c->tables[0] = c->baseTable;
    for (int t = 1; t < c->numTables; ++t)
    {
        c->tables[t] = (float*) mpool_alloc(sizeof(float) * c->size, c->mempool);
    }

    // Copy table
    for (int i = 0; i < c->size; ++i)
    {
        c->baseTable[i] = table[i];
    }

    // Make bandlimited copies
    f = c->sampleRate * 0.25f; //start at half nyquist
    // Not worth going over order 8 I think, and even 8 is only marginally better than 4.
    tButterworth_create(&c->mempool,&c->bl);
      tButterworth_init(leaf,c->bl  , 8, -1.0f, f);
    for (int t = 1; t < c->numTables; ++t)
    {
        tButterworth_setF2(c->bl, f);
        // Do several passes here to prevent errors at the beginning of the waveform
        // Not sure how many passes to do, seem to need more as the filter cutoff goes down
        // 12 might be excessive but seems to work for now.
        for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
        {
            for (int i = 0; i < c->size; ++i)
            {
                c->tables[t][i] = tButterworth_tick(c->bl, c->tables[t-1][i]);
            }
        }
        f *= 0.5f; //halve the cutoff for next pass
    }
    tButterworth_free(&c->bl);
}

void tWaveTable_free(tWaveTable** const cy)
{
    tWaveTable* c = *cy;

    mpool_free((char*)c->baseTable, c->mempool);
    for (int t = 1; t < c->numTables; ++t)
    {
        mpool_free((char*)c->tables[t], c->mempool);
    }
    mpool_free((char*)c->tables, c->mempool);
    mpool_free((char*)c, c->mempool);
}

void tWaveTable_setSampleRate(LEAF* const leaf, tWaveTable* const c, float sr)
{
    // Changing the sample rate of a wavetable requires up to partially reinitialize
    for (int t = 1; t < c->numTables; ++t)
    {
        mpool_free((char*)c->tables[t], c->mempool);
    }
    mpool_free((char*)c->tables, c->mempool);

    c->sampleRate = sr;

    // Determine base frequency
    c->baseFreq = c->sampleRate / (float) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;

    // Determine how many tables we need
    // Assume we need at least 2, the fundamental + one to account for setting extra anti aliasing
    c->numTables = 2;
    float f = c->baseFreq;
    while (f < c->maxFreq)
    {
        c->numTables++;
        f *= 2.0f; // pass this multiplier in to set spacing of tables? would need to change setFreq too
    }

    // Allocate memory for the tables
    c->tables = (float**) mpool_alloc(sizeof(float*) * c->numTables, c->mempool);
    c->tables[0] = c->baseTable;
    for (int t = 1; t < c->numTables; ++t)
    {
        c->tables[t] = (float*) mpool_alloc(sizeof(float) * c->size, c->mempool);
    }

    // Make bandlimited copies
    f = c->sampleRate * 0.25f; //start at half nyquist
    // Not worth going over order 8 I think, and even 8 is only marginally better than 4.
    tButterworth_create(&c->mempool,&c->bl);
    tButterworth_init(leaf,c->bl, 8, -1.0f, f);
    tButterworth_setSampleRate(c->bl, c->sampleRate);
    for (int t = 1; t < c->numTables; ++t)
    {
        tButterworth_setF2(c->bl, f);
        // Do several passes here to prevent errors at the beginning of the waveform
        // Not sure how many passes to do, seem to need more as the filter cutoff goes down
        // 12 might be excessive but seems to work for now.
        for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
        {
            for (int i = 0; i < c->size; ++i)
            {
                c->tables[t][i] = tButterworth_tick(c->bl, c->tables[t-1][i]);
            }
        }
        f *= 0.5f; //halve the cutoff for next pass
    }
    tButterworth_free(&c->bl);
}

//================================================================================================
//================================================================================================

void tWaveOsc_create(tMempool** const mp, tWaveOsc** const cy)
{
    ALLOC_FROM_POOL(tWaveOsc, cy, mp);
}

void tWaveOsc_init(LEAF* const leaf, tWaveOsc* const c, tWaveTable** tables, int numTables)
{

    c->tables =  tables;
    c->numTables = numTables;

    c->index = 0.0f;
    c->o1 = 0;
    c->o2 = 1;
    c->mix = 0.0f;
    c->phase = 0;
    c->inc = 0;
    c->oct = 0;
    c->size = c->tables[0]->size;
    c->w = 0.0f;
    c->aa = 0.5f;
    c->sampleRate = leaf->sampleRate;
    // Determine base frequency
    c->baseFreq = c->sampleRate / (float) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;

    c->invSampleRateTimesTwoTo32 = leaf->invSampleRate * TWO_TO_32;
    c->maxFreq = c->tables[0]->maxFreq;

}

void tWaveOsc_free(tWaveOsc** const cy)
{
    tWaveOsc* c = *cy;
    mpool_free((char*)c, c->mempool);
}

float tWaveOsc_tick(tWaveOsc* const c)
{
    // Phasor increment (unsigned 32bit int wraps automatically with overflow so no need for if branch checks, as you need with float)
    c->phase += c->inc;
    float floatPhase = (double)c->phase * 2.32830643654e-10;
    float s1 = 0.f, s2 = 0.f;

    float temp;
    int idx;
    float frac;
    float samp0;
    float samp1;

    int oct = c->oct;

    int sizeMask = c->tables[c->o1]->sizeMask;
    float** tables = c->tables[c->o1]->tables;

    // Wavetable synthesis
    temp = sizeMask * floatPhase;
    idx = (int)temp;
    frac = temp - (float)idx;
    samp0 = tables[oct][idx];
    idx = (idx + 1) & sizeMask;
    samp1 = tables[oct][idx];

    float oct0 = (samp0 + (samp1 - samp0) * frac);

    temp = sizeMask * floatPhase;
    idx = (int)temp;
    frac = temp - (float)idx;
    samp0 = tables[oct+1][idx];
    idx = (idx + 1) & sizeMask;
    samp1 = tables[oct+1][idx];

    float oct1 = (samp0 + (samp1 - samp0) * frac);

    s1 = oct0 + (oct1 - oct0) * c->w;

    sizeMask = c->tables[c->o2]->sizeMask;
    tables = c->tables[c->o2]->tables;

    // Wavetable synthesis
    temp = sizeMask * floatPhase;
    idx = (int)temp;
    frac = temp - (float)idx;
    samp0 = tables[oct][idx];
    idx = (idx + 1) & sizeMask;
    samp1 = tables[oct][idx];

    oct0 = (samp0 + (samp1 - samp0) * frac);

    temp = sizeMask * floatPhase;
    idx = (int)temp;
    frac = temp - (float)idx;
    samp0 = tables[oct+1][idx];
    idx = (idx + 1) & sizeMask;
    samp1 = tables[oct+1][idx];

    oct1 = (samp0 + (samp1 - samp0) * frac);

    s2 = oct0 + (oct1 - oct0) * c->w;

    // Ideally should determine correlation to get a good equal power fade between tables
    return s1 + (s2 - s1) * c->mix;
}

void tWaveOsc_setFreq(tWaveOsc* const c, float freq)
{
    c->freq  = freq;

    c->inc = c->freq * c->invSampleRateTimesTwoTo32;

    // abs for negative frequencies
    c->w = fabsf(c->freq * c->invBaseFreq);

    // Probably ok to use a log2 approx here; won't affect tuning at all, just crossfading between octave tables
    // I bet we could turn this into a lookup and save a lot of processing
    c->w = log2f_approx(c->w) + c->aa;//+ LEAF_SQRT2 - 1.0f; adding an offset here will shift our table selection upward, reducing aliasing but lower high freq fidelity. +1.0f should remove all aliasing
    if (c->w < 0.0f) c->w = 0.0f; // If c->w is < 0.0f, then freq is less than our base freq
    c->oct = (int)c->w;
    c->w -= c->oct;
    if (c->oct >= c->numSubTables - 1) c->oct = c->numSubTables - 2;
}

void tWaveOsc_setAntiAliasing(tWaveOsc* const c, float aa)
{
    c->aa = aa;
}

void tWaveOsc_setIndex(tWaveOsc* const c, float index)
{
    c->index = index;
    float f = c->index * (c->numTables - 1);

    c->o1 = (int)f;
    c->o2 = c->o1 + 1;
    if (c->index >= 1.0f) c->o2 = c->o1;
    c->mix = f - c->o1;
}

void tWaveOsc_setTables(tWaveOsc* const c, tWaveTable* tables, int numTables)
{
    LEAF* leaf = c->mempool->leaf;
    c->tables =  &tables;
    c->numTables = numTables;
    c->size = c->tables[0]->size;

    c->sampleRate = leaf->sampleRate;
    // Determine base frequency
    c->baseFreq = c->sampleRate / (float) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;
}

/*//// eventually gotta finish this so you can X/Y control the indices and fade between non-adjacent tables
void tWaveOscS_setIndexXY(tWaveOscS* const cy, float indexX, float indexY)
{
    _tWaveOscS* c = *cy;
    c->index = index;
    float f1 = c->index * (c->numTables - 1);

    c->o1 = (int)f1;
    c->o2 = c->o1 + 1;
    if (c->index >= 1.0f) c->o2 = c->o1;
    c->mix = f1 - c->o1;

    float f2 = c->index * (c->numTables - 1);
}
*/

void tWaveOsc_setSampleRate(tWaveOsc* const c, float sr)
{
    c->sampleRate = sr;
    // Determine base frequency
    c->baseFreq = c->sampleRate / (float) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;
    c->invSampleRateTimesTwoTo32 = 1.f/c->sampleRate * TWO_TO_32;

    tWaveOsc_setFreq(c, c->freq);
}

//=======================================================================================
//=======================================================================================

void tWaveTableS_create(tMempool** const mp, tWaveTableS** const cy)
{
    ALLOC_FROM_POOL(tWaveTableS, cy, mp);
}

void tWaveTableS_init(LEAF* const leaf, tWaveTableS* const c, float* table, int size, float maxFreq)
{


    c->sampleRate = leaf->sampleRate;

    c->maxFreq = maxFreq;

    // Determine base frequency
    c->baseFreq = c->sampleRate / (float) size;
    c->invBaseFreq = 1.0f / c->baseFreq;

    // Determine how many tables we need
    c->numTables = 2;
    float f = c->baseFreq;
    while (f < maxFreq)
    {
        c->numTables++;
        f *= 2.0f; // pass this multiplier in to set spacing of tables?

}

    // Allocate memory for the tables
    c->tables = (float**) mpool_alloc(sizeof(float*) * c->numTables, c->mempool);
    c->sizes = (int*) mpool_alloc(sizeof(int) * c->numTables, c->mempool);
    c->sizeMasks = (int*) mpool_alloc(sizeof(int) * c->numTables, c->mempool);
    c->sizes[0] = size;
    c->sizeMasks[0] = (c->sizes[0] - 1);
    c->baseTable = (float*) mpool_alloc(sizeof(float) * c->sizes[0], c->mempool);
    c->tables[0] = c->baseTable;
    for (int t = 1; t < c->numTables; ++t)
    {
        c->sizes[t] = c->sizes[t-1] / 2 > 128 ? c->sizes[t-1] / 2 : 128;
        c->sizeMasks[t] = (c->sizes[t] - 1);
        c->tables[t] = (float*) mpool_alloc(sizeof(float) * c->sizes[t], c->mempool);
    }

    // Copy table
    for (int i = 0; i < c->sizes[0]; ++i)
    {
        c->baseTable[i] = table[i];
    }

    // Make bandlimited copies
    f = c->sampleRate * 0.25; //start at half nyquist
    // Not worth going over order 8 I think, and even 8 is only marginally better than 4.
    tButterworth_create(&c->mempool,&c->bl);
    tButterworth_init(leaf,c->bl, 8, -1.0f, f);
    tOversampler_create(&c->mempool,&c->ds);
    tOversampler_init(leaf,c->ds,2, 1);
    for (int t = 1; t < c->numTables; ++t)
    {
        // Size is going down; we need to downsample
        if (c->sizes[t] < c->sizes[t-1])
        {
            // Similar to tWaveTable, doing multiple passes here helps, but not sure what number is optimal
            for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
            {
                for (int i = 0; i < c->sizes[t]; ++i)
                {
                    c->dsBuffer[0] = tButterworth_tick(c->bl, c->tables[t-1][i*2]);
                    c->dsBuffer[1] = tButterworth_tick(c->bl, c->tables[t-1][(i*2)+1]);
                    c->tables[t][i] = tOversampler_downsample(c->ds, c->dsBuffer);
                }
            }
        }
        else
        {
            tButterworth_setF2(c->bl, f);
            for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
            {
                for (int i = 0; i < c->sizes[t]; ++i)
                {
                    c->tables[t][i] = tButterworth_tick(c->bl, c->tables[t-1][i]);
                }
            }
            f *= 0.5f; //halve the cutoff for next pass
        }
    }
    tOversampler_free(&c->ds);
    tButterworth_free(&c->bl);
}

void    tWaveTableS_free(tWaveTableS** const cy)
{
    tWaveTableS* c = *cy;

    mpool_free((char*)c->baseTable, c->mempool);
    for (int t = 1; t < c->numTables; ++t)
    {
        mpool_free((char*)c->tables[t], c->mempool);
    }
    mpool_free((char*)c->tables, c->mempool);
    mpool_free((char*)c->sizes, c->mempool);
    mpool_free((char*)c->sizeMasks, c->mempool);
    mpool_free((char*)c, c->mempool);
}

void    tWaveTableS_setSampleRate(tWaveTableS* const c, float sr)
{
    int size = c->sizes[0];

    for (int t = 1; t < c->numTables; ++t)
    {
        mpool_free((char*)c->tables[t], c->mempool);
    }
    mpool_free((char*)c->tables, c->mempool);
    mpool_free((char*)c->sizes, c->mempool);
    mpool_free((char*)c->sizeMasks, c->mempool);

    c->sampleRate = sr;

    // Determine base frequency
    c->baseFreq = c->sampleRate / (float) size;
    c->invBaseFreq = 1.0f / c->baseFreq;

    // Determine how many tables we need
    c->numTables = 2;
    float f = c->baseFreq;
    while (f < c->maxFreq)
    {
        c->numTables++;
        f *= 2.0f; // pass this multiplier in to set spacing of tables?
    }

    // Allocate memory for the tables
    c->tables = (float**) mpool_alloc(sizeof(float*) * c->numTables, c->mempool);
    c->sizes = (int*) mpool_alloc(sizeof(int) * c->numTables, c->mempool);
    c->sizeMasks = (int*) mpool_alloc(sizeof(int) * c->numTables, c->mempool);
    c->sizes[0] = size;
    c->sizeMasks[0] = (c->sizes[0] - 1);
    c->tables[0] = c->baseTable;
    for (int t = 1; t < c->numTables; ++t)
    {
        c->sizes[t] = c->sizes[t-1] / 2 > 128 ? c->sizes[t-1] / 2 : 128;
        c->sizeMasks[t] = (c->sizes[t] - 1);
        c->tables[t] = (float*) mpool_alloc(sizeof(float) * c->sizes[t], c->mempool);
    }

    // Make bandlimited copies
    f = c->sampleRate * 0.25; //start at half nyquist
    // Not worth going over order 8 I think, and even 8 is only marginally better than 4.
    tButterworth_create(&c->mempool,&c->bl);
    tButterworth_init(c->mempool->leaf,c->bl, 8, -1.0f, f);
    tOversampler_create(&c->mempool,&c->ds);
    tOversampler_init(c->mempool->leaf,c->ds,2, 1);
    for (int t = 1; t < c->numTables; ++t)
    {
        // Size is going down; we need to downsample
        if (c->sizes[t] < c->sizes[t-1])
        {
            // Similar to tWaveTable, doing multiple passes here helps, but not sure what number is optimal
            for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
            {
                for (int i = 0; i < c->sizes[t]; ++i)
                {
                    c->dsBuffer[0] = tButterworth_tick(c->bl, c->tables[t-1][i*2]);
                    c->dsBuffer[1] = tButterworth_tick(c->bl, c->tables[t-1][(i*2)+1]);
                    c->tables[t][i] = tOversampler_downsample(c->ds, c->dsBuffer);
                }
            }
        }
        else
        {
            tButterworth_setF2(c->bl, f);
            for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
            {
                for (int i = 0; i < c->sizes[t]; ++i)
                {
                    c->tables[t][i] = tButterworth_tick(c->bl, c->tables[t-1][i]);
                }
            }
            f *= 0.5f; //halve the cutoff for next pass
        }
    }
    tOversampler_free(&c->ds);
    tButterworth_free(&c->bl);
}

//================================================================================================
//================================================================================================

void tWaveOscS_create(tMempool** const mp, tWaveOscS** const cy)
{
    ALLOC_FROM_POOL(tWaveOscS, cy, mp);
}

void tWaveOscS_init(LEAF* const leaf, tWaveOscS* const c, tWaveTableS** tables, int numTables)
{

    c->tables = tables;
    c->numTables = numTables;

    c->index = 0.0f;
    c->o1 = 0;
    c->o2 = 1;
    c->mix = 0.0f;
    c->phase = 0;
    c->inc = 0;
    c->oct = 0;
    c->size = c->tables[0]->sizes[0];
    c->w = 0.0f;
    c->aa = 0.5f;
    c->sampleRate = leaf->sampleRate;
    // Determine base frequency
    c->baseFreq = c->sampleRate / (float) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;

    c->invSampleRateTimesTwoTo32 = leaf->invSampleRate * TWO_TO_32;
    c->maxFreq = c->tables[0]->maxFreq;

}

void tWaveOscS_free(tWaveOscS** const cy)
{
    tWaveOscS* c = *cy;

    mpool_free((char*)c, c->mempool);
}

volatile int errorCounter = 0;
float tWaveOscS_tick(tWaveOscS* const c)
{
    // Phasor increment (unsigned 32bit int wraps automatically with overflow so no need for if branch checks, as you need with float)
    c->phase += c->inc;
    float floatPhase = (double)c->phase * 2.32830643654e-10;
    float s1 = 0.f, s2 = 0.f;

    float temp;
    int idx;
    float frac;
    float samp0;
    float samp1;

    int oct = c->oct;

    float** tables = c->tables[c->o1]->tables;
    int* sizes = c->tables[c->o1]->sizes;
    int* sizeMasks = c->tables[c->o1]->sizeMasks;

    // Wavetable synthesis
    temp = sizes[oct] * floatPhase;
    idx = (int)temp;
    frac = temp - (float)idx;
    samp0 = tables[oct][idx];
    idx = (idx + 1) & sizeMasks[oct];
    samp1 = tables[oct][idx];

    float oct0 = (samp0 + (samp1 - samp0) * frac);

    temp = sizes[oct+1] * floatPhase;
    idx = (int)temp;
    frac = temp - (float)idx;
    samp0 = tables[oct+1][idx];
    idx = (idx + 1) & sizeMasks[oct+1];
    samp1 = tables[oct+1][idx];

    float oct1 = (samp0 + (samp1 - samp0) * frac);

    s1 = oct0 + (oct1 - oct0) * c->w;

    tables = c->tables[c->o2]->tables;
    sizes = c->tables[c->o2]->sizes;
    sizeMasks = c->tables[c->o2]->sizeMasks;

    // Wavetable synthesis
    temp = sizes[oct] * floatPhase;
    idx = (int)temp;
    frac = temp - (float)idx;
    samp0 = tables[oct][idx];
    idx = (idx + 1) & sizeMasks[oct];
    samp1 = tables[oct][idx];

    oct0 = (samp0 + (samp1 - samp0) * frac);

    temp = sizes[oct+1] * floatPhase;
    idx = (int)temp;
    frac = temp - (float)idx;
    samp0 = tables[oct+1][idx];
    idx = (idx + 1) & sizeMasks[oct+1];
    samp1 = tables[oct+1][idx];

    oct1 = (samp0 + (samp1 - samp0) * frac);

    s2 = oct0 + (oct1 - oct0) * c->w;

    // Ideally should determine correlation to get a good equal power fade between tables
    return s1 + (s2 - s1) * c->mix;
}

void tWaveOscS_setFreq(tWaveOscS* const c, float freq)
{
    c->freq  = freq;

    c->inc = c->freq * c->invSampleRateTimesTwoTo32;

    // abs for negative frequencies
    c->w = fabsf(c->freq * c->invBaseFreq);

    // Probably ok to use a log2 approx here; won't effect tuning at all, just crossfading between octave tables
    // I bet we could turn this into a lookup and save a lot of processing
    c->w = log2f_approx(c->w) + c->aa;//+ LEAF_SQRT2 - 1.0f; adding an offset here will shift our table selection upward, reducing aliasing but lower high freq fidelity. +1.0f should remove all aliasing
    if (c->w < 0.0f) c->w = 0.0f; // If c->w is < 0.0f, then freq is less than our base freq
    c->oct = (int)c->w;
    c->w -= c->oct;
    if (c->oct >= c->numSubTables - 1) c->oct = c->numSubTables - 2;
}

void tWaveOscS_setAntiAliasing(tWaveOscS* const c, float aa)
{
    c->aa = aa;
}

void tWaveOscS_setIndex(tWaveOscS* const c, float index)
{
    c->index = index;
    float f = c->index * (c->numTables - 1);

    c->o1 = (int)f;
    c->o2 = c->o1 + 1;
    if (c->index >= 1.0f) c->o2 = c->o1;
    c->mix = f - c->o1;
}

/*//// eventually gotta finish this so you can X/Y control the indices and fade between non-adjacent tables
void tWaveOscS_setIndexXY(tWaveOscS* const cy, float indexX, float indexY)
{
    _tWaveOscS* c = *cy;
    c->index = index;
    float f1 = c->index * (c->numTables - 1);

    c->o1 = (int)f1;
    c->o2 = c->o1 + 1;
    if (c->index >= 1.0f) c->o2 = c->o1;
    c->mix = f1 - c->o1;

    float f2 = c->index * (c->numTables - 1);
}
*/

void tWaveOscS_setSampleRate(tWaveOscS* const c, float sr)
{
    if (c->sampleRate == sr) return;

    c->sampleRate = sr;
    // Determine base frequency
    c->baseFreq = c->sampleRate / (float) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;
    c->invSampleRateTimesTwoTo32 = (1.f/c->sampleRate) * TWO_TO_32;

    tWaveOscS_setFreq(c, c->freq);
}
//
//void tWaveOscS_setIndexTable(tWaveOscS* const cy, int i, float* table, int size)
//{
//    _tWaveOscS* c = *cy;
//    if (i >= c->numTables) return;
//    tWaveTableS_free(&c->tables[i]);
//    tWaveTableS_initToPool(&c->tables[i], table, size, c->maxFreq, &c->mempool);
//}


//beep boop adding intphasro
// Cycle
void tIntPhasor_create(tMempool** const mp, tIntPhasor** const cy)
{
    ALLOC_FROM_POOL(tIntPhasor, cy, mp);
}

void tIntPhasor_init(LEAF* const leaf, tIntPhasor* const c)
{


    c->phase    =  0;
    c->inc  = 0;
    c->invSampleRateTimesTwoTo32 = (leaf->invSampleRate * TWO_TO_32);

}

void    tIntPhasor_free (tIntPhasor** const cy)
{
    tIntPhasor* c = *cy;

    mpool_free((char*)c, c->mempool);
}


float   tIntPhasor_tick(tIntPhasor* const c)
{
    // Phasor increment
    c->phase = (c->phase + c->inc);

    return c->phase * INV_TWO_TO_32;
}

float   tIntPhasor_tickBiPolar(tIntPhasor* const c)
{
    // Phasor increment
    c->phase = (c->phase + c->inc);

    return (c->phase * INV_TWO_TO_32 * 2.0f) - 1.0f;
}

void     tIntPhasor_setFreq(tIntPhasor* const c, float freq)
{
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tIntPhasor_setPhase(tIntPhasor* const c, float phase)
{
    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tIntPhasor_setSampleRate (tIntPhasor* const c, float sr)
{
    c->invSampleRateTimesTwoTo32 = (1.0f/sr) * TWO_TO_32;
    tIntPhasor_setFreq(c, c->freq);
}

//////SQUARE(PUSHER)LFO
void tSquareLFO_create(tMempool** const mp, tSquareLFO** const cy)
{
    ALLOC_FROM_POOL(tSquareLFO, cy, mp);
}

void tSquareLFO_init(LEAF* const leaf, tSquareLFO* const c)
{

tIntPhasor_init(leaf,&c->phasor);
    tIntPhasor_init(leaf,&c->invPhasor);
    tSquareLFO_setPulseWidth(c, 0.5f);

}

void    tSquareLFO_free (tSquareLFO** const cy)
{
    tSquareLFO* c = *cy;
    mpool_free((char*)c, c->mempool);
}

//need to check bounds and wrap table properly to allow through-zero FM
float   tSquareLFO_tick(tSquareLFO* const c)
{
    // Phasor increment
    float a = tIntPhasor_tick(&c->phasor);
    float b = tIntPhasor_tick(&c->invPhasor);
    float tmp = ((a - b)) + c->pulsewidth - 0.5f;
    return 2 * tmp;
}

void     tSquareLFO_setFreq(tSquareLFO* const c, float freq)
{
    tIntPhasor_setFreq(&c->phasor,freq);
    tIntPhasor_setFreq(&c->invPhasor,freq);
}



void     tSquareLFO_setSampleRate (tSquareLFO* const c, float sr)
{
    tIntPhasor_setSampleRate(&c->phasor, sr);
    tIntPhasor_setSampleRate(&c->invPhasor, sr);
}

void tSquareLFO_setPulseWidth(tSquareLFO* const c, float pw)
{
    c->pulsewidth = pw;
    tIntPhasor_setPhase(&c->invPhasor, c->pulsewidth + (c->phasor.phase * INV_TWO_TO_32));
}

void tSquareLFO_setPhase(tSquareLFO* const c, float phase)
{
    tIntPhasor_setPhase(&c->phasor, phase);
    tIntPhasor_setPhase(&c->invPhasor, c->pulsewidth + (c->phasor.phase * INV_TWO_TO_32));
}

void tSawSquareLFO_create(tMempool** const mp, tSawSquareLFO** const cy)
{
    ALLOC_FROM_POOL(tSawSquareLFO, cy, mp);
}

void tSawSquareLFO_init(LEAF* const leaf, tSawSquareLFO* const c)
{

tSquareLFO_init(leaf,&c->square);
    tIntPhasor_init(leaf,&c->saw);

}
void    tSawSquareLFO_free        (tSawSquareLFO** const cy)
{
    tSawSquareLFO* c = *cy;
    mpool_free((char*)c, c->mempool);
}

float   tSawSquareLFO_tick        (tSawSquareLFO* const c)
{
    float a = (tIntPhasor_tick(&c->saw) - 0.5f ) * 2.0f;
    float b = tSquareLFO_tick(&c->square);
    return  (1 - c->shape) * a + c->shape * b;
}
void    tSawSquareLFO_setFreq     (tSawSquareLFO* const c, float freq)
{
    tSquareLFO_setFreq(&c->square, freq);
    tIntPhasor_setFreq(&c->saw, freq);
}
void    tSawSquareLFO_setSampleRate (tSawSquareLFO* const c, float sr)
{
    tSquareLFO_setSampleRate(&c->square, sr);
    tIntPhasor_setSampleRate(&c->saw, sr);
}
void    tSawSquareLFO_setPhase (tSawSquareLFO* const c, float phase)
{
    tSquareLFO_setPhase(&c->square, phase);
    tIntPhasor_setPhase(&c->saw, phase);
}


void    tSawSquareLFO_setShape (tSawSquareLFO* const c, float shape)
{
    c->shape = shape;
}


///tri
void tTriLFO_create(tMempool** const mp, tTriLFO** const cy)
{
    ALLOC_FROM_POOL(tTriLFO, cy, mp);
}

void tTriLFO_init(LEAF* const leaf, tTriLFO* const c)
{


    c->inc      =  0;
    c->phase    =  0;
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = (c->invSampleRate * TWO_TO_32);
    tTriLFO_setFreq(c, 220.0f);

}

void    tTriLFO_free(tTriLFO** const cy)
{
    tTriLFO* c = *cy;

    mpool_free((char*)c, c->mempool);
}

//need to check bounds and wrap table properly to allow through-zero FM
float   tTriLFO_tick(tTriLFO* const c)
{
    c->phase += c->inc;

    //bitmask fun
    int32_t shiftedPhase = c->phase + 1073741824; // offset by 1/4" wave by adding 2^30 to get things in phase with the other LFO oscillators
    uint32_t mask = shiftedPhase >> 31; //get the sign bit
    shiftedPhase = shiftedPhase + mask; // add 1 if negative, zero if positive, to balance
    shiftedPhase = shiftedPhase ^ mask; //invert the value to get absolute value of integer
    float output = (((float)shiftedPhase * INV_TWO_TO_31) - 0.5f) * 2.0f; //scale it to -1.0f to 1.0f float
    return output;

}

void     tTriLFO_setFreq(tTriLFO* const c, float freq)
{
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tTriLFO_setPhase(tTriLFO* const c, float phase)
{
    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32_INT;
}

void     tTriLFO_setSampleRate (tTriLFO* const c, float sr)
{
    c->invSampleRate = (1.0f/sr);
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tTriLFO_setFreq(c, c->freq);
}
///sinetri

void tSineTriLFO_create(tMempool** const mp, tSineTriLFO** const cy)
{
    ALLOC_FROM_POOL(tSineTriLFO, cy, mp);
}

void tSineTriLFO_init(LEAF* const leaf, tSineTriLFO* const c)
{

tTriLFO_init(leaf,&c->tri);
    tCycle_init(leaf,&c->sine);


}
void    tSineTriLFO_free        (tSineTriLFO** const cy)
{
    tSineTriLFO* c = *cy;
    mpool_free((char*)c, c->mempool);
}

float   tSineTriLFO_tick        (tSineTriLFO* const c)
{
    float a = tCycle_tick(&c->sine);
    float b = tTriLFO_tick(&c->tri);
    return  (1.0f - c->shape) * a + c->shape * b;
}
void    tSineTriLFO_setFreq     (tSineTriLFO* const c, float freq)
{
    tTriLFO_setFreq(&c->tri, freq);
    tCycle_setFreq(&c->sine, freq);
}
void    tSineTriLFO_setSampleRate (tSineTriLFO* const c, float sr)
{
    tTriLFO_setSampleRate(&c->tri, sr);
    tCycle_setSampleRate(&c->sine, sr);
}
void    tSineTriLFO_setPhase (tSineTriLFO* const c, float phase)
{
    tTriLFO_setPhase(&c->tri, phase);
    tCycle_setPhase(&c->sine, phase);
}

 void    tSineTriLFO_setShape (tSineTriLFO* const c, float shape)
 {
    c->shape = shape;

 }




 void tDampedOscillator_create(tMempool** const mp, tDampedOscillator** const cy)
{
    ALLOC_FROM_POOL(tDampedOscillator, cy, mp);
}




 void tDampedOscillator_init(LEAF* const leaf, tDampedOscillator* const c)
{



     c->freq_ = 0.0f;
     c->decay_ = 1.0f;

     tDampedOscillator_setSampleRate(c, leaf->sampleRate);
     c->loop_gain_ = cosf(c->freq_ * c->two_pi_by_sample_rate_);
     	  float g = sqrtf((1.0f - c->loop_gain_) / (1.0f + c->loop_gain_));
    c->turns_ratio_ = g;
    tDampedOscillator_reset(c);


}
 void    tDampedOscillator_free        (tDampedOscillator** const cy)
 {
	 tDampedOscillator* c = *cy;

     mpool_free((char*)c, c->mempool);
 }

 float   tDampedOscillator_tick        (tDampedOscillator* const c)
 {
	   float w = c->decay_ * c->x_;
	   float z = c->loop_gain_ * (c->y_ + w);
	   c->x_ = z - c->y_;
	   c->y_ = z + w;
	   return c->y_;
 }
 void    tDampedOscillator_setFreq     (tDampedOscillator* const c, float freq_hz)
 {
	  c->freq_ = freq_hz;

#ifdef ARM_MATH_CM7
	  c->loop_gain_ = arm_cos_f32(freq_hz * c->two_pi_by_sample_rate_);
	  float g = 1.0f;
	  arm_sqrt_f32((1.0f - c->loop_gain_) / (1.0f + c->loop_gain_), &g);
#else
	  c->loop_gain_ = cosf(freq_hz * c->two_pi_by_sample_rate_);
	  float g = sqrtf((1.0f - c->loop_gain_) / (1.0f + c->loop_gain_));
#endif
	  // scale state variable in preparation for the next step
	  c->x_ *= g / c->turns_ratio_;
	  c->turns_ratio_ = g;

 }

 void    tDampedOscillator_setDecay    (tDampedOscillator* const c, float decay)
 {

	 float r = fastExp4(-decay * c->two_pi_by_sample_rate_);


	 c->decay_ = r * r;

 }

 void    tDampedOscillator_setSampleRate (tDampedOscillator* const c, float sr)
 {
	 c->two_pi_by_sample_rate_ = TWO_PI / sr;
 }


  void    tDampedOscillator_reset (tDampedOscillator* const c)
  {
	  c->x_ = c->turns_ratio_;
	  c->y_ = 0.0f;
  }


void tPlutaQuadOsc_create(tMempool** const mp, tPlutaQuadOsc** const cy)
{
    ALLOC_FROM_POOL(tPlutaQuadOsc, cy, mp);
}

void tPlutaQuadOsc_init(LEAF* const leaf, tPlutaQuadOsc* const c, uint32_t const oversamplingRatio)
{

    c->oversamplingRatio = oversamplingRatio;
    for (int i = 0; i < 4; i++)
    {
        c->inc[i]      =  0;
        c->phase[i]    =  0;
        c->biPolarOutputs[i] = 0.0f;
        c->freq[i] = 0.0f;
        for (int j = 0; j < 4; j++)
        {
            c->fmMatrix[i][j] = 0.0f;

}
        c->outputAmplitudes[i] = 1.0f;
    }
    float oversampledSamplingRate = (leaf->sampleRate * c->oversamplingRatio);

    float nyquistFreq = leaf->sampleRate * 0.47f; //nyquist of main leaf sample rate (filter will drop it down to this)

    //set up the lowpass for the decimation.
    //butterworth lowpass - would be better to create a butterworth object that is lowpass only and uses tSVFtickLP for efficiency
    tButterworth_create (&c->mempool,&c->lowpass);
    tButterworth_init(leaf,c->lowpass, 8, 0.0f, nyquistFreq);
    //correct samplerate to take into account oversampling
    tButterworth_setSampleRate (c->lowpass, oversampledSamplingRate);
    //now reset the frequencies with new samplerate
    tButterworth_setF2 (c->lowpass, nyquistFreq);

    float invSampleRate = 1.0 / oversampledSamplingRate;
    c->invSampleRateTimesTwoTo32 = (invSampleRate * TWO_TO_32);
    for (int i = 0; i < 4; i++)
    {
        tPlutaQuadOsc_setFreq(c, i, 220.0f);
    }
}

float   tPlutaQuadOsc_tick        (tPlutaQuadOsc* const c)
{
    float outputSample = 0.0f;
    for (int i = 0; i < c->oversamplingRatio; i++)
    {
        float currentSample = 0.0f;
        for (int j =0; j < 4; j++)
        {
            //apply freq modulation
            float freqModSum = c->biPolarOutputs[0] * c->fmMatrix[i][0] + c->biPolarOutputs[1] * c->fmMatrix[i][1]+ c->biPolarOutputs[2] * c->fmMatrix[i][2] + c->biPolarOutputs[3] * c->fmMatrix[i][3];

            uint32_t tempInc = c->inc[i] + (uint32_t)(freqModSum * c->invSampleRateTimesTwoTo32);
            //increment oscillator
            c->phase[j] += tempInc;

            //get output and add to mix
            // this version is sawtooth, could be sine or triangle or square too
            float out = (c->phase[j] * INV_TWO_TO_32 * 2.0f) - 1.0f;
            c->biPolarOutputs[i] = out;
            currentSample += out * c->outputAmplitudes[i];
        }
        outputSample = tButterworth_tick(c->lowpass, currentSample); //lowpass before decimation
    }
    //only last sample of the oversampled buffer gets used (decimation step)
    return outputSample * 0.249f;
}

void   tPlutaQuadOsc_setFreq        (tPlutaQuadOsc* const c, uint32_t const whichOsc, float const freq)
{
    c->freq[whichOsc]  = freq;
    c->inc[whichOsc] = (uint32_t)(freq * c->invSampleRateTimesTwoTo32);
}

void   tPlutaQuadOsc_setFmAmount        (tPlutaQuadOsc* const c, uint32_t const whichCarrier, uint32_t const whichModulator, float const amount)
{
    c->fmMatrix[whichCarrier][whichModulator] = amount;
}

void   tPlutaQuadOsc_setOutputAmplitude        (tPlutaQuadOsc* const c, uint32_t const whichOsc, float const amplitude)
{
    c->outputAmplitudes[whichOsc] = amplitude;
}
void tPlutaQuadOsc_free(tPlutaQuadOsc** osc)
{
    tPlutaQuadOsc* c = *osc;
    mpool_free((char*)c->lowpass,c->mempool);
    mpool_free((char*)c, c->mempool);
}
