/*
  ==============================================================================

    leaf-globals.h
    Created: 23 Jan 2017 10:34:10pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OPPSGLOBALS_H_INCLUDED
#define OPPSGLOBALS_H_INCLUDED

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                                       *
 * If your application requires use of many instances of one component or is facing memory limitations,  *
 * use this set of defines to increase or limit the number of instances of each component. The library   *
 * will pre-allocate only the number of instances defined here.                                          *
 *                                                                                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "leaf-mempool.h"

typedef struct _LEAF
{
    float   sampleRate;
    float   invSampleRate;
    int     blockSize;
    
    float   (*random)(void);
} LEAF;

extern LEAF leaf;

#define SHAPER1_TABLE_SIZE 65536
extern const float shaper1[SHAPER1_TABLE_SIZE];

#define NUM_VOICES 8
#define NUM_SHIFTERS 4
#define MPOLY_NUM_MAX_VOICES 8
#define NUM_OSC 4
#define INV_NUM_OSC (1.0f / NUM_OSC)
#define PS_FRAME_SIZE 1024 // SNAC_FRAME_SIZE in LEAFCore.h should match (or be smaller than?) this
#define ENV_WINDOW_SIZE 1024
#define ENV_HOP_SIZE 256
#define NUM_KNOBS 4

// Arbitrary number of instances.
#define NI 0

//#define     N_MYOBJECT            0   // Zero instances is fine.
#define     N_808SNARE           0
#define     N_808HIHAT           0
#define     N_808COWBELL         0
#define     N_COMPRESSOR         0
#define     N_PRCREV             1
#define     N_NREV               0
#define		N_PERIOD			 1
#define		N_PITCHSHIFT		 NUM_SHIFTERS
#define     N_PITCHSHIFTER       0
#define     N_PLUCK              0
#define     N_STIFKARP           0
#define     N_NEURON             0
#define     N_PHASOR             0
#define     N_CYCLE              1
#define     N_SAWTOOTH           NUM_VOICES * NUM_OSC + 1
#define     N_TRIANGLE           0
#define     N_SQUARE             0
#define     N_NOISE              1 + (1 * N_STIFKARP) + (1 * N_PLUCK) // StifKarp and Pluck each contain 1 Noise component.
#define     N_ONEPOLE            0 + (1 * N_PLUCK)
#define     N_TWOPOLE            0
#define     N_ONEZERO            0 + (1 * N_STIFKARP) + (1 * N_PLUCK) + (1 * N_NEURON)
#define     N_BUTTERWORTH        0
#define     N_TWOZERO            0
#define     N_POLEZERO           0 + (1 * N_NEURON)
#define     N_BIQUAD             0 + (4 * N_STIFKARP)
#define     N_SVF                6 + 32*N_BUTTERWORTH
#define     N_SVFE               0
#define     N_HIGHPASS           3 + (1 * N_PITCHSHIFTER) + (1 * N_PITCHSHIFT)
#define     N_DELAY              0 + (14 * N_NREV) + (3 * N_PRCREV)
#define     N_DELAYL             1 + (1 * N_STIFKARP) + (1 * N_PLUCK)
#define     N_DELAYA             0 + (1 * N_PRCREV) + (1 * N_STIFKARP)
#define     N_ENVELOPE           0
#define     N_ENV                0 + (1 * N_PITCHSHIFTER) + (1 * N_PERIOD)
#define     N_ADSR               0
#define     N_ENVELOPEFOLLOW     2
#define     N_VOCODER            0
#define     N_TALKBOX            1
#define     N_POLY               0
#define     N_MPOLY              1
#define     N_STACK              0 + (2 * N_MPOLY)
#define     N_SOLAD              0 + (1 * N_PITCHSHIFTER) + (1 * N_PITCHSHIFT)
#define     N_SNAC               0 + (1 * N_PITCHSHIFTER) + (1 * N_PERIOD)
#define     N_ATKDTK             0
#define     N_RAMP               5 + MPOLY_NUM_MAX_VOICES + (N_MPOLY * MPOLY_NUM_MAX_VOICES) + (1 * NUM_KNOBS)
#define     N_LOCKHARTWAVEFOLDER 0
#define     N_FORMANTSHIFTER     1

// Preprocessor defines to determine whether to include component files in build.
#define INC_UTILITIES       (N_ENV || N_MPOLY || N_STACK || N_ENVELOPE || N_ENVELOPEFOLLOW || N_RAMP || N_ADSR || N_COMPRESSOR || N_POLY || N_LOCKHARTWAVEFOLDER)

#define INC_DELAY           (N_DELAY || N_DELAYL || N_DELAYA)

#define INC_FILTER          (N_BUTTERWORTH || N_ONEPOLE || N_TWOPOLE || N_ONEZERO || N_TWOZERO || N_POLEZERO || N_BIQUAD || N_SVF || N_SVFE || N_HIGHPASS || N_FORMANTSHIFTER || N_PITCHSHIFTER || N_PERIOD || N_PITCHSHIFT)

#define INC_OSCILLATOR      (N_PHASOR || N_SAWTOOTH || N_CYCLE || N_TRIANGLE || N_SQUARE || N_NOISE)

#define INC_REVERB          (N_NREV || N_PRCREV)

#define INC_INSTRUMENT      (N_STIFKARP || N_PLUCK || N_VOCODER || N_TALKBOX || N_808SNARE || N_808HIHAT || N_808COWBELL)

#define     DELAY_LENGTH        16000   // The maximum delay length of all Delay/DelayL/DelayA components.
                                            // Feel free to change to suit memory constraints or desired delay max length / functionality.

#define TALKBOX_BUFFER_LENGTH   1600    // Every talkbox instance introduces 5 buffers of this size

#endif  // OPPSGLOBALS_H_INCLUDED


