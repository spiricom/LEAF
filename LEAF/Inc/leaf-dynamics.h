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
    
    //==============================================================================
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /* Compressor */
    typedef struct _tCompressor
    {
        float tauAttack, tauRelease;
        float T, R, W, M; // Threshold, compression Ratio, decibel Width of knee transition, decibel Make-up gain
        
        float x_G[2], y_G[2], x_T[2], y_T[2];
        
        oBool isActive;
        
    } _tCompressor;
    
    typedef _tCompressor* tCompressor;
    
    void    tCompressor_init        (tCompressor* const);
    void    tCompressor_free        (tCompressor* const);
    void    tCompressor_initToPool  (tCompressor* const, tMempool* const);
    void    tCompressor_freeFromPool(tCompressor* const, tMempool* const);
    
    float   tCompressor_tick        (tCompressor* const, float input);
    
    ///
    /* Feedback leveller */
    // An auto VCA that you put into a feedback circuit to make it stay at the same level.
    // It can enforce level bidirectionally (amplifying and attenuating as needed) or
    // just attenutating. The former option allows for infinite sustain strings, for example, while
    // The latter option allows for decaying strings, which can never exceed
    // a specific level.
    
    typedef struct _tFeedbackLeveler {
        float targetLevel;    // target power level
        float strength;        // how strongly level difference affects the VCA
        int      mode;            // 0 for upwards limiting only, 1 for biderctional limiting
        float curr;
        tPowerFollower pwrFlw;    // internal power follower needed for level tracking
        
    } _tFeedbackLeveler;
    
    typedef _tFeedbackLeveler* tFeedbackLeveler;
    
    void    tFeedbackLeveler_init           (tFeedbackLeveler* const, float targetLevel, float factor, float strength, int mode);
    void    tFeedbackLeveler_free           (tFeedbackLeveler* const);
    void    tFeedbackLeveler_initToPool     (tFeedbackLeveler* const, float targetLevel, float factor, float strength, int mode, tMempool* const);
    void    tFeedbackLeveler_freeFromPool   (tFeedbackLeveler* const, tMempool* const);
    
    float   tFeedbackLeveler_tick           (tFeedbackLeveler* const, float input);
    float   tFeedbackLeveler_sample         (tFeedbackLeveler* const);
    void    tFeedbackLeveler_setTargetLevel (tFeedbackLeveler* const, float TargetLevel);
    void    tFeedbackLeveler_setFactor      (tFeedbackLeveler* const, float factor);
    void    tFeedbackLeveler_setMode        (tFeedbackLeveler* const, int mode); // 0 for upwards limiting only, 1 for biderctional limiting
    void    tFeedbackLeveler_setStrength    (tFeedbackLeveler* const, float strength);
    
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_DYNAMICS_H_INCLUDED

//==============================================================================

