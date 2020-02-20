/*==============================================================================
 
 leaf-analysis.h
 Created: 25 Oct 2019 10:30:52am
 Author:  Matthew Wang
 
 ==============================================================================*/

#ifndef LEAF_ANALYSIS_H_INCLUDED
#define LEAF_ANALYSIS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-global.h"
#include "leaf-mempool.h"
#include "leaf-math.h"
#include "leaf-filters.h"
#include "leaf-envelopes.h"
    
    //==============================================================================
    
    /* Envelope Follower */
    typedef struct _tEnvelopeFollower
    {
        float y;
        float a_thresh;
        float d_coeff;
        
    } _tEnvelopeFollower;
    
    typedef _tEnvelopeFollower* tEnvelopeFollower;
    
    void    tEnvelopeFollower_init          (tEnvelopeFollower* const, float attackThreshold, float decayCoeff);
    void    tEnvelopeFollower_free          (tEnvelopeFollower* const);
    void    tEnvelopeFollower_initToPool    (tEnvelopeFollower* const, float attackThreshold, float decayCoeff, tMempool* const);
    void    tEnvelopeFollower_freeFromPool  (tEnvelopeFollower* const, tMempool* const);
    
    float   tEnvelopeFollower_tick          (tEnvelopeFollower* const, float x);
    int     tEnvelopeFollower_decayCoeff    (tEnvelopeFollower* const, float decayCoeff);
    int     tEnvelopeFollower_attackThresh  (tEnvelopeFollower* const, float attackThresh);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /* PowerEnvelopeFollower */
    typedef struct _tPowerFollower {
        float factor, oneminusfactor;
        float curr;
        
    } _tPowerFollower;
    
    typedef _tPowerFollower* tPowerFollower;
    
    void    tPowerFollower_init         (tPowerFollower* const, float factor);
    void    tPowerFollower_free         (tPowerFollower* const);
    void    tPowerFollower_initToPool   (tPowerFollower* const, float factor, tMempool* const);
    void    tPowerFollower_freeFromPool (tPowerFollower* const, tMempool* const);
    
    float   tPowerFollower_tick         (tPowerFollower* const, float input);
    float   tPowerFollower_sample       (tPowerFollower* const);
    int     tPowerFollower_setFactor    (tPowerFollower* const, float factor);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    // ENV~ from PD, modified for LEAF
#define MAXOVERLAP 32
#define INITVSTAKEN 64
#define ENV_WINDOW_SIZE 1024
#define ENV_HOP_SIZE 256
    
    typedef struct _tEnvPD
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
    } _tEnvPD;
    
    typedef _tEnvPD* tEnvPD;
    
    void    tEnvPD_init             (tEnvPD* const, int windowSize, int hopSize, int blockSize);
    void    tEnvPD_free             (tEnvPD* const);
    void    tEnvPD_initToPool       (tEnvPD* const, int windowSize, int hopSize, int blockSize, tMempool* const);
    void    tEnvPD_freeFromPool     (tEnvPD* const, tMempool* const);
    
    float   tEnvPD_tick             (tEnvPD* const);
    void    tEnvPD_processBlock     (tEnvPD* const, float* in);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /* tAttackDetection */
#define DEFBLOCKSIZE 1024
#define DEFTHRESHOLD 6
#define DEFATTACK    10
#define DEFRELEASE    10
    
    typedef struct _tAttackDetection
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
    } _tAttackDetection;
    
    typedef _tAttackDetection* tAttackDetection;
    
    void    tAttackDetection_init           (tAttackDetection* const, int blocksize, int atk, int rel);
    void    tAttackDetection_free           (tAttackDetection* const);
    void    tAttackDetection_initToPool     (tAttackDetection* const, int blocksize, int atk, int rel, tMempool* const);
    void    tAttackDetection_freeFromPool   (tAttackDetection* const, tMempool* const);
    
    // set expected input blocksize
    void    tAttackDetection_setBlocksize   (tAttackDetection* const, int size);
    
    // change atkDetector sample rate
    void    tAttackDetection_setSamplerate  (tAttackDetection* const, int inRate);
    
    // set attack time and coeff
    void    tAttackDetection_setAttack      (tAttackDetection* const, int inAtk);
    
    // set release time and coeff
    void    tAttackDetection_setRelease     (tAttackDetection* const, int inRel);
    
    // set level above which values are identified as attacks
    void    tAttackDetection_setThreshold   (tAttackDetection* const, float thres);
    
    // find largest transient in input block, return index of attack
    int     tAttackDetection_detect         (tAttackDetection* const, float *in);
    
    //==============================================================================
    
    // tSNAC: period detector
