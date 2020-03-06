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
    
    /*!
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    /*!
     * @defgroup oscillators Oscillators
     */
    
    //==============================================================================
    
    /*!
     * @ingroup oscillators
     * @defgroup tcycle tCycle
     * A cycle/sine waveform oscillator. Uses wavetable synthesis.
     * @{
     */
    
    typedef struct _tCycle
    {
        // Underlying phasor
        float phase;
        float inc,freq;
        
    } _tCycle;
    
    typedef _tCycle* tCycle;
    
    //! Initializes a tCycle to the default LEAF mempool.
    /*!
     @param osc A pointer to the tCycle to be initialized.
     */
    void    tCycle_init         (tCycle* const osc);
    
    
    //! Frees a tCycle from the default LEAF mempool.
    /*!
     @param osc A pointer to the tCycle to be freed.
     */
    void    tCycle_free         (tCycle* const osc);
    
    
    //! Initializes a tCycle to a specified mempool.
    /*!
     @param osc A pointer to the tCycle to be initialized.
     @param pool A pointer to the tMempool to which the tCycle should be initialized.
     */
    void    tCycle_initToPool   (tCycle* const osc, tMempool* const pool);
    
    
    //! Frees a tCycle from a specified mempool.
    /*!
     @param osc A pointer to the tCycle to be freed.
     @param pool A pointer to the tMempool from which the tCycle should be freed.
     */
    void    tCycle_freeFromPool (tCycle* const osc, tMempool* const pool);
    
    
    //! Tick a tCycle oscillator.
    /*!
     @param osc A pointer to the relevant tCycle.
     @return The ticked sample.
     */
    float   tCycle_tick         (tCycle* const osc);
    
    
    //! Set the frequency of a tCycle oscillator.
    /*!
     @param osc A pointer to the relevant tCycle.
     @param freq The frequency to set the oscillator to.
     */
    void    tCycle_setFreq      (tCycle* const osc, float freq);
    
    /*! @} */
    
    //==============================================================================
    
    /*!
     * @ingroup oscillators
     * @defgroup ttriangle tTriangle
     * An anti-aliased triangle waveform oscillator. Uses wavetable synthesis.
     * @{
     */
    
    /* tTriangle: Anti-aliased Triangle waveform using wavetable interpolation. Wavetables constructed from sine components. */
    typedef struct _tTriangle
    {
        // Underlying phasor
        float phase;
        float inc,freq;
        
    } _tTriangle;
    
    typedef _tTriangle* tTriangle;
    
    //! Initializes a tTriangle to the default LEAF mempool.
    /*!
     @param osc A pointer to the tTriangle to be initialized.
     */
    void    tTriangle_init          (tTriangle* const osc);
    
    
    //! Frees a tTriangle from the default LEAF mempool.
    /*!
     @param osc A pointer to the tTriangle to be freed.
     */
    void    tTriangle_free          (tTriangle* const osc);
    
    
    //! Initializes a tTriangle to a specified mempool.
    /*!
     @param osc A pointer to the tTriangle to be initialized.
     @param pool A pointer to the tMempool to which the tTriangle should be initialized.
     */
    void    tTriangle_initToPool    (tTriangle* const osc, tMempool* const pool);
    
    
    //! Frees a tTriangle from a specified mempool.
    /*!
     @param osc A pointer to the tTriangle to be freed.
     @param pool A pointer to the tMempool from which the tTriangle should be freed.
     */
    void    tTriangle_freeFromPool  (tTriangle* const osc, tMempool* const pool);
    
    
    //! Tick a tTriangle oscillator.
    /*!
     @param osc A pointer to the relevant tTriangle.
     @return The ticked sample.
     */
    float   tTriangle_tick          (tTriangle* const osc);
    
    
    //! Set the frequency of a tTriangle oscillator.
    /*!
     @param osc A pointer to the relevant tTriangle.
     @param freq The frequency to set the oscillator to.
     */
    int     tTriangle_setFreq       (tTriangle* const osc, float freq);
    
    /*! @} */
    
    //==============================================================================
    
    /*!
     * @ingroup oscillators
     * @defgroup tsquare tSquare
     * An anti-aliased square waveform oscillator. Uses wavetable synthesis.
     * @{
     */
    
    /* tSquare: Anti-aliased Square waveform using wavetable interpolation. Wavetables constructed from sine components. */
    typedef struct _tSquare
    {
        // Underlying phasor
        float phase;
        float inc,freq;
        
    } _tSquare;
    
    typedef _tSquare* tSquare;
    
    //! Initializes a tSquare to the default LEAF mempool.
    /*!
     @param osc A pointer to the tSquare to be initialized.
     */
    void    tSquare_init        (tSquare* const osc);
    
    
    //! Frees a tSquare from the default LEAF mempool.
    /*!
     @param osc A pointer to the tSquare to be freed.
     */
    void    tSquare_free        (tSquare* const osc);
    
    
    //! Initializes a tSquare to a specified mempool.
    /*!
     @param osc A pointer to the tSquare to be initialized.
     @param pool A pointer to the tMempool to which the tSquare should be initialized.
     */
    void    tSquare_initToPool  (tSquare* const osc, tMempool* const);
    
    
    //! Frees a tSquare from a specified mempool.
    /*!
     @param osc A pointer to the tSquare to be freed.
     @param pool A pointer to the tMempool from which the tSquare should be freed.
     */
    void    tSquare_freeFromPool(tSquare* const osc, tMempool* const);
    
    
    //! Tick a tSquare oscillator.
    /*!
     @param osc A pointer to the relevant tSquare.
     @return The ticked sample.
     */
    float   tSquare_tick        (tSquare* const osc);

    
    //! Set the frequency of a tSquare oscillator.
    /*!
     @param osc A pointer to the relevant tSquare.
     @param freq The frequency to set the oscillator to.
     */
    int     tSquare_setFreq     (tSquare* const osc, float freq);
    
    //==============================================================================
    
    /* tSawtooth: Anti-aliased Sawtooth waveform using wavetable interpolation. Wavetables constructed from sine components. */
    typedef struct _tSawtooth
    {
        // Underlying phasor
        float phase;
        float inc,freq;
        
    } _tSawtooth;
    
    typedef _tSawtooth* tSawtooth;
    
    void    tSawtooth_init          (tSawtooth* const osc);
    void    tSawtooth_free          (tSawtooth* const osc);
    void    tSawtooth_initToPool    (tSawtooth* const osc, tMempool* const);
    void    tSawtooth_freeFromPool  (tSawtooth* const osc, tMempool* const);
    
    float   tSawtooth_tick          (tSawtooth* const osc);
    int     tSawtooth_setFreq       (tSawtooth* const osc, float freq);
    
    //==============================================================================
    
    /* tPhasor: Aliasing phasor [0.0, 1.0) */
    typedef struct _tPhasor
    {
        float phase;
        float inc,freq;
        
    } _tPhasor;
    
    typedef _tPhasor* tPhasor;
    
    void    tPhasor_init        (tPhasor* const osc);
    void    tPhasor_free        (tPhasor* const osc);
    void    tPhasor_initToPool  (tPhasor* const osc, tMempool* const);
    void    tPhasor_freeFromPool(tPhasor* const osc, tMempool* const);
    
    float   tPhasor_tick        (tPhasor* const osc);
    int     tPhasor_setFreq     (tPhasor* const osc, float freq);
    
    //==============================================================================
    
    /* tNoise. WhiteNoise, PinkNoise. */
    typedef enum NoiseType
    {
        WhiteNoise=0,
        PinkNoise,
        NoiseTypeNil,
    } NoiseType;
    
    typedef struct _tNoise
    {
        NoiseType type;
        float pinkb0, pinkb1, pinkb2;
        float(*rand)(void);
        
    } _tNoise;
    
    typedef _tNoise* tNoise;
    
    void    tNoise_init         (tNoise* const noise, NoiseType type);
    void    tNoise_free         (tNoise* const noise);
    void    tNoise_initToPool   (tNoise* const noise, NoiseType type, tMempool* const);
    void    tNoise_freeFromPool (tNoise* const noise, tMempool* const);
    
    float   tNoise_tick         (tNoise* const noise);
    
    //==============================================================================
    
    /* tNeuron */
    typedef enum NeuronMode
    {
        NeuronNormal = 0,
        NeuronTanh,
        NeuronAaltoShaper,
        NeuronModeNil
    } NeuronMode;
    
    typedef struct _tNeuron
    {
        tPoleZero f;
        
        NeuronMode mode;
        
        float voltage, current;
        float timeStep;
        
        float alpha[3];
        float beta[3];
        float rate[3];
        float V[3];
        float P[3];
        float gK, gN, gL, C;
    } _tNeuron;
    
    typedef _tNeuron* tNeuron;
    
    void    tNeuron_init        (tNeuron* const neuron);
    void    tNeuron_free        (tNeuron* const neuron);
    void    tNeuron_initToPool  (tNeuron* const neuron, tMempool* const);
    void    tNeuron_freeFromPool(tNeuron* const neuron, tMempool* const);
    
    void    tNeuron_reset       (tNeuron* const neuron);
    float   tNeuron_tick        (tNeuron* const neuron);
    void    tNeuron_setMode     (tNeuron* const neuron, NeuronMode mode);
    void    tNeuron_setCurrent  (tNeuron* const neuron, float current);
    void    tNeuron_setK        (tNeuron* const neuron, float K);
    void    tNeuron_setL        (tNeuron* const neuron, float L);
    void    tNeuron_setN        (tNeuron* const neuron, float N);
    void    tNeuron_setC        (tNeuron* const neuron, float C);
    void    tNeuron_setV1       (tNeuron* const neuron, float V1);
    void    tNeuron_setV2       (tNeuron* const neuron, float V2);
    void    tNeuron_setV3       (tNeuron* const neuron, float V3);
    void    tNeuron_setTimeStep (tNeuron* const neuron, float timestep);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_OSCILLATORS_H_INCLUDED

//==============================================================================

