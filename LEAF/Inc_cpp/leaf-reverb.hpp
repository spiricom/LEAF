/*
  ==============================================================================

    LEAFReverb.h
    Created: 20 Jan 2017 12:02:04pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LEAFREVERB_H_INCLUDED
#define LEAFREVERB_H_INCLUDED

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#include "leaf-globals.h"
#include "leaf-math.h"

#include "leaf-delay.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* PRCRev: Reverb, reimplemented from STK (Cook and Scavone). */
typedef struct _tPRCRev
{
    float mix, t60;
    
    float inv_441;
    
    tDelay* allpassDelays[2];
    tDelay* combDelay;
    float allpassCoeff;
    float combCoeff;
    
    float lastIn, lastOut;
    
} tPRCRev;

void    tPRCRev_init    (tPRCRev* const, float t60);
void    tPRCRev_free    (tPRCRev* const);

float   tPRCRev_tick    (tPRCRev* const, float input);

// Set reverb time in seconds.
void    tPRCRev_setT60  (tPRCRev* const, float t60);

// Set mix between dry input and wet output signal.
void    tPRCRev_setMix  (tPRCRev* const, float mix);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* NRev: Reverb, reimplemented from STK (Cook and Scavone). */
typedef struct _tNRev
{
    float mix, t60;
    
    float inv_sr, inv_441;
    
    tDelay* allpassDelays[8];
    tDelay* combDelays[6];
    float allpassCoeff;
    float combCoeffs[6];
    float lowpassState;
    
    float lastIn, lastOut;
    
} tNRev;

void    tNRev_init      (tNRev* const, float t60);
void    tNRev_free      (tNRev* const);

float   tNRev_tick      (tNRev* const, float input);

// Set reverb time in seconds.
void    tNRev_setT60    (tNRev* const, float t60);

// Set mix between dry input and wet output signal.
void    tNRev_setMix    (tNRev*  const, float mix);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#endif  // LEAFREVERB_H_INCLUDED
