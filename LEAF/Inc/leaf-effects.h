/*==============================================================================

    leaf-effects.h
    Created: 20 Jan 2017 12:01:54pm
    Author:  Michael R Mulshine

==============================================================================*/

#ifndef LEAF_EFFECTS_H_INCLUDED
#define LEAF_EFFECTS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================

#include "leaf-global.h"
#include "leaf-math.h"
#include "leaf-analysis.h"

//==============================================================================

/* tTalkbox */
#define NUM_TALKBOX_PARAM 4

typedef struct _tTalkbox
{
    float param[NUM_TALKBOX_PARAM];
    
    int bufsize;
    float* car0;
    float* car1;
    float* window;
    float* buf0;
    float* buf1;
    
    float emphasis;
    int32_t K, N, O, pos;
    float wet, dry, FX;
    float d0, d1, d2, d3, d4;
    float u0, u1, u2, u3, u4;
    
} tTalkbox;

void        tTalkbox_init        (tTalkbox* const, int bufsize);
void        tTalkbox_free        (tTalkbox* const);
float       tTalkbox_tick        (tTalkbox* const, float synth, float voice);
void        tTalkbox_update      (tTalkbox* const);
void        tTalkbox_suspend     (tTalkbox* const);
void        tTalkbox_lpcDurbin   (float *r, int p, float *k, float *g);
void        tTalkbox_lpc         (float *buf, float *car, int32_t n, int32_t o);
void		tTalkbox_setQuality  (tTalkbox* const, float quality);

//==============================================================================

/* tVocoder */
#define NUM_VOCODER_PARAM 8
#define NBANDS 16

typedef struct _tVocoder
{
    float param[NUM_VOCODER_PARAM];
    
    float gain;         //output level
    float thru, high;   //hf thru
    float kout;         //downsampled output
    int32_t  kval;      //downsample counter
    int32_t  nbnd;      //number of bands
    
    //filter coeffs and buffers - seems it's faster to leave this global than make local copy
    float f[NBANDS][13]; //[0-8][0 1 2 | 0 1 2 3 | 0 1 2 3 | val rate]
    
} tVocoder;

void        tVocoder_init        (tVocoder* const);
void        tVocoder_free        (tVocoder* const);
float       tVocoder_tick        (tVocoder* const, float synth, float voice);
void        tVocoder_update      (tVocoder* const);
void        tVocoder_suspend     (tVocoder* const);
    
//==============================================================================


//==============================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* tSOLAD : pitch shifting algorithm that underlies tRetune etc */
#define LOOPSIZE (2048*2)      // (4096*2) // loop size must be power of two
#define LOOPMASK (LOOPSIZE - 1)
#define PITCHFACTORDEFAULT 1.0f
#define INITPERIOD 64.0f
#define MAXPERIOD (float)((LOOPSIZE - w->blocksize) * 0.8f)
#define MINPERIOD 8.0f

typedef struct _tSOLAD
{
    uint16_t timeindex;              // current reference time, write index
    uint16_t blocksize;              // signal input / output block size
    float pitchfactor;        // pitch factor between 0.25 and 4
    float readlag;            // read pointer's lag behind write pointer
    float period;             // period length in input signal
    float jump;               // read pointer jump length and direction
    float xfadelength;        // crossfade length expressed at input sample rate
    float xfadevalue;         // crossfade phase and value
    
    float* delaybuf;
    
} tSOLAD;

void    tSOLAD_init             (tSOLAD* const);
void    tSOLAD_free             (tSOLAD* const);
// send one block of input samples, receive one block of output samples
void    tSOLAD_ioSamples        (tSOLAD *w, float* in, float* out, int blocksize);
// set periodicity analysis data
void    tSOLAD_setPeriod        (tSOLAD *w, float period);
// set pitch factor between 0.25 and 4
void    tSOLAD_setPitchFactor   (tSOLAD *w, float pitchfactor);
// force readpointer lag
void    tSOLAD_setReadLag       (tSOLAD *w, float readlag);
// reset state variables
void    tSOLAD_resetState       (tSOLAD *w);
    
