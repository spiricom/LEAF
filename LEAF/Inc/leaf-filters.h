/*==============================================================================

    leaf-filters.h
    Created: 20 Jan 2017 12:01:10pm
    Author:  Michael R Mulshine

==============================================================================*/

#ifndef LEAF_FILTERS_H_INCLUDED
#define LEAF_FILTERS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
    
#include "leaf-math.h"
#include "leaf-delay.h"
#include "leaf-tables.h"

//==============================================================================
    
/* tAllpass: Schroeder allpass. Comb-filter with feedforward and feedback. */
typedef struct _tAllpass
{
    float gain;
    
    tLinearDelay delay;
    
    float lastOut;
    
} tAllpass;

void        tAllpass_init           (tAllpass* const, float initDelay, uint32_t maxDelay);
void        tAllpass_free           (tAllpass* const);

float       tAllpass_tick           (tAllpass* const, float input);
void        tAllpass_setGain        (tAllpass* const, float gain);
void        tAllpass_setDelay       (tAllpass* const f, float delay);


//==============================================================================
    
/* tOnePole: OnePole filter, reimplemented from STK (Cook and Scavone). */
typedef struct _tOnePole
{
    float gain;
    float a0,a1;
    float b0,b1;
    
    float coef;
    
    float freq;
    
    float lastIn, lastOut;
    
} tOnePole;

void        tOnePole_init           (tOnePole* const, float thePole);
void        tOnePole_free           (tOnePole* const);

float       tOnePole_tick           (tOnePole* const, float input);
void        tOnePole_setB0          (tOnePole* const, float b0);
void        tOnePole_setA1          (tOnePole* const, float a1);
void        tOnePole_setPole        (tOnePole* const, float thePole);
void        tOnePole_setFreq        (tOnePole* const, float freq);
void        tOnePole_setCoefficients(tOnePole* const, float b0, float a1);
void        tOnePole_setGain        (tOnePole* const, float gain);

//==============================================================================
    
/* TwoPole filter, reimplemented from STK (Cook and Scavone). */
typedef struct _tTwoPole
{
    float gain;
    float a0, a1, a2;
    float b0;
    
    float radius, frequency;
    oBool normalize;
    
    float lastOut[2];
    
} tTwoPole;

void        tTwoPole_init           (tTwoPole*  const);
void        tTwoPole_free           (tTwoPole*  const);

float       tTwoPole_tick           (tTwoPole*  const, float input);
void        tTwoPole_setB0          (tTwoPole*  const, float b0);
void        tTwoPole_setA1          (tTwoPole*  const, float a1);
void        tTwoPole_setA2          (tTwoPole*  const, float a2);
void        tTwoPole_setResonance   (tTwoPole*  const, float freq, float radius, oBool normalize);
void        tTwoPole_setCoefficients(tTwoPole*  const, float b0, float a1, float a2);
void        tTwoPole_setGain        (tTwoPole*  const, float gain);

//==============================================================================
    
/* OneZero filter, reimplemented from STK (Cook and Scavone). */
typedef struct _tOneZero
{
    float gain;
    float b0,b1;
    float lastIn, lastOut, frequency;
    
} tOneZero;

void        tOneZero_init           (tOneZero*  const, float theZero);
void        tOneZero_free           (tOneZero*  const);
float       tOneZero_tick           (tOneZero*  const, float input);
void        tOneZero_setB0          (tOneZero*  const, float b0);
void        tOneZero_setB1          (tOneZero*  const, float b1);
void        tOneZero_setZero        (tOneZero*  const, float theZero);
void        tOneZero_setCoefficients(tOneZero*  const, float b0, float b1);
void        tOneZero_setGain        (tOneZero*  const, float gain);
float       tOneZero_getPhaseDelay(tOneZero *f, float frequency );

//==============================================================================
    
/* TwoZero filter, reimplemented from STK (Cook and Scavone). */
typedef struct _tTwoZero
{
    float gain;
    float b0, b1, b2;
    
    float frequency, radius;
    
    float lastIn[2];
    
} tTwoZero;

void        tTwoZero_init           (tTwoZero*  const);
void        tTwoZero_free           (tTwoZero*  const);

float       tTwoZero_tick           (tTwoZero*  const, float input);
void        tTwoZero_setB0          (tTwoZero*  const, float b0);
void        tTwoZero_setB1          (tTwoZero*  const, float b1);
void        tTwoZero_setB2          (tTwoZero*  const, float b2);
void        tTwoZero_setNotch       (tTwoZero*  const, float frequency, float radius);
void        tTwoZero_setCoefficients(tTwoZero*  const, float b0, float b1, float b2);
void        tTwoZero_setGain        (tTwoZero*  const, float gain);

//==============================================================================
    
/* PoleZero filter, reimplemented from STK (Cook and Scavone). */
typedef struct _tPoleZero
{
    float gain;
    float a0,a1;
    float b0,b1;
    
    float lastIn, lastOut;
    
} tPoleZero;

void        tPoleZero_init              (tPoleZero*  const);
void        tPoleZero_free              (tPoleZero*  const);

