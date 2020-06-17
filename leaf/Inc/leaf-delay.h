/*==============================================================================
 
 leaf-delay.h
 Created: 20 Jan 2017 12:01:24pm
 Author:  Michael R Mulshine
 
 ==============================================================================*/

#ifndef LEAF_DELAY_H_INCLUDED
#define LEAF_DELAY_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-math.h"
#include "leaf-mempool.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    /* Non-interpolating delay, reimplemented from STK (Cook and Scavone). */
    typedef struct _tDelay
    {
        tMempool mempool;
        
        float gain;
        float* buff;
        
        float lastOut, lastIn;
        
        uint32_t inPoint, outPoint;
        
        uint32_t delay, maxDelay;
        
    } _tDelay;
    
    typedef _tDelay* tDelay;
    
    void        tDelay_init         (tDelay* const, uint32_t delay, uint32_t maxDelay);
    void        tDelay_initToPool   (tDelay* const, uint32_t delay, uint32_t maxDelay, tMempool* const);
    void        tDelay_free         (tDelay* const);
    
    void        tDelay_clear        (tDelay* const);
    int         tDelay_setDelay     (tDelay* const, uint32_t delay);
    uint32_t    tDelay_getDelay     (tDelay* const);
    void        tDelay_tapIn        (tDelay* const, float in, uint32_t tapDelay);
    float       tDelay_tapOut       (tDelay* const, uint32_t tapDelay);
    float       tDelay_addTo        (tDelay* const, float value, uint32_t tapDelay);
    float       tDelay_tick         (tDelay* const, float sample);
    float       tDelay_getLastOut   (tDelay* const);
    float       tDelay_getLastIn    (tDelay* const);
    
    //==============================================================================
    
    /* Linearly-interpolating delay, reimplemented from STK (Cook and Scavone). */
    typedef struct _tLinearDelay
    {
        tMempool mempool;
        
        float gain;
        float* buff;
        
        float lastOut, lastIn;
        
        uint32_t inPoint, outPoint;
        
        uint32_t maxDelay;
        
        float delay;
        
        float alpha, omAlpha;
        
    } _tLinearDelay;
    
    typedef _tLinearDelay* tLinearDelay;
    
    void    tLinearDelay_init        (tLinearDelay* const, float delay, uint32_t maxDelay);
    void    tLinearDelay_initToPool  (tLinearDelay* const, float delay, uint32_t maxDelay, tMempool* const);
    void    tLinearDelay_free        (tLinearDelay* const);
    
    void    tLinearDelay_clear		 (tLinearDelay* const dl);
    int     tLinearDelay_setDelay    (tLinearDelay* const, float delay);
    float   tLinearDelay_getDelay    (tLinearDelay* const);
    void    tLinearDelay_tapIn       (tLinearDelay* const, float in, uint32_t tapDelay);
    float 	tLinearDelay_tapOut 	 (tLinearDelay* const, uint32_t tapDelay);
    float   tLinearDelay_addTo       (tLinearDelay* const, float value, uint32_t tapDelay);
    float   tLinearDelay_tick        (tLinearDelay* const, float sample);
    void    tLinearDelay_tickIn      (tLinearDelay* const, float input);
    float   tLinearDelay_tickOut     (tLinearDelay* const);
    float   tLinearDelay_getLastOut  (tLinearDelay* const);
    float   tLinearDelay_getLastIn   (tLinearDelay* const);
    


    //==============================================================================

    /* Hermite-interpolating delay, created by adapting STK linear delay with Hermite interpolation */
    typedef struct _tHermiteDelay
    {
        tMempool mempool;
        
        float gain;
        float* buff;

        float lastOut, lastIn;

        uint32_t inPoint, outPoint;

        uint32_t maxDelay;

        float delay;

        float alpha, omAlpha;
    } _tHermiteDelay;

    typedef _tHermiteDelay* tHermiteDelay;

    void   	tHermiteDelay_init 			(tHermiteDelay* const dl, float delay, uint32_t maxDelay);
    void    tHermiteDelay_initToPool  	(tHermiteDelay* const dl, float delay, uint32_t maxDelay, tMempool* const mp);
    void     tHermiteDelay_free            (tHermiteDelay* const dl);
    
    void    tHermiteDelay_clear			(tHermiteDelay* const dl);
    float   tHermiteDelay_tick 			(tHermiteDelay* const dl, float input);
    void   	tHermiteDelay_tickIn 		(tHermiteDelay* const dl, float input);
    float   tHermiteDelay_tickOut 		(tHermiteDelay* const dl);
    int     tHermiteDelay_setDelay 		(tHermiteDelay* const dl, float delay);
    float 	tHermiteDelay_tapOut 		(tHermiteDelay* const dl, uint32_t tapDelay);
    void 	tHermiteDelay_tapIn 		(tHermiteDelay* const dl, float value, uint32_t tapDelay);
    float 	tHermiteDelay_addTo 		(tHermiteDelay* const dl, float value, uint32_t tapDelay);
    float   tHermiteDelay_getDelay 		(tHermiteDelay* const dl);
    float   tHermiteDelay_getLastOut 	(tHermiteDelay* const dl);
    float   tHermiteDelay_getLastIn 	(tHermiteDelay* const dl);
    void 	tHermiteDelay_setGain 		(tHermiteDelay* const dl, float gain);
    float 	tHermiteDelay_getGain 		(tHermiteDelay* const dl);


    //==============================================================================
    
    /* Allpass-interpolating delay, reimplemented from STK (Cook and Scavone). */
    typedef struct _tAllpassDelay
    {
        tMempool mempool;
        
        float gain;
        float* buff;
        
        float lastOut, lastIn;
        
        uint32_t inPoint, outPoint;
        
        uint32_t maxDelay;
        
        float delay;
        
        float alpha, omAlpha, coeff;
        
        float apInput;
        
    } _tAllpassDelay;
    
    typedef _tAllpassDelay* tAllpassDelay;
    
    void    tAllpassDelay_init        (tAllpassDelay* const, float delay, uint32_t maxDelay);
    void    tAllpassDelay_initToPool  (tAllpassDelay* const, float delay, uint32_t maxDelay, tMempool* const);
    void    tAllpassDelay_free        (tAllpassDelay* const);
    
    void    tAllpassDelay_clear       (tAllpassDelay* const);
    int     tAllpassDelay_setDelay    (tAllpassDelay* const, float delay);
    float   tAllpassDelay_getDelay    (tAllpassDelay* const);
    void    tAllpassDelay_tapIn       (tAllpassDelay* const, float in, uint32_t tapDelay);
    float   tAllpassDelay_tapOut      (tAllpassDelay* const, uint32_t tapDelay);
    float   tAllpassDelay_addTo       (tAllpassDelay* const, float value, uint32_t tapDelay);
    float   tAllpassDelay_tick        (tAllpassDelay* const, float sample);
    float   tAllpassDelay_getLastOut  (tAllpassDelay* const);
    float   tAllpassDelay_getLastIn   (tAllpassDelay* const);
    
    //==============================================================================
    
    /* Linear interpolating delay with fixed read and write pointers, variable rate. */
    typedef struct _tTapeDelay
    {
        tMempool mempool;
        
        float gain;
        float* buff;
        
        float lastOut, lastIn;
        
        uint32_t inPoint;
        
        uint32_t maxDelay;
        
        float delay, inc, idx;
        
        float apInput;
        
    } _tTapeDelay;
    
    typedef _tTapeDelay* tTapeDelay;
    
    void    tTapeDelay_init        (tTapeDelay* const, float delay, uint32_t maxDelay);
    void    tTapeDelay_initToPool  (tTapeDelay* const, float delay, uint32_t maxDelay, tMempool* const);
    void    tTapeDelay_free        (tTapeDelay* const);
    
    void    tTapeDelay_clear       (tTapeDelay* const);
    void    tTapeDelay_setDelay    (tTapeDelay* const, float delay);
    float   tTapeDelay_getDelay    (tTapeDelay* const);
    void    tTapeDelay_tapIn       (tTapeDelay* const, float in, uint32_t tapDelay);
    float   tTapeDelay_tapOut      (tTapeDelay* const d, float tapDelay);
    float   tTapeDelay_addTo       (tTapeDelay* const, float value, uint32_t tapDelay);
    float   tTapeDelay_tick        (tTapeDelay* const, float sample);
    void    tTapeDelay_incrementInPoint(tTapeDelay* const dl);
    float   tTapeDelay_getLastOut  (tTapeDelay* const);
    float   tTapeDelay_getLastIn   (tTapeDelay* const);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_DELAY_H_INCLUDED

//==============================================================================

