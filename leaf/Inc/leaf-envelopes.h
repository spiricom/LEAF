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
     
     @fn void    tEnvelope_init          (tEnvelope* const, Lfloat attack, Lfloat decay, int loop, LEAF* const leaf)
     @brief Initialize a tEnvelope to the default mempool of a LEAF instance.
     @param envelope A pointer to the tEnvelope to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tEnvelope_initToPool    (tEnvelope* const, Lfloat attack, Lfloat decay, int loop, tMempool* const)
     @brief Initialize a tEnvelope to a specified mempool.
     @param envelope A pointer to the tEnvelope to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tEnvelope_free          (tEnvelope* const)
     @brief Free a tEnvelope from its mempool.
     @param envelope A pointer to the tEnvelope to free.
     
     @fn Lfloat   tEnvelope_tick          (tEnvelope* const)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @fn void    tEnvelope_setAttack     (tEnvelope* const, Lfloat attack)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @fn void    tEnvelope_setDecay      (tEnvelope* const, Lfloat decay)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @fn void    tEnvelope_loop          (tEnvelope* const, int loop)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @fn void    tEnvelope_on            (tEnvelope* const, Lfloat velocity)
     @brief
     @param envelope A pointer to the relevant tEnvelope.
     
     @} */
    
    typedef struct _tEnvelope
    {
        
        tMempool mempool;
        
        const Lfloat *exp_buff;
        const Lfloat *inc_buff;
        uint32_t buff_size;
        
        Lfloat next;
        
        Lfloat attackInc, decayInc, rampInc;
        
        int inAttack, inDecay, inRamp;
        
        int loop;
        
        Lfloat gain, rampPeak;
        
        Lfloat attackPhase, decayPhase, rampPhase;
        
    } _tEnvelope;
    
    typedef _tEnvelope* tEnvelope;
    
    void    tEnvelope_init          (tEnvelope* const, Lfloat attack, Lfloat decay, int loop, LEAF* const leaf);
    void    tEnvelope_initToPool    (tEnvelope* const, Lfloat attack, Lfloat decay, int loop, tMempool* const);
    void    tEnvelope_free          (tEnvelope* const);
    
    Lfloat  tEnvelope_tick          (tEnvelope const);

    void    tEnvelope_setAttack     (tEnvelope const, Lfloat attack);
    void    tEnvelope_setDecay      (tEnvelope const, Lfloat decay);
    void    tEnvelope_loop          (tEnvelope const, int loop);
    void    tEnvelope_on            (tEnvelope const, Lfloat velocity);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    
    /*!
     @defgroup texpsmooth tExpSmooth
     @ingroup envelopes
     @brief Exponential curve smoother.
     @{
     
     @fn void    tExpSmooth_init         (tExpSmooth* const, Lfloat val, Lfloat factor, LEAF* const leaf)
     @brief Initialize a tExpSmooth to the default mempool of a LEAF instance.
     @param smooth A pointer to the tExpSmooth to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tExpSmooth_initToPool   (tExpSmooth* const, Lfloat val, Lfloat factor, tMempool* const)
     @brief Initialize a tExpSmooth to a specified mempool.
     @param smooth A pointer to the tExpSmooth to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tExpSmooth_free         (tExpSmooth* const)
     @brief Free a tExpSmooth from its mempool.
     @param smooth A pointer to the tExpSmooth to free.
     
     @fn Lfloat   tExpSmooth_tick         (tExpSmooth* const)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn Lfloat   tExpSmooth_sample       (tExpSmooth* const)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn void    tExpSmooth_setFactor    (tExpSmooth* const, Lfloat factor)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn void    tExpSmooth_setDest      (tExpSmooth* const, Lfloat dest)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn void    tExpSmooth_setVal       (tExpSmooth* const, Lfloat val)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @fn  void    tExpSmooth_setValAndDest(tExpSmooth* const expsmooth, Lfloat val)
     @brief
     @param smooth A pointer to the relevant tExpSmooth.
     
     @} */
    
    typedef struct _tExpSmooth
    {
        
        tMempool mempool;
        Lfloat factor, oneminusfactor;
        Lfloat curr,dest;
        //Lfloat invSampleRate;
    } _tExpSmooth;
    
    typedef _tExpSmooth* tExpSmooth;
    
    void    tExpSmooth_init          (tExpSmooth* const, Lfloat val, Lfloat factor,
                                      LEAF* const leaf);
    void    tExpSmooth_initToPool    (tExpSmooth* const, Lfloat val, Lfloat factor,
                                      tMempool* const);
    void    tExpSmooth_free          (tExpSmooth* const);
    

