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
    
    /*!
     @defgroup tdelay tDelay
     @ingroup delay
     @brief Non-interpolating delay, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void        tDelay_init         (tDelay* const, uint32_t delay, uint32_t maxDelay, LEAF* const leaf)
     @brief
     @param
     
     @fn void        tDelay_initToPool   (tDelay* const, uint32_t delay, uint32_t maxDelay, tMempool* const)
     @brief
     @param
     
     @fn void        tDelay_free         (tDelay* const)
     @brief
     @param
     
     @fn void        tDelay_clear        (tDelay* const)
     @brief
     @param
     
     @fn int         tDelay_setDelay     (tDelay* const, uint32_t delay)
     @brief
     @param
     
     @fn uint32_t    tDelay_getDelay     (tDelay* const)
     @brief
     @param
     
     @fn void        tDelay_tapIn        (tDelay* const, float in, uint32_t tapDelay)
     @brief
     @param
     
     @fn float       tDelay_tapOut       (tDelay* const, uint32_t tapDelay)
     @brief
     @param
     
     @fn float       tDelay_addTo        (tDelay* const, float value, uint32_t tapDelay)
     @brief
     @param
     
     @fn float       tDelay_tick         (tDelay* const, float sample)
     @brief
     @param
     
     @fn float       tDelay_getLastOut   (tDelay* const)
     @brief
     @param
     
     @fn float       tDelay_getLastIn    (tDelay* const)
     @brief
     @param
     ￼￼￼
     @} */
    
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
    
    void        tDelay_init         (tDelay* const, uint32_t delay, uint32_t maxDelay, LEAF* const leaf);
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
    
    /*!
     @defgroup tlineardelay tLinearDelay
     @ingroup delay
     @brief Linearly-interpolating delay, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tLinearDelay_init        (tLinearDelay* const, float delay, uint32_t maxDelay, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tLinearDelay_initToPool  (tLinearDelay* const, float delay, uint32_t maxDelay, tMempool* const)
     @brief
     @param
     
     @fn void    tLinearDelay_free        (tLinearDelay* const)
     @brief
     @param
     
     @fn void    tLinearDelay_clear         (tLinearDelay* const dl)
     @brief
     @param
     
     @fn int     tLinearDelay_setDelay    (tLinearDelay* const, float delay)
     @brief
     @param
     
     @fn float   tLinearDelay_getDelay    (tLinearDelay* const)
     @brief
     @param
     
     @fn void    tLinearDelay_tapIn       (tLinearDelay* const, float in, uint32_t tapDelay)
     @brief
     @param
     
     @fn float     tLinearDelay_tapOut      (tLinearDelay* const, uint32_t tapDelay)
     @brief
     @param
     
     @fn float   tLinearDelay_addTo       (tLinearDelay* const, float value, uint32_t tapDelay)
     @brief
     @param
     
     @fn float   tLinearDelay_tick        (tLinearDelay* const, float sample)
     @brief
     @param
     
     @fn void    tLinearDelay_tickIn      (tLinearDelay* const, float input)
     @brief
     @param
     
     @fn float   tLinearDelay_tickOut     (tLinearDelay* const)
     @brief
     @param
     
     @fn float   tLinearDelay_getLastOut  (tLinearDelay* const)
     @brief
     @param
     
     @fn float   tLinearDelay_getLastIn   (tLinearDelay* const)
     @brief
     @param
     ￼￼￼
     @} */
    
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
    
    void    tLinearDelay_init        (tLinearDelay* const, float delay, uint32_t maxDelay, LEAF* const leaf);
    void    tLinearDelay_initToPool  (tLinearDelay* const, float delay, uint32_t maxDelay, tMempool* const);
    void    tLinearDelay_free        (tLinearDelay* const);
    
    void    tLinearDelay_clear         (tLinearDelay* const dl);
    int     tLinearDelay_setDelay    (tLinearDelay* const, float delay);
    float   tLinearDelay_getDelay    (tLinearDelay* const);
    void    tLinearDelay_tapIn       (tLinearDelay* const, float in, uint32_t tapDelay);
    float     tLinearDelay_tapOut      (tLinearDelay* const, uint32_t tapDelay);
    float   tLinearDelay_addTo       (tLinearDelay* const, float value, uint32_t tapDelay);
    float   tLinearDelay_tick        (tLinearDelay* const, float sample);
    void    tLinearDelay_tickIn      (tLinearDelay* const, float input);
    float   tLinearDelay_tickOut     (tLinearDelay* const);
    float   tLinearDelay_getLastOut  (tLinearDelay* const);
    float   tLinearDelay_getLastIn   (tLinearDelay* const);
    
    
    
    //==============================================================================
    
    /*!
     @defgroup thermitedelay tHermiteDelay
     @ingroup delay
     @brief Hermite-interpolating delay, created by adapting STK linear delay with Hermite interpolation.
     @{
     
     @fn void       tHermiteDelay_init             (tHermiteDelay* const dl, float delay, uint32_t maxDelay, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tHermiteDelay_initToPool      (tHermiteDelay* const dl, float delay, uint32_t maxDelay, tMempool* const mp)
     @brief
     @param
     
     @fn void     tHermiteDelay_free            (tHermiteDelay* const dl)
     @brief
     @param
     
     @fn void    tHermiteDelay_clear            (tHermiteDelay* const dl)
     @brief
     @param
     
     @fn float   tHermiteDelay_tick             (tHermiteDelay* const dl, float input)
     @brief
     @param
     
     @fn void       tHermiteDelay_tickIn         (tHermiteDelay* const dl, float input)
     @brief
     @param
     
     @fn float   tHermiteDelay_tickOut         (tHermiteDelay* const dl)
     @brief
     @param
     
     @fn int     tHermiteDelay_setDelay         (tHermiteDelay* const dl, float delay)
     @brief
     @param
     
     @fn float     tHermiteDelay_tapOut         (tHermiteDelay* const dl, uint32_t tapDelay)
     @brief
     @param
     
     @fn void     tHermiteDelay_tapIn         (tHermiteDelay* const dl, float value, uint32_t tapDelay)
     @brief
     @param
     
     @fn float     tHermiteDelay_addTo         (tHermiteDelay* const dl, float value, uint32_t tapDelay)
     @brief
     @param
     
     @fn float   tHermiteDelay_getDelay         (tHermiteDelay* const dl)
     @brief
     @param
     
     @fn float   tHermiteDelay_getLastOut     (tHermiteDelay* const dl)
     @brief
     @param
     
     @fn float   tHermiteDelay_getLastIn     (tHermiteDelay* const dl)
     @brief
     @param
     
     @fn void     tHermiteDelay_setGain         (tHermiteDelay* const dl, float gain)
     @brief
     @param
     
     @fn float     tHermiteDelay_getGain         (tHermiteDelay* const dl)
     @brief
     @param
     ￼￼￼
     @} */
    
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
    
    void       tHermiteDelay_init             (tHermiteDelay* const dl, float delay, uint32_t maxDelay, LEAF* const leaf);
    void    tHermiteDelay_initToPool      (tHermiteDelay* const dl, float delay, uint32_t maxDelay, tMempool* const mp);
    void     tHermiteDelay_free            (tHermiteDelay* const dl);
    
    void    tHermiteDelay_clear            (tHermiteDelay* const dl);
    float   tHermiteDelay_tick             (tHermiteDelay* const dl, float input);
    void       tHermiteDelay_tickIn         (tHermiteDelay* const dl, float input);
    float   tHermiteDelay_tickOut         (tHermiteDelay* const dl);
    int     tHermiteDelay_setDelay         (tHermiteDelay* const dl, float delay);
    float     tHermiteDelay_tapOut         (tHermiteDelay* const dl, uint32_t tapDelay);
    void     tHermiteDelay_tapIn         (tHermiteDelay* const dl, float value, uint32_t tapDelay);
    float     tHermiteDelay_addTo         (tHermiteDelay* const dl, float value, uint32_t tapDelay);
    float   tHermiteDelay_getDelay         (tHermiteDelay* const dl);
    float   tHermiteDelay_getLastOut     (tHermiteDelay* const dl);
    float   tHermiteDelay_getLastIn     (tHermiteDelay* const dl);
    void     tHermiteDelay_setGain         (tHermiteDelay* const dl, float gain);
    float     tHermiteDelay_getGain         (tHermiteDelay* const dl);
    
    
    //==============================================================================
    
    /*!
     @defgroup tallpassdelay tAllpassDelay
     @ingroup delay
     @brief Allpass-interpolating delay, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tAllpassDelay_init        (tAllpassDelay* const, float delay, uint32_t maxDelay, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tAllpassDelay_initToPool  (tAllpassDelay* const, float delay, uint32_t maxDelay, tMempool* const)
     @brief
     @param
     
     @fn void    tAllpassDelay_free        (tAllpassDelay* const)
     @brief
     @param
     
     @fn void    tAllpassDelay_clear       (tAllpassDelay* const)
     @brief
     @param
     
     @fn int     tAllpassDelay_setDelay    (tAllpassDelay* const, float delay)
     @brief
     @param
     
     @fn float   tAllpassDelay_getDelay    (tAllpassDelay* const)
     @brief
     @param
     
     @fn void    tAllpassDelay_tapIn       (tAllpassDelay* const, float in, uint32_t tapDelay)
     @brief
     @param
     
     @fn float   tAllpassDelay_tapOut      (tAllpassDelay* const, uint32_t tapDelay)
     @brief
     @param
     
     @fn float   tAllpassDelay_addTo       (tAllpassDelay* const, float value, uint32_t tapDelay)
     @brief
     @param
     
     @fn float   tAllpassDelay_tick        (tAllpassDelay* const, float sample)
     @brief
     @param
     
     @fn float   tAllpassDelay_getLastOut  (tAllpassDelay* const)
     @brief
     @param
     
     @fn float   tAllpassDelay_getLastIn   (tAllpassDelay* const)
     @brief
     @param
     ￼￼￼
     @} */
    
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
    
    void    tAllpassDelay_init        (tAllpassDelay* const, float delay, uint32_t maxDelay, LEAF* const leaf);
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
    
    /*!
     @defgroup ttapedelay tTapeDelay
     @ingroup delay
     @brief Linear interpolating delay with fixed read and write pointers, variable rate.
     @{
     
     @fn void    tTapeDelay_init        (tTapeDelay* const, float delay, uint32_t maxDelay, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tTapeDelay_initToPool  (tTapeDelay* const, float delay, uint32_t maxDelay, tMempool* const)
     @brief
     @param
     
     @fn void    tTapeDelay_free        (tTapeDelay* const)
     @brief
     @param
     
     @fn void    tTapeDelay_clear       (tTapeDelay* const)
     @brief
     @param
     
     @fn void    tTapeDelay_setDelay    (tTapeDelay* const, float delay)
     @brief
     @param
     
     @fn float   tTapeDelay_getDelay    (tTapeDelay* const)
     @brief
     @param
     
     @fn void    tTapeDelay_tapIn       (tTapeDelay* const, float in, uint32_t tapDelay)
     @brief
     @param
     
     @fn float   tTapeDelay_tapOut      (tTapeDelay* const d, float tapDelay)
     @brief
     @param
     
     @fn float   tTapeDelay_addTo       (tTapeDelay* const, float value, uint32_t tapDelay)
     @brief
     @param
     
     @fn float   tTapeDelay_tick        (tTapeDelay* const, float sample)
     @brief
     @param
     
     @fn void    tTapeDelay_incrementInPoint(tTapeDelay* const dl)
     @brief
     @param
     
     @fn float   tTapeDelay_getLastOut  (tTapeDelay* const)
     @brief
     @param
     
     @fn float   tTapeDelay_getLastIn   (tTapeDelay* const)
     @brief
     @param
     ￼￼￼
     @} */
    
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
    
    void    tTapeDelay_init        (tTapeDelay* const, float delay, uint32_t maxDelay, LEAF* const leaf);
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
    
    /*!
     @defgroup tringbuffer tRingBuffer
     @ingroup delay
     @brief
     @{
     
     @fn void    tRingBuffer_init     (tRingBuffer* const ring, int size, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tRingBuffer_initToPool   (tRingBuffer* const ring, int size, tMempool* const mempool)
     @brief
     @param
     
     @fn void    tRingBuffer_free     (tRingBuffer* const ring)
     @brief
     @param
     
     @fn void    tRingBuffer_push     (tRingBuffer* const ring, float val)
     @brief
     @param
     
     @fn float   tRingBuffer_getNewest    (tRingBuffer* const ring)
     @brief
     @param
     
     @fn float   tRingBuffer_getOldest    (tRingBuffer* const ring)
     @brief
     @param
     
     @fn float   tRingBuffer_get      (tRingBuffer* const ring, int index)
     @brief
     @param
     
     @fn float   tRingBuffer_clear    (tRingBuffer* const ring)
     @brief
     @param
     
     @fn int     tRingBuffer_getSize  (tRingBuffer* const ring)
     @brief
     @param
     ￼￼￼
     @} */
    typedef struct _tRingBuffer
    {
        
        tMempool mempool;
        
        float* buffer;
        unsigned int size;
        unsigned int pos;
        unsigned int mask;
    } _tRingBuffer;
    
    typedef _tRingBuffer* tRingBuffer;
    
    void    tRingBuffer_init     (tRingBuffer* const ring, int size, LEAF* const leaf);
    void    tRingBuffer_initToPool   (tRingBuffer* const ring, int size, tMempool* const mempool);
    void    tRingBuffer_free     (tRingBuffer* const ring);
    
    void    tRingBuffer_push     (tRingBuffer* const ring, float val);
    float   tRingBuffer_getNewest    (tRingBuffer* const ring);
    float   tRingBuffer_getOldest    (tRingBuffer* const ring);
    float   tRingBuffer_get      (tRingBuffer* const ring, int index);
    float   tRingBuffer_clear    (tRingBuffer* const ring);
    int     tRingBuffer_getSize  (tRingBuffer* const ring);
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_DELAY_H_INCLUDED

//==============================================================================


