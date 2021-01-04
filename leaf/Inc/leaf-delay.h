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
     @brief Initialize a tDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tDelay to initialize.
     @param initalLength
     @param maxLength
     @param leaf A pointer to the leaf instance.
     
     @fn void        tDelay_initToPool   (tDelay* const, uint32_t delay, uint32_t maxDelay, tMempool* const)
     @brief Initialize a tDelay to a specified mempool.
     @param delay A pointer to the tDelay to initialize.
     @param initalLength
     @param maxLength
     @param mempool A pointer to the tMempool to use.
     
     @fn void        tDelay_free         (tDelay* const)
     @brief Free a tDelay from its mempool.
     @param delay A pointer to the tDelay to free.
     
     @fn void        tDelay_clear        (tDelay* const)
     @brief
     @param delay A pointer to the relevant tDelay.
     
     @fn void        tDelay_setDelay     (tDelay* const, uint32_t delay)
     @brief
     @param delay A pointer to the relevant tDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.
     
     @fn uint32_t    tDelay_getDelay     (tDelay* const)
     @brief
     @param delay A pointer to the relevant tDelay.
     @return The current delay length.
     
     @fn void        tDelay_tapIn        (tDelay* const, float in, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tDelay.
     @param input
     @param position
     
     @fn float       tDelay_tapOut       (tDelay* const, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tDelay.
     @param position
     @return
     
     @fn float       tDelay_addTo        (tDelay* const, float value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tDelay.
     @param input
     @param position
     @return
     
     @fn float       tDelay_tick         (tDelay* const, float sample)
     @brief
     @param delay A pointer to the relevant tDelay.
     @param input
     @return
     
     @fn float       tDelay_getLastOut   (tDelay* const)
     @brief
     @param delay A pointer to the relevant tDelay.
     @return
     
     @fn float       tDelay_getLastIn    (tDelay* const)
     @brief
     @param delay A pointer to the relevant tDelay.
     @return
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
    void        tDelay_setDelay     (tDelay* const, uint32_t delay);
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
     @brief Initialize a tLinearDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tLinearDelay to initialize.
     @param initialLength
     @param maxLength
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLinearDelay_initToPool  (tLinearDelay* const, float delay, uint32_t maxDelay, tMempool* const)
     @brief Initialize a tLinearDelay to a specified mempool.
     @param delay A pointer to the tLinearDelay to initialize.
     @param initialLength
     @param maxLength
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tLinearDelay_free        (tLinearDelay* const)
     @brief Free a tLinearDelay from its mempool.
     @param delay A pointer to the tLinearDelay to free.
     
     @fn void    tLinearDelay_clear         (tLinearDelay* const dl)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     
     @fn void   tLinearDelay_setDelay    (tLinearDelay* const, float delay)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.
     
     @fn float   tLinearDelay_getDelay    (tLinearDelay* const)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @return
     
     @fn void    tLinearDelay_tapIn       (tLinearDelay* const, float in, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param input
     @param position
     
     @fn float     tLinearDelay_tapOut      (tLinearDelay* const, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param position
     @return
     
     @fn float   tLinearDelay_addTo       (tLinearDelay* const, float value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param input
     @param position
     @return
     
     @fn float   tLinearDelay_tick        (tLinearDelay* const, float sample)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param input
     @return
     
     @fn void    tLinearDelay_tickIn      (tLinearDelay* const, float input)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param input
     
     @fn float   tLinearDelay_tickOut     (tLinearDelay* const)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @return
     
     @fn float   tLinearDelay_getLastOut  (tLinearDelay* const)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @return
     
     @fn float   tLinearDelay_getLastIn   (tLinearDelay* const)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @return
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
    void    tLinearDelay_setDelay    (tLinearDelay* const, float delay);
    float   tLinearDelay_getDelay    (tLinearDelay* const);
    void    tLinearDelay_tapIn       (tLinearDelay* const, float in, uint32_t tapDelay);
    float   tLinearDelay_tapOut      (tLinearDelay* const, uint32_t tapDelay);
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
     @brief Initialize a tHermiteDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tHermiteDelay to initialize.
     @param initialLength
     @param maxLength
     @param leaf A pointer to the leaf instance.
     
     @fn void    tHermiteDelay_initToPool      (tHermiteDelay* const dl, float delay, uint32_t maxDelay, tMempool* const mp)
     @brief Initialize a tHermiteDelay to a specified mempool.
     @param delay A pointer to the tHermiteDelay to initialize.
     @param initialLength
     @param maxLength
     @param mempool A pointer to the tMempool to use.
     
     @fn void     tHermiteDelay_free            (tHermiteDelay* const dl)
     @brief Free a tHermiteDelay from its mempool.
     @param delay A pointer to the tHermiteDelay to free.
     
     @fn void    tHermiteDelay_clear            (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     
     @fn float   tHermiteDelay_tick             (tHermiteDelay* const dl, float input)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     @return
     
     @fn void       tHermiteDelay_tickIn         (tHermiteDelay* const dl, float input)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     
     @fn float   tHermiteDelay_tickOut         (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
     
     @fn void    tHermiteDelay_setDelay         (tHermiteDelay* const dl, float delay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.
     
     @fn float     tHermiteDelay_tapOut         (tHermiteDelay* const dl, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param position
     @return
     
     @fn void     tHermiteDelay_tapIn         (tHermiteDelay* const dl, float value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     @param position
     
     @fn float     tHermiteDelay_addTo         (tHermiteDelay* const dl, float value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     @param position
     @return
     
     @fn float   tHermiteDelay_getDelay         (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
     
     @fn float   tHermiteDelay_getLastOut     (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
     
     @fn float   tHermiteDelay_getLastIn     (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
     
     @fn void     tHermiteDelay_setGain         (tHermiteDelay* const dl, float gain)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param gain
     
     @fn float     tHermiteDelay_getGain         (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
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
    
    void    tHermiteDelay_init (tHermiteDelay* const dl, float delay, uint32_t maxDelay, LEAF* const leaf);
    void    tHermiteDelay_initToPool (tHermiteDelay* const dl, float delay, uint32_t maxDelay, tMempool* const mp);
    void    tHermiteDelay_free          (tHermiteDelay* const dl);
    
    void    tHermiteDelay_clear         (tHermiteDelay* const dl);
    float   tHermiteDelay_tick          (tHermiteDelay* const dl, float input);
    void    tHermiteDelay_tickIn        (tHermiteDelay* const dl, float input);
    float   tHermiteDelay_tickOut       (tHermiteDelay* const dl);
    void    tHermiteDelay_setDelay      (tHermiteDelay* const dl, float delay);
    float   tHermiteDelay_tapOut        (tHermiteDelay* const dl, uint32_t tapDelay);
    void    tHermiteDelay_tapIn         (tHermiteDelay* const dl, float value, uint32_t tapDelay);
    float   tHermiteDelay_addTo         (tHermiteDelay* const dl, float value, uint32_t tapDelay);
    float   tHermiteDelay_getDelay      (tHermiteDelay* const dl);
    float   tHermiteDelay_getLastOut    (tHermiteDelay* const dl);
    float   tHermiteDelay_getLastIn     (tHermiteDelay* const dl);
    void    tHermiteDelay_setGain       (tHermiteDelay* const dl, float gain);
    float   tHermiteDelay_getGain       (tHermiteDelay* const dl);
    
    
    //==============================================================================
    
    /*!
     @defgroup tallpassdelay tAllpassDelay
     @ingroup delay
     @brief Allpass-interpolating delay, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tAllpassDelay_init        (tAllpassDelay* const, float delay, uint32_t maxDelay, LEAF* const leaf)
     @brief Initialize a tAllpassDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tAllpassDelay to initialize.
     @param initialLength
     @param maxLength
     @param leaf A pointer to the leaf instance.
     
     @fn void    tAllpassDelay_initToPool  (tAllpassDelay* const, float delay, uint32_t maxDelay, tMempool* const)
     @brief Initialize a tAllpassDelay to a specified mempool.
     @param delay A pointer to the tAllpassDelay to initialize.
     @param initialLength
     @param maxLength
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tAllpassDelay_free        (tAllpassDelay* const)
     @brief Free a tAllpassDelay from its mempool.
     @param delay A pointer to the tAllpassDelay to free.
     
     @fn void    tAllpassDelay_clear       (tAllpassDelay* const)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     
     @fn void    tAllpassDelay_setDelay    (tAllpassDelay* const, float delay)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.
     
     @fn float   tAllpassDelay_getDelay    (tAllpassDelay* const)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @return The current delay length in samples.
     
     @fn void    tAllpassDelay_tapIn       (tAllpassDelay* const, float in, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param inptu
     @param position
     
     @fn float   tAllpassDelay_tapOut      (tAllpassDelay* const, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param position
     @return
     
     @fn float   tAllpassDelay_addTo       (tAllpassDelay* const, float value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param input
     @param position
     @return
     
     @fn float   tAllpassDelay_tick        (tAllpassDelay* const, float sample)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param input
     @return
     
     @fn float   tAllpassDelay_getLastOut  (tAllpassDelay* const)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @return
     
     @fn float   tAllpassDelay_getLastIn   (tAllpassDelay* const)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @return
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
    void    tAllpassDelay_setDelay    (tAllpassDelay* const, float delay);
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
     @brief Initialize a tTapeDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tTapeDelay to initialize.
     @param initialLength
     @param maxLength
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTapeDelay_initToPool  (tTapeDelay* const, float delay, uint32_t maxDelay, tMempool* const)
     @brief Initialize a tTapeDelay to a specified mempool.
     @param delay A pointer to the tTapeDelay to initialize.
     @param initialLength
     @param maxLength
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTapeDelay_free        (tTapeDelay* const)
     @brief Free a tTapeDelay from its mempool.
     @param delay A pointer to the tTapeDelay to free.
     
     @fn void    tTapeDelay_clear       (tTapeDelay* const)
     @brief 
     @param delay A pointer to the relevant tTapeDelay.
     
     @fn void    tTapeDelay_setDelay    (tTapeDelay* const, float delay)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.
     
     @fn float   tTapeDelay_getDelay    (tTapeDelay* const)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @return
     
     @fn void    tTapeDelay_tapIn       (tTapeDelay* const, float in, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param input
     @param position
     
     @fn float   tTapeDelay_tapOut      (tTapeDelay* const d, float tapDelay)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param position
     @return
     
     @fn float   tTapeDelay_addTo       (tTapeDelay* const, float value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param input
     @param position
     @return
     
     @fn float   tTapeDelay_tick        (tTapeDelay* const, float sample)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param input
     @return
     
     @fn void    tTapeDelay_incrementInPoint(tTapeDelay* const dl)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     
     @fn float   tTapeDelay_getLastOut  (tTapeDelay* const)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @return
     
     @fn float   tTapeDelay_getLastIn   (tTapeDelay* const)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @return
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
     @brief Ring buffer.
     @{
     
     @fn void    tRingBuffer_init     (tRingBuffer* const ring, int size, LEAF* const leaf)
     @brief Initialize a tRingBuffer to the default mempool of a LEAF instance.
     @param buffer A pointer to the tRingbuffer to initialize.
     @param size Size of the buffer. Should be a power of 2. Will otherwise be adjusted to the nearest greater power of 2.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tRingBuffer_initToPool   (tRingBuffer* const ring, int size, tMempool* const mempool)
     @brief Initialize a tRingBuffer to a specified mempool.
     @param buffer A pointer to the tRingbuffer to initialize.
     @param size Size of the buffer. Should be a power of 2. Will otherwise be adjusted to the nearest greater power of 2.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tRingBuffer_free     (tRingBuffer* const ring)
     @brief Free a tRingBuffer from its mempool.
     @param buffer A pointer to the tRingBuffer to free.
     
     @fn void    tRingBuffer_push     (tRingBuffer* const ring, float val)
     @brief Push a value to the ring buffer, overwriting the oldest value if the buffer is full.
     @param buffer A pointer to the relevant tRingBuffer.
     @param input The value to push to the buffer.
     
     @fn float   tRingBuffer_getNewest    (tRingBuffer* const ring)
     @brief Get the newest value in the ring buffer.
     @param buffer A pointer to the relevant tRingBuffer.
     @return The newest value in the ring buffer.
     
     @fn float   tRingBuffer_getOldest    (tRingBuffer* const ring)
     @brief Get the oldest value in the ring buffer.
     @param buffer A pointer to the relevant tRingBuffer.
     @return The oldest value in the ring buffer.
     
     @fn float   tRingBuffer_get      (tRingBuffer* const ring, int index)
     @brief Get the value at an index of the buffer.
     @param buffer A pointer to the relevant tRingBuffer.
     @param index The index to access.
     @return The value at the given index.

     @fn int     tRingBuffer_getSize  (tRingBuffer* const ring)
     @brief Get the size of the ring buffer.
     @param buffer A pointer to the relevant tRingBuffer.
     @return The size of the buffer.
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
    int     tRingBuffer_getSize  (tRingBuffer* const ring);
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_DELAY_H_INCLUDED

//==============================================================================