#define SNAC_FRAME_SIZE 1024           // default analysis framesize // should be the same as (or smaller than?) PS_FRAME_SIZE
#define DEFOVERLAP 1                // default overlap
#define DEFBIAS 0.2f        // default bias
#define DEFMINRMS 0.003f   // default minimum RMS
#define SEEK 0.85f       // seek-length as ratio of framesize
    
    typedef struct _tSNAC
    {
        float* inputbuf;
        float* processbuf;
        float* spectrumbuf;
        float* biasbuf;
        uint16_t timeindex;
        uint16_t framesize;
        uint16_t overlap;
        uint16_t periodindex;
        
        float periodlength;
        float fidelity;
        float biasfactor;
        float minrms;
        
    } _tSNAC;
    
    typedef _tSNAC* tSNAC;
    
    void    tSNAC_init          (tSNAC* const, int overlaparg);
    void    tSNAC_free          (tSNAC* const);
    void    tSNAC_initToPool    (tSNAC* const, int overlaparg, tMempool* const);
    void    tSNAC_freeFromPool  (tSNAC* const, tMempool* const);
    
    void    tSNAC_ioSamples     (tSNAC *s, float *in, float *out, int size);
    void    tSNAC_setOverlap    (tSNAC *s, int lap);
    void    tSNAC_setBias       (tSNAC *s, float bias);
    void    tSNAC_setMinRMS     (tSNAC *s, float rms);
    
    /*To get freq, perform SAMPLE_RATE/snac_getperiod() */
    float   tSNAC_getPeriod     (tSNAC *s);
    float   tSNAC_getFidelity   (tSNAC *s);
    
#define DEFPITCHRATIO 2.0f
#define DEFTIMECONSTANT 100.0f
#define DEFHOPSIZE 64
#define DEFWINDOWSIZE 64
#define FBA 20
#define HPFREQ 40.0f
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    // Period detection
    typedef struct _tPeriodDetection
    {
        tEnvPD env;
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
        float smoothPeriod;
        
        uint16_t hopSize;
        uint16_t windowSize;
        uint8_t fba;
    
        tExpSmooth smooth;
        
        float timeConstant;
        float radius;
        float max;
        float lastmax;
        float deltamax;
        
    } _tPeriodDetection;
    
    typedef _tPeriodDetection* tPeriodDetection;
    
    void    tPeriodDetection_init               (tPeriodDetection* const, float* in, float* out, int bufSize, int frameSize);
    void    tPeriodDetection_free               (tPeriodDetection* const);
    void    tPeriodDetection_initToPool         (tPeriodDetection* const, float* in, float* out, int bufSize, int frameSize, tMempool* const);
    void    tPeriodDetection_freeFromPool       (tPeriodDetection* const, tMempool* const);
    
    float   tPeriodDetection_tick               (tPeriodDetection* const, float sample);
    float   tPeriodDetection_getPeriod          (tPeriodDetection* const);
    void    tPeriodDetection_setSmoothAmount    (tPeriodDetection* const, float amount);
    void    tPeriodDetection_setHopSize         (tPeriodDetection* p, int hs);
    void    tPeriodDetection_setWindowSize      (tPeriodDetection* p, int ws);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_ANALYSIS_H_INCLUDED

//==============================================================================