#ifdef ITCMRAM
Lfloat  __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tExpSmooth_tick(tExpSmooth* const expsmooth);
#else
    Lfloat  tExpSmooth_tick          (tExpSmooth const expsmooth);
#endif

    Lfloat  tExpSmooth_sample        (tExpSmooth const);
    void    tExpSmooth_setFactor     (tExpSmooth const, Lfloat factor);
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tExpSmooth_setDest      (tExpSmooth* const, Lfloat dest);
#else
    void    tExpSmooth_setDest       (tExpSmooth const, Lfloat dest);
#endif
    void    tExpSmooth_setVal        (tExpSmooth const, Lfloat val);
    void    tExpSmooth_setValAndDest (tExpSmooth const, Lfloat val);
    void    tExpSmooth_setSampleRate (tExpSmooth const, Lfloat sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tadsr tADSR
     @ingroup envelopes
     @brief
     @{
     
     @fn void    tADSR_init    (tADSR* const adsrenv, Lfloat attack, Lfloat decay, Lfloat sustain, Lfloat release, LEAF* const leaf)
     @brief Initialize a tADSR to the default mempool of a LEAF instance.
     @param adsr A pointer to the tADSR to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tADSR_initToPool    (tADSR* const adsrenv, Lfloat attack, Lfloat decay, Lfloat sustain, Lfloat release, tMempool* const mp)
     @brief Initialize a tADSR to a specified mempool.
     @param adsr A pointer to the tADSR to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tADSR_free          (tADSR* const)
     @brief Free a tADSR from its mempool.
     @param adsr A pointer to the tADSR to free.
     
     @fn Lfloat   tADSR_tick          (tADSR* const)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setAttack     (tADSR* const, Lfloat attack)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setDecay      (tADSR* const, Lfloat decay)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setSustain    (tADSR* const, Lfloat sustain)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setRelease    (tADSR* const, Lfloat release)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_setLeakFactor (tADSR* const, Lfloat leakFactor)
     @brief
     @param adsr A pointer to the relevant tADSR.
     
     @fn void    tADSR_on            (tADSR* const, Lfloat velocity)
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
        
        const Lfloat *exp_buff;
        const Lfloat *inc_buff;
        uint32_t buff_size;
        
        Lfloat next;
        
        Lfloat attack, decay, release;
        
        Lfloat attackInc, decayInc, releaseInc, rampInc;
        
        int inAttack, inDecay, inSustain, inRelease, inRamp;
        
        Lfloat sustain, gain, rampPeak, releasePeak;
        
        Lfloat attackPhase, decayPhase, releasePhase, rampPhase;
        
        Lfloat baseLeakFactor, leakFactor;
        
        Lfloat invSampleRate;
    } _tADSR;
    
    typedef _tADSR* tADSR;
    
    void    tADSR_init          (tADSR* const adsrenv, Lfloat attack, Lfloat decay,
                                 Lfloat sustain, Lfloat release, LEAF* const leaf);
    void    tADSR_initToPool    (tADSR* const adsrenv, Lfloat attack, Lfloat decay,
                                 Lfloat sustain, Lfloat release, tMempool* const mp);
    void    tADSR_free          (tADSR* const);
    
    Lfloat   tADSR_tick         (tADSR const);

    void    tADSR_setAttack     (tADSR const, Lfloat attack);
    void    tADSR_setDecay      (tADSR const, Lfloat decay);
    void    tADSR_setSustain    (tADSR const, Lfloat sustain);
    void    tADSR_setRelease    (tADSR const, Lfloat release);
    void    tADSR_setLeakFactor (tADSR const, Lfloat leakFactor);
    void    tADSR_on            (tADSR const, Lfloat velocity);
    void    tADSR_off           (tADSR const);
    void    tADSR_setSampleRate (tADSR const, Lfloat sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tadsrt tADSRT
     @ingroup envelopes
     @brief
     @{
     
     @fn void    tADSRT_init          (tADSRT* const, Lfloat attack, Lfloat decay, Lfloat sustain, Lfloat release, Lfloat* expBuffer, int bufferSize, LEAF* const leaf)
     @brief Initialize a tADSRT to the default mempool of a LEAF instance.
     @param adsr A pointer to the tADSRT to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tADSRT_initToPool    (tADSRT* const, Lfloat attack, Lfloat decay, Lfloat sustain, Lfloat release, Lfloat* expBuffer, int bufferSize, tMempool* const)
     @brief Initialize a tADSRT to a specified mempool.
     @param adsr A pointer to the tADSRT to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tADSRT_free          (tADSRT* const)
     @brief Free a tADSRT from its mempool.
     @param adsr A pointer to the tADSRT to free.
     
     @fn Lfloat   tADSRT_tick          (tADSRT* const)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn Lfloat   tADSRT_tickNoInterp  (tADSRT* const adsrenv)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setAttack     (tADSRT* const, Lfloat attack)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setDecay      (tADSRT* const, Lfloat decay)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setSustain    (tADSRT* const, Lfloat sustain)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setRelease    (tADSRT* const, Lfloat release)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_setLeakFactor (tADSRT* const, Lfloat leakFactor)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_on            (tADSRT* const, Lfloat velocity)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @fn void    tADSRT_off           (tADSRT* const)
     @brief
     @param adsr A pointer to the relevant tADSRT.
     
     @} */
    
    typedef struct _tADSRT
    {
        
        tMempool mempool;
        const Lfloat *exp_buff;
        uint32_t buff_size;
        uint32_t buff_sizeMinusOne;
        Lfloat sampleRate;
        Lfloat bufferSizeDividedBySampleRateInMs;
        Lfloat next;
        Lfloat sustainWithLeak;
        
        Lfloat attack, decay, release;
        Lfloat attackInc, decayInc, releaseInc, rampInc;
        
        uint32_t whichStage;
        
        Lfloat sustain, gain, rampPeak, releasePeak;
        
        Lfloat attackPhase, decayPhase, releasePhase, rampPhase;
        
        Lfloat baseLeakFactor, leakFactor;
        
        Lfloat invSampleRate;
    } _tADSRT;
    
    typedef _tADSRT* tADSRT;
    
    void    tADSRT_init          (tADSRT* const, Lfloat attack, Lfloat decay, Lfloat sustain, Lfloat release, Lfloat* expBuffer, int bufferSize, LEAF* const leaf);
    void    tADSRT_initToPool    (tADSRT* const, Lfloat attack, Lfloat decay, Lfloat sustain, Lfloat release, Lfloat* expBuffer, int bufferSize, tMempool* const);
    void    tADSRT_free          (tADSRT* const);
    
    Lfloat  tADSRT_tick          (tADSRT const);
    Lfloat  tADSRT_tickNoInterp  (tADSRT const adsrenv);

    void    tADSRT_setAttack     (tADSRT const, Lfloat attack);
    void    tADSRT_setDecay      (tADSRT const, Lfloat decay);
    void    tADSRT_setSustain    (tADSRT const, Lfloat sustain);
    void    tADSRT_setRelease    (tADSRT const, Lfloat release);
    void    tADSRT_setLeakFactor (tADSRT const, Lfloat leakFactor);
    void    tADSRT_on            (tADSRT const, Lfloat velocity);
    void    tADSRT_off           (tADSRT const);
    void 	tADSRT_clear		 (tADSRT const adsrenv);
    void    tADSRT_setSampleRate (tADSRT const, Lfloat sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tadsrs tADSRS
     @ingroup envelopes
     @brief
     @{
     
     @fn void    tADSRS_init          (tADSRS* const, Lfloat attack, Lfloat decay, Lfloat sustain, Lfloat release, LEAF* const leaf)
     @brief Initialize a tADSRS to the default mempool of a LEAF instance.
     @param adsr A pointer to the tADSRS to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tADSRS_initToPool    (tADSRS* const, Lfloat attack, Lfloat decay, Lfloat sustain, Lfloat release, tMempool* const)
     @brief Initialize a tADSRS to a specified mempool.
     @param adsr A pointer to the tADSRS to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tADSRS_free          (tADSRS* const)
     @brief Free a tADSRS from its mempool.
     @param adsr A pointer to the tADSRS to free.
     
     @fn Lfloat   tADSRS_tick          (tADSRS* const)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setAttack     (tADSRS* const, Lfloat attack)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setDecay      (tADSRS* const, Lfloat decay)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setSustain    (tADSRS* const, Lfloat sustain)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setRelease    (tADSRS* const, Lfloat release)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_setLeakFactor (tADSRS* const, Lfloat leakFactor)
     @brief
     @param adsr A pointer to the relevant tADSRS.
     
     @fn void    tADSRS_on            (tADSRS* const, Lfloat velocity)
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
        Lfloat sampleRate;
        Lfloat sampleRateInMs;
        int state;
        Lfloat output;
        Lfloat attack;
        Lfloat decay;
        Lfloat release;
        Lfloat attackRate;
        Lfloat decayRate;
        Lfloat releaseRate;
        Lfloat attackCoef;
        Lfloat decayCoef;
        Lfloat releaseCoef;
        Lfloat sustainLevel;
        Lfloat targetRatioA;
        Lfloat targetRatioDR;
        Lfloat attackBase;
        Lfloat decayBase;
        Lfloat releaseBase;
        Lfloat baseLeakFactor, leakFactor;
        Lfloat targetGainSquared;
        Lfloat factor;
        Lfloat oneMinusFactor;
        Lfloat gain;
        Lfloat invSampleRate;
    } _tADSRS;
    
    typedef _tADSRS* tADSRS;
    
    void    tADSRS_init          (tADSRS* const, Lfloat attack, Lfloat decay,
                                  Lfloat sustain, Lfloat release, LEAF* const leaf);
    void    tADSRS_initToPool    (tADSRS* const, Lfloat attack, Lfloat decay,
                                  Lfloat sustain, Lfloat release, tMempool* const);
    void    tADSRS_free          (tADSRS* const);
    
    Lfloat  tADSRS_tick          (tADSRS const);

    void    tADSRS_setAttack     (tADSRS const, Lfloat attack);
    void    tADSRS_setDecay      (tADSRS const, Lfloat decay);
    void    tADSRS_setSustain    (tADSRS const, Lfloat sustain);
    void    tADSRS_setRelease    (tADSRS const, Lfloat release);
    void    tADSRS_setLeakFactor (tADSRS const, Lfloat leakFactor);
    void    tADSRS_on            (tADSRS const, Lfloat velocity);
    void    tADSRS_off           (tADSRS const);
    void    tADSRS_setSampleRate (tADSRS const, Lfloat sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tramp tRamp
     @ingroup envelopes
     @brief Value ramp.
     @{
     
     @fn void    tRamp_init          (tRamp* const, Lfloat time, int samplesPerTick, LEAF* const leaf)
     @brief Initialize a tRamp to the default mempool of a LEAF instance.
     @param ramp A pointer to the tRamp to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tRamp_initToPool    (tRamp* const, Lfloat time, int samplesPerTick, tMempool* const)
     @brief Initialize a tRamp to a specified mempool.
     @param ramp A pointer to the tRamp to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tRamp_free          (tRamp* const)
     @brief Free a tRamp from its mempool.
     @param ramp A pointer to the tRamp to free.
     
     @fn Lfloat   tRamp_tick          (tRamp* const)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @fn Lfloat   tRamp_sample        (tRamp* const)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @fn void    tRamp_setTime       (tRamp* const, Lfloat time)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @fn void    tRamp_setDest       (tRamp* const, Lfloat dest)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @fn void    tRamp_setVal        (tRamp* const, Lfloat val)
     @brief
     @param ramp A pointer to the relevant tRamp.
     
     @} */
    
    typedef struct _tRamp
    {
        tMempool mempool;
        Lfloat inc;
        Lfloat sampleRate;
        Lfloat inv_sr_ms;
        Lfloat minimum_time;
        Lfloat curr,dest;
        Lfloat time;
        Lfloat factor;
        int samples_per_tick;
    } _tRamp;
    
    typedef _tRamp* tRamp;
    
    void    tRamp_init          (tRamp* const, Lfloat time, int samplesPerTick,
                                 LEAF* const leaf);
    void    tRamp_initToPool    (tRamp* const, Lfloat time, int samplesPerTick,
                                 tMempool* const);
    void    tRamp_free          (tRamp* const);
    
    Lfloat  tRamp_tick          (tRamp const);

    Lfloat  tRamp_sample        (tRamp const);
    void    tRamp_setTime       (tRamp const, Lfloat time);
    void    tRamp_setDest       (tRamp const, Lfloat dest);
    void    tRamp_setVal        (tRamp const, Lfloat val);
    void    tRamp_setSampleRate (tRamp const, Lfloat sr);
    
    /*!
     @defgroup trampupdown tRampUpDown
     @ingroup envelopes
     @brief Value ramp with variable rate depending on direction.
     @{
     
     @fn void    tRampUpDown_init          (tRampUpDown* const, Lfloat upTime, Lfloat downTime, int samplesPerTick, LEAF* const leaf)
     @brief Initialize a tRampUpDown to the default mempool of a LEAF instance.
     @param ramp A pointer to the tRampUpDown to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tRampUpDown_initToPool    (tRampUpDown* const, Lfloat upTime, Lfloat downTime, int samplesPerTick, tMempool* const)
     @brief Initialize a tRampUpDown to a specified mempool.
     @param ramp A pointer to the tRampUpDown to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tRampUpDown_free          (tRampUpDown* const)
     @brief Free a tRampUpDown from its mempool.
     @param ramp A pointer to the tRampUpDown to free.
     
     @fn Lfloat   tRampUpDown_tick          (tRampUpDown* const)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn Lfloat   tRampUpDown_sample        (tRampUpDown* const)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn void    tRampUpDown_setUpTime       (tRampUpDown* const, Lfloat upTime)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn void    tRampUpDown_setDownTime       (tRampUpDown* const, Lfloat downTime)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn void    tRampUpDown_setDest       (tRampUpDown* const, Lfloat dest)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @fn void    tRampUpDown_setVal        (tRampUpDown* const, Lfloat val)
     @brief
     @param ramp A pointer to the relevant tRampUpDown.
     
     @} */
    
    typedef struct _tRampUpDown
    {
        tMempool mempool;
        Lfloat upInc;
        Lfloat downInc;
        Lfloat sampleRate;
        Lfloat inv_sr_ms;
        Lfloat minimum_time;
        Lfloat curr,dest;
        Lfloat upTime;
        Lfloat downTime;
        int samples_per_tick;
    } _tRampUpDown;
    
    typedef _tRampUpDown* tRampUpDown;
    
    void    tRampUpDown_init          (tRampUpDown* const, Lfloat upTime, Lfloat downTime,
                                       int samplesPerTick, LEAF* const leaf);
    void    tRampUpDown_initToPool    (tRampUpDown* const, Lfloat upTime, Lfloat downTime,
                                       int samplesPerTick, tMempool* const);
    void    tRampUpDown_free          (tRampUpDown* const);
    
    Lfloat  tRampUpDown_tick          (tRampUpDown const);

    Lfloat  tRampUpDown_sample        (tRampUpDown const);
    void    tRampUpDown_setUpTime     (tRampUpDown const, Lfloat upTime);
    void    tRampUpDown_setDownTime   (tRampUpDown const, Lfloat downTime);
    void    tRampUpDown_setDest       (tRampUpDown const, Lfloat dest);
    void    tRampUpDown_setVal        (tRampUpDown const, Lfloat val);
    void    tRampUpDown_setSampleRate (tRampUpDown const, Lfloat sr);
    
    /*!
     @defgroup tslide tSlide
     @ingroup envelopes
     @brief Based on Max/MSP's slide~
     @{
     
     @fn void    tSlide_init          (tSlide* const, Lfloat upSlide, Lfloat downSlide, LEAF* const leaf)
     @brief Initialize a tSlide to the default mempool of a LEAF instance.
     @param slide A pointer to the tSlide to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSlide_initToPool    (tSlide* const, Lfloat upSlide, Lfloat downSlide, tMempool* const)
     @brief Initialize a tSlide to a specified mempool.
     @param slide A pointer to the tSlide to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSlide_free          (tSlide* const)
     @brief Free a tSlide from its mempool.
     @param slide A pointer to the tSlide to free.
     
     @fn Lfloat   tSlide_tick         (tSlide* const, Lfloat in)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @fn Lfloat   tSlide_tickNoInput    (tSlide* const sl)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @fn void    tSlide_setUpSlide    (tSlide* const sl, Lfloat upSlide)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @fn void    tSlide_setDownSlide    (tSlide* const sl, Lfloat downSlide)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @fn void    tSlide_setDest        (tSlide* const sl, Lfloat dest)
     @brief
     @param slide A pointer to the relevant tSlide.
     
     @} */
    
    typedef struct _tSlide
    {
        tMempool mempool;
        Lfloat prevOut;
        Lfloat currentOut;
        Lfloat prevIn;
        Lfloat invUpSlide;
        Lfloat invDownSlide;
        Lfloat dest;
    } _tSlide;
    
    typedef _tSlide* tSlide;
    
    void    tSlide_init          (tSlide* const, Lfloat upSlide, Lfloat downSlide, LEAF* const leaf);
    void    tSlide_initToPool    (tSlide* const, Lfloat upSlide, Lfloat downSlide, tMempool* const);
    void    tSlide_free          (tSlide* const);
    
    Lfloat  tSlide_tick          (tSlide const, Lfloat in);
    Lfloat  tSlide_tickNoInput   (tSlide const);

    void    tSlide_setUpSlide    (tSlide const, Lfloat upSlide);
    void    tSlide_setDownSlide  (tSlide const, Lfloat downSlide);
    void    tSlide_setDest       (tSlide const, Lfloat dest);
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_ENVELOPES_H_INCLUDED




