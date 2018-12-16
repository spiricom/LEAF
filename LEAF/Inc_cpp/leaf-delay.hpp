/*
  ==============================================================================

    LEAFDelay.h
    Created: 20 Jan 2017 12:01:24pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LEAFDELAY_H_INCLUDED
#define LEAFDELAY_H_INCLUDED

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#include "leaf-globals.h"
#include "leaf-math.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* Non-interpolating delay, reimplemented from STK (Cook and Scavone). */
typedef struct _tDelay
{
    float gain;
    float* buff;
    
    float lastOut, lastIn;
    
    uint32_t inPoint, outPoint;
    
    uint32_t delay, maxDelay;
    
} tDelay;

void        tDelay_init         (tDelay*  const, uint32_t delay, uint32_t maxDelay);
void        tDelay_free         (tDelay* const);

int         tDelay_setDelay     (tDelay*  const, uint32_t delay);
uint32_t    tDelay_getDelay     (tDelay*  const);
void        tDelay_tapIn        (tDelay*  const, float in, uint32_t tapDelay);
float       tDelay_tapOut       (tDelay*  const, uint32_t tapDelay);
float       tDelay_addTo        (tDelay*  const, float value, uint32_t tapDelay);
float       tDelay_tick         (tDelay*  const, float sample);
float       tDelay_getLastOut   (tDelay*  const);
float       tDelay_getLastIn    (tDelay*  const);


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* Linearly-interpolating delay, reimplemented from STK (Cook and Scavone). */
typedef struct _tDelayL
{
    float gain;
    float* buff;
    
    float lastOut, lastIn;
    
    uint32_t inPoint, outPoint;
    
    uint32_t maxDelay;
    
    float delay;
    
    float alpha, omAlpha;
    
} tDelayL;

void        tDelayL_init        (tDelayL*  const, float delay, uint32_t maxDelay);
void        tDelayL_free        (tDelayL* const);

int         tDelayL_setDelay    (tDelayL*  const, float delay);
float       tDelayL_getDelay    (tDelayL*  const);
void        tDelayL_tapIn       (tDelayL*  const, float in, uint32_t tapDelay);
float       tDelayL_tapOut      (tDelayL*  const, uint32_t tapDelay);
float       tDelayL_addTo       (tDelayL*  const, float value, uint32_t tapDelay);
float       tDelayL_tick        (tDelayL*  const, float sample);
float       tDelayL_getLastOut  (tDelayL*  const);
float       tDelayL_getLastIn   (tDelayL*  const);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* Allpass-interpolating delay, reimplemented from STK (Cook and Scavone). */
typedef struct _tDelayA
{
    float gain;
    float* buff;
    
    float lastOut, lastIn;
    
    uint32_t inPoint, outPoint;
    
    uint32_t maxDelay;
    
    float delay;
    
    float alpha, omAlpha, coeff;
    
    float apInput;
    
} tDelayA;

void        tDelayA_init        (tDelayA*  const, float delay, uint32_t maxDelay);
void        tDelayA_free        (tDelayA* const);

int         tDelayA_setDelay    (tDelayA*  const, float delay);
float       tDelayA_getDelay    (tDelayA*  const);
void        tDelayA_tapIn       (tDelayA*  const, float in, uint32_t tapDelay);
float       tDelayA_tapOut      (tDelayA*  const, uint32_t tapDelay);
float       tDelayA_addTo       (tDelayA*  const, float value, uint32_t tapDelay);
float       tDelayA_tick        (tDelayA*  const, float sample);
float       tDelayA_getLastOut  (tDelayA*  const);
float       tDelayA_getLastIn   (tDelayA*  const);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#endif  // LEAFDELAY_H_INCLUDED
