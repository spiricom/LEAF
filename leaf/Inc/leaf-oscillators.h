/*==============================================================================
 leaf-oscillators.h
 Created: 20 Jan 2017 12:00:58pm
 Author:  Michael R Mulshine
 ==============================================================================*/

#ifndef LEAF_OSCILLATORS_H_INCLUDED
#define LEAF_OSCILLATORS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-tables.h"
#include "leaf-filters.h"
#include "leaf-distortion.h"
    
    /*!
     Header.
     @include basic-oscillators.h
     @example basic-oscillators.c
     An example.
     */
    
    //==============================================================================
    
    /*!
     @defgroup tcycle tCycle
     @ingroup oscillators
     @brief Wavetable cycle/sine wave oscillator
     @{

     @fn void    tCycle_init(tCycle** const osc, LEAF* const leaf)
     @brief Initialize a tCycle to the default mempool of a LEAF instance.
     @param osc A pointer to the tCycle to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tCycle_initToPool(tCycle** const osc, tMempool** const mempool)
     @brief Initialize a tCycle to a specified mempool.
     @param osc A pointer to the tCycle to initialize.
     @param mempool A pointer to the tMempool to use.

     @fn void    tCycle_free(tCycle** const osc)
     @brief Free a tCycle from its mempool.
     @param osc A pointer to the tCycle to free.
     
     @fn Lfloat   tCycle_tick         (tCycle* const osc)
     @brief Tick a tCycle oscillator.
     @param osc A pointer to the relevant tCycle.
     @return The ticked sample as a Lfloat from -1 to 1.

     @fn void    tCycle_setFreq      (tCycle* const osc, Lfloat freq)
     @brief Set the frequency of a tCycle oscillator.
     @param osc A pointer to the relevant tCycle.
     @param freq The frequency to set the oscillator to.
     
    ￼￼￼
     @} */
    
    typedef struct tCycle
    {
        tMempool* mempool;
        // Underlying phasor
        uint32_t phase;
        int32_t inc;
		Lfloat freq;
        Lfloat invSampleRateTimesTwoTo32;
        uint32_t mask;
    } tCycle;

    // Memory handlers for `tCycle`
    void    tCycle_init          (tCycle** const osc, LEAF* const leaf);
    void    tCycle_initToPool    (tCycle** const osc, tMempool** const mempool);
    void    tCycle_free          (tCycle** const osc);

    // Tick function for `tCycle`
    Lfloat  tCycle_tick          (tCycle* const osc);

    // Setter functions for `tCycle`
    void    tCycle_setFreq       (tCycle* const osc, Lfloat freq);
    void    tCycle_setPhase      (tCycle* const osc, Lfloat phase);
    void    tCycle_setSampleRate (tCycle* const osc, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup ttriangle tTriangle
     @ingroup oscillators
     @brief Anti-aliased wavetable triangle wave oscillator.
     @{
     
     @fn void    tTriangle_init(tTriangle** const osc, LEAF* const leaf)
     @brief Initialize a tTriangle to the default mempool of a LEAF instance.
     @param osc A pointer to the tTriangle to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTriangle_initToPool(tTriangle** const osc, tMempool** const mempool)
     @brief Initialize a tTriangle to a specified mempool.
     @param osc A pointer to the tTriangle to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTriangle_free(tTriangle** const osc)
     @brief Free a tTriangle from its mempool.
     @param osc A pointer to the tTriangle to free.
     
     @fn Lfloat   tTriangle_tick         (tTriangle* const osc)
     @brief Tick a tTriangle oscillator.
     @param osc A pointer to the relevant tTriangle.
     @return The ticked sample as a Lfloat from -1 to 1.
     
     @fn void    tTriangle_setFreq      (tTriangle* const osc, Lfloat freq)
     @brief Set the frequency of a tTriangle oscillator.
     @param osc A pointer to the relevant tTriangle.
     @param freq The frequency to set the oscillator to.
     
     @} */

    typedef struct tTriangle
    {
        tMempool* mempool;
        // Underlying phasor
        uint32_t phase;
        int32_t inc;
        Lfloat freq;
        int oct;
        Lfloat w;
        Lfloat invSampleRate;
        Lfloat invSampleRateTimesTwoTo32;
        uint32_t mask;
    } tTriangle;

    // Memory handlers for `tTriangle`
    void    tTriangle_init          (tTriangle** const osc, LEAF* const leaf);
    void    tTriangle_initToPool    (tTriangle** const osc, tMempool** const mempool);
    void    tTriangle_free          (tTriangle** const osc);

    // Tick function for `tTriangle`
    Lfloat  tTriangle_tick          (tTriangle* const osc);

    // Setter functions for `tTriangle`
    void    tTriangle_setFreq       (tTriangle* const osc, Lfloat freq);
    void    tTriangle_setPhase      (tTriangle* const osc, Lfloat phase);
    void    tTriangle_setSampleRate (tTriangle* const osc, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tsquare tSquare
     @ingroup oscillators
     @brief Anti-aliased wavetable square wave oscillator.
     @{
     
     @fn void    tSquare_init(tSquare** const osc, LEAF* const leaf)
     @brief Initialize a tSquare to the default mempool of a LEAF instance.
     @param osc A pointer to the tSquare to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSquare_initToPool(tSquare** const osc, tMempool** const mempool)
     @brief Initialize a tSquare to a specified mempool.
     @param osc A pointer to the tSquare to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSquare_free(tSquare** const osc)
     @brief Free a tSquare from its mempool.
     @param osc A pointer to the tSquare to free.
     
     @fn Lfloat   tSquare_tick         (tSquare* const osc)
     @brief Tick a tSquare oscillator.
     @param osc A pointer to the relevant tSquare.
     @return The ticked sample as a Lfloat from -1 to 1.
     
     @fn void    tSquare_setFreq      (tSquare* const osc, Lfloat freq)
     @brief Set the frequency of a tSquare oscillator.
     @param osc A pointer to the relevant tSquare.
     @param freq The frequency to set the oscillator to.
     ￼￼￼
     @} */
    
    typedef struct tSquare
    {
        tMempool* mempool;
        // Underlying phasor
        uint32_t phase;
        int32_t inc;
        Lfloat freq;
        int oct;
        Lfloat w;
        Lfloat invSampleRate;
        Lfloat invSampleRateTimesTwoTo32;
        uint32_t mask;
    } tSquare;

    // Memory handlers for `tSquare`
    void    tSquare_init          (tSquare** const osc, LEAF* const leaf);
    void    tSquare_initToPool    (tSquare** const osc, tMempool** const mempool);
    void    tSquare_free          (tSquare** const osc);

    // Tick function for `tSquare`
    Lfloat  tSquare_tick          (tSquare* const osc);

    // Setter functions for `tSquare`
    void    tSquare_setFreq       (tSquare* const osc, Lfloat freq);
    void    tSquare_setPhase      (tSquare* const osc, Lfloat phase);
    void    tSquare_setSampleRate (tSquare* const osc, Lfloat sr);
    
    /*!￼￼￼
     @} */
    
    //==============================================================================
    
    /*!
     @defgroup tsawtooth tSawtooth
     @ingroup oscillators
     @brief Anti-aliased wavetable saw wave oscillator.
     @{
     
     @fn void    tSawtooth_init(tSawtooth** const osc, LEAF* const leaf)
     @brief Initialize a tSawtooth to the default mempool of a LEAF instance.
     @param osc A pointer to the tSawtooth to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSawtooth_initToPool(tSawtooth** const osc, tMempool** const mempool)
     @brief Initialize a tSawtooth to a specified mempool.
     @param osc A pointer to the tSawtooth to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSawtooth_free(tSawtooth** const osc)
     @brief Free a tSawtooth from its mempool.
     @param osc A pointer to the tSawtooth to free.
     
     @fn Lfloat   tSawtooth_tick         (tSawtooth* const osc)
     @brief Tick a tSawtooth oscillator.
     @param osc A pointer to the relevant tSawtooth.
     @return The ticked sample as a Lfloat from -1 to 1.
     
     @fn void    tSawtooth_setFreq      (tSawtooth* const osc, Lfloat freq)
     @brief Set the frequency of a tSawtooth oscillator.
     @param osc A pointer to the relevant tSawtooth.
     @param freq The frequency to set the oscillator to.
     ￼￼￼
     @} */
    
    typedef struct tSawtooth
    {
        tMempool* mempool;
        // Underlying phasor
        uint32_t phase;
        int32_t inc;
        Lfloat freq;
        int oct;
        Lfloat w;
        Lfloat invSampleRate;
        Lfloat invSampleRateTimesTwoTo32;
        uint32_t mask;
    } tSawtooth;

    // Memory handlers for `tSawtooth`
    void    tSawtooth_init          (tSawtooth** const osc, LEAF* const leaf);
    void    tSawtooth_initToPool    (tSawtooth** const osc, tMempool** const mempool);
    void    tSawtooth_free          (tSawtooth** const osc);

    // Tick function for `tSawtooth`
    Lfloat  tSawtooth_tick          (tSawtooth* const osc);

    // Setter functions for `tSawtooth`
    void    tSawtooth_setFreq       (tSawtooth* const osc, Lfloat freq);
    void    tSawtooth_setPhase      (tSawtooth* const osc, Lfloat phase);
    void    tSawtooth_setSampleRate (tSawtooth* const osc, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tpbtriangle tPBTriangle
     @ingroup oscillators
     @brief Triangle wave oscillator with polyBLEP anti-aliasing.
     @{
     
     @fn void   tPBTriangle_init(tPBTriangle** const osc, LEAF* const leaf)
     @brief Initialize a tPBTriangle to the default mempool of a LEAF instance.
     @param osc A pointer to the tPBTriangle to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPBTriangle_initToPool(tPBTriangle** const osc, tMempool** const mempool)
     @brief Initialize a tPBTriangle to a specified mempool.
     @param osc A pointer to the tPBTriangle to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPBTriangle_free(tPBTriangle** const osc)
     @brief Free a tTri from its mempool.
     @param osc A pointer to the tPBTriangle to free.
     
     @fn Lfloat   tPBTriangle_tick          (tPBTriangle* const osc)
     @brief
     @param osc A pointer to the relevant tPBTriangle.
     
     @fn void    tPBTriangle_setFreq       (tPBTriangle* const osc, Lfloat freq)
     @brief
     @param osc A pointer to the relevant tPBTriangle.
     
     @fn void    tPBTriangle_setSkew       (tPBTriangle* const osc, Lfloat skew)
     @brief
     @param osc A pointer to the relevant tPBTriangle.
     ￼￼￼
     @} */

    typedef struct tPBSineTriangle
    {
        tMempool* mempool;
        uint32_t phase;
        tCycle* sine;
        int32_t inc;
        Lfloat freq;
        Lfloat shape;
        Lfloat oneMinusShape;
        Lfloat invSampleRate;
        Lfloat invSampleRateTimesTwoTo32;
    } tPBSineTriangle;

    // Memory handlers for `tPBSineTriangle`
    void    tPBSineTriangle_init          (tPBSineTriangle** const osc, LEAF* const leaf);
    void    tPBSineTriangle_initToPool    (tPBSineTriangle** const osc, tMempool** const mempool);
    void    tPBSineTriangle_free          (tPBSineTriangle** const osc);


#ifdef ITCMRAM
    Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSineTriangle_tick          (tPBSineTriangle* const osc);
#else
    // Tick function for `tPBSineTriangle`
    Lfloat  tPBSineTriangle_tick          (tPBSineTriangle* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSineTriangle_setFreq       (tPBSineTriangle* const osc, Lfloat freq);
#else
    // Setter functions for `tPBSineTriangle`
    void    tPBSineTriangle_setFreq       (tPBSineTriangle* const osc, Lfloat freq);
#endif
    void    tPBSineTriangle_setShape      (tPBSineTriangle* const osc, Lfloat shape);
    void    tPBSineTriangle_setSampleRate (tPBSineTriangle* const osc, Lfloat sr);

    //==============================================================================


    typedef struct tPBTriangle
    {
        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        Lfloat freq;
        uint32_t width;
        uint32_t oneMinusWidth;
        Lfloat lastOutput;
        Lfloat invSampleRate;
        Lfloat invSampleRateTimesTwoTo32;
    } tPBTriangle;

    // Memory handlers for `tPBTriangle`
    void    tPBTriangle_init          (tPBTriangle** const osc, LEAF* const leaf);
    void    tPBTriangle_initToPool    (tPBTriangle** const osc, tMempool** const mempool);
    void    tPBTriangle_free          (tPBTriangle** const osc);
    
#ifdef ITCMRAM
    Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBTriangle_tick          (tPBTriangle* const osc);
#else
    // Tick function for `tPBTriangle`
    Lfloat  tPBTriangle_tick          (tPBTriangle* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tPBTriangle_setFreq       (tPBTriangle* const osc, Lfloat freq);
#else
    // Setter functions for `tPBTriangle`
    void    tPBTriangle_setFreq       (tPBTriangle* const osc, Lfloat freq);
#endif
    void    tPBTriangle_setSkew       (tPBTriangle* const osc, Lfloat skew);
    void    tPBTriangle_setSampleRate (tPBTriangle* const osc, Lfloat sr);
    
    //==============================================================================
    /*!
     @defgroup tpbpulse tPBPulse
     @ingroup oscillators
     @brief Pulse wave oscillator with polyBLEP anti-aliasing.
     @{
     
     @fn void    tPBPulse_init(tPBPulse** const osc, LEAF* const leaf)
     @brief Initialize a tPBPulse to the default mempool of a LEAF instance.
     @param osc A pointer to the tPBPulse to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPBPulse_initToPool(tPBPulse** const osc, tMempool** const)
     @brief Initialize a tPBPulse to a specified mempool.
     @param osc A pointer to the tPBPulse to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPBPulse_free(tPBPulse** const osc)
     @brief Free a tPBPulse from its mempool.
     @param osc A pointer to the tPBPulse to free.
     
     @fn Lfloat   tPBPulse_tick        (tPBPulse* const osc)
     @brief
     @param osc A pointer to the relevant tPBPulse.
     
     @fn void    tPBPulse_setFreq     (tPBPulse* const osc, Lfloat freq)
     @brief
     @param osc A pointer to the relevant tPBPulse.
     
     @fn void    tPBPulse_setWidth    (tPBPulse* const osc, Lfloat width)
     @brief
     @param osc A pointer to the relevant tPBPulse.
     ￼￼￼
     @} */
    
    typedef struct tPBPulse
    {
        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        Lfloat freq;
        uint32_t width;
        uint32_t oneMinusWidth;
        Lfloat invSampleRate;
        Lfloat invSampleRateTimesTwoTo32;
    } tPBPulse;

    // Memory handlers for `tPBPulse`
    void    tPBPulse_init          (tPBPulse** const osc, LEAF* const leaf);
    void    tPBPulse_initToPool    (tPBPulse** const osc, tMempool** const);
    void    tPBPulse_free          (tPBPulse** const osc);
    
#ifdef ITCMRAM
    Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBPulse_tick        (tPBPulse* const osc);
#else
    // Tick function for `tPBPulse`
    Lfloat  tPBPulse_tick          (tPBPulse* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBPulse_setFreq     (tPBPulse* const osc, Lfloat freq);
#else
    // Setter functions for `tPBPulse`
    void    tPBPulse_setFreq       (tPBPulse* const osc, Lfloat freq);
#endif
    void    tPBPulse_setWidth      (tPBPulse* const osc, Lfloat width);
    void    tPBPulse_setSampleRate (tPBPulse* const osc, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tpbsaw tPBSaw
     @ingroup oscillators
     @brief Saw wave oscillator with polyBLEP anti-aliasing.
     @{
     
     @fn void    tPBSaw_init(tPBSaw** const osc, LEAF* const leaf)
     @brief Initialize a tPBSaw to the default mempool of a LEAF instance.
     @param osc A pointer to the tPBSaw to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPBSaw_initToPool(tPBSaw** const osc, tMempool** const mempool)
     @brief Initialize a tPBSaw to a specified mempool.
     @param osc A pointer to the tPBSaw to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPBSaw_free(tPBSaw** const osc)
     @brief Free a tPBSaw from its mempool.
     @param osc A pointer to the tPBSaw to free.
     
     @fn Lfloat   tPBSaw_tick          (tPBSaw* const osc)
     @brief
     @param osc A pointer to the relevant tPBSaw.
     
     @fn void    tPBSaw_setFreq       (tPBSaw* const osc, Lfloat freq)
     @brief
     @param osc A pointer to the relevant tPBSaw.
     ￼￼￼
     @} */
    
    typedef struct tPBSaw
    {
        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        Lfloat freq;
        Lfloat invSampleRate;
        Lfloat invSampleRateTimesTwoTo32;
        Lfloat lastsyncin;
        Lfloat sync;
    } tPBSaw;

    // Memory handlers for `tPBSaw`
    void    tPBSaw_init          (tPBSaw** const osc, LEAF* const leaf);
    void    tPBSaw_initToPool    (tPBSaw** const osc, tMempool** const mempool);
    void    tPBSaw_free          (tPBSaw** const osc);
    
#ifdef ITCMRAM
    Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSaw_tick          (tPBSaw* const osc);
#else
    // Tick function for `tPBSaw`
    Lfloat  tPBSaw_tick          (tPBSaw* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSaw_setFreq       (tPBSaw* const osc, Lfloat freq);
#else
    // Setter functions for `tPBSaw`
    void    tPBSaw_setFreq       (tPBSaw* const osc, Lfloat freq);
#endif
    void    tPBSaw_setSampleRate (tPBSaw* const osc, Lfloat sr);
    
    //==============================================================================
    
typedef struct tPBSawSquare
{
    tMempool* mempool;
    uint32_t phase;
    int32_t inc;
    Lfloat freq;
    Lfloat invSampleRate;
    Lfloat invSampleRateTimesTwoTo32;
    Lfloat lastsyncin;
    Lfloat sync;
    Lfloat shape;
    Lfloat oneMinusShape;

} tPBSawSquare;

    // Memory handlers for `tPBSawSquare`
    void    tPBSawSquare_init          (tPBSawSquare** const osc, LEAF* const leaf);
    void    tPBSawSquare_initToPool    (tPBSawSquare** const osc, tMempool** const mempool);
    void    tPBSawSquare_free          (tPBSawSquare** const osc);

#ifdef ITCMRAM
    Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSawSquare_tick          (tPBSawSquare* const osc);
#else
    // Tick function for `tPBSawSquare`
    Lfloat  tPBSawSquare_tick          (tPBSawSquare* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSawSquare_setFreq       (tPBSawSquare* const osc, Lfloat freq);
#else
    // Setter functions for `tPBSawSquare`
    void    tPBSawSquare_setFreq       (tPBSawSquare* const osc, Lfloat freq);
#endif
    void    tPBSawSquare_setShape      (tPBSawSquare* const osc, Lfloat shape);
    void    tPBSawSquare_setSampleRate (tPBSawSquare* const osc, Lfloat sr);

//==============================================================================
    typedef struct tSawOS
    {
        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        Lfloat freq;
        int32_t mask;
        uint8_t phaseDidReset;
        Lfloat invSampleRateOS;
        uint8_t OSratio;
        uint32_t invSampleRateTimesTwoTo32OS;
        tSVF** aaFilters;
        uint8_t filterOrder;
    } tSawOS;

    // Memory handlers for `tSawOS`
    void    tSawOS_init          (tSawOS** const osc, uint8_t OS_ratio, uint8_t filterOrder, LEAF* const leaf);
    void    tSawOS_initToPool    (tSawOS** const osc, uint8_t OS_ratio, uint8_t filterOrder, tMempool** const mp);
    void    tSawOS_free          (tSawOS** const osc);

    // Tick function for `tSawOS`
    Lfloat  tSawOS_tick          (tSawOS* const osc);

    // Setter functions for `tSawOS`
    void    tSawOS_setFreq       (tSawOS* const osc, Lfloat freq);
    void    tSawOS_setSampleRate (tSawOS* const osc, Lfloat sr);

    //==============================================================================


    /*!
     @defgroup tphasor tPhasor
     @ingroup oscillators
     @brief Aliasing phasor.
     @{
     
     @fn void    tPhasor_init(tPhasor** const osc, LEAF* const leaf)
     @brief Initialize a tPhasor to the default mempool of a LEAF instance.
     @param osc A pointer to the tPhasor to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPhasor_initToPool(tPhasor** const osc, tMempool** const)
     @brief Initialize a tPhasor to a specified mempool.
     @param osc A pointer to the tPhasor to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPhasor_free(tPhasor** const osc)
     @brief Free a tPhasor from its mempool.
     @param osc A pointer to the tPhasor to free.
     
     @fn Lfloat   tPhasor_tick        (tPhasor* const osc)
     @brief
     @param osc A pointer to the relevant tPhasor.
     
     @fn void    tPhasor_setFreq     (tPhasor* const osc, Lfloat freq)
     @brief
     @param osc A pointer to the relevant tPhasor.
     ￼￼￼
     @} */
    
    typedef struct tPhasor
    {

        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        Lfloat freq;
        Lfloat invSampleRate;
        Lfloat invSampleRateTimesTwoTo32;
    } tPhasor;

    // Memory handlers for `tPhasor`
    void    tPhasor_init          (tPhasor** const osc, LEAF* const leaf);
    void    tPhasor_initToPool    (tPhasor** const osc, tMempool** const);
    void    tPhasor_free          (tPhasor** const osc);

    // Tick function for `tPhasor`
    Lfloat  tPhasor_tick          (tPhasor* const osc);

    // Setter functions for `tPhasor`
    void    tPhasor_setFreq       (tPhasor* const osc, Lfloat freq);
    void    tPhasor_setSampleRate (tPhasor* const osc, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tnoise tNoise
     @ingroup oscillators
     @brief Noise generator, capable of producing white or pink noise.
     @{
     
     @fn void    tNoise_init(tNoise** const noise, NoiseType type, LEAF* const leaf)
     @brief Initialize a tNoise to the default mempool of a LEAF instance.
     @param noise A pointer to the tNoise to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tNoise_initToPool(tNoise** const noise, NoiseType type, tMempool** const)
     @brief Initialize a tNoise to a specified mempool.
     @param noise A pointer to the tNoise to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tNoise_free(tNoise** const noise)
     @brief Free a tNoise from its mempool.
     @param noise A pointer to the tNoise to free.
     
     @fn Lfloat   tNoise_tick         (tNoise* const noise)
     @brief
     @param noise A pointer to the relevant tNoise.
     */
    
    /* tNoise. WhiteNoise, PinkNoise. */
    /*!
     * Noise types
     */
    typedef enum NoiseType
    {
        WhiteNoise, //!< White noise. Full spectrum.
        PinkNoise, //!< Pink noise. Inverse frequency-proportional spectrum.
        NoiseTypeNil,
    } NoiseType;
    
     /*!￼￼￼ @} */

    
    typedef struct tNoise
    {
        tMempool* mempool;
        NoiseType type;
        Lfloat pinkb0, pinkb1, pinkb2;
        Lfloat(*rand)(void);
    } tNoise;

    // Memory handlers for `tNoise`
    void    tNoise_init         (tNoise** const noise, NoiseType type, LEAF* const leaf);
    void    tNoise_initToPool   (tNoise** const noise, NoiseType type, tMempool** const);
    void    tNoise_free         (tNoise** const noise);

    // Tick function for `tNoise`
    Lfloat  tNoise_tick         (tNoise* const noise);
    
    //==============================================================================
    
    /*!
     @defgroup tneuron tNeuron
     @ingroup oscillators
     @brief Model of a neuron, adapted to act as an oscillator.
     @{
     
     @fn void    tNeuron_init(tNeuron** const neuron, LEAF* const leaf)
     @brief Initialize a tNeuron to the default mempool of a LEAF instance.
     @param neuron A pointer to the tNeuron to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tNeuron_initToPool(tNeuron** const neuron, tMempool** const mempool)
     @brief Initialize a tNeuron to a specified mempool.
     @param neuron A pointer to the tNeuron to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tNeuron_free(tNeuron** const neuron)
     @brief Free a tNeuron from its mempool.
     @param neuron A pointer to the tNeuron to free.
     
     @fn void    tNeuron_reset       (tNeuron* const neuron)
     @brief Reset the neuron model.
     @param neuron A pointer to the relevant tNeuron.
     
     @fn Lfloat   tNeuron_tick        (tNeuron* const neuron)
     @brief Tick a tNeuron oscillator.
     @param neuron A pointer to the relevant tNeuron.
     @return The ticked sample as a Lfloat from -1 to 1.
     
     @fn void    tNeuron_setMode     (tNeuron* const neuron, NeuronMode mode)
     @brief Set the tNeuron shaping mode.
     @param neuron A pointer to the relevant tNeuron.
     @param mode The mode to set the tNeuron to.
     
     @fn void    tNeuron_setCurrent  (tNeuron* const neuron, Lfloat current)
     @brief Set the current.
     @param neuron A pointer to the relevant tNeuron.
     @param current The new current.
     
     @fn void    tNeuron_setK        (tNeuron* const neuron, Lfloat K)
     @brief Set the potassium value.
     @param neuron A pointer to the relevant tNeuron.
     @param K The new potassium.
     
     @fn void    tNeuron_setL        (tNeuron* const neuron, Lfloat L)
     @brief Set the chloride value.
     @param neuron A pointer to the relevant tNeuron.
     @param L The new chloride value.
     
     @fn void    tNeuron_setN        (tNeuron* const neuron, Lfloat N)
     @brief Set the sodium value.
     @param neuron A pointer to the relevant tNeuron.
     @param N The new sodium value.
     
     @fn void    tNeuron_setC        (tNeuron* const neuron, Lfloat C)
     @brief Set the calcium value.
     @param neuron A pointer to the relevant tNeuron.
     @param C The new calcium.
     
     @fn  void    tNeuron_setV1       (tNeuron* const neuron, Lfloat V1)
     @brief Set the V1 value.
     @param neuron A pointer to the relevant tNeuron.
     @param V1 The new V1.
     
     @fn void    tNeuron_setV2       (tNeuron* const neuron, Lfloat V2)
     @brief Set the V2 value.
     @param neuron A pointer to the relevant tNeuron.
     @param V2 The new V2.
     
     @fn void    tNeuron_setV3       (tNeuron* const neuron, Lfloat V3)
     @brief Set the V3 value.
     @param neuron A pointer to the relevant tNeuron.
     @param V3 The new V3.
     
     @fn void    tNeuron_setTimeStep (tNeuron* const neuron, Lfloat timestep)
     @brief Set the time step of the model.
     @param neuron A pointer to the relevant tNeuron.
     @param timestep The new time step.
     
     @brief Shaping modes for tNeuron output.
     */
    typedef enum NeuronMode
    {
        NeuronNormal, //!< Normal operation
        NeuronTanh, //!< Tanh voltage shaping
        NeuronAaltoShaper, //!< Aalto voltage shaping
        NeuronModeNil
    } NeuronMode;
    
    /*!￼￼￼ @} */
    
    typedef struct tNeuron
    {
        tMempool* mempool;
        
        tPoleZero* f;
        
        NeuronMode mode;
        
        Lfloat voltage, current;
        Lfloat timeStep;
        Lfloat invSampleRate;
        Lfloat alpha[3];
        Lfloat beta[3];
        Lfloat rate[3];
        Lfloat V[3];
        Lfloat P[3];
        Lfloat gK, gN, gL, C;
    } tNeuron;

    // Memory handlers for `tNeuron`
    void    tNeuron_init          (tNeuron** const neuron, LEAF* const leaf);
    void    tNeuron_initToPool    (tNeuron** const neuron, tMempool** const mempool);
    void    tNeuron_free          (tNeuron** const neuron);

    // Tick function for `tNeuron`
    Lfloat   tNeuron_tick         (tNeuron* const neuron);

    // Setter functions for `tNeuron`
    void    tNeuron_reset         (tNeuron* const neuron);
    void    tNeuron_setMode       (tNeuron* const neuron, NeuronMode mode);
    void    tNeuron_setCurrent    (tNeuron* const neuron, Lfloat current);
    void    tNeuron_setK          (tNeuron* const neuron, Lfloat K);
    void    tNeuron_setL          (tNeuron* const neuron, Lfloat L);
    void    tNeuron_setN          (tNeuron* const neuron, Lfloat N);
    void    tNeuron_setC          (tNeuron* const neuron, Lfloat C);
    void    tNeuron_setV1         (tNeuron* const neuron, Lfloat V1);
    void    tNeuron_setV2         (tNeuron* const neuron, Lfloat V2);
    void    tNeuron_setV3         (tNeuron* const neuron, Lfloat V3);
    void    tNeuron_setTimeStep   (tNeuron* const neuron, Lfloat timestep);
    void    tNeuron_setSampleRate (tNeuron* const neuron, Lfloat sr);

    //==============================================================================
    
    
#define FILLEN 128 //was 256 in dekrispator code, but it seems like it just needs to be longer than dd step length (72) and probably a power of 2.
    // smaller buffer means refilling more often but a less intense load each time it refills

    
    /*!
     @defgroup tmbpulse tMBPulse
     @ingroup oscillators
     @brief Pulse wave oscillator with minBLEP anti-aliasing.
     @{
     
     @fn void tMBPulse_init(tMBPulse** const osc, LEAF* const leaf)
     @brief Initialize a tMBPulse to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBPulse to initialize.
     
     @fn void tMBPulse_initToPool(tMBPulse** const osc, tMempool** const mempool)
     @brief Initialize a tMBPulse to a specified mempool.
     @param osc A pointer to the tMBPulse to initialize.
     
     @fn void tMBPulse_free(tMBPulse** const osc)
     @brief Free a tMBPulse from its mempool.
     @param osc A pointer to the tMBPulse to free.
     
     @fn Lfloat tMBPulse_tick(tMBPulse* const osc)
     @brief
     @param osc A pointer to the relevant tMBPulse.
     
     @fn void tMBPulse_setFreq(tMBPulse* const osc, Lfloat f)
     @brief
     @param osc A pointer to the relevant tMBPulse.
     
     @fn void tMBPulse_setWidth(tMBPulse* const osc, Lfloat w)
     @brief
     @param osc A pointer to the relevant tMBPulse.
     
     @fn Lfloat tMBPulse_sync(tMBPulse* const osc, Lfloat sync)
     @brief
     @param osc A pointer to the relevant tMBPulse.
     
     @fn void tMBPulse_setSyncMode(tMBPulse* const osc, int hardOrSoft)
     @brief Set the sync behavior of the oscillator.
     @param hardOrSoft 0 for hard sync, 1 for soft sync
     ￼￼￼
     @} */
    
    typedef struct tMBPulse
    {

        tMempool* mempool;
        Lfloat    out;
        Lfloat    freq;
        Lfloat    waveform;    // duty cycle, must be in [-1, 1]
        Lfloat    lastsyncin;
        Lfloat    sync;
        Lfloat    syncdir;
        int      softsync;
        Lfloat   _p, _w, _b, _x, _z;
        Lfloat _inv_w;
        int     _j, _k;
        Lfloat   _f [8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t BLEPindices[64];
        Lfloat 	BLEPproperties[64][2];
        Lfloat invSampleRate;

    } tMBPulse;

    // Memory handlers for `tMBPulse`
    void    tMBPulse_init                   (tMBPulse** const osc, LEAF* const leaf);
    void    tMBPulse_initToPool             (tMBPulse** const osc, tMempool** const mempool);
    void    tMBPulse_free                   (tMBPulse** const osc);
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBPulse_place_step_dd_noBuffer(tMBPulse* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale);
#else
    void    tMBPulse_place_step_dd_noBuffer (tMBPulse* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale);
#endif
    // Tick function for `tMBPulse`
    Lfloat  tMBPulse_tick                   (tMBPulse* const osc);

    // Setter functions for `tMBPulse`
    Lfloat  tMBPulse_sync                   (tMBPulse* const osc, Lfloat sync);
    void    tMBPulse_setFreq                (tMBPulse* const osc, Lfloat f);
    void    tMBPulse_setWidth               (tMBPulse* const osc, Lfloat w);
    void    tMBPulse_setPhase               (tMBPulse* const osc, Lfloat phase);
    void    tMBPulse_setSyncMode            (tMBPulse* const osc, int hardOrSoft);
    void    tMBPulse_setBufferOffset        (tMBPulse* const osc, uint32_t offset);
    void    tMBPulse_setSampleRate          (tMBPulse* const osc, Lfloat sr);
    
    /*!
     @defgroup tmbtriangle tMBTriangle
     @ingroup oscillators
     @brief Triangle wave oscillator with minBLEP anti-aliasing.
     @{
     
     @fn void tMBTriangle_init(tMBTriangle** const osc, LEAF* const leaf)
     @brief Initialize a tMBTriangle to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBTriangle to initialize.
     
     @fn void tMBTriangle_initToPool(tMBTriangle** const osc, tMempool** const mempool)
     @brief Initialize a tMBTriangle to a specified mempool.
     @param osc A pointer to the tMBTriangle to initialize.
     
     @fn void tMBTriangle_free(tMBTriangle** const osc)
     @brief Free a tMBTriangle from its mempool.
     @param osc A pointer to the tMBTriangle to free.
     
     @fn Lfloat tMBTriangle_tick(tMBTriangle* const osc)
     @brief
     @param osc A pointer to the relevant tMBTriangle.
     
     @fn void tMBTriangle_setFreq(tMBTriangle* const osc, Lfloat f)
     @brief
     @param osc A pointer to the relevant tMBTriangle.
     
     @fn void tMBTriangle_setWidth(tMBTriangle* const osc, Lfloat w)
     @brief
     @param osc A pointer to the relevant tMBTriangle.
     
     @fn Lfloat tMBTriangle_sync(tMBTriangle* const osc, Lfloat sync)
     @brief
     @param osc A pointer to the relevant tMBTriangle.
     
     @fn void tMBTriangle_setSyncMode(tMBTriangle* const osc, int hardOrSoft)
     @brief Set the sync behavior of the oscillator.
     @param hardOrSoft 0 for hard sync, 1 for soft sync

     @} */
    
    typedef struct tMBTriangle
    {

        tMempool* mempool;
        Lfloat    out;
        Lfloat    freq;
        Lfloat    waveform;    // duty cycle, must be in [-1, 1]
        Lfloat    lastsyncin;
        Lfloat    sync;
        Lfloat    syncdir;
        int      softsync;
        Lfloat   _p, _w, _b, _z, quarterwaveoffset;
        int     _j, _k;
        Lfloat _inv_w;
        Lfloat 	shape;
        Lfloat   _f [8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t maxBLEPphaseSlope;
        uint16_t BLEPindices[64];
        Lfloat 	BLEPproperties[64][3];
        Lfloat invSampleRate;
    } tMBTriangle;

    // Memory handlers for `tMBTriangle`
    void    tMBTriangle_init              (tMBTriangle** const osc, LEAF* const leaf);
    void    tMBTriangle_initToPool        (tMBTriangle** const osc, tMempool** const mempool);
    void    tMBTriangle_free              (tMBTriangle** const osc);
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBTriangle_place_dd_noBuffer(tMBTriangle* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale, Lfloat stepOrSlope, Lfloat w);
#else
    void    tMBTriangle_place_dd_noBuffer (tMBTriangle* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale,
                                           Lfloat stepOrSlope, Lfloat w);
#endif

    // Tick function for `tMBTriangle`
    Lfloat  tMBTriangle_tick              (tMBTriangle* const osc);

    // Setter functions for `tMBTriangle`
    Lfloat  tMBTriangle_sync              (tMBTriangle* const osc, Lfloat sync);
    void    tMBTriangle_setFreq           (tMBTriangle* const osc, Lfloat f);
    void    tMBTriangle_setWidth          (tMBTriangle* const osc, Lfloat w);
    void    tMBTriangle_setPhase          (tMBTriangle* const osc, Lfloat phase);
    void    tMBTriangle_setSyncMode       (tMBTriangle* const osc, int hardOrSoft);
    void    tMBTriangle_setBufferOffset   (tMBTriangle* const osc, uint32_t offset);
    void    tMBTriangle_setSampleRate     (tMBTriangle* const osc, Lfloat sr);
    
    



    typedef struct tMBSineTri
    {
        tMempool* mempool;
        Lfloat    out;
        Lfloat    freq;
        Lfloat    waveform;    // duty cycle, must be in [-1, 1]
        Lfloat    lastsyncin;
        Lfloat    sync;
        Lfloat    syncdir;
        int      softsync;
        Lfloat   _p, _w, _b, _z;
        Lfloat _sinPhase;
        Lfloat shape;
        int     _j, _k;
        Lfloat _inv_w;
        Lfloat   _f [8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t maxBLEPphaseSlope;
        uint16_t BLEPindices[64];
        Lfloat 	BLEPproperties[64][3];
        Lfloat invSampleRate;
        uint32_t sineMask;
    } tMBSineTri;

    // Memory handlers for `tMBSineTri`
    void    tMBSineTri_init              (tMBSineTri** const osc, LEAF* const leaf);
    void    tMBSineTri_initToPool        (tMBSineTri** const osc, tMempool** const mempool);
    void    tMBSineTri_free              (tMBSineTri** const osc);
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSineTri_place_dd_noBuffer(tMBSineTri* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale, Lfloat stepOrSlope, Lfloat w);
#else
    void    tMBSineTri_place_dd_noBuffer (tMBSineTri* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale,
                                          Lfloat stepOrSlope, Lfloat w);
#endif
    // Tick function for `tMBSineTri`
    Lfloat  tMBSineTri_tick              (tMBSineTri* const osc);

    // Setter functions for `tMBSineTri`
    Lfloat  tMBSineTri_sync              (tMBSineTri* const osc, Lfloat sync);
    void    tMBSineTri_setFreq           (tMBSineTri* const osc, Lfloat f);
    void    tMBSineTri_setWidth          (tMBSineTri* const osc, Lfloat w);
    void    tMBSineTri_setPhase          (tMBSineTri* const osc, Lfloat phase);
    void    tMBSineTri_setShape          (tMBSineTri* const osc, Lfloat shape);
    void    tMBSineTri_setSyncMode       (tMBSineTri* const osc, int hardOrSoft);
    void    tMBSineTri_setBufferOffset   (tMBSineTri* const osc, uint32_t offset);
    void    tMBSineTri_setSampleRate     (tMBSineTri* const osc, Lfloat sr);


    /*!
     @defgroup tmbsaw tMBSaw
     @ingroup oscillators
     @brief Saw wave oscillator with minBLEP anti-aliasing.
     @{
     
     @fn void tMBSaw_init(tMBSaw** const osc, LEAF* const leaf)
     @brief Initialize a tMBSaw to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBSaw to initialize.
     
     @fn void tMBSaw_initToPool(tMBSaw** const osc, tMempool** const mempool)
     @brief Initialize a tMBSaw to a specified mempool.
     @param osc A pointer to the tMBSaw to initialize.
     
     @fn void tMBSaw_free(tMBSaw** const osc)
     @brief Free a tMBSaw from its mempool.
     @param osc A pointer to the tMBSaw to free.
     
     @fn Lfloat tMBSaw_tick(tMBSaw* const osc)
     @brief Tick the oscillator.
     @param osc A pointer to the relevant tMBSaw.
     @return The ticked sample.
     
     @fn void tMBSaw_setFreq(tMBSaw* const osc, Lfloat f)
     @brief Set the frequency of the oscillator.
     @param osc A pointer to the relevant tMBSaw.
     @param freq The new frequency.
     
     @fn Lfloat tMBSaw_sync(tMBSaw* const osc, Lfloat sync)
     @brief Sync this oscillator to another signal.
     @param osc A pointer to the relevant tMBSaw.
     @param sync A sample of the signal to sync to.
     @return The passed in sample.
     
     @fn void tMBSaw_setSyncMode(tMBSaw* const osc, int hardOrSoft)
     @brief Set the sync behavior of the oscillator.
     @param hardOrSoft 0 for hard sync, 1 for soft sync
     ￼￼￼
     @} */
    
    typedef struct tMBSaw
    {
        tMempool* mempool;
        Lfloat    out;
        Lfloat    freq;
        Lfloat    lastsyncin;
        Lfloat    sync;
        Lfloat    syncdir;
        int      softsync;
        Lfloat   _p, _w, _z;
        Lfloat   _inv_w;
        int     _j;
        Lfloat   _f[8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t 	BLEPindices[64];
        Lfloat 	BLEPproperties[64][2];
        Lfloat invSampleRate;
    } tMBSaw;

    // Memory handlers for `tMBSaw`
    void    tMBSaw_init                   (tMBSaw** const osc, LEAF* const leaf);
    void    tMBSaw_initToPool             (tMBSaw** const osc, tMempool** const mempool);
    void    tMBSaw_free                   (tMBSaw** const osc);

    void    tMBSaw_place_step_dd_noBuffer (tMBSaw* const osc, int index, Lfloat phase, Lfloat w, Lfloat scale);

    // Tick function for `tMBSaw`
    Lfloat  tMBSaw_tick                   (tMBSaw* const osc);

    // Setter functions for `tMBSaw`
    Lfloat  tMBSaw_sync                   (tMBSaw* const osc, Lfloat sync);
    void    tMBSaw_setFreq                (tMBSaw* const osc, Lfloat f);
    void    tMBSaw_setPhase               (tMBSaw* const osc, Lfloat phase);
    void    tMBSaw_setSyncMode            (tMBSaw* const osc, int hardOrSoft);
    void    tMBSaw_setBufferOffset        (tMBSaw* const osc, uint32_t offset);
    void    tMBSaw_setSampleRate          (tMBSaw* const osc, Lfloat sr);

    //==============================================================================
    /*!
     @defgroup tmbsaw tMBSawPulse
     @ingroup oscillators
     @brief Saw wave mixed with Pulse wave oscillator with minBLEP anti-aliasing.
     @{

     @fn void tMBSaw_init(tMBSaw** const osc, LEAF* const leaf)
     @brief Initialize a tMBSaw to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBSaw to initialize.

     @fn void tMBSaw_initToPool(tMBSaw** const osc, tMempool** const mempool)
     @brief Initialize a tMBSaw to a specified mempool.
     @param osc A pointer to the tMBSaw to initialize.

     @fn void tMBSaw_free(tMBSaw** const osc)
     @brief Free a tMBSaw from its mempool.
     @param osc A pointer to the tMBSaw to free.

     @fn Lfloat tMBSaw_tick(tMBSaw* const osc)
     @brief Tick the oscillator.
     @param osc A pointer to the relevant tMBSaw.
     @return The ticked sample.

     @fn void tMBSaw_setFreq(tMBSaw* const osc, Lfloat f)
     @brief Set the frequency of the oscillator.
     @param osc A pointer to the relevant tMBSaw.
     @param freq The new frequency.

     @fn Lfloat tMBSaw_sync(tMBSaw* const osc, Lfloat sync)
     @brief Sync this oscillator to another signal.
     @param osc A pointer to the relevant tMBSaw.
     @param sync A sample of the signal to sync to.
     @return The passed in sample.

     @fn void tMBSaw_setSyncMode(tMBSaw* const osc, int hardOrSoft)
     @brief Set the sync behavior of the oscillator.
     @param hardOrSoft 0 for hard sync, 1 for soft sync
     ￼￼￼
     @} */

    typedef struct tMBSawPulse
    {
        tMempool* mempool;
        Lfloat    out;
        Lfloat    freq;
        Lfloat    lastsyncin;
        Lfloat    sync;
        Lfloat    syncdir;
        int      softsync;
        Lfloat    waveform;
        Lfloat   _p, _w, _b, _x, _z, _k;
        int     _j;
        Lfloat _inv_w;
        Lfloat invSampleRate;
        Lfloat 	shape;
        Lfloat   _f [8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t BLEPindices[64];
        Lfloat 	BLEPproperties[64][2];
        Lfloat gain;
        int active;

    } tMBSawPulse;

    // Memory handlers for `tMBSawPulse`
    void    tMBSawPulse_init                   (tMBSawPulse** const osc, LEAF* const leaf);
    void    tMBSawPulse_initToPool             (tMBSawPulse** const osc, tMempool** const mempool);
    void    tMBSawPulse_free                   (tMBSawPulse** const osc);
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSawPulse_place_step_dd_noBuffer(tMBSawPulse* const osc, int index, Lfloat phase, Lfloat inv_w, Lfloat scale);
#else
    void    tMBSawPulse_place_step_dd_noBuffer (tMBSawPulse* const osc, int index, Lfloat phase, Lfloat inv_w,
                                                Lfloat scale);
#endif
    // Tick function for `tMBSawPulse`
    Lfloat  tMBSawPulse_tick                   (tMBSawPulse* const osc);

    // Setter functions for `tMBSawPulse`
    Lfloat  tMBSawPulse_sync                   (tMBSawPulse* const osc, Lfloat sync);
    void    tMBSawPulse_setFreq                (tMBSawPulse* const osc, Lfloat f);
    void    tMBSawPulse_setPhase               (tMBSawPulse* const osc, Lfloat phase);
    void    tMBSawPulse_setShape               (tMBSawPulse* const osc, Lfloat shape);
    void    tMBSawPulse_setSyncMode            (tMBSawPulse* const osc, int hardOrSoft);
    void    tMBSawPulse_setBufferOffset        (tMBSawPulse* const osc, uint32_t offset);
    void    tMBSawPulse_setSampleRate          (tMBSawPulse* const osc, Lfloat sr);

    //==============================================================================
    /*!
     @defgroup ttable tTable
     @ingroup oscillators
     @brief Simple aliasing wavetable oscillator.
     @{
     
     @fn void    tTable_init(tTable** const osc, Lfloat* table, int size, LEAF* const leaf)
     @brief Initialize a tTable to the default mempool of a LEAF instance.
     @param osc A pointer to the tTable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wavetable.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTable_initToPool(tTable** const osc, Lfloat* table, int size, tMempool** const mempool)
     @brief Initialize a tTable to a specified mempool.
     @param osc A pointer to the tTable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wave table.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTable_free(tTable** const osc)
     @brief Free a tTable from its mempool.
     @param osc A pointer to the tTable to free.
     
     @fn Lfloat   tTable_tick         (tTable* const osc)
     @brief Tick a tTable oscillator.
     @param osc A pointer to the relevant tTable.
     @return The ticked sample as a Lfloat from -1 to 1.
     
     @fn void    tTable_setFreq      (tTable* const osc, Lfloat freq)
     @brief Set the frequency of a tTable oscillator.
     @param osc A pointer to the relevant tTable.
     @param freq The frequency to set the oscillator to.
     
     @} */
    
    typedef struct tTable
    {
        tMempool* mempool;
        
        Lfloat* waveTable;
        int size;
        Lfloat inc, freq;
        Lfloat phase;
        Lfloat invSampleRate;
    } tTable;

    // Memory handlers for `tTable`
    void    tTable_init          (tTable** const osc, Lfloat* table, int size, LEAF* const leaf);
    void    tTable_initToPool    (tTable** const osc, Lfloat* table, int size, tMempool** const mempool);
    void    tTable_free          (tTable** const osc);

    // Tick function for `tTable`
    Lfloat  tTable_tick          (tTable* const osc);

    // Setter functions for `tTable`
    void    tTable_setFreq       (tTable* const osc, Lfloat freq);
    void    tTable_setSampleRate (tTable* const osc, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup twavetable tWaveTable
     @ingroup oscillators
     @brief Anti-aliased wavetable generator.
     @{
     
     @fn void    tWaveTable_init(tWaveTable** const osc, Lfloat* table, int size, Lfloat maxFreq, LEAF* const leaf)
     @brief Initialize a tWaveTable to the default mempool of a LEAF instance.
     @param osc A pointer to the tWaveTable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wavetable.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tWaveTable_initToPool(tWaveTable** const osc, Lfloat* table, int size, Lfloat maxFreq, tMempool** const mempool)
     @brief Initialize a tWaveTable to a specified mempool.
     @param osc A pointer to the tWaveTable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wave table.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tWaveTable_free(tWaveTable** const osc)
     @brief Free a tWaveTable from its mempool.
     @param osc A pointer to the tWaveTable to free.
     
     @} */
    
#define LEAF_NUM_WAVETABLE_FILTER_PASSES 5
    
    typedef struct tWaveTable
    {
        tMempool* mempool;
        
        Lfloat* baseTable;
        Lfloat** tables;
        int size;
        int sizeMask;
        int numTables;
        Lfloat maxFreq;
        Lfloat baseFreq, invBaseFreq;
        tButterworth* bl;
        Lfloat sampleRate;
    } tWaveTable;

    // Memory handlers for `tWaveTable`
    void    tWaveTable_init          (tWaveTable** const osc, Lfloat* table, int size, Lfloat maxFreq, LEAF* const leaf);
    void    tWaveTable_initToPool    (tWaveTable** const osc, Lfloat* table, int size, Lfloat maxFreq,
                                      tMempool** const mempool);
    void    tWaveTable_free          (tWaveTable** const osc);

    // Setter functions for `tWaveTable`
    void    tWaveTable_setSampleRate (tWaveTable* const osc, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup twaveosc tWaveOsc
     @ingroup oscillators
     @brief Set of anti-aliased wavetable oscillators that can be faded between.
     @{
     
     @fn void    tWaveOsc_init(tWaveOsc** const osc, const Lfloat** tables, int n, int size, Lfloat maxFreq, LEAF* const leaf)
     @brief Initialize a tWaveOsc to the default mempool of a LEAF instance.
     @param osc A pointer to the tWaveOsc to initialize.
     @param tables An array of pointers to wavetable data.
     @param n The number of wavetables.
     @param leaf A pointer to the leaf instance.
     
     @fn void  tWaveOsc_initToPool(tWaveOsc** const osc, const Lfloat** tables, int n, int size, Lfloat maxFreq, tMempool** const mempool)
     @brief Initialize a tWaveOsc to a specified mempool.
     @param osc A pointer to the tWaveTable to initialize.
     @param tables An array of pointers to wavetable data.
     @param n The number of wavetables.
     @param size The number of samples in each of the wavetables.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tWaveOsc_free(tWaveOsc** const osc)
     @brief Free a tWaveOsc from its mempool.
     @param osc A pointer to the tWaveOsc to free.
     
     @fn Lfloat   tWaveOsc_tick         (tWaveOsc* const osc)
     @brief Tick a tWaveOsc oscillator.
     @param osc A pointer to the relevant tWaveOsc.
     @return The ticked sample as a Lfloat from -1 to 1.
     
     @fn void    tWaveOsc_setFreq      (tWaveOsc* const osc, Lfloat freq)
     @brief Set the frequency of a tWaveOsc oscillator.
     @param osc A pointer to the relevant tWaveOsc.
     @param freq The frequency to set the oscillator to.
     
     @fn void    tWaveOsc_setIndex(tWaveOsc* const osc, Lfloat index)
     @brief Set the output index of the wavetable set.
     @param index The new index from 0.0 to 1.0 as a smooth fade from the first wavetable in the set to the last.
     
     @} */
    
    typedef struct tWaveOsc
       {
           tMempool* mempool;
           tWaveTable** tables;
           int numTables;
           Lfloat index;
           Lfloat maxFreq;
           int o1;
           int o2;
           Lfloat mix;
           uint32_t phase;
           uint32_t inc;
           Lfloat freq;
           Lfloat invSampleRateTimesTwoTo32;
           int oct;
           int size;

           // Determine base frequency
           Lfloat baseFreq;
           Lfloat invBaseFreq;
           Lfloat sampleRate;
           Lfloat w;
           Lfloat aa;
           int numSubTables;

       } tWaveOsc;

    // Memory handlers for `tWaveOsc`
    void    tWaveOsc_init            (tWaveOsc** const cy, tWaveTable** tables, int numTables, LEAF* const leaf);
    void    tWaveOsc_initToPool      (tWaveOsc** const cy, tWaveTable** tables, int numTables, tMempool** const mp);
    void    tWaveOsc_free            (tWaveOsc** const osc);

    // Tick function for `tWaveOsc`
    Lfloat  tWaveOsc_tick            (tWaveOsc* const osc);

    // Setter functions for `tWaveOsc`
    void 	tWaveOsc_setFreq         (tWaveOsc* const cy, Lfloat freq);
    void    tWaveOsc_setAntiAliasing (tWaveOsc* const osc, Lfloat aa);
    void    tWaveOsc_setIndex        (tWaveOsc* const osc, Lfloat index);
    void 	tWaveOsc_setTables       (tWaveOsc* const cy, tWaveTable* tables, int numTables);
    void    tWaveOsc_setSampleRate   (tWaveOsc* const osc, Lfloat sr);

    //==============================================================================
    
    /*!
     @defgroup ttwavetables tWaveTableS
     @ingroup oscillators
     @brief A more space-efficient anti-aliased wavetable generator than tWaveTable but with slightly worse fidelity.
     @{
     
     @fn void    tWaveTableS_init(tWaveTableS** const osc, Lfloat* table, int size, Lfloat maxFreq, LEAF* const leaf)
     @brief Initialize a tWaveTableS to the default mempool of a LEAF instance.
     @param osc A pointer to the tWaveTableS to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wavetable.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tWaveTableS_initToPool(tWaveTableS** const osc, Lfloat* table, int size, Lfloat maxFreq, tMempool** const mempool)
     @brief Initialize a tWaveTableS to a specified mempool.
     @param osc A pointer to the tWaveTableS to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wave table.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tWaveTableS_free(tWaveTableS** const osc)
     @brief Free a tWaveTableS from its mempool.
     @param osc A pointer to the tWaveTableS to free.
     
     @} */
    
    typedef struct tWaveTableS
    {
        tMempool* mempool;
        
        Lfloat* baseTable;
        Lfloat** tables;
        int numTables;
        int* sizes;
        int* sizeMasks;
        Lfloat maxFreq;
        Lfloat baseFreq, invBaseFreq;
        tButterworth* bl;
        Lfloat dsBuffer[2];
        tOversampler* ds;
        Lfloat sampleRate;
    } tWaveTableS;

    // Memory handlers for `tWaveTableS`
    void    tWaveTableS_init          (tWaveTableS** const osc, Lfloat* table, int size, Lfloat maxFreq,
                                       LEAF* const leaf);
    void    tWaveTableS_initToPool    (tWaveTableS** const osc, Lfloat* table, int size, Lfloat maxFreq,
                                       tMempool** const mempool);
    void    tWaveTableS_free          (tWaveTableS** const osc);

    // Setter functions for `tWaveTableS`
    void    tWaveTableS_setSampleRate (tWaveTableS* const osc, Lfloat sr);
    

    //==============================================================================
    /*!
     @defgroup twaveoscs tWaveOscS
     @ingroup oscillators
     @brief Set of anti-aliased wavetable oscillators that can be faded between.
     @{
     
     @fn void    tWaveOscS_init(tWaveOsc** const osc, const Lfloat** tables, int n, int size, Lfloat maxFreq, LEAF* const leaf)
     @brief Initialize a tWaveOscS to the default mempool of a LEAF instance.
     @param osc A pointer to the tWaveOscS to initialize.
     @param tables An array of pointers to wavetable data.
     @param n The number of wavetables.
     @param leaf A pointer to the leaf instance.
     
     @fn void  tWaveOscS_initToPool(tWaveOsc** const osc, const Lfloat** tables, int n, int size, Lfloat maxFreq, tMempool** const mempool)
     @brief Initialize a tWaveOscS to a specified mempool.
     @param osc A pointer to the tWaveTableS to initialize.
     @param tables An array of pointers to wavetable data.
     @param n The number of wavetables.
     @param size The number of samples in each of the wavetables.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tWaveOscS_free(tWaveOsc** const osc)
     @brief Free a tWaveOscS from its mempool.
     @param osc A pointer to the tWaveOscS to free.
     
     @fn Lfloat   tWaveOscS_tick         (tWaveOsc* const osc)
     @brief Tick a tWaveOscS oscillator.
     @param osc A pointer to the relevant tWaveOscS.
     @return The ticked sample as a Lfloat from -1 to 1.
     
     @fn void    tWaveOscS_setFreq      (tWaveOsc* const osc, Lfloat freq)
     @brief Set the frequency of a tWaveOscS oscillator.
     @param osc A pointer to the relevant tWaveOscS.
     @param freq The frequency to set the oscillator to.
     
     @fn void    tWaveOscS_setIndex(tWaveOsc* const osc, Lfloat index)
     @brief Set the output index of the wavetable set.
     @param index The new index from 0.0 to 1.0 as a smooth fade from the first wavetable in the set to the last.
     
     @} */
    
    typedef struct tWaveOscS
    {
        tMempool* mempool;
        
        //tWaveTableS* tables;

        tWaveTableS** tables;

        int numTables;
        Lfloat index;
        Lfloat maxFreq;
        int o1;
        int o2;
        Lfloat mix;
        uint32_t phase;
        uint32_t inc;
        Lfloat freq;
        Lfloat invSampleRateTimesTwoTo32;
        int oct;
        int size;

        // Determine base frequency
        Lfloat baseFreq;
        Lfloat invBaseFreq;
        Lfloat sampleRate;
        Lfloat w;
        Lfloat aa;
        int numSubTables;

    } tWaveOscS;

    // Memory handlers for `tWaveOscS`
    void 	tWaveOscS_init            (tWaveOscS** const cy, tWaveTableS** tables, int numTables, LEAF* const leaf);
    void    tWaveOscS_initToPool      (tWaveOscS** const osc, tWaveTableS** tables, int numTables,
                                       tMempool** const mempool);
    void    tWaveOscS_free            (tWaveOscS** const osc);

    // Tick function for `tWaveOscS`
    Lfloat  tWaveOscS_tick            (tWaveOscS* const osc);

    // Setter functions for `tWaveOscS`
    void    tWaveOscS_setFreq         (tWaveOscS* const osc, Lfloat freq);
    void    tWaveOscS_setAntiAliasing (tWaveOscS* const osc, Lfloat aa);
    void    tWaveOscS_setIndex        (tWaveOscS* const osc, Lfloat index);
    void    tWaveOscS_setSampleRate   (tWaveOscS* const osc, Lfloat sr);
    



     //==============================================================================
    
    /*!
     @defgroup tIntphasor tIntPhasor
     @ingroup oscillators
     @brief Aliasing phasor.
     @{
     
     @fn void    tIntPhasor_init(tIntPhasor** const osc, LEAF* const leaf)
     @brief Initialize a tIntPhasor to the default mempool of a LEAF instance.
     @param osc A pointer to the tIntPhasor to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tIntPhasor_initToPool(tIntPhasor** const osc, tMempool** const)
     @brief Initialize a tIntPhasor to a specified mempool.
     @param osc A pointer to the tIntPhasor to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tIntPhasor_free(tIntPhasor** const osc)
     @brief Free a tIntPhasor from its mempool.
     @param osc A pointer to the tIntPhasor to free.
     
     @fn Lfloat   tIntPhasor_tick        (tIntPhasor* const osc)
     @brief
     @param osc A pointer to the relevant tIntPhasor.
     
     @fn void    tIntPhasor_setFreq     (tIntPhasor* const osc, Lfloat freq)
     @brief
     @param osc A pointer to the relevant tIntPhasor.
     ￼￼￼
     @} */
    
    typedef struct tIntPhasor
    {

        tMempool* mempool;
        uint32_t phase;
        uint32_t inc;
        Lfloat freq;
        int32_t mask;
        uint8_t phaseDidReset;
        Lfloat invSampleRateTimesTwoTo32;
    } tIntPhasor;

    // Memory handlers for `tIntPhasor`
    void    tIntPhasor_init          (tIntPhasor** const osc, LEAF* const leaf);
    void    tIntPhasor_initToPool    (tIntPhasor** const osc, tMempool** const);
    void    tIntPhasor_free          (tIntPhasor** const osc);

    // Tick function for `tIntPhasor`
    Lfloat  tIntPhasor_tick          (tIntPhasor* const osc);
    Lfloat  tIntPhasor_tickBiPolar   (tIntPhasor* const osc);
    // Setter functions for `tIntPhasor`
    void    tIntPhasor_setFreq       (tIntPhasor* const osc, Lfloat freq);
    void    tIntPhasor_setSampleRate (tIntPhasor* const osc, Lfloat sr);
    void    tIntPhasor_setPhase      (tIntPhasor* const cy, Lfloat phase);
    
         //==============================================================================
    
    /*!
     @defgroup tSquareLFO tSquareLFO
     @ingroup oscillators
     @brief Aliasing phasor.
     @{
     
     @fn void    tSquareLFO_init(tSquareLFO** const osc, LEAF* const leaf)
     @brief Initialize a tSquareLFO to the default mempool of a LEAF instance.
     @param osc A pointer to the tSquareLFO to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSquareLFO_initToPool(tSquareLFO** const osc, tMempool** const)
     @brief Initialize a tSquareLFO to a specified mempool.
     @param osc A pointer to the tSquareLFO to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSquareLFO_free(tSquareLFO** const osc)
     @brief Free a tSquareLFO from its mempool.
     @param osc A pointer to the tSquareLFO to free.
     
     @fn Lfloat   tSquareLFO_tick        (tSquareLFO* const osc)
     @brief
     @param osc A pointer to the relevant tSquareLFO.
     
     @fn void    tSquareLFO_setFreq     (tSquareLFO* const osc, Lfloat freq)
     @brief
     @param osc A pointer to the relevant tSquareLFO.
     ￼￼￼
     @} */
    
    typedef struct tSquareLFO
    {

        tMempool* mempool;
        Lfloat pulsewidth;
        tIntPhasor* phasor;
        tIntPhasor* invPhasor;
    } tSquareLFO;

    // Memory handlers for `tSquareLFO`
    void    tSquareLFO_init          (tSquareLFO** const osc, LEAF* const leaf);
    void    tSquareLFO_initToPool    (tSquareLFO** const osc, tMempool** const);
    void    tSquareLFO_free          (tSquareLFO** const osc);

    // Tick function for `tSquareLFO`
    Lfloat  tSquareLFO_tick          (tSquareLFO* const osc);

    // Setter functions for `tSquareLFO`
    void    tSquareLFO_setFreq       (tSquareLFO* const osc, Lfloat freq);
    void    tSquareLFO_setSampleRate (tSquareLFO* const osc, Lfloat sr);
    void    tSquareLFO_setPulseWidth (tSquareLFO* const cy, Lfloat pw);
    void    tSquareLFO_setPhase      (tSquareLFO* const cy, Lfloat phase);

    typedef struct tSawSquareLFO
    {
        tMempool* mempool;
        Lfloat shape;
        tIntPhasor*  saw;
        tSquareLFO*  square;
    } tSawSquareLFO;

    // Memory handlers for `tSawSquareLFO`
    void    tSawSquareLFO_init          (tSawSquareLFO** const osc, LEAF* const leaf);
    void    tSawSquareLFO_initToPool    (tSawSquareLFO** const osc, tMempool** const);
    void    tSawSquareLFO_free          (tSawSquareLFO** const osc);

    // Tick function for `tSawSquareLFO`
    Lfloat  tSawSquareLFO_tick          (tSawSquareLFO* const osc);

    // Setter functions for `tSawSquareLFO`
    void    tSawSquareLFO_setFreq       (tSawSquareLFO* const osc, Lfloat freq);
    void    tSawSquareLFO_setSampleRate (tSawSquareLFO* const osc, Lfloat sr);
    void    tSawSquareLFO_setPhase      (tSawSquareLFO* const cy, Lfloat phase);
    void    tSawSquareLFO_setShape      (tSawSquareLFO* const cy, Lfloat shape);

        //==============================================================================
 /*!
     @defgroup tTriLFO tTriLFO
     @ingroup oscillators
     @brief Aliasing phasor.
     @{
     
     @fn void    tTriLFO_init(tTriLFO** const osc, LEAF* const leaf)
     @brief Initialize a tTriLFO to the default mempool of a LEAF instance.
     @param osc A pointer to the tTriLFO to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTriLFO_initToPool(tTriLFO** const osc, tMempool** const)
     @brief Initialize a tTriLFO to a specified mempool.
     @param osc A pointer to the tTriLFO to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTriLFO_free(tTriLFO** const osc)
     @brief Free a tTriLFO from its mempool.
     @param osc A pointer to the tTriLFO to free.
     
     @fn Lfloat   tTriLFO_tick        (tTriLFO* const osc)
     @brief
     @param osc A pointer to the relevant tTriLFO.
     
     @fn void    tTriLFO_setFreq     (tTriLFO* const osc, Lfloat freq)
     @brief
     @param osc A pointer to the relevant tTriLFO.
     ￼￼￼
     @} */
    
    typedef struct tTriLFO
    {

        tMempool* mempool;
        int32_t phase;
        int32_t inc;
        Lfloat freq;
        Lfloat invSampleRate;
        Lfloat invSampleRateTimesTwoTo32;
    } tTriLFO;

    // Memory handlers for `tTriLFO`
    void    tTriLFO_init          (tTriLFO** const osc, LEAF* const leaf);
    void    tTriLFO_initToPool    (tTriLFO** const osc, tMempool** const);
    void    tTriLFO_free          (tTriLFO** const osc);

    // Tick function for `tTriLFO`
    Lfloat  tTriLFO_tick          (tTriLFO* const osc);

    // Setter functions for `tTriLFO`
    void    tTriLFO_setFreq       (tTriLFO* const osc, Lfloat freq);
    void    tTriLFO_setSampleRate (tTriLFO* const osc, Lfloat sr);
    void    tTriLFO_setPhase      (tTriLFO* const cy, Lfloat phase);

    typedef struct tSineTriLFO
    {
        tMempool* mempool;
        Lfloat shape;
        tTriLFO*  tri;
        tCycle*  sine;
    } tSineTriLFO;

    // Memory handlers for `tSineTriLFO`
    void    tSineTriLFO_init          (tSineTriLFO** const osc, LEAF* const leaf);
    void    tSineTriLFO_initToPool    (tSineTriLFO** const osc, tMempool** const);
    void    tSineTriLFO_free          (tSineTriLFO** const osc);

    // Tick function for `tSineTriLFO`
    Lfloat  tSineTriLFO_tick          (tSineTriLFO* const osc);

    // Setter functions for `tSineTriLFO`
    void    tSineTriLFO_setFreq       (tSineTriLFO* const osc, Lfloat freq);
    void    tSineTriLFO_setSampleRate (tSineTriLFO* const osc, Lfloat sr);
    void    tSineTriLFO_setPhase      (tSineTriLFO* const cy, Lfloat phase);
    void    tSineTriLFO_setShape      (tSineTriLFO* const cy, Lfloat shape);



typedef struct tDampedOscillator
	{
		tMempool* mempool;

		Lfloat freq_;
		Lfloat decay_;
		Lfloat two_pi_by_sample_rate_;
		Lfloat loop_gain_;
		Lfloat turns_ratio_;
		Lfloat x_;
		Lfloat y_;
	} tDampedOscillator;

    // Memory handlers for `tDampedOscillator`
	void    tDampedOscillator_init          (tDampedOscillator** const osc, LEAF* const leaf);
	void    tDampedOscillator_initToPool    (tDampedOscillator** const osc, tMempool** const mempool);
	void    tDampedOscillator_free          (tDampedOscillator** const osc);

    // Tick function for `tDampedOscillator`
	Lfloat  tDampedOscillator_tick          (tDampedOscillator* const osc);

    // Setter functions for `tDampedOscillator`
	void    tDampedOscillator_setFreq       (tDampedOscillator* const osc, Lfloat freq);
	void    tDampedOscillator_setSampleRate (tDampedOscillator* const osc, Lfloat sr);
	void 	tDampedOscillator_setDecay      (tDampedOscillator* const osc, Lfloat decay);
	void 	tDampedOscillator_reset         (tDampedOscillator* const osc);


    typedef struct tPlutaQuadOsc
    {
        tMempool* mempool;
        uint32_t oversamplingRatio;
        uint32_t phase[4];
        Lfloat biPolarOutputs[4];
        uint32_t inc[4];
        Lfloat freq[4];
        Lfloat fmMatrix[4][4];
        Lfloat outputAmplitudes[4];
        tButterworth*  lowpass;
        int32_t mask;
        Lfloat invSampleRateTimesTwoTo32;
    } tPlutaQuadOsc;

    // Memory handlers for `tDampedOscillator`
    void    tPlutaQuadOsc_init          (tPlutaQuadOsc** const osc,  uint32_t oversamplingRatio, LEAF* const leaf);
    void    tPlutaQuadOsc_initToPool    (tPlutaQuadOsc** const cy, uint32_t oversamplingRatio, tMempool** const mp);
    void    tPlutaQuadOsc_free          (tPlutaQuadOsc** const osc);

    // Tick function for `tDampedOscillator`
    Lfloat  tPlutaQuadOsc_tick          (tPlutaQuadOsc* const osc);
    void   tPlutaQuadOsc_setFreq        (tPlutaQuadOsc* const c, uint32_t whichOsc, Lfloat freq);
    void   tPlutaQuadOsc_setFmAmount        (tPlutaQuadOsc* const c, uint32_t const whichCarrier, uint32_t const whichModulator, Lfloat const amount);
    void   tPlutaQuadOsc_setOutputAmplitude        (tPlutaQuadOsc* const c, uint32_t const whichOsc, Lfloat const amplitude);
#ifdef __cplusplus
}
#endif
#endif  // LEAF_OSCILLATORS_H_INCLUDED

//==============================================================================
