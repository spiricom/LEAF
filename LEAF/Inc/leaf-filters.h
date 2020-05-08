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
#include "leaf-mempool.h"
#include "leaf-delay.h"
#include "leaf-tables.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    /* tAllpass: Schroeder allpass. Comb-filter with feedforward and feedback. */
    typedef struct _tAllpass
    {
        float gain;
        
        tLinearDelay delay;
        
        float lastOut;
        
    } _tAllpass;
    
    typedef _tAllpass* tAllpass;
    
    void    tAllpass_init           (tAllpass* const, float initDelay, uint32_t maxDelay);
    void    tAllpass_free           (tAllpass* const);
    void    tAllpass_initToPool     (tAllpass* const, float initDelay, uint32_t maxDelay, tMempool* const);
    void    tAllpass_freeFromPool   (tAllpass* const, tMempool* const);
    
    float   tAllpass_tick           (tAllpass* const, float input);
    void    tAllpass_setGain        (tAllpass* const, float gain);
    void    tAllpass_setDelay       (tAllpass* const, float delay);
    
    
    //==============================================================================
    
    /* tOnePole: OnePole filter, reimplemented from STK (Cook and Scavone). */
    typedef struct _tOnePole
    {
        float gain;
        float a0,a1;
        float b0,b1;
        float lastIn, lastOut;

        
    } _tOnePole;
    
    typedef _tOnePole* tOnePole;
    
    void    tOnePole_init           (tOnePole* const, float thePole);
    void    tOnePole_free           (tOnePole* const);
    void    tOnePole_initToPool     (tOnePole* const, float thePole, tMempool* const);
    void    tOnePole_freeFromPool   (tOnePole* const, tMempool* const);
    
    float   tOnePole_tick           (tOnePole* const, float input);
    void    tOnePole_setB0          (tOnePole* const, float b0);
    void    tOnePole_setA1          (tOnePole* const, float a1);
    void    tOnePole_setPole        (tOnePole* const, float thePole);
    void    tOnePole_setFreq        (tOnePole* const, float freq);
    void    tOnePole_setCoefficients(tOnePole* const, float b0, float a1);
    void    tOnePole_setGain        (tOnePole* const, float gain);
    
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
        
    } _tTwoPole;
    
    typedef _tTwoPole* tTwoPole;
    
    void    tTwoPole_init           (tTwoPole* const);
    void    tTwoPole_free           (tTwoPole* const);
    void    tTwoPole_initToPool     (tTwoPole* const, tMempool* const);
    void    tTwoPole_freeFromPool   (tTwoPole* const, tMempool* const);
    
    float   tTwoPole_tick           (tTwoPole* const, float input);
    void    tTwoPole_setB0          (tTwoPole* const, float b0);
    void    tTwoPole_setA1          (tTwoPole* const, float a1);
    void    tTwoPole_setA2          (tTwoPole* const, float a2);
    void    tTwoPole_setResonance   (tTwoPole* const, float freq, float radius, oBool normalize);
    void    tTwoPole_setCoefficients(tTwoPole* const, float b0, float a1, float a2);
    void    tTwoPole_setGain        (tTwoPole* const, float gain);
    
    //==============================================================================
    
    /* OneZero filter, reimplemented from STK (Cook and Scavone). */
    typedef struct _tOneZero
    {
        float gain;
        float b0,b1;
        float lastIn, lastOut, frequency;
        
    } _tOneZero;
    
    typedef _tOneZero* tOneZero;
    
    void    tOneZero_init           (tOneZero* const, float theZero);
    void    tOneZero_free           (tOneZero* const);
    void    tOneZero_initToPool     (tOneZero* const, float theZero, tMempool* const);
    void    tOneZero_freeFromPool   (tOneZero* const, tMempool* const);
    
    float   tOneZero_tick           (tOneZero* const, float input);
    void    tOneZero_setB0          (tOneZero* const, float b0);
    void    tOneZero_setB1          (tOneZero* const, float b1);
    void    tOneZero_setZero        (tOneZero* const, float theZero);
    void    tOneZero_setCoefficients(tOneZero* const, float b0, float b1);
    void    tOneZero_setGain        (tOneZero* const, float gain);
    float   tOneZero_getPhaseDelay  (tOneZero *f, float frequency );
    
    //==============================================================================
    
    /* TwoZero filter, reimplemented from STK (Cook and Scavone). */
    typedef struct _tTwoZero
    {
        float gain;
        float b0, b1, b2;
        
        float frequency, radius;
        
        float lastIn[2];
        
    } _tTwoZero;
    
    typedef _tTwoZero* tTwoZero;
    
    void    tTwoZero_init           (tTwoZero* const);
    void    tTwoZero_free           (tTwoZero* const);
    void    tTwoZero_initToPool     (tTwoZero* const, tMempool* const);
    void    tTwoZero_freeFromPool   (tTwoZero* const, tMempool* const);
    
    float   tTwoZero_tick           (tTwoZero* const, float input);
    void    tTwoZero_setB0          (tTwoZero* const, float b0);
    void    tTwoZero_setB1          (tTwoZero* const, float b1);
    void    tTwoZero_setB2          (tTwoZero* const, float b2);
    void    tTwoZero_setNotch       (tTwoZero* const, float frequency, float radius);
    void    tTwoZero_setCoefficients(tTwoZero* const, float b0, float b1, float b2);
    void    tTwoZero_setGain        (tTwoZero* const, float gain);
    
    //==============================================================================
    
    /* PoleZero filter, reimplemented from STK (Cook and Scavone). */
    typedef struct _tPoleZero
    {
        float gain;
        float a0,a1;
        float b0,b1;
        
        float lastIn, lastOut;
        
    } _tPoleZero;
    
    typedef _tPoleZero* tPoleZero;
    
    void    tPoleZero_init              (tPoleZero* const);
    void    tPoleZero_free              (tPoleZero* const);
    void    tPoleZero_initToPool        (tPoleZero* const, tMempool* const);
    void    tPoleZero_freeFromPool      (tPoleZero* const, tMempool* const);
    
    float   tPoleZero_tick              (tPoleZero* const, float input);
    void    tPoleZero_setB0             (tPoleZero* const, float b0);
    void    tPoleZero_setB1             (tPoleZero* const, float b1);
    void    tPoleZero_setA1             (tPoleZero* const, float a1);
    void    tPoleZero_setCoefficients   (tPoleZero* const, float b0, float b1, float a1);
    void    tPoleZero_setAllpass        (tPoleZero* const, float coeff);
    void    tPoleZero_setBlockZero      (tPoleZero* const, float thePole);
    void    tPoleZero_setGain           (tPoleZero* const, float gain);
    
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
    } _tBiQuad;
    
    typedef _tBiQuad* tBiQuad;
    
    void    tBiQuad_init           (tBiQuad* const);
    void    tBiQuad_free           (tBiQuad* const);
    void    tBiQuad_initToPool     (tBiQuad* const, tMempool* const);
    void    tBiQuad_freeFromPool   (tBiQuad* const, tMempool* const);
    
    float   tBiQuad_tick           (tBiQuad* const, float input);
    void    tBiQuad_setB0          (tBiQuad* const, float b0);
    void    tBiQuad_setB1          (tBiQuad* const, float b1);
    void    tBiQuad_setB2          (tBiQuad* const, float b2);
    void    tBiQuad_setA1          (tBiQuad* const, float a1);
    void    tBiQuad_setA2          (tBiQuad* const, float a2);
    void    tBiQuad_setNotch       (tBiQuad* const, float freq, float radius);
    void    tBiQuad_setResonance   (tBiQuad* const, float freq, float radius, oBool normalize);
    void    tBiQuad_setCoefficients(tBiQuad* const, float b0, float b1, float b2, float a1, float a2);
    void    tBiQuad_setGain        (tBiQuad* const, float gain);
    
    //==============================================================================
    