float       tPoleZero_tick              (tPoleZero*  const, float input);
void        tPoleZero_setB0             (tPoleZero*  const, float b0);
void        tPoleZero_setB1             (tPoleZero*  const, float b1);
void        tPoleZero_setA1             (tPoleZero*  const, float a1);
void        tPoleZero_setCoefficients   (tPoleZero*  const, float b0, float b1, float a1);
void        tPoleZero_setAllpass        (tPoleZero*  const, float coeff);
void        tPoleZero_setBlockZero      (tPoleZero*  const, float thePole);
void        tPoleZero_setGain           (tPoleZero*  const, float gain);

//==============================================================================
    
/* BiQuad filter, reimplemented from STK (Cook and Scavone). */
typedef struct _tBiQuad
{
    float gain;
    float a0, a1, a2;
    float b0, b1, b2;
    
    float lastIn[2];
    float lastOut[2];
    
    float frequency, radius;
    oBool normalize;
} tBiQuad;

void        tBiQuad_init           (tBiQuad*  const);
void        tBiQuad_free           (tBiQuad*  const);

float       tBiQuad_tick           (tBiQuad*  const, float input);
void        tBiQuad_setB0          (tBiQuad*  const, float b0);
void        tBiQuad_setB1          (tBiQuad*  const, float b1);
void        tBiQuad_setB2          (tBiQuad*  const, float b2);
void        tBiQuad_setA1          (tBiQuad*  const, float a1);
void        tBiQuad_setA2          (tBiQuad*  const, float a2);
void        tBiQuad_setNotch       (tBiQuad*  const, float freq, float radius);
void        tBiQuad_setResonance   (tBiQuad*  const, float freq, float radius, oBool normalize);
void        tBiQuad_setCoefficients(tBiQuad*  const, float b0, float b1, float b2, float a1, float a2);
void        tBiQuad_setGain        (tBiQuad*  const, float gain);

//==============================================================================
    
/* State Variable Filter, algorithm from Andy Simper. */
typedef enum SVFType
{
    SVFTypeHighpass = 0,
    SVFTypeLowpass,
    SVFTypeBandpass,
    SVFTypeNotch,
    SVFTypePeak,
} SVFType;

typedef struct _tSVF
{
    SVFType type;
    float cutoff, Q;
    float ic1eq,ic2eq;
    float g,k,a1,a2,a3;
    
} tSVF;

void        tSVF_init       (tSVF*  const, SVFType type, float freq, float Q);
void        tSVF_free       (tSVF*  const);

float       tSVF_tick       (tSVF*  const, float v0);
int         tSVF_setFreq    (tSVF*  const, float freq);
int         tSVF_setQ       (tSVF*  const, float Q);

//==============================================================================
    
/* Efficient State Variable Filter for 14-bit control input, [0, 4096). */
typedef struct _tEfficientSVF
{
    SVFType type;
    float cutoff, Q;
    float ic1eq,ic2eq;
    float g,k,a1,a2,a3;
    
} tEfficientSVF;

void        tEfficientSVF_init      (tEfficientSVF*  const, SVFType type, uint16_t controlFreq, float Q);
void        tEfficientSVF_free      (tEfficientSVF*  const);

float       tEfficientSVF_tick      (tEfficientSVF*  const, float v0);
int         tEfficientSVF_setFreq   (tEfficientSVF*  const, uint16_t controlFreq);
int         tEfficientSVF_setQ      (tEfficientSVF*  const, float Q);

//==============================================================================
    
/* Simple Highpass filter. */
typedef struct _tHighpass
{
    float xs, ys, R;
    float frequency;
    
} tHighpass;

void        tHighpass_init      (tHighpass*  const, float freq);
void        tHighpass_free      (tHighpass*  const);

float       tHighpass_tick      (tHighpass*  const, float x);
void        tHighpass_setFreq   (tHighpass*  const, float freq);
float       tHighpass_getFreq   (tHighpass*  const);

//==============================================================================
    
// Butterworth Filter
#define NUM_SVF_BW 16
typedef struct _tButterworth
{
    float gain;
    
    float N;
    
    tSVF low[NUM_SVF_BW];
    tSVF high[NUM_SVF_BW];
    
    float f1,f2;
    
} tButterworth;

void        tButterworth_init       (tButterworth* const, int N, float f1, float f2);
void        tButterworth_free       (tButterworth* const);

float       tButterworth_tick       (tButterworth* const, float input);
void        tButterworth_setF1      (tButterworth* const, float in);
void        tButterworth_setF2      (tButterworth* const, float in);
void        tButterworth_setFreqs   (tButterworth* const, float f1, float f2);

//==============================================================================

typedef struct _tFIR
{
    float* past;
    float* coeff;
    int numTaps;

} tFIR;

void        tFIR_init       (tFIR* const, float* coeffs, int numTaps);
void        tFIR_free       (tFIR* const);

float       tFIR_tick       (tFIR* const, float input);
void        tFIR_coeffs      (tFIR* const, float in);

//==============================================================================

#ifdef __cplusplus
}
#endif

#endif  // LEAF_FILTERS_H_INCLUDED

//==============================================================================
