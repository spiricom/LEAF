/*
  ==============================================================================

    leaf-crusher.h
    Created: 7 Feb 2019 10:58:22am
    Author:  airship

  ==============================================================================
*/

#ifndef LEAF_CRUSHER_H_INCLUDED
#define LEAF_CRUSHER_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================

#include "leaf-global.h"
#include "leaf-math.h"

//==============================================================================
    
typedef struct _tCrusher
{
    float srr;
    float mult, div;
    float rnd;
    
    uint32_t  op; //which bitwise operation (0-7)
    
    float gain;
    
} tCrusher;
    

void    tCrusher_init    (tCrusher* const);
void    tCrusher_free    (tCrusher* const);

float   tCrusher_tick    (tCrusher* const, float input);
    
// 0.0 - 1.0
void    tCrusher_setOperation (tCrusher* const, float op);
 
// 0.0 - 1.0
void    tCrusher_setQuality (tCrusher* const, float val);
    
// what division to round to
void    tCrusher_setRound (tCrusher* const, float rnd);
    
// sampling ratio
void    tCrusher_setSamplingRatio (tCrusher* const, float ratio);
    
//==============================================================================

#ifdef __cplusplus
}
#endif

#endif // LEAF_WAVEFOLDER_H_INCLUDED

//==============================================================================
