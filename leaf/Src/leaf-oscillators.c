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

inline float blamp0(float x) {
  return 1.0f / 3.0f * x * x * x;
}

inline float blamp1(float x) {
  x = x - 1.0f;
  return -1.0f / 3.0f * x * x * x;
}

//PolyBLEP Waveform generator ported from the Jesusonic code by Tale
//http://www.taletn.com/reaper/mono_synth/
//from Martin Finke's githubb polyblep project
// Derived from blep().
inline float blamp(float t, float dt) {
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
void    tCycle_init(tCycle* const cy, LEAF* const leaf)
{
    tCycle_initToPool(cy, &leaf->mempool);
}

void    tCycle_initToPool   (tCycle* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tCycle* c = *cy = (_tCycle*) mpool_alloc(sizeof(_tCycle), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->inc      =  0;
    c->phase    =  0;
    c->invSampleRateTimesTwoTo32 = (leaf->invSampleRate * TWO_TO_32);
    c->mask = SINE_TABLE_SIZE - 1;
}

void    tCycle_free (tCycle* const cy)
{
    _tCycle* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

//need to check bounds and wrap table properly to allow through-zero FM
Lfloat   tCycle_tick(tCycle* const cy)
{
    _tCycle* c = *cy;
    uint32_t tempFrac;
    uint32_t idx;
    Lfloat samp0;
    Lfloat samp1;
    
    // Phasor increment
    c->phase += c->inc;
    // Wavetable synthesis
    idx = c->phase >> 21; //11 bit table 
    tempFrac = (c->phase & 2097151); //(2^21 - 1) all the lower bits i.e. the remainder of a division by 2^21  (2097151 is the 21 bits after the 11 bits that represent the main index) 
    
    samp0 = __leaf_table_sinewave[idx];
    idx = (idx + 1) & c->mask;
    samp1 = __leaf_table_sinewave[idx];
    
    return (samp0 + (samp1 - samp0) * ((Lfloat)tempFrac * 0.000000476837386f)); // 1/2097151 
}

void     tCycle_setFreq(tCycle* const cy, Lfloat freq)
{
    _tCycle* c = *cy;
    
    //if (!isfinite(freq)) return;
    
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tCycle_setPhase(tCycle* const cy, Lfloat phase)
{
    _tCycle* c = *cy;
    
    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tCycle_setSampleRate (tCycle* const cy, Lfloat sr)
{
    _tCycle* c = *cy;
    
    c->invSampleRateTimesTwoTo32 = (1.0f/sr) * TWO_TO_32;
    tCycle_setFreq(cy, c->freq);
}
#endif // LEAF_INCLUDE_SINE_TABLE

#if LEAF_INCLUDE_TRIANGLE_TABLE
//========================================================================
/* Triangle */
void   tTriangle_init(tTriangle* const cy, LEAF* const leaf)
{
    tTriangle_initToPool(cy, &leaf->mempool);
}

void    tTriangle_initToPool    (tTriangle* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTriangle* c = *cy = (_tTriangle*) mpool_alloc(sizeof(_tTriangle), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->inc      =  0;
    c->phase    =  0;
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = (c->invSampleRate * TWO_TO_32);
    c->mask = TRI_TABLE_SIZE - 1;
    tTriangle_setFreq(cy, 220);
}

void    tTriangle_free  (tTriangle* const cy)
{
    _tTriangle* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

Lfloat   tTriangle_tick(tTriangle* const cy)
{
    _tTriangle* c = *cy;
    
    uint32_t idx;
    Lfloat frac;
    Lfloat samp0;
    Lfloat samp1;
    
    // Phasor increment
    c->phase += c->inc;
    // Wavetable synthesis
    idx = c->phase >> 21;
    uint32_t idx2 = (idx + 1) & c->mask;
    uint32_t tempFrac = (c->phase & 2097151);
    frac = (Lfloat)tempFrac * 0.000000476837386f;// 1/2097151 (2097151 is the 21 bits after the 11 bits that represent the main index)
    
    samp0 = __leaf_table_triangle[c->oct][idx];
    samp1 = __leaf_table_triangle[c->oct][idx2];
    Lfloat oct0 = (samp0 + (samp1 - samp0) * frac);
    
    samp0 = __leaf_table_triangle[c->oct+1][idx];
    samp1 = __leaf_table_triangle[c->oct+1][idx2];
    Lfloat oct1 = (samp0 + (samp1 - samp0) * frac);
    
    return oct0 + (oct1 - oct0) * c->w;
}

void tTriangle_setFreq(tTriangle* const cy, Lfloat freq)
{
    _tTriangle* c = *cy;
    
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

void tTriangle_setPhase(tTriangle* const cy, Lfloat phase)
{
    _tTriangle* c = *cy;
    
    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tTriangle_setSampleRate (tTriangle* const cy, Lfloat sr)
{
    _tTriangle* c = *cy;
    
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tTriangle_setFreq(cy, c->freq);
}
#endif // LEAF_INCLUDE_TRIANGLE_TABLE

#if LEAF_INCLUDE_SQUARE_TABLE
//========================================================================
/* Square */
void   tSquare_init(tSquare* const cy, LEAF* const leaf)
{
    tSquare_initToPool(cy, &leaf->mempool);
}

void    tSquare_initToPool  (tSquare* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSquare* c = *cy = (_tSquare*) mpool_alloc(sizeof(_tSquare), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->inc      =  0;
    c->phase    =  0;
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = (c->invSampleRate * TWO_TO_32);
    c->mask = SQR_TABLE_SIZE - 1;
    tSquare_setFreq(cy, 220);
}

void    tSquare_free (tSquare* const cy)
{
    _tSquare* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

Lfloat   tSquare_tick(tSquare* const cy)
{
    _tSquare* c = *cy;
    
    uint32_t idx;
    Lfloat frac;
    Lfloat samp0;
    Lfloat samp1;
    
    // Phasor increment
    c->phase += c->inc;
    // Wavetable synthesis
    idx = c->phase >> 21;
    uint32_t idx2 = (idx + 1) & c->mask;
    uint32_t tempFrac = (c->phase & 2097151);
    frac = (Lfloat)tempFrac * 0.000000476837386f;// 1/2097151 (2097151 is the 21 bits after the 11 bits that represent the main index)
    
    samp0 = __leaf_table_squarewave[c->oct][idx];
    samp1 = __leaf_table_squarewave[c->oct][idx2];
    Lfloat oct0 = (samp0 + (samp1 - samp0) * frac);
    
    samp0 = __leaf_table_squarewave[c->oct+1][idx];
    samp1 = __leaf_table_squarewave[c->oct+1][idx2];
    Lfloat oct1 = (samp0 + (samp1 - samp0) * frac);
    
    return oct0 + (oct1 - oct0) * c->w;
}

void    tSquare_setFreq(tSquare* const cy, Lfloat freq)
{
    _tSquare* c = *cy;
    
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

void    tSquare_setPhase(tSquare* const cy, Lfloat phase)
{
    _tSquare* c = *cy;
    
    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tSquare_setSampleRate (tSquare* const cy, Lfloat sr)
{
    _tSquare* c = *cy;
    
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tSquare_setFreq(cy, c->freq);
}
#endif // LEAF_INCLUDE_SQUARE_TABLE

#if LEAF_INCLUDE_SAWTOOTH_TABLE
//=====================================================================
// Sawtooth
void    tSawtooth_init(tSawtooth* const cy, LEAF* const leaf)
{
    tSawtooth_initToPool(cy, &leaf->mempool);
}

void    tSawtooth_initToPool    (tSawtooth* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSawtooth* c = *cy = (_tSawtooth*) mpool_alloc(sizeof(_tSawtooth), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->inc      = 0;
    c->phase    = 0;
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = (c->invSampleRate * TWO_TO_32);
    c->mask = SAW_TABLE_SIZE - 1;
    tSawtooth_setFreq(cy, 220);
}

void    tSawtooth_free (tSawtooth* const cy)
{
    _tSawtooth* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

Lfloat   tSawtooth_tick(tSawtooth* const cy)
{
    _tSawtooth* c = *cy;
    
    uint32_t idx;
    Lfloat frac;
    Lfloat samp0;
    Lfloat samp1;
    
    // Phasor increment
    c->phase += c->inc;
    // Wavetable synthesis
    idx = c->phase >> 21;
    uint32_t idx2 = (idx + 1) & c->mask;
    uint32_t tempFrac = (c->phase & 2097151);
    frac = (Lfloat)tempFrac * 0.000000476837386f; // 1/2097151 (2097151 is the 21 bits after the 11 bits that represent the main index)
    
    samp0 = __leaf_table_sawtooth[c->oct][idx];
    samp1 = __leaf_table_sawtooth[c->oct][idx2];
    Lfloat oct0 = (samp0 + (samp1 - samp0) * frac);
    
    samp0 = __leaf_table_sawtooth[c->oct+1][idx];
    samp1 = __leaf_table_sawtooth[c->oct+1][idx2];
    Lfloat oct1 = (samp0 + (samp1 - samp0) * frac);
    
    
    return oct0 + (oct1 - oct0) * c->w;
}

void    tSawtooth_setFreq(tSawtooth* const cy, Lfloat freq)
{
    _tSawtooth* c = *cy;
    
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

void tSawtooth_setPhase(tSawtooth* const cy, Lfloat phase)
{
    _tSawtooth* c = *cy;
    
    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tSawtooth_setSampleRate (tSawtooth* const cy, Lfloat sr)
{
    _tSawtooth* c = *cy;
    
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tSawtooth_setFreq(cy, c->freq);
}

#endif // LEAF_INCLUDE_SAWTOOTH_TABLE

//==============================================================================

/* tTri: Anti-aliased Triangle waveform. */
void    tPBTriangle_init          (tPBTriangle* const osc, LEAF* const leaf)
{
    tPBTriangle_initToPool(osc, &leaf->mempool);
}

void    tPBTriangle_initToPool    (tPBTriangle* const osc, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPBTriangle* c = *osc = (_tPBTriangle*) mpool_alloc(sizeof(_tPBTriangle), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;

    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->width     =  (0.5f * TWO_TO_32);
    c->oneMinusWidth =  c->width;
    c->freq = 0.0f;
}

void    tPBTriangle_free (tPBTriangle* const cy)
{
    _tPBTriangle* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

#ifdef ITCMRAM
    Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBTriangle_tick          (tPBTriangle* const osc)
#else
Lfloat   tPBTriangle_tick          (tPBTriangle* const osc)
#endif

{
    _tPBTriangle* c = *osc;

    uint32_t halfWidth =(c->width >> 1);
    Lfloat floatWidth = c->width * INV_TWO_TO_32;
    uint32_t t1 = c->phase + halfWidth;

    uint32_t t2 = c->phase + (4294967296u - halfWidth);
    
    Lfloat t1F = t1 * INV_TWO_TO_32;
    Lfloat t2F = t2 * INV_TWO_TO_32;
    Lfloat t = c->phase * INV_TWO_TO_32;
    Lfloat incFloat = c->inc * INV_TWO_TO_32;
    
    float y = t * 2.0f;

    if (y >= 2.0f - floatWidth) {
        y = (y - 2.0f) / floatWidth;
    } else if (y >= floatWidth) {
        y = 1.0f - (y - floatWidth) / (1.0f - floatWidth);
    } else {
        y /= floatWidth;
    }
    Lfloat blampOne = blamp(t1F, incFloat);
    Lfloat blampTwo = blamp(t2F, incFloat);
    Lfloat scaling = incFloat / (floatWidth - floatWidth * floatWidth) ;
    y += scaling * (blampOne - blampTwo);
    c->phase += c->inc;
    return y;
}

#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBTriangle_setFreq       (tPBTriangle* const osc, Lfloat freq)
#else
void    tPBTriangle_setFreq       (tPBTriangle* const osc, Lfloat freq)
#endif
{
    _tPBTriangle* c = *osc;
    
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tPBTriangle_setSkew       (tPBTriangle* const osc, Lfloat width)
{
    _tPBTriangle* c = *osc;
    width = width*0.5f + 0.5f;
    width = LEAF_clip(0.01f, width, 0.99f);
    c->oneMinusWidth = (1.0f - width) * TWO_TO_32;
    c->width = width * TWO_TO_32;
}

void     tPBTriangle_setSampleRate (tPBTriangle* const osc, Lfloat sr)
{
    _tPBTriangle* c = *osc;
    
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBTriangle_setFreq(osc, c->freq);
}


//==============================================================================

/* tPBSineTriangle: Anti-aliased Triangle waveform with sine wave, crossfaded between them with shape param. */
void    tPBSineTriangle_init          (tPBSineTriangle* const osc, LEAF* const leaf)
{
	tPBSineTriangle_initToPool(osc, &leaf->mempool);
}

void    tPBSineTriangle_initToPool    (tPBSineTriangle* const osc, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPBSineTriangle* c = *osc = (_tPBSineTriangle*) mpool_alloc(sizeof(_tPBSineTriangle), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    tCycle_initToPool(&c->sine, mp);
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->freq = 0.0f;
    c->shape     =  0.0f;
    c->oneMinusShape = 1.0f;
}

void    tPBSineTriangle_free (tPBSineTriangle* const cy)
{
    _tPBSineTriangle* c = *cy;
    tCycle_free(&c->sine);
    mpool_free((char*)c, c->mempool);
}

#ifdef ITCMRAM
    Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSineTriangle_tick          (tPBSineTriangle* const osc)
#else
Lfloat   tPBSineTriangle_tick          (tPBSineTriangle* const osc)
#endif
{
    _tPBSineTriangle* c = *osc;

    uint32_t t1 = c->phase + TWO_TO_32_ONE_QUARTER;

    uint32_t t2 = c->phase + TWO_TO_32_THREE_QUARTERS;
    
    Lfloat t1F = t1 * INV_TWO_TO_32;
    Lfloat t2F = t2 * INV_TWO_TO_32;
    Lfloat t = c->phase * INV_TWO_TO_32;
    Lfloat incFloat = c->inc * INV_TWO_TO_32;
    
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
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSineTriangle_setFreq       (tPBSineTriangle* const osc, Lfloat freq)
#else
void    tPBSineTriangle_setFreq       (tPBSineTriangle* const osc, Lfloat freq)
#endif
{
    _tPBSineTriangle* c = *osc;
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
    tCycle_setFreq(&c->sine, freq);
}

void    tPBSineTriangle_setShape       (tPBSineTriangle* const osc, Lfloat shape)
{
    _tPBSineTriangle* c = *osc;
    c->shape = -1.0f * shape; // inverted because triangle output is inverted
    c->oneMinusShape = 1.0f - shape;
}

void    tPBSineTriangle_setSampleRate (tPBSineTriangle* const osc, Lfloat sr)
{
    _tPBSineTriangle* c = *osc;

    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBSineTriangle_setFreq(osc, c->freq);
}
//==============================================================================

/* tPulse: Anti-aliased pulse waveform. */
void    tPBPulse_init        (tPBPulse* const osc, LEAF* const leaf)
{
    tPBPulse_initToPool(osc, &leaf->mempool);
}

void    tPBPulse_initToPool  (tPBPulse* const osc, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPBPulse* c = *osc = (_tPBPulse*) mpool_alloc(sizeof(_tPBPulse), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->width     =  (0.5f * TWO_TO_32);
    c->oneMinusWidth =  c->width;
    c->freq = 0.0f;
}

void    tPBPulse_free (tPBPulse* const osc)
{
    _tPBPulse* c = *osc;
    
    mpool_free((char*)c, c->mempool);
}

#ifdef ITCMRAM
    Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBPulse_tick        (tPBPulse* const osc)
#else
Lfloat   tPBPulse_tick        (tPBPulse* const osc)
#endif
{
    _tPBPulse* c = *osc;
    
    Lfloat phaseFloat = c->phase *  INV_TWO_TO_32;
    Lfloat incFloat = c->inc *  INV_TWO_TO_32;
    Lfloat backwardsPhaseFloat = (c->phase + c->oneMinusWidth) * INV_TWO_TO_32;
    Lfloat widthFloat =c->width *INV_TWO_TO_32;
    Lfloat out = -2.0f * widthFloat;
    if (phaseFloat < widthFloat) {
        out += 2.0f;
    }
    out += LEAF_poly_blep(phaseFloat,incFloat);
    out -= LEAF_poly_blep(backwardsPhaseFloat, incFloat);
    c->phase += c->inc;
    return out;
    
}

#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBPulse_setFreq     (tPBPulse* const osc, Lfloat freq)
#else
void    tPBPulse_setFreq     (tPBPulse* const osc, Lfloat freq)
#endif
{
    _tPBPulse* c = *osc;
    
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tPBPulse_setWidth    (tPBPulse* const osc, Lfloat width)
{
    _tPBPulse* c = *osc;
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

void    tPBPulse_setSampleRate (tPBPulse* const osc, Lfloat sr)
{
    _tPBPulse* c = *osc;
    
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBPulse_setFreq(osc, c->freq);
}

//==============================================================================

/* tSawtooth: Anti-aliased Sawtooth waveform. */
void    tPBSaw_init          (tPBSaw* const osc, LEAF* const leaf)
{
    tPBSaw_initToPool(osc, &leaf->mempool);
}

void    tPBSaw_initToPool    (tPBSaw* const osc, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPBSaw* c = *osc = (_tPBSaw*) mpool_alloc(sizeof(_tPBSaw), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->freq = 0.0f;
}

void    tPBSaw_free  (tPBSaw* const osc)
{
    _tPBSaw* c = *osc;
    
    mpool_free((char*)c, c->mempool);
}

#ifdef ITCMRAM
    Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSaw_tick          (tPBSaw* const osc)
#else
Lfloat   tPBSaw_tick          (tPBSaw* const osc)
#endif
{
    _tPBSaw* c = *osc;
    Lfloat out = (c->phase * INV_TWO_TO_31) - 1.0f;

    Lfloat phaseFloat = c->phase * INV_TWO_TO_32;
    Lfloat incFloat = c->inc * INV_TWO_TO_32;
    out -= LEAF_poly_blep(phaseFloat, incFloat);
    c->phase += c->inc;
    return (-1.0f * out);
}

#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tPBSaw_setFreq       (tPBSaw* const osc, Lfloat freq)
#else
void    tPBSaw_setFreq       (tPBSaw* const osc, Lfloat freq)
#endif
{
    _tPBSaw* c = *osc;
    
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tPBSaw_setSampleRate (tPBSaw* const osc, Lfloat sr)
{
    _tPBSaw* c = *osc;
    
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBSaw_setFreq(osc, c->freq);
}

//========================================================================


/* tSawtooth: Anti-aliased Sawtooth waveform. */
void    tPBSawSquare_init          (tPBSawSquare* const osc, LEAF* const leaf)
{
    tPBSawSquare_initToPool(osc, &leaf->mempool);
}

void    tPBSawSquare_initToPool    (tPBSawSquare* const osc, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPBSawSquare* c = *osc = (_tPBSawSquare*) mpool_alloc(sizeof(_tPBSawSquare), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    c->inc      =  0;
    c->phase    =  0;
    c->freq = 0.0f;

}

void    tPBSawSquare_free  (tPBSawSquare* const osc)
{
    _tPBSawSquare* c = *osc;
    
    mpool_free((char*)c, c->mempool);
}


#ifdef ITCMRAM
Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tPBSawSquare_tick          (tPBSawSquare* const osc)
#else
Lfloat   tPBSawSquare_tick          (tPBSawSquare* const osc)
#endif
{
    _tPBSawSquare* c = *osc;

    //Lfloat squareOut = ((c->phase < 2147483648u) * 2.0f) - 1.0f;
    Lfloat sawOut = (c->phase * INV_TWO_TO_32 * 2.0f) - 1.0f;
    Lfloat phaseFloat = c->phase * INV_TWO_TO_32;
    Lfloat incFloat = c->inc * INV_TWO_TO_32;
    Lfloat backwardsPhaseFloat = (c->phase + 2147483648u) * INV_TWO_TO_32;
    Lfloat resetBlep = LEAF_poly_blep(phaseFloat,incFloat);
    Lfloat midBlep = LEAF_poly_blep(backwardsPhaseFloat, incFloat);
    
    Lfloat squareOut = -1.0f;
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
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tPBSawSquare_setFreq       (tPBSawSquare* const osc, Lfloat freq)
#else
void    tPBSawSquare_setFreq       (tPBSawSquare* const osc, Lfloat freq)
#endif
{
    _tPBSawSquare* c = *osc;
    
    c->freq  = freq;
    c->inc = (freq * c->invSampleRateTimesTwoTo32);

}

void    tPBSawSquare_setShape      (tPBSawSquare* const osc, Lfloat inputShape)
{
    _tPBSawSquare* c = *osc;
    
    c->shape  = inputShape;
    c->oneMinusShape = 1.0f - inputShape;
}

void    tPBSawSquare_setSampleRate (tPBSawSquare* const osc, Lfloat sr)
{
    _tPBSawSquare* c = *osc;
    
    c->invSampleRate = 1.0f/sr;
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tPBSawSquare_setFreq(osc, c->freq);
}

//========================================================================


//==============================================================================

/* tSawtooth: Anti-aliased Sawtooth waveform. */
void    tSawOS_init          (tSawOS* const osc, uint8_t OS_ratio, uint8_t filterOrder, LEAF* const leaf)
{
	tSawOS_initToPool(osc, OS_ratio, filterOrder, &leaf->mempool);
}

void    tSawOS_initToPool    (tSawOS* const osc, uint8_t OS_ratio, uint8_t filterOrder, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSawOS* c = *osc = (_tSawOS*) mpool_alloc(sizeof(_tSawOS), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    c->OSratio = OS_ratio;
    c->inc      = 0;
    c->phase    = 0;
    c->invSampleRateOS = 1.0f / (leaf->sampleRate * OS_ratio);
    c->invSampleRateTimesTwoTo32OS = (c->invSampleRateOS * TWO_TO_32);
    c->filterOrder = filterOrder;
    c->aaFilter = (tSVF*) mpool_alloc(sizeof(tSVF) * filterOrder, m);
    
    for (int i = 0; i < filterOrder; i++)
    {
        Lfloat Qval = 0.5f/cosf((1.0f+2.0f*i)*PI/(4*filterOrder));

		tSVF_initToPool(&c->aaFilter[i], SVFTypeLowpass, (19000.0f / OS_ratio), Qval, mp);
    }
    tSawOS_setFreq(osc, 220.0f);
}

void    tSawOS_free  (tSawOS* const osc)
{
    _tSawOS* c = *osc;
    for (int i = 0; i < c->filterOrder; ++i) tSVF_free(&c->aaFilter[i]);
    mpool_free((char*)c->aaFilter, c->mempool);
    mpool_free((char*)c, c->mempool);
}

Lfloat   tSawOS_tick          (tSawOS* const osc)
{
    _tSawOS* c = *osc;

    Lfloat tempFloat = 0.0f;
    for (int i = 0; i < c->OSratio; i++)
    {
    	c->phase = (c->phase + c->inc);
        tempFloat = (c->phase * INV_TWO_TO_16)- 1.0f; // inv 2 to 32, then multiplied by 2, same as inv 2 to 16
    	for (int k = 0; k < c->filterOrder; k++)
    	{
    		tempFloat = tSVF_tick(&c->aaFilter[k], tempFloat);
    	}
    }
    return tempFloat;
}

void    tSawOS_setFreq       (tSawOS* const osc, Lfloat freq)
{
    _tSawOS* c = *osc;

    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32OS;
}

void    tSawOS_setSampleRate (tSawOS* const osc, Lfloat sr)
{
    _tSawOS* c = *osc;

    c->invSampleRateOS = 1.0f/(sr * c->OSratio);
    tSawOS_setFreq(osc, c->freq);
}

//========================================================================

/* Phasor */

void    tPhasor_init(tPhasor* const ph, LEAF* const leaf)
{
    tPhasor_initToPool(ph, &leaf->mempool);
}

void    tPhasor_initToPool  (tPhasor* const ph, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPhasor* p = *ph = (_tPhasor*) mpool_alloc(sizeof(_tPhasor), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;
    
    p->phase = 0;
    p->inc = 0;
    p->invSampleRate = leaf->invSampleRate;
    p->invSampleRateTimesTwoTo32 = p->invSampleRate * TWO_TO_32;
}

void    tPhasor_free (tPhasor* const ph)
{
    _tPhasor* p = *ph;
    
    mpool_free((char*)p, p->mempool);
}

void    tPhasor_setFreq(tPhasor* const ph, Lfloat freq)
{
    _tPhasor* p = *ph;

    p->freq  = freq;
    p->inc = freq * p->invSampleRateTimesTwoTo32;
}

Lfloat   tPhasor_tick(tPhasor* const ph)
{
    _tPhasor* p = *ph;
    
    p->phase += p->inc; // no need to phase wrap, since integer overflow does it for us
    return p->phase * INV_TWO_TO_32; //smush back to 0.0-1.0 range
}

void     tPhasor_setSampleRate (tPhasor* const ph, Lfloat sr)
{
    _tPhasor* p = *ph;
    
    p->invSampleRate = 1.0f/sr;
    p->invSampleRateTimesTwoTo32 = p->invSampleRate * TWO_TO_32;
    tPhasor_setFreq(ph, p->freq);
};

//========================================================================
/* Noise */
void    tNoise_init(tNoise* const ns, NoiseType type, LEAF* const leaf)
{
    tNoise_initToPool(ns, type, &leaf->mempool);
}

void    tNoise_initToPool   (tNoise* const ns, NoiseType type, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNoise* n = *ns = (_tNoise*) mpool_alloc(sizeof(_tNoise), m);
    n->mempool = m;
    LEAF* leaf = n->mempool->leaf;
    
    n->type = type;
    n->rand = leaf->random;
}

void    tNoise_free (tNoise* const ns)
{
    _tNoise* n = *ns;
    
    mpool_free((char*)n, n->mempool);
}

Lfloat   tNoise_tick(tNoise* const ns)
{
    _tNoise* n = *ns;
    
    Lfloat rand = (n->rand() * 2.0f) - 1.0f;
    
    if (n->type == PinkNoise)
    {
        Lfloat tmp;
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
/* Neuron */

void tNeuron_init(tNeuron* const nr, LEAF* const leaf)
{
    tNeuron_initToPool(nr, &leaf->mempool);
}

void tNeuron_initToPool  (tNeuron* const nr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNeuron* n = *nr = (_tNeuron*) mpool_alloc(sizeof(_tNeuron), m);
    n->mempool = m;
    LEAF* leaf = n->mempool->leaf;

    tPoleZero_initToPool(&n->f, mp);
    
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

void    tNeuron_free (tNeuron* const nr)
{
    _tNeuron* n = *nr;
    
    tPoleZero_free(&n->f);
    mpool_free((char*)n, n->mempool);
}

void   tNeuron_reset(tNeuron* const nr)
{
    _tNeuron* n = *nr;
    
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

void tNeuron_setV1(tNeuron* const nr, Lfloat V1)
{
    _tNeuron* n = *nr;
    n->V[0] = V1;
}

void tNeuron_setV2(tNeuron* const nr, Lfloat V2)
{
    _tNeuron* n = *nr;
    n->V[1] = V2;
}

void tNeuron_setV3(tNeuron* const nr, Lfloat V3)
{
    _tNeuron* n = *nr;
    n->V[2] = V3;
}

void tNeuron_setTimeStep(tNeuron* const nr, Lfloat timeStep)
{
    _tNeuron* n = *nr;
    n->timeStep = (44100.0f * n->invSampleRate) * timeStep;
}

void tNeuron_setK(tNeuron* const nr, Lfloat K)
{
    _tNeuron* n = *nr;
    n->gK = K;
}

void tNeuron_setL(tNeuron* const nr, Lfloat L)
{
    _tNeuron* n = *nr;
    n->gL = L;
    n->rate[2] = n->gL/n->C;
}

void tNeuron_setN(tNeuron* const nr, Lfloat N)
{
    _tNeuron* n = *nr;
    n->gN = N;
}

void tNeuron_setC(tNeuron* const nr, Lfloat C)
{
    _tNeuron* n = *nr;
    n->C = C;
    n->rate[2] = n->gL/n->C;
}

Lfloat tNeuron_tick(tNeuron* const nr)
{
    _tNeuron* n = *nr;
    
    Lfloat output = 0.0f;
    Lfloat voltage = n->voltage;
    
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
        Lfloat shapeVoltage = 0.01f * n->voltage;
        
        Lfloat w, c, xc, xc2, xc4;
        
        Lfloat sqrt8 = 2.82842712475f;
        
        Lfloat wscale = 1.30612244898f;
        Lfloat m_drive = 1.0f;
        
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

void tNeuron_setMode  (tNeuron* const nr, NeuronMode mode)
{
    _tNeuron* n = *nr;
    n->mode = mode;
}

void tNeuron_setCurrent  (tNeuron* const nr, Lfloat current)
{
    _tNeuron* n = *nr;
    n->current = current;
}

void tNeuron_setSampleRate (tNeuron* const nr, Lfloat sr)
{
    _tNeuron* n = *nr;
    n->invSampleRate = 1.0f/sr;
    n->timeStep = (44100.0f * n->invSampleRate) / 50.0f;
}

//----------------------------------------------------------------------------------------------------------

void tMBPulse_init(tMBPulse* const osc, LEAF* const leaf)
{
    tMBPulse_initToPool(osc, &leaf->mempool);
}
                          
void tMBPulse_initToPool(tMBPulse* const osc, tMempool* const pool)
{
    _tMempool* m = *pool;
    _tMBPulse* c = *osc = (_tMBPulse*) mpool_alloc(sizeof(_tMBPulse), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
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
    memset (c->_f, 0, 8 * sizeof (Lfloat));
}

void tMBPulse_free(tMBPulse* const osc)
{
    _tMBPulse* c = *osc;
    mpool_free((char*)c, c->mempool);
}

//#ifdef ITCMRAM
//void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBPulse_place_step_dd_noBuffer(tMBPulse* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale)
//#else
void tMBPulse_place_step_dd_noBuffer(tMBPulse* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale)
//#endif
{
	_tMBPulse* c = *osc;
	Lfloat r;
	long i;

	r = MINBLEP_PHASES * phase * inv_w;
	i = lrintf(r - 0.5f);
	r -= (Lfloat)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
	c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
	c->BLEPindices[c->mostRecentBLEP] = i;
    c->BLEPproperties[c->mostRecentBLEP][0] = r;
    c->BLEPproperties[c->mostRecentBLEP][1] = scale;
    c->numBLEPs = (c->numBLEPs + 1) & 63;
}


Lfloat tMBPulse_tick(tMBPulse* const osc)
{
    _tMBPulse* c = *osc;
    
    int    j, k;
    Lfloat  sync;
    Lfloat  b, p, w, x, z, sw;
    
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
    Lfloat inv_sw = c->_inv_w * c->syncdir;
    p += sw - (int)sw;
    
    if (sync > 0.0f && c->softsync == 0) {  /* sync to master */
        Lfloat eof_offset = sync * sw;
        Lfloat p_at_reset = p - eof_offset;
    
        if (sw > 0) p = eof_offset;
        else if (sw < 0) p = 1.0f - eof_offset;
        
        /* place any DDs that may have occurred in subsample before reset */
        if (!k) {
            if (sw > 0)
            {
                if (p_at_reset >= b) {
                	tMBPulse_place_step_dd_noBuffer(osc, j, p_at_reset - b + eof_offset, inv_sw, -1.0f);
                    k = 1;
                    x = -0.5f;
                }
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    tMBPulse_place_step_dd_noBuffer(osc, j, p_at_reset + eof_offset, inv_sw, 1.0f);
                    k = 0;
                    x = 0.5f;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    tMBPulse_place_step_dd_noBuffer(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
                    k = 1;
                    x = -0.5f;
                }
                if (k && p_at_reset < b) {
                	tMBPulse_place_step_dd_noBuffer(osc, j, b - p_at_reset - eof_offset, -inv_sw, 1.0f);
                    k = 0;
                    x = 0.5f;
                }
            }
        } else {
            if (sw > 0)
            {
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    tMBPulse_place_step_dd_noBuffer(osc, j, p_at_reset + eof_offset, inv_sw, 1.0f);
                    k = 0;
                    x = 0.5f;
                }
                if (!k && p_at_reset >= b) {
                	tMBPulse_place_step_dd_noBuffer(osc, j, p_at_reset - b + eof_offset, inv_sw, -1.0f);
                    k = 1;
                    x = -0.5f;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < b) {
                	tMBPulse_place_step_dd_noBuffer(osc, j, b - p_at_reset - eof_offset, -inv_sw, 1.0f);
                    k = 0;
                    x = 0.5f;
                }
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    tMBPulse_place_step_dd_noBuffer(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
                    k = 1;
                    x = -0.5f;
                }
            }
        }
        
        /* now place reset DD */
        if (sw > 0)
        {
            if (k) {
            	tMBPulse_place_step_dd_noBuffer(osc, j, p, inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
            if (p >= b) {
            	tMBPulse_place_step_dd_noBuffer(osc, j, p - b, inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
        }
        else if (sw < 0)
        {
            if (!k) {
            	tMBPulse_place_step_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
            if (p < b) {
            	tMBPulse_place_step_dd_noBuffer(osc, j, b - p, -inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
        }
    } else if (!k) {  /* normal operation, signal currently high */
        
        if (sw > 0)
        {
            if (p >= b) {
            	tMBPulse_place_step_dd_noBuffer(osc, j, p - b, inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
            if (p >= 1.0f) {
                p -= 1.0f;
                tMBPulse_place_step_dd_noBuffer(osc, j, p, inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
        }
        else if (sw < 0)
        {
            if (p < 0.0f) {
                p += 1.0f;
                tMBPulse_place_step_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
            if (k && p < b) {
            	tMBPulse_place_step_dd_noBuffer(osc, j, b - p, -inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
        }
        
    } else {  /* normal operation, signal currently low */
        
        if (sw > 0)
        {
            if (p >= 1.0f) {
                p -= 1.0f;
                tMBPulse_place_step_dd_noBuffer(osc, j, p, inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
            if (!k && p >= b) {
            	tMBPulse_place_step_dd_noBuffer(osc, j, p - b, inv_sw, -1.0f);
                k = 1;
                x = -0.5f;
            }
        }
        else if (sw < 0)
        {
            if (p < b) {
            	tMBPulse_place_step_dd_noBuffer(osc, j, b - p, -inv_sw, 1.0f);
                k = 0;
                x = 0.5f;
            }
            if (p < 0.0f) {
                p += 1.0f;
                tMBPulse_place_step_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -1.0f);
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

void tMBPulse_setFreq(tMBPulse* const osc, Lfloat f)
{
    _tMBPulse* c = *osc;
    c->freq = f;
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_inv_w = 1.0f / c->_w;
}

void tMBPulse_setWidth(tMBPulse* const osc, Lfloat w)
{
    _tMBPulse* c = *osc;
    c->waveform = w;
    c->_b = 0.5f * (1.0f + c->waveform);
}

Lfloat tMBPulse_sync(tMBPulse* const osc, Lfloat value)
{
    _tMBPulse* c = *osc;
    
    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    Lfloat last = c->lastsyncin;
    Lfloat delta = value - last;
    Lfloat crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;
    
    return value;
}

void tMBPulse_setPhase(tMBPulse* const osc, Lfloat phase)
{
    _tMBPulse* c = *osc;
    c->_p = phase;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBPulse_setBufferOffset(tMBPulse* const osc, uint32_t offset)
{
	_tMBPulse* c = *osc;
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBPulse_setSyncMode(tMBPulse* const osc, int hardOrSoft)
{
    _tMBPulse* c = *osc;
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

void tMBPulse_setSampleRate(tMBPulse* const osc, Lfloat sr)
{
    _tMBPulse* c = *osc;
    c->invSampleRate = 1.0f/sr;
}

//==========================================================================================================
//==========================================================================================================

void tMBTriangle_init(tMBTriangle* const osc, LEAF* const leaf)
{
    tMBTriangle_initToPool(osc, &leaf->mempool);
}

void tMBTriangle_initToPool(tMBTriangle* const osc, tMempool* const pool)
{
    _tMempool* m = *pool;
    _tMBTriangle* c = *osc = (_tMBTriangle*) mpool_alloc(sizeof(_tMBTriangle), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
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
    memset (c->_f, 0, 8 * sizeof (Lfloat));
}

void tMBTriangle_free(tMBTriangle* const osc)
{
    _tMBTriangle* c = *osc;
    mpool_free((char*)c, c->mempool);
}

//#ifdef ITCMRAM
//void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBTriangle_place_dd_noBuffer(tMBTriangle* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale, Lfloat stepOrSlope, Lfloat w)
//#else
void tMBTriangle_place_dd_noBuffer(tMBTriangle* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale, Lfloat stepOrSlope, Lfloat w)
//#endif
{
	_tMBTriangle* c = *osc;
	Lfloat r;
	long i;

	r = MINBLEP_PHASES * phase * inv_w;
	i = lrintf(r - 0.5f);
	r -= (Lfloat)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
	scale *= w;
	c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
	c->BLEPindices[c->mostRecentBLEP] = i;
    c->BLEPproperties[c->mostRecentBLEP][0] = r;
    c->BLEPproperties[c->mostRecentBLEP][1] = scale;
    c->BLEPproperties[c->mostRecentBLEP][2] = stepOrSlope;
    c->numBLEPs = (c->numBLEPs + 1) & 63;
}

Lfloat tMBTriangle_tick(tMBTriangle* const osc)
{
    _tMBTriangle* c = *osc;
    
    int    j, k;
    Lfloat  sync;
    Lfloat  b, b1, invB, invB1, p, w, sw, z;
    Lfloat  x = 0.5f;
    
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
    Lfloat inv_sw = c->_inv_w * c->syncdir;
    p += sw - (int)sw;
    
    if (sync > 0.0f && c->softsync == 0) {  /* sync to master */
        Lfloat eof_offset = sync * sw;
        Lfloat p_at_reset = p - eof_offset;
        
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
                    tMBTriangle_place_dd_noBuffer(osc, j, p_at_reset - b + eof_offset, inv_sw, -invB1 - invB, 1.0f, sw);
                    k = 1;
                }
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    x = -0.5f + p_at_reset * invB;
                    tMBTriangle_place_dd_noBuffer(osc, j, p_at_reset + eof_offset, inv_sw, invB + invB1, 1.0f, sw);
                    k = 0;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    x = 0.5f - (p_at_reset - b)  * invB1;
                    tMBTriangle_place_dd_noBuffer(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, invB + invB1, 1.0f, -sw);
                    k = 1;
                }
                if (k && p_at_reset < b) {
                    x = -0.5f + p_at_reset * invB;
                    tMBTriangle_place_dd_noBuffer(osc, j, b - p_at_reset - eof_offset, -inv_sw, -invB1 - invB, 1.0f, -sw);
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
                    tMBTriangle_place_dd_noBuffer(osc, j, p_at_reset + eof_offset, inv_sw, invB + invB1, 1.0f, sw);
                    k = 0;
                }
                if (!k && p_at_reset >= b) {
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBTriangle_place_dd_noBuffer(osc, j, p_at_reset - b + eof_offset, inv_sw, -invB1 - invB, 1.0f, sw);
                    k = 1;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < b) {
                    x = -0.5f + p_at_reset * invB;
                    tMBTriangle_place_dd_noBuffer(osc, j, b - p_at_reset - eof_offset, -inv_sw, -invB1 - invB, 1.0f, -sw);
                    k = 0;
                }
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBTriangle_place_dd_noBuffer(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, invB + invB1, 1.0f, -sw);
                    k = 1;
                }
            }
        }
        
        /* now place reset DDs */
        if (sw > 0)
        {
            if (k)
            	tMBTriangle_place_dd_noBuffer(osc, j, p, inv_sw, invB + invB1, 1.0f, sw);
            tMBTriangle_place_dd_noBuffer(osc, j, p, inv_sw, -0.5f - x, 0.0f, sw);
            x = -0.5f + p * invB;
            k = 0;
            if (p >= b) {
                x = 0.5f - (p - b) * invB1;
                tMBTriangle_place_dd_noBuffer(osc, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
        }
        else if (sw < 0)
        {
            if (!k)
            	tMBTriangle_place_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
            tMBTriangle_place_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -0.5f - x, 0.0f, -sw);
            x = 0.5f - (p - b) * invB1;
            k = 1;
            if (p < b) {
                x = -0.5f + p * invB;
                tMBTriangle_place_dd_noBuffer(osc, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
        }
    } else if (!k) {  /* normal operation, slope currently up */
        
        x = -0.5f + p * invB;
        if (sw > 0)
        {
            if (p >= b) {
                x = 0.5f - (p - b) * invB1;;
                tMBTriangle_place_dd_noBuffer(osc, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
            if (p >= 1.0f) {
                p -= 1.0f;
                x = -0.5f + p * invB;
                tMBTriangle_place_dd_noBuffer(osc, j, p, inv_sw, invB + invB1, 1.0f, sw);
                k = 0;
            }
        }
        else if (sw < 0)
        {
            if (p < 0.0f) {
                p += 1.0f;
                x = 0.5f - (p - b) * invB1;
                tMBTriangle_place_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
                k = 1;
            }
            if (k && p < b) {
                x = -0.5f + p * invB;
                tMBTriangle_place_dd_noBuffer(osc, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
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
                tMBTriangle_place_dd_noBuffer(osc, j, p, inv_sw, invB + invB1, 1.0f, sw);
                k = 0;
            }
            if (!k && p >= b) {
                x = 0.5f - (p - b) * invB1;
                tMBTriangle_place_dd_noBuffer(osc, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
        }
        else if (sw < 0)
        {
            if (p < b) {
                x = -0.5f + p * invB;
                tMBTriangle_place_dd_noBuffer(osc, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
            if (p < 0.0f) {
                p += 1.0f;
                x = 0.5f - (p - b) * invB1;
                tMBTriangle_place_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
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

void tMBTriangle_setFreq(tMBTriangle* const osc, Lfloat f)
{
    _tMBTriangle* c = *osc;
    c->freq = f;
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_inv_w = 1.0f / c->_w;
    //c->quarterwaveoffset = c->_w * 0.25f;
}

void tMBTriangle_setWidth(tMBTriangle* const osc, Lfloat w)
{
    _tMBTriangle* c = *osc;
    w = LEAF_clip(0.0f, w, 0.99f);
    c->waveform = w;
}

Lfloat tMBTriangle_sync(tMBTriangle* const osc, Lfloat value)
{
    _tMBTriangle* c = *osc;
    
    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    Lfloat last = c->lastsyncin;
    Lfloat delta = value - last;
    Lfloat crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;
    
    return value;
}

void tMBTriangle_setPhase(tMBTriangle* const osc, Lfloat phase)
{
    _tMBTriangle* c = *osc;
    c->_p = phase;
}

void tMBTriangle_setSyncMode(tMBTriangle* const osc, int hardOrSoft)
{
    _tMBTriangle* c = *osc;
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBTriangle_setBufferOffset(tMBTriangle* const osc, uint32_t offset)
{
	_tMBTriangle* c = *osc;
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBTriangle_setSampleRate(tMBTriangle* const osc, Lfloat sr)
{
    _tMBTriangle* c = *osc;
    c->invSampleRate = 1.0f/sr;
}



//==========================================================================================================

void tMBSineTri_init(tMBSineTri* const osc, LEAF* const leaf)
{
    tMBSineTri_initToPool(osc, &leaf->mempool);
}

void tMBSineTri_initToPool(tMBSineTri* const osc, tMempool* const pool)
{
    _tMempool* m = *pool;
    _tMBSineTri* c = *osc = (_tMBSineTri*) mpool_alloc(sizeof(_tMBSineTri), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;

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
    memset (c->_f, 0, 8 * sizeof (Lfloat));
}

void tMBSineTri_free(tMBSineTri* const osc)
{
    _tMBSineTri* c = *osc;
    mpool_free((char*)c, c->mempool);
}

//#ifdef ITCMRAM
//void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSineTri_place_dd_noBuffer(tMBSineTri* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale, Lfloat stepOrSlope, Lfloat w)
//#else
void tMBSineTri_place_dd_noBuffer(tMBSineTri* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale, Lfloat stepOrSlope, Lfloat w)
//#endif
{
	_tMBSineTri* c = *osc;
	Lfloat r;
	long i;

	r = MINBLEP_PHASES * phase * inv_w;
	i = lrintf(r - 0.5f);
	r -= (Lfloat)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
	scale *= w;
	c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
	c->BLEPindices[c->mostRecentBLEP] = i;
    c->BLEPproperties[c->mostRecentBLEP][0] = r;
    c->BLEPproperties[c->mostRecentBLEP][1] = scale;
    c->BLEPproperties[c->mostRecentBLEP][2] = stepOrSlope;
    c->numBLEPs = (c->numBLEPs + 1) & 63;
}

Lfloat tMBSineTri_tick(tMBSineTri* const osc)
{
    _tMBSineTri* c = *osc;

    int    j, k;
    Lfloat  sync;
    Lfloat  b, b1, invB, invB1, p, sinPhase, w, sw, z;
    Lfloat  x = 0.5f;

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
    Lfloat inv_sw = c->_inv_w * c->syncdir;
    Lfloat inc_amount = sw - (int)sw;
    p += inc_amount;
    sinPhase += inc_amount;

    if (sync > 0.0f && c->softsync == 0) {  /* sync to master */
        Lfloat eof_offset = sync * sw;
        Lfloat p_at_reset = p - eof_offset;
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
                    tMBSineTri_place_dd_noBuffer(osc, j, p_at_reset - b + eof_offset, inv_sw, -invB1 - invB, 1.0f, sw);
                    k = 1;
                }
                if (p_at_reset >= 1.0f) {
                    p_at_reset -= 1.0f;
                    x = -0.5f + p_at_reset * invB;
                    tMBSineTri_place_dd_noBuffer(osc, j, p_at_reset + eof_offset, inv_sw, invB + invB1, 1.0f, sw);
                    k = 0;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    x = 0.5f - (p_at_reset - b)  * invB1;
                    tMBSineTri_place_dd_noBuffer(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, invB + invB1, 1.0f, -sw);
                    k = 1;
                }
                if (k && p_at_reset < b) {
                    x = -0.5f + p_at_reset * invB;
                    tMBSineTri_place_dd_noBuffer(osc, j, b - p_at_reset - eof_offset, -inv_sw, -invB1 - invB, 1.0f, -sw);
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
                    tMBSineTri_place_dd_noBuffer(osc, j, p_at_reset + eof_offset, inv_sw, invB + invB1, 1.0f, sw);
                    k = 0;
                }
                if (!k && p_at_reset >= b) {
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBSineTri_place_dd_noBuffer(osc, j, p_at_reset - b + eof_offset, inv_sw, -invB1 - invB, 1.0f, sw);
                    k = 1;
                }
            }
            else if (sw < 0)
            {
                if (p_at_reset < b) {
                    x = -0.5f + p_at_reset * invB;
                    tMBSineTri_place_dd_noBuffer(osc, j, b - p_at_reset - eof_offset, -inv_sw, -invB1 - invB, 1.0f, -sw);
                    k = 0;
                }
                if (p_at_reset < 0.0f) {
                    p_at_reset += 1.0f;
                    x = 0.5f - (p_at_reset - b) * invB1;
                    tMBSineTri_place_dd_noBuffer(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, invB + invB1, 1.0f, -sw);
                    k = 1;
                }
            }
        }

        /* now place reset DDs */
        if (sw > 0)
        {
            if (k)
            	tMBSineTri_place_dd_noBuffer(osc, j, p, inv_sw, invB + invB1, 1.0f, sw);
            tMBSineTri_place_dd_noBuffer(osc, j, p, inv_sw, 0.0f - x, 0.0f, sw);
            x = -0.5f + p * invB;
            k = 0;
            if (p >= b) {
                x = 0.5f - (p - b) * invB1;
                tMBSineTri_place_dd_noBuffer(osc, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
        }
        else if (sw < 0)
        {
            if (!k)
            	tMBSineTri_place_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
            tMBSineTri_place_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, 0.0f - x, 0.0f, -sw);
            x = 0.5f - (p - b) * invB1;
            k = 1;
            if (p < b) {
                x = -0.5f + p * invB;
                tMBSineTri_place_dd_noBuffer(osc, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
        }
    } else if (!k) {  /* normal operation, slope currently up */

        x = -0.5f + p * invB;
        if (sw > 0)
        {
            if (p >= b) {
                x = 0.5f - (p - b) * invB1;;
                tMBSineTri_place_dd_noBuffer(osc, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
            if (p >= 1.0f) {
                p -= 1.0f;
                x = -0.5f + p * invB;
                tMBSineTri_place_dd_noBuffer(osc, j, p, inv_sw, invB + invB1, 1.0f, sw);
                k = 0;
            }
        }
        else if (sw < 0)
        {
            if (p < 0.0f) {
                p += 1.0f;
                x = 0.5f - (p - b) * invB1;
                tMBSineTri_place_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
                k = 1;
            }
            if (k && p < b) {
                x = -0.5f + p * invB;
                tMBSineTri_place_dd_noBuffer(osc, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
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
                tMBSineTri_place_dd_noBuffer(osc, j, p, inv_sw, invB + invB1, 1.0f, sw);
                k = 0;
            }
            if (!k && p >= b) {
                x = 0.5f - (p - b) * invB1;
                tMBSineTri_place_dd_noBuffer(osc, j, p - b, inv_sw, -invB1 - invB, 1.0f, sw);
                k = 1;
            }
        }
        else if (sw < 0)
        {
            if (p < b) {
                x = -0.5f + p * invB;
                tMBSineTri_place_dd_noBuffer(osc, j, b - p, -inv_sw, -invB1 - invB, 1.0f, -sw);
                k = 0;
            }
            if (p < 0.0f) {
                p += 1.0f;
                x = 0.5f - (p - b) * invB1;
                tMBSineTri_place_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, invB + invB1, 1.0f, -sw);
                k = 1;
            }
        }
    }
    int currentSamp = (j + DD_SAMPLE_DELAY) & 7;

    c->_f[currentSamp] = x * c->shape; //add the triangle


    Lfloat tempFrac;
    uint32_t idx;
    Lfloat samp0;
    Lfloat samp1;

    // Wavetable synthesis
    while (sinPhase >= 1.0f)
    {
    	sinPhase -= 1.0f;
    }

    while (sinPhase < 0.0f)
    {
    	sinPhase += 1.0f;
    }
    Lfloat tempPhase = (sinPhase * 2048.0f);
    idx = (uint32_t)tempPhase; //11 bit table
    tempFrac = tempPhase - idx;
    samp0 = __leaf_table_sinewave[idx];
    idx = (idx + 1) & c->sineMask;
    samp1 = __leaf_table_sinewave[idx];

    Lfloat sinOut = (samp0 + (samp1 - samp0) * tempFrac) * 0.5f; // 1/2097151

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

void tMBSineTri_setFreq(tMBSineTri* const osc, Lfloat f)
{
    _tMBSineTri* c = *osc;
    c->freq = f;
    c->_w = c->freq * c->invSampleRate;  /* phase increment */
    c->_inv_w = 1.0f / c->_w;
    //c->quarterwaveoffset = c->_w * 0.25f;
}

void tMBSineTri_setWidth(tMBSineTri* const osc, Lfloat w)
{
    _tMBSineTri* c = *osc;
    w = LEAF_clip(0.0f, w, 0.99f);
    c->waveform = w;
}

Lfloat tMBSineTri_sync(tMBSineTri* const osc, Lfloat value)
{
    _tMBSineTri* c = *osc;

    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    Lfloat last = c->lastsyncin;
    Lfloat delta = value - last;
    Lfloat crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;

    return value;
}

void tMBSineTri_setPhase(tMBSineTri* const osc, Lfloat phase)
{
    _tMBSineTri* c = *osc;
    c->_p = phase;
}

void tMBSineTri_setShape(tMBSineTri* const osc, Lfloat shape)
{
    _tMBSineTri* c = *osc;
    c->shape = shape;
}


void tMBSineTri_setSyncMode(tMBSineTri* const osc, int hardOrSoft)
{
    _tMBSineTri* c = *osc;
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBSineTri_setBufferOffset(tMBSineTri* const osc, uint32_t offset)
{
	_tMBSineTri* c = *osc;
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBSineTri_setSampleRate(tMBSineTri* const osc, Lfloat sr)
{
    _tMBSineTri* c = *osc;
    c->invSampleRate = 1.0f/sr;
}
//==================================================================================================
//==================================================================================================

void tMBSaw_init(tMBSaw* const osc, LEAF* const leaf)
{
    tMBSaw_initToPool(osc, &leaf->mempool);
}

void tMBSaw_initToPool(tMBSaw* const osc, tMempool* const pool)
{
    _tMempool* m = *pool;
    _tMBSaw* c = *osc = (_tMBSaw*) mpool_alloc(sizeof(_tMBSaw), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
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
    memset (c->_f, 0, 8 * sizeof (Lfloat));
}

void tMBSaw_free(tMBSaw* const osc)
{
    _tMBSaw* c = *osc;
    mpool_free((char*)c, c->mempool);
}


//#ifdef ITCMRAM
//void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSaw_place_step_dd_noBuffer(tMBSaw* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale)
//#else
void tMBSaw_place_step_dd_noBuffer(tMBSaw* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale)
//#endif
{
	_tMBSaw* c = *osc;
	Lfloat r;
	long i;

	r = MINBLEP_PHASES * phase * inv_w;
	i = lrintf(r - 0.5f);
	r -= (Lfloat)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
	c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
	c->BLEPindices[c->mostRecentBLEP] = i;
    c->BLEPproperties[c->mostRecentBLEP][0] = r;
    c->BLEPproperties[c->mostRecentBLEP][1] = scale;
    c->numBLEPs = (c->numBLEPs + 1) & 63;
}



Lfloat tMBSaw_tick(tMBSaw* const osc)
{
    _tMBSaw* c = *osc;

    int    j;
    Lfloat  sync;
    Lfloat  p, sw, z;

    sync = c->sync;


    p = c->_p;  /* phase [0, 1) */
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */


    if (sync > 0.0f && c->softsync > 0) c->syncdir = -c->syncdir;
    sw = c->_w * c->syncdir;
    Lfloat inv_sw = c->_inv_w * c->syncdir;
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
        Lfloat eof_offset = sync * sw;
        Lfloat p_at_reset = p - eof_offset;

        if (sw > 0) p = eof_offset;
        else if (sw < 0) p = 1.0f - eof_offset;

        /* place any DD that may have occurred in subsample before reset */
        if (p_at_reset >= 1.0f) {
            p_at_reset -= 1.0f;
            tMBSaw_place_step_dd_noBuffer(osc, j, p_at_reset + eof_offset, inv_sw, 1.0f);
        }
        if (p_at_reset < 0.0f) {
            p_at_reset += 1.0f;
            tMBSaw_place_step_dd_noBuffer(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
        }

        /* now place reset DD */
        if (sw > 0)
        	tMBSaw_place_step_dd_noBuffer(osc, j, p, inv_sw, p_at_reset);
        else if (sw < 0)
        	tMBSaw_place_step_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -p_at_reset);

    } else if (p >= 1.0f) {  /* normal phase reset */
        p -= 1.0f;
        tMBSaw_place_step_dd_noBuffer(osc, j, p, inv_sw, 1.0f);

    } else if (p < 0.0f) {
        p += 1.0f;
        tMBSaw_place_step_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -1.0f);
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

void tMBSaw_setFreq(tMBSaw* const osc, Lfloat f)
{
    _tMBSaw* c = *osc;
    c->freq = f;

    c->_w = c->freq * c->invSampleRate;
    c->_inv_w = 1.0f / c->_w;
}

Lfloat tMBSaw_sync(tMBSaw* const osc, Lfloat value)
{
    _tMBSaw* c = *osc;
    
    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    Lfloat last = c->lastsyncin;
    Lfloat delta = value - last;
    Lfloat crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;
    
    return value;
}

void tMBSaw_setPhase(tMBSaw* const osc, Lfloat phase)
{
    _tMBSaw* c = *osc;
    c->_p = phase;
}

void tMBSaw_setSyncMode(tMBSaw* const osc, int hardOrSoft)
{
    _tMBSaw* c = *osc;
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBSaw_setBufferOffset(tMBSaw* const osc, uint32_t offset)
{
	_tMBSaw* c = *osc;
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBSaw_setSampleRate(tMBSaw* const osc, Lfloat sr)
{
    _tMBSaw* c = *osc;
    c->invSampleRate = 1.0f/sr;
}


//==================================================================================================

void tMBSawPulse_init(tMBSawPulse* const osc, LEAF* const leaf)
{
    tMBSawPulse_initToPool(osc, &leaf->mempool);
}

void tMBSawPulse_initToPool(tMBSawPulse* const osc, tMempool* const pool)
{
    _tMempool* m = *pool;
    _tMBSawPulse* c = *osc = (_tMBSawPulse*) mpool_alloc(sizeof(_tMBSawPulse), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
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
    memset (c->_f, 0, 8 * sizeof (Lfloat));

}

void tMBSawPulse_free(tMBSawPulse* const osc)
{
    _tMBSawPulse* c = *osc;
    mpool_free((char*)c, c->mempool);
}


#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSawPulse_place_step_dd_noBuffer(tMBSawPulse* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale)
#else
void tMBSawPulse_place_step_dd_noBuffer(tMBSawPulse* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale)
#endif
{
    _tMBSawPulse* c = *osc;

    if (c->active)
    {
		Lfloat r;
		long i;

		r = MINBLEP_PHASES * phase * inv_w;
		i = lrintf(r - 0.5f);
		r -= (Lfloat)i;
		i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
		c->mostRecentBLEP = (c->mostRecentBLEP + 1) & 63;
		c->BLEPindices[c->mostRecentBLEP] = i;
		c->BLEPproperties[c->mostRecentBLEP][0] = r;
		c->BLEPproperties[c->mostRecentBLEP][1] = scale;

		c->numBLEPs = (c->numBLEPs + 1) & 63;
    }
}



#ifdef ITCMRAM
Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSawPulse_tick(tMBSawPulse* const osc)
#else
Lfloat tMBSawPulse_tick(tMBSawPulse* const osc)
#endif
{
    _tMBSawPulse* c = *osc;
    int    j, k;
    Lfloat  sync;
    Lfloat  b, p, w, x, z, sw;
    Lfloat shape = c->shape;
    Lfloat sawShape = 1.0f - c->shape;
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
    Lfloat inv_sw = c->_inv_w * c->syncdir;
    p += sw - (int)sw;
    if (sync > 0.0f && c->softsync == 0)
    {  /* sync to master */
        Lfloat eof_offset = sync * sw;
        Lfloat p_at_reset = p - eof_offset;

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
					 tMBSawPulse_place_step_dd_noBuffer(osc, j, p_at_reset - b + eof_offset, inv_sw, -1.0f * shape);
					 k = 1;
					 x = -0.5f;
				 }
				 if (p_at_reset >= 1.0f)
				 {
					 p_at_reset -= 1.0f;
					 tMBSawPulse_place_step_dd_noBuffer(osc, j, p_at_reset + eof_offset, inv_sw, 1.0f);
					 k = 0;
					 x = 0.5f;
				 }
			 }
			 else if (sw < 0)
			 {
				 if (p_at_reset < 0.0f)
				 {
					 p_at_reset += 1.0f;
					 tMBSawPulse_place_step_dd_noBuffer(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
					 k = 1;
					 x = -0.5f;
				 }
				 if (k && p_at_reset < b)
				 {
					 tMBSawPulse_place_step_dd_noBuffer(osc, j, b - p_at_reset - eof_offset, -inv_sw, 1.0f * shape);
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
					 tMBSawPulse_place_step_dd_noBuffer(osc, j, p_at_reset + eof_offset, inv_sw, 1.0f);
					 k = 0;
					 x = 0.5f;
				 }
				 if (!k && p_at_reset >= b)
				 {
					 tMBSawPulse_place_step_dd_noBuffer(osc, j, p_at_reset - b + eof_offset, inv_sw, -1.0f * shape);
					 k = 1;
					 x = -0.5f;
				 }
			 }
			 else if (sw < 0)
			 {
				 if (p_at_reset < b)
				 {
					 tMBSawPulse_place_step_dd_noBuffer(osc, j, b - p_at_reset - eof_offset, -inv_sw, 1.0f * shape);
					 k = 0;
					 x = 0.5f;
				 }
				 if (p_at_reset < 0.0f)
				 {
					 p_at_reset += 1.0f;
					 tMBSawPulse_place_step_dd_noBuffer(osc, j, 1.0f - p_at_reset - eof_offset, -inv_sw, -1.0f);
					 k = 1;
					 x = -0.5f;
				 }
			 }
		 }



		if (sw > 0)
		{
			/* now place reset DD for saw*/
			tMBSawPulse_place_step_dd_noBuffer(osc, j, p, inv_sw, p_at_reset * sawShape);
            /* now place reset DD for pulse */
            if (k) {
            	tMBSawPulse_place_step_dd_noBuffer(osc, j, p, inv_sw, 1.0f * shape);
				k = 0;
				x = 0.5f;
			}
			if (p >= b) {
				tMBSawPulse_place_step_dd_noBuffer(osc, j, p - b, inv_sw, -1.0f * shape);
				k = 1;
				x = -0.5f;
			}
		}
		else if (sw < 0)
		{
	        /* now place reset DD for saw*/
			tMBSawPulse_place_step_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -p_at_reset * sawShape);
			 /* now place reset DD for pulse */
			if (!k) {
				tMBSawPulse_place_step_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -1.0f * shape);
				k = 1;
				x = -0.5f;
			}
			if (p < b) {
				tMBSawPulse_place_step_dd_noBuffer(osc, j, b - p, -inv_sw, 1.0f * shape);
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
				tMBSawPulse_place_step_dd_noBuffer(osc, j, p - b, inv_sw, -1.0f * shape);
				k = 1;
				x = -0.5f;
			}
			if (p >= 1.0f) {
				p -= 1.0f;
				tMBSawPulse_place_step_dd_noBuffer(osc, j, p, inv_sw, 1.0f);
				k = 0;
				x = 0.5f;
			}
		}
		else if (sw < 0)
		{
			if (p < 0.0f) {
				p += 1.0f;
				tMBSawPulse_place_step_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -1.0f);
				k = 1;
				x = -0.5f;
			}
			if (k && p < b) {
				tMBSawPulse_place_step_dd_noBuffer(osc, j, b - p, -inv_sw, 1.0f * shape);
				k = 0;
				x = 0.5f;
			}
		}

	} else {  /* normal operation, signal currently low */

		if (sw > 0)
		{
			if (p >= 1.0f) {
				p -= 1.0f;
				tMBSawPulse_place_step_dd_noBuffer(osc, j, p, inv_sw, 1.0f);
				k = 0;
				x = 0.5f;
			}
			if (!k && p >= b) {
				tMBSawPulse_place_step_dd_noBuffer(osc, j, p - b, inv_sw, -1.0f * shape);
				k = 1;
				x = -0.5f;
			}
		}
		else if (sw < 0)
		{
			if (p < b) {
				tMBSawPulse_place_step_dd_noBuffer(osc, j, b - p, -inv_sw, 1.0f * shape);
				k = 0;
				x = 0.5f;
			}
			if (p < 0.0f) {
				p += 1.0f;
				tMBSawPulse_place_step_dd_noBuffer(osc, j, 1.0f - p, -inv_sw, -1.0f);
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
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tMBSawPulse_setFreq(tMBSawPulse* const osc, Lfloat f)
#else
void tMBSawPulse_setFreq(tMBSawPulse* const osc, Lfloat f)
#endif
{
    _tMBSawPulse* c = *osc;
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
Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tMBSawPulse_sync(tMBSawPulse* const osc, Lfloat value)
#else
Lfloat tMBSawPulse_sync(tMBSawPulse* const osc, Lfloat value)
#endif
{
    _tMBSawPulse* c = *osc;

    //based on https://github.com/VCVRack/Fundamental/blob/5799ee2a9b21492b42ebcb9b65d5395ef5c1cbe2/src/VCO.cpp#L123
    Lfloat last = c->lastsyncin;
    Lfloat delta = value - last;
    Lfloat crossing = -last / delta;
    c->lastsyncin = value;
    if ((0.f < crossing) && (crossing <= 1.f) && (value >= 0.f))
        c->sync = (1.f - crossing) * delta;
    else c->sync = 0.f;

    return value;
}

void tMBSawPulse_setPhase(tMBSawPulse* const osc, Lfloat phase)
{
    _tMBSawPulse* c = *osc;
    c->_p = phase;
}

void tMBSawPulse_setShape(tMBSawPulse* const osc, Lfloat shape)
{
    _tMBSawPulse* c = *osc;
    c->shape = shape;
}

void tMBSawPulse_setSyncMode(tMBSawPulse* const osc, int hardOrSoft)
{
    _tMBSawPulse* c = *osc;
    c->softsync = hardOrSoft > 0 ? 1 : 0;
}

//useful if you have several oscillators so the buffer refill is not synchronized
void tMBSawPulse_setBufferOffset(tMBSawPulse* const osc, uint32_t offset)
{
	_tMBSawPulse* c = *osc;
	offset = offset & (FILLEN-1);
	c->_j = offset;
}

void tMBSawPulse_setSampleRate(tMBSawPulse* const osc, Lfloat sr)
{
    _tMBSawPulse* c = *osc;
    c->invSampleRate = 1.0f/sr;
}


// WaveTable
void    tTable_init(tTable* const cy, Lfloat* waveTable, int size, LEAF* const leaf)
{
    tTable_initToPool(cy, waveTable, size, &leaf->mempool);
}

void    tTable_initToPool(tTable* const cy, Lfloat* waveTable, int size, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTable* c = *cy = (_tTable*)mpool_alloc(sizeof(_tTable), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->waveTable = waveTable;
    c->size = size;
    c->inc = 0.0f;
    c->phase = 0.0f;
    c->invSampleRate = leaf->invSampleRate;
}

void    tTable_free(tTable* const cy)
{
    _tTable* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

void     tTable_setFreq(tTable* const cy, Lfloat freq)
{
    _tTable* c = *cy;
    
    if (!isfinite(freq)) return;
    
    c->freq = freq;
    c->inc = freq * c->invSampleRate;
    c->inc -= (int)c->inc;
}

Lfloat   tTable_tick(tTable* const cy)
{
    _tTable* c = *cy;
    Lfloat temp;
    int intPart;
    Lfloat fracPart;
    Lfloat samp0;
    Lfloat samp1;
    
    // Phasor increment
    c->phase += c->inc;
    if (c->phase >= 1.0f) c->phase -= 1.0f;
    if (c->phase < 0.0f) c->phase += 1.0f;
    
    // Wavetable synthesis
    
    temp = c->size * c->phase;
    intPart = (int)temp;
    fracPart = temp - (Lfloat)intPart;
    samp0 = c->waveTable[intPart];
    if (++intPart >= c->size) intPart = 0;
    samp1 = c->waveTable[intPart];
    
    return (samp0 + (samp1 - samp0) * fracPart);
}

void     tTable_setSampleRate(tTable* const cy, Lfloat sr)
{
    _tTable* c = *cy;
    c->invSampleRate = 1.0f/sr;
    tTable_setFreq(cy, c->freq);
}

void tWaveTable_init(tWaveTable* const cy, Lfloat* table, int size, Lfloat maxFreq, LEAF* const leaf)
{
    tWaveTable_initToPool(cy, table, size, maxFreq, &leaf->mempool);
}

void tWaveTable_initToPool(tWaveTable* const cy, Lfloat* table, int size, Lfloat maxFreq, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tWaveTable* c = *cy = (_tWaveTable*) mpool_alloc(sizeof(_tWaveTable), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->sampleRate = leaf->sampleRate;
    
    c->maxFreq = maxFreq;
    
    // Determine base frequency
    c->baseFreq = c->sampleRate / (Lfloat) size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    
    // Determine how many tables we need
    // Assume we need at least 2, the fundamental + one to account for setting extra anti aliasing
    c->numTables = 2;
    Lfloat f = c->baseFreq;
    while (f < c->maxFreq)
    {
        c->numTables++;
        f *= 2.0f; // pass this multiplier in to set spacing of tables? would need to change setFreq too
    }
    
    c->size = size;
    c->sizeMask = size-1;
    // Allocate memory for the tables
    c->tables = (Lfloat**) mpool_alloc(sizeof(Lfloat*) * c->numTables, c->mempool);
    c->baseTable = (Lfloat*) mpool_alloc(sizeof(Lfloat) * c->size, c->mempool);
    c->tables[0] = c->baseTable;
    for (int t = 1; t < c->numTables; ++t)
    {
        c->tables[t] = (Lfloat*) mpool_alloc(sizeof(Lfloat) * c->size, c->mempool);
    }
    
    // Copy table
    for (int i = 0; i < c->size; ++i)
    {
        c->baseTable[i] = table[i];
    }
    
    // Make bandlimited copies
    f = c->sampleRate * 0.25; //start at half nyquist
    // Not worth going over order 8 I think, and even 8 is only marginally better than 4.
    tButterworth_initToPool(&c->bl, 8, -1.0f, f, mp);
    for (int t = 1; t < c->numTables; ++t)
    {
        tButterworth_setF2(&c->bl, f);
        // Do several passes here to prevent errors at the beginning of the waveform
        // Not sure how many passes to do, seem to need more as the filter cutoff goes down
        // 12 might be excessive but seems to work for now.
        for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
        {
            for (int i = 0; i < c->size; ++i)
            {
                c->tables[t][i] = tButterworth_tick(&c->bl, c->tables[t-1][i]);
            }
        }
        f *= 0.5f; //halve the cutoff for next pass
    }
    tButterworth_free(&c->bl);
}

void tWaveTable_free(tWaveTable* const cy)
{
    _tWaveTable* c = *cy;
    
    mpool_free((char*)c->baseTable, c->mempool);
    for (int t = 1; t < c->numTables; ++t)
    {
        mpool_free((char*)c->tables[t], c->mempool);
    }
    mpool_free((char*)c->tables, c->mempool);
    mpool_free((char*)c, c->mempool);
}

void tWaveTable_setSampleRate(tWaveTable* const cy, Lfloat sr)
{
    _tWaveTable* c = *cy;
        
    // Changing the sample rate of a wavetable requires up to partially reinitialize
    for (int t = 1; t < c->numTables; ++t)
    {
        mpool_free((char*)c->tables[t], c->mempool);
    }
    mpool_free((char*)c->tables, c->mempool);
    
    c->sampleRate = sr;
    
    // Determine base frequency
    c->baseFreq = c->sampleRate / (Lfloat) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    
    // Determine how many tables we need
    // Assume we need at least 2, the fundamental + one to account for setting extra anti aliasing
    c->numTables = 2;
    Lfloat f = c->baseFreq;
    while (f < c->maxFreq)
    {
        c->numTables++;
        f *= 2.0f; // pass this multiplier in to set spacing of tables? would need to change setFreq too
    }
    
    // Allocate memory for the tables
    c->tables = (Lfloat**) mpool_alloc(sizeof(Lfloat*) * c->numTables, c->mempool);
    c->tables[0] = c->baseTable;
    for (int t = 1; t < c->numTables; ++t)
    {
        c->tables[t] = (Lfloat*) mpool_alloc(sizeof(Lfloat) * c->size, c->mempool);
    }
    
    // Make bandlimited copies
    f = c->sampleRate * 0.25f; //start at half nyquist
    // Not worth going over order 8 I think, and even 8 is only marginally better than 4.
    tButterworth_initToPool(&c->bl, 8, -1.0f, f, &c->mempool);
    tButterworth_setSampleRate(&c->bl, c->sampleRate);
    for (int t = 1; t < c->numTables; ++t)
    {
        tButterworth_setF2(&c->bl, f);
        // Do several passes here to prevent errors at the beginning of the waveform
        // Not sure how many passes to do, seem to need more as the filter cutoff goes down
        // 12 might be excessive but seems to work for now.
        for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
        {
            for (int i = 0; i < c->size; ++i)
            {
                c->tables[t][i] = tButterworth_tick(&c->bl, c->tables[t-1][i]);
            }
        }
        f *= 0.5f; //halve the cutoff for next pass
    }
    tButterworth_free(&c->bl);
}

//================================================================================================
//================================================================================================

void tWaveOsc_init(tWaveOsc* const cy, tWaveTable* tables, int numTables, LEAF* const leaf)
{
    tWaveOsc_initToPool(cy, tables, numTables, &leaf->mempool);
}

void tWaveOsc_initToPool(tWaveOsc* const cy, tWaveTable* tables, int numTables, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tWaveOsc* c = *cy = (_tWaveOsc*) mpool_alloc(sizeof(_tWaveOsc), m);

    c->mempool = m;

    LEAF* leaf = c->mempool->leaf;
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
    c->baseFreq = c->sampleRate / (Lfloat) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;

    c->invSampleRateTimesTwoTo32 = leaf->invSampleRate * TWO_TO_32;
    c->maxFreq = c->tables[0]->maxFreq;
}

void tWaveOsc_free(tWaveOsc* const cy)
{
    _tWaveOsc* c = *cy;
    mpool_free((char*)c, c->mempool);
}

Lfloat tWaveOsc_tick(tWaveOsc* const cy)
{
    _tWaveOsc* c = *cy;
    
    // Phasor increment (unsigned 32bit int wraps automatically with overflow so no need for if branch checks, as you need with Lfloat)
    c->phase += c->inc;
    Lfloat LfloatPhase = (double)c->phase * 2.32830643654e-10;
    Lfloat s1 = 0.f, s2 = 0.f;

    Lfloat temp;
    int idx;
    Lfloat frac;
    Lfloat samp0;
    Lfloat samp1;

    int oct = c->oct;

    int sizeMask = c->tables[c->o1]->sizeMask;
    Lfloat** tables = c->tables[c->o1]->tables;

    // Wavetable synthesis
    temp = sizeMask * LfloatPhase;
    idx = (int)temp;
    frac = temp - (Lfloat)idx;
    samp0 = tables[oct][idx];
    idx = (idx + 1) & sizeMask;
    samp1 = tables[oct][idx];

    Lfloat oct0 = (samp0 + (samp1 - samp0) * frac);

    temp = sizeMask * LfloatPhase;
    idx = (int)temp;
    frac = temp - (Lfloat)idx;
    samp0 = tables[oct+1][idx];
    idx = (idx + 1) & sizeMask;
    samp1 = tables[oct+1][idx];

    Lfloat oct1 = (samp0 + (samp1 - samp0) * frac);

    s1 = oct0 + (oct1 - oct0) * c->w;

    sizeMask = c->tables[c->o2]->sizeMask;
    tables = c->tables[c->o2]->tables;

    // Wavetable synthesis
    temp = sizeMask * LfloatPhase;
    idx = (int)temp;
    frac = temp - (Lfloat)idx;
    samp0 = tables[oct][idx];
    idx = (idx + 1) & sizeMask;
    samp1 = tables[oct][idx];

    oct0 = (samp0 + (samp1 - samp0) * frac);

    temp = sizeMask * LfloatPhase;
    idx = (int)temp;
    frac = temp - (Lfloat)idx;
    samp0 = tables[oct+1][idx];
    idx = (idx + 1) & sizeMask;
    samp1 = tables[oct+1][idx];

    oct1 = (samp0 + (samp1 - samp0) * frac);

    s2 = oct0 + (oct1 - oct0) * c->w;

    // Ideally should determine correlation to get a good equal power fade between tables
    return s1 + (s2 - s1) * c->mix;
}

void tWaveOsc_setFreq(tWaveOsc* const cy, Lfloat freq)
{
    _tWaveOsc* c = *cy;

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

void tWaveOsc_setAntiAliasing(tWaveOsc* const cy, Lfloat aa)
{
    _tWaveOsc* c = *cy;
    c->aa = aa;
}

void tWaveOsc_setIndex(tWaveOsc* const cy, Lfloat index)
{
    _tWaveOsc* c = *cy;
    c->index = index;
    Lfloat f = c->index * (c->numTables - 1);

    c->o1 = (int)f;
    c->o2 = c->o1 + 1;
    if (c->index >= 1.0f) c->o2 = c->o1;
    c->mix = f - c->o1;
}

void tWaveOsc_setTables(tWaveOsc* const cy, tWaveTable* tables, int numTables)
{
    _tWaveOsc* c = *cy;
    LEAF* leaf = c->mempool->leaf;
    c->tables =  tables;
    c->numTables = numTables;
    c->size = c->tables[0]->size;

    c->sampleRate = leaf->sampleRate;
    // Determine base frequency
    c->baseFreq = c->sampleRate / (Lfloat) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;
}

/*//// eventually gotta finish this so you can X/Y control the indices and fade between non-adjacent tables
void tWaveOscS_setIndexXY(tWaveOscS* const cy, Lfloat indexX, Lfloat indexY)
{
    _tWaveOscS* c = *cy;
    c->index = index;
    Lfloat f1 = c->index * (c->numTables - 1);

    c->o1 = (int)f1;
    c->o2 = c->o1 + 1;
    if (c->index >= 1.0f) c->o2 = c->o1;
    c->mix = f1 - c->o1;

    Lfloat f2 = c->index * (c->numTables - 1);
}
*/

void tWaveOsc_setSampleRate(tWaveOsc* const cy, Lfloat sr)
{
    _tWaveOsc* c = *cy;
    
    c->sampleRate = sr;
    // Determine base frequency
    c->baseFreq = c->sampleRate / (Lfloat) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;
    c->invSampleRateTimesTwoTo32 = 1.f/c->sampleRate * TWO_TO_32;
    
    tWaveOsc_setFreq(cy, c->freq);
}

//=======================================================================================
//=======================================================================================

void tWaveTableS_init(tWaveTableS* const cy, Lfloat* table, int size, Lfloat maxFreq, LEAF* const leaf)
{
    tWaveTableS_initToPool(cy, table, size, maxFreq, &leaf->mempool);
}

void tWaveTableS_initToPool(tWaveTableS* const cy, Lfloat* table, int size, Lfloat maxFreq, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tWaveTableS* c = *cy = (_tWaveTableS*) mpool_alloc(sizeof(_tWaveTableS), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->sampleRate = leaf->sampleRate;
    
    c->maxFreq = maxFreq;
    
    // Determine base frequency
    c->baseFreq = c->sampleRate / (Lfloat) size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    
    // Determine how many tables we need
    c->numTables = 2;
    Lfloat f = c->baseFreq;
    while (f < maxFreq)
    {
        c->numTables++;
        f *= 2.0f; // pass this multiplier in to set spacing of tables?
    }
    
    // Allocate memory for the tables
    c->tables = (Lfloat**) mpool_alloc(sizeof(Lfloat*) * c->numTables, c->mempool);
    c->sizes = (int*) mpool_alloc(sizeof(int) * c->numTables, c->mempool);
    c->sizeMasks = (int*) mpool_alloc(sizeof(int) * c->numTables, c->mempool);
    c->sizes[0] = size;
    c->sizeMasks[0] = (c->sizes[0] - 1);
    c->baseTable = (Lfloat*) mpool_alloc(sizeof(Lfloat) * c->sizes[0], c->mempool);
    c->tables[0] = c->baseTable;
    for (int t = 1; t < c->numTables; ++t)
    {
        c->sizes[t] = c->sizes[t-1] / 2 > 128 ? c->sizes[t-1] / 2 : 128;
        c->sizeMasks[t] = (c->sizes[t] - 1);
        c->tables[t] = (Lfloat*) mpool_alloc(sizeof(Lfloat) * c->sizes[t], c->mempool);
    }
    
    // Copy table
    for (int i = 0; i < c->sizes[0]; ++i)
    {
        c->baseTable[i] = table[i];
    }
    
    // Make bandlimited copies
    f = c->sampleRate * 0.25; //start at half nyquist
    // Not worth going over order 8 I think, and even 8 is only marginally better than 4.
    tButterworth_initToPool(&c->bl, 8, -1.0f, f, mp);
    tOversampler_initToPool(&c->ds, 2, 1, mp);
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
                    c->dsBuffer[0] = tButterworth_tick(&c->bl, c->tables[t-1][i*2]);
                    c->dsBuffer[1] = tButterworth_tick(&c->bl, c->tables[t-1][(i*2)+1]);
                    c->tables[t][i] = tOversampler_downsample(&c->ds, c->dsBuffer);
                }
            }
        }
        else
        {
            tButterworth_setF2(&c->bl, f);
            for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
            {
                for (int i = 0; i < c->sizes[t]; ++i)
                {
                    c->tables[t][i] = tButterworth_tick(&c->bl, c->tables[t-1][i]);
                }
            }
            f *= 0.5f; //halve the cutoff for next pass
        }
    }
    tOversampler_free(&c->ds);
    tButterworth_free(&c->bl);
}

void    tWaveTableS_free(tWaveTableS* const cy)
{
    _tWaveTableS* c = *cy;
    
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

void    tWaveTableS_setSampleRate(tWaveTableS* const cy, Lfloat sr)
{
    _tWaveTableS* c = *cy;
    
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
    c->baseFreq = c->sampleRate / (Lfloat) size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    
    // Determine how many tables we need
    c->numTables = 2;
    Lfloat f = c->baseFreq;
    while (f < c->maxFreq)
    {
        c->numTables++;
        f *= 2.0f; // pass this multiplier in to set spacing of tables?
    }
    
    // Allocate memory for the tables
    c->tables = (Lfloat**) mpool_alloc(sizeof(Lfloat*) * c->numTables, c->mempool);
    c->sizes = (int*) mpool_alloc(sizeof(int) * c->numTables, c->mempool);
    c->sizeMasks = (int*) mpool_alloc(sizeof(int) * c->numTables, c->mempool);
    c->sizes[0] = size;
    c->sizeMasks[0] = (c->sizes[0] - 1);
    c->tables[0] = c->baseTable;
    for (int t = 1; t < c->numTables; ++t)
    {
        c->sizes[t] = c->sizes[t-1] / 2 > 128 ? c->sizes[t-1] / 2 : 128;
        c->sizeMasks[t] = (c->sizes[t] - 1);
        c->tables[t] = (Lfloat*) mpool_alloc(sizeof(Lfloat) * c->sizes[t], c->mempool);
    }
    
    // Make bandlimited copies
    f = c->sampleRate * 0.25; //start at half nyquist
    // Not worth going over order 8 I think, and even 8 is only marginally better than 4.
    tButterworth_initToPool(&c->bl, 8, -1.0f, f, &c->mempool);
    tOversampler_initToPool(&c->ds, 2, 1, &c->mempool);
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
                    c->dsBuffer[0] = tButterworth_tick(&c->bl, c->tables[t-1][i*2]);
                    c->dsBuffer[1] = tButterworth_tick(&c->bl, c->tables[t-1][(i*2)+1]);
                    c->tables[t][i] = tOversampler_downsample(&c->ds, c->dsBuffer);
                }
            }
        }
        else
        {
            tButterworth_setF2(&c->bl, f);
            for (int p = 0; p < LEAF_NUM_WAVETABLE_FILTER_PASSES; ++p)
            {
                for (int i = 0; i < c->sizes[t]; ++i)
                {
                    c->tables[t][i] = tButterworth_tick(&c->bl, c->tables[t-1][i]);
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

void tWaveOscS_init(tWaveOscS* const cy, tWaveTableS* tables, int numTables, LEAF* const leaf)
{
    tWaveOscS_initToPool(cy, tables, numTables, &leaf->mempool);
}

void tWaveOscS_initToPool(tWaveOscS* const cy, tWaveTableS* tables, int numTables, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tWaveOscS* c = *cy = (_tWaveOscS*) mpool_alloc(sizeof(_tWaveOscS), m);

    c->mempool = m;

    LEAF* leaf = c->mempool->leaf;
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
    c->baseFreq = c->sampleRate / (Lfloat) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;

    c->invSampleRateTimesTwoTo32 = leaf->invSampleRate * TWO_TO_32;
    c->maxFreq = c->tables[0]->maxFreq;
}

void tWaveOscS_free(tWaveOscS* const cy)
{
    _tWaveOscS* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

volatile int errorCounter = 0;
Lfloat tWaveOscS_tick(tWaveOscS* const cy)
{
    _tWaveOscS* c = *cy;
    
    // Phasor increment (unsigned 32bit int wraps automatically with overflow so no need for if branch checks, as you need with Lfloat)
    c->phase += c->inc;
    Lfloat LfloatPhase = (double)c->phase * 2.32830643654e-10;
    Lfloat s1 = 0.f, s2 = 0.f;

    Lfloat temp;
    int idx;
    Lfloat frac;
    Lfloat samp0;
    Lfloat samp1;

    int oct = c->oct;
    
    Lfloat** tables = c->tables[c->o1]->tables;
    int* sizes = c->tables[c->o1]->sizes;
    int* sizeMasks = c->tables[c->o1]->sizeMasks;

    // Wavetable synthesis
    temp = sizes[oct] * LfloatPhase;
    idx = (int)temp;
    frac = temp - (Lfloat)idx;
    samp0 = tables[oct][idx];
    idx = (idx + 1) & sizeMasks[oct];
    samp1 = tables[oct][idx];

    Lfloat oct0 = (samp0 + (samp1 - samp0) * frac);

    temp = sizes[oct+1] * LfloatPhase;
    idx = (int)temp;
    frac = temp - (Lfloat)idx;
    samp0 = tables[oct+1][idx];
    idx = (idx + 1) & sizeMasks[oct+1];
    samp1 = tables[oct+1][idx];

    Lfloat oct1 = (samp0 + (samp1 - samp0) * frac);

    s1 = oct0 + (oct1 - oct0) * c->w;

    tables = c->tables[c->o2]->tables;
    sizes = c->tables[c->o2]->sizes;
    sizeMasks = c->tables[c->o2]->sizeMasks;

    // Wavetable synthesis
    temp = sizes[oct] * LfloatPhase;
    idx = (int)temp;
    frac = temp - (Lfloat)idx;
    samp0 = tables[oct][idx];
    idx = (idx + 1) & sizeMasks[oct];
    samp1 = tables[oct][idx];

    oct0 = (samp0 + (samp1 - samp0) * frac);

    temp = sizes[oct+1] * LfloatPhase;
    idx = (int)temp;
    frac = temp - (Lfloat)idx;
    samp0 = tables[oct+1][idx];
    idx = (idx + 1) & sizeMasks[oct+1];
    samp1 = tables[oct+1][idx];

    oct1 = (samp0 + (samp1 - samp0) * frac);

    s2 = oct0 + (oct1 - oct0) * c->w;

    // Ideally should determine correlation to get a good equal power fade between tables
    return s1 + (s2 - s1) * c->mix;
}

void tWaveOscS_setFreq(tWaveOscS* const cy, Lfloat freq)
{
    _tWaveOscS* c = *cy;

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

void tWaveOscS_setAntiAliasing(tWaveOscS* const cy, Lfloat aa)
{
    _tWaveOscS* c = *cy;
    c->aa = aa;
}

void tWaveOscS_setIndex(tWaveOscS* const cy, Lfloat index)
{
    _tWaveOscS* c = *cy;
    c->index = index;
    Lfloat f = c->index * (c->numTables - 1);

    c->o1 = (int)f;
    c->o2 = c->o1 + 1;
    if (c->index >= 1.0f) c->o2 = c->o1;
    c->mix = f - c->o1;
}

/*//// eventually gotta finish this so you can X/Y control the indices and fade between non-adjacent tables
void tWaveOscS_setIndexXY(tWaveOscS* const cy, Lfloat indexX, Lfloat indexY)
{
    _tWaveOscS* c = *cy;
    c->index = index;
    Lfloat f1 = c->index * (c->numTables - 1);

    c->o1 = (int)f1;
    c->o2 = c->o1 + 1;
    if (c->index >= 1.0f) c->o2 = c->o1;
    c->mix = f1 - c->o1;

    Lfloat f2 = c->index * (c->numTables - 1);
}
*/

void tWaveOscS_setSampleRate(tWaveOscS* const cy, Lfloat sr)
{
    _tWaveOscS* c = *cy;
    
    if (c->sampleRate == sr) return;
    
    c->sampleRate = sr;
    // Determine base frequency
    c->baseFreq = c->sampleRate / (Lfloat) c->size;
    c->invBaseFreq = 1.0f / c->baseFreq;
    c->numSubTables = c->tables[0]->numTables;
    c->invSampleRateTimesTwoTo32 = (1.f/c->sampleRate) * TWO_TO_32;
    
    tWaveOscS_setFreq(cy, c->freq);
}
//
//void tWaveOscS_setIndexTable(tWaveOscS* const cy, int i, Lfloat* table, int size)
//{
//    _tWaveOscS* c = *cy;
//    if (i >= c->numTables) return;
//    tWaveTableS_free(&c->tables[i]);
//    tWaveTableS_initToPool(&c->tables[i], table, size, c->maxFreq, &c->mempool);
//}


//beep boop adding intphasro
// Cycle
void    tIntPhasor_init(tIntPhasor* const cy, LEAF* const leaf)
{
    tIntPhasor_initToPool(cy, &leaf->mempool);
}

void    tIntPhasor_initToPool   (tIntPhasor* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tIntPhasor* c = *cy = (_tIntPhasor*) mpool_alloc(sizeof(_tIntPhasor), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->phase    =  0;
    c->inc  = 0;
    c->invSampleRateTimesTwoTo32 = (leaf->invSampleRate * TWO_TO_32);
}

void    tIntPhasor_free (tIntPhasor* const cy)
{
    _tIntPhasor* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}


Lfloat   tIntPhasor_tick(tIntPhasor* const cy)
{
    _tIntPhasor* c = *cy;
    // Phasor increment
    c->phase = (c->phase + c->inc);
    
    return c->phase * INV_TWO_TO_32; 
}

void     tIntPhasor_setFreq(tIntPhasor* const cy, Lfloat freq)
{
    _tIntPhasor* c = *cy;
    
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tIntPhasor_setPhase(tIntPhasor* const cy, Lfloat phase)
{
    _tIntPhasor* c = *cy;
    
    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32;
}

void     tIntPhasor_setSampleRate (tIntPhasor* const cy, Lfloat sr)
{
    _tIntPhasor* c = *cy;
    
    c->invSampleRateTimesTwoTo32 = (1.0f/sr) * TWO_TO_32;
    tIntPhasor_setFreq(cy, c->freq);
}

//////SQUARE(PUSHER)LFO
void    tSquareLFO_init(tSquareLFO* const cy, LEAF* const leaf)
{
    tSquareLFO_initToPool(cy, &leaf->mempool);
}

void    tSquareLFO_initToPool   (tSquareLFO* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSquareLFO* c = *cy = (_tSquareLFO*) mpool_alloc(sizeof(_tSquareLFO), m);
    c->mempool = m;
    tIntPhasor_initToPool(&c->phasor,mp);
    tIntPhasor_initToPool(&c->invPhasor,mp); 
    tSquareLFO_setPulseWidth(cy, 0.5f);
}

void    tSquareLFO_free (tSquareLFO* const cy)
{
    _tSquareLFO* c = *cy;
    tIntPhasor_free(&c->phasor);
    tIntPhasor_free(&c->invPhasor);
    mpool_free((char*)c, c->mempool);
}

//need to check bounds and wrap table properly to allow through-zero FM
Lfloat   tSquareLFO_tick(tSquareLFO* const cy)
{
    _tSquareLFO* c = *cy;
    // Phasor increment
    Lfloat a = tIntPhasor_tick(&c->phasor);
    Lfloat b = tIntPhasor_tick(&c->invPhasor);
    Lfloat tmp = ((a - b)) + c->pulsewidth - 0.5f;
    return 2 * tmp;
}

void     tSquareLFO_setFreq(tSquareLFO* const cy, Lfloat freq)
{
    _tSquareLFO* c = *cy;
    tIntPhasor_setFreq(&c->phasor,freq);
    tIntPhasor_setFreq(&c->invPhasor,freq);
}



void     tSquareLFO_setSampleRate (tSquareLFO* const cy, Lfloat sr)
{
    _tSquareLFO* c = *cy;
    tIntPhasor_setSampleRate(&c->phasor, sr);
    tIntPhasor_setSampleRate(&c->invPhasor, sr);
}

void tSquareLFO_setPulseWidth(tSquareLFO* const cy, Lfloat pw)
{
    _tSquareLFO *c = *cy;

    c->pulsewidth = pw;
    tIntPhasor_setPhase(&c->invPhasor, c->pulsewidth + (c->phasor->phase * INV_TWO_TO_32));
}

void tSquareLFO_setPhase(tSquareLFO* const cy, Lfloat phase)
{
    _tSquareLFO *c = *cy;
    tIntPhasor_setPhase(&c->phasor, phase);
    tIntPhasor_setPhase(&c->invPhasor, c->pulsewidth + (c->phasor->phase * INV_TWO_TO_32));
}

void    tSawSquareLFO_init        (tSawSquareLFO* const cy, LEAF* const leaf)
{
    tSawSquareLFO_initToPool(cy, &leaf->mempool);

}

void    tSawSquareLFO_initToPool  (tSawSquareLFO* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSawSquareLFO* c = *cy = (_tSawSquareLFO*) mpool_alloc(sizeof(_tSawSquareLFO), m);
    c->mempool = m;
    tSquareLFO_initToPool(&c->square,mp);
    tIntPhasor_initToPool(&c->saw,mp); 
}
void    tSawSquareLFO_free        (tSawSquareLFO* const cy)
{
    _tSawSquareLFO* c = *cy;
    tIntPhasor_free(&c->saw);
    tSquareLFO_free(&c->square);
    mpool_free((char*)c, c->mempool);
}
    
Lfloat   tSawSquareLFO_tick        (tSawSquareLFO* const cy)
{
    _tSawSquareLFO* c = *cy;
    Lfloat a = (tIntPhasor_tick(&c->saw) - 0.5f ) * 2.0f;
    Lfloat b = tSquareLFO_tick(&c->square);
    return  (1 - c->shape) * a + c->shape * b; 
}
void    tSawSquareLFO_setFreq     (tSawSquareLFO* const cy, Lfloat freq)
{
    _tSawSquareLFO* c = *cy;
    tSquareLFO_setFreq(&c->square, freq);
    tIntPhasor_setFreq(&c->saw, freq);
}
void    tSawSquareLFO_setSampleRate (tSawSquareLFO* const cy, Lfloat sr)
{
    _tSawSquareLFO* c = *cy;
    tSquareLFO_setSampleRate(&c->square, sr);
    tIntPhasor_setSampleRate(&c->saw, sr);
}
void    tSawSquareLFO_setPhase (tSawSquareLFO* const cy, Lfloat phase)
{
    _tSawSquareLFO* c = *cy;
    tSquareLFO_setPhase(&c->square, phase);
    tIntPhasor_setPhase(&c->saw, phase);
}


void    tSawSquareLFO_setShape (tSawSquareLFO* const cy, Lfloat shape)
{
    _tSawSquareLFO* c = *cy;
    c->shape = shape; 
}




///tri
void    tTriLFO_init(tTriLFO* const cy, LEAF* const leaf)
{
    tTriLFO_initToPool(cy, &leaf->mempool);
}

void    tTriLFO_initToPool   (tTriLFO* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTriLFO* c = *cy = (_tTriLFO*) mpool_alloc(sizeof(_tTriLFO), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    
    c->inc      =  0;
    c->phase    =  0;
    c->invSampleRate = leaf->invSampleRate;
    c->invSampleRateTimesTwoTo32 = (c->invSampleRate * TWO_TO_32);
    tTriLFO_setFreq(cy, 220.0f);
}

void    tTriLFO_free (tTriLFO* const cy)
{
    _tTriLFO* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

//need to check bounds and wrap table properly to allow through-zero FM
Lfloat   tTriLFO_tick(tTriLFO* const cy)
{
    _tTriLFO* c = *cy;
    c->phase += c->inc;
    
    //bitmask fun
    int32_t shiftedPhase = c->phase + 1073741824; // offset by 1/4" wave by adding 2^30 to get things in phase with the other LFO oscillators
    uint32_t mask = shiftedPhase >> 31; //get the sign bit
    shiftedPhase = shiftedPhase + mask; // add 1 if negative, zero if positive, to balance
    shiftedPhase = shiftedPhase ^ mask; //invert the value to get absolute value of integer
    Lfloat output = (((Lfloat)shiftedPhase * INV_TWO_TO_31) - 0.5f) * 2.0f; //scale it to -1.0f to 1.0f Lfloat
    return output;

}

void     tTriLFO_setFreq(tTriLFO* const cy, Lfloat freq)
{
    _tTriLFO* c = *cy;
    
    c->freq  = freq;
    c->inc = freq * c->invSampleRateTimesTwoTo32;
}

void    tTriLFO_setPhase(tTriLFO* const cy, Lfloat phase)
{
    _tTriLFO* c = *cy;
    int i = phase;
    phase -= i;
    c->phase = phase * TWO_TO_32_INT;
}

void     tTriLFO_setSampleRate (tTriLFO* const cy, Lfloat sr)
{
    _tTriLFO* c = *cy;
    c->invSampleRate = (1.0f/sr);
    c->invSampleRateTimesTwoTo32 = c->invSampleRate * TWO_TO_32;
    tTriLFO_setFreq(cy, c->freq);
}
///sinetri

void    tSineTriLFO_init        (tSineTriLFO* const cy, LEAF* const leaf)
{
    tSineTriLFO_initToPool(cy, &leaf->mempool);
}

void    tSineTriLFO_initToPool  (tSineTriLFO* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSineTriLFO* c = *cy = (_tSineTriLFO*) mpool_alloc(sizeof(_tSineTriLFO), m);
    c->mempool = m;
    tTriLFO_initToPool(&c->tri,mp);
    tCycle_initToPool(&c->sine,mp); 
   
}
void    tSineTriLFO_free        (tSineTriLFO* const cy)
{
    _tSineTriLFO* c = *cy;
    tCycle_free(&c->sine);
    tTriLFO_free(&c->tri);
    mpool_free((char*)c, c->mempool);
}
    
Lfloat   tSineTriLFO_tick        (tSineTriLFO* const cy)
{
    _tSineTriLFO* c = *cy;
    Lfloat a = tCycle_tick(&c->sine);
    Lfloat b = tTriLFO_tick(&c->tri);
    return  (1.0f - c->shape) * a + c->shape * b;
}
void    tSineTriLFO_setFreq     (tSineTriLFO* const cy, Lfloat freq)
{
    _tSineTriLFO* c = *cy;
    tTriLFO_setFreq(&c->tri, freq);
    tCycle_setFreq(&c->sine, freq);
}
void    tSineTriLFO_setSampleRate (tSineTriLFO* const cy, Lfloat sr)
{
    _tSineTriLFO* c = *cy;
    tTriLFO_setSampleRate(&c->tri, sr);
    tCycle_setSampleRate(&c->sine, sr);
}
void    tSineTriLFO_setPhase (tSineTriLFO* const cy, Lfloat phase)
{
    _tSineTriLFO* c = *cy;
    tTriLFO_setPhase(&c->tri, phase);
    tCycle_setPhase(&c->sine, phase);
}

 void    tSineTriLFO_setShape (tSineTriLFO* const cy, Lfloat shape)
 {
    _tSineTriLFO* c = *cy;
    c->shape = shape;

 }




 void    tDampedOscillator_init        (tDampedOscillator* const cy, LEAF* const leaf)
 {
	 tDampedOscillator_initToPool(cy, &leaf->mempool);
 }

 void    tDampedOscillator_initToPool  (tDampedOscillator* const cy, tMempool* const mp)
 {
     _tMempool* m = *mp;
     _tDampedOscillator* c = *cy = (_tDampedOscillator*) mpool_alloc(sizeof(_tDampedOscillator), m);
     c->mempool = m;
     LEAF* leaf = c->mempool->leaf;


     c->freq_ = 0.0f;
     c->decay_ = 1.0f;

     tDampedOscillator_setSampleRate(cy, leaf->sampleRate);
     c->loop_gain_ = cosf(c->freq_ * c->two_pi_by_sample_rate_);
     	  Lfloat g = sqrtf((1.0f - c->loop_gain_) / (1.0f + c->loop_gain_));
    c->turns_ratio_ = g;
    tDampedOscillator_reset(cy);

 }
 void    tDampedOscillator_free        (tDampedOscillator* const cy)
 {
	 _tDampedOscillator* c = *cy;

     mpool_free((char*)c, c->mempool);
 }

 Lfloat   tDampedOscillator_tick        (tDampedOscillator* const cy)
 {
	 _tDampedOscillator* c = *cy;
	   Lfloat w = c->decay_ * c->x_;
	   Lfloat z = c->loop_gain_ * (c->y_ + w);
	   c->x_ = z - c->y_;
	   c->y_ = z + w;
	   return c->y_;
 }
 void    tDampedOscillator_setFreq     (tDampedOscillator* const cy, Lfloat freq_hz)
 {
	 _tDampedOscillator* c = *cy;

	  c->freq_ = freq_hz;

#ifdef ARM_MATH_CM7
	  c->loop_gain_ = arm_cos_f32(freq_hz * c->two_pi_by_sample_rate_);
	  Lfloat g = 1.0f;
	  arm_sqrt_f32((1.0f - c->loop_gain_) / (1.0f + c->loop_gain_), &g);
#else
	  c->loop_gain_ = cosf(freq_hz * c->two_pi_by_sample_rate_);
	  Lfloat g = sqrtf((1.0f - c->loop_gain_) / (1.0f + c->loop_gain_));
#endif
	  // scale state variable in preparation for the next step
	  c->x_ *= g / c->turns_ratio_;
	  c->turns_ratio_ = g;

 }

 void    tDampedOscillator_setDecay    (tDampedOscillator* const cy, Lfloat decay)
 {
	 _tDampedOscillator* c = *cy;


	 Lfloat r = fastExp4(-decay * c->two_pi_by_sample_rate_);


	 c->decay_ = r * r;

 }

 void    tDampedOscillator_setSampleRate (tDampedOscillator* const cy, Lfloat sr)
 {
	 _tDampedOscillator* c = *cy;
	 c->two_pi_by_sample_rate_ = TWO_PI / sr;
 }


  void    tDampedOscillator_reset (tDampedOscillator* const cy)
  {
	  _tDampedOscillator* c = *cy;
	  c->x_ = c->turns_ratio_;
	  c->y_ = 0.0f;

  }

