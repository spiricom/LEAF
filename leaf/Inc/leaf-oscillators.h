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

     @fn void    tCycle_init(tCycle* const osc, LEAF* const leaf)
     @brief Initialize a tCycle to the default mempool of a LEAF instance.
     @param osc A pointer to the tCycle to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tCycle_init                 (tMempool** const mempool)
     @brief Initialize a tCycle to a specified mempool.
     @param osc A pointer to the tCycle to initialize.
     @param mempool A pointer to the tMempool to use.

     @fn void    tCycle_free(tCycle** const osc)
     @brief Free a tCycle from its mempool.
     @param osc A pointer to the tCycle to free.
     
     @fn float   tCycle_tick         (tCycle* const osc)
     @brief Tick a tCycle oscillator.
     @param osc A pointer to the relevant tCycle.
     @return The ticked sample as a float from -1 to 1.

     @fn void    tCycle_setFreq      (tCycle* const osc, tCycle** const osc, float freq)
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
		float freq;
        float invSampleRateTimesTwoTo32;
        uint32_t mask;
    } tCycle;

    // Memory handlers for `tCycle`
    void    tCycle_create               (tMempool** const mempool, tCycle** const);
    void    tCycle_init                 (LEAF* const leaf, tCycle* const osc);
    void    tCycle_free          (tCycle** const osc);

    // Tick function for `tCycle`
    float  tCycle_tick          (tCycle* const osc);

    // Setter functions for `tCycle`
    void    tCycle_setFreq       (tCycle* const osc, float freq);
    void    tCycle_setPhase      (tCycle* const osc, float phase);
    void    tCycle_setSampleRate (tCycle* const osc, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup ttriangle tTriangle
     @ingroup oscillators
     @brief Anti-aliased wavetable triangle wave oscillator.
     @{
     
     @fn void    tTriangle_init(tTriangle* const osc, LEAF* const leaf)
     @brief Initialize a tTriangle to the default mempool of a LEAF instance.
     @param osc A pointer to the tTriangle to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTriangle_init                 (tMempool** const mempool)
     @brief Initialize a tTriangle to a specified mempool.
     @param osc A pointer to the tTriangle to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTriangle_free(tTriangle** const osc)
     @brief Free a tTriangle from its mempool.
     @param osc A pointer to the tTriangle to free.
     
     @fn float   tTriangle_tick         (tTriangle* const osc)
     @brief Tick a tTriangle oscillator.
     @param osc A pointer to the relevant tTriangle.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tTriangle_setFreq      (tTriangle* const osc, tTriangle** const osc, float freq)
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
        float freq;
        int oct;
        float w;
        float invSampleRate;
        float invSampleRateTimesTwoTo32;
        uint32_t mask;
    } tTriangle;

    // Memory handlers for `tTriangle`
    void    tTriangle_create               (tMempool** const mempool, tTriangle** const);
    void    tTriangle_init                 (LEAF* const leaf, tTriangle* const osc);
    void    tTriangle_free          (tTriangle** const osc);

    // Tick function for `tTriangle`
    float  tTriangle_tick          (tTriangle* const osc);

    // Setter functions for `tTriangle`
    void    tTriangle_setFreq       (tTriangle* const osc, float freq);
    void    tTriangle_setPhase      (tTriangle* const osc, float phase);
    void    tTriangle_setSampleRate (tTriangle* const osc, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tsquare tSquare
     @ingroup oscillators
     @brief Anti-aliased wavetable square wave oscillator.
     @{
     
     @fn void    tSquare_init(tSquare* const osc, LEAF* const leaf)
     @brief Initialize a tSquare to the default mempool of a LEAF instance.
     @param osc A pointer to the tSquare to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSquare_init                 (tMempool** const mempool)
     @brief Initialize a tSquare to a specified mempool.
     @param osc A pointer to the tSquare to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSquare_free(tSquare** const osc)
     @brief Free a tSquare from its mempool.
     @param osc A pointer to the tSquare to free.
     
     @fn float   tSquare_tick         (tSquare* const osc)
     @brief Tick a tSquare oscillator.
     @param osc A pointer to the relevant tSquare.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tSquare_setFreq      (tSquare* const osc, tSquare** const osc, float freq)
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
        float freq;
        int oct;
        float w;
        float invSampleRate;
        float invSampleRateTimesTwoTo32;
        uint32_t mask;
    } tSquare;

    // Memory handlers for `tSquare`
    void    tSquare_create               (tMempool** const mempool, tSquare** const);
    void    tSquare_init                 (LEAF* const leaf, tSquare* const osc);
    void    tSquare_free          (tSquare** const osc);

    // Tick function for `tSquare`
    float  tSquare_tick          (tSquare* const osc);

    // Setter functions for `tSquare`
    void    tSquare_setFreq       (tSquare* const osc, float freq);
    void    tSquare_setPhase      (tSquare* const osc, float phase);
    void    tSquare_setSampleRate (tSquare* const osc, float sr);
    
    /*!￼￼￼
     @} */
    
    //==============================================================================
    
    /*!
     @defgroup tsawtooth tSawtooth
     @ingroup oscillators
     @brief Anti-aliased wavetable saw wave oscillator.
     @{
     
     @fn void    tSawtooth_init(tSawtooth* const osc, LEAF* const leaf)
     @brief Initialize a tSawtooth to the default mempool of a LEAF instance.
     @param osc A pointer to the tSawtooth to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSawtooth_init                 (tMempool** const mempool)
     @brief Initialize a tSawtooth to a specified mempool.
     @param osc A pointer to the tSawtooth to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSawtooth_free(tSawtooth** const osc)
     @brief Free a tSawtooth from its mempool.
     @param osc A pointer to the tSawtooth to free.
     
     @fn float   tSawtooth_tick         (tSawtooth* const osc)
     @brief Tick a tSawtooth oscillator.
     @param osc A pointer to the relevant tSawtooth.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tSawtooth_setFreq      (tSawtooth* const osc, tSawtooth** const osc, float freq)
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
        float freq;
        int oct;
        float w;
        float invSampleRate;
        float invSampleRateTimesTwoTo32;
        uint32_t mask;
    } tSawtooth;

    // Memory handlers for `tSawtooth`
    void    tSawtooth_create               (tMempool** const mempool, tSawtooth** const);
    void    tSawtooth_init                 (LEAF* const leaf, tSawtooth* const osc);
    void    tSawtooth_free          (tSawtooth** const osc);

    // Tick function for `tSawtooth`
    float  tSawtooth_tick          (tSawtooth* const osc);

    // Setter functions for `tSawtooth`
    void    tSawtooth_setFreq       (tSawtooth* const osc, float freq);
    void    tSawtooth_setPhase      (tSawtooth* const osc, float phase);
    void    tSawtooth_setSampleRate (tSawtooth* const osc, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tpbtriangle tPBTriangle
     @ingroup oscillators
     @brief Triangle wave oscillator with polyBLEP anti-aliasing.
     @{
     
     @fn void   tPBTriangle_init(tPBTriangle* const osc, LEAF* const leaf)
     @brief Initialize a tPBTriangle to the default mempool of a LEAF instance.
     @param osc A pointer to the tPBTriangle to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPBTriangle_init                 (tMempool** const mempool)
     @brief Initialize a tPBTriangle to a specified mempool.
     @param osc A pointer to the tPBTriangle to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPBTriangle_free(tPBTriangle** const osc)
     @brief Free a tTri from its mempool.
     @param osc A pointer to the tPBTriangle to free.
     
     @fn float   tPBTriangle_tick          (tPBTriangle* const osc)
     @brief
     @param osc A pointer to the relevant tPBTriangle.
     
     @fn void    tPBTriangle_setFreq       (tPBTriangle* const osc, tPBTriangle** const osc, float freq)
     @brief
     @param osc A pointer to the relevant tPBTriangle.
     
     @fn void    tPBTriangle_setSkew       (tPBTriangle* const osc, float skew)
     @brief
     @param osc A pointer to the relevant tPBTriangle.
     ￼￼￼
     @} */

    typedef struct tPBSineTriangle
    {
        tMempool* mempool;
        uint32_t phase;
        tCycle sine;
        int32_t inc;
        float freq;
        float shape;
        float oneMinusShape;
        float invSampleRate;
        float invSampleRateTimesTwoTo32;
    } tPBSineTriangle;

    // Memory handlers for `tPBSineTriangle`
    void    tPBSineTriangle_create               (tMempool** const mempool, tPBSineTriangle** const);
    void    tPBSineTriangle_init                 (LEAF* const leaf, tPBSineTriangle* const osc);
    void    tPBSineTriangle_free          (tPBSineTriangle** const osc);


#ifdef ITCMRAM
    float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSineTriangle_tick          (tPBSineTriangle* const osc);
#else
    // Tick function for `tPBSineTriangle`
    float  tPBSineTriangle_tick          (tPBSineTriangle* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSineTriangle_setFreq       (tPBSineTriangle* const osc, float freq);
#else
    // Setter functions for `tPBSineTriangle`
    void    tPBSineTriangle_setFreq       (tPBSineTriangle* const osc, float freq);
#endif
    void    tPBSineTriangle_setShape      (tPBSineTriangle* const osc, float shape);
    void    tPBSineTriangle_setSampleRate (tPBSineTriangle* const osc, float sr);

    //==============================================================================


    typedef struct tPBTriangle
    {
        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        float freq;
        uint32_t width;
        uint32_t oneMinusWidth;
        float lastOutput;
        float invSampleRate;
        float invSampleRateTimesTwoTo32;
    } tPBTriangle;

    // Memory handlers for `tPBTriangle`
    void    tPBTriangle_create               (tMempool** const mempool, tPBTriangle** const);
    void    tPBTriangle_init                 (LEAF* const leaf, tPBTriangle* const osc);
    void    tPBTriangle_free          (tPBTriangle** const osc);
    
#ifdef ITCMRAM
    float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBTriangle_tick          (tPBTriangle* const osc);
#else
    // Tick function for `tPBTriangle`
    float  tPBTriangle_tick          (tPBTriangle* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32)))  tPBTriangle_setFreq       (tPBTriangle* const osc, float freq);
#else
    // Setter functions for `tPBTriangle`
    void    tPBTriangle_setFreq       (tPBTriangle* const osc, float freq);
#endif
    void    tPBTriangle_setSkew       (tPBTriangle* const osc, float skew);
    void    tPBTriangle_setSampleRate (tPBTriangle* const osc, float sr);
    
    //==============================================================================
    /*!
     @defgroup tpbpulse tPBPulse
     @ingroup oscillators
     @brief Pulse wave oscillator with polyBLEP anti-aliasing.
     @{
     
     @fn void    tPBPulse_init(tPBPulse* const osc, LEAF* const leaf)
     @brief Initialize a tPBPulse to the default mempool of a LEAF instance.
     @param osc A pointer to the tPBPulse to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPBPulse_init                 (tMempool** const)
     @brief Initialize a tPBPulse to a specified mempool.
     @param osc A pointer to the tPBPulse to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPBPulse_free(tPBPulse** const osc)
     @brief Free a tPBPulse from its mempool.
     @param osc A pointer to the tPBPulse to free.
     
     @fn float   tPBPulse_tick        (tPBPulse* const osc)
     @brief
     @param osc A pointer to the relevant tPBPulse.
     
     @fn void    tPBPulse_setFreq     (tPBPulse* const osc, tPBPulse** const osc, float freq)
     @brief
     @param osc A pointer to the relevant tPBPulse.
     
     @fn void    tPBPulse_setWidth    (tPBPulse* const osc, float width)
     @brief
     @param osc A pointer to the relevant tPBPulse.
     ￼￼￼
     @} */
    
    typedef struct tPBPulse
    {
        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        float freq;
        uint32_t width;
        uint32_t oneMinusWidth;
        float invSampleRate;
        float invSampleRateTimesTwoTo32;
    } tPBPulse;

    // Memory handlers for `tPBPulse`
    void    tPBPulse_create               (tMempool** const mempool, tPBPulse** const);
    void    tPBPulse_init                 (LEAF* const leaf, tPBPulse* const osc);
    void    tPBPulse_free          (tPBPulse** const osc);
    
#ifdef ITCMRAM
    float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBPulse_tick        (tPBPulse* const osc);
#else
    // Tick function for `tPBPulse`
    float  tPBPulse_tick          (tPBPulse* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBPulse_setFreq     (tPBPulse* const osc, float freq);
#else
    // Setter functions for `tPBPulse`
    void    tPBPulse_setFreq       (tPBPulse* const osc, float freq);
#endif
    void    tPBPulse_setWidth      (tPBPulse* const osc, float width);
    void    tPBPulse_setSampleRate (tPBPulse* const osc, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tpbsaw tPBSaw
     @ingroup oscillators
     @brief Saw wave oscillator with polyBLEP anti-aliasing.
     @{
     
     @fn void    tPBSaw_init(tPBSaw* const osc, LEAF* const leaf)
     @brief Initialize a tPBSaw to the default mempool of a LEAF instance.
     @param osc A pointer to the tPBSaw to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPBSaw_init                 (tMempool** const mempool)
     @brief Initialize a tPBSaw to a specified mempool.
     @param osc A pointer to the tPBSaw to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPBSaw_free(tPBSaw** const osc)
     @brief Free a tPBSaw from its mempool.
     @param osc A pointer to the tPBSaw to free.
     
     @fn float   tPBSaw_tick          (tPBSaw* const osc)
     @brief
     @param osc A pointer to the relevant tPBSaw.
     
     @fn void    tPBSaw_setFreq       (tPBSaw* const osc, tPBSaw** const osc, float freq)
     @brief
     @param osc A pointer to the relevant tPBSaw.
     ￼￼￼
     @} */
    
    typedef struct tPBSaw
    {
        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        float freq;
        float invSampleRate;
        float invSampleRateTimesTwoTo32;
        float lastsyncin;
        float sync;
    } tPBSaw;

    // Memory handlers for `tPBSaw`
    void    tPBSaw_create               (tMempool** const mempool, tPBSaw** const);
    void    tPBSaw_init                 (LEAF* const leaf, tPBSaw* const osc);
    void    tPBSaw_free          (tPBSaw** const osc);
    
#ifdef ITCMRAM
    float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSaw_tick          (tPBSaw* const osc);
#else
    // Tick function for `tPBSaw`
    float  tPBSaw_tick          (tPBSaw* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSaw_setFreq       (tPBSaw* const osc, float freq);
#else
    // Setter functions for `tPBSaw`
    void    tPBSaw_setFreq       (tPBSaw* const osc, float freq);
#endif
    void    tPBSaw_setSampleRate (tPBSaw* const osc, float sr);
    
    //==============================================================================
    
typedef struct tPBSawSquare
{
    tMempool* mempool;
    uint32_t phase;
    int32_t inc;
    float freq;
    float invSampleRate;
    float invSampleRateTimesTwoTo32;
    float lastsyncin;
    float sync;
    float shape;
    float oneMinusShape;

} tPBSawSquare;

    // Memory handlers for `tPBSawSquare`
    void    tPBSawSquare_create               (tMempool** const mempool, tPBSawSquare** const);
    void    tPBSawSquare_init                 (LEAF* const leaf, tPBSawSquare* const osc);
    void    tPBSawSquare_free          (tPBSawSquare** const osc);

#ifdef ITCMRAM
    float __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSawSquare_tick          (tPBSawSquare* const osc);
#else
    // Tick function for `tPBSawSquare`
    float  tPBSawSquare_tick          (tPBSawSquare* const osc);
#endif
#ifdef ITCMRAM
    void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tPBSawSquare_setFreq       (tPBSawSquare* const osc, float freq);
#else
    // Setter functions for `tPBSawSquare`
    void    tPBSawSquare_setFreq       (tPBSawSquare* const osc, float freq);
#endif
    void    tPBSawSquare_setShape      (tPBSawSquare* const osc, float shape);
    void    tPBSawSquare_setSampleRate (tPBSawSquare* const osc, float sr);

//==============================================================================
    typedef struct tSawOS
    {
        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        float freq;
        int32_t mask;
        uint8_t phaseDidReset;
        float invSampleRateOS;
        uint8_t OSratio;
        uint32_t invSampleRateTimesTwoTo32OS;
        tSVF* aaFilters;
        uint8_t filterOrder;
    } tSawOS;

    // Memory handlers for `tSawOS`
    void    tSawOS_create               (tMempool** const mempool, tSawOS** const);
    void    tSawOS_init                 (LEAF* const leaf, tSawOS* const osc, uint8_t OS_ratio, uint8_t filterOrder);
    void    tSawOS_free          (tSawOS** const osc);

    // Tick function for `tSawOS`
    float  tSawOS_tick          (tSawOS* const osc);

    // Setter functions for `tSawOS`
    void    tSawOS_setFreq       (tSawOS* const osc, float freq);
    void    tSawOS_setSampleRate (tSawOS* const osc, float sr);

    //==============================================================================


    /*!
     @defgroup tphasor tPhasor
     @ingroup oscillators
     @brief Aliasing phasor.
     @{
     
     @fn void    tPhasor_init(tPhasor* const osc, LEAF* const leaf)
     @brief Initialize a tPhasor to the default mempool of a LEAF instance.
     @param osc A pointer to the tPhasor to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPhasor_init                 (tMempool** const)
     @brief Initialize a tPhasor to a specified mempool.
     @param osc A pointer to the tPhasor to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPhasor_free(tPhasor** const osc)
     @brief Free a tPhasor from its mempool.
     @param osc A pointer to the tPhasor to free.
     
     @fn float   tPhasor_tick        (tPhasor* const osc)
     @brief
     @param osc A pointer to the relevant tPhasor.
     
     @fn void    tPhasor_setFreq     (tPhasor* const osc, tPhasor** const osc, float freq)
     @brief
     @param osc A pointer to the relevant tPhasor.
     ￼￼￼
     @} */
    
    typedef struct tPhasor
    {

        tMempool* mempool;
        uint32_t phase;
        int32_t inc;
        float freq;
        float invSampleRate;
        float invSampleRateTimesTwoTo32;
    } tPhasor;

    // Memory handlers for `tPhasor`
    void    tPhasor_create               (tMempool** const mempool, tPhasor** const);
    void    tPhasor_init                 (LEAF* const leaf, tPhasor* const osc);
    void    tPhasor_free          (tPhasor** const osc);

    // Tick function for `tPhasor`
    float  tPhasor_tick          (tPhasor* const osc);

    // Setter functions for `tPhasor`
    void    tPhasor_setFreq       (tPhasor* const osc, float freq);
    void    tPhasor_setSampleRate (tPhasor* const osc, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tnoise tNoise
     @ingroup oscillators
     @brief Noise generator, capable of producing white or pink noise.
     @{
     
     @fn void    tNoise_init(tNoise* const noise, NoiseType type, LEAF* const leaf)
     @brief Initialize a tNoise to the default mempool of a LEAF instance.
     @param noise A pointer to the tNoise to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tNoise_init                 (tMempool** const)
     @brief Initialize a tNoise to a specified mempool.
     @param noise A pointer to the tNoise to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tNoise_free(tNoise** const noise)
     @brief Free a tNoise from its mempool.
     @param noise A pointer to the tNoise to free.
     
     @fn float   tNoise_tick         (tNoise* const noise)
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

    
    typedef struct tNoise tNoise;
    struct tNoise
    {
        tMempool* mempool;
        NoiseType type;
        float pinkb0, pinkb1, pinkb2;
        float   (*rand)(void);
    };

    // Memory handlers for `tNoise`
    void    tNoise_create               (tMempool** const mempool, tNoise** const);
    void    tNoise_init                 (LEAF* const leaf, tNoise* const noise, NoiseType type);
    void    tNoise_free         (tNoise** const noise);

    // Tick function for `tNoise`
    float  tNoise_tick         (tNoise* const noise);
    
    //==============================================================================

    /*!
     @defgroup tperlinnoise tPerlinNoise
     @ingroup oscillators
     @brief Perlin noise generator, capable of producing different resolutions of Perlin noise.
     @{

     @fn void    tPerlinNoise_init(tPerlinNoise* const perlinNoise, InterpOrder order, LEAF* const leaf)
     @brief Initialize a tPerlinNoise to the default mempool of a LEAF instance.
     @param perlinNoise A pointer to the tPerlinNoise to initialize.
     @param leaf A pointer to the leaf instance.

     @fn void    tPerlinNoise_init                 (tMempool** const)
     @brief Initialize a tPerlinNoise to a specified mempool.
     @param perlinNoise A pointer to the tPerlinNoise to initialize.
     @param mempool A pointer to the tMempool to use.

     @fn void    tPerlinNoise_free(tPerlinNoise** const perlinNoise)
     @brief Free a tPerlinNoise from its mempool.
     @param perlinNoise A pointer to the tNoise to free.

     @fn float   tPerlinNoise_tick         (tPerlinNoise* const perlinNoise)
     @brief
     @param perlinNoise A pointer to the relevant tPerlinNoise.
     */

    typedef struct tPerlinNoise tPerlinNoise;
    struct tPerlinNoise
    {
        tMempool* mempool;

        float rateMs;
        float energy;

        int counter;
        int buffSize;
        float x1;
        float x2;
        float m1;
        float m2;
        float currX;
        float scaler;

        float   (*rand)(void);
        float   (*interper)(float, float, float, float, float);
    };

    // Memory handlers for `tPerlinNoise`
    void    tPerlinNoise_create               (tMempool** const mempool, tPerlinNoise** const);
    void    tPerlinNoise_init                 (LEAF* const leaf, tPerlinNoise* const perlinNoise, float rateMs, float energy);
    void    tPerlinNoise_free         (tPerlinNoise** const perlinNoise);

    // Tick function for `tPerlinNoise`
    float  tPerlinNoise_tick        (tPerlinNoise* const perlinNoise);

    // Setter functions for `tPerlinNoise`
    void    tPerlinNoise_setRate    (LEAF* const leaf, tPerlinNoise* const perlinNoise, float rate);
    void    tPerlinNoise_setEnergy  (LEAF* const leaf, tPerlinNoise* const perlinNoise, float energy);

//==============================================================================
    
    /*!
     @defgroup tneuron tNeuron
     @ingroup oscillators
     @brief Model of a neuron, adapted to act as an oscillator.
     @{
     
     @fn void    tNeuron_init(tNeuron* const neuron, LEAF* const leaf)
     @brief Initialize a tNeuron to the default mempool of a LEAF instance.
     @param neuron A pointer to the tNeuron to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tNeuron_init                 (tMempool** const mempool)
     @brief Initialize a tNeuron to a specified mempool.
     @param neuron A pointer to the tNeuron to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tNeuron_free(tNeuron** const neuron)
     @brief Free a tNeuron from its mempool.
     @param neuron A pointer to the tNeuron to free.
     
     @fn void    tNeuron_reset       (tNeuron* const neuron)
     @brief Reset the neuron model.
     @param neuron A pointer to the relevant tNeuron.
     
     @fn float   tNeuron_tick        (tNeuron* const neuron)
     @brief Tick a tNeuron oscillator.
     @param neuron A pointer to the relevant tNeuron.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tNeuron_setMode     (tNeuron* const neuron, tNeuron** const neuron, NeuronMode mode)
     @brief Set the tNeuron shaping mode.
     @param neuron A pointer to the relevant tNeuron.
     @param mode The mode to set the tNeuron to.
     
     @fn void    tNeuron_setCurrent  (tNeuron* const neuron, float current)
     @brief Set the current.
     @param neuron A pointer to the relevant tNeuron.
     @param current The new current.
     
     @fn void    tNeuron_setK        (tNeuron* const neuron, float K)
     @brief Set the potassium value.
     @param neuron A pointer to the relevant tNeuron.
     @param K The new potassium.
     
     @fn void    tNeuron_setL        (tNeuron* const neuron, float L)
     @brief Set the chloride value.
     @param neuron A pointer to the relevant tNeuron.
     @param L The new chloride value.
     
     @fn void    tNeuron_setN        (tNeuron* const neuron, float N)
     @brief Set the sodium value.
     @param neuron A pointer to the relevant tNeuron.
     @param N The new sodium value.
     
     @fn void    tNeuron_setC        (tNeuron* const neuron, float C)
     @brief Set the calcium value.
     @param neuron A pointer to the relevant tNeuron.
     @param C The new calcium.
     
     @fn  void    tNeuron_setV1       (tNeuron* const neuron, float V1)
     @brief Set the V1 value.
     @param neuron A pointer to the relevant tNeuron.
     @param V1 The new V1.
     
     @fn void    tNeuron_setV2       (tNeuron* const neuron, float V2)
     @brief Set the V2 value.
     @param neuron A pointer to the relevant tNeuron.
     @param V2 The new V2.
     
     @fn void    tNeuron_setV3       (tNeuron* const neuron, float V3)
     @brief Set the V3 value.
     @param neuron A pointer to the relevant tNeuron.
     @param V3 The new V3.
     
     @fn void    tNeuron_setTimeStep (tNeuron* const neuron, float timestep)
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
        
        tPoleZero f;
        
        NeuronMode mode;
        
        float voltage, current;
        float timeStep;
        float invSampleRate;
        float alpha[3];
        float beta[3];
        float rate[3];
        float V[3];
        float P[3];
        float gK, gN, gL, C;
    } tNeuron;

    // Memory handlers for `tNeuron`
    void    tNeuron_create               (tMempool** const mempool, tNeuron** const);
    void    tNeuron_init                 (LEAF* const leaf, tNeuron* const neuron);
    void    tNeuron_free          (tNeuron** const neuron);

    // Tick function for `tNeuron`
    float   tNeuron_tick         (tNeuron* const neuron);

    // Setter functions for `tNeuron`
    void    tNeuron_reset         (tNeuron* const neuron);
    void    tNeuron_setMode       (tNeuron* const neuron, NeuronMode mode);
    void    tNeuron_setCurrent    (tNeuron* const neuron, float current);
    void    tNeuron_setK          (tNeuron* const neuron, float K);
    void    tNeuron_setL          (tNeuron* const neuron, float L);
    void    tNeuron_setN          (tNeuron* const neuron, float N);
    void    tNeuron_setC          (tNeuron* const neuron, float C);
    void    tNeuron_setV1         (tNeuron* const neuron, float V1);
    void    tNeuron_setV2         (tNeuron* const neuron, float V2);
    void    tNeuron_setV3         (tNeuron* const neuron, float V3);
    void    tNeuron_setTimeStep   (tNeuron* const neuron, float timestep);
    void    tNeuron_setSampleRate (tNeuron* const neuron, float sr);

    //==============================================================================
    
    
#define FILLEN 128 //was 256 in dekrispator code, but it seems like it just needs to be longer than dd step length (72) and probably a power of 2.
    // smaller buffer means refilling more often but a less intense load each time it refills

    
    /*!
     @defgroup tmbpulse tMBPulse
     @ingroup oscillators
     @brief Pulse wave oscillator with minBLEP anti-aliasing.
     @{
     
     @fn void tMBPulse_init(tMBPulse* const osc, LEAF* const leaf)
     @brief Initialize a tMBPulse to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBPulse to initialize.
     
     @fn void    tMBPulse_init                 (tMempool** const mempool)
     @brief Initialize a tMBPulse to a specified mempool.
     @param osc A pointer to the tMBPulse to initialize.
     
     @fn void tMBPulse_free(tMBPulse** const osc)
     @brief Free a tMBPulse from its mempool.
     @param osc A pointer to the tMBPulse to free.
     
     @fn float tMBPulse_tick(tMBPulse* const osc)
     @brief
     @param osc A pointer to the relevant tMBPulse.
     
     @fn void tMBPulse_setFreq(tMBPulse* const osc, tMBPulse** const osc, float f)
     @brief
     @param osc A pointer to the relevant tMBPulse.
     
     @fn void tMBPulse_setWidth(tMBPulse* const osc, float w)
     @brief
     @param osc A pointer to the relevant tMBPulse.
     
     @fn float tMBPulse_sync(tMBPulse* const osc, float sync)
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
        float    out;
        float    freq;
        float    waveform;    // duty cycle, must be in [-1, 1]
        float    lastsyncin;
        float    sync;
        float    syncdir;
        int      softsync;
        float   _p, _w, _b, _x, _z;
        float _inv_w;
        int     _j, _k;
        float   _f [8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t BLEPindices[64];
        float 	BLEPproperties[64][2];
        float invSampleRate;

    } tMBPulse;

    // Memory handlers for `tMBPulse`
    void    tMBPulse_create               (tMempool** const mempool, tMBPulse** const);
    void    tMBPulse_init                 (LEAF* const leaf, tMBPulse* const osc);
    void    tMBPulse_free                   (tMBPulse** const osc);
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBPulse_place_step_dd_noBuffer(tMBPulse* const osc, int index, float phase, float inv_w, float scale);
#else
    void    tMBPulse_place_step_dd_noBuffer (tMBPulse* const osc, int index, float phase, float inv_w, float scale);
#endif
    // Tick function for `tMBPulse`
    float  tMBPulse_tick                   (tMBPulse* const osc);

    // Setter functions for `tMBPulse`
    float  tMBPulse_sync                   (tMBPulse* const osc, float sync);
    void    tMBPulse_setFreq                (tMBPulse* const osc, float f);
    void    tMBPulse_setWidth               (tMBPulse* const osc, float w);
    void    tMBPulse_setPhase               (tMBPulse* const osc, float phase);
    void    tMBPulse_setSyncMode            (tMBPulse* const osc, int hardOrSoft);
    void    tMBPulse_setBufferOffset        (tMBPulse* const osc, uint32_t offset);
    void    tMBPulse_setSampleRate          (tMBPulse* const osc, float sr);
    
    /*!
     @defgroup tmbtriangle tMBTriangle
     @ingroup oscillators
     @brief Triangle wave oscillator with minBLEP anti-aliasing.
     @{
     
     @fn void tMBTriangle_init(tMBTriangle* const osc, LEAF* const leaf)
     @brief Initialize a tMBTriangle to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBTriangle to initialize.
     
     @fn void    tMBTriangle_init                 (tMempool** const mempool)
     @brief Initialize a tMBTriangle to a specified mempool.
     @param osc A pointer to the tMBTriangle to initialize.
     
     @fn void tMBTriangle_free(tMBTriangle** const osc)
     @brief Free a tMBTriangle from its mempool.
     @param osc A pointer to the tMBTriangle to free.
     
     @fn float tMBTriangle_tick(tMBTriangle* const osc)
     @brief
     @param osc A pointer to the relevant tMBTriangle.
     
     @fn void tMBTriangle_setFreq(tMBTriangle* const osc, tMBTriangle** const osc, float f)
     @brief
     @param osc A pointer to the relevant tMBTriangle.
     
     @fn void tMBTriangle_setWidth(tMBTriangle* const osc, float w)
     @brief
     @param osc A pointer to the relevant tMBTriangle.
     
     @fn float tMBTriangle_sync(tMBTriangle* const osc, float sync)
     @brief
     @param osc A pointer to the relevant tMBTriangle.
     
     @fn void tMBTriangle_setSyncMode(tMBTriangle* const osc, int hardOrSoft)
     @brief Set the sync behavior of the oscillator.
     @param hardOrSoft 0 for hard sync, 1 for soft sync

     @} */
    
    typedef struct tMBTriangle
    {

        tMempool* mempool;
        float    out;
        float    freq;
        float    waveform;    // duty cycle, must be in [-1, 1]
        float    lastsyncin;
        float    sync;
        float    syncdir;
        int      softsync;
        float   _p, _w, _b, _z, quarterwaveoffset;
        int     _j, _k;
        float _inv_w;
        float 	shape;
        float   _f [8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t maxBLEPphaseSlope;
        uint16_t BLEPindices[64];
        float 	BLEPproperties[64][3];
        float invSampleRate;
    } tMBTriangle;

    // Memory handlers for `tMBTriangle`
    void    tMBTriangle_create               (tMempool** const mempool, tMBTriangle** const);
    void    tMBTriangle_init                 (LEAF* const leaf, tMBTriangle* const osc);
    void    tMBTriangle_free              (tMBTriangle** const osc);
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBTriangle_place_dd_noBuffer(tMBTriangle* const osc, int index, float phase, float inv_w, float scale, float stepOrSlope, float w);
#else
    void    tMBTriangle_place_dd_noBuffer (tMBTriangle* const osc, int index, float phase, float inv_w, float scale,
                                           float stepOrSlope, float w);
#endif

    // Tick function for `tMBTriangle`
    float  tMBTriangle_tick              (tMBTriangle* const osc);

    // Setter functions for `tMBTriangle`
    float  tMBTriangle_sync              (tMBTriangle* const osc, float sync);
    void    tMBTriangle_setFreq           (tMBTriangle* const osc, float f);
    void    tMBTriangle_setWidth          (tMBTriangle* const osc, float w);
    void    tMBTriangle_setPhase          (tMBTriangle* const osc, float phase);
    void    tMBTriangle_setSyncMode       (tMBTriangle* const osc, int hardOrSoft);
    void    tMBTriangle_setBufferOffset   (tMBTriangle* const osc, uint32_t offset);
    void    tMBTriangle_setSampleRate     (tMBTriangle* const osc, float sr);
    
    



    typedef struct tMBSineTri
    {
        tMempool* mempool;
        float    out;
        float    freq;
        float    waveform;    // duty cycle, must be in [-1, 1]
        float    lastsyncin;
        float    sync;
        float    syncdir;
        int      softsync;
        float   _p, _w, _b, _z;
        float _sinPhase;
        float shape;
        int     _j, _k;
        float _inv_w;
        float   _f [8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t maxBLEPphaseSlope;
        uint16_t BLEPindices[64];
        float 	BLEPproperties[64][3];
        float invSampleRate;
        uint32_t sineMask;
    } tMBSineTri;

    // Memory handlers for `tMBSineTri`
    void    tMBSineTri_create               (tMempool** const mempool, tMBSineTri** const);
    void    tMBSineTri_init                 (LEAF* const leaf, tMBSineTri* const osc);
    void    tMBSineTri_free              (tMBSineTri** const osc);
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSineTri_place_dd_noBuffer(tMBSineTri* const osc, int index, float phase, float inv_w, float scale, float stepOrSlope, float w);
#else
    void    tMBSineTri_place_dd_noBuffer (tMBSineTri* const osc, int index, float phase, float inv_w, float scale,
                                          float stepOrSlope, float w);
#endif
    // Tick function for `tMBSineTri`
    float  tMBSineTri_tick              (tMBSineTri* const osc);

    // Setter functions for `tMBSineTri`
    float  tMBSineTri_sync              (tMBSineTri* const osc, float sync);
    void    tMBSineTri_setFreq           (tMBSineTri* const osc, float f);
    void    tMBSineTri_setWidth          (tMBSineTri* const osc, float w);
    void    tMBSineTri_setPhase          (tMBSineTri* const osc, float phase);
    void    tMBSineTri_setShape          (tMBSineTri* const osc, float shape);
    void    tMBSineTri_setSyncMode       (tMBSineTri* const osc, int hardOrSoft);
    void    tMBSineTri_setBufferOffset   (tMBSineTri* const osc, uint32_t offset);
    void    tMBSineTri_setSampleRate     (tMBSineTri* const osc, float sr);


    /*!
     @defgroup tmbsaw tMBSaw
     @ingroup oscillators
     @brief Saw wave oscillator with minBLEP anti-aliasing.
     @{
     
     @fn void tMBSaw_init(tMBSaw* const osc, LEAF* const leaf)
     @brief Initialize a tMBSaw to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBSaw to initialize.
     
     @fn void    tMBSaw_init                 (tMempool** const mempool)
     @brief Initialize a tMBSaw to a specified mempool.
     @param osc A pointer to the tMBSaw to initialize.
     
     @fn void tMBSaw_free(tMBSaw** const osc)
     @brief Free a tMBSaw from its mempool.
     @param osc A pointer to the tMBSaw to free.
     
     @fn float tMBSaw_tick(tMBSaw* const osc)
     @brief Tick the oscillator.
     @param osc A pointer to the relevant tMBSaw.
     @return The ticked sample.
     
     @fn void tMBSaw_setFreq(tMBSaw* const osc, tMBSaw** const osc, float f)
     @brief Set the frequency of the oscillator.
     @param osc A pointer to the relevant tMBSaw.
     @param freq The new frequency.
     
     @fn float tMBSaw_sync(tMBSaw* const osc, float sync)
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
        float    out;
        float    freq;
        float    lastsyncin;
        float    sync;
        float    syncdir;
        int      softsync;
        float   _p, _w, _z;
        float   _inv_w;
        int     _j;
        float   _f[8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t 	BLEPindices[64];
        float 	BLEPproperties[64][2];
        float invSampleRate;
    } tMBSaw;

    // Memory handlers for `tMBSaw`
    void    tMBSaw_create               (tMempool** const mempool, tMBSaw** const);
    void    tMBSaw_init                 (LEAF* const leaf, tMBSaw* const osc);
    void    tMBSaw_free                   (tMBSaw** const osc);

    void    tMBSaw_place_step_dd_noBuffer (tMBSaw* const osc, int index, float phase, float w, float scale);

    // Tick function for `tMBSaw`
    float  tMBSaw_tick                   (tMBSaw* const osc);

    // Setter functions for `tMBSaw`
    float  tMBSaw_sync                   (tMBSaw* const osc, float sync);
    void    tMBSaw_setFreq                (tMBSaw* const osc, float f);
    void    tMBSaw_setPhase               (tMBSaw* const osc, float phase);
    void    tMBSaw_setSyncMode            (tMBSaw* const osc, int hardOrSoft);
    void    tMBSaw_setBufferOffset        (tMBSaw* const osc, uint32_t offset);
    void    tMBSaw_setSampleRate          (tMBSaw* const osc, float sr);

    //==============================================================================
    /*!
     @defgroup tmbsaw tMBSawPulse
     @ingroup oscillators
     @brief Saw wave mixed with Pulse wave oscillator with minBLEP anti-aliasing.
     @{

     @fn void tMBSaw_init(tMBSaw* const osc, LEAF* const leaf)
     @brief Initialize a tMBSaw to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBSaw to initialize.

     @fn void    tMBSaw_init                 (tMempool** const mempool)
     @brief Initialize a tMBSaw to a specified mempool.
     @param osc A pointer to the tMBSaw to initialize.

     @fn void tMBSaw_free(tMBSaw** const osc)
     @brief Free a tMBSaw from its mempool.
     @param osc A pointer to the tMBSaw to free.

     @fn float tMBSaw_tick(tMBSaw* const osc)
     @brief Tick the oscillator.
     @param osc A pointer to the relevant tMBSaw.
     @return The ticked sample.

     @fn void tMBSaw_setFreq(tMBSaw* const osc, tMBSaw** const osc, float f)
     @brief Set the frequency of the oscillator.
     @param osc A pointer to the relevant tMBSaw.
     @param freq The new frequency.

     @fn float tMBSaw_sync(tMBSaw* const osc, float sync)
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
        float    out;
        float    freq;
        float    lastsyncin;
        float    sync;
        float    syncdir;
        int      softsync;
        float    waveform;
        float   _p, _w, _b, _x, _z, _k;
        int     _j;
        float _inv_w;
        float invSampleRate;
        float 	shape;
        float   _f [8];
        uint16_t numBLEPs;
        uint16_t mostRecentBLEP;
        uint16_t maxBLEPphase;
        uint16_t BLEPindices[64];
        float 	BLEPproperties[64][2];
        float gain;
        int active;

    } tMBSawPulse;

    // Memory handlers for `tMBSawPulse`
    void    tMBSawPulse_create               (tMempool** const mempool, tMBSawPulse** const);
    void    tMBSawPulse_init                 (LEAF* const leaf, tMBSawPulse* const osc);
    void    tMBSawPulse_free                   (tMBSawPulse** const osc);
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tMBSawPulse_place_step_dd_noBuffer(tMBSawPulse* const osc, int index, float phase, float inv_w, float scale);
#else
    void    tMBSawPulse_place_step_dd_noBuffer (tMBSawPulse* const osc, int index, float phase, float inv_w,
                                                float scale);
#endif
    // Tick function for `tMBSawPulse`
    float  tMBSawPulse_tick                   (tMBSawPulse* const osc);

    // Setter functions for `tMBSawPulse`
    float  tMBSawPulse_sync                   (tMBSawPulse* const osc, float sync);
    void    tMBSawPulse_setFreq                (tMBSawPulse* const osc, float f);
    void    tMBSawPulse_setPhase               (tMBSawPulse* const osc, float phase);
    void    tMBSawPulse_setShape               (tMBSawPulse* const osc, float shape);
    void    tMBSawPulse_setSyncMode            (tMBSawPulse* const osc, int hardOrSoft);
    void    tMBSawPulse_setBufferOffset        (tMBSawPulse* const osc, uint32_t offset);
    void    tMBSawPulse_setSampleRate          (tMBSawPulse* const osc, float sr);

    //==============================================================================
    /*!
     @defgroup ttable tTable
     @ingroup oscillators
     @brief Simple aliasing wavetable oscillator.
     @{
     
     @fn void    tTable_init(tTable* const osc, float* table, int size, LEAF* const leaf)
     @brief Initialize a tTable to the default mempool of a LEAF instance.
     @param osc A pointer to the tTable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wavetable.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTable_init                 (tMempool** const mempool)
     @brief Initialize a tTable to a specified mempool.
     @param osc A pointer to the tTable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wave table.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTable_free(tTable** const osc)
     @brief Free a tTable from its mempool.
     @param osc A pointer to the tTable to free.
     
     @fn float   tTable_tick         (tTable* const osc)
     @brief Tick a tTable oscillator.
     @param osc A pointer to the relevant tTable.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tTable_setFreq      (tTable* const osc, tTable** const osc, float* table, int size, float freq)
     @brief Set the frequency of a tTable oscillator.
     @param osc A pointer to the relevant tTable.
     @param freq The frequency to set the oscillator to.
     
     @} */
    
    typedef struct tTable
    {
        tMempool* mempool;
        
        float* waveTable;
        int size;
        float inc, freq;
        float phase;
        float invSampleRate;
    } tTable;

    // Memory handlers for `tTable`
    void    tTable_create               (tMempool** const mempool, tTable** const);
    void    tTable_init                 (LEAF* const leaf, tTable* const osc, float* table, int size);
    void    tTable_free          (tTable** const osc);

    // Tick function for `tTable`
    float  tTable_tick          (tTable* const osc);

    // Setter functions for `tTable`
    void    tTable_setFreq       (tTable* const osc, float freq);
    void    tTable_setSampleRate (tTable* const osc, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup twavetable tWaveTable
     @ingroup oscillators
     @brief Anti-aliased wavetable generator.
     @{
     
     @fn void    tWaveTable_init(tWaveTable* const osc, float* table, int size, float maxFreq, LEAF* const leaf)
     @brief Initialize a tWaveTable to the default mempool of a LEAF instance.
     @param osc A pointer to the tWaveTable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wavetable.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tWaveTable_init                 (tMempool** const mempool)
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
    
    typedef struct tWaveTable tWaveTable;

    // Memory handlers for `tWaveTable`
    void    tWaveTable_create               (tMempool** const, tWaveTable** const osc);
    void    tWaveTable_init                 (LEAF* const leaf, tWaveTable* const osc, float* table, int size, float maxFreq);
    void    tWaveTable_free          (tWaveTable** const osc);

    // Setter functions for `tWaveTable`
    void    tWaveTable_setSampleRate (LEAF* const leaf,tWaveTable* const osc, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup twaveosc tWaveOsc
     @ingroup oscillators
     @brief Set of anti-aliased wavetable oscillators that can be faded between.
     @{
     
     @fn void    tWaveOsc_init(tWaveOsc* const osc, const float** tables, int n, int size, float maxFreq, LEAF* const leaf)
     @brief Initialize a tWaveOsc to the default mempool of a LEAF instance.
     @param osc A pointer to the tWaveOsc to initialize.
     @param tables An array of pointers to wavetable data.
     @param n The number of wavetables.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tWaveOsc_init                 (tMempool** const mempool)
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
     
     @fn float   tWaveOsc_tick         (tWaveOsc* const osc)
     @brief Tick a tWaveOsc oscillator.
     @param osc A pointer to the relevant tWaveOsc.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tWaveOsc_setFreq      (tWaveOsc* const osc, tWaveOsc** const osc, const float** tables, int n, int size, float maxFreq, float freq)
     @brief Set the frequency of a tWaveOsc oscillator.
     @param osc A pointer to the relevant tWaveOsc.
     @param freq The frequency to set the oscillator to.
     
     @fn void    tWaveOsc_setIndex(tWaveOsc* const osc, float index)
     @brief Set the output index of the wavetable set.
     @param index The new index from 0.0 to 1.0 as a smooth fade from the first wavetable in the set to the last.
     
     @} */
    
    typedef struct tWaveOsc
       {
           tMempool* mempool;
           tWaveTable** tables;
           int numTables;
           float index;
           float maxFreq;
           int o1;
           int o2;
           float mix;
           uint32_t phase;
           uint32_t inc;
           float freq;
           float invSampleRateTimesTwoTo32;
           int oct;
           int size;

           // Determine base frequency
           float baseFreq;
           float invBaseFreq;
           float sampleRate;
           float w;
           float aa;
           int numSubTables;

       } tWaveOsc;

    // Memory handlers for `tWaveOsc`
    void    tWaveOsc_create               (tMempool** const mempool, tWaveOsc** const);
    void    tWaveOsc_init                 (LEAF* const leaf, tWaveOsc* const cy, tWaveTable** tables, int numTables);
    void    tWaveOsc_free            (tWaveOsc** const osc);

    // Tick function for `tWaveOsc`
    float  tWaveOsc_tick            (tWaveOsc* const osc);

    // Setter functions for `tWaveOsc`
    void 	tWaveOsc_setFreq         (tWaveOsc* const cy, float freq);
    void    tWaveOsc_setAntiAliasing (tWaveOsc* const osc, float aa);
    void    tWaveOsc_setIndex        (tWaveOsc* const osc, float index);
    void 	tWaveOsc_setTables       (tWaveOsc* const cy, tWaveTable* tables, int numTables);
    void    tWaveOsc_setSampleRate   (tWaveOsc* const osc, float sr);

    //==============================================================================
    
    /*!
     @defgroup ttwavetables tWaveTableS
     @ingroup oscillators
     @brief A more space-efficient anti-aliased wavetable generator than tWaveTable but with slightly worse fidelity.
     @{
     
     @fn void    tWaveTableS_init(tWaveTableS* const osc, float* table, int size, float maxFreq, LEAF* const leaf)
     @brief Initialize a tWaveTableS to the default mempool of a LEAF instance.
     @param osc A pointer to the tWaveTableS to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wavetable.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tWaveTableS_init                 (tMempool** const mempool)
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
    
    typedef struct tWaveTableS tWaveTableS;

    // Memory handlers for `tWaveTableS`
    void    tWaveTableS_create         (tMempool** const,tWaveTableS** const osc);
    void    tWaveTableS_init                (LEAF* const leaf, tWaveTableS* const osc, float* table, int size, float maxFreq);
    void    tWaveTableS_free          (tWaveTableS** const osc);

    // Setter functions for `tWaveTableS`
    void    tWaveTableS_setSampleRate (tWaveTableS* const osc, float sr);
    

    //==============================================================================
    /*!
     @defgroup twaveoscs tWaveOscS
     @ingroup oscillators
     @brief Set of anti-aliased wavetable oscillators that can be faded between.
     @{
     
     @fn void    tWaveOscS_init(tWaveOsc* const osc, const float** tables, int n, int size, float maxFreq, LEAF* const leaf)
     @brief Initialize a tWaveOscS to the default mempool of a LEAF instance.
     @param osc A pointer to the tWaveOscS to initialize.
     @param tables An array of pointers to wavetable data.
     @param n The number of wavetables.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tWaveOscS_init                 (tMempool** const mempool)
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
     
     @fn float   tWaveOscS_tick         (tWaveOsc* const osc)
     @brief Tick a tWaveOscS oscillator.
     @param osc A pointer to the relevant tWaveOscS.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tWaveOscS_setFreq      (tWaveOsc* const osc, tWaveOsc** const osc, const float** tables, int n, int size, float maxFreq, float freq)
     @brief Set the frequency of a tWaveOscS oscillator.
     @param osc A pointer to the relevant tWaveOscS.
     @param freq The frequency to set the oscillator to.
     
     @fn void    tWaveOscS_setIndex(tWaveOsc* const osc, float index)
     @brief Set the output index of the wavetable set.
     @param index The new index from 0.0 to 1.0 as a smooth fade from the first wavetable in the set to the last.
     
     @} */
    
    typedef struct tWaveOscS
    {
        tMempool* mempool;

        //tWaveTableS* tables;
        //this is passed in
        tWaveTableS** tables;

        int numTables;
        float index;
        float maxFreq;
        int o1;
        int o2;
        float mix;
        uint32_t phase;
        uint32_t inc;
        float freq;
        float invSampleRateTimesTwoTo32;
        int oct;
        int size;

        // Determine base frequency
        float baseFreq;
        float invBaseFreq;
        float sampleRate;
        float w;
        float aa;
        int numSubTables;

    } tWaveOscS;

    // Memory handlers for `tWaveOscS`
    void    tWaveOscS_create               (tMempool** const mempool, tWaveOscS** const);
    void    tWaveOscS_init                 (LEAF* const leaf, tWaveOscS* const osc, tWaveTableS** tables, int numTables);
    void    tWaveOscS_free            (tWaveOscS** const osc);

    // Tick function for `tWaveOscS`
    float  tWaveOscS_tick            (tWaveOscS* const osc);

    // Setter functions for `tWaveOscS`
    void    tWaveOscS_setFreq         (tWaveOscS* const osc, float freq);
    void    tWaveOscS_setAntiAliasing (tWaveOscS* const osc, float aa);
    void    tWaveOscS_setIndex        (tWaveOscS* const osc, float index);
    void    tWaveOscS_setSampleRate   (tWaveOscS* const osc, float sr);
    



     //==============================================================================
    
    /*!
     @defgroup tIntphasor tIntPhasor
     @ingroup oscillators
     @brief Aliasing phasor.
     @{
     
     @fn void    tIntPhasor_init(tIntPhasor* const osc, LEAF* const leaf)
     @brief Initialize a tIntPhasor to the default mempool of a LEAF instance.
     @param osc A pointer to the tIntPhasor to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tIntPhasor_init                 (tMempool** const)
     @brief Initialize a tIntPhasor to a specified mempool.
     @param osc A pointer to the tIntPhasor to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tIntPhasor_free(tIntPhasor** const osc)
     @brief Free a tIntPhasor from its mempool.
     @param osc A pointer to the tIntPhasor to free.
     
     @fn float   tIntPhasor_tick        (tIntPhasor* const osc)
     @brief
     @param osc A pointer to the relevant tIntPhasor.
     
     @fn void    tIntPhasor_setFreq     (tIntPhasor* const osc, tIntPhasor** const osc, float freq)
     @brief
     @param osc A pointer to the relevant tIntPhasor.
     ￼￼￼
     @} */
    
    typedef struct tIntPhasor
    {

        tMempool* mempool;
        uint32_t phase;
        uint32_t inc;
        float freq;
        int32_t mask;
        uint8_t phaseDidReset;
        float invSampleRateTimesTwoTo32;
    } tIntPhasor;

    // Memory handlers for `tIntPhasor`
    void    tIntPhasor_create               (tMempool** const mempool, tIntPhasor** const);
    void    tIntPhasor_init                 (LEAF* const leaf, tIntPhasor* const osc);
    void    tIntPhasor_free          (tIntPhasor** const osc);

    // Tick function for `tIntPhasor`
    float  tIntPhasor_tick          (tIntPhasor* const osc);
    float  tIntPhasor_tickBiPolar   (tIntPhasor* const osc);
    // Setter functions for `tIntPhasor`
    void    tIntPhasor_setFreq       (tIntPhasor* const osc, float freq);
    void    tIntPhasor_setSampleRate (tIntPhasor* const osc, float sr);
    void    tIntPhasor_setPhase      (tIntPhasor* const cy, float phase);
    
         //==============================================================================
    
    /*!
     @defgroup tSquareLFO tSquareLFO
     @ingroup oscillators
     @brief Aliasing phasor.
     @{
     
     @fn void    tSquareLFO_init(tSquareLFO* const osc, LEAF* const leaf)
     @brief Initialize a tSquareLFO to the default mempool of a LEAF instance.
     @param osc A pointer to the tSquareLFO to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSquareLFO_init                 (tMempool** const)
     @brief Initialize a tSquareLFO to a specified mempool.
     @param osc A pointer to the tSquareLFO to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSquareLFO_free(tSquareLFO** const osc)
     @brief Free a tSquareLFO from its mempool.
     @param osc A pointer to the tSquareLFO to free.
     
     @fn float   tSquareLFO_tick        (tSquareLFO* const osc)
     @brief
     @param osc A pointer to the relevant tSquareLFO.
     
     @fn void    tSquareLFO_setFreq     (tSquareLFO* const osc, tSquareLFO** const osc, float freq)
     @brief
     @param osc A pointer to the relevant tSquareLFO.
     ￼￼￼
     @} */
    
    typedef struct tSquareLFO
    {

        tMempool* mempool;
        float pulsewidth;
        tIntPhasor phasor;
        tIntPhasor invPhasor;
    } tSquareLFO;

    // Memory handlers for `tSquareLFO`
    void    tSquareLFO_create               (tMempool** const mempool, tSquareLFO** const);
    void    tSquareLFO_init                 (LEAF* const leaf, tSquareLFO* const osc);
    void    tSquareLFO_free          (tSquareLFO** const osc);

    // Tick function for `tSquareLFO`
    float  tSquareLFO_tick          (tSquareLFO* const osc);

    // Setter functions for `tSquareLFO`
    void    tSquareLFO_setFreq       (tSquareLFO* const osc, float freq);
    void    tSquareLFO_setSampleRate (tSquareLFO* const osc, float sr);
    void    tSquareLFO_setPulseWidth (tSquareLFO* const cy, float pw);
    void    tSquareLFO_setPhase      (tSquareLFO* const cy, float phase);

    typedef struct tSawSquareLFO
    {
        tMempool* mempool;
        float shape;
        tIntPhasor  saw;
        tSquareLFO  square;
    } tSawSquareLFO;

    // Memory handlers for `tSawSquareLFO`
    void    tSawSquareLFO_create               (tMempool** const mempool, tSawSquareLFO** const);
    void    tSawSquareLFO_init                 (LEAF* const leaf, tSawSquareLFO* const osc);
    void    tSawSquareLFO_free          (tSawSquareLFO** const osc);

    // Tick function for `tSawSquareLFO`
    float  tSawSquareLFO_tick          (tSawSquareLFO* const osc);

    // Setter functions for `tSawSquareLFO`
    void    tSawSquareLFO_setFreq       (tSawSquareLFO* const osc, float freq);
    void    tSawSquareLFO_setSampleRate (tSawSquareLFO* const osc, float sr);
    void    tSawSquareLFO_setPhase      (tSawSquareLFO* const cy, float phase);
    void    tSawSquareLFO_setShape      (tSawSquareLFO* const cy, float shape);

        //==============================================================================
 /*!
     @defgroup tTriLFO tTriLFO
     @ingroup oscillators
     @brief Aliasing phasor.
     @{
     
     @fn void    tTriLFO_init(tTriLFO* const osc, LEAF* const leaf)
     @brief Initialize a tTriLFO to the default mempool of a LEAF instance.
     @param osc A pointer to the tTriLFO to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTriLFO_init                 (tMempool** const)
     @brief Initialize a tTriLFO to a specified mempool.
     @param osc A pointer to the tTriLFO to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTriLFO_free(tTriLFO** const osc)
     @brief Free a tTriLFO from its mempool.
     @param osc A pointer to the tTriLFO to free.
     
     @fn float   tTriLFO_tick        (tTriLFO* const osc)
     @brief
     @param osc A pointer to the relevant tTriLFO.
     
     @fn void    tTriLFO_setFreq     (tTriLFO* const osc, tTriLFO** const osc, float freq)
     @brief
     @param osc A pointer to the relevant tTriLFO.
     ￼￼￼
     @} */
    
    typedef struct tTriLFO
    {

        tMempool* mempool;
        int32_t phase;
        int32_t inc;
        float freq;
        float invSampleRate;
        float invSampleRateTimesTwoTo32;
    } tTriLFO;

    // Memory handlers for `tTriLFO`
    void    tTriLFO_create               (tMempool** const mempool, tTriLFO** const);
    void    tTriLFO_init                 (LEAF* const leaf, tTriLFO* const osc);
    void    tTriLFO_free          (tTriLFO** const osc);

    // Tick function for `tTriLFO`
    float  tTriLFO_tick          (tTriLFO* const osc);

    // Setter functions for `tTriLFO`
    void    tTriLFO_setFreq       (tTriLFO* const osc, float freq);
    void    tTriLFO_setSampleRate (tTriLFO* const osc, float sr);
    void    tTriLFO_setPhase      (tTriLFO* const cy, float phase);

    typedef struct tSineTriLFO
    {
        tMempool* mempool;
        float shape;
        tTriLFO  tri;
        tCycle  sine;
    } tSineTriLFO;

    // Memory handlers for `tSineTriLFO`
    void    tSineTriLFO_create               (tMempool** const mempool, tSineTriLFO** const);
    void    tSineTriLFO_init                 (LEAF* const leaf, tSineTriLFO* const osc);
    void    tSineTriLFO_free          (tSineTriLFO** const osc);

    // Tick function for `tSineTriLFO`
    float  tSineTriLFO_tick          (tSineTriLFO* const osc);

    // Setter functions for `tSineTriLFO`
    void    tSineTriLFO_setFreq       (tSineTriLFO* const osc, float freq);
    void    tSineTriLFO_setSampleRate (tSineTriLFO* const osc, float sr);
    void    tSineTriLFO_setPhase      (tSineTriLFO* const cy, float phase);
    void    tSineTriLFO_setShape      (tSineTriLFO* const cy, float shape);



typedef struct tDampedOscillator
	{
		tMempool* mempool;

		float freq_;
		float decay_;
		float two_pi_by_sample_rate_;
		float loop_gain_;
		float turns_ratio_;
		float x_;
		float y_;
	} tDampedOscillator;

    // Memory handlers for `tDampedOscillator`
	void    tDampedOscillator_create               (tMempool** const mempool, tDampedOscillator** const);
	void    tDampedOscillator_init                 (LEAF* const leaf, tDampedOscillator* const osc);
	void    tDampedOscillator_free          (tDampedOscillator** const osc);

    // Tick function for `tDampedOscillator`
	float  tDampedOscillator_tick          (tDampedOscillator* const osc);

    // Setter functions for `tDampedOscillator`
	void    tDampedOscillator_setFreq       (tDampedOscillator* const osc, float freq);
	void    tDampedOscillator_setSampleRate (tDampedOscillator* const osc, float sr);
	void 	tDampedOscillator_setDecay      (tDampedOscillator* const osc, float decay);
	void 	tDampedOscillator_reset         (tDampedOscillator* const osc);


    typedef struct tPlutaQuadOsc
    {
        tMempool* mempool;
        uint32_t oversamplingRatio;
        uint32_t phase[4];
        float biPolarOutputs[4];
        uint32_t inc[4];
        float freq[4];
        float fmMatrix[4][4];
        float outputAmplitudes[4];
        tButterworth*  lowpass;
        int32_t mask;
        float invSampleRateTimesTwoTo32;
    } tPlutaQuadOsc;

    // Memory handlers for `tDampedOscillator`
    void    tPlutaQuadOsc_create               (tMempool** const mempool, tPlutaQuadOsc** const);
    void    tPlutaQuadOsc_init                 (LEAF* const leaf, tPlutaQuadOsc* const cy, uint32_t oversamplingRatio);
    void    tPlutaQuadOsc_free          (tPlutaQuadOsc** const osc);

    // Tick function for `tDampedOscillator`
    float  tPlutaQuadOsc_tick          (tPlutaQuadOsc* const osc);
    void   tPlutaQuadOsc_setFreq        (tPlutaQuadOsc* const c, uint32_t whichOsc, float freq);
    void   tPlutaQuadOsc_setFmAmount        (tPlutaQuadOsc* const c, uint32_t const whichCarrier, uint32_t const whichModulator, float const amount);
    void   tPlutaQuadOsc_setOutputAmplitude        (tPlutaQuadOsc* const c, uint32_t const whichOsc, float const amplitude);
#ifdef __cplusplus
}
#endif
#endif  // LEAF_OSCILLATORS_H_INCLUDED

//==============================================================================
