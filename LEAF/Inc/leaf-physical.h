/*
  ==============================================================================

    leaf-physical.h
    Created: 30 Nov 2018 10:41:55am
    Author:  airship

  ==============================================================================
*/

#ifndef LEAF_PHYSICAL_H_INCLUDED
#define LEAF_PHYSICAL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
//==============================================================================

#include "leaf-global.h"
#include "leaf-math.h"
#include "leaf-delay.h"
#include "leaf-filters.h"
#include "leaf-oscillators.h"
#include "leaf-envelopes.h"
#include "leaf-dynamics.h"

//==============================================================================

/* Karplus Strong model */
typedef struct _tPluck
{
    tAllpassDelay     delayLine; // Allpass or Linear??  big difference...
    tOneZero    loopFilter;
    tOnePole    pickFilter;
    tNoise      noise;
    
    float lastOut;
    float loopGain;
    float lastFreq;
    
    float sr;
    
} tPluck;

void        tPluck_init          (tPluck*  const, float lowestFrequency); //float delayBuff[DELAY_LENGTH]);
void        tPluck_free          (tPluck*  const);

float       tPluck_tick          (tPluck*  const);

// Pluck the string.
void        tPluck_pluck         (tPluck*  const, float amplitude);

// Start a note with the given frequency and amplitude.;
void        tPluck_noteOn        (tPluck*  const, float frequency, float amplitude );

// Stop a note with the given amplitude (speed of decay).
void        tPluck_noteOff       (tPluck*  const, float amplitude );

// Set instrument parameters for a particular frequency.
void        tPluck_setFrequency  (tPluck*  const, float frequency );

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void        tPluck_controlChange (tPluck*  const, int number, float value);

// tPluck Utilities.
float       tPluck_getLastOut    (tPluck*  const);

//==============================================================================
    
/* Stif Karplus Strong model */
typedef struct _tKarplusStrong
{
    tAllpassDelay  delayLine;
    tLinearDelay combDelay;
    tOneZero filter;
    tNoise   noise;
    tBiQuad  biquad[4];
    
    
    
    uint32_t length;
    float loopGain;
    float baseLoopGain;
    float lastFrequency;
    float lastLength;
    float stretching;
    float pluckAmplitude;
    float pickupPosition;
    
    float lastOut;
    
} tKarplusStrong;

typedef enum SKControlType
{
    SKPickPosition = 0,
    SKStringDamping,
    SKDetune,
    SKControlTypeNil
} SKControlType;

void        tKarplusStrong_init               (tKarplusStrong* const, float lowestFrequency); // float delayBuff[2][DELAY_LENGTH]);
void        tKarplusStrong_free               (tKarplusStrong* const);

float       tKarplusStrong_tick               (tKarplusStrong*  const);

// Pluck the string.
void        tKarplusStrong_pluck              (tKarplusStrong*  const, float amplitude);

// Start a note with the given frequency and amplitude.;
void        tKarplusStrong_noteOn             (tKarplusStrong*  const, float frequency, float amplitude );

// Stop a note with the given amplitude (speed of decay).
void        tKarplusStrong_noteOff            (tKarplusStrong*  const, float amplitude );

// Set instrument parameters for a particular frequency.
void        tKarplusStrong_setFrequency       (tKarplusStrong*  const, float frequency );

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
// Use SKPickPosition, SKStringDamping, or SKDetune for type.
void        tKarplusStrong_controlChange      (tKarplusStrong*  const, SKControlType type, float value);

// Set the stretch "factor" of the string (0.0 - 1.0).
void        tKarplusStrong_setStretch         (tKarplusStrong*  const, float stretch );

// Set the pluck or "excitation" position along the string (0.0 - 1.0).
void        tKarplusStrong_setPickupPosition  (tKarplusStrong*  const, float position );

// Set the base loop gain.
void        tKarplusStrong_setBaseLoopGain    (tKarplusStrong*  const, float aGain );

