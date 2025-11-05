/*==============================================================================
 
 leaf-effects.h
 
 
 ==============================================================================*/

#ifndef LEAF_EFFECTS_H_INCLUDED
#define LEAF_EFFECTS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
#include "leaf-global.h"
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-dynamics.h"
#include "leaf-analysis.h"
#include "leaf-envelopes.h"
    
    
    
    //==============================================================================
    
    /*!
     @defgroup ttalkbox tTalkbox
     @ingroup effects
     @brief High resolution vocoder from mda using Levinson-Durbin LPC algorithm.
     @{
     
     @fn void    tTalkbox_init(tTalkbox** const, int bufsize, LEAF* const leaf)
     @brief Initialize a tTalkbox to the default mempool of a LEAF instance.
     @param talkbox A pointer to the tTalkbox to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTalkbox_initToPool(tTalkbox** const, int bufsize, tMempool** const)
     @brief Initialize a tTalkbox to a specified mempool.
     @param talkbox A pointer to the tTalkbox to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTalkbox_free(tTalkbox** const)
     @brief Free a tTalkbox from its mempool.
     @param talkbox A pointer to the tTalkbox to free.
     
     @fn float   tTalkbox_tick           (tTalkbox* const, float synth, float voice)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn float     tTalkbox_tickFrozen        (tTalkbox* const voc, float synth, float voice)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void    tTalkbox_update         (tTalkbox* const)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void    tTalkbox_suspend        (tTalkbox* const)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void    tTalkbox_lpcDurbin      (float *r, int p, float *k, float *g)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_lpc            (float *buf, float *car, double* dl, double* Rt, int32_t n, int32_t o, float warp, int warpOn, float *k, int freeze, float *G)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void    tTalkbox_setQuality     (tTalkbox* const, float quality)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_setWarpFactor    (tTalkbox* const voc, float warp)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_setWarpOn        (tTalkbox* const voc, float warpOn)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_setFreeze        (tTalkbox* const voc, float freeze)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_warpedAutocorrelate    (float * x, double* dl, double* Rt, unsigned int L, float * R, unsigned int P, float lambda)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @} */
    
