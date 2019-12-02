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
    
    //==============================================================================
    
    /* Non-interpolating delay, reimplemented from STK (Cook and Scavone). */
    typedef struct _tDelay
    {
        float gain;
        float* buff;
        
        float lastOut, lastIn;
        
        uint32_t inPoint, outPoint;
        
        uint32_t delay, maxDelay;
        
    } _tDelay;
    
    typedef _tDelay* tDelay;
    
    void        tDelay_init         (tDelay*  const, uint32_t delay, uint32_t maxDelay);
    void        tDelay_free         (tDelay* const);
    
    int         tDelay_setDelay     (tDelay*  const, uint32_t delay);
    uint32_t    tDelay_getDelay     (tDelay*  const);
    void        tDelay_tapIn        (tDelay*  const, float in, uint32_t tapDelay);
    float       tDelay_tapOut       (tDelay*  const, uint32_t tapDelay);
    float       tDelay_addTo        (tDelay*  const, float value, uint32_t tapDelay);
    float       tDelay_tick         (tDelay*  const, float sample);
    float       tDelay_getLastOut   (tDelay*  const);
    float       tDelay_getLastIn    (tDelay*  const);
    
    //==============================================================================
    
    /* Linearly-interpolating delay, reimplemented from STK (Cook and Scavone). */
    typedef struct _tLinearDelay
    {
        float gain;
        float* buff;
        
        float lastOut, lastIn;
        
        uint32_t inPoint, outPoint;
        
        uint32_t maxDelay;
        
        float delay;
        
        float alpha, omAlpha;
        
    } _tLinearDelay;
    
    typedef _tLinearDelay* tLinearDelay;
    
    void        tLinearDelay_init        (tLinearDelay*  const, float delay, uint32_t maxDelay);
    void        tLinearDelay_free        (tLinearDelay*  const);
    
    int         tLinearDelay_setDelay    (tLinearDelay*  const, float delay);
    float       tLinearDelay_getDelay    (tLinearDelay*  const);
    void        tLinearDelay_tapIn       (tLinearDelay*  const, float in, uint32_t tapDelay);
    float       tLinearDelay_tapOut      (tLinearDelay*  const, float tapDelay);
    float       tLinearDelay_addTo       (tLinearDelay*  const, float value, uint32_t tapDelay);
    float       tLinearDelay_tick        (tLinearDelay*  const, float sample);
    void        tLinearDelay_tickIn      (tLinearDelay*  const, float input);
    float       tLinearDelay_tickOut     (tLinearDelay*  const);
    float       tLinearDelay_getLastOut  (tLinearDelay*  const);
    float       tLinearDelay_getLastIn   (tLinearDelay*  const);
    
    //==============================================================================
    
    /* Allpass-interpolating delay, reimplemented from STK (Cook and Scavone). */
    typedef struct _tAllpassDelay
    {
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
    
    void        tAllpassDelay_init        (tAllpassDelay*  const, float delay, uint32_t maxDelay);
    void        tAllpassDelay_free        (tAllpassDelay* const);
    
    int         tAllpassDelay_setDelay    (tAllpassDelay*  const, float delay);
    float       tAllpassDelay_getDelay    (tAllpassDelay*  const);
    void        tAllpassDelay_tapIn       (tAllpassDelay*  const, float in, uint32_t tapDelay);
    float       tAllpassDelay_tapOut      (tAllpassDelay*  const, uint32_t tapDelay);
    float       tAllpassDelay_addTo       (tAllpassDelay*  const, float value, uint32_t tapDelay);
    float       tAllpassDelay_tick        (tAllpassDelay*  const, float sample);
    float       tAllpassDelay_getLastOut  (tAllpassDelay*  const);
    float       tAllpassDelay_getLastIn   (tAllpassDelay*  const);
    
    //==============================================================================
    
    /* Linear interpolating delay with fixed read and write pointers, variable rate. */
    typedef struct _tTapeDelay
    {
        float gain;
        float* buff;
        
        float lastOut, lastIn;
        
        uint32_t inPoint;
        
        uint32_t maxDelay;
        
        float delay, inc, idx;
        
        float apInput;
        
    } _tTapeDelay;
    
    typedef _tTapeDelay* tTapeDelay;
    
    void        tTapeDelay_init        (tTapeDelay*  const, float delay, uint32_t maxDelay);
    void        tTapeDelay_free        (tTapeDelay* const);
    
    void         tTapeDelay_setDelay    (tTapeDelay*  const, float delay);
    float       tTapeDelay_getDelay    (tTapeDelay*  const);
    void        tTapeDelay_tapIn       (tTapeDelay*  const, float in, uint32_t tapDelay);
    float       tTapeDelay_tapOut      (tTapeDelay* const d, float tapDelay);
    float       tTapeDelay_addTo       (tTapeDelay*  const, float value, uint32_t tapDelay);
    float       tTapeDelay_tick        (tTapeDelay*  const, float sample);
    float       tTapeDelay_getLastOut  (tTapeDelay*  const);
    float       tTapeDelay_getLastIn   (tTapeDelay*  const);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_DELAY_H_INCLUDED

//==============================================================================