/* State Variable Filter, algorithm from Andy Simper. */
    typedef enum SVFType
    {
        SVFTypeHighpass = 0,
        SVFTypeLowpass,
        SVFTypeBandpass,
        SVFTypeNotch,
        SVFTypePeak,
        SVFTypeLowShelf,
        SVFTypeHighShelf
    } SVFType;
    
    typedef struct _tSVF
    {
        SVFType type;
        float cutoff, Q;
        float ic1eq,ic2eq;
        float g,k,a1,a2,a3,cH,cB,cL,cBK;

        
    } _tSVF;
    
    typedef _tSVF* tSVF;
    
    void    tSVF_init           (tSVF* const, SVFType type, float freq, float Q);
    void    tSVF_free           (tSVF* const);
    void    tSVF_initToPool     (tSVF* const, SVFType type, float freq, float Q, tMempool* const);
    void    tSVF_freeFromPool   (tSVF* const, tMempool* const);
    
    float   tSVF_tick           (tSVF* const, float v0);
    void    tSVF_setFreq        (tSVF* const, float freq);
    void    tSVF_setQ           (tSVF* const, float Q);
    void    tSVF_setFreqAndQ    (tSVF* const svff, float freq, float Q);
    //==============================================================================
    
    /* Efficient State Variable Filter for 14-bit control input, [0, 4096). */
    typedef struct _tEfficientSVF
    {
        SVFType type;
        float cutoff, Q;
        float ic1eq,ic2eq;
        float g,k,a1,a2,a3;
        
    } _tEfficientSVF;
    
    typedef _tEfficientSVF* tEfficientSVF;
    
    void    tEfficientSVF_init          (tEfficientSVF* const, SVFType type, uint16_t input, float Q);
    void    tEfficientSVF_free          (tEfficientSVF* const);
    void    tEfficientSVF_initToPool    (tEfficientSVF* const, SVFType type, uint16_t input, float Q, tMempool* const);
    void    tEfficientSVF_freeFromPool  (tEfficientSVF* const, tMempool* const);
    
    float   tEfficientSVF_tick          (tEfficientSVF* const, float v0);
    void    tEfficientSVF_setFreq       (tEfficientSVF* const, uint16_t controlFreq);
    void    tEfficientSVF_setQ          (tEfficientSVF* const, float Q);
    
    //==============================================================================
    
    /* Simple Highpass filter. */
    typedef struct _tHighpass
    {
        float xs, ys, R;
        float frequency;
        
    } _tHighpass;
    
    typedef _tHighpass* tHighpass;
    
    void    tHighpass_init          (tHighpass* const, float freq);
    void    tHighpass_free          (tHighpass* const);
    void    tHighpass_initToPool    (tHighpass* const, float freq, tMempool* const);
    void    tHighpass_freeFromPool  (tHighpass* const, tMempool* const);
    
    float   tHighpass_tick          (tHighpass* const, float x);
    void    tHighpass_setFreq       (tHighpass* const, float freq);
    float   tHighpass_getFreq       (tHighpass* const);
    
    //==============================================================================
    
    // Butterworth Filter