#define NUM_TALKBOX_PARAM 4
    
    typedef struct tTalkbox
    {

        tMempool* mempool;
        
        float param[NUM_TALKBOX_PARAM];
        
        int bufsize;
        float* car0;
        float* car1;
        float* window;
        float* buf0;
        float* buf1;
        float* k;
        float warpFactor;
        int32_t warpOn;
        int freeze;
        float emphasis;
        int32_t K, N, O, pos;
        float wet, dry, FX;
        float d0, d1, d2, d3, d4;
        float u0, u1, u2, u3, u4;
        float G;
        double* dl;
        double* Rt;
        
        float sampleRate;
        
    } tTalkbox;

    void    tTalkbox_init                (tTalkbox** const, int bufsize, LEAF* const leaf);
    void    tTalkbox_initToPool          (tTalkbox** const, int bufsize, tMempool** const);
    void    tTalkbox_free                (tTalkbox** const);

    float  tTalkbox_tick                (tTalkbox* const, float synth, float voice);
    float  tTalkbox_tickFrozen          (tTalkbox* const voc, float synth, float voice);

    void    tTalkbox_update              (tTalkbox* const);
    void    tTalkbox_suspend             (tTalkbox* const);
    void    tTalkbox_setSampleRate       (tTalkbox* const voc, float sr);
    void    tTalkbox_setQuality          (tTalkbox* const, float quality);
    void    tTalkbox_setWarpFactor       (tTalkbox* const voc, float warp);
    void    tTalkbox_setWarpOn           (tTalkbox*const voc, float warpOn);
    void    tTalkbox_setFreeze           (tTalkbox* const voc, float freeze);
    void    tTalkbox_lpcDurbin           (float *r, int p, float *k, float *g);
    void    tTalkbox_lpc                 (float *buf, float *car, double* dl, double* Rt, int32_t n, int32_t o, float warp, int warpOn, float *k, int freeze, float *G);
    void    tTalkbox_warpedAutocorrelate (float * x, double* dl, double* Rt, unsigned int L, float * R, unsigned int P, float lambda);


    //==============================================================================
    
    
    /*!
     @defgroup ttalkboxfloat tTalkboxfloat
     @ingroup effects
     @brief High resolution vocoder from mda using Levinson-Durbin LPC algorithm.
     @{
     
     @fn void    tTalkboxfloat_init(tTalkboxfloat** const, int bufsize, LEAF* const leaf)
     @brief Initialize a tTalkboxfloat to the default mempool of a LEAF instance.
     @param talkbox A pointer to the tTalkboxfloat to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTalkboxfloat_initToPool(tTalkboxfloat** const, int bufsize, tMempool** const)
     @brief Initialize a tTalkboxfloat to a specified mempool.
     @param talkbox A pointer to the tTalkboxfloat to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTalkboxfloat_free(tTalkboxfloat** const)
     @brief Free a tTalkboxfloat from its mempool.
     @param talkbox A pointer to the tTalkboxfloat to free.
     
     @fn float   tTalkboxfloat_tick           (tTalkboxfloat* const, float synth, float voice)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn float     tTalkboxfloat_tickFrozen        (tTalkboxfloat* const voc, float synth, float voice)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn void    tTalkboxfloat_update         (tTalkboxfloat* const)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn void    tTalkboxfloat_suspend        (tTalkboxfloat* const)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn void    tTalkboxfloat_lpcDurbin      (float *r, int p, float *k, float *g)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn void     tTalkboxfloat_lpc            (float *buf, float *car, float* dl, float* Rt, int32_t n, int32_t o, float warp, int warpOn, float *k, int freeze, float *G)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn void    tTalkboxfloat_setQuality     (tTalkboxfloat* const, float quality)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn void     tTalkboxfloat_setWarpFactor    (tTalkboxfloat* const voc, float warp)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn void     tTalkboxfloat_setWarpOn        (tTalkboxfloat* const voc, float warpOn)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn void     tTalkboxfloat_setFreeze        (tTalkboxfloat* const voc, float freeze)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @fn void     tTalkboxfloat_warpedAutocorrelate    (float * x, float* dl, float* Rt, unsigned int L, float * R, unsigned int P, float lambda)
     @brief
     @param talkbox A pointer to the relevant tTalkboxfloat.
     
     @} */
    
    typedef struct tTalkboxfloat
    {

        tMempool* mempool;
        
        float param[NUM_TALKBOX_PARAM];
        
        int bufsize;
        float* car0;
        float* car1;
        float* window;
        float* buf0;
        float* buf1;
        float* k;
        float warpFactor;
        int32_t warpOn;
        int freeze;
        float emphasis;
        int32_t K, N, O, pos;
        float wet, dry, FX;
        float d0, d1, d2, d3, d4;
        float u0, u1, u2, u3, u4;
        float G;
        float* dl;
        float* Rt;
        
        float sampleRate;
        
    } tTalkboxfloat;

    void    tTalkboxfloat_init                (tTalkboxfloat** const, int bufsize, LEAF* const leaf);
    void    tTalkboxfloat_initToPool          (tTalkboxfloat** const, int bufsize, tMempool** const);
    void    tTalkboxfloat_free                (tTalkboxfloat** const);
    
    float  tTalkboxfloat_tick                (tTalkboxfloat* const, float synth, float voice);
    float  tTalkboxfloat_tickFrozen          (tTalkboxfloat* const voc, float synth, float voice);

    void    tTalkboxfloat_update              (tTalkboxfloat* const);
    void    tTalkboxfloat_suspend             (tTalkboxfloat* const);
    void    tTalkboxfloat_setQuality          (tTalkboxfloat* const, float quality);
    void    tTalkboxfloat_setWarpFactor       (tTalkboxfloat* const voc, float warp);
    void    tTalkboxfloat_setWarpOn           (tTalkboxfloat* const voc, int warpOn);
    void    tTalkboxfloat_setFreeze           (tTalkboxfloat* const voc, int freeze);
    void    tTalkboxfloat_setSampleRate       (tTalkboxfloat* const voc, float sr);
    void    tTalkboxfloat_lpcDurbin           (float *r, int p, float *k, float *g);
    void    tTalkboxfloat_lpc                 (float *buf, float *car, float* dl, float* Rt, int32_t n, int32_t o, float warp, int warpOn, float *k, int freeze, float *G);
    void    tTalkboxfloat_warpedAutocorrelate (float * x, float* dl, float* Rt, unsigned int L, float * R, unsigned int P, float lambda);

    //==============================================================================
    
    
    /*!
     @defgroup tvocoder tVocoder
     @ingroup effects
     @brief Channel vocoder from mda.
     @{
     
     @fn void    tVocoder_init(tVocoder** const, LEAF* const leaf)
     @brief Initialize a tVocoder to the default mempool of a LEAF instance.
     @param vocoder A pointer to the tVocoder to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tVocoder_initToPool(tVocoder** const, tMempool** const)
     @brief Initialize a tVocoder to a specified mempool.
     @param vocoder A pointer to the tVocoder to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tVocoder_free(tVocoder** const)
     @brief Free a tVocoder from its mempool.
     @param vocoder A pointer to the tVocoder to free.
     
     @fn float   tVocoder_tick           (tVocoder* const, float synth, float voice)
     @brief
     @param vocoder A pointer to the relevant tVocoder.
     
     @fn void    tVocoder_update         (tVocoder* const)
     @brief
     @param vocoder A pointer to the relevant tVocoder.
     
     @fn void    tVocoder_suspend        (tVocoder* const)
     @brief
     @param vocoder A pointer to the relevant tVocoder.
     
     @} */
    
