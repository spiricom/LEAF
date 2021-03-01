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
    
    /*!
     @defgroup tenvelope tEnvelope
     @ingroup envelopes
     @brief Basic attack-decay envelope.
     @{
     
     @fn void    tEnvelope_init          (tEnvelope* const, float attack, float decay, int loop, LEAF* const leaf)
     @brief Initialize a tEnvelope to the default mempool of a LEAF instance.
     @param envelope A pointer to the tEnvelope to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tEnvelope_initToPool    (tEnvelope* const, float attack, float decay, int loop, tMempool* const)
     @brief Initialize a tEnvelope to a specified mempool.
     @param envelope A pointer to the tEnvelope to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tEnvelope_free          (tEnvelope* const)
     @brief Free a tEnvelope from its mempool.
     @param envelope A pointer to the tEnvelope to free.
     
     @fn float   tEnvelope_tick          (tEnvelope* const)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @fn void    tEnvelope_setAttack     (tEnvelope* const, float attack)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @fn void    tEnvelope_setDecay      (tEnvelope* const, float decay)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @fn void    tEnvelope_loop          (tEnvelope* const, int loop)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @fn void    tEnvelope_on            (tEnvelope* const, float velocity)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @} */
    
    typedef struct _tEnvelope
    {
        
        tMempool mempool;
        
        const float *exp_buff;
        const float *inc_buff;
        uint32_t buff_size;
        
        float next;
        
        float attackInc, decayInc, rampInc;
        
        int inAttack, inDecay, inRamp;
        
        int loop;
        
        float gain, rampPeak;
        
        float attackPhase, decayPhase, rampPhase;
        
    } _tEnvelope;
    
    typedef _tEnvelope* tEnvelope;
    
    void    tEnvelope_init          (tEnvelope* const, float attack, float decay, int loop, LEAF* const leaf);
    void    tEnvelope_initToPool    (tEnvelope* const, float attack, float decay, int loop, tMempool* const);
    void    tEnvelope_free          (tEnvelope* const);
    
    float   tEnvelope_tick          (tEnvelope* const);
    void    tEnvelope_setAttack     (tEnvelope* const, float attack);
    void    tEnvelope_setDecay      (tEnvelope* const, float decay);
    void    tEnvelope_loop          (tEnvelope* const, int loop);
    void    tEnvelope_on            (tEnvelope* const, float velocity);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    
    /*!
     @defgroup texpsmooth tExpSmooth
     @ingroup envelopes
     @brief Exponential curve smoother.
     @{
     
     @fn void    tExpSmooth_init         (tExpSmooth* const, float val, float factor, LEAF* const leaf)
     @brief Initialize a tExpSmooth to the default mempool of a LEAF instance.
     @param smooth A pointer to the tExpSmooth to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tExpSmooth_initToPool   (tExpSmooth* const, float val, float factor, tMempool* const)
     @brief Initialize a tExpSmooth to a specified mempool.
     @param smooth A pointer to the tExpSmooth to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tExpSmooth_free         (tExpSmooth* const)
     @brief Free a tExpSmooth from its mempool.
     @param smooth A pointer to the tExpSmooth to free.
     
     @fn float   tExpSmooth_tick         (tExpSmooth* const)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn float   tExpSmooth_sample       (tExpSmooth* const)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn void    tExpSmooth_setFactor    (tExpSmooth* const, float factor)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn void    tExpSmooth_setDest      (tExpSmooth* const, float dest)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn void    tExpSmooth_setVal       (tExpSmooth* const, float val)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn  void    tExpSmooth_setValAndDest(tExpSmooth* const expsmooth, float val)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @} */
    
    typedef struct _tExpSmooth
    {
        
        tMempool mempool;
        float baseFactor, factor, oneminusfactor;
        float curr,dest;
        float invSampleRate;
    } _tExpSmooth;
    
    typedef _tExpSmooth* tExpSmooth;
    
    void    tExpSmooth_init         (tExpSmooth* const, float val, float factor, LEAF* const leaf);
    void    tExpSmooth_initToPool   (tExpSmooth* const, float val, float factor, tMempool* const);
    void    tExpSmooth_free         (tExpSmooth* const);
    
    float   tExpSmooth_tick         (tExpSmooth* const);
    float   tExpSmooth_sample       (tExpSmooth* const);
    void    tExpSmooth_setFactor    (tExpSmooth* const, float factor);
    void    tExpSmooth_setDest      (tExpSmooth* const, float dest);
    void    tExpSmooth_setVal       (tExpSmooth* const, float val);
    void    tExpSmooth_setValAndDest(tExpSmooth* const, float val);
    void    tExpSmooth_setSampleRate(tExpSmooth* const, float sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tadsr tADSR
     @ingroup envelopes
     @brief
     @{
     
     @fn void    tADSR_init    (tADSR* const adsrenv, float attack, float decay, float sustain, float release, LEAF* const leaf)
     @brief Initialize a tADSR to the default mempool of a LEAF instance.
     @param adsr A pointer to the tADSR to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tADSR_initToPool    (tADSR* const adsrenv, float attack, float decay, float sustain, float release, tMempool* const mp)
     @brief Initialize a tADSR to a specified mempool.
     @param adsr A pointer to the tADSR to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tADSR_free          (tADSR* const)
     @brief Free a tADSR from its mempool.
     @param adsr A pointer to the tADSR to free.
     
     @fn float   tADSR_tick          (tADSR* const)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setAttack     (tADSR* const, float attack)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setDecay      (tADSR* const, float decay)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setSustain    (tADSR* const, float sustain)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setRelease    (tADSR* const, float release)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setLeakFactor (tADSR* const, float leakFactor)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_on            (tADSR* const, float velocity)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_off           (tADSR* const)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @} */
    
    /* ADSR */
    typedef struct _tADSR
    {
        
        tMempool mempool;
        
        const float *exp_buff;
        const float *inc_buff;
        uint32_t buff_size;
        
        float next;
        
        float attack, decay, release;
        
        float attackInc, decayInc, releaseInc, rampInc;
        
        int inAttack, inDecay, inSustain, inRelease, inRamp;
        
        float sustain, gain, rampPeak, releasePeak;
        
        float attackPhase, decayPhase, releasePhase, rampPhase;
        
        float baseLeakFactor, leakFactor;
        
        float invSampleRate;
    } _tADSR;
    
    typedef _tADSR* tADSR;
    
    void    tADSR_init    (tADSR* const adsrenv, float attack, float decay, float sustain, float release, LEAF* const leaf);
    void    tADSR_initToPool    (tADSR* const adsrenv, float attack, float decay, float sustain, float release, tMempool* const mp);
    void    tADSR_free          (tADSR* const);
    
    float   tADSR_tick          (tADSR* const);
    void    tADSR_setAttack     (tADSR* const, float attack);
    void    tADSR_setDecay      (tADSR* const, float decay);
    void    tADSR_setSustain    (tADSR* const, float sustain);
    void    tADSR_setRelease    (tADSR* const, float release);
    void    tADSR_setLeakFactor (tADSR* const, float leakFactor);
    void    tADSR_on            (tADSR* const, float velocity);
    void    tADSR_off           (tADSR* const);
    void    tADSR_setSampleRate (tADSR* const, float sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tadsrt tADSRT
     @ingroup envelopes
     @brief
     @{
     
     @fn void    tADSRT_init          (tADSRT* const, float attack, float decay, float sustain, float release, float* expBuffer, int bufferSize, LEAF* const leaf)
     @brief Initialize a tADSRT to the default mempool of a LEAF instance.
     @param adsr A pointer to the tADSRT to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tADSRT_initToPool    (tADSRT* const, float attack, float decay, float sustain, float release, float* expBuffer, int bufferSize, tMempool* const)
     @brief Initialize a tADSRT to a specified mempool.
     @param adsr A pointer to the tADSRT to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tADSRT_free          (tADSRT* const)
     @brief Free a tADSRT from its mempool.
     @param adsr A pointer to the tADSRT to free.
     
     @fn float   tADSRT_tick          (tADSRT* const)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn float   tADSRT_tickNoInterp  (tADSRT* const adsrenv)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setAttack     (tADSRT* const, float attack)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setDecay      (tADSRT* const, float decay)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setSustain    (tADSRT* const, float sustain)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setRelease    (tADSRT* const, float release)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setLeakFactor (tADSRT* const, float leakFactor)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_on            (tADSRT* const, float velocity)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_off           (tADSRT* const)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @} */
    
    typedef struct _tADSRT
    {
        
        tMempool mempool;
        const float *exp_buff;
        uint32_t buff_size;
        uint32_t buff_sizeMinusOne;
        float sampleRate;
        float bufferSizeDividedBySampleRateInMs;
        float next;
        
        float attack, decay, release;
        float attackInc, decayInc, releaseInc, rampInc;
        
        uint32_t whichStage;
        
        float sustain, gain, rampPeak, releasePeak;
        
        float attackPhase, decayPhase, releasePhase, rampPhase;
        
        float baseLeakFactor, leakFactor;
        
        float invSampleRate;
    } _tADSRT;
    
    typedef _tADSRT* tADSRT;
    
    void    tADSRT_init          (tADSRT* const, float attack, float decay, float sustain, float release, float* expBuffer, int bufferSize, LEAF* const leaf);
    void    tADSRT_initToPool    (tADSRT* const, float attack, float decay, float sustain, float release, float* expBuffer, int bufferSize, tMempool* const);
    void    tADSRT_free          (tADSRT* const);
    
    float   tADSRT_tick          (tADSRT* const);
    float   tADSRT_tickNoInterp  (tADSRT* const adsrenv);
    void    tADSRT_setAttack     (tADSRT* const, float attack);
    void    tADSRT_setDecay      (tADSRT* const, float decay);
    void    tADSRT_setSustain    (tADSRT* const, float sustain);
    void    tADSRT_setRelease    (tADSRT* const, float release);
    void    tADSRT_setLeakFactor (tADSRT* const, float leakFactor);
    void    tADSRT_on            (tADSRT* const, float velocity);
    void    tADSRT_off           (tADSRT* const);
    void    tADSRT_setSampleRate (tADSRT* const, float sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tadsrs tADSRS
     @ingroup envelopes
     @brief
     @{
     
     @fn void    tADSRS_init          (tADSRS* const, float attack, float decay, float sustain, float release, LEAF* const leaf)
     @brief Initialize a tADSRS to the default mempool of a LEAF instance.
     @param adsr A pointer to the tADSRS to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tADSRS_initToPool    (tADSRS* const, float attack, float decay, float sustain, float release, tMempool* const)
     @brief Initialize a tADSRS to a specified mempool.
     @param adsr A pointer to the tADSRS to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tADSRS_free          (tADSRS* const)
     @brief Free a tADSRS from its mempool.
     @param adsr A pointer to the tADSRS to free.
     
     @fn float   tADSRS_tick          (tADSRS* const)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setAttack     (tADSRS* const, float attack)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setDecay      (tADSRS* const, float decay)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setSustain    (tADSRS* const, float sustain)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setRelease    (tADSRS* const, float release)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setLeakFactor (tADSRS* const, float leakFactor)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_on            (tADSRS* const, float velocity)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_off           (tADSRS* const)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @} */
    
    enum envState {
        env_idle = 0,
        env_attack,
        env_decay,
        env_sustain,
        env_release,
        env_ramp
    };
    
    typedef struct _tADSRS
    {
        
        tMempool mempool;
        float sampleRate;
        float sampleRateInMs;
        int state;
        float output;
        float attack;
        float decay;
        float release;
        float attackRate;
        float decayRate;
        float releaseRate;
        float attackCoef;
        float decayCoef;
        float releaseCoef;
        float sustainLevel;
        float targetRatioA;
        float targetRatioDR;
        float attackBase;
        float decayBase;
        float releaseBase;
        float baseLeakFactor, leakFactor;
        float targetGainSquared;
        float factor;
        float oneMinusFactor;
        float gain;
        float invSampleRate;
    } _tADSRS;
    
    typedef _tADSRS* tADSRS;
    
    void    tADSRS_init          (tADSRS* const, float attack, float decay, float sustain, float release, LEAF* const leaf);
    void    tADSRS_initToPool    (tADSRS* const, float attack, float decay, float sustain, float release, tMempool* const);
    void    tADSRS_free          (tADSRS* const);
    
    float   tADSRS_tick          (tADSRS* const);
    void    tADSRS_setAttack     (tADSRS* const, float attack);
    void    tADSRS_setDecay      (tADSRS* const, float decay);
    void    tADSRS_setSustain    (tADSRS* const, float sustain);
    void    tADSRS_setRelease    (tADSRS* const, float release);
    void    tADSRS_setLeakFactor (tADSRS* const, float leakFactor);
    void    tADSRS_on            (tADSRS* const, float velocity);
    void    tADSRS_off           (tADSRS* const);
    void    tADSRS_setSampleRate (tADSRS* const, float sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tramp tRamp
     @ingroup envelopes
     @brief Value ramp.
     @{
     
     @fn void    tRamp_init          (tRamp* const, float time, int samplesPerTick, LEAF* const leaf)
     @brief Initialize a tRamp to the default mempool of a LEAF instance.
     @param ramp A pointer to the tRamp to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tRamp_initToPool    (tRamp* const, float time, int samplesPerTick, tMempool* const)
     @brief Initialize a tRamp to a specified mempool.
     @param ramp A pointer to the tRamp to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tRamp_free          (tRamp* const)
     @brief Free a tRamp from its mempool.
     @param ramp A pointer to the tRamp to free.
     
     @fn float   tRamp_tick          (tRamp* const)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @fn float   tRamp_sample        (tRamp* const)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @fn void    tRamp_setTime       (tRamp* const, float time)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @fn void    tRamp_setDest       (tRamp* const, float dest)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @fn void    tRamp_setVal        (tRamp* const, float val)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @} */
    
    typedef struct _tRamp
    {
        tMempool mempool;
        float inc;
        float sampleRate;
        float inv_sr_ms;
        float minimum_time;
        float curr,dest;
        float time;
        float factor;
        int samples_per_tick;
    } _tRamp;
    
    typedef _tRamp* tRamp;
    
    void    tRamp_init          (tRamp* const, float time, int samplesPerTick, LEAF* const leaf);
    void    tRamp_initToPool    (tRamp* const, float time, int samplesPerTick, tMempool* const);
    void    tRamp_free          (tRamp* const);
    
    float   tRamp_tick          (tRamp* const);
    float   tRamp_sample        (tRamp* const);
    void    tRamp_setTime       (tRamp* const, float time);
    void    tRamp_setDest       (tRamp* const, float dest);
    void    tRamp_setVal        (tRamp* const, float val);
    void    tRamp_setSampleRate (tRamp* const, float sr);
    
    /*!
     @defgroup trampupdown tRampUpDown
     @ingroup envelopes
     @brief Value ramp with variable rate depending on direction.
     @{
     
     @fn void    tRampUpDown_init          (tRampUpDown* const, float upTime, float downTime, int samplesPerTick, LEAF* const leaf)
     @brief Initialize a tRampUpDown to the default mempool of a LEAF instance.
     @param ramp A pointer to the tRampUpDown to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tRampUpDown_initToPool    (tRampUpDown* const, float upTime, float downTime, int samplesPerTick, tMempool* const)
     @brief Initialize a tRampUpDown to a specified mempool.
     @param ramp A pointer to the tRampUpDown to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tRampUpDown_free          (tRampUpDown* const)
     @brief Free a tRampUpDown from its mempool.
     @param ramp A pointer to the tRampUpDown to free.
     
     @fn float   tRampUpDown_tick          (tRampUpDown* const)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn float   tRampUpDown_sample        (tRampUpDown* const)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn void    tRampUpDown_setUpTime       (tRampUpDown* const, float upTime)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn void    tRampUpDown_setDownTime       (tRampUpDown* const, float downTime)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn void    tRampUpDown_setDest       (tRampUpDown* const, float dest)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn void    tRampUpDown_setVal        (tRampUpDown* const, float val)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @} */
    
    typedef struct _tRampUpDown
    {
        tMempool mempool;
        float upInc;
        float downInc;
        float sampleRate;
        float inv_sr_ms;
        float minimum_time;
        float curr,dest;
        float upTime;
        float downTime;
        int samples_per_tick;
    } _tRampUpDown;
    
    typedef _tRampUpDown* tRampUpDown;
    
    void    tRampUpDown_init          (tRampUpDown* const, float upTime, float downTime, int samplesPerTick, LEAF* const leaf);
    void    tRampUpDown_initToPool    (tRampUpDown* const, float upTime, float downTime, int samplesPerTick, tMempool* const);
    void    tRampUpDown_free          (tRampUpDown* const);
    
    float   tRampUpDown_tick          (tRampUpDown* const);
    float   tRampUpDown_sample        (tRampUpDown* const);
    void    tRampUpDown_setUpTime       (tRampUpDown* const, float upTime);
    void    tRampUpDown_setDownTime       (tRampUpDown* const, float downTime);
    void    tRampUpDown_setDest       (tRampUpDown* const, float dest);
    void    tRampUpDown_setVal        (tRampUpDown* const, float val);
    void    tRampUpDown_setSampleRate (tRampUpDown* const, float sr);
    
    /*!
     @defgroup tslide tSlide
     @ingroup envelopes
     @brief Based on Max/MSP's slide~
     @{
     
     @fn void    tSlide_init          (tSlide* const, float upSlide, float downSlide, LEAF* const leaf)
     @brief Initialize a tSlide to the default mempool of a LEAF instance.
     @param slide A pointer to the tSlide to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSlide_initToPool    (tSlide* const, float upSlide, float downSlide, tMempool* const)
     @brief Initialize a tSlide to a specified mempool.
     @param slide A pointer to the tSlide to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSlide_free          (tSlide* const)
     @brief Free a tSlide from its mempool.
     @param slide A pointer to the tSlide to free.
     
     @fn float   tSlide_tick         (tSlide* const, float in)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @fn float   tSlide_tickNoInput    (tSlide* const sl)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @fn void    tSlide_setUpSlide    (tSlide* const sl, float upSlide)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @fn void    tSlide_setDownSlide    (tSlide* const sl, float downSlide)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @fn void    tSlide_setDest        (tSlide* const sl, float dest)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @} */
    
    typedef struct _tSlide
    {
        tMempool mempool;
        float prevOut;
        float currentOut;
        float prevIn;
        float invUpSlide;
        float invDownSlide;
        float dest;
    } _tSlide;
    
    typedef _tSlide* tSlide;
    
    void    tSlide_init          (tSlide* const, float upSlide, float downSlide, LEAF* const leaf);
    void    tSlide_initToPool    (tSlide* const, float upSlide, float downSlide, tMempool* const);
    void    tSlide_free          (tSlide* const);
    
    float   tSlide_tick         (tSlide* const, float in);
    float   tSlide_tickNoInput    (tSlide* const sl);
    void    tSlide_setUpSlide    (tSlide* const sl, float upSlide);
    void    tSlide_setDownSlide    (tSlide* const sl, float downSlide);
    void    tSlide_setDest        (tSlide* const sl, float dest);
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_ENVELOPES_H_INCLUDED




