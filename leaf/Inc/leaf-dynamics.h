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
    
    /*!
     @defgroup tcompressor tCompressor
     @ingroup dynamics
     @brief Standard compressor.
     @{
     
     @fn void    tCompressor_init        (tCompressor* const, LEAF* const leaf)
     @brief Initialize a tCompressor to the default mempool of a LEAF instance.
     @param compressor A pointer to the tCompressor to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tCompressor_initToPool  (tCompressor* const, tMempool* const)
     @brief Initialize a tCompressor to a specified mempool.
     @param compressor A pointer to the tCompressor to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tCompressor_free        (tCompressor* const)
     @brief Free a tCompressor from its mempool.
     @param compressor A pointer to the tCompressor to free.
     
     @fn float   tCompressor_tick        (tCompressor* const, float input)
     @brief
     @param compressor A pointer to the relevant tCompressor.
     ￼￼￼
     @} */
   
    typedef struct _tCompressor
    {
        
        tMempool mempool;
        
        float tauAttack, tauRelease;
        float T, R, W, M; // Threshold, compression Ratio, decibel Width of knee transition, decibel Make-up gain
        
        float x_G[2], y_G[2], x_T[2], y_T[2];
        
        int isActive;
        
        float sampleRate;
        
    } _tCompressor;
    
    typedef _tCompressor* tCompressor;
    
    void    tCompressor_init        (tCompressor* const, LEAF* const leaf);
    void    tCompressor_initToPool  (tCompressor* const, tMempool* const);
    void    tCompressor_free        (tCompressor* const);
    
    float   tCompressor_tick        (tCompressor* const, float input);
    
    
    /*!
     @defgroup tfeedbackleveler tFeedbackLeveler
     @ingroup dynamics
     @brief An auto VCA that you put into a feedback circuit to make it stay at the same level.
     @details An auto VCA that you put into a feedback circuit to make it stay at the same level. It can enforce level bidirectionally (amplifying and attenuating as needed) or just attenutating. The former option allows for infinite sustain strings, for example, while the latter option allows for decaying strings, which can never exceed a specific level.
     @{
     
     @fn void tFeedbackLeveler_init (tFeedbackLeveler* const, float targetLevel, float factor, float strength, int mode, LEAF* const leaf)
     @brief Initialize a tFeedbackLeveler to the default mempool of a LEAF instance.
     @param leveler A pointer to the tFeedbackLeveler to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void tFeedbackLeveler_initToPool (tFeedbackLeveler* const, float targetLevel, float factor, float strength, int mode, tMempool* const)
     @brief Initialize a tFeedbackLeveler to a specified mempool.
     @param leveler A pointer to the tFeedbackLeveler to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tFeedbackLeveler_free           (tFeedbackLeveler* const)
     @brief Free a tFeedbackLeveler from its mempool.
     @param leveler A pointer to the tFeedbackLeveler to free.
     
     @fn float   tFeedbackLeveler_tick           (tFeedbackLeveler* const, float input)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     
     @fn float   tFeedbackLeveler_sample         (tFeedbackLeveler* const)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     
     @fn void    tFeedbackLeveler_setTargetLevel (tFeedbackLeveler* const, float TargetLevel)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     
     @fn void    tFeedbackLeveler_setFactor      (tFeedbackLeveler* const, float factor)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     
     @fn void    tFeedbackLeveler_setMode        (tFeedbackLeveler* const, int mode)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     @param mode 0 for upwards limiting only, 1 for biderctional limiting
    
     @fn void    tFeedbackLeveler_setStrength    (tFeedbackLeveler* const, float strength)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     ￼￼￼
     @} */
    
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
    
    void    tFeedbackLeveler_init           (tFeedbackLeveler* const, float targetLevel, float factor, float strength, int mode, LEAF* const leaf);
    void    tFeedbackLeveler_initToPool     (tFeedbackLeveler* const, float targetLevel, float factor, float strength, int mode, tMempool* const);
    void    tFeedbackLeveler_free           (tFeedbackLeveler* const);
    
    float   tFeedbackLeveler_tick           (tFeedbackLeveler* const, float input);
    float   tFeedbackLeveler_sample         (tFeedbackLeveler* const);
    void    tFeedbackLeveler_setTargetLevel (tFeedbackLeveler* const, float TargetLevel);
    void    tFeedbackLeveler_setFactor      (tFeedbackLeveler* const, float factor);
    void    tFeedbackLeveler_setMode        (tFeedbackLeveler* const, int mode); // 0 for upwards limiting only, 1 for biderctional limiting
    void    tFeedbackLeveler_setStrength    (tFeedbackLeveler* const, float strength);
    
    
    //==============================================================================
    

    /*!
     @defgroup tthreshold tThreshold
     @ingroup dynamics
     @brief Threshold with hysteresis (like Max/MSP thresh~ object)
     @{
     
     @fn void    tThreshold_init        (tThreshold* const, float low, float high, LEAF* const leaf)
     @brief Initialize a tThreshold to the default mempool of a LEAF instance.
     @param threshold A pointer to the tThreshold to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tThreshold_initToPool  (tThreshold* const, float low, float high, tMempool* const)
     @brief Initialize a tThreshold to a specified mempool.
     @param threshold A pointer to the tThreshold to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tThreshold_free        (tThreshold* const)
     @brief Free a tThreshold from its mempool.
     @param threshold A pointer to the tThreshold to free.
     
     @fn int   tThreshold_tick        (tThreshold* const, float input)
     @brief
     @param threshold A pointer to the relevant tThreshold.
     
     @fn void   tThreshold_setLow        (tThreshold* const, float low)
     @brief
     @param threshold A pointer to the relevant tThreshold.
     
     @fn void   tThreshold_setHigh       (tThreshold* const, float high)
     @brief
     @param threshold A pointer to the relevant tThreshold.
     
     @} */

    typedef struct _tThreshold
    {
        
        tMempool mempool;
        float highThresh, lowThresh;
		int currentValue;
    } _tThreshold;

    typedef _tThreshold* tThreshold;

    void    tThreshold_init        (tThreshold* const, float low, float high, LEAF* const leaf);
    void    tThreshold_initToPool  (tThreshold* const, float low, float high, tMempool* const);
    void    tThreshold_free        (tThreshold* const);

    int    tThreshold_tick        (tThreshold* const, float input);
    void   tThreshold_setLow        (tThreshold* const, float low);
    void   tThreshold_setHigh       (tThreshold* const, float high);



    //////======================================================================

#ifdef __cplusplus
}
#endif

#endif // LEAF_DYNAMICS_H_INCLUDED

//==============================================================================