#define NUM_SVF_BW 16
    typedef struct _tButterworth
    {
        float gain;
        
        int N;
        
        tSVF low[NUM_SVF_BW];
        tSVF high[NUM_SVF_BW];
        
        float f1,f2;
        
    } _tButterworth;
    
    typedef _tButterworth* tButterworth;
    
    void    tButterworth_init           (tButterworth* const, int N, float f1, float f2);
    void    tButterworth_free           (tButterworth* const);
    void    tButterworth_initToPool     (tButterworth* const, int N, float f1, float f2, tMempool* const);
    void    tButterworth_freeFromPool   (tButterworth* const, tMempool* const);
    
    float   tButterworth_tick           (tButterworth* const, float input);
    void    tButterworth_setF1          (tButterworth* const, float in);
    void    tButterworth_setF2          (tButterworth* const, float in);
    void    tButterworth_setFreqs       (tButterworth* const, float f1, float f2);
    
    //==============================================================================
    
    typedef struct _tFIR
    {
        float* past;
        float* coeff;
        int numTaps;
    } _tFIR;
    
    typedef _tFIR* tFIR;
    
    void    tFIR_init           (tFIR* const, float* coeffs, int numTaps);
    void    tFIR_free           (tFIR* const);
    void    tFIR_initToPool     (tFIR* const, float* coeffs, int numTaps, tMempool* const);
    void    tFIR_freeFromPool   (tFIR* const, tMempool* const);
    
    float   tFIR_tick           (tFIR* const, float input);

    
    //==============================================================================
    

    typedef struct _tMedianFilter
    {
    	float* val;
    	int* age;
    	int m;
    	int size;
    	int middlePosition;
    	int last;
    	int pos;
    } _tMedianFilter;

    typedef _tMedianFilter* tMedianFilter;

    void    tMedianFilter_init           (tMedianFilter* const, int size);
	void    tMedianFilter_free           (tMedianFilter* const);
	void    tMedianFilter_initToPool     (tMedianFilter* const, int size, tMempool* const);
	void    tMedianFilter_freeFromPool   (tMedianFilter* const, tMempool* const);

	float   tMedianFilter_tick           (tMedianFilter* const, float input);





	//Vadim Zavalishin style from VA book (from implementation in RSlib posted to kvr forum)

    typedef enum VZFilterType
    {
        Highpass = 0,
        Lowpass,
        BandpassSkirt,
        BandpassPeak,
        BandReject,
        Bell,
        Lowshelf,
        Highshelf,
        Morph,
        Bypass,
        Allpass
    } VZFilterType;
    
    typedef struct _tVZFilter
    {
        VZFilterType type;
        // state:
        float s1, s2;
        
        // filter coefficients:
        float g;          // embedded integrator gain
        float R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        float h;          // factor for feedback (== 1/(1+2*R*g+g*g))
        float cL, cB, cH; // coefficients for low-, band-, and highpass signals
        
        // parameters:
        float fs;    // sample-rate
        float fc;    // characteristic frequency
        float G;     // gain
        float invG;     //1/gain
        float B;     // bandwidth (in octaves)
        float m;     // morph parameter (0...1)
        
        float sr;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        float inv_sr;
        
    } _tVZFilter;
    
    typedef _tVZFilter* tVZFilter;
    
    void    tVZFilter_init           (tVZFilter* const, VZFilterType type, float freq, float Q);
    void    tVZFilter_free           (tVZFilter* const);
    void    tVZFilter_initToPool     (tVZFilter* const, VZFilterType type, float freq, float Q, tMempool* const);
    void    tVZFilter_freeFromPool   (tVZFilter* const, tMempool* const);
    void    tVZFilter_setSampleRate  (tVZFilter* const, float sampleRate);
    float   tVZFilter_tick              (tVZFilter* const, float input);
    float   tVZFilter_tickEfficient             (tVZFilter* const vf, float in);
    void   tVZFilter_calcCoeffs           (tVZFilter* const);
    void   tVZFilter_setBandwidth           (tVZFilter* const, float bandWidth);
    void   tVZFilter_setFreq           (tVZFilter* const, float freq);
    void   tVZFilter_setFreqAndBandwidth    (tVZFilter* const vf, float freq, float bw);
    void   tVZFilter_setGain                (tVZFilter* const, float gain);
    void   tVZFilter_setType                (tVZFilter* const, VZFilterType type);
    float   tVZFilter_BandwidthToR      (tVZFilter* const vf, float B);
    
    
    
    
    //diode ladder filter
    typedef struct _tDiodeFilter
    {
        
        float f;
        float r;
        float Vt;
        float n;
        float gamma;
        float zi;
        float g0inv;
        float g1inv;
        float g2inv;
        float s0, s1, s2, s3;
        
    } _tDiodeFilter;
    
    typedef _tDiodeFilter* tDiodeFilter;
    
    void    tDiodeFilter_init           (tDiodeFilter* const, float freq, float Q);
    void    tDiodeFilter_free           (tDiodeFilter* const);
    void    tDiodeFilter_initToPool     (tDiodeFilter* const, float freq, float Q, tMempool* const);
    void    tDiodeFilter_freeFromPool   (tDiodeFilter* const, tMempool* const);
    
    float   tDiodeFilter_tick               (tDiodeFilter* const, float input);
    void    tDiodeFilter_setFreq     (tDiodeFilter* const vf, float cutoff);
    void    tDiodeFilter_setQ     (tDiodeFilter* const vf, float resonance);


#ifdef __cplusplus
}
#endif

#endif  // LEAF_FILTERS_H_INCLUDED

//==============================================================================

