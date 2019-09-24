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

#include "leaf-globals.h"
#include "leaf-math.h"

//==============================================================================

/* tLockhartWavefolder */
#define THRESH 10e-10
#define ILL_THRESH 10e-10
#define LOCKHART_RL 7.5e3
#define LOCKHART_R 15e3
#define LOCKHART_VT 26e-3
#define LOCKHART_Is 10e-16
#define LOCKHART_A 2.0*LOCKHART_RL/LOCKHART_R
#define LOCKHART_B (LOCKHART_R+2.0*LOCKHART_RL)/(LOCKHART_VT*LOCKHART_R)
#define LOCKHART_D (LOCKHART_RL*LOCKHART_Is)/LOCKHART_VT
#define VT_DIV_B LOCKHART_VT/LOCKHART_B

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
