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
#include "leaf-filters.h"
    
    //==============================================================================
    
    /* tCycle: Cycle/Sine waveform. Wavetable synthesis.*/
    typedef struct _tCycle
    {
        // Underlying phasor
        float phase;
        float inc,freq;
        
    } _tCycle;
    
    typedef _tCycle* tCycle;
    
    void        tCycle_init         (tCycle*  const);
    void        tCycle_free         (tCycle*  const);
    
    float       tCycle_tick         (tCycle*  const);
    void         tCycle_setFreq      (tCycle*  const, float freq);
    
    //==============================================================================
    
    /* tTriangle: Anti-aliased Triangle waveform using wavetable interpolation. Wavetables constructed from sine components. */
    typedef struct _tTriangle
    {
        // Underlying phasor
        float phase;
        float inc,freq;
        
    } _tTriangle;
    
    typedef _tTriangle* tTriangle;
    
    void        tTriangle_init      (tTriangle*  const);
    void        tTriangle_free      (tTriangle*  const);
    
    float       tTriangle_tick      (tTriangle*  const);
    int         tTriangle_setFreq   (tTriangle*  const, float freq);
    
    //==============================================================================
    
    /* tSquare: Anti-aliased Square waveform using wavetable interpolation. Wavetables constructed from sine components. */
    typedef struct _tSquare
    {
        // Underlying phasor
        float phase;
        float inc,freq;
        
    } _tSquare;
    
    typedef _tSquare* tSquare;
    
    void        tSquare_init        (tSquare*  const);
    void        tSquare_free        (tSquare*  const);
    
    float       tSquare_tick        (tSquare*  const);
    int         tSquare_setFreq     (tSquare*  const, float freq);
    
    //==============================================================================
    
    /* tSawtooth: Anti-aliased Sawtooth waveform using wavetable interpolation. Wavetables constructed from sine components. */
    typedef struct _tSawtooth
    {
        // Underlying phasor
        float phase;
        float inc,freq;
        
    } _tSawtooth;
    
    typedef _tSawtooth* tSawtooth;
    
    void        tSawtooth_init      (tSawtooth*  const);
    void        tSawtooth_free      (tSawtooth*  const);
    
    float       tSawtooth_tick      (tSawtooth*  const);
    int         tSawtooth_setFreq   (tSawtooth*  const, float freq);
    
    //==============================================================================
    
    /* tPhasor: Aliasing phasor [0.0, 1.0) */
    typedef struct _tPhasor
    {
        float phase;
        float inc,freq;
        
    } _tPhasor;
    
    typedef _tPhasor* tPhasor;
    
    void        tPhasor_init        (tPhasor*  const);
    void        tPhasor_free        (tPhasor*  const);
    
    float       tPhasor_tick        (tPhasor*  const);
    int         tPhasor_setFreq     (tPhasor*  const, float freq);
    
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
    
    void        tNoise_init          (tNoise* const, NoiseType type);
    void        tNoise_free          (tNoise* const);
    
    float       tNoise_tick          (tNoise*  const);
    
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
    
    void        tNeuron_init        (tNeuron* const);
    void        tNeuron_free        (tNeuron* const);
    
    void        tNeuron_reset       (tNeuron* const);
    float       tNeuron_Tick        (tNeuron* const);
    void        tNeuron_setMode     (tNeuron* const, NeuronMode mode);
    void        tNeuron_setCurrent  (tNeuron* const, float current);
    void        tNeuron_setK        (tNeuron* const, float K);
    void        tNeuron_setL        (tNeuron* const, float L);
    void        tNeuron_setN        (tNeuron* const, float N);
    void        tNeuron_setC        (tNeuron* const, float C);
    void        tNeuron_setV1       (tNeuron* const, float V1);
    void        tNeuron_setV2       (tNeuron* const, float V2);
    void        tNeuron_setV3       (tNeuron* const, float V3);
    void        tNeuron_setTimeStep (tNeuron* const, float timestep);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_OSCILLATORS_H_INCLUDED

//==============================================================================

