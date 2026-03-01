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

     The talkbox splits the input into carrier and modulator signals, computes LPC coefficients via the Levinson-Durbin algorithm on hte modulator,
     and filters the carrier through that spectral envvelope to produce the "talking instrument" effect

     Example
    @code{.c}
    //initialize
    tTalkbox* tb = NULL;
    tTalkbox_init(&tb,
                  1024,   //analysis buffer size (samples)
                  leaf);

    //aydio loop
    for (int i = 0; i < numSamples; ++i) {
        float vocoded = tTalkbox_tick(tb,
                                      synthSamples[i],   // carrier input
                                       voiceSamples[i]);  //modulator input
            outputSamples[i] = vocoded;
        }

    // freeze the LPC envelope to hold a vowel tone
    tTalkbox_setFreeze(tb, 1.0f);

    // change LPC warp factor for formant shifting
    tTalkbox_setWarpFactor(tb, 0.8f);

    // resume live tracking
    tTalkbox_setFreeze(tb, 0.0f);

    //when done
    tTalkbox_free(&tb);
    @endcode
     
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
     
     @fn Lfloat   tTalkbox_tick           (tTalkbox* const, Lfloat synth, Lfloat voice)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn Lfloat     tTalkbox_tickFrozen        (tTalkbox* const voc, Lfloat synth, Lfloat voice)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void    tTalkbox_update         (tTalkbox* const)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void    tTalkbox_suspend        (tTalkbox* const)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void    tTalkbox_lpcDurbin      (Lfloat *r, int p, Lfloat *k, Lfloat *g)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_lpc            (Lfloat *buf, Lfloat *car, double* dl, double* Rt, int32_t n, int32_t o, Lfloat warp, int warpOn, Lfloat *k, int freeze, Lfloat *G)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void    tTalkbox_setQuality     (tTalkbox* const, Lfloat quality)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_setWarpFactor    (tTalkbox* const voc, Lfloat warp)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_setWarpOn        (tTalkbox* const voc, Lfloat warpOn)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_setFreeze        (tTalkbox* const voc, Lfloat freeze)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @fn void     tTalkbox_warpedAutocorrelate    (Lfloat * x, double* dl, double* Rt, unsigned int L, Lfloat * R, unsigned int P, Lfloat lambda)
     @brief
     @param talkbox A pointer to the relevant tTalkbox.
     
     @} */
    
