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

     @fn void    tCycle_init         (tCycle* const osc, LEAF* const leaf)
     @brief Initialize a tCycle to the default mempool of a LEAF instance.
     @param osc A pointer to the tCycle to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tCycle_initToPool   (tCycle* const osc, tMempool* const mempool)
     @brief Initialize a tCycle to a specified mempool.
     @param osc A pointer to the tCycle to initialize.
     @param mempool A pointer to the tMempool to use.

     @fn void    tCycle_free         (tCycle* const osc)
     @brief Free a tCycle from its mempool.
     @param osc A pointer to the tCycle to free.
     
     @fn float   tCycle_tick         (tCycle* const osc)
     @brief Tick a tCycle oscillator.
     @param osc A pointer to the relevant tCycle.
     @return The ticked sample as a float from -1 to 1.

     @fn void    tCycle_setFreq      (tCycle* const osc, float freq)
     @brief Set the frequency of a tCycle oscillator.
     @param osc A pointer to the relevant tCycle.
     @param freq The frequency to set the oscillator to.
     
    ￼￼￼
     @} */
    
    typedef struct _tCycle
    {
        
        tMempool mempool;
        // Underlying phasor
        float phase;
        float inc,freq;
    } _tCycle;
    
    typedef _tCycle* tCycle;
    
    void    tCycle_init         (tCycle* const osc, LEAF* const leaf);
    void    tCycle_initToPool   (tCycle* const osc, tMempool* const mempool);
    void    tCycle_free         (tCycle* const osc);
    
    float   tCycle_tick         (tCycle* const osc);
    void    tCycle_setFreq      (tCycle* const osc, float freq);
    
    //==============================================================================
    
    /*!
     @defgroup ttriangle tTriangle
     @ingroup oscillators
     @brief Anti-aliased wavetable triangle wave oscillator.
     @{
     
     @fn void    tTriangle_init         (tTriangle* const osc, LEAF* const leaf)
     @brief Initialize a tTriangle to the default mempool of a LEAF instance.
     @param osc A pointer to the tTriangle to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTriangle_initToPool   (tTriangle* const osc, tMempool* const mempool)
     @brief Initialize a tTriangle to a specified mempool.
     @param osc A pointer to the tTriangle to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTriangle_free         (tTriangle* const osc)
     @brief Free a tTriangle from its mempool.
     @param osc A pointer to the tTriangle to free.
     
     @fn float   tTriangle_tick         (tTriangle* const osc)
     @brief Tick a tTriangle oscillator.
     @param osc A pointer to the relevant tTriangle.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tTriangle_setFreq      (tTriangle* const osc, float freq)
     @brief Set the frequency of a tTriangle oscillator.
     @param osc A pointer to the relevant tTriangle.
     @param freq The frequency to set the oscillator to.
     
     @} */

    typedef struct _tTriangle
    {
        
        tMempool mempool;
        // Underlying phasor
        float phase;
        float inc,freq;
        int oct;
        float w;
    } _tTriangle;
    
    typedef _tTriangle* tTriangle;

    void    tTriangle_init          (tTriangle* const osc, LEAF* const leaf);
    void    tTriangle_initToPool    (tTriangle* const osc, tMempool* const mempool);
    void    tTriangle_free          (tTriangle* const osc);
    
    float   tTriangle_tick          (tTriangle* const osc);
    void    tTriangle_setFreq       (tTriangle* const osc, float freq);

    
    //==============================================================================
    
    /*!
     @defgroup tsquare tSquare
     @ingroup oscillators
     @brief Anti-aliased wavetable square wave oscillator.
     @{
     
     @fn void    tSquare_init         (tSquare* const osc, LEAF* const leaf)
     @brief Initialize a tSquare to the default mempool of a LEAF instance.
     @param osc A pointer to the tSquare to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSquare_initToPool   (tSquare* const osc, tMempool* const mempool)
     @brief Initialize a tSquare to a specified mempool.
     @param osc A pointer to the tSquare to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSquare_free         (tSquare* const osc)
     @brief Free a tSquare from its mempool.
     @param osc A pointer to the tSquare to free.
     
     @fn float   tSquare_tick         (tSquare* const osc)
     @brief Tick a tSquare oscillator.
     @param osc A pointer to the relevant tSquare.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tSquare_setFreq      (tSquare* const osc, float freq)
     @brief Set the frequency of a tSquare oscillator.
     @param osc A pointer to the relevant tSquare.
     @param freq The frequency to set the oscillator to.
     ￼￼￼
     @} */
    
    typedef struct _tSquare
    {
        
        tMempool mempool;
        // Underlying phasor
        float phase;
        float inc,freq;
        int oct;
        float w;
    } _tSquare;
    
    typedef _tSquare* tSquare;

    void    tSquare_init        (tSquare* const osc, LEAF* const leaf);
    void    tSquare_initToPool  (tSquare* const osc, tMempool* const);
    void    tSquare_free        (tSquare* const osc);

    float   tSquare_tick        (tSquare* const osc);
    void    tSquare_setFreq     (tSquare* const osc, float freq);
    
    /*!￼￼￼
     @} */
    
    //==============================================================================
    
    /*!
     @defgroup tsawtooth tSawtooth
     @ingroup oscillators
     @brief Anti-aliased wavetable saw wave oscillator.
     @{
     
     @fn void    tSawtooth_init         (tSawtooth* const osc, LEAF* const leaf)
     @brief Initialize a tSawtooth to the default mempool of a LEAF instance.
     @param osc A pointer to the tSawtooth to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSawtooth_initToPool   (tSawtooth* const osc, tMempool* const mempool)
     @brief Initialize a tSawtooth to a specified mempool.
     @param osc A pointer to the tSawtooth to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSawtooth_free         (tSawtooth* const osc)
     @brief Free a tSawtooth from its mempool.
     @param osc A pointer to the tSawtooth to free.
     
     @fn float   tSawtooth_tick         (tSawtooth* const osc)
     @brief Tick a tSawtooth oscillator.
     @param osc A pointer to the relevant tSawtooth.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tSawtooth_setFreq      (tSawtooth* const osc, float freq)
     @brief Set the frequency of a tSawtooth oscillator.
     @param osc A pointer to the relevant tSawtooth.
     @param freq The frequency to set the oscillator to.
     ￼￼￼
     @} */
    
    typedef struct _tSawtooth
    {
        
        tMempool mempool;
        // Underlying phasor
        float phase;
        float inc,freq;
        int oct;
        float w;
    } _tSawtooth;
    
    typedef _tSawtooth* tSawtooth;

    void    tSawtooth_init          (tSawtooth* const osc, LEAF* const leaf);
    void    tSawtooth_initToPool    (tSawtooth* const osc, tMempool* const mempool);
    void    tSawtooth_free          (tSawtooth* const osc);

    float   tSawtooth_tick          (tSawtooth* const osc);
    void    tSawtooth_setFreq       (tSawtooth* const osc, float freq);
    
    //==============================================================================
    
    /*!
     @defgroup tpbtriangle tPBTriangle
     @ingroup oscillators
     @brief Triangle wave oscillator with polyBLEP anti-aliasing.
     @{
     
     @fn void   tPBTriangle_init          (tPBTriangle* const osc, LEAF* const leaf)
     @brief Initialize a tPBTriangle to the default mempool of a LEAF instance.
     @param osc A pointer to the tPBTriangle to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPBTriangle_initToPool    (tPBTriangle* const osc, tMempool* const mempool)
     @brief Initialize a tPBTriangle to a specified mempool.
     @param osc A pointer to the tPBTriangle to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPBTriangle_free          (tPBTriangle* const osc)
     @brief Free a tTri from its mempool.
     @param osc A pointer to the tPBTriangle to free.
     
     @fn float   tPBTriangle_tick          (tPBTriangle* const osc)
     @brief
     @param osc A pointer to the relevant tPBTriangle.
     
     @fn void    tPBTriangle_setFreq       (tPBTriangle* const osc, float freq)
     @brief
     @param osc A pointer to the relevant tPBTriangle.
     
     @fn void    tPBTriangle_setSkew       (tPBTriangle* const osc, float skew)
     @brief
     @param osc A pointer to the relevant tPBTriangle.
     ￼￼￼
     @} */

    typedef struct _tPBTriangle
    {
        tMempool mempool;
        float phase;
        float inc,freq;
        float skew;
        float lastOut;
    } _tPBTriangle;
    
    typedef _tPBTriangle* tPBTriangle;
    
    void    tPBTriangle_init          (tPBTriangle* const osc, LEAF* const leaf);
    void    tPBTriangle_initToPool    (tPBTriangle* const osc, tMempool* const mempool);
    void    tPBTriangle_free          (tPBTriangle* const osc);
    
    float   tPBTriangle_tick          (tPBTriangle* const osc);
    void    tPBTriangle_setFreq       (tPBTriangle* const osc, float freq);
    void    tPBTriangle_setSkew       (tPBTriangle* const osc, float skew);
    
    //==============================================================================
    
    /*!
     @defgroup tpbpulse tPBPulse
     @ingroup oscillators
     @brief Pulse wave oscillator with polyBLEP anti-aliasing.
     @{
     
     @fn void    tPBPulse_init        (tPBPulse* const osc, LEAF* const leaf)
     @brief Initialize a tPBPulse to the default mempool of a LEAF instance.
     @param osc A pointer to the tPBPulse to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPBPulse_initToPool  (tPBPulse* const osc, tMempool* const)
     @brief Initialize a tPBPulse to a specified mempool.
     @param osc A pointer to the tPBPulse to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPBPulse_free        (tPBPulse* const osc)
     @brief Free a tPBPulse from its mempool.
     @param osc A pointer to the tPBPulse to free.
     
     @fn float   tPBPulse_tick        (tPBPulse* const osc)
     @brief
     @param osc A pointer to the relevant tPBPulse.
     
     @fn void    tPBPulse_setFreq     (tPBPulse* const osc, float freq)
     @brief
     @param osc A pointer to the relevant tPBPulse.
     
     @fn void    tPBPulse_setWidth    (tPBPulse* const osc, float width)
     @brief
     @param osc A pointer to the relevant tPBPulse.
     ￼￼￼
     @} */
    
    typedef struct _tPBPulse
    {
        tMempool mempool;
        float phase;
        float inc,freq;
        float width;
    } _tPBPulse;
    
    typedef _tPBPulse* tPBPulse;
    
    void    tPBPulse_init        (tPBPulse* const osc, LEAF* const leaf);
    void    tPBPulse_initToPool  (tPBPulse* const osc, tMempool* const);
    void    tPBPulse_free        (tPBPulse* const osc);
    
    float   tPBPulse_tick        (tPBPulse* const osc);
    void    tPBPulse_setFreq     (tPBPulse* const osc, float freq);
    void    tPBPulse_setWidth    (tPBPulse* const osc, float width);
    
    //==============================================================================
    
    /*!
     @defgroup tpbsaw tPBSaw
     @ingroup oscillators
     @brief Saw wave oscillator with polyBLEP anti-aliasing.
     @{
     
     @fn void    tPBSaw_init          (tPBSaw* const osc, LEAF* const leaf)
     @brief Initialize a tPBSaw to the default mempool of a LEAF instance.
     @param osc A pointer to the tPBSaw to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPBSaw_initToPool    (tPBSaw* const osc, tMempool* const mempool)
     @brief Initialize a tPBSaw to a specified mempool.
     @param osc A pointer to the tPBSaw to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPBSaw_free          (tPBSaw* const osc)
     @brief Free a tPBSaw from its mempool.
     @param osc A pointer to the tPBSaw to free.
     
     @fn float   tPBSaw_tick          (tPBSaw* const osc)
     @brief
     @param osc A pointer to the relevant tPBSaw.
     
     @fn void    tPBSaw_setFreq       (tPBSaw* const osc, float freq)
     @brief
     @param osc A pointer to the relevant tPBSaw.
     ￼￼￼
     @} */
    
    typedef struct _tPBSaw
    {
        tMempool mempool;
        float phase;
        float inc,freq;
    } _tPBSaw;
    
    typedef _tPBSaw* tPBSaw;
    
    void    tPBSaw_init          (tPBSaw* const osc, LEAF* const leaf);
    void    tPBSaw_initToPool    (tPBSaw* const osc, tMempool* const mempool);
    void    tPBSaw_free          (tPBSaw* const osc);
    
    float   tPBSaw_tick          (tPBSaw* const osc);
    void    tPBSaw_setFreq       (tPBSaw* const osc, float freq);
    
    //==============================================================================
    
    /*!
     @defgroup tphasor tPhasor
     @ingroup oscillators
     @brief Aliasing phasor.
     @{
     
     @fn void    tPhasor_init        (tPhasor* const osc, LEAF* const leaf)
     @brief Initialize a tPhasor to the default mempool of a LEAF instance.
     @param osc A pointer to the tPhasor to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPhasor_initToPool  (tPhasor* const osc, tMempool* const)
     @brief Initialize a tPhasor to a specified mempool.
     @param osc A pointer to the tPhasor to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPhasor_free        (tPhasor* const osc)
     @brief Free a tPhasor from its mempool.
     @param osc A pointer to the tPhasor to free.
     
     @fn float   tPhasor_tick        (tPhasor* const osc)
     @brief
     @param osc A pointer to the relevant tPhasor.
     
     @fn void    tPhasor_setFreq     (tPhasor* const osc, float freq)
     @brief
     @param osc A pointer to the relevant tPhasor.
     ￼￼￼
     @} */
    
    typedef struct _tPhasor
    {
        
        tMempool mempool;
        float phase;
        float inc,freq;
        uint8_t phaseDidReset;
    } _tPhasor;
    
    typedef _tPhasor* tPhasor;
    
    void    tPhasor_init        (tPhasor* const osc, LEAF* const leaf);
    void    tPhasor_initToPool  (tPhasor* const osc, tMempool* const);
    void    tPhasor_free        (tPhasor* const osc);
    
    float   tPhasor_tick        (tPhasor* const osc);
    void    tPhasor_setFreq     (tPhasor* const osc, float freq);
    
    //==============================================================================
    
    /*!
     @defgroup tnoise tNoise
     @ingroup oscillators
     @brief Noise generator, capable of producing white or pink noise.
     @{
     
     @fn void    tNoise_init         (tNoise* const noise, NoiseType type, LEAF* const leaf)
     @brief Initialize a tNoise to the default mempool of a LEAF instance.
     @param noise A pointer to the tNoise to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tNoise_initToPool   (tNoise* const noise, NoiseType type, tMempool* const)
     @brief Initialize a tNoise to a specified mempool.
     @param noise A pointer to the tNoise to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tNoise_free         (tNoise* const noise)
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
    enum NoiseType
    {
        WhiteNoise, //!< White noise. Full spectrum.
        PinkNoise, //!< Pink noise. Inverse frequency-proportional spectrum.
        NoiseTypeNil,
    };
    
     /*!￼￼￼ @} */
    
    typedef enum NoiseType NoiseType;
    
    typedef struct _tNoise
    {
        
        tMempool mempool;
        NoiseType type;
        float pinkb0, pinkb1, pinkb2;
        float(*rand)(void);
    } _tNoise;
    
    typedef _tNoise* tNoise;

    void    tNoise_init         (tNoise* const noise, NoiseType type, LEAF* const leaf);
    void    tNoise_initToPool   (tNoise* const noise, NoiseType type, tMempool* const);
    void    tNoise_free         (tNoise* const noise);
    
    float   tNoise_tick         (tNoise* const noise);
    
    //==============================================================================
    
    /*!
     @defgroup tneuron tNeuron
     @ingroup oscillators
     @brief Model of a neuron, adapted to act as an oscillator.
     @{
     
     @fn void    tNeuron_init        (tNeuron* const neuron, LEAF* const leaf)
     @brief Initialize a tNeuron to the default mempool of a LEAF instance.
     @param neuron A pointer to the tNeuron to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tNeuron_initToPool  (tNeuron* const neuron, tMempool* const mempool)
     @brief Initialize a tNeuron to a specified mempool.
     @param neuron A pointer to the tNeuron to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tNeuron_free        (tNeuron* const neuron)
     @brief Free a tNeuron from its mempool.
     @param neuron A pointer to the tNeuron to free.
     
     @fn void    tNeuron_reset       (tNeuron* const neuron)
     @brief Reset the neuron model.
     @param neuron A pointer to the relevant tNeuron.
     
     @fn float   tNeuron_tick        (tNeuron* const neuron)
     @brief Tick a tNeuron oscillator.
     @param neuron A pointer to the relevant tNeuron.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tNeuron_setMode     (tNeuron* const neuron, NeuronMode mode)
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
    enum NeuronMode
    {
        NeuronNormal, //!< Normal operation
        NeuronTanh, //!< Tanh voltage shaping
        NeuronAaltoShaper, //!< Aalto voltage shaping
        NeuronModeNil
    };
    
    /*!￼￼￼ @} */
    
    typedef enum NeuronMode NeuronMode;
    
    typedef struct _tNeuron
    {
        tMempool mempool;
        
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
    
    void    tNeuron_init        (tNeuron* const neuron, LEAF* const leaf);
    void    tNeuron_initToPool  (tNeuron* const neuron, tMempool* const mempool);
    void    tNeuron_free        (tNeuron* const neuron);
    
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
    
    
#define FILLEN 256
    
    /*!
     @defgroup tmbpulse tMBPulse
     @ingroup oscillators
     @brief Pulse wave oscillator with minBLEP anti-aliasing.
     @{
     
     @fn void tMBPulse_init(tMBPulse* const osc, LEAF* const leaf)
     @brief Initialize a tMBPulse to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBPulse to initialize.
     
     @fn void tMBPulse_initToPool(tMBPulse* const osc, tMempool* const mempool)
     @brief Initialize a tMBPulse to a specified mempool.
     @param osc A pointer to the tMBPulse to initialize.
     
     @fn void tMBPulse_free(tMBPulse* const osc)
     @brief Free a tMBPulse from its mempool.
     @param osc A pointer to the tMBPulse to free.
     
     @fn float tMBPulse_tick(tMBPulse* const osc)
     @brief
     @param osc A pointer to the relevant tMBPulse.
     
     @fn void tMBPulse_setFreq(tMBPulse* const osc, float f)
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
    
    typedef struct _tMBPulse
    {
        
        tMempool mempool;
        float    out;
        float    amp;
        float    last_amp;
        float    freq;
        float    waveform;    // duty cycle, must be in [-1, 1]
        float    lastsyncin;
        float    sync;
        float    syncdir;
        int      softsync;
        float   _p, _w, _b, _x, _z;
        float   _f [FILLEN + STEP_DD_PULSE_LENGTH];
        int     _j, _k;
        bool    _init;
    } _tMBPulse;
    
    typedef _tMBPulse* tMBPulse;
    
    void tMBPulse_init(tMBPulse* const osc, LEAF* const leaf);
    void tMBPulse_initToPool(tMBPulse* const osc, tMempool* const mempool);
    void tMBPulse_free(tMBPulse* const osc);
    
    float tMBPulse_tick(tMBPulse* const osc);
    void tMBPulse_setFreq(tMBPulse* const osc, float f);
    void tMBPulse_setWidth(tMBPulse* const osc, float w);
    float tMBPulse_sync(tMBPulse* const osc, float sync);
    void tMBPulse_setSyncMode(tMBPulse* const osc, int hardOrSoft);
    
    /*!
     @defgroup tmbtriangle tMBTriangle
     @ingroup oscillators
     @brief Triangle wave oscillator with minBLEP anti-aliasing.
     @{
     
     @fn void tMBTriangle_init(tMBTriangle* const osc, LEAF* const leaf)
     @brief Initialize a tMBTriangle to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBTriangle to initialize.
     
     @fn void tMBTriangle_initToPool(tMBTriangle* const osc, tMempool* const mempool)
     @brief Initialize a tMBTriangle to a specified mempool.
     @param osc A pointer to the tMBTriangle to initialize.
     
     @fn void tMBTriangle_free(tMBTriangle* const osc)
     @brief Free a tMBTriangle from its mempool.
     @param osc A pointer to the tMBTriangle to free.
     
     @fn float tMBTriangle_tick(tMBTriangle* const osc)
     @brief
     @param osc A pointer to the relevant tMBTriangle.
     
     @fn void tMBTriangle_setFreq(tMBTriangle* const osc, float f)
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
    
    typedef struct _tMBTriangle
    {
        
        tMempool mempool;
        float    out;
        float    amp;
        float    last_amp;
        float    freq;
        float    waveform;    // duty cycle, must be in [-1, 1]
        float    lastsyncin;
        float    sync;
        float    syncdir;
        int      softsync;
        float   _p, _w, _b, _z;
        float   _f [FILLEN + LONGEST_DD_PULSE_LENGTH];
        int     _j, _k;
        bool    _init;
    } _tMBTriangle;
    
    typedef _tMBTriangle* tMBTriangle;
    
    void tMBTriangle_init(tMBTriangle* const osc, LEAF* const leaf);
    void tMBTriangle_initToPool(tMBTriangle* const osc, tMempool* const mempool);
    void tMBTriangle_free(tMBTriangle* const osc);
    
    float tMBTriangle_tick(tMBTriangle* const osc);
    void tMBTriangle_setFreq(tMBTriangle* const osc, float f);
    void tMBTriangle_setWidth(tMBTriangle* const osc, float w);
    float tMBTriangle_sync(tMBTriangle* const osc, float sync);
    void tMBTriangle_setSyncMode(tMBTriangle* const osc, int hardOrSoft);
    
    
    /*!
     @defgroup tmbsaw tMBSaw
     @ingroup oscillators
     @brief Saw wave oscillator with minBLEP anti-aliasing.
     @{
     
     @fn void tMBSaw_init(tMBSaw* const osc, LEAF* const leaf)
     @brief Initialize a tMBSaw to the default mempool of a LEAF instance.
     @param osc A pointer to the tMBSaw to initialize.
     
     @fn void tMBSaw_initToPool(tMBSaw* const osc, tMempool* const mempool)
     @brief Initialize a tMBSaw to a specified mempool.
     @param osc A pointer to the tMBSaw to initialize.
     
     @fn void tMBSaw_free(tMBSaw* const osc)
     @brief Free a tMBSaw from its mempool.
     @param osc A pointer to the tMBSaw to free.
     
     @fn float tMBSaw_tick(tMBSaw* const osc)
     @brief Tick the oscillator.
     @param osc A pointer to the relevant tMBSaw.
     @return The ticked sample.
     
     @fn void tMBSaw_setFreq(tMBSaw* const osc, float f)
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
    
    typedef struct _tMBSaw
    {
        tMempool mempool;
        float    out;
        float    amp;
        float    last_amp;
        float    freq;
        float    lastsyncin;
        float    sync;
        float    syncdir;
        int      softsync;
        float   _p, _w, _z;
        float   _f [FILLEN + STEP_DD_PULSE_LENGTH];
        int     _j;
        bool    _init;
    } _tMBSaw;
    
    typedef _tMBSaw* tMBSaw;
    
    void tMBSaw_init(tMBSaw* const osc, LEAF* const leaf);
    void tMBSaw_initToPool(tMBSaw* const osc, tMempool* const mempool);
    void tMBSaw_free(tMBSaw* const osc);
    
    float tMBSaw_tick(tMBSaw* const osc);
    void tMBSaw_setFreq(tMBSaw* const osc, float f);
    float tMBSaw_sync(tMBSaw* const osc, float sync);
    void tMBSaw_setSyncMode(tMBSaw* const osc, int hardOrSoft);
    
    //==============================================================================
    
    /*!
     @defgroup ttable tTable
     @ingroup oscillators
     @brief Simple aliasing wavetable oscillator.
     @{
     
     @fn void    tTable_init  (tTable* const osc, float* table, int size, LEAF* const leaf)
     @brief Initialize a tTable to the default mempool of a LEAF instance.
     @param osc A pointer to the tTable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wavetable.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTable_initToPool   (tTable* const osc, float* table, int size, tMempool* const mempool)
     @brief Initialize a tTable to a specified mempool.
     @param osc A pointer to the tTable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wave table.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTable_free         (tTable* const osc)
     @brief Free a tTable from its mempool.
     @param osc A pointer to the tTable to free.
     
     @fn float   tTable_tick         (tTable* const osc)
     @brief Tick a tTable oscillator.
     @param osc A pointer to the relevant tTable.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tTable_setFreq      (tTable* const osc, float freq)
     @brief Set the frequency of a tTable oscillator.
     @param osc A pointer to the relevant tTable.
     @param freq The frequency to set the oscillator to.
     
     @} */
    
    typedef struct _tTable
    {
        tMempool mempool;
        
        float* waveTable;
        int size;
        float inc, freq;
        float phase;
    } _tTable;
    
    typedef _tTable* tTable;
    
    void    tTable_init(tTable* const osc, float* table, int size, LEAF* const leaf);
    void    tTable_initToPool(tTable* const osc, float* table, int size, tMempool* const mempool);
    void    tTable_free(tTable* const osc);
    
    float   tTable_tick(tTable* const osc);
    void    tTable_setFreq(tTable* const osc, float freq);
    
    //==============================================================================
    
    /*!
     @defgroup twavetable tWavetable
     @ingroup oscillators
     @brief Anti-aliased wavetable oscillator.
     @{
     
     @fn void    tWavetable_init  (tWavetable* const osc, float* table, int size, float maxFreq, LEAF* const leaf)
     @brief Initialize a tWavetable to the default mempool of a LEAF instance.
     @param osc A pointer to the tWavetable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wavetable.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tWavetable_initToPool   (tWavetable* const osc, float* table, int size, float maxFreq, tMempool* const mempool)
     @brief Initialize a tWavetable to a specified mempool.
     @param osc A pointer to the tWavetable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wave table.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tWavetable_free         (tWavetable* const osc)
     @brief Free a tWavetable from its mempool.
     @param osc A pointer to the tWavetable to free.
     
     @fn float   tWavetable_tick         (tWavetable* const osc)
     @brief Tick a tWavetable oscillator.
     @param osc A pointer to the relevant tWavetable.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tWavetable_setFreq      (tWavetable* const osc, float freq)
     @brief Set the frequency of a tWavetable oscillator.
     @param osc A pointer to the relevant tWavetable.
     @param freq The frequency to set the oscillator to.
     
     @} */
    
    typedef struct _tWavetable
    {
        tMempool mempool;
        
        float** tables;
        int size;
        int numTables;
        float baseFreq, invBaseFreq;
        float inc, freq;
        float phase;
        
        int oct;
        float w;
        float aa;
        
        tButterworth bl;
    } _tWavetable;
    
    typedef _tWavetable* tWavetable;
    
    void    tWavetable_init(tWavetable* const osc, const float* table, int size, float maxFreq, LEAF* const leaf);
    void    tWavetable_initToPool(tWavetable* const osc, const float* table, int size, float maxFreq, tMempool* const mempool);
    void    tWavetable_free(tWavetable* const osc);
    
    float   tWavetable_tick(tWavetable* const osc);
    void    tWavetable_setFreq(tWavetable* const osc, float freq);
    void    tWavetable_setAntiAliasing(tWavetable* const osc, float aa);
    
    //==============================================================================
    
    /*!
     @defgroup tcompactwavetable tCompactWavetable
     @ingroup oscillators
     @brief A more space-efficient anti-aliased wavetable oscillator than tWavetable but with slightly worse fidelity.
     @{
     
     @fn void    tCompactWavetable_init  (tCompactWavetable* const osc, float* table, int size, float maxFreq, LEAF* const leaf)
     @brief Initialize a tCompactWavetable to the default mempool of a LEAF instance.
     @param osc A pointer to the tCompactWavetable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wavetable.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tCompactWavetable_initToPool   (tCompactWavetable* const osc, float* table, int size, float maxFreq, tMempool* const mempool)
     @brief Initialize a tCompactWavetable to a specified mempool.
     @param osc A pointer to the tCompactWavetable to initialize.
     @param table A pointer to the wavetable data.
     @param size The number of samples in the wave table.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tCompactWavetable_free         (tCompactWavetable* const osc)
     @brief Free a tCompactWavetable from its mempool.
     @param osc A pointer to the tCompactWavetable to free.
     
     @fn float   tCompactWavetable_tick         (tCompactWavetable* const osc)
     @brief Tick a tCompactWavetable oscillator.
     @param osc A pointer to the relevant tCompactWavetable.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tCompactWavetable_setFreq      (tCompactWavetable* const osc, float freq)
     @brief Set the frequency of a tCompactWavetable oscillator.
     @param osc A pointer to the relevant tCompactWavetable.
     @param freq The frequency to set the oscillator to.
     
     @} */
    
    typedef struct _tCompactWavetable
    {
        tMempool mempool;
        
        float** tables;
        int numTables;
        int* sizes;
        float baseFreq, invBaseFreq;
        float inc, freq;
        float phase;
        
        int oct;
        float w;
        float aa;
        
        tButterworth bl;
        
        float dsBuffer[2];
        tOversampler ds;
    } _tCompactWavetable;
    
    typedef _tCompactWavetable* tCompactWavetable;
    
    void    tCompactWavetable_init(tCompactWavetable* const osc, const float* table, int size, float maxFreq, LEAF* const leaf);
    void    tCompactWavetable_initToPool(tCompactWavetable* const osc, const float* table, int size, float maxFreq, tMempool* const mempool);
    void    tCompactWavetable_free(tCompactWavetable* const osc);
    
    float   tCompactWavetable_tick(tCompactWavetable* const osc);
    void    tCompactWavetable_setFreq(tCompactWavetable* const osc, float freq);
    void    tCompactWavetable_setAntiAliasing(tCompactWavetable* const osc, float aa);
    
    //==============================================================================
    
    /*!
     @defgroup twaveset tWaveset
     @ingroup oscillators
     @brief Set of anti-aliased wavetable oscillators that can be faded between.
     @{
     
     @fn void    tWaveset_init(tWaveset* const osc, const float** tables, int n, int size, float maxFreq, LEAF* const leaf)
     @brief Initialize a tWaveset to the default mempool of a LEAF instance.
     @param osc A pointer to the tWaveset to initialize.
     @param tables An array of pointers to wavetable data.
     @param n The number of wavetables.
     @param size The number of samples in each of the wavetables.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param leaf A pointer to the leaf instance.
     
     @fn void  tWaveset_initToPool(tWaveset* const osc, const float** tables, int n, int size, float maxFreq, tMempool* const mempool)
     @brief Initialize a tWaveset to a specified mempool.
     @param osc A pointer to the tWavetable to initialize.
     @param tables An array of pointers to wavetable data.
     @param n The number of wavetables.
     @param size The number of samples in each of the wavetables.
     @param maxFreq The maximum expected frequency of the oscillator. The higher this is, the more memory will be needed.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tWaveset_free         (tWaveset* const osc)
     @brief Free a tWaveset from its mempool.
     @param osc A pointer to the tWaveset to free.
     
     @fn float   tWaveset_tick         (tWaveset* const osc)
     @brief Tick a tWaveset oscillator.
     @param osc A pointer to the relevant tWaveset.
     @return The ticked sample as a float from -1 to 1.
     
     @fn void    tWaveset_setFreq      (tWaveset* const osc, float freq)
     @brief Set the frequency of a tWaveset oscillator.
     @param osc A pointer to the relevant tWaveset.
     @param freq The frequency to set the oscillator to.
     
     @fn void    tWaveset_setIndex(tWaveset* const osc, float index)
     @brief Set the output index of the wavetable set.
     @param index The new index from 0.0 to 1.0 as a smooth fade from the first wavetable in the set to the last.
     
     @} */
    
    typedef struct _tWaveset
    {
        tMempool mempool;
        
        tWavetable* wt;
        int n;
        float* g;
        float index;
    } _tWaveset;
    
    typedef _tWaveset* tWaveset;
    
    void    tWaveset_init(tWaveset* const osc, const float** tables, int n, int size, float maxFreq, LEAF* const leaf);
    void    tWaveset_initToPool(tWaveset* const osc, const float** tables, int n, int size, float maxFreq, tMempool* const mempool);
    void    tWaveset_free(tWaveset* const osc);
    
    float   tWaveset_tick(tWaveset* const osc);
    void    tWaveset_setFreq(tWaveset* const osc, float freq);
    void    tWaveset_setAntiAliasing(tWaveset* const osc, float aa);
    void    tWaveset_setIndex(tWaveset* const osc, float index);
    void    tWaveset_setIndexGain(tWaveset* const osc, int i, float gain);

    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_OSCILLATORS_H_INCLUDED

//==============================================================================



