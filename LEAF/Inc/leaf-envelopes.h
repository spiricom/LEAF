/*
 ==============================================================================
 
 leaf-envelopes.h
 Created: 20 Jan 2017 12:02:17pm
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#ifndef LEAF_ENVELOPES_H_INCLUDED
#define LEAF_ENVELOPES_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-filters.h"
#include "leaf-delay.h"
#include "leaf-analysis.h"
#include "leaf-envelopes.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */

    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /* Attack-Decay envelope */
    typedef struct _tEnvelope {
        
       const float *exp_buff;
       const float *inc_buff;
        uint32_t buff_size;
        
        float next;
        
        float attackInc, decayInc, rampInc;
        
        oBool inAttack, inDecay, inRamp;
        
        oBool loop;
        
        float gain, rampPeak;
        
        float attackPhase, decayPhase, rampPhase;
        
    } _tEnvelope;
    
    typedef _tEnvelope* tEnvelope;
    
    void    tEnvelope_init          (tEnvelope* const, float attack, float decay, oBool loop);
    void    tEnvelope_free          (tEnvelope* const);
    void    tEnvelope_initToPool    (tEnvelope* const, float attack, float decay, oBool loop, tMempool* const);
    void    tEnvelope_freeFromPool  (tEnvelope* const, tMempool* const);
    
    float   tEnvelope_tick          (tEnvelope* const);
    void    tEnvelope_setAttack     (tEnvelope* const, float attack);
    void    tEnvelope_setDecay      (tEnvelope* const, float decay);
    void    tEnvelope_loop          (tEnvelope* const, oBool loop);
    void    tEnvelope_on            (tEnvelope* const, float velocity);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    


    /* Exponential Smoother */
    typedef struct _tExpSmooth {
        float factor, oneminusfactor;
        float curr,dest;

    } _tExpSmooth;

    typedef _tExpSmooth* tExpSmooth;

    void    tExpSmooth_init         (tExpSmooth* const, float val, float factor);
    void    tExpSmooth_free         (tExpSmooth* const);
    void    tExpSmooth_initToPool   (tExpSmooth* const, float val, float factor, tMempool* const);
    void    tExpSmooth_freeFromPool (tExpSmooth* const, tMempool* const);

    float   tExpSmooth_tick         (tExpSmooth* const);
    float   tExpSmooth_sample       (tExpSmooth* const);
    void    tExpSmooth_setFactor    (tExpSmooth* const, float factor);
    void    tExpSmooth_setDest      (tExpSmooth* const, float dest);
    void    tExpSmooth_setVal       (tExpSmooth* const, float val);

    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~


    /* ADSR */
    typedef struct _tADSR
    {
       const float *exp_buff;
       const float *inc_buff;
        uint32_t buff_size;
        
        float next;
        
        float attackInc, decayInc, releaseInc, rampInc;
        
        oBool inAttack, inDecay, inSustain, inRelease, inRamp;
        
        float sustain, gain, rampPeak, releasePeak;
        
        float attackPhase, decayPhase, releasePhase, rampPhase;

        float leakFactor;


    } _tADSR;
    
    typedef _tADSR* tADSR;
    
    void    tADSR_init          (tADSR* const, float attack, float decay, float sustain, float release);
    void    tADSR_free          (tADSR* const);
    void    tADSR_initToPool    (tADSR* const, float attack, float decay, float sustain, float release, tMempool* const);
    void    tADSR_freeFromPool  (tADSR* const, tMempool* const);
    
    float   tADSR_tick          (tADSR* const);
    void    tADSR_setAttack     (tADSR* const, float attack);
    void    tADSR_setDecay      (tADSR* const, float decay);
    void    tADSR_setSustain    (tADSR* const, float sustain);
    void    tADSR_setRelease    (tADSR* const, float release);
    void 	tADSR_setLeakFactor	(tADSR* const, float leakFactor);
    void    tADSR_on            (tADSR* const, float velocity);
    void    tADSR_off           (tADSR* const);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /* Ramp */
    typedef struct _tRamp {
        float inc;
        float inv_sr_ms;
        float minimum_time;
        float curr,dest;
        float time;
        int samples_per_tick;
        
    } _tRamp;
    
    typedef _tRamp* tRamp;
    
    void    tRamp_init          (tRamp* const, float time, int samplesPerTick);
    void    tRamp_free          (tRamp* const);
    void    tRamp_initToPool    (tRamp* const, float time, int samplesPerTick, tMempool* const);
    void    tRamp_freeFromPool  (tRamp* const, tMempool* const);
    
    float   tRamp_tick          (tRamp* const);
    float   tRamp_sample        (tRamp* const);
    void    tRamp_setTime       (tRamp* const, float time);
    void    tRamp_setDest       (tRamp* const, float dest);
    void    tRamp_setVal        (tRamp* const, float val);

    ///=============
    /* Ramp Updown*/
    typedef struct _tRampUpDown {
        float upInc;
        float downInc;
        float inv_sr_ms;
        float minimum_time;
        float curr,dest;
        float upTime;
        float downTime;
        int samples_per_tick;

    } _tRampUpDown;

    typedef _tRampUpDown* tRampUpDown;

    void    tRampUpDown_init          (tRampUpDown* const, float upTime, float downTime, int samplesPerTick);
    void    tRampUpDown_free          (tRampUpDown* const);
    void    tRampUpDown_initToPool    (tRampUpDown* const, float upTime, float downTime, int samplesPerTick, tMempool* const);
    void    tRampUpDown_freeFromPool  (tRampUpDown* const, tMempool* const);
    
    float   tRampUpDown_tick          (tRampUpDown* const);
    float   tRampUpDown_sample        (tRampUpDown* const);
    void    tRampUpDown_setUpTime       (tRampUpDown* const, float upTime);
    void    tRampUpDown_setDownTime       (tRampUpDown* const, float downTime);
    void    tRampUpDown_setDest       (tRampUpDown* const, float dest);
    void    tRampUpDown_setVal        (tRampUpDown* const, float val);


    ///=========================
    //Slide (based on Max/MSP's slide~)
	typedef struct _tSlide {
		float prevOut;
		float currentOut;
		float prevIn;
		float invUpSlide;
		float invDownSlide;

	} _tSlide;

	typedef _tSlide* tSlide;

	void    tSlide_init          (tSlide* const, float upSlide, float downSlide);
	void    tSlide_free          (tSlide* const);
	void    tSlide_initToPool    (tSlide* const, float upSlide, float downSlide, tMempool* const);
	void    tSlide_freeFromPool  (tSlide* const, tMempool* const);

	float   tSlide_tick          (tSlide* const, float in);

#ifdef __cplusplus
}
#endif

#endif  // LEAF_ENVELOPES_H_INCLUDED