// tKarplusStrong utilities.
float       tKarplusStrong_getLastOut         (tKarplusStrong*  const);
    
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* Simple Living String */
typedef struct _tSimpleLivingString {
    float freq, waveLengthInSamples;        // the frequency of the string, determining delay length
    float dampFreq;    // frequency for the bridge LP filter, in Hz
    float decay; // amplitude damping factor for the string (only active in mode 0)
    float levMode;
    float curr;
    tLinearDelay delayLine;
    tOnePole bridgeFilter;
    tHighpass DCblocker;
    tFeedbackLeveler fbLev;
    tExpSmooth wlSmooth;
} tSimpleLivingString;

void    tSimpleLivingString_init      (tSimpleLivingString* const, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode);
void    tSimpleLivingString_free      (tSimpleLivingString* const);
float   tSimpleLivingString_tick      (tSimpleLivingString* const, float input);
float   tSimpleLivingString_sample    (tSimpleLivingString* const);
int     tSimpleLivingString_setFreq               (tSimpleLivingString* const, float freq);
int     tSimpleLivingString_setWaveLength        (tSimpleLivingString* const, float waveLength); // in samples
int     tSimpleLivingString_setDampFreq           (tSimpleLivingString* const, float dampFreq);
int     tSimpleLivingString_setDecay             (tSimpleLivingString* const, float decay); // should be near 1.0
int     tSimpleLivingString_setTargetLev        (tSimpleLivingString* const, float targetLev);
int     tSimpleLivingString_setLevSmoothFactor  (tSimpleLivingString* const, float levSmoothFactor);
int     tSimpleLivingString_setLevStrength        (tSimpleLivingString* const, float levStrength);
int     tSimpleLivingString_setLevMode        (tSimpleLivingString* const, int levMode);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* Living String */
typedef struct _tLivingString {
    float freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
    float pickPos;    // the pick position, dividing the string in two, in ratio
    float prepIndex;    // the amount of pressure on the pickpoint of the string (near 0=soft obj, near 1=hard obj)
    float dampFreq;    // frequency for the bridge LP filter, in Hz
    float decay; // amplitude damping factor for the string (only active in mode 0)
    float levMode;
    float curr;
    tLinearDelay delLF,delUF,delUB,delLB;    // delay for lower/upper/forward/backward part of the waveguide model
    tOnePole bridgeFilter, nutFilter, prepFilterU, prepFilterL;
    tHighpass DCblockerL, DCblockerU;
    tFeedbackLeveler fbLevU, fbLevL;
    tExpSmooth wlSmooth, ppSmooth;
} tLivingString;

void    tLivingString_init      (tLivingString* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay,
                                 float targetLev, float levSmoothFactor, float levStrength, int levMode);
void    tLivingString_free      (tLivingString* const);
float   tLivingString_tick      (tLivingString* const, float input);
float   tLivingString_sample    (tLivingString* const);
int     tLivingString_setFreq               (tLivingString* const, float freq);
int     tLivingString_setWaveLength            (tLivingString* const, float waveLength); // in samples
int     tLivingString_setPickPos               (tLivingString* const, float pickPos);
int     tLivingString_setPrepIndex             (tLivingString* const, float prepIndex);
int     tLivingString_setDampFreq           (tLivingString* const, float dampFreq);
int     tLivingString_setDecay                 (tLivingString* const, float decay); // should be near 1.0
int     tLivingString_setTargetLev            (tLivingString* const, float targetLev);
int     tLivingString_setLevSmoothFactor      (tLivingString* const, float levSmoothFactor);
int     tLivingString_setLevStrength        (tLivingString* const, float levStrength);
int     tLivingString_setLevMode            (tLivingString* const, int levMode);
    
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

//Reed Table - borrowed from STK
typedef struct _tReedTable {
    float offset, slope;
} tReedTable;

void    tReedTable_init      (tReedTable* const, float offset, float slope);
void    tReedTable_free      (tReedTable* const);
float   tReedTable_tick      (tReedTable* const, float input);
float   tReedTable_tanh_tick     (tReedTable* const p, float input); //tanh softclip version of reed table - replacing the hard clip in original stk code
void     tReedTable_setOffset   (tReedTable* const, float offset);
void     tReedTable_setSlope (tReedTable* const, float slope);

//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_PHYSICAL_H_INCLUDED

//==============================================================================
