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
    

    float*  sinewave;
    float*  sawtooth[11];
    float*  square[11];
    float*  triangle[11];


    float   (*random)(void);
} LEAF;
    
//==============================================================================

extern LEAF leaf; // The global instance of LEAF.
    
//==============================================================================

#define PS_FRAME_SIZE 1024 // SNAC_FRAME_SIZE in leaf-pitch.h should match (or be smaller than?) this
#define ENV_WINDOW_SIZE 1024
#define ENV_HOP_SIZE 256
    
union unholy_t { /* a union between a float and an integer */
    float f;
    int i;
};
    
//==============================================================================
    
#ifdef __cplusplus
}
#endif
    
#endif  // LEAF_GLOBALS_H_INCLUDED

//==============================================================================
