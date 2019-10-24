/*==============================================================================

    leaf-wavefolder.h
    Created: 30 Nov 2018 11:57:05am
    Author:  airship

==============================================================================*/

#ifndef LEAF_WAVEFOLDER_H_INCLUDED
#define LEAF_WAVEFOLDER_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================

#include "leaf-global.h"
#include "leaf-math.h"

//==============================================================================

/* tLockhartWavefolder */

typedef struct _tLockhartWavefolder
{
    double Ln1;
    double Fn1;
    float xn1;
    
} tLockhartWavefolder;

void    tLockhartWavefolder_init    (tLockhartWavefolder* const);
void    tLockhartWavefolder_free    (tLockhartWavefolder* const);

float   tLockhartWavefolder_tick    (tLockhartWavefolder* const, float samp);

    
//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_WAVEFOLDER_H_INCLUDED

//==============================================================================