// Pitch shift
typedef struct _tPitchShift
{
    tSOLAD sola;
    tHighpass hp;
    tPeriodDetection* p;
    
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

void        tPitchShift_init                (tPitchShift* const, tPeriodDetection* const, float* out, int bufSize);
void        tPitchShift_free                (tPitchShift* const);
float       tPitchShift_shift               (tPitchShift* const);
float       tPitchShift_shiftToFunc         (tPitchShift* const, float (*fun)(float));
float       tPitchShift_shiftToFreq         (tPitchShift* const, float freq);
void        tPitchShift_setPitchFactor      (tPitchShift* const, float pf);
    
// Retune
typedef struct _tRetune
{
    tPeriodDetection pd;
    tPitchShift* ps;
    
    float* inBuffer;
    float** outBuffers;
    float* tickOutput;
    int frameSize;
    int bufSize;
    
    uint16_t hopSize;
    uint16_t windowSize;
    uint8_t fba;
    
    float* pitchFactor;
    float timeConstant;
    float radius;
    
    float inputPeriod;
    
    int numVoices;
} tRetune;

void        tRetune_init                (tRetune* const, int numVoices, int bufSize, int frameSize);
void        tRetune_free                (tRetune* const);

float*      tRetune_tick                (tRetune* const, float sample);
void        tRetune_setNumVoices        (tRetune* const, int numVoices);
void        tRetune_setPitchFactors     (tRetune* const, float pf);
void        tRetune_setPitchFactor      (tRetune* const, float pf, int voice);
void        tRetune_setTimeConstant     (tRetune* const, float tc);
void        tRetune_setHopSize          (tRetune* const, int hs);
void        tRetune_setWindowSize       (tRetune* const, int ws);
float       tRetune_getInputPeriod      (tRetune* const);
float       tRetune_getInputFreq        (tRetune* const);
    
// Autotune
typedef struct _tAutotune
{
    tPeriodDetection pd;
    tPitchShift* ps;
    
    float* inBuffer;
    float** outBuffers;
    float* tickOutput;
    int frameSize;
    int bufSize;
    
    uint16_t hopSize;
    uint16_t windowSize;
    uint8_t fba;
    
    float* freq;
    float timeConstant;
    float radius;
    
    float inputPeriod;
    
    int numVoices;
} tAutotune;

void        tAutotune_init                (tAutotune* const, int numVoices, int bufSize, int frameSize);
void        tAutotune_free                (tAutotune* const);

float*      tAutotune_tick                (tAutotune* const, float sample);
void        tAutotune_setNumVoices        (tAutotune* const, int numVoices);
void        tAutotune_setFreq             (tAutotune* const, float f, int voice);
void        tAutotune_setTimeConstant     (tAutotune* const, float tc);
void        tAutotune_setHopSize          (tAutotune* const, int hs);
void        tAutotune_setWindowSize       (tAutotune* const, int ws);
float       tAutotune_getInputPeriod      (tAutotune* const);
float       tAutotune_getInputFreq        (tAutotune* const);

//==============================================================================
    

#define FORD 7
#define FORMANT_BUFFER_SIZE 2048

typedef struct _tFormantShifter
{
    int ford;
    int bufsize;
    float falph;
    float flamb;
    float* fk;
    float* fb;
    float* fc;
    float* frb;
    float* frc;
    float* fsig;
    float* fsmooth;
    float fhp;
    float flp;
    float flpa;
    float** fbuff;
    float* ftvec;
    float fmute;
    float fmutealph;
    unsigned int cbi;
    
} tFormantShifter;

void        tFormantShifter_init            (tFormantShifter* const, int bufsize, int order);
void        tFormantShifter_free            (tFormantShifter* const);

float       tFormantShifter_tick            (tFormantShifter* const, float input, float fwarp);
float       tFormantShifter_remove          (tFormantShifter* const, float input);
float       tFormantShifter_add             (tFormantShifter* const, float input, float fwarp);
void        tFormantShifter_ioSamples       (tFormantShifter* const, float* in, float* out, int size, float fwarp);

//==============================================================================
    
#ifdef __cplusplus
}
#endif
        
#endif  // LEAF_EFFECTS_H_INCLUDED

//==============================================================================