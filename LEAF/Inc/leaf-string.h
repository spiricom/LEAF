/*
  ==============================================================================

    leaf-string.h
    Created: 30 Nov 2018 10:41:55am
    Author:  airship

  ==============================================================================
*/

#ifndef LEAF_STRING_H_INCLUDED
#define LEAF_STRING_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
//==============================================================================

#include "leaf-globals.h"
#include "leaf-math.h"

#include "leaf-delay.h"
#include "leaf-filter.h"
#include "leaf-oscillator.h"

//==============================================================================

/* Karplus Strong model */
typedef struct _tPluck
{
    tDelayA*     delayLine; // Allpass or Linear??  big difference...
    tOneZero*    loopFilter;
    tOnePole*    pickFilter;
    tNoise*      noise;
    
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
typedef struct _tStifKarp
{
    tDelayA*  delayLine;
    tDelayL* combDelay;
    tOneZero* filter;
    tNoise*   noise;
    tBiQuad*  biquad[4];
    
    
    
    uint32_t length;
    float loopGain;
    float baseLoopGain;
    float lastFrequency;
    float lastLength;
    float stretching;
    float pluckAmplitude;
    float pickupPosition;
    
    float lastOut;
    
} tStifKarp;

typedef enum SKControlType
{
    SKPickPosition = 0,
    SKStringDamping,
    SKDetune,
    SKControlTypeNil
} SKControlType;

void        tStifKarp_init               (tStifKarp* const, float lowestFrequency); // float delayBuff[2][DELAY_LENGTH]);
void        tStifKarp_free               (tStifKarp* const);

float       tStifKarp_tick               (tStifKarp*  const);

// Pluck the string.
void        tStifKarp_pluck              (tStifKarp*  const, float amplitude);

// Start a note with the given frequency and amplitude.;
void        tStifKarp_noteOn             (tStifKarp*  const, float frequency, float amplitude );

// Stop a note with the given amplitude (speed of decay).
void        tStifKarp_noteOff            (tStifKarp*  const, float amplitude );

// Set instrument parameters for a particular frequency.
void        tStifKarp_setFrequency       (tStifKarp*  const, float frequency );

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
// Use SKPickPosition, SKStringDamping, or SKDetune for type.
void        tStifKarp_controlChange      (tStifKarp*  const, SKControlType type, float value);

// Set the stretch "factor" of the string (0.0 - 1.0).
void        tStifKarp_setStretch         (tStifKarp*  const, float stretch );

// Set the pluck or "excitation" position along the string (0.0 - 1.0).
void        tStifKarp_setPickupPosition  (tStifKarp*  const, float position );

// Set the base loop gain.
void        tStifKarp_setBaseLoopGain    (tStifKarp*  const, float aGain );

// tStifKarp utilities.
float       tStifKarp_getLastOut         (tStifKarp*  const);

//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_STRING_H_INCLUDED

//==============================================================================
