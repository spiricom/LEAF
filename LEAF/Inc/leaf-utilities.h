/*==============================================================================

    leaf-utilities.h
    Created: 20 Jan 2017 12:02:17pm
    Author:  Michael R Mulshine

==============================================================================*/

#ifndef LEAF_UTILITIES_H_INCLUDED
#define LEAF_UTILITIES_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================

#include "leaf-globals.h"
#include "leaf-math.h"

//==============================================================================

// STACK implementation (fixed size)
#define STACK_SIZE 128
typedef struct _tStack
{
    int data[STACK_SIZE];
    uint16_t pos;
    uint16_t size;
    uint16_t capacity;
    oBool ordered;
    
} tStack;

void    tStack_init                 (tStack* const);
void    tStack_free                 (tStack* const);

void    tStack_setCapacity          (tStack* const, uint16_t cap);
int     tStack_addIfNotAlreadyThere (tStack* const, uint16_t item);
void    tStack_add                  (tStack* const, uint16_t item);
int     tStack_remove               (tStack* const, uint16_t item);
void    tStack_clear                (tStack* const);
int     tStack_first                (tStack* const);
int     tStack_getSize              (tStack* const);
int     tStack_contains             (tStack* const, uint16_t item);
int     tStack_next                 (tStack* const);
int     tStack_get                  (tStack* const, int which);

//==============================================================================

/* Ramp */
typedef struct _tRamp {
    float inc;
    float inv_sr_ms;
    float minimum_time;
    float curr,dest;
    float time;
    int samples_per_tick;
    
} tRamp;

void    tRamp_init      (tRamp* const, float time, int samplesPerTick);
void    tRamp_free      (tRamp* const);

float   tRamp_tick      (tRamp* const);
float   tRamp_sample    (tRamp* const);
int     tRamp_setTime   (tRamp* const, float time);
int     tRamp_setDest   (tRamp* const, float dest);
int     tRamp_setVal    (tRamp* const, float val);

//==============================================================================

/* Compressor */
typedef struct _tCompressor
{
    float tauAttack, tauRelease;
    float T, R, W, M; // Threshold, compression Ratio, decibel Width of knee transition, decibel Make-up gain
    
    float x_G[2], y_G[2], x_T[2], y_T[2];
    
    oBool isActive;

}tCompressor;

void    tCompressor_init    (tCompressor* const);
void    tCompressor_free    (tCompressor* const);
float   tCompressor_tick    (tCompressor* const, float input);

//==============================================================================

/* Attack-Decay envelope */
typedef struct _tEnvelope {
    
    const float *exp_buff;
    const float *inc_buff;
    uint32_t buff_size;
    
    float next;
    
    float attackInc, decayInc, rampInc;
    
    oBool inAttack, inDecay, inRamp;
    
    oBool loop;
    
    float gain, rampPeak;
    
    float attackPhase, decayPhase, rampPhase;
    
} tEnvelope;

void    tEnvelope_init      (tEnvelope* const, float attack, float decay, oBool loop);
void    tEnvelope_free      (tEnvelope* const);

float   tEnvelope_tick      (tEnvelope* const);
int     tEnvelope_setAttack (tEnvelope*  const, float attack);
int     tEnvelope_setDecay  (tEnvelope*  const, float decay);
int     tEnvelope_loop      (tEnvelope*  const, oBool loop);
int     tEnvelope_on        (tEnvelope*  const, float velocity);

//==============================================================================

/* ADSR */
typedef struct _tADSR
{
    const float *exp_buff;
    const float *inc_buff;
    uint32_t buff_size;
    
    float next;
    
    float attackInc, decayInc, releaseInc, rampInc;
    
    oBool inAttack, inDecay, inSustain, inRelease, inRamp;
    
    float sustain, gain, rampPeak, releasePeak;
    
    float attackPhase, decayPhase, releasePhase, rampPhase;

} tADSR;

void    tADSR_init      (tADSR*  const, float attack, float decay, float sustain, float release);
void    tADSR_free      (tADSR*  const);

float   tADSR_tick      (tADSR*  const);
int     tADSR_setAttack (tADSR*  const, float attack);
int     tADSR_setDecay  (tADSR*  const, float decay);
int     tADSR_setSustain(tADSR*  const, float sustain);
int     tADSR_setRelease(tADSR*  const, float release);
int     tADSR_on        (tADSR*  const, float velocity);
int     tADSR_off       (tADSR*  const);

//==============================================================================

/* Envelope Follower */
typedef struct _tEnvelopeFollower
{
    float y;
    float a_thresh;
    float d_coeff;
    
} tEnvelopeFollower;

void    tEnvelopeFollower_init           (tEnvelopeFollower*  const, float attackThreshold, float decayCoeff);
void    tEnvelopeFollower_free           (tEnvelopeFollower*  const);

float   tEnvelopeFollower_tick           (tEnvelopeFollower*  const, float x);
int     tEnvelopeFollower_decayCoeff     (tEnvelopeFollower*  const, float decayCoeff);
int     tEnvelopeFollower_attackThresh   (tEnvelopeFollower*  const, float attackThresh);

//==============================================================================

/* tAtkDtk */
#define DEFBLOCKSIZE 1024
#define DEFTHRESHOLD 6
#define DEFATTACK    10
#define DEFRELEASE    10

typedef struct _tAtkDtk
{
    float env;
    
    //Attack & Release times in msec
    int atk;
    int rel;
    
    //Attack & Release coefficients based on times
    float atk_coeff;
    float rel_coeff;
    
    int blocksize;
    int samplerate;
    
    //RMS amplitude of previous block - used to decide if attack is present
    float prevAmp;
    
    float threshold;
} tAtkDtk;

void    tAtkDtk_init            (tAtkDtk* const, int blocksize);
void    tAtkDtk_init_expanded   (tAtkDtk* const, int blocksize, int atk, int rel);
void    tAtkDtk_free            (tAtkDtk* const);

// set expected input blocksize
void    tAtkDtk_setBlocksize    (tAtkDtk* const, int size);

// change atkDetector sample rate
void    tAtkDtk_setSamplerate   (tAtkDtk* const, int inRate);

// set attack time and coeff
void    tAtkDtk_setAtk          (tAtkDtk* const, int inAtk);

// set release time and coeff
void    tAtkDtk_setRel          (tAtkDtk* const, int inRel);

// set level above which values are identified as attacks
void    tAtkDtk_setThreshold    (tAtkDtk* const, float thres);

// find largest transient in input block, return index of attack
int     tAtkDtk_detect          (tAtkDtk* const, float *in);

//==============================================================================

// ENV~ from PD, modified for LEAF
#define MAXOVERLAP 32
#define INITVSTAKEN 64

typedef struct _tEnv
{
    float buf[ENV_WINDOW_SIZE + INITVSTAKEN];
    uint16_t x_phase;                    /* number of points since last output */
    uint16_t x_period;                   /* requested period of output */
    uint16_t x_realperiod;               /* period rounded up to vecsize multiple */
    uint16_t x_npoints;                  /* analysis window size in samples */
    float x_result;                 /* result to output */
    float x_sumbuf[MAXOVERLAP];     /* summing buffer */
    float x_f;
    uint16_t windowSize, hopSize, blockSize;
    uint16_t x_allocforvs;               /* extra buffer for DSP vector size */
} tEnv;

void    tEnv_init           (tEnv* const, int windowSize, int hopSize, int blockSize);
void    tEnv_free           (tEnv* const);
float   tEnv_tick           (tEnv* const);
void    tEnv_processBlock   (tEnv* const, float* in);

//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_UTILITIES_H_INCLUDED

//==============================================================================


