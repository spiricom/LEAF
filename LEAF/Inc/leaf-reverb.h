/*
  ==============================================================================

    LEAFReverb.h
    Created: 20 Jan 2017 12:02:04pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LEAFREVERB_H_INCLUDED
#define LEAFREVERB_H_INCLUDED

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#include "leaf-globals.h"
#include "leaf-math.h"

#include "leaf-delay.h"
#include "leaf-filter.h"
#include "leaf-oscillator.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

typedef struct _tDattorro
{
    float   predelay;
    float   input_filter;
    float   feedback_filter;
    float   feedback_gain;
    float   mix;
    
    
    float   size, size_max, t;
    
    float   f1_delay_2_last,
    f2_delay_2_last;
    
    float   f1_last,
    f2_last;
    
    // INPUT
    tTapeDelay  in_delay;
    tOnePole    in_filter;
    tAllpass    in_allpass[4];
    
    // FEEDBACK 1
    tAllpass    f1_allpass;
    tTapeDelay  f1_delay_1;
    tOnePole    f1_filter;
    tTapeDelay  f1_delay_2;
    tTapeDelay  f1_delay_3;
    tHighpass   f1_hp;
    
    tCycle      f1_lfo;
    
    // FEEDBACK 2
    tAllpass    f2_allpass;
    tTapeDelay  f2_delay_1;
    tOnePole    f2_filter;
    tTapeDelay  f2_delay_2;
    tTapeDelay  f2_delay_3;
    tHighpass   f2_hp;
    
    tCycle      f2_lfo;
    
} tDattorro;

void    tDattorro_init              (tDattorro* const);
void    tDattorro_free              (tDattorro* const);

float   tDattorro_tick              (tDattorro* const, float input);

void    tDattorro_setMix            (tDattorro* const, float mix);
void    tDattorro_setSize           (tDattorro* const, float size);
void    tDattorro_setInputDelay     (tDattorro* const, float preDelay);
void    tDattorro_setInputFilter    (tDattorro* const, float freq);
void    tDattorro_setFeedbackFilter (tDattorro* const, float freq);
void    tDattorro_setFeedbackGain   (tDattorro* const, float gain);

#endif  // LEAFREVERB_H_INCLUDED