#define NUM_TALKBOX_PARAM 4
    
    typedef struct tTalkbox
    {

        tMempool* mempool;
        
        Lfloat param[NUM_TALKBOX_PARAM];
        
        int bufsize;
        Lfloat* car0;
        Lfloat* car1;
        Lfloat* window;
        Lfloat* buf0;
        Lfloat* buf1;
        Lfloat* k;
        Lfloat warpFactor;
        int32_t warpOn;
        int freeze;
        Lfloat emphasis;
        int32_t K, N, O, pos;
        Lfloat wet, dry, FX;
        Lfloat d0, d1, d2, d3, d4;
        Lfloat u0, u1, u2, u3, u4;
        Lfloat G;
        double* dl;
        double* Rt;
        
        Lfloat sampleRate;
        
    } tTalkbox;

    void    tTalkbox_init                (tTalkbox** const, int bufsize, LEAF* const leaf);
    void    tTalkbox_initToPool          (tTalkbox** const, int bufsize, tMempool** const);
    void    tTalkbox_free                (tTalkbox** const);

    Lfloat  tTalkbox_tick                (tTalkbox* const, Lfloat synth, Lfloat voice);
    Lfloat  tTalkbox_tickFrozen          (tTalkbox* const voc, Lfloat synth, Lfloat voice);

    void    tTalkbox_update              (tTalkbox* const);
    void    tTalkbox_suspend             (tTalkbox* const);
    void    tTalkbox_setSampleRate       (tTalkbox* const voc, Lfloat sr);
    void    tTalkbox_setQuality          (tTalkbox* const, Lfloat quality);
    void    tTalkbox_setWarpFactor       (tTalkbox* const voc, Lfloat warp);
    void    tTalkbox_setWarpOn           (tTalkbox*const voc, Lfloat warpOn);
    void    tTalkbox_setFreeze           (tTalkbox* const voc, Lfloat freeze);
    void    tTalkbox_lpcDurbin           (Lfloat *r, int p, Lfloat *k, Lfloat *g);
    void    tTalkbox_lpc                 (Lfloat *buf, Lfloat *car, double* dl, double* Rt, int32_t n, int32_t o, Lfloat warp, int warpOn, Lfloat *k, int freeze, Lfloat *G);
    void    tTalkbox_warpedAutocorrelate (Lfloat * x, double* dl, double* Rt, unsigned int L, Lfloat * R, unsigned int P, Lfloat lambda);


    //==============================================================================
    
    
    /*!
     @defgroup ttalkboxLfloat tTalkboxLfloat
     @ingroup effects
     @brief High resolution vocoder from mda using Levinson-Durbin LPC algorithm.

    Example
    @code{.c}
    tTalkboxLfloat* tb = NULL;
    tTalkboxLfloat_init(&tb,
                        1024,       //analysis buffer size
                        leaf);

    //audio loop
    for (int i = 0; i < numSamples; ++i){
        float out = tTalkboxLfloat_tick(tb,
                                        synthSamples[i],
                                        voiceSamples[i]);
        outputSamples[i] = out;
    }

    //freezing/resuming the envelope
    tTalkboxLfloat_setFreeze(tb, 1);
    tTalkboxLfloat_setFreeze(tb, 0);

    //shifting formats
    tTalkboxLfloat_setWarpFactor(tb, 0.8f);

    //when done
    tTalkboxLfloat_free(&tb);
    @endcode
     
     @fn void    tTalkboxLfloat_init(tTalkboxLfloat** const, int bufsize, LEAF* const leaf)
     @brief Initialize a tTalkboxLfloat to the default mempool of a LEAF instance.
     @param talkbox A pointer to the tTalkboxLfloat to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTalkboxLfloat_initToPool(tTalkboxLfloat** const, int bufsize, tMempool** const)
     @brief Initialize a tTalkboxLfloat to a specified mempool.
     @param talkbox A pointer to the tTalkboxLfloat to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTalkboxLfloat_free(tTalkboxLfloat** const)
     @brief Free a tTalkboxLfloat from its mempool.
     @param talkbox A pointer to the tTalkboxLfloat to free.
     
     @fn Lfloat   tTalkboxLfloat_tick           (tTalkboxLfloat* const, Lfloat synth, Lfloat voice)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn Lfloat     tTalkboxLfloat_tickFrozen        (tTalkboxLfloat* const voc, Lfloat synth, Lfloat voice)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn void    tTalkboxLfloat_update         (tTalkboxLfloat* const)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn void    tTalkboxLfloat_suspend        (tTalkboxLfloat* const)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn void    tTalkboxLfloat_lpcDurbin      (Lfloat *r, int p, Lfloat *k, Lfloat *g)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn void     tTalkboxLfloat_lpc            (Lfloat *buf, Lfloat *car, Lfloat* dl, Lfloat* Rt, int32_t n, int32_t o, Lfloat warp, int warpOn, Lfloat *k, int freeze, Lfloat *G)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn void    tTalkboxLfloat_setQuality     (tTalkboxLfloat* const, Lfloat quality)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn void     tTalkboxLfloat_setWarpFactor    (tTalkboxLfloat* const voc, Lfloat warp)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn void     tTalkboxLfloat_setWarpOn        (tTalkboxLfloat* const voc, Lfloat warpOn)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn void     tTalkboxLfloat_setFreeze        (tTalkboxLfloat* const voc, Lfloat freeze)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @fn void     tTalkboxLfloat_warpedAutocorrelate    (Lfloat * x, Lfloat* dl, Lfloat* Rt, unsigned int L, Lfloat * R, unsigned int P, Lfloat lambda)
     @brief
     @param talkbox A pointer to the relevant tTalkboxLfloat.
     
     @} */
    
    typedef struct tTalkboxLfloat
    {

        tMempool* mempool;
        
        Lfloat param[NUM_TALKBOX_PARAM];
        
        int bufsize;
        Lfloat* car0;
        Lfloat* car1;
        Lfloat* window;
        Lfloat* buf0;
        Lfloat* buf1;
        Lfloat* k;
        Lfloat warpFactor;
        int32_t warpOn;
        int freeze;
        Lfloat emphasis;
        int32_t K, N, O, pos;
        Lfloat wet, dry, FX;
        Lfloat d0, d1, d2, d3, d4;
        Lfloat u0, u1, u2, u3, u4;
        Lfloat G;
        Lfloat* dl;
        Lfloat* Rt;
        
        Lfloat sampleRate;
        
    } tTalkboxLfloat;

    void    tTalkboxLfloat_init                (tTalkboxLfloat** const, int bufsize, LEAF* const leaf);
    void    tTalkboxLfloat_initToPool          (tTalkboxLfloat** const, int bufsize, tMempool** const);
    void    tTalkboxLfloat_free                (tTalkboxLfloat** const);
    
    Lfloat  tTalkboxLfloat_tick                (tTalkboxLfloat* const, Lfloat synth, Lfloat voice);
    Lfloat  tTalkboxLfloat_tickFrozen          (tTalkboxLfloat* const voc, Lfloat synth, Lfloat voice);

    void    tTalkboxLfloat_update              (tTalkboxLfloat* const);
    void    tTalkboxLfloat_suspend             (tTalkboxLfloat* const);
    void    tTalkboxLfloat_setQuality          (tTalkboxLfloat* const, Lfloat quality);
    void    tTalkboxLfloat_setWarpFactor       (tTalkboxLfloat* const voc, Lfloat warp);
    void    tTalkboxLfloat_setWarpOn           (tTalkboxLfloat* const voc, int warpOn);
    void    tTalkboxLfloat_setFreeze           (tTalkboxLfloat* const voc, int freeze);
    void    tTalkboxLfloat_setSampleRate       (tTalkboxLfloat* const voc, Lfloat sr);
    void    tTalkboxLfloat_lpcDurbin           (Lfloat *r, int p, Lfloat *k, Lfloat *g);
    void    tTalkboxLfloat_lpc                 (Lfloat *buf, Lfloat *car, Lfloat* dl, Lfloat* Rt, int32_t n, int32_t o, Lfloat warp, int warpOn, Lfloat *k, int freeze, Lfloat *G);
    void    tTalkboxLfloat_warpedAutocorrelate (Lfloat * x, Lfloat* dl, Lfloat* Rt, unsigned int L, Lfloat * R, unsigned int P, Lfloat lambda);

    //==============================================================================
    
    
    /*!
     @defgroup tvocoder tVocoder
     @ingroup effects
     @brief Channel vocoder from mda.

     Example
    @code{.c}
    //initialize
    tVocoder* voc = NULL;
    tVocoder_init(&voc, leaf);

    // in your audio loop:
    for (int i = 0; i < numSamples; ++i) {
        float vocoded = tVocoder_tick(voc,
                                    carrierSamples[i],  //carrier input
                                    voiceSamples[i]);   //modulator input
        outputSamples[i] = vocoded;
    }

    // suspend envelope tracking (holds current filters)
    tVocoder_suspend(voc);

    // resume live envelope updates:
    tVocoder_update(voc);

    //adjust sample rate for correct band timing
    tVocoder_setSampleRate(voc, sampleRate);

    //when done:
    tVocoder_free(&voc);
    @endcode
     
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
     
     @fn Lfloat   tVocoder_tick           (tVocoder* const, Lfloat synth, Lfloat voice)
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
        
        Lfloat param[NUM_VOCODER_PARAM];
        
        Lfloat gain;         //output level
        Lfloat thru, high;   //hf thru
        Lfloat kout;         //downsampled output
        int32_t  kval;      //downsample counter
        int32_t  nbnd;      //number of bands
        
        //filter coeffs and buffers - seems it's faster to leave this global than make local copy
        Lfloat f[NBANDS][13]; //[0-8][0 1 2 | 0 1 2 3 | 0 1 2 3 | val rate]
        
        Lfloat invSampleRate;
    } tVocoder;

    void    tVocoder_init           (tVocoder** const, LEAF* const leaf);
    void    tVocoder_initToPool     (tVocoder** const, tMempool** const);
    void    tVocoder_free           (tVocoder** const);
    
    Lfloat  tVocoder_tick           (tVocoder* const, Lfloat synth, Lfloat voice);

    void    tVocoder_update         (tVocoder* const);
    void    tVocoder_suspend        (tVocoder* const);
    void    tVocoder_setSampleRate  (tVocoder* const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup trosenbergglottalpulse tRosenbergGlottalPulse
     @ingroup effects
     @brief Rosenberg glottal pulse approximator.

    The Rosenberg glottal pulse produced a simplified glottal excitation model by:
     1. Advancing a phase accumulator at a user set frequency
     2. Generating an opening slope over the first 'openLength' part of the cycle
     3. Genearting a closing slope over the next 'pulseLength' portion
     4. Holding zero for the remainder of the cycle

     Example
     @code{.c}
     //initialize
     tRosebBergGlottalPulse* gp = NULL;
     tRosenbergPulse_init(&gp, leaf);

     //set frequency of glottal pulse
     tRosenbergGlottalPulse_setFreq(gp, 120.0f);
     //set opening and close of cycle
     tRosenbergGlottalPulse_setOpenLengthAndPulseLength(gp, 0.5, 0.6f);
     //set sample rate for correct timing
     tRosenbergGlottalPulse_setSampleRate(gp, sampleRate);

     //audio loop
     for (int i = 0; i < sampleRate; ++i){
        float pulse = tRosenbergGlottalPulse_tick(gp);
        output[i] = pulse
    }

    //when done
    tRosenbergGlottalPulse_free(&gp);
    @endcode
     
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
     
     @fn Lfloat   tRosenbergGlottalPulse_tick           (tRosenbergGlottalPulse* const)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn Lfloat   tRosenbergGlottalPulse_tickHQ           (tRosenbergGlottalPulse* const gp)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn void   tRosenbergGlottalPulse_setFreq           (tRosenbergGlottalPulse* const, Lfloat freq)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn void   tRosenbergGlottalPulse_setOpenLength           (tRosenbergGlottalPulse* const, Lfloat openLength)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn void   tRosenbergGlottalPulse_setPulseLength           (tRosenbergGlottalPulse* const, Lfloat pulseLength)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @fn void   tRosenbergGlottalPulse_setOpenLengthAndPulseLength           (tRosenbergGlottalPulse* const gp, Lfloat openLength, Lfloat pulseLength)
     @brief
     @param pulse A pointer to the relevant tRosenbergGlottalPulse.
     
     @} */
    
    typedef struct tRosenbergGlottalPulse
    {

        tMempool* mempool;
        Lfloat phase;
        Lfloat openLength;
        Lfloat pulseLength;
        Lfloat invPulseLengthMinusOpenLength;
        Lfloat freq;
        Lfloat inc;
        Lfloat invSampleRate;
    } tRosenbergGlottalPulse;

    void    tRosenbergGlottalPulse_init                        (tRosenbergGlottalPulse** const, LEAF* const leaf);
    void    tRosenbergGlottalPulse_initToPool                  (tRosenbergGlottalPulse** const, tMempool** const);
    void    tRosenbergGlottalPulse_free                        (tRosenbergGlottalPulse** const);

    Lfloat  tRosenbergGlottalPulse_tick                        (tRosenbergGlottalPulse* const);
    Lfloat  tRosenbergGlottalPulse_tickHQ                      (tRosenbergGlottalPulse* const gp);

    void    tRosenbergGlottalPulse_setFreq                     (tRosenbergGlottalPulse* const, Lfloat freq);
    void    tRosenbergGlottalPulse_setOpenLength               (tRosenbergGlottalPulse* const, Lfloat openLength);
    void    tRosenbergGlottalPulse_setPulseLength              (tRosenbergGlottalPulse* const, Lfloat pulseLength);
    void    tRosenbergGlottalPulse_setOpenLengthAndPulseLength (tRosenbergGlottalPulse* const, Lfloat openLength, Lfloat pulseLength);
    void    tRosenbergGlottalPulse_setSampleRate               (tRosenbergGlottalPulse* const, Lfloat sr);

    //==============================================================================
    
    /*!
     @defgroup tsolad tSOLAD
     @ingroup effects
     @brief pitch shifting algorithm that underlies tRetune etc from Katja Vetters http://www.katjaas.nl/pitchshiftlowlatency/pitchshiftlowlatency.html

    The SOLAD algorithm performs real-time pitch shifting by
    1. Writing incoming blocks into a circular delay buffer (`loopSize` samples)
    2. Reading from the buffer with a lag adjusted by `pitchFactor` to change pitch
    3. Crossfading between successive read pointers over `xfadeLength` samples to avoid clicks
    4. Optionally using a periodicity estimate (`period`) and forced `readLag` to stabilize the shift

    Example
    @code{.c}
    //initialize
    tSOLAD* solad = NULL;
    tSOLAD_init(&solad,
                4096,   //loopSize (power of two)
                leaf);

    //2× speed-up (one octave up)
    tSOLAD_setPitchFactor(solad, 2.0f);
    //lock read lag for smoother output
    tSOLAD_setReadLag(solad, 512.0f);
    // provide an estimated period for transients (in samples)
    tSOLAD_setPeriod(solad, 64.0f);

    const int blockSize = 128;
    float inBlock[blockSize], outBlock[blockSize];
    while (hasAudio()) {
        fillInput(inBlock, blockSize);
        tSOLAD_ioSamples(solad, inBlock, outBlock, blockSize);
         playOutput(outBlock, blockSize);
    }

    // reset internal state if needed
    tSOLAD_resetState(solad);
    // update sample rate for internal filters
    tSOLAD_setSampleRate(solad, sampleRate);

    //when done
    tSOLAD_free(&solad);
    @endcode
     
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
     
     @fn void    tSOLAD_ioSamples        (tSOLAD *w, Lfloat* in, Lfloat* out, int blocksize)
     @brief Send one block of input samples, receive one block of output samples
     @param solad A pointer to the relevant tSOLAD.
     
     @fn void    tSOLAD_setPeriod        (tSOLAD *w, Lfloat period)
     @brief Set periodicity analysis data
     @param solad A pointer to the relevant tSOLAD.
     
     @fn void    tSOLAD_setPitchFactor   (tSOLAD *w, Lfloat pitchfactor)
     @brief Set pitch factor between 0.25 and 4
     @param solad A pointer to the relevant tSOLAD.
     
     @fn void    tSOLAD_setReadLag       (tSOLAD *w, Lfloat readlag)
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
    //#define MAXPERIOD (Lfloat)((LOOPSIZE - w->blocksize) * 0.8f)
#define MINPERIOD 8.0f
    
    typedef struct tSOLAD
    {
        tMempool* mempool;
        
        tAttackDetection*  ad;
        tHighpass*  hp;
        
        int loopSize;
        uint16_t timeindex;              // current reference time, write index
        uint16_t blocksize;              // signal input / output block size
        Lfloat pitchfactor;        // pitch factor between 0.25 and 4
        Lfloat readlag;            // read pointer's lag behind write pointer
        Lfloat period;             // period length in input signal
        Lfloat jump;               // read pointer jump length and direction
        Lfloat xfadelength;        // crossfade length expressed at input sample rate
        Lfloat xfadevalue;         // crossfade phase and value
        
        Lfloat* delaybuf;
        
    } tSOLAD;

    void    tSOLAD_init             (tSOLAD** const, int loopSize, LEAF* const leaf);
    void    tSOLAD_initToPool       (tSOLAD** const, int loopSize, tMempool** const);
    void    tSOLAD_free             (tSOLAD** const);
    
    // send one block of input samples, receive one block of output samples
    void    tSOLAD_ioSamples        (tSOLAD* w, Lfloat* in, Lfloat* out, int blocksize);
    // set periodicity analysis data
    void    tSOLAD_setPeriod        (tSOLAD* w, Lfloat period);
    // set pitch factor between 0.25 and 4
    void    tSOLAD_setPitchFactor   (tSOLAD* w, Lfloat pitchfactor);
    // force readpointer lag
    void    tSOLAD_setReadLag       (tSOLAD* w, Lfloat readlag);
    // reset state variables
    void    tSOLAD_resetState       (tSOLAD* w);
    void    tSOLAD_setSampleRate    (tSOLAD* const, Lfloat sr);
    
    /*!
     @defgroup tpitchshift tPitchShift
     @ingroup effects
     @brief SOLAD-based pitch shifter.

    The pitch shift combines a dual pitch detector to track the desired pitch shift factor,
     and the SOLAD buffer to perform the time-domain shift
     On each block or sample, you feed in audio and either specify a shift factory directly or
     let the dual detector drive the factory automatically

     Example
    @code{.c}
    //initialize
    tDualPitchDetector* dpd = NULL;
    tDualPitchDetector_init(&dpd, 50.0f, 2000.0f, leaf);

    //create a PitchShift instance
    tPitchShift* ps = NULL;
    int bufSize = 1024;
    tPitchShift_init(&ps, dpd, bufSize, leaf);

    //audio loop
    float inBuf[bufSize], outBuf[bufSize];
    // fill inBuf...
    tPitchShift_shiftBy(ps, 1.5f, inBuf, outBuf);  // +50% pitch


    //adjust pickiness (how strongly it follows the detector)
    tPitchShift_setPickiness(ps, 0.8f);

    //set sample rate
    tPitchShift_setSampleRate(ps, sampleRate);

    //when done:
    tPitchShift_free(&ps);
    tDualPitchDetector_free(&dpd);
    @endcode
     
     @fn void    tPitchShift_init(tPitchShift** const, tPeriodDetection* const, Lfloat* out, int bufSize, LEAF* const leaf)
     @brief Initialize a tPitchShift to the default mempool of a LEAF instance.
     @param pitchshift A pointer to the tPitchShift to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPitchShift_initToPool(tPitchShift** const, tPeriodDetection* const, Lfloat* out, int bufSize, tMempool** const)
     @brief Initialize a tPitchShift to a specified mempool.
     @param pitchshift A pointer to the tPitchShift to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPitchShift_free(tPitchShift** const)
     @brief Free a tPitchShift from its mempool.
     @param pitchshift A pointer to the tPitchShift to free.
     
     @fn Lfloat   tPitchShift_shift           (tPitchShift* const)
     @brief
     @param pitchshift A pointer to the relevant tPitchShift.
     
     @fn Lfloat   tPitchShift_shiftToFunc     (tPitchShift* const, Lfloat (*fun)(Lfloat))
     @brief
     @param pitchshift A pointer to the relevant tPitchShift.
     
     @fn Lfloat   tPitchShift_shiftToFreq     (tPitchShift* const, Lfloat freq)
     @brief
     @param pitchshift A pointer to the relevant tPitchShift.
     
     @fn void    tPitchShift_setPitchFactor  (tPitchShift* const, Lfloat pf)
     @brief
     @param pitchshift A pointer to the relevant tPitchShift.
     
     @} */
    
    typedef struct tPitchShift
    {
        tMempool* mempool;
        
        tDualPitchDetector*  pd;
        tSOLAD*  sola;
        
        Lfloat* outBuffer;
        Lfloat* inBuffer;
        int bufSize;
        int index;
        
        Lfloat pickiness;
        
        Lfloat sampleRate;
    } tPitchShift;

    void    tPitchShift_init          (tPitchShift** const, tDualPitchDetector** const, int bufSize, LEAF* const leaf);
    void    tPitchShift_initToPool    (tPitchShift** const, tDualPitchDetector** const, int bufSize, tMempool** const);
    void    tPitchShift_free          (tPitchShift** const);
    
    void    tPitchShift_shiftBy       (tPitchShift* const, Lfloat factor, Lfloat* in, Lfloat* out);
    void    tPitchShift_shiftTo       (tPitchShift* const, Lfloat freq, Lfloat* in, Lfloat* out);
    void    tPitchShift_setPickiness  (tPitchShift* const, Lfloat p);
    void    tPitchShift_setSampleRate (tPitchShift* const, Lfloat sr);
    
    /*!
     @defgroup tsimpleretune tSimpleRetune
     @ingroup effects
     @brief Wrapper for multiple pitch shifters with single-channel output.

    The simple retune spawns `numVoices` independent SOLAD‐based pitch shifters, all driven by tDualPitchDetector
    It mixes their outputs into one channel, allowing detune, chorusing, or harmonization with minimal setup

    Example
    @code{.c}
    // initialize
    tSimpleRetune* tr = NULL;
    tSimpleRetune_init(&tr,
                        3,        //number of voices
                        100.0f,   //minimum input frequency
                        1000.0f,  //maximum input frequency
                        512,      //buffer size
                        leaf);

    //audio loop
    for (int i = 0; i < numSamples; ++i) {
        float out = tSimpleRetune_tick(tr, inputSamples[i]);
        outputSamples[i] = out;
    }

     //adjust responsiveness
     tSimpleRetune_setPickiness(tr, 0.7f);

     //when done
     tSimpleRetune_free(&tr);
     @endcode
     
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
     
     @fn Lfloat*  tSimpleRetune_tick                  (tSimpleRetune* const, Lfloat sample)
     @brief
     @param retune A pointer to the relevant tSimpleRetune.
     
     @fn void    tSimpleRetune_setNumVoices          (tSimpleRetune* const, int numVoices)
     @brief
     @param retune A pointer to the relevant tSimpleRetune.
     
     @fn void    tSimpleRetune_tuneVoices              (tSimpleRetune* const, Lfloat f)
     @brief
     @param retune A pointer to the relevant tSimpleRetune.
     
     @fn void    tSimpleRetune_tuneVoice              (tSimpleRetune* const, Lfloat f, int voice)
     @brief
     @param retune A pointer to the relevant tSimpleRetune.
     
     @} */
    
    typedef struct tSimpleRetune
    {
        tMempool* mempool;
        
        tDualPitchDetector* dp;
        Lfloat minInputFreq, maxInputFreq;
        
        tPitchShift** ps;
        
        Lfloat* pdBuffer;
        Lfloat* inBuffer;
        Lfloat* outBuffer;
        int bufSize;
        int index;
        
        void (*shiftFunction)(tPitchShift* const, Lfloat, Lfloat*, Lfloat*);
        
        Lfloat* shiftValues;
        int numVoices;
    } tSimpleRetune;

    void    tSimpleRetune_init                  (tSimpleRetune** const, int numVoices, Lfloat minInputFreq, Lfloat maxInputFreq, int bufSize, LEAF* const leaf);
    void    tSimpleRetune_initToPool            (tSimpleRetune** const, int numVoices, Lfloat minInputFreq, Lfloat maxInputFreq, int bufSize, tMempool** const);
    void    tSimpleRetune_free                  (tSimpleRetune** const);
    
    Lfloat  tSimpleRetune_tick                  (tSimpleRetune* const, Lfloat sample);

    void    tSimpleRetune_setMode               (tSimpleRetune* const, int mode);
    void    tSimpleRetune_setNumVoices          (tSimpleRetune* const, int numVoices);
    void    tSimpleRetune_setPickiness          (tSimpleRetune* const, Lfloat p);
    void    tSimpleRetune_tuneVoices            (tSimpleRetune* const, Lfloat* t);
    void    tSimpleRetune_tuneVoice             (tSimpleRetune* const, int voice, Lfloat t);
    Lfloat  tSimpleRetune_getInputFrequency     (tSimpleRetune* const);
    void    tSimpleRetune_setSampleRate         (tSimpleRetune* const, Lfloat sr);

    /*!
     @defgroup tretune tRetune
     @ingroup effects
     @brief Wrapper for multiple pitch shifters with multi-channel output.

    tRetune builds on SOLAD and dual‐detector pitch tracking to drive `numVoices` independent
     pitch shifters, each producing its own channel.  You can use it for chorus, harmonies,
     or multi‐voice layering with minimal setup

    Example
     @code{.c}
     //initialize
     tRetune* rt = NULL;
     tRetune_init(&rt,
                  4,       //number of voices
                  80.0f,   //minimum input frequency
                  1200.0f, //maximum input frequency
                  512,     //buffer size
                  leaf);

     //audio loop,
     for (int i = 0; i < numSamples; ++i) {
         Lfloat* outs = tRetune_tick(rt, inputSamples[i]);
         for (int v = 0; v < 4; ++v)
             outputBuffers[v][i] = outs[v];
     }

     //adjust number of voices
     tRetune_setNumVoices(rt, 3);

     //set individual pitch factors
     tRetune_setPitchFactor(rt, 1.5f, 0);  //voice 0 up a fifth
     tRetune_setPitchFactor(rt, 0.8f, 1);  //voice 1 down a minor third

     //when done
     tRetune_free(&rt);
     @endcode
     
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
     
     @fn Lfloat*  tRetune_tick                (tRetune* const, Lfloat sample)
     @brief
     @param retune A pointer to the relevant tRetune.
     
     @fn void    tRetune_setNumVoices        (tRetune* const, int numVoices)
     @brief
     @param retune A pointer to the relevant tRetune.
     
     @fn void    tRetune_setPitchFactors     (tRetune* const, Lfloat pf)
     @brief
     @param retune A pointer to the relevant tRetune.
     
     @fn void    tRetune_setPitchFactor      (tRetune* const, Lfloat pf, int voice)
     @brief
     @param retune A pointer to the relevant tRetune.
     
     @} */
    
    typedef struct tRetune
    {
        tMempool* mempool;
        
        tDualPitchDetector** dp;
        Lfloat minInputFreq, maxInputFreq;
        
        tPitchShift** ps;
        
        Lfloat* pdBuffer;
        Lfloat* inBuffer;
        Lfloat** outBuffers;
        int bufSize;
        int index;
        
        Lfloat* output;
        
        void (*shiftFunction)(tPitchShift* const, Lfloat, Lfloat*, Lfloat*);
        
        Lfloat* shiftValues;
        int numVoices;
    } tRetune;

    void    tRetune_init                (tRetune** const, int numVoices, Lfloat minInputFreq, Lfloat maxInputFreq,  int bufSize, LEAF* const leaf);
    void    tRetune_initToPool          (tRetune** const,  int numVoices, Lfloat minInputFreq, Lfloat maxInputFreq, int bufSize, tMempool** const);
    void    tRetune_free                (tRetune** const);
    
    Lfloat* tRetune_tick                (tRetune* const, Lfloat sample);

    void    tRetune_setMode             (tRetune* const, int mode);
    void    tRetune_setNumVoices        (tRetune* const, int numVoices);
    void    tRetune_setPickiness        (tRetune* const, Lfloat p);
    void    tRetune_tuneVoices          (tRetune* const, Lfloat* t);
    void    tRetune_tuneVoice           (tRetune* const, int voice, Lfloat t);
    Lfloat  tRetune_getInputFrequency   (tRetune* const);
    void    tRetune_setSampleRate       (tRetune* const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tformantshifter tFormantShifter
     @ingroup effects
     @brief Formant remover and adder, allowing for formant shifting.

    The formant shifter analyzes the spectral envelope of incoming audio via LPC-like filters
     and then either subtracts that envelope (removal) or reapplies it shifted by `shiftFactor`
     (addition).  An `intensity` control blends between dry and processed signals.

     Example
     @code{.c}
     //initialize
     tFormantShifter* fs = NULL;
     tFormantShifter_init(&fs,
                          12,    // filter order
                          leaf);

     // in your audio loop:
     for (int i = 0; i < numSamples; ++i) {
         float in  = inputSamples[i];
         //remove formants for a robotic sound
         float removed = tFormantShifter_remove(fs, in);
         //add shifted formants (e.g. 1.2×):
         tFormantShifter_setShiftFactor(fs, 1.2f);
         float added   = tFormantShifter_add(fs, in);
         //mix dry and shifted
         float out = fs->intensity * added + (1.0f - fs->intensity) * in;
         outputSamples[i] = out;
     }

     //for block processing with warp factor fwarp
     float inBuf[256], outBuf[256];
     tFormantShifter_ioSamples(fs, inBuf, outBuf, 256, 0.8f);

     //adjust intensity of effect
     tFormantShifter_setIntensity(fs, 0.75f);

     //when done
     tFormantShifter_free(&fs);
     @endcode
     
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
     
     @fn Lfloat   tFormantShifter_tick            (tFormantShifter* const, Lfloat input)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn Lfloat   tFormantShifter_remove          (tFormantShifter* const, Lfloat input)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn Lfloat   tFormantShifter_add             (tFormantShifter* const, Lfloat input)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn void    tFormantShifter_ioSamples       (tFormantShifter* const, Lfloat* in, Lfloat* out, int size, Lfloat fwarp)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn void    tFormantShifter_setShiftFactor  (tFormantShifter* const, Lfloat shiftFactor)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @fn void    tFormantShifter_setIntensity    (tFormantShifter* const, Lfloat intensity)
     @brief
     @param formant A pointer to the relevant tFormantShifter.
     
     @} */
    
    typedef struct tFormantShifter
    {

        tMempool* mempool;
        int ford;
        Lfloat falph;
        Lfloat flamb;
        Lfloat* fk;
        Lfloat* fb;
        Lfloat* fc;
        Lfloat* frb;
        Lfloat* frc;
        Lfloat* fsig;
        Lfloat* fsmooth;
        Lfloat fhp;
        Lfloat flp;
        Lfloat flpa;
        Lfloat* fbuff;
        Lfloat* ftvec;
        Lfloat fmute;
        Lfloat fmutealph;
        unsigned int cbi;
        Lfloat shiftFactor;
        Lfloat intensity, invIntensity;
        tHighpass* hp;
        tHighpass* hp2;
        tFeedbackLeveler* fbl1;
        tFeedbackLeveler* fbl2;
        Lfloat sampleRate;
        Lfloat invSampleRate;
    } tFormantShifter;

    void    tFormantShifter_init            (tFormantShifter** const, int order, LEAF* const leaf);
    void    tFormantShifter_initToPool      (tFormantShifter** const, int order, tMempool** const);
    void    tFormantShifter_free            (tFormantShifter** const);
    
    Lfloat  tFormantShifter_tick            (tFormantShifter* const, Lfloat input);

    Lfloat  tFormantShifter_remove          (tFormantShifter* const, Lfloat input);
    Lfloat  tFormantShifter_add             (tFormantShifter* const, Lfloat input);
    void    tFormantShifter_ioSamples       (tFormantShifter* const, Lfloat* in, Lfloat* out, int size, Lfloat fwarp);
    void    tFormantShifter_setShiftFactor  (tFormantShifter* const, Lfloat shiftFactor);
    void    tFormantShifter_setIntensity    (tFormantShifter* const, Lfloat intensity);
    void    tFormantShifter_setSampleRate   (tFormantShifter* const fsr, Lfloat sr);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_EFFECTS_H_INCLUDED

//==============================================================================

