/*
  ==============================================================================

    LEAFOscillator.h
    Created: 20 Jan 2017 12:00:58pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LEAFOSCILLATOR_H_INCLUDED
#define LEAFOSCILLATOR_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#include "leaf-globals.h"
#include "leaf-math.h"

#include "leaf-filter.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

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
} tNeuron;

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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* tPhasor: Aliasing phasor [0.0, 1.0) */
typedef struct _tPhasor
{
    float phase;
    float inc,freq;
    
} tPhasor;

void        tPhasor_init        (tPhasor*  const);
void        tPhasor_free        (tPhasor*  const);

float       tPhasor_tick        (tPhasor*  const);
int         tPhasor_setFreq     (tPhasor*  const, float freq);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* tCycle: Cycle/Sine waveform. Wavetable synthesis.*/
typedef struct _tCycle
{
    // Underlying phasor
    float phase;
    float inc,freq;
    
} tCycle;

void        tCycle_init         (tCycle*  const);
void        tCycle_free         (tCycle*  const);

float       tCycle_tick         (tCycle*  const);
int         tCycle_setFreq      (tCycle*  const, float freq);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* tSawtooth: Anti-aliased Sawtooth waveform using wavetable interpolation. Wavetables constructed from sine components. */
typedef struct _tSawtooth
{
    // Underlying phasor
    float phase;
    float inc,freq;
    
} tSawtooth;

void        tSawtooth_init      (tSawtooth*  const);
void        tSawtooth_free      (tSawtooth*  const);

float       tSawtooth_tick      (tSawtooth*  const);
int         tSawtooth_setFreq   (tSawtooth*  const, float freq);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* tTriangle: Anti-aliased Triangle waveform using wavetable interpolation. Wavetables constructed from sine components. */
typedef struct _tTriangle
{
    // Underlying phasor
    float phase;
    float inc,freq;
    
} tTriangle;

void        tTriangle_init      (tTriangle*  const);
void        tTriangle_free      (tTriangle*  const);

float       tTriangle_tick      (tTriangle*  const);
int         tTriangle_setFreq   (tTriangle*  const, float freq);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* tSquare: Anti-aliased Square waveform using wavetable interpolation. Wavetables constructed from sine components. */
typedef struct _tSquare
{
    // Underlying phasor
    float phase;
    float inc,freq;
    
} tSquare;

void        tSquare_init        (tSquare*  const);
void        tSquare_free        (tSquare*  const);

float       tSquare_tick        (tSquare*  const);
int         tSquare_setFreq     (tSquare*  const, float freq);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

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
    float(*rand)();
    
} tNoise;

void        tNoise_init          (tNoise* const, NoiseType type);
void        tNoise_free          (tNoise* const);

float       tNoise_tick          (tNoise*  const);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
#ifdef __cplusplus
}
#endif
        
#endif  // LEAFOSCILLATOR_H_INCLUDED