#define NUM_VOCODER_PARAM 8
#define NBANDS 16
    
    typedef struct tVocoder
    {

        tMempool* mempool;
        
        float param[NUM_VOCODER_PARAM];
        
        float gain;         //output level
        float thru, high;   //hf thru
        float kout;         //downsampled output
        int32_t  kval;      //downsample counter
        int32_t  nbnd;      //number of bands
        
        //filter coeffs and buffers - seems it's faster to leave this global than make local copy
        float f[NBANDS][13]; //[0-8][0 1 2 | 0 1 2 3 | 0 1 2 3 | val rate]
        
        float invSampleRate;
    } tVocoder;

    void    tVocoder_init           (tVocoder** const, LEAF* const leaf);
    void    tVocoder_initToPool     (tVocoder** const, tMempool** const);
    void    tVocoder_free           (tVocoder** const);
    
    float  tVocoder_tick           (tVocoder* const, float synth, float voice);

    void    tVocoder_update         (tVocoder* const);
    void    tVocoder_suspend        (tVocoder* const);
    void    tVocoder_setSampleRate  (tVocoder* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup trosenbergglottalpulse tRosenbergGlottalPulse
     @ingroup effects
     @brief Rosenberg glottal pulse approximator.
     @{
     
     @fn void    tRosenbergGlottalPulse_init(tRosenbergGlottalPulse** const, LEAF* const leaf)
     @brief Initialize a tRosenbergGlottalPulse to the default mempool of a LEAF instance.
     @param pulse A pointer to the tRosenbergGlottalPulse to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tRosenbergGlottalPulse_initToPool(tRosenbergGlottalPulse** const, tMempool** const)
     @brief Initialize a tRosenbergGlottalPulse to a specified mempool.
     @param pulse A pointer to the tRosenbergGlottalPulse to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tRosenbergGlottalPulse_free(tRosenbergGlottalPulse** const)
     @brief Free a tRosenbergGlottalPulse from its mempool.
     @param pulse A pointer to the tRosenbergGlottalPulse to free.
     
     @fn float   tRosenbergGlottalPulse_tick           (tRosenbergGlottalPulse* const)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn float   tRosenbergGlottalPulse_tickHQ           (tRosenbergGlottalPulse* const gp)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn void   tRosenbergGlottalPulse_setFreq           (tRosenbergGlottalPulse* const, float freq)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn void   tRosenbergGlottalPulse_setOpenLength           (tRosenbergGlottalPulse* const, float openLength)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn void   tRosenbergGlottalPulse_setPulseLength           (tRosenbergGlottalPulse* const, float pulseLength)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn void   tRosenbergGlottalPulse_setOpenLengthAndPulseLength           (tRosenbergGlottalPulse* const gp, float openLength, float pulseLength)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @} */
    
    typedef struct tRosenbergGlottalPulse
    {

        tMempool* mempool;
        float phase;
        float openLength;
        float pulseLength;
        float invPulseLengthMinusOpenLength;
        float freq;
        float inc;
        float invSampleRate;
    } tRosenbergGlottalPulse;

    void    tRosenbergGlottalPulse_init                        (tRosenbergGlottalPulse** const, LEAF* const leaf);
    void    tRosenbergGlottalPulse_initToPool                  (tRosenbergGlottalPulse** const, tMempool** const);
    void    tRosenbergGlottalPulse_free                        (tRosenbergGlottalPulse** const);

    float  tRosenbergGlottalPulse_tick                        (tRosenbergGlottalPulse* const);
    float  tRosenbergGlottalPulse_tickHQ                      (tRosenbergGlottalPulse* const gp);

    void    tRosenbergGlottalPulse_setFreq                     (tRosenbergGlottalPulse* const, float freq);
    void    tRosenbergGlottalPulse_setOpenLength               (tRosenbergGlottalPulse* const, float openLength);
    void    tRosenbergGlottalPulse_setPulseLength              (tRosenbergGlottalPulse* const, float pulseLength);
    void    tRosenbergGlottalPulse_setOpenLengthAndPulseLength (tRosenbergGlottalPulse* const, float openLength, float pulseLength);
    void    tRosenbergGlottalPulse_setSampleRate               (tRosenbergGlottalPulse* const, float sr);

    //==============================================================================
    
    /*!
     @defgroup tsolad tSOLAD
     @ingroup effects
     @brief pitch shifting algorithm that underlies tRetune etc from Katja Vetters http://www.katjaas.nl/pitchshiftlowlatency/pitchshiftlowlatency.html
     @{
     
     @fn void    tSOLAD_init(tSOLAD** const, LEAF* const leaf)
     @brief Initialize a tSOLAD to the default mempool of a LEAF instance.
     @param solad A pointer to the tSOLAD to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSOLAD_initToPool(tSOLAD** const, tMempool** const)
     @brief Initialize a tSOLAD to a specified mempool.
     @param solad A pointer to the tSOLAD to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSOLAD_free(tSOLAD** const)
     @brief Free a tSOLAD from its mempool.
     @param solad A pointer to the tSOLAD to free.
     
     @fn void    tSOLAD_ioSamples        (tSOLAD *w, float* in, float* out, int blocksize)
     @brief Send one block of input samples, receive one block of output samples
     @param solad A pointer to the relevant tSOLAD.
     
     @fn void    tSOLAD_setPeriod        (tSOLAD *w, float period)
     @brief Set periodicity analysis data
     @param solad A pointer to the relevant tSOLAD.
     
     @fn void    tSOLAD_setPitchFactor   (tSOLAD *w, float pitchfactor)
     @brief Set pitch factor between 0.25 and 4
     @param solad A pointer to the relevant tSOLAD.
     
     @fn void    tSOLAD_setReadLag       (tSOLAD *w, float readlag)
     @brief Force readpointer lag
     @param solad A pointer to the relevant tSOLAD.
     
     @fn void    tSOLAD_resetState       (tSOLAD *w)
     @brief Reset state variables
     @param solad A pointer to the relevant tSOLAD.
     
     @} */
    
    //#define LOOPSIZE (2048*2)      // (4096*2) // loop size must be power of two
    //#define LOOPMASK (LOOPSIZE - 1)
#define PITCHFACTORDEFAULT 1.0f
#define INITPERIOD 64.0f
    //#define MAXPERIOD (float)((LOOPSIZE - w->blocksize) * 0.8f)
#define MINPERIOD 8.0f
    
    typedef struct tSOLAD
    {
        tMempool* mempool;
        
        tAttackDetection*  ad;
        tHighpass*  hp;
        
        int loopSize;
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

    void    tSOLAD_init             (tSOLAD** const, int loopSize, LEAF* const leaf);
    void    tSOLAD_initToPool       (tSOLAD** const, int loopSize, tMempool** const);
    void    tSOLAD_free             (tSOLAD** const);
    
    // send one block of input samples, receive one block of output samples
    void    tSOLAD_ioSamples        (tSOLAD* w, float* in, float* out, int blocksize);
    // set periodicity analysis data
    void    tSOLAD_setPeriod        (tSOLAD* w, float period);
    // set pitch factor between 0.25 and 4
    void    tSOLAD_setPitchFactor   (tSOLAD* w, float pitchfactor);
    // force readpointer lag
    void    tSOLAD_setReadLag       (tSOLAD* w, float readlag);
    // reset state variables
    void    tSOLAD_resetState       (tSOLAD* w);
    void    tSOLAD_setSampleRate    (tSOLAD* const, float sr);
    
    /*!
     @defgroup tpitchshift tPitchShift
     @ingroup effects
     @brief SOLAD-based pitch shifter.
     @{
     
     @fn void    tPitchShift_init(tPitchShift** const, tPeriodDetection* const, float* out, int bufSize, LEAF* const leaf)
     @brief Initialize a tPitchShift to the default mempool of a LEAF instance.
     @param pitchshift A pointer to the tPitchShift to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPitchShift_initToPool(tPitchShift** const, tPeriodDetection* const, float* out, int bufSize, tMempool** const)
     @brief Initialize a tPitchShift to a specified mempool.
     @param pitchshift A pointer to the tPitchShift to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPitchShift_free(tPitchShift** const)
     @brief Free a tPitchShift from its mempool.
     @param pitchshift A pointer to the tPitchShift to free.
     
     @fn float   tPitchShift_shift           (tPitchShift* const)
     @brief
     @param pitchshift A pointer to the relevant tPitchShift.
     
     @fn float   tPitchShift_shiftToFunc     (tPitchShift* const, float (*fun)(float))
     @brief
     @param pitchshift A pointer to the relevant tPitchShift.
     
     @fn float   tPitchShift_shiftToFreq     (tPitchShift* const, float freq)
     @brief
     @param pitchshift A pointer to the relevant tPitchShift.
     
     @fn void    tPitchShift_setPitchFactor  (tPitchShift* const, float pf)
     @brief
     @param pitchshift A pointer to the relevant tPitchShift.
     
     @} */
    
    typedef struct tPitchShift
    {
        tMempool* mempool;
        
        tDualPitchDetector*  pd;
        tSOLAD*  sola;
        
        float* outBuffer;
        float* inBuffer;
        int bufSize;
        int index;
        
        float pickiness;
        
        float sampleRate;
    } tPitchShift;

    void    tPitchShift_init          (tPitchShift** const, tDualPitchDetector** const, int bufSize, LEAF* const leaf);
    void    tPitchShift_initToPool    (tPitchShift** const, tDualPitchDetector** const, int bufSize, tMempool** const);
    void    tPitchShift_free          (tPitchShift** const);
    
    void    tPitchShift_shiftBy       (tPitchShift* const, float factor, float* in, float* out);
    void    tPitchShift_shiftTo       (tPitchShift* const, float freq, float* in, float* out);
    void    tPitchShift_setPickiness  (tPitchShift* const, float p);
    void    tPitchShift_setSampleRate (tPitchShift* const, float sr);
    
    /*!
     @defgroup tsimpleretune tSimpleRetune
     @ingroup effects
     @brief Wrapper for multiple pitch shifters with single-channel output.
     @{
     
     @fn void    tSimpleRetune_init(tSimpleRetune** const, int numVoices, int bufSize, int frameSize, LEAF* const leaf)
     @brief Initialize a tSimpleRetune to the default mempool of a LEAF instance.
     @param retune A pointer to the tSimpleRetune to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSimpleRetune_initToPool(tSimpleRetune** const, int numVoices, int bufSize, int frameSize, tMempool** const)
     @brief Initialize a tSimpleRetune to a specified mempool.
     @param retune A pointer to the tSimpleRetune to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSimpleRetune_free(tSimpleRetune** const)
     @brief Free a tSimpleRetune from its mempool.
     @param retune A pointer to the tSimpleRetune to free.
     
     @fn float*  tSimpleRetune_tick                  (tSimpleRetune* const, float sample)
     @brief
     @param retune A pointer to the relevant tSimpleRetune.
     
     @fn void    tSimpleRetune_setNumVoices          (tSimpleRetune* const, int numVoices)
     @brief
     @param retune A pointer to the relevant tSimpleRetune.
     
     @fn void    tSimpleRetune_tuneVoices              (tSimpleRetune* const, float f)
     @brief
     @param retune A pointer to the relevant tSimpleRetune.
     
     @fn void    tSimpleRetune_tuneVoice              (tSimpleRetune* const, float f, int voice)
     @brief
     @param retune A pointer to the relevant tSimpleRetune.
     
     @} */
    
    typedef struct tSimpleRetune
    {
        tMempool* mempool;
        
        tDualPitchDetector* dp;
        float minInputFreq, maxInputFreq;
        
        tPitchShift** ps;
        
        float* pdBuffer;
        float* inBuffer;
        float* outBuffer;
        int bufSize;
        int index;
        
        void (*shiftFunction)(tPitchShift* const, float, float*, float*);
        
        float* shiftValues;
        int numVoices;
    } tSimpleRetune;

    void    tSimpleRetune_init                  (tSimpleRetune** const, int numVoices, float minInputFreq, float maxInputFreq, int bufSize, LEAF* const leaf);
    void    tSimpleRetune_initToPool            (tSimpleRetune** const, int numVoices, float minInputFreq, float maxInputFreq, int bufSize, tMempool** const);
    void    tSimpleRetune_free                  (tSimpleRetune** const);
    
    float  tSimpleRetune_tick                  (tSimpleRetune* const, float sample);

    void    tSimpleRetune_setMode               (tSimpleRetune* const, int mode);
    void    tSimpleRetune_setNumVoices          (tSimpleRetune* const, int numVoices);
    void    tSimpleRetune_setPickiness          (tSimpleRetune* const, float p);
    void    tSimpleRetune_tuneVoices            (tSimpleRetune* const, float* t);
    void    tSimpleRetune_tuneVoice             (tSimpleRetune* const, int voice, float t);
    float  tSimpleRetune_getInputFrequency     (tSimpleRetune* const);
    void    tSimpleRetune_setSampleRate         (tSimpleRetune* const, float sr);

    /*!
     @defgroup tretune tRetune
     @ingroup effects
     @brief Wrapper for multiple pitch shifters with multi-channel output.
     @{
     
     @fn void    tRetune_init(tRetune** const, int numVoices, int bufSize, int frameSize, LEAF* const leaf)
     @brief Initialize a tRetune to the default mempool of a LEAF instance.
     @param retune A pointer to the tRetune to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tRetune_initToPool(tRetune** const, int numVoices, int bufSize, int frameSize, tMempool** const)
     @brief Initialize a tRetune to a specified mempool.
     @param retune A pointer to the tRetune to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tRetune_free(tRetune** const)
     @brief Free a tRetune from its mempool.
     @param retune A pointer to the tRetune to free.
     
     @fn float*  tRetune_tick                (tRetune* const, float sample)
     @brief
     @param retune A pointer to the relevant tRetune.
     
     @fn void    tRetune_setNumVoices        (tRetune* const, int numVoices)
     @brief
     @param retune A pointer to the relevant tRetune.
     
     @fn void    tRetune_setPitchFactors     (tRetune* const, float pf)
     @brief
     @param retune A pointer to the relevant tRetune.
     
     @fn void    tRetune_setPitchFactor      (tRetune* const, float pf, int voice)
     @brief
     @param retune A pointer to the relevant tRetune.
     
     @} */
    
    typedef struct tRetune
    {
        tMempool* mempool;
        
        tDualPitchDetector** dp;
        float minInputFreq, maxInputFreq;
        
        tPitchShift** ps;
        
        float* pdBuffer;
        float* inBuffer;
        float** outBuffers;
        int bufSize;
        int index;
        
        float* output;
        
        void (*shiftFunction)(tPitchShift* const, float, float*, float*);
        
        float* shiftValues;
        int numVoices;
    } tRetune;

    void    tRetune_init                (tRetune** const, int numVoices, float minInputFreq, float maxInputFreq,  int bufSize, LEAF* const leaf);
    void    tRetune_initToPool          (tRetune** const,  int numVoices, float minInputFreq, float maxInputFreq, int bufSize, tMempool** const);
    void    tRetune_free                (tRetune** const);
    
    float* tRetune_tick                (tRetune* const, float sample);

    void    tRetune_setMode             (tRetune* const, int mode);
    void    tRetune_setNumVoices        (tRetune* const, int numVoices);
    void    tRetune_setPickiness        (tRetune* const, float p);
    void    tRetune_tuneVoices          (tRetune* const, float* t);
    void    tRetune_tuneVoice           (tRetune* const, int voice, float t);
    float  tRetune_getInputFrequency   (tRetune* const);
    void    tRetune_setSampleRate       (tRetune* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tformantshifter tFormantShifter
     @ingroup effects
     @brief Formant remover and adder, allowing for formant shifting.
     @{
     
     @fn void    tFormantShifter_init(tFormantShifter** const, int order, LEAF* const leaf)
     @brief Initialize a tFormantShifter to the default mempool of a LEAF instance.
     @param formant A pointer to the tFormantShifter to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tFormantShifter_initToPool(tFormantShifter** const, int order, tMempool** const)
     @brief Initialize a tFormantShifter to a specified mempool.
     @param formant A pointer to the tFormantShifter to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tFormantShifter_free(tFormantShifter** const)
     @brief Free a tFormantShifter from its mempool.
     @param formant A pointer to the tFormantShifter to free.
     
     @fn float   tFormantShifter_tick            (tFormantShifter* const, float input)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn float   tFormantShifter_remove          (tFormantShifter* const, float input)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn float   tFormantShifter_add             (tFormantShifter* const, float input)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn void    tFormantShifter_ioSamples       (tFormantShifter* const, float* in, float* out, int size, float fwarp)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn void    tFormantShifter_setShiftFactor  (tFormantShifter* const, float shiftFactor)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn void    tFormantShifter_setIntensity    (tFormantShifter* const, float intensity)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @} */
    
    typedef struct tFormantShifter
    {

        tMempool* mempool;
        int ford;
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
        float* fbuff;
        float* ftvec;
        float fmute;
        float fmutealph;
        unsigned int cbi;
        float shiftFactor;
        float intensity, invIntensity;
        tHighpass* hp;
        tHighpass* hp2;
        tFeedbackLeveler* fbl1;
        tFeedbackLeveler* fbl2;
        float sampleRate;
        float invSampleRate;
    } tFormantShifter;

    void    tFormantShifter_init            (tFormantShifter** const, int order, LEAF* const leaf);
    void    tFormantShifter_initToPool      (tFormantShifter** const, int order, tMempool** const);
    void    tFormantShifter_free            (tFormantShifter** const);
    
    float  tFormantShifter_tick            (tFormantShifter* const, float input);

    float  tFormantShifter_remove          (tFormantShifter* const, float input);
    float  tFormantShifter_add             (tFormantShifter* const, float input);
    void    tFormantShifter_ioSamples       (tFormantShifter* const, float* in, float* out, int size, float fwarp);
    void    tFormantShifter_setShiftFactor  (tFormantShifter* const, float shiftFactor);
    void    tFormantShifter_setIntensity    (tFormantShifter* const, float intensity);
    void    tFormantShifter_setSampleRate   (tFormantShifter* const fsr, float sr);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_EFFECTS_H_INCLUDED

//==============================================================================

