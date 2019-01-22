/*
  ==============================================================================

    leaf-pitch.h
    Created: 30 Nov 2018 11:03:13am
    Author:  airship

  ==============================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#include "leaf-globals.h"
#include "leaf-math.h"

#include "leaf-filter.h"
#include "leaf-utilities.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#define DEFPITCHRATIO 2.0f
#define DEFTIMECONSTANT 100.0f
#define DEFHOPSIZE 64
#define DEFWINDOWSIZE 64
#define FBA 20
#define HPFREQ 40.0f

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* tSOLAD : pitch shifting algorithm that underlies tPitchShifter etc */
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
    
    float delaybuf[LOOPSIZE+16];
    
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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

// tSNAC: period detector
#define SNAC_FRAME_SIZE 1024           // default analysis framesize // should be the same as (or smaller than?) PS_FRAME_SIZE
#define DEFOVERLAP 1                // default overlap
#define DEFBIAS 0.2f        // default bias
#define DEFMINRMS 0.003f   // default minimum RMS
#define SEEK 0.85f       // seek-length as ratio of framesize

typedef struct _tSNAC
{
    float inputbuf[SNAC_FRAME_SIZE];
    float processbuf[SNAC_FRAME_SIZE * 2];
    float spectrumbuf[SNAC_FRAME_SIZE / 2];
    float biasbuf[SNAC_FRAME_SIZE];
    
    uint16_t timeindex;
    uint16_t framesize;
    uint16_t overlap;
    uint16_t periodindex;
    
    float periodlength;
    float fidelity;
    float biasfactor;
    float minrms;
    
} tSNAC;

void    tSNAC_init          (tSNAC* const, int overlaparg);    // constructor
void    tSNAC_free          (tSNAC* const);    // destructor

void    tSNAC_ioSamples     (tSNAC *s, float *in, float *out, int size);
void    tSNAC_setOverlap    (tSNAC *s, int lap);
void    tSNAC_setBias       (tSNAC *s, float bias);
void    tSNAC_setMinRMS     (tSNAC *s, float rms);

/*To get freq, perform SAMPLE_RATE/snac_getperiod() */
float   tSNAC_getPeriod     (tSNAC *s);
float   tSNAC_getfidelity   (tSNAC *s);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
#ifdef __cplusplus
}
#endif
