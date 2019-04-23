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

// Preprocessor defines to determine whether to include component files in build.
#define INC_UTILITIES       (N_ENV || N_MPOLY || N_STACK || N_ENVELOPE || N_ENVELOPEFOLLOW || N_RAMP || N_ADSR || N_COMPRESSOR || N_POLY || N_LOCKHARTWAVEFOLDER)

#define INC_DELAY           (N_DELAY || N_DELAYL || N_DELAYA)

#define INC_FILTER          (N_BUTTERWORTH || N_ONEPOLE || N_TWOPOLE || N_ONEZERO || N_TWOZERO || N_POLEZERO || N_BIQUAD || N_SVF || N_SVFE || N_HIGHPASS || N_FORMANTSHIFTER || N_PITCHSHIFTER || N_PERIOD || N_PITCHSHIFT)

#define INC_OSCILLATOR      (N_PHASOR || N_SAWTOOTH || N_CYCLE || N_TRIANGLE || N_SQUARE || N_NOISE)

#define INC_REVERB          (N_NREV || N_PRCREV)

#define INC_INSTRUMENT      (N_STIFKARP || N_PLUCK || N_VOCODER || N_TALKBOX || N_808SNARE || N_808HIHAT || N_808COWBELL)

#define DELAY_LENGTH        16000   // The maximum delay length of all Delay/DelayL/DelayA components.
                                            // Feel free to change to suit memory constraints or desired delay max length / functionality.

#define TALKBOX_BUFFER_LENGTH   1600    // Every talkbox instance introduces 5 buffers of this size

#endif  // OPPSGLOBALS_H_INCLUDED


