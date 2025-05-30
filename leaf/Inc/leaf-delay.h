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
     
     @fn void        tDelay_tapIn        (tDelay* const, Lfloat in, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tDelay.
     @param input
     @param position
     
     @fn Lfloat       tDelay_tapOut       (tDelay* const, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tDelay.
     @param position
     @return
     
     @fn Lfloat       tDelay_addTo        (tDelay* const, Lfloat value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tDelay.
     @param input
     @param position
     @return
     
     @fn Lfloat       tDelay_tick         (tDelay* const, Lfloat sample)
     @brief
     @param delay A pointer to the relevant tDelay.
     @param input
     @return
     
     @fn Lfloat       tDelay_getLastOut   (tDelay* const)
     @brief
     @param delay A pointer to the relevant tDelay.
     @return
     
     @fn Lfloat       tDelay_getLastIn    (tDelay* const)
     @brief
     @param delay A pointer to the relevant tDelay.
     @return
     ￼￼￼
     @} */
    
    typedef struct tDelay
    {
        tMempool mempool;
        
        Lfloat gain;
        Lfloat* buff;
        
        Lfloat lastOut, lastIn;
        
        uint32_t inPoint, outPoint;
        
        uint32_t delay, maxDelay;
        
    } tDelay;

    void        tDelay_init         (tDelay** const, uint32_t delay, uint32_t maxDelay, LEAF* const leaf);
    void        tDelay_initToPool   (tDelay** const, uint32_t delay, uint32_t maxDelay, tMempool* const);
    void        tDelay_free         (tDelay** const);

    Lfloat      tDelay_tick         (tDelay* const, Lfloat sample);

    void        tDelay_clear        (tDelay* const);
    void        tDelay_setDelay     (tDelay* const, uint32_t delay);
    uint32_t    tDelay_getDelay     (tDelay* const);
    void        tDelay_tapIn        (tDelay* const, Lfloat in, uint32_t tapDelay);
    Lfloat      tDelay_tapOut       (tDelay* const, uint32_t tapDelay);
    Lfloat      tDelay_addTo        (tDelay* const, Lfloat value, uint32_t tapDelay);
    Lfloat      tDelay_getLastOut   (tDelay* const);
    Lfloat      tDelay_getLastIn    (tDelay* const);
    
    //==============================================================================
    
    /*!
     @defgroup tlineardelay tLinearDelay
     @ingroup delay
     @brief Linearly-interpolating delay, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tLinearDelay_init        (tLinearDelay* const, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
     @brief Initialize a tLinearDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tLinearDelay to initialize.
     @param initialLength
     @param maxLength
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLinearDelay_initToPool  (tLinearDelay* const, Lfloat delay, uint32_t maxDelay, tMempool* const)
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
     
     @fn void   tLinearDelay_setDelay    (tLinearDelay* const, Lfloat delay)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.
     
     @fn Lfloat   tLinearDelay_getDelay    (tLinearDelay* const)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @return
     
     @fn void    tLinearDelay_tapIn       (tLinearDelay* const, Lfloat in, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param input
     @param position
     
     @fn Lfloat     tLinearDelay_tapOut      (tLinearDelay* const, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param position
     @return
     
     @fn Lfloat   tLinearDelay_addTo       (tLinearDelay* const, Lfloat value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param input
     @param position
     @return
     
     @fn Lfloat   tLinearDelay_tick        (tLinearDelay* const, Lfloat sample)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param input
     @return
     
     @fn void    tLinearDelay_tickIn      (tLinearDelay* const, Lfloat input)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @param input
     
     @fn Lfloat   tLinearDelay_tickOut     (tLinearDelay* const)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @return
     
     @fn Lfloat   tLinearDelay_getLastOut  (tLinearDelay* const)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @return
     
     @fn Lfloat   tLinearDelay_getLastIn   (tLinearDelay* const)
     @brief
     @param delay A pointer to the relevant tLinearDelay.
     @return
     ￼￼￼
     @} */
    
    typedef struct tLinearDelay
    {
        tMempool mempool;
        
        Lfloat gain;
        Lfloat* buff;
        
        Lfloat lastOut, lastIn;
        
        uint32_t inPoint, outPoint;
        
        uint32_t maxDelay;
        
        Lfloat delay;
        
        Lfloat alpha, omAlpha;
        
    } tLinearDelay;

    void    tLinearDelay_init               (tLinearDelay** const, Lfloat delay, uint32_t maxDelay, LEAF* const leaf);
    void    tLinearDelay_initToPool         (tLinearDelay** const, Lfloat delay, uint32_t maxDelay, tMempool* const);
    void    tLinearDelay_free               (tLinearDelay** const);

    Lfloat  tLinearDelay_tick               (tLinearDelay* const, Lfloat sample);
    void    tLinearDelay_tickIn             (tLinearDelay* const, Lfloat input);
    Lfloat  tLinearDelay_tickOut            (tLinearDelay* const);

    void    tLinearDelay_clear              (tLinearDelay* const dl);
    void    tLinearDelay_setDelay           (tLinearDelay* const, Lfloat delay);
    Lfloat  tLinearDelay_getDelay           (tLinearDelay* const);
    void    tLinearDelay_tapIn              (tLinearDelay* const, Lfloat in, uint32_t tapDelay);
    Lfloat  tLinearDelay_tapOut             (tLinearDelay* const, uint32_t tapDelay);
    Lfloat  tLinearDelay_tapOutInterpolated (tLinearDelay* const dl, Lfloat tapDelay);
    Lfloat  tLinearDelay_addTo              (tLinearDelay* const, Lfloat value, uint32_t tapDelay);
    Lfloat  tLinearDelay_getLastOut         (tLinearDelay* const);
    Lfloat  tLinearDelay_getLastIn          (tLinearDelay* const);
    
    
    
    //==============================================================================
    
    /*!
     @defgroup thermitedelay tHermiteDelay
     @ingroup delay
     @brief Hermite-interpolating delay, created by adapting STK linear delay with Hermite interpolation.
     @{
     
     @fn void       tHermiteDelay_init             (tHermiteDelay* const dl, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
     @brief Initialize a tHermiteDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tHermiteDelay to initialize.
     @param initialLength
     @param maxLength
     @param leaf A pointer to the leaf instance.
     
     @fn void    tHermiteDelay_initToPool      (tHermiteDelay* const dl, Lfloat delay, uint32_t maxDelay, tMempool* const mp)
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
     
     @fn Lfloat   tHermiteDelay_tick             (tHermiteDelay* const dl, Lfloat input)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     @return
     
     @fn void       tHermiteDelay_tickIn         (tHermiteDelay* const dl, Lfloat input)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     
     @fn Lfloat   tHermiteDelay_tickOut         (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
     
     @fn void    tHermiteDelay_setDelay         (tHermiteDelay* const dl, Lfloat delay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.
     
     @fn Lfloat     tHermiteDelay_tapOut         (tHermiteDelay* const dl, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param position
     @return
     
     @fn Lfloat   tHermiteDelay_tapOutInterpolated (tHermiteDelay* const dl, uint32_t tapDelay, Lfloat alpha);
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param integer position
     @param fractional portion of the position
     @return
     
     @fn void     tHermiteDelay_tapIn         (tHermiteDelay* const dl, Lfloat value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     @param position
     
     @fn Lfloat     tHermiteDelay_addTo         (tHermiteDelay* const dl, Lfloat value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     @param position
     @return
     
     @fn Lfloat   tHermiteDelay_getDelay         (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
     
     @fn Lfloat   tHermiteDelay_getLastOut     (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
     
     @fn Lfloat   tHermiteDelay_getLastIn     (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
     
     @fn void     tHermiteDelay_setGain         (tHermiteDelay* const dl, Lfloat gain)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param gain
     
     @fn Lfloat     tHermiteDelay_getGain         (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return
     ￼￼￼
     @} */
    
    typedef struct tHermiteDelay
    {
        tMempool mempool;
        
        Lfloat* buff;
        uint32_t bufferMask;
        Lfloat lastOut, lastIn;
        
        uint32_t inPoint, outPoint;
        
        uint32_t maxDelay;
        Lfloat gain;
        Lfloat delay;
        
        Lfloat alpha, omAlpha;
    } tHermiteDelay;

    void    tHermiteDelay_init (tHermiteDelay** const dl, Lfloat delay, uint32_t maxDelay, LEAF* const leaf);
    void    tHermiteDelay_initToPool (tHermiteDelay** const dl, Lfloat delay, uint32_t maxDelay, tMempool* const mp);
    void    tHermiteDelay_free          (tHermiteDelay** const dl);

    Lfloat  tHermiteDelay_tick               (tHermiteDelay* const dl, Lfloat input);
    void    tHermiteDelay_tickIn             (tHermiteDelay* const dl, Lfloat input);
    Lfloat  tHermiteDelay_tickOut            (tHermiteDelay* const dl);

    void    tHermiteDelay_clear              (tHermiteDelay* const dl);
    void    tHermiteDelay_setDelay           (tHermiteDelay* const dl, Lfloat delay);
    Lfloat  tHermiteDelay_tapOut             (tHermiteDelay* const dl, uint32_t tapDelay);
    Lfloat  tHermiteDelay_tapOutInterpolated (tHermiteDelay* const dl, uint32_t tapDelay, Lfloat alpha);
    void    tHermiteDelay_tapIn              (tHermiteDelay* const dl, Lfloat value, uint32_t tapDelay);
    Lfloat  tHermiteDelay_addTo              (tHermiteDelay* const dl, Lfloat value, uint32_t tapDelay);
    Lfloat  tHermiteDelay_getDelay           (tHermiteDelay* const dl);
    Lfloat  tHermiteDelay_getLastOut         (tHermiteDelay* const dl);
    Lfloat  tHermiteDelay_getLastIn          (tHermiteDelay* const dl);
    void    tHermiteDelay_setGain            (tHermiteDelay* const dl, Lfloat gain);
    Lfloat  tHermiteDelay_getGain            (tHermiteDelay* const dl);
    
    
    //==============================================================================
    
    //==============================================================================

    /*!
     @defgroup thermitedelay tHermiteDelay
     @ingroup delay
     @brief Hermite-interpolating delay, created by adapting STK linear delay with Hermite interpolation.
     @{

     @fn void       tHermiteDelay_init             (tHermiteDelay* const dl, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
     @brief Initialize a tHermiteDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tHermiteDelay to initialize.
     @param initialLength
     @param maxLength
     @param leaf A pointer to the leaf instance.

     @fn void    tHermiteDelay_initToPool      (tHermiteDelay* const dl, Lfloat delay, uint32_t maxDelay, tMempool* const mp)
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

     @fn Lfloat   tHermiteDelay_tick             (tHermiteDelay* const dl, Lfloat input)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     @return

     @fn void       tHermiteDelay_tickIn         (tHermiteDelay* const dl, Lfloat input)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input

     @fn Lfloat   tHermiteDelay_tickOut         (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return

     @fn void    tHermiteDelay_setDelay         (tHermiteDelay* const dl, Lfloat delay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.

     @fn Lfloat     tHermiteDelay_tapOut         (tHermiteDelay* const dl, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param position
     @return

     @fn Lfloat   tHermiteDelay_tapOutInterpolated (tHermiteDelay* const dl, uint32_t tapDelay, Lfloat alpha);
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param integer position
     @param fractional portion of the position
     @return

     @fn void     tHermiteDelay_tapIn         (tHermiteDelay* const dl, Lfloat value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     @param position

     @fn Lfloat     tHermiteDelay_addTo         (tHermiteDelay* const dl, Lfloat value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @param input
     @param position
     @return

     @fn Lfloat   tHermiteDelay_getDelay         (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return

     @fn Lfloat   tHermiteDelay_getLastOut     (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return

     @fn Lfloat   tHermiteDelay_getLastIn     (tHermiteDelay* const dl)
     @brief
     @param delay A pointer to the relevant tHermiteDelay.
     @return

     ￼￼￼
     @} */

    typedef struct tLagrangeDelay
    {
        tMempool mempool;

        Lfloat* buff;
        uint32_t bufferMask;
        Lfloat lastOut, lastIn;

        uint32_t inPoint, outPoint;

        uint32_t maxDelay;
        Lfloat delay;

        //coefficients for lagrange interpolation (calculated when delay length changes)
        Lfloat h0;
        Lfloat h1;
        Lfloat h2;
        Lfloat h3;
    } tLagrangeDelay;

    void    tLagrangeDelay_init               (tLagrangeDelay** const dl, Lfloat delay, uint32_t maxDelay, LEAF* const leaf);
    void    tLagrangeDelay_initToPool         (tLagrangeDelay** const dl, Lfloat delay, uint32_t maxDelay, tMempool* const mp);
    void    tLagrangeDelay_free               (tLagrangeDelay** const dl);

    Lfloat  tLagrangeDelay_tick               (tLagrangeDelay* const dl, Lfloat input);
    void    tLagrangeDelay_tickIn             (tLagrangeDelay* const dl, Lfloat input);
    Lfloat  tLagrangeDelay_tickOut            (tLagrangeDelay* const dl);

    void    tLagrangeDelay_clear              (tLagrangeDelay* const dl);
    void    tLagrangeDelay_setDelay           (tLagrangeDelay* const dl, Lfloat delay);
    Lfloat  tLagrangeDelay_tapOut             (tLagrangeDelay* const dl, uint32_t tapDelay);
    Lfloat  tLagrangeDelay_tapOutInterpolated (tLagrangeDelay* const dl, uint32_t tapDelay, Lfloat alpha);
    void    tLagrangeDelay_tapIn              (tLagrangeDelay* const dl, Lfloat value, uint32_t tapDelay);
    Lfloat  tLagrangeDelay_addTo              (tLagrangeDelay* const dl, Lfloat value, uint32_t tapDelay);
    Lfloat  tLagrangeDelay_getDelay           (tLagrangeDelay* const dl);
    Lfloat  tLagrangeDelay_getMaxDelay        (tLagrangeDelay* const dl);
    Lfloat  tLagrangeDelay_getLastOut         (tLagrangeDelay* const dl);
    Lfloat  tLagrangeDelay_getLastIn          (tLagrangeDelay* const dl);

    //==============================================================================

    /*!
     @defgroup tallpassdelay tAllpassDelay
     @ingroup delay
     @brief Allpass-interpolating delay, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tAllpassDelay_init        (tAllpassDelay* const, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
     @brief Initialize a tAllpassDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tAllpassDelay to initialize.
     @param initialLength
     @param maxLength
     @param leaf A pointer to the leaf instance.
     
     @fn void    tAllpassDelay_initToPool  (tAllpassDelay* const, Lfloat delay, uint32_t maxDelay, tMempool* const)
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
     
     @fn void    tAllpassDelay_setDelay    (tAllpassDelay* const, Lfloat delay)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.
     
     @fn Lfloat   tAllpassDelay_getDelay    (tAllpassDelay* const)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @return The current delay length in samples.
     
     @fn void    tAllpassDelay_tapIn       (tAllpassDelay* const, Lfloat in, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param inptu
     @param position
     
     @fn Lfloat   tAllpassDelay_tapOut      (tAllpassDelay* const, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param position
     @return
     
     @fn Lfloat   tAllpassDelay_addTo       (tAllpassDelay* const, Lfloat value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param input
     @param position
     @return
     
     @fn Lfloat   tAllpassDelay_tick        (tAllpassDelay* const, Lfloat sample)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @param input
     @return
     
     @fn Lfloat   tAllpassDelay_getLastOut  (tAllpassDelay* const)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @return
     
     @fn Lfloat   tAllpassDelay_getLastIn   (tAllpassDelay* const)
     @brief
     @param delay A pointer to the relevant tAllpassDelay.
     @return
     ￼￼￼
     @} */
    
    typedef struct tAllpassDelay
    {
        tMempool mempool;
        
        Lfloat gain;
        Lfloat* buff;
        
        Lfloat lastOut, lastIn;
        
        uint32_t inPoint, outPoint;
        
        uint32_t maxDelay;
        
        Lfloat delay;
        
        Lfloat alpha, omAlpha, coeff;
        
        Lfloat apInput;
        
    } tAllpassDelay;

    void    tAllpassDelay_init        (tAllpassDelay** const, Lfloat delay, uint32_t maxDelay, LEAF* const leaf);
    void    tAllpassDelay_initToPool  (tAllpassDelay** const, Lfloat delay, uint32_t maxDelay, tMempool* const);
    void    tAllpassDelay_free        (tAllpassDelay** const);

    Lfloat  tAllpassDelay_tick        (tAllpassDelay* const, Lfloat sample);

    void    tAllpassDelay_clear       (tAllpassDelay* const);
    void    tAllpassDelay_setDelay    (tAllpassDelay* const, Lfloat delay);
    Lfloat  tAllpassDelay_getDelay    (tAllpassDelay* const);
    void    tAllpassDelay_tapIn       (tAllpassDelay* const, Lfloat in, uint32_t tapDelay);
    Lfloat  tAllpassDelay_tapOut      (tAllpassDelay* const, uint32_t tapDelay);
    Lfloat  tAllpassDelay_addTo       (tAllpassDelay* const, Lfloat value, uint32_t tapDelay);
    Lfloat  tAllpassDelay_getLastOut  (tAllpassDelay* const);
    Lfloat  tAllpassDelay_getLastIn   (tAllpassDelay* const);
    
    //==============================================================================
    
    /*!
     @defgroup ttapedelay tTapeDelay
     @ingroup delay
     @brief Linear interpolating delay with fixed read and write pointers, variable rate.
     @{
     
     @fn void    tTapeDelay_init        (tTapeDelay* const, Lfloat delay, uint32_t maxDelay, LEAF* const leaf)
     @brief Initialize a tTapeDelay to the default mempool of a LEAF instance.
     @param delay A pointer to the tTapeDelay to initialize.
     @param initialLength
     @param maxLength
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTapeDelay_initToPool  (tTapeDelay* const, Lfloat delay, uint32_t maxDelay, tMempool* const)
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
     
     @fn void    tTapeDelay_setDelay    (tTapeDelay* const, Lfloat delay)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param delayLength The new delay length in samples. Cannot be greater than the max delay length given on initialization.
     
     @fn Lfloat   tTapeDelay_getDelay    (tTapeDelay* const)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @return
     
     @fn void    tTapeDelay_tapIn       (tTapeDelay* const, Lfloat in, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param input
     @param position
     
     @fn Lfloat   tTapeDelay_tapOut      (tTapeDelay* const d, Lfloat tapDelay)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param position
     @return
     
     @fn Lfloat   tTapeDelay_addTo       (tTapeDelay* const, Lfloat value, uint32_t tapDelay)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param input
     @param position
     @return
     
     @fn Lfloat   tTapeDelay_tick        (tTapeDelay* const, Lfloat sample)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @param input
     @return
     
     @fn void    tTapeDelay_incrementInPoint(tTapeDelay* const dl)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     
     @fn Lfloat   tTapeDelay_getLastOut  (tTapeDelay* const)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @return
     
     @fn Lfloat   tTapeDelay_getLastIn   (tTapeDelay* const)
     @brief
     @param delay A pointer to the relevant tTapeDelay.
     @return
     ￼￼￼
     @} */
    
    typedef struct tTapeDelay
    {
        tMempool mempool;
        
        Lfloat gain;
        Lfloat* buff;
        
        Lfloat lastOut, lastIn;
        
        uint32_t inPoint;
        
        uint32_t maxDelay;
        
        Lfloat delay, inc, idx;
        
        Lfloat apInput;
        
    } tTapeDelay;

    void    tTapeDelay_init             (tTapeDelay** const, Lfloat delay, uint32_t maxDelay, LEAF* const leaf);
    void    tTapeDelay_initToPool       (tTapeDelay** const, Lfloat delay, uint32_t maxDelay, tMempool* const);
    void    tTapeDelay_free             (tTapeDelay** const);

    Lfloat  tTapeDelay_tick             (tTapeDelay* const, Lfloat sample);

    void    tTapeDelay_clear            (tTapeDelay* const);
    void    tTapeDelay_setDelay         (tTapeDelay* const, Lfloat delay);
    Lfloat  tTapeDelay_getDelay         (tTapeDelay* const);
    void    tTapeDelay_tapIn            (tTapeDelay* const, Lfloat in, uint32_t tapDelay);
    Lfloat  tTapeDelay_tapOut           (tTapeDelay* const d, Lfloat tapDelay);
    Lfloat  tTapeDelay_addTo            (tTapeDelay* const, Lfloat value, uint32_t tapDelay);
    void    tTapeDelay_incrementInPoint (tTapeDelay* const dl);
    Lfloat  tTapeDelay_getLastOut       (tTapeDelay* const);
    Lfloat  tTapeDelay_getLastIn        (tTapeDelay* const);
    
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
     
     @fn void    tRingBuffer_push     (tRingBuffer* const ring, Lfloat val)
     @brief Push a value to the ring buffer, overwriting the oldest value if the buffer is full.
     @param buffer A pointer to the relevant tRingBuffer.
     @param input The value to push to the buffer.
     
     @fn Lfloat   tRingBuffer_getNewest    (tRingBuffer* const ring)
     @brief Get the newest value in the ring buffer.
     @param buffer A pointer to the relevant tRingBuffer.
     @return The newest value in the ring buffer.
     
     @fn Lfloat   tRingBuffer_getOldest    (tRingBuffer* const ring)
     @brief Get the oldest value in the ring buffer.
     @param buffer A pointer to the relevant tRingBuffer.
     @return The oldest value in the ring buffer.
     
     @fn Lfloat   tRingBuffer_get      (tRingBuffer* const ring, int index)
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
    typedef struct tRingBuffer
    {
        
        tMempool mempool;
        
        Lfloat* buffer;
        unsigned int size;
        unsigned int pos;
        unsigned int mask;
    } tRingBuffer;

    void    tRingBuffer_init       (tRingBuffer** const ring, int size, LEAF* const leaf);
    void    tRingBuffer_initToPool (tRingBuffer** const ring, int size, tMempool* const mempool);
    void    tRingBuffer_free       (tRingBuffer** const ring);
    
    void    tRingBuffer_push       (tRingBuffer* const ring, Lfloat val);
    Lfloat  tRingBuffer_getNewest  (tRingBuffer* const ring);
    Lfloat  tRingBuffer_getOldest  (tRingBuffer* const ring);
    Lfloat  tRingBuffer_get        (tRingBuffer* const ring, int index);
    int     tRingBuffer_getSize    (tRingBuffer* const ring);
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_DELAY_H_INCLUDED

//==============================================================================


