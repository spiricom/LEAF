/*==============================================================================
 
 leaf-dynamics.h
 Created: 30 Nov 2018 11:57:05am
 Author:  airship
 
 ==============================================================================*/

#ifndef LEAF_DYNAMICS_H_INCLUDED
#define LEAF_DYNAMICS_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-global.h"
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-analysis.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /* Compressor */
    typedef struct _tCompressor
    {
        tMempool mempool;
        
        float tauAttack, tauRelease;
        float T, R, W, M; // Threshold, compression Ratio, decibel Width of knee transition, decibel Make-up gain
        
        float x_G[2], y_G[2], x_T[2], y_T[2];
        
        oBool isActive;
        
    } _tCompressor;
    
    typedef _tCompressor* tCompressor;
    
    void    tCompressor_init        (tCompressor* const);
    void    tCompressor_initToPool  (tCompressor* const, tMempool* const);
    void    tCompressor_free        (tCompressor* const);
    
    float   tCompressor_tick        (tCompressor* const, float input);
    
    ///
    /* Feedback leveller */
    // An auto VCA that you put into a feedback circuit to make it stay at the same level.
    // It can enforce level bidirectionally (amplifying and attenuating as needed) or
    // just attenutating. The former option allows for infinite sustain strings, for example, while
    // The latter option allows for decaying strings, which can never exceed
    // a specific level.
    
    typedef struct _tFeedbackLeveler
    {
        tMempool mempool;
        float targetLevel;    // target power level
        float strength;        // how strongly level difference affects the VCA
        int      mode;            // 0 for upwards limiting only, 1 for biderctional limiting
        float curr;
        tPowerFollower pwrFlw;    // internal power follower needed for level tracking
        
    } _tFeedbackLeveler;
    
    typedef _tFeedbackLeveler* tFeedbackLeveler;
    
    void    tFeedbackLeveler_init           (tFeedbackLeveler* const, float targetLevel, float factor, float strength, int mode);
    void    tFeedbackLeveler_initToPool     (tFeedbackLeveler* const, float targetLevel, float factor, float strength, int mode, tMempool* const);
    void    tFeedbackLeveler_free           (tFeedbackLeveler* const);
    
    float   tFeedbackLeveler_tick           (tFeedbackLeveler* const, float input);
    float   tFeedbackLeveler_sample         (tFeedbackLeveler* const);
    void    tFeedbackLeveler_setTargetLevel (tFeedbackLeveler* const, float TargetLevel);
    void    tFeedbackLeveler_setFactor      (tFeedbackLeveler* const, float factor);
    void    tFeedbackLeveler_setMode        (tFeedbackLeveler* const, int mode); // 0 for upwards limiting only, 1 for biderctional limiting
    void    tFeedbackLeveler_setStrength    (tFeedbackLeveler* const, float strength);
    
    
    //==============================================================================
    

    //Threshold with hysteresis (like Max/MSP thresh~ object)

    typedef struct _tThreshold
    {
        tMempool mempool;
        float highThresh, lowThresh;
		int currentValue;
    } _tThreshold;

    typedef _tThreshold* tThreshold;

    void    tThreshold_init        (tThreshold* const, float low, float high);
    void    tThreshold_initToPool  (tThreshold* const, float low, float high, tMempool* const);
    void    tThreshold_free        (tThreshold* const);

    int   tThreshold_tick        (tThreshold* const, float input);
    void   tThreshold_setLow        (tThreshold* const, float low);
    void   tThreshold_setHigh       (tThreshold* const, float high);



    //////======================================================================

#ifdef __cplusplus
}
#endif

#endif // LEAF_DYNAMICS_H_INCLUDED

//==============================================================================

