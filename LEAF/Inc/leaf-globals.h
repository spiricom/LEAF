/*==============================================================================

    leaf-globals.h
    Created: 23 Jan 2017 10:34:10pm
    Author:  Michael R Mulshine

==============================================================================*/

#ifndef LEAF_GLOBALS_H_INCLUDED
#define LEAF_GLOBALS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
//==============================================================================

#include "leaf-mempool.h"
    
//==============================================================================
    
typedef struct _LEAF
{
    float   sampleRate;
    float   invSampleRate;
    int     blockSize;
    
    /*
    float*  sinewave;
    float*  sawtooth[11];
    float*  square[11];
    float*  triangle[11];
     */

    float   (*random)(void);
} LEAF;
    
//==============================================================================

extern LEAF leaf; // The global instance of LEAF.
    
//==============================================================================

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

#define     DELAY_LENGTH        16000   // The maximum delay length of all Delay/DelayL/DelayA components.
                                            // Feel free to change to suit memory constraints or desired delay max length / functionality.

#define TALKBOX_BUFFER_LENGTH   1600    // Every talkbox instance introduces 5 buffers of this size

    
//==============================================================================
    
#ifdef __cplusplus
}
#endif
    
#endif  // LEAF_GLOBALS_H_INCLUDED

//==============================================================================
