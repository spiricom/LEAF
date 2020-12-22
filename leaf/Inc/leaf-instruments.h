/*==============================================================================
 
 leaf-instruments.h
 Created: 30 Nov 2018 10:24:44am
 Author:  airship
 
 ==============================================================================*/

#ifndef LEAF_INSTRUMENTS_H_INCLUDED
#define LEAF_INSTRUMENTS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-oscillators.h"
#include "leaf-filters.h"
#include "leaf-envelopes.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    /*!
     @defgroup t808cowbell t808Cowbell
     @ingroup instruments
     @brief
     @{
     
     @fn void    t808Cowbell_init            (t808Cowbell* const, int useStick, LEAF* const leaf)
     @brief Initialize a t808Cowbell to the default mempool of a LEAF instance.
     @param cowbell A pointer to the t808Cowbell to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    t808Cowbell_initToPool      (t808Cowbell* const, int useStick, tMempool* const)
     @brief Initialize a t808Cowbell to a specified mempool.
     @param cowbell A pointer to the t808Cowbell to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    t808Cowbell_free            (t808Cowbell* const)
     @brief Free a t808Cowbell from its mempool.
     @param cowbell A pointer to the t808Cowbell to free.
     
     @fn float   t808Cowbell_tick            (t808Cowbell* const)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_on              (t808Cowbell* const, float vel)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setDecay        (t808Cowbell* const, float decay)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setHighpassFreq (t808Cowbell* const, float freq)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setBandpassFreq (t808Cowbell* const, float freq)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setFreq         (t808Cowbell* const, float freq)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setOscMix       (t808Cowbell* const, float oscMix)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setStick        (t808Cowbell* const, int useStick)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @} */
    
    typedef struct _t808Cowbell
    {
        tMempool mempool;
        tSquare p[2];
        tNoise stick;
        tSVF bandpassOsc;
        tSVF bandpassStick;
        tEnvelope envGain;
        tEnvelope envStick;
        tEnvelope envFilter;
        tHighpass highpass;
        float oscMix;
        float filterCutoff;
        uint8_t useStick;
    } _t808Cowbell;
    
    typedef _t808Cowbell* t808Cowbell;
    
    void    t808Cowbell_init            (t808Cowbell* const, int useStick, LEAF* const leaf);
    void    t808Cowbell_initToPool      (t808Cowbell* const, int useStick, tMempool* const);
    void    t808Cowbell_free            (t808Cowbell* const);
    
    float   t808Cowbell_tick            (t808Cowbell* const);
    void    t808Cowbell_on              (t808Cowbell* const, float vel);
    void    t808Cowbell_setDecay        (t808Cowbell* const, float decay);
    void    t808Cowbell_setHighpassFreq (t808Cowbell* const, float freq);
    void    t808Cowbell_setBandpassFreq (t808Cowbell* const, float freq);
    void    t808Cowbell_setFreq         (t808Cowbell* const, float freq);
    void    t808Cowbell_setOscMix       (t808Cowbell* const, float oscMix);
    void    t808Cowbell_setStick        (t808Cowbell* const, int useStick);
    
    //==============================================================================
    
    /*!
     @defgroup t808hihat t808Hihat
     @ingroup instruments
     @brief
     @{
     
     @fn void    t808Hihat_init                  (t808Hihat* const, LEAF* const leaf)
     @brief Initialize a t808Hihat to the default mempool of a LEAF instance.
     @param hihat A pointer to the t808Hihat to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    t808Hihat_initToPool            (t808Hihat* const, tMempool* const)
     @brief Initialize a t808Hihat to a specified mempool.
     @param hihat A pointer to the t808Hihat to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    t808Hihat_free                  (t808Hihat* const)
     @brief Free a t808Hihat from its mempool.
     @param hihat A pointer to the t808Hihat to free.
     
     @fn float   t808Hihat_tick                  (t808Hihat* const)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_on                    (t808Hihat* const, float vel)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setOscNoiseMix        (t808Hihat* const, float oscNoiseMix)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setDecay              (t808Hihat* const, float decay)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setHighpassFreq       (t808Hihat* const, float freq)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setOscBandpassFreq    (t808Hihat* const, float freq)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setOscBandpassQ       (t808Hihat* const hihat, float Q)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setStickBandPassFreq  (t808Hihat* const, float freq)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setStickBandPassQ     (t808Hihat* const hihat, float Q)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setOscFreq            (t808Hihat* const, float freq)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setStretch            (t808Hihat* const hihat, float stretch)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setFM                 (t808Hihat* const hihat, float FM_amount)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
    
     @} */
    
    typedef struct _t808Hihat
    {
        
        tMempool mempool;
        // 6 Square waves
        tSquare p[6];
        tNoise n;
        tSVF bandpassOsc;
        tSVF bandpassStick;
        tEnvelope envGain;
        tEnvelope envStick;
        tEnvelope noiseFMGain;
        tHighpass highpass;
        tNoise stick;
        
        float freq;
        float stretch;
        float FM_amount;
        float oscNoiseMix;
    } _t808Hihat;
    
    typedef _t808Hihat* t808Hihat;
    
    void    t808Hihat_init                  (t808Hihat* const, LEAF* const leaf);
    void    t808Hihat_initToPool            (t808Hihat* const, tMempool* const);
    void    t808Hihat_free                  (t808Hihat* const);
    
    float   t808Hihat_tick                  (t808Hihat* const);
    void    t808Hihat_on                    (t808Hihat* const, float vel);
    void    t808Hihat_setOscNoiseMix        (t808Hihat* const, float oscNoiseMix);
    void    t808Hihat_setDecay              (t808Hihat* const, float decay);
    void    t808Hihat_setHighpassFreq       (t808Hihat* const, float freq);
    void    t808Hihat_setOscBandpassFreq    (t808Hihat* const, float freq);
    void    t808Hihat_setOscBandpassQ       (t808Hihat* const hihat, float Q);
    void    t808Hihat_setStickBandPassFreq  (t808Hihat* const, float freq);
    void    t808Hihat_setStickBandPassQ     (t808Hihat* const hihat, float Q);
    void    t808Hihat_setOscFreq            (t808Hihat* const, float freq);
    void    t808Hihat_setStretch            (t808Hihat* const hihat, float stretch);
    void    t808Hihat_setFM                 (t808Hihat* const hihat, float FM_amount);
    
    //==============================================================================
    
    /*!
     @defgroup t808snare t808Snare
     @ingroup instruments
     @brief
     @{
     
     @fn void    t808Snare_init                  (t808Snare* const, LEAF* const leaf)
     @brief Initialize a t808Snare to the default mempool of a LEAF instance.
     @param snare A pointer to the t808Snare to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    t808Snare_initToPool            (t808Snare* const, tMempool* const)
     @brief Initialize a t808Snare to a specified mempool.
     @param snare A pointer to the t808Snare to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    t808Snare_free                  (t808Snare* const)
     @brief Free a t808Snare from its mempool.
     @param snare A pointer to the t808Snare to free.
     
     @fn float   t808Snare_tick                  (t808Snare* const)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_on                    (t808Snare* const, float vel)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setTone1Freq          (t808Snare* const, float freq)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setTone2Freq          (t808Snare* const, float freq)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setTone1Decay         (t808Snare* const, float decay)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setTone2Decay         (t808Snare* const, float decay)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setNoiseDecay         (t808Snare* const, float decay)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setToneNoiseMix       (t808Snare* const, float toneNoiseMix)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setNoiseFilterFreq    (t808Snare* const, float noiseFilterFreq)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setNoiseFilterQ       (t808Snare* const, float noiseFilterQ)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @} */
    
    typedef struct _t808Snare
    {
        
        tMempool mempool;
        // Tone 1, Tone 2, Noise
        tTriangle tone[2]; // Tri (not yet antialiased or wavetabled)
        tNoise noiseOsc;
        tSVF toneLowpass[2];
        tSVF noiseLowpass; // Lowpass from SVF filter
        tEnvelope toneEnvOsc[2];
        tEnvelope toneEnvGain[2];
        tEnvelope noiseEnvGain;
        tEnvelope toneEnvFilter[2];
        tEnvelope noiseEnvFilter;
        
        float toneGain[2];
        float noiseGain;
        
        float toneNoiseMix;
        
        float tone1Freq, tone2Freq;
        
        float noiseFilterFreq;
    } _t808Snare;
    
    typedef _t808Snare* t808Snare;
    
    void    t808Snare_init                  (t808Snare* const, LEAF* const leaf);
    void    t808Snare_initToPool            (t808Snare* const, tMempool* const);
    void    t808Snare_free                  (t808Snare* const);
    
    float   t808Snare_tick                  (t808Snare* const);
    void    t808Snare_on                    (t808Snare* const, float vel);
    void    t808Snare_setTone1Freq          (t808Snare* const, float freq);
    void    t808Snare_setTone2Freq          (t808Snare* const, float freq);
    void    t808Snare_setTone1Decay         (t808Snare* const, float decay);
    void    t808Snare_setTone2Decay         (t808Snare* const, float decay);
    void    t808Snare_setNoiseDecay         (t808Snare* const, float decay);
    void    t808Snare_setToneNoiseMix       (t808Snare* const, float toneNoiseMix);
    void    t808Snare_setNoiseFilterFreq    (t808Snare* const, float noiseFilterFreq);
    void    t808Snare_setNoiseFilterQ       (t808Snare* const, float noiseFilterQ);
    
    //==============================================================================
    
    /*!
     @defgroup t808kick t808Kick
     @ingroup instruments
     @brief
     @{
     
     @fn void    t808Kick_init               (t808Kick* const, LEAF* const leaf)
     @brief Initialize a t808Kick to the default mempool of a LEAF instance.
     @param kick A pointer to the t808Kick to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    t808Kick_initToPool         (t808Kick* const, tMempool* const)
     @brief Initialize a t808Kick to a specified mempool.
     @param kick A pointer to the t808Kick to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    t808Kick_free               (t808Kick* const)
     @brief Free a t808Kick from its mempool.
     @param kick A pointer to the t808Kick to free.
     
     @fn float   t808Kick_tick               (t808Kick* const)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_on                 (t808Kick* const, float vel)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setToneFreq        (t808Kick* const, float freq)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setToneDecay       (t808Kick* const, float decay)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setNoiseDecay      (t808Kick* const, float decay)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setSighAmount      (t808Kick* const, float sigh)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setChirpAmount     (t808Kick* const, float chirp)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setToneNoiseMix    (t808Kick* const, float toneNoiseMix)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setNoiseFilterFreq (t808Kick* const, float noiseFilterFreq)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setNoiseFilterQ    (t808Kick* const, float noiseFilterQ)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @} */
    
    typedef struct _t808Kick
    {
        
        tMempool mempool;
        
        tCycle tone; // Tri
        tNoise noiseOsc;
        tSVF toneLowpass;
        tEnvelope toneEnvOscChirp;
        tEnvelope toneEnvOscSigh;
        tEnvelope toneEnvGain;
        tEnvelope noiseEnvGain;
        tEnvelope toneEnvFilter;
        
        float toneGain;
        float noiseGain;
        
        float toneInitialFreq;
        float sighAmountInHz;
        float chirpRatioMinusOne;
        float noiseFilterFreq;
    } _t808Kick;
    
    typedef _t808Kick* t808Kick;
    
    void    t808Kick_init               (t808Kick* const, LEAF* const leaf);
    void    t808Kick_initToPool         (t808Kick* const, tMempool* const);
    void    t808Kick_free               (t808Kick* const);
    
    float   t808Kick_tick               (t808Kick* const);
    void    t808Kick_on                 (t808Kick* const, float vel);
    void    t808Kick_setToneFreq        (t808Kick* const, float freq);
    void    t808Kick_setToneDecay       (t808Kick* const, float decay);
    void    t808Kick_setNoiseDecay      (t808Kick* const, float decay);
    void    t808Kick_setSighAmount      (t808Kick* const, float sigh);
    void    t808Kick_setChirpAmount     (t808Kick* const, float chirp);
    void    t808Kick_setToneNoiseMix    (t808Kick* const, float toneNoiseMix);
    void    t808Kick_setNoiseFilterFreq (t808Kick* const, float noiseFilterFreq);
    void    t808Kick_setNoiseFilterQ    (t808Kick* const, float noiseFilterQ);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_INSTRUMENTS_H_INCLUDED

//==============================================================================



