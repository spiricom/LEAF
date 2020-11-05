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
     @brief
     @{
     
     @fn void    tTalkbox_init           (tTalkbox* const, int bufsize, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tTalkbox_initToPool     (tTalkbox* const, int bufsize, tMempool* const)
     @brief
     @param
     
     @fn void    tTalkbox_free           (tTalkbox* const)
     @brief
     @param
     
     @fn float   tTalkbox_tick           (tTalkbox* const, float synth, float voice)
     @brief
     @param
     
     @fn float     tTalkbox_tickFrozen        (tTalkbox* const voc, float synth, float voice)
     @brief
     @param
     
     @fn void    tTalkbox_update         (tTalkbox* const)
     @brief
     @param
     
     @fn void    tTalkbox_suspend        (tTalkbox* const)
     @brief
     @param
     
     @fn void    tTalkbox_lpcDurbin      (float *r, int p, float *k, float *g)
     @brief
     @param
     
     @fn void     tTalkbox_lpc            (float *buf, float *car, double* dl, double* Rt, int32_t n, int32_t o, float warp, int warpOn, float *k, int freeze, float *G)
     @brief
     @param
     
     @fn void    tTalkbox_setQuality     (tTalkbox* const, float quality)
     @brief
     @param
     
     @fn void     tTalkbox_setWarpFactor    (tTalkbox* const voc, float warp)
     @brief
     @param
     
     @fn void     tTalkbox_setWarpOn        (tTalkbox* const voc, float warpOn)
     @brief
     @param
     
     @fn void     tTalkbox_setFreeze        (tTalkbox* const voc, float freeze)
     @brief
     @param
     
     @fn void     tTalkbox_warpedAutocorrelate    (float * x, double* dl, double* Rt, unsigned int L, float * R, unsigned int P, float lambda)
     @brief
     @param
     
     @} */
     
#define NUM_TALKBOX_PARAM 4
    
    typedef struct _tTalkbox
    {
        
        tMempool mempool;
        
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

        
    } _tTalkbox;
    
    typedef _tTalkbox* tTalkbox;
    
    void    tTalkbox_init           (tTalkbox* const, int bufsize, LEAF* const leaf);
    void    tTalkbox_initToPool     (tTalkbox* const, int bufsize, tMempool* const);
    void    tTalkbox_free           (tTalkbox* const);
    
    float   tTalkbox_tick           (tTalkbox* const, float synth, float voice);
    float   tTalkbox_tickFrozen     (tTalkbox* const voc, float synth, float voice);
    void    tTalkbox_update         (tTalkbox* const);
    void    tTalkbox_suspend        (tTalkbox* const);
    void    tTalkbox_lpcDurbin      (float *r, int p, float *k, float *g);
    void    tTalkbox_lpc            (float *buf, float *car, double* dl, double* Rt, int32_t n, int32_t o, float warp, int warpOn, float *k, int freeze, float *G);
    void    tTalkbox_setQuality     (tTalkbox* const, float quality);
    void    tTalkbox_setWarpFactor  (tTalkbox* const voc, float warp);
    void    tTalkbox_setWarpOn      (tTalkbox* const voc, float warpOn);
    void    tTalkbox_setFreeze      (tTalkbox* const voc, float freeze);
    void    tTalkbox_warpedAutocorrelate    (float * x, double* dl, double* Rt, unsigned int L, float * R, unsigned int P, float lambda);
    
    
    //==============================================================================
    

    /*!
     @defgroup ttalkboxfloat tTalkboxFloat
     @ingroup effects
     @brief
     @{
     
     @fn void    tTalkboxFloat_init           (tTalkboxFloat* const, int bufsize, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tTalkboxFloat_initToPool     (tTalkboxFloat* const, int bufsize, tMempool* const)
     @brief
     @param
     
     @fn void    tTalkboxFloat_free           (tTalkboxFloat* const)
     @brief
     @param
     
     @fn float   tTalkboxFloat_tick           (tTalkboxFloat* const, float synth, float voice)
     @brief
     @param
     
     @fn float     tTalkboxFloat_tickFrozen        (tTalkboxFloat* const voc, float synth, float voice)
     @brief
     @param
     
     @fn void    tTalkboxFloat_update         (tTalkboxFloat* const)
     @brief
     @param
     
     @fn void    tTalkboxFloat_suspend        (tTalkboxFloat* const)
     @brief
     @param
     
     @fn void    tTalkboxFloat_lpcDurbin      (float *r, int p, float *k, float *g)
     @brief
     @param
     
     @fn void     tTalkboxFloat_lpc            (float *buf, float *car, float* dl, float* Rt, int32_t n, int32_t o, float warp, int warpOn, float *k, int freeze, float *G)
     @brief
     @param
     
     @fn void    tTalkboxFloat_setQuality     (tTalkboxFloat* const, float quality)
     @brief
     @param
     
     @fn void     tTalkboxFloat_setWarpFactor    (tTalkboxFloat* const voc, float warp)
     @brief
     @param
     
     @fn void     tTalkboxFloat_setWarpOn        (tTalkboxFloat* const voc, float warpOn)
     @brief
     @param
     
     @fn void     tTalkboxFloat_setFreeze        (tTalkboxFloat* const voc, float freeze)
     @brief
     @param
     
     @fn void     tTalkboxFloat_warpedAutocorrelate    (float * x, float* dl, float* Rt, unsigned int L, float * R, unsigned int P, float lambda)
     @brief
     @param
     
     @} */

    typedef struct _tTalkboxFloat
      {
            
          tMempool mempool;
          
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

      } _tTalkboxFloat;

      typedef _tTalkboxFloat* tTalkboxFloat;

      void    tTalkboxFloat_init           (tTalkboxFloat* const, int bufsize, LEAF* const leaf);
      void    tTalkboxFloat_initToPool     (tTalkboxFloat* const, int bufsize, tMempool* const);
      void    tTalkboxFloat_free           (tTalkboxFloat* const);

      float   tTalkboxFloat_tick           (tTalkboxFloat* const, float synth, float voice);
      float     tTalkboxFloat_tickFrozen        (tTalkboxFloat* const voc, float synth, float voice);
      void    tTalkboxFloat_update         (tTalkboxFloat* const);
      void    tTalkboxFloat_suspend        (tTalkboxFloat* const);
      void    tTalkboxFloat_lpcDurbin      (float *r, int p, float *k, float *g);
      void  tTalkboxFloat_lpc           (float *buf, float *car, float* dl, float* Rt, int32_t n, int32_t o, float warp, int warpOn, float *k, int freeze, float *G);
      void    tTalkboxFloat_setQuality     (tTalkboxFloat* const, float quality);
      void  tTalkboxFloat_setWarpFactor (tTalkboxFloat* const voc, float warp);
      void  tTalkboxFloat_setWarpOn     (tTalkboxFloat* const voc, int warpOn);
      void  tTalkboxFloat_setFreeze     (tTalkboxFloat* const voc, int freeze);
      void  tTalkboxFloat_warpedAutocorrelate   (float * x, float* dl, float* Rt, unsigned int L, float * R, unsigned int P, float lambda);
      //==============================================================================


    /*!
     @defgroup tvocoder tVocoder
     @ingroup effects
     @brief
     @{
     
     @fn void    tVocoder_init           (tVocoder* const, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tVocoder_initToPool     (tVocoder* const, tMempool* const)
     @brief
     @param
     
     @fn void    tVocoder_free           (tVocoder* const)
     @brief
     @param
     
     @fn float   tVocoder_tick           (tVocoder* const, float synth, float voice)
     @brief
     @param
     
     @fn void    tVocoder_update         (tVocoder* const)
     @brief
     @param
     
     @fn void    tVocoder_suspend        (tVocoder* const)
     @brief
     @param
     
     @} */
    
#define NUM_VOCODER_PARAM 8
#define NBANDS 16
    
    typedef struct _tVocoder
    {
        
        tMempool mempool;
        
        float param[NUM_VOCODER_PARAM];
        
        float gain;         //output level
        float thru, high;   //hf thru
        float kout;         //downsampled output
        int32_t  kval;      //downsample counter
        int32_t  nbnd;      //number of bands
        
        //filter coeffs and buffers - seems it's faster to leave this global than make local copy
        float f[NBANDS][13]; //[0-8][0 1 2 | 0 1 2 3 | 0 1 2 3 | val rate]
        
    } _tVocoder;
    
    typedef _tVocoder* tVocoder;
    
    void    tVocoder_init           (tVocoder* const, LEAF* const leaf);
    void    tVocoder_initToPool     (tVocoder* const, tMempool* const);
    void    tVocoder_free           (tVocoder* const);
    
    float   tVocoder_tick           (tVocoder* const, float synth, float voice);
    void    tVocoder_update         (tVocoder* const);
    void    tVocoder_suspend        (tVocoder* const);
    
    //==============================================================================
    
    /*!
     @defgroup trosenbergglottalpulse tRosenbergGlottalPulse
     @ingroup effects
     @brief
     @{
     
     @fn void    tRosenbergGlottalPulse_init           (tRosenbergGlottalPulse* const, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tRosenbergGlottalPulse_initToPool     (tRosenbergGlottalPulse* const, tMempool* const)
     @brief
     @param
     
     @fn void    tRosenbergGlottalPulse_free           (tRosenbergGlottalPulse* const)
     @brief
     @param
     
     @fn float   tRosenbergGlottalPulse_tick           (tRosenbergGlottalPulse* const)
     @brief
     @param
     
     @fn float   tRosenbergGlottalPulse_tickHQ           (tRosenbergGlottalPulse* const gp)
     @brief
     @param
     
     @fn void   tRosenbergGlottalPulse_setFreq           (tRosenbergGlottalPulse* const, float freq)
     @brief
     @param
     
     @fn void   tRosenbergGlottalPulse_setOpenLength           (tRosenbergGlottalPulse* const, float openLength)
     @brief
     @param
     
     @fn void   tRosenbergGlottalPulse_setPulseLength           (tRosenbergGlottalPulse* const, float pulseLength)
     @brief
     @param
     
     @fn void   tRosenbergGlottalPulse_setOpenLengthAndPulseLength           (tRosenbergGlottalPulse* const gp, float openLength, float pulseLength)
     @brief
     @param
     
     @} */

    typedef struct _tRosenbergGlottalPulse
    {
        
        tMempool mempool;
        float phase;
        float openLength;
        float pulseLength;
        float invPulseLengthMinusOpenLength;
        float freq;
        float inc;
    } _tRosenbergGlottalPulse;

    typedef _tRosenbergGlottalPulse* tRosenbergGlottalPulse;

    void    tRosenbergGlottalPulse_init           (tRosenbergGlottalPulse* const, LEAF* const leaf);
    void    tRosenbergGlottalPulse_initToPool     (tRosenbergGlottalPulse* const, tMempool* const);
    void    tRosenbergGlottalPulse_free           (tRosenbergGlottalPulse* const);

    float   tRosenbergGlottalPulse_tick           (tRosenbergGlottalPulse* const);
    float   tRosenbergGlottalPulse_tickHQ           (tRosenbergGlottalPulse* const gp);
    void   tRosenbergGlottalPulse_setFreq           (tRosenbergGlottalPulse* const, float freq);

    void   tRosenbergGlottalPulse_setOpenLength           (tRosenbergGlottalPulse* const, float openLength);

    void   tRosenbergGlottalPulse_setPulseLength           (tRosenbergGlottalPulse* const, float pulseLength);
    void   tRosenbergGlottalPulse_setOpenLengthAndPulseLength           (tRosenbergGlottalPulse* const gp, float openLength, float pulseLength);
    //==============================================================================
    
    /*!
     @defgroup tsolad tSOLAD
     @ingroup effects
     @brief pitch shifting algorithm that underlies tRetune etc from Katja Vetters http://www.katjaas.nl/pitchshiftlowlatency/pitchshiftlowlatency.html
     @{
     
     @fn void    tSOLAD_init             (tSOLAD* const, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tSOLAD_initToPool       (tSOLAD* const, tMempool* const)
     @brief
     @param
     
     @fn void    tSOLAD_free             (tSOLAD* const)
     @brief
     @param
     
     @fn void    tSOLAD_ioSamples        (tSOLAD *w, float* in, float* out, int blocksize)
     @brief send one block of input samples, receive one block of output samples
     @param

     @fn void    tSOLAD_setPeriod        (tSOLAD *w, float period)
     @brief set periodicity analysis data
     @param
     
     @fn void    tSOLAD_setPitchFactor   (tSOLAD *w, float pitchfactor)
     @brief set pitch factor between 0.25 and 4
     @param
     
     @fn void    tSOLAD_setReadLag       (tSOLAD *w, float readlag)
     @brief force readpointer lag
     @param
     
     @fn void    tSOLAD_resetState       (tSOLAD *w)
     @brief reset state variables
     @param
     
     @} */
    
#define LOOPSIZE (2048*2)      // (4096*2) // loop size must be power of two
#define LOOPMASK (LOOPSIZE - 1)
#define PITCHFACTORDEFAULT 1.0f
#define INITPERIOD 64.0f
#define MAXPERIOD (float)((LOOPSIZE - w->blocksize) * 0.8f)
#define MINPERIOD 8.0f
    
    typedef struct _tSOLAD
    {
        tMempool mempool;
        
        tAttackDetection ad;
        tHighpass hp;
        
        uint16_t timeindex;              // current reference time, write index
        uint16_t blocksize;              // signal input / output block size
        float pitchfactor;        // pitch factor between 0.25 and 4
        float readlag;            // read pointer's lag behind write pointer
        float period;             // period length in input signal
        float jump;               // read pointer jump length and direction
        float xfadelength;        // crossfade length expressed at input sample rate
        float xfadevalue;         // crossfade phase and value
        
        float* delaybuf;
        
    } _tSOLAD;
    
    typedef _tSOLAD* tSOLAD;
    
    void    tSOLAD_init             (tSOLAD* const, LEAF* const leaf);
    void    tSOLAD_initToPool       (tSOLAD* const, tMempool* const);
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
    
    /*!
     @defgroup tpitchshift tPitchShift
     @ingroup effects
     @brief
     @{
     
     @fn void    tPitchShift_init            (tPitchShift* const, tPeriodDetection* const, float* out, int bufSize, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tPitchShift_initToPool      (tPitchShift* const, tPeriodDetection* const, float* out, int bufSize, tMempool* const)
     @brief
     @param
     
     @fn void    tPitchShift_free            (tPitchShift* const)
     @brief
     @param
     
     @fn float   tPitchShift_shift           (tPitchShift* const)
     @brief
     @param
     
     @fn float   tPitchShift_shiftToFunc     (tPitchShift* const, float (*fun)(float))
     @brief
     @param
     
     @fn float   tPitchShift_shiftToFreq     (tPitchShift* const, float freq)
     @brief
     @param
     
     @fn void    tPitchShift_setPitchFactor  (tPitchShift* const, float pf)
     @brief
     @param
     
     @} */
    
    typedef struct _tPitchShift
    {
        tMempool mempool;
        
        _tDualPitchDetector* pd;
        tSOLAD sola;
        
        float* outBuffer;
        float* inBuffer;
        int bufSize;
        int index;
    } _tPitchShift;
    
    typedef _tPitchShift* tPitchShift;
    
    void    tPitchShift_init (tPitchShift* const, tDualPitchDetector* const, LEAF* const leaf);
    void    tPitchShift_initToPool (tPitchShift* const, tDualPitchDetector* const, tMempool* const);
    void    tPitchShift_free (tPitchShift* const);
    
    void    tPitchShift_shiftBy (tPitchShift* const, float factor, float* in, float* out, int bufSize);
    void    tPitchShift_shiftTo (tPitchShift* const, float freq, float* in, float* out, int bufSize);
    
    /*!
     @defgroup tretune tRetune
     @ingroup effects
     @brief
     @{
     
     @fn void    tRetune_init                (tRetune* const, int numVoices, int bufSize, int frameSize, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tRetune_initToPool          (tRetune* const, int numVoices, int bufSize, int frameSize, tMempool* const)
     @brief
     @param
     
     @fn void    tRetune_free                (tRetune* const)
     @brief
     @param
     
     @fn float*  tRetune_tick                (tRetune* const, float sample)
     @brief
     @param
     
     @fn void    tRetune_setNumVoices        (tRetune* const, int numVoices)
     @brief
     @param
     
     @fn void    tRetune_setPitchFactors     (tRetune* const, float pf)
     @brief
     @param
     
     @fn void    tRetune_setPitchFactor      (tRetune* const, float pf, int voice)
     @brief
     @param
     
     @} */
    
    typedef struct _tRetune
    {
        tMempool mempool;
        
        tDualPitchDetector dp;
        float minInputFreq, maxInputFreq;
        
        tPitchShift* ps;
        
        float* inBuffer;
        float* outBuffer;
        float* lastOutBuffer;
        int bufSize;
        int index;
        
        float* pitchFactors;
        int numVoices;
    } _tRetune;
    
    typedef _tRetune* tRetune;
    
    void    tRetune_init                (tRetune* const, int numVoices, float minInputFreq, float maxInputFreq,  int bufSize, LEAF* const leaf);
    void    tRetune_initToPool          (tRetune* const,  int numVoices, float minInputFreq, float maxInputFreq, int bufSize, tMempool* const);
    void    tRetune_free                (tRetune* const);
    
    float   tRetune_tick                (tRetune* const, float sample);
    void    tRetune_setNumVoices        (tRetune* const, int numVoices);
    void    tRetune_setPitchFactors     (tRetune* const, float pf);
    void    tRetune_setPitchFactor      (tRetune* const, float pf, int voice);
    
    /*!
     @defgroup tautotune tAutotune
     @ingroup effects
     @brief
     @{
     
     @fn void    tAutotune_init                  (tAutotune* const, int numVoices, int bufSize, int frameSize, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tAutotune_initToPool            (tAutotune* const, int numVoices, int bufSize, int frameSize, tMempool* const)
     @brief
     @param
     
     @fn void    tAutotune_free                  (tAutotune* const)
     @brief
     @param
     
     @fn float*  tAutotune_tick                  (tAutotune* const, float sample)
     @brief
     @param
     
     @fn void    tAutotune_setNumVoices          (tAutotune* const, int numVoices)
     @brief
     @param
     
     @fn void    tAutotune_setFreqs              (tAutotune* const, float f)
     @brief
     @param
     
     @fn void    tAutotune_setFreq               (tAutotune* const, float f, int voice)
     @brief
     @param
     
     @} */
    
    typedef struct _tAutotune
    {
        tMempool mempool;
        
        tDualPitchDetector dp;
        float minInputFreq, maxInputFreq;
        
        tPitchShift* ps;
        
        float* inBuffer;
        float* outBuffer;
        float* lastOutBuffer;
        int bufSize;
        int index;
        
        float* freqs;
        int numVoices;
    } _tAutotune;
    
    typedef _tAutotune* tAutotune;
    
    void    tAutotune_init                  (tAutotune* const, int numVoices, float minInputFreq, float maxInputFreq, int bufSize, LEAF* const leaf);
    void    tAutotune_initToPool            (tAutotune* const, int numVoices, float minInputFreq, float maxInputFreq, int bufSize, tMempool* const);
    void    tAutotune_free                  (tAutotune* const);
    
    float   tAutotune_tick                  (tAutotune* const, float sample);
    void    tAutotune_setNumVoices          (tAutotune* const, int numVoices);
    void    tAutotune_setFreqs              (tAutotune* const, float f);
    void    tAutotune_setFreq               (tAutotune* const, float f, int voice);
    
    //==============================================================================
    
    /*!
     @defgroup tformantshifter tFormantShifter
     @ingroup effects
     @brief
     @{
     
     @fn void    tFormantShifter_init            (tFormantShifter* const, int order, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tFormantShifter_initToPool      (tFormantShifter* const, int order, tMempool* const)
     @brief
     @param
     
     @fn void    tFormantShifter_free            (tFormantShifter* const)
     @brief
     @param
     
     @fn float   tFormantShifter_tick            (tFormantShifter* const, float input)
     @brief
     @param
     
     @fn float   tFormantShifter_remove          (tFormantShifter* const, float input)
     @brief
     @param
     
     @fn float   tFormantShifter_add             (tFormantShifter* const, float input)
     @brief
     @param
     
     @fn void    tFormantShifter_ioSamples       (tFormantShifter* const, float* in, float* out, int size, float fwarp)
     @brief
     @param
     
     @fn void    tFormantShifter_setShiftFactor  (tFormantShifter* const, float shiftFactor)
     @brief
     @param
     
     @fn void    tFormantShifter_setIntensity    (tFormantShifter* const, float intensity)
     @brief
     @param
     
     @} */
    
    typedef struct _tFormantShifter
    {
        
        tMempool mempool;
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
        tHighpass hp;
        tHighpass hp2;
        tFeedbackLeveler fbl1;
        tFeedbackLeveler fbl2;
    } _tFormantShifter;
    
    typedef _tFormantShifter* tFormantShifter;
    
    void    tFormantShifter_init            (tFormantShifter* const, int order, LEAF* const leaf);
    void    tFormantShifter_initToPool      (tFormantShifter* const, int order, tMempool* const);
    void    tFormantShifter_free            (tFormantShifter* const);
    
    float   tFormantShifter_tick            (tFormantShifter* const, float input);
    float   tFormantShifter_remove          (tFormantShifter* const, float input);
    float   tFormantShifter_add             (tFormantShifter* const, float input);
    void    tFormantShifter_ioSamples       (tFormantShifter* const, float* in, float* out, int size, float fwarp);
    void    tFormantShifter_setShiftFactor  (tFormantShifter* const, float shiftFactor);
    void    tFormantShifter_setIntensity    (tFormantShifter* const, float intensity);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_EFFECTS_H_INCLUDED

//==============================================================================
