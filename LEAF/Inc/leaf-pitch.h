/*
  ==============================================================================

    leaf-pitch.h
    Created: 30 Nov 2018 11:03:13am
    Author:  airship

  ==============================================================================
*/

#ifndef LEAF_PITCH_H_INCLUDED
#define LEAF_PITCH_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================

#include "leaf-globals.h"
#include "leaf-math.h"

#include "leaf-filter.h"
#include "leaf-utilities.h"
#include "../leaf.h"
//==============================================================================
    
#define DEFPITCHRATIO 2.0f
#define DEFTIMECONSTANT 100.0f
#define DEFHOPSIZE 64
#define DEFWINDOWSIZE 64
#define FBA 20
#define HPFREQ 40.0f


//==============================================================================
    
// Pitch shifter 
typedef struct _tPitchShifter
{
    tEnv env;
    tSNAC snac;
    tSOLAD sola;
    tHighpass hp;
    
    float* inBuffer;
    float* outBuffer;
    int frameSize;
    int bufSize;
    int framesPerBuffer;
    int curBlock;
    int lastBlock;
    int index;
    
    uint16_t hopSize;
    uint16_t windowSize;
    uint8_t fba;
    
    float pitchFactor;
    float timeConstant;
    float radius;
    float max;
    float lastmax;
    float deltamax;
    
} tPitchShifter;

void        tPitchShifter_init              (tPitchShifter* const, float* in, float* out, int bufSize, int frameSize);
void        tPitchShifter_free              (tPitchShifter* const);

float       tPitchShifter_tick              (tPitchShifter* const, float sample);
float       tPitchShifterToFreq_tick        (tPitchShifter* const, float sample, float freq);
float       tPitchShifterToFunc_tick        (tPitchShifter* const, float sample, float (*fun)(float));
void        tPitchShifter_ioSamples_toFreq  (tPitchShifter* const, float* in, float* out, int size, float toFreq);
void        tPitchShifter_ioSamples_toPeriod(tPitchShifter* const, float* in, float* out, int size, float toPeriod);
void        tPitchShifter_ioSamples_toFunc  (tPitchShifter* const, float* in, float* out, int size, float (*fun)(float));
void        tPitchShifter_setPitchFactor    (tPitchShifter* const, float pf);
void        tPitchShifter_setTimeConstant   (tPitchShifter* const, float tc);
void        tPitchShifter_setHopSize        (tPitchShifter* const, int hs);
void        tPitchShifter_setWindowSize     (tPitchShifter* const, int ws);
float       tPitchShifter_getPeriod         (tPitchShifter* const);

//==============================================================================
    
// Period detection
typedef struct _tPeriod
{
    tEnv env;
    tSNAC snac;
    float* inBuffer;
    float* outBuffer;
    int frameSize;
    int bufSize;
    int framesPerBuffer;
    int curBlock;
    int lastBlock;
    int i;
    int indexstore;
    int iLast;
    int index;
    float period;
    
    uint16_t hopSize;
    uint16_t windowSize;
    uint8_t fba;
    
    float timeConstant;
    float radius;
    float max;
    float lastmax;
    float deltamax;
    
}tPeriod;

void        tPeriod_init                    (tPeriod* const, float* in, float* out, int bufSize, int frameSize);
void        tPeriod_free                    (tPeriod* const);

float       tPeriod_findPeriod              (tPeriod* const, float sample);
void        tPeriod_setHopSize              (tPeriod* p, int hs);
void        tPeriod_setWindowSize           (tPeriod* p, int ws);

//==============================================================================
    
// Pitch shift
typedef struct _tPitchShift
{
    tSOLAD sola;
    tHighpass hp;
    tPeriod* p;
    
    float* outBuffer;
    int frameSize;
    int bufSize;
    
    int framesPerBuffer;
    int curBlock;
    int lastBlock;
    int index;
    
    float pitchFactor;
    float timeConstant;
    float radius;
} tPitchShift;

void        tPitchShift_init                (tPitchShift* const, tPeriod* const, float* out, int bufSize);
void        tPitchShift_free                (tPitchShift* const);

float       tPitchShift_shift               (tPitchShift* const);
float       tPitchShift_shiftToFunc         (tPitchShift* const, float (*fun)(float));
float       tPitchShift_shiftToFreq         (tPitchShift* const, float freq);
void        tPitchShift_setPitchFactor      (tPitchShift* const, float pf);

//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_PITCH_H_INCLUDED

//==============================================================================
