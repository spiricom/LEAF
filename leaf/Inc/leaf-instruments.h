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
     @brief 808 drum machine cowbell.
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
     
     @fn Lfloat   t808Cowbell_tick            (t808Cowbell* const)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_on              (t808Cowbell* const, Lfloat vel)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setDecay        (t808Cowbell* const, Lfloat decay)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setHighpassFreq (t808Cowbell* const, Lfloat freq)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setBandpassFreq (t808Cowbell* const, Lfloat freq)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setFreq         (t808Cowbell* const, Lfloat freq)
     @brief
     @param cowbell A pointer to the relevant t808Cowbell.
     
     @fn void    t808Cowbell_setOscMix       (t808Cowbell* const, Lfloat oscMix)
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
        Lfloat oscMix;
        Lfloat filterCutoff;
        uint8_t useStick;
    } _t808Cowbell;
    
    typedef _t808Cowbell* t808Cowbell;
    
    void    t808Cowbell_init            (t808Cowbell* const, int useStick, LEAF* const leaf);
    void    t808Cowbell_initToPool      (t808Cowbell* const, int useStick, tMempool* const);
    void    t808Cowbell_free            (t808Cowbell* const);
    
    Lfloat  t808Cowbell_tick            (t808Cowbell const);

    void    t808Cowbell_on              (t808Cowbell const, Lfloat vel);
    void    t808Cowbell_setDecay        (t808Cowbell const, Lfloat decay);
    void    t808Cowbell_setHighpassFreq (t808Cowbell const, Lfloat freq);
    void    t808Cowbell_setBandpassFreq (t808Cowbell const, Lfloat freq);
    void    t808Cowbell_setFreq         (t808Cowbell const, Lfloat freq);
    void    t808Cowbell_setOscMix       (t808Cowbell const, Lfloat oscMix);
    void    t808Cowbell_setStick        (t808Cowbell const, int useStick);
    void    t808Cowbell_setSampleRate   (t808Cowbell const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup t808hihat t808Hihat
     @ingroup instruments
     @brief 808 drum machine hihat.
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
     
     @fn Lfloat   t808Hihat_tick                  (t808Hihat* const)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_on                    (t808Hihat* const, Lfloat vel)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setOscNoiseMix        (t808Hihat* const, Lfloat oscNoiseMix)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setDecay              (t808Hihat* const, Lfloat decay)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setHighpassFreq       (t808Hihat* const, Lfloat freq)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setOscBandpassFreq    (t808Hihat* const, Lfloat freq)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setOscBandpassQ       (t808Hihat* const hihat, Lfloat Q)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setStickBandPassFreq  (t808Hihat* const, Lfloat freq)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setStickBandPassQ     (t808Hihat* const hihat, Lfloat Q)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setOscFreq            (t808Hihat* const, Lfloat freq)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setStretch            (t808Hihat* const hihat, Lfloat stretch)
     @brief
     @param hihat A pointer to the relevant t808Hihat.
     
     @fn void    t808Hihat_setFM                 (t808Hihat* const hihat, Lfloat FM_amount)
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
        
        Lfloat freq;
        Lfloat stretch;
        Lfloat FM_amount;
        Lfloat oscNoiseMix;
    } _t808Hihat;
    
    typedef _t808Hihat* t808Hihat;
    
    void    t808Hihat_init                  (t808Hihat* const, LEAF* const leaf);
    void    t808Hihat_initToPool            (t808Hihat* const, tMempool* const);
    void    t808Hihat_free                  (t808Hihat* const);
    
    Lfloat  t808Hihat_tick                  (t808Hihat const);

    void    t808Hihat_on                    (t808Hihat const, Lfloat vel);
    void    t808Hihat_setOscNoiseMix        (t808Hihat const, Lfloat oscNoiseMix);
    void    t808Hihat_setDecay              (t808Hihat const, Lfloat decay);
    void    t808Hihat_setHighpassFreq       (t808Hihat const, Lfloat freq);
    void    t808Hihat_setOscBandpassFreq    (t808Hihat const, Lfloat freq);
    void    t808Hihat_setOscBandpassQ       (t808Hihat const hihat, Lfloat Q);
    void    t808Hihat_setStickBandPassFreq  (t808Hihat const, Lfloat freq);
    void    t808Hihat_setStickBandPassQ     (t808Hihat const hihat, Lfloat Q);
    void    t808Hihat_setOscFreq            (t808Hihat const, Lfloat freq);
    void    t808Hihat_setStretch            (t808Hihat const hihat, Lfloat stretch);
    void    t808Hihat_setFM                 (t808Hihat const hihat, Lfloat FM_amount);
    void    t808Hihat_setSampleRate         (t808Hihat const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup t808snare t808Snare
     @ingroup instruments
     @brief 808 drum machine snare.
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
     
     @fn Lfloat   t808Snare_tick                  (t808Snare* const)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_on                    (t808Snare* const, Lfloat vel)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setTone1Freq          (t808Snare* const, Lfloat freq)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setTone2Freq          (t808Snare* const, Lfloat freq)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setTone1Decay         (t808Snare* const, Lfloat decay)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setTone2Decay         (t808Snare* const, Lfloat decay)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setNoiseDecay         (t808Snare* const, Lfloat decay)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setToneNoiseMix       (t808Snare* const, Lfloat toneNoiseMix)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setNoiseFilterFreq    (t808Snare* const, Lfloat noiseFilterFreq)
     @brief
     @param snare A pointer to the relevant t808Snare.
     
     @fn void    t808Snare_setNoiseFilterQ       (t808Snare* const, Lfloat noiseFilterQ)
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
        
        Lfloat toneGain[2];
        Lfloat noiseGain;
        
        Lfloat toneNoiseMix;
        
        Lfloat tone1Freq, tone2Freq;
        
        Lfloat noiseFilterFreq;
    } _t808Snare;
    
    typedef _t808Snare* t808Snare;
    
    void    t808Snare_init                  (t808Snare* const, LEAF* const leaf);
    void    t808Snare_initToPool            (t808Snare* const, tMempool* const);
    void    t808Snare_free                  (t808Snare* const);
    
    Lfloat  t808Snare_tick                  (t808Snare const);

    void    t808Snare_on                    (t808Snare const, Lfloat vel);
    void    t808Snare_setTone1Freq          (t808Snare const, Lfloat freq);
    void    t808Snare_setTone2Freq          (t808Snare const, Lfloat freq);
    void    t808Snare_setTone1Decay         (t808Snare const, Lfloat decay);
    void    t808Snare_setTone2Decay         (t808Snare const, Lfloat decay);
    void    t808Snare_setNoiseDecay         (t808Snare const, Lfloat decay);
    void    t808Snare_setToneNoiseMix       (t808Snare const, Lfloat toneNoiseMix);
    void    t808Snare_setNoiseFilterFreq    (t808Snare const, Lfloat noiseFilterFreq);
    void    t808Snare_setNoiseFilterQ       (t808Snare const, Lfloat noiseFilterQ);
    void    t808Snare_setSampleRate         (t808Snare const, Lfloat sr);
    
    //==============================================================================
     //==============================================================================
    
    /*!
     @defgroup t808SnareSmall t808SnareSmall
     @ingroup instruments
     @brief 808 drum machine snare.
     @{
     
     @fn void    t808SnareSmall_init                  (t808SnareSmall* const, LEAF* const leaf)
     @brief Initialize a t808SnareSmall to the default mempool of a LEAF instance.
     @param snare A pointer to the t808SnareSmall to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    t808SnareSmall_initToPool            (t808SnareSmall* const, tMempool* const)
     @brief Initialize a t808SnareSmall to a specified mempool.
     @param snare A pointer to the t808SnareSmall to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    t808SnareSmall_free                  (t808SnareSmall* const)
     @brief Free a t808SnareSmall from its mempool.
     @param snare A pointer to the t808SnareSmall to free.
     
     @fn Lfloat   t808SnareSmall_tick                  (t808SnareSmall* const)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @fn void    t808SnareSmall_on                    (t808SnareSmall* const, Lfloat vel)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @fn void    t808SnareSmall_setTone1Freq          (t808SnareSmall* const, Lfloat freq)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @fn void    t808SnareSmall_setTone2Freq          (t808SnareSmall* const, Lfloat freq)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @fn void    t808SnareSmall_setTone1Decay         (t808SnareSmall* const, Lfloat decay)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @fn void    t808SnareSmall_setTone2Decay         (t808SnareSmall* const, Lfloat decay)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @fn void    t808SnareSmall_setNoiseDecay         (t808SnareSmall* const, Lfloat decay)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @fn void    t808SnareSmall_setToneNoiseMix       (t808SnareSmall* const, Lfloat toneNoiseMix)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @fn void    t808SnareSmall_setNoiseFilterFreq    (t808SnareSmall* const, Lfloat noiseFilterFreq)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @fn void    t808SnareSmall_setNoiseFilterQ       (t808SnareSmall* const, Lfloat noiseFilterQ)
     @brief
     @param snare A pointer to the relevant t808SnareSmall.
     
     @} */
    
    typedef struct _t808SnareSmall
    {
        
        tMempool mempool;
        // Tone 1, Tone 2, Noise
        tPBTriangle tone[2]; // Tri 
        tNoise noiseOsc;
        tSVF toneLowpass[2];
        tSVF noiseLowpass; // Lowpass from SVF filter
        tADSRS toneEnvOsc[2];
        tADSRS toneEnvGain[2];
        tADSRS noiseEnvGain;
        tADSRS toneEnvFilter[2];
        tADSRS noiseEnvFilter;
        
        Lfloat toneGain[2];
        Lfloat noiseGain;
        
        Lfloat toneNoiseMix;
        
        Lfloat tone1Freq, tone2Freq;
        
        Lfloat noiseFilterFreq;
    } _t808SnareSmall;
    
    typedef _t808SnareSmall* t808SnareSmall;
    
    void    t808SnareSmall_init                  (t808SnareSmall* const, LEAF* const leaf);
    void    t808SnareSmall_initToPool            (t808SnareSmall* const, tMempool* const);
    void    t808SnareSmall_free                  (t808SnareSmall* const);
    
    Lfloat  t808SnareSmall_tick                  (t808SnareSmall const);

    void    t808SnareSmall_on                    (t808SnareSmall const, Lfloat vel);
    void    t808SnareSmall_setTone1Freq          (t808SnareSmall const, Lfloat freq);
    void    t808SnareSmall_setTone2Freq          (t808SnareSmall const, Lfloat freq);
    void    t808SnareSmall_setTone1Decay         (t808SnareSmall const, Lfloat decay);
    void    t808SnareSmall_setTone2Decay         (t808SnareSmall const, Lfloat decay);
    void    t808SnareSmall_setNoiseDecay         (t808SnareSmall const, Lfloat decay);
    void    t808SnareSmall_setToneNoiseMix       (t808SnareSmall const, Lfloat toneNoiseMix);
    void    t808SnareSmall_setNoiseFilterFreq    (t808SnareSmall const, Lfloat noiseFilterFreq);
    void    t808SnareSmall_setNoiseFilterQ       (t808SnareSmall const, Lfloat noiseFilterQ);
    void    t808SnareSmall_setSampleRate         (t808SnareSmall const, Lfloat sr);
    
    //==============================================================================
    /*!
     @defgroup t808kick t808Kick
     @ingroup instruments
     @brief 808 drum machine kick.
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
     
     @fn Lfloat   t808Kick_tick               (t808Kick* const)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_on                 (t808Kick* const, Lfloat vel)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setToneFreq        (t808Kick* const, Lfloat freq)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setToneDecay       (t808Kick* const, Lfloat decay)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setNoiseDecay      (t808Kick* const, Lfloat decay)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setSighAmount      (t808Kick* const, Lfloat sigh)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setChirpAmount     (t808Kick* const, Lfloat chirp)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setToneNoiseMix    (t808Kick* const, Lfloat toneNoiseMix)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setNoiseFilterFreq (t808Kick* const, Lfloat noiseFilterFreq)
     @brief
     @param kick A pointer to the relevant t808Kick.
     
     @fn void    t808Kick_setNoiseFilterQ    (t808Kick* const, Lfloat noiseFilterQ)
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
        
        Lfloat toneGain;
        Lfloat noiseGain;
        
        Lfloat toneInitialFreq;
        Lfloat sighAmountInHz;
        Lfloat chirpRatioMinusOne;
        Lfloat noiseFilterFreq;
    } _t808Kick;
    
    typedef _t808Kick* t808Kick;
    
    void    t808Kick_init               (t808Kick* const, LEAF* const leaf);
    void    t808Kick_initToPool         (t808Kick* const, tMempool* const);
    void    t808Kick_free               (t808Kick* const);
    
    Lfloat  t808Kick_tick               (t808Kick const);

    void    t808Kick_on                 (t808Kick const, Lfloat vel);
    void    t808Kick_setToneFreq        (t808Kick const, Lfloat freq);
    void    t808Kick_setToneDecay       (t808Kick const, Lfloat decay);
    void    t808Kick_setNoiseDecay      (t808Kick const, Lfloat decay);
    void    t808Kick_setSighAmount      (t808Kick const, Lfloat sigh);
    void    t808Kick_setChirpAmount     (t808Kick const, Lfloat chirp);
    void    t808Kick_setToneNoiseMix    (t808Kick const, Lfloat toneNoiseMix);
    void    t808Kick_setNoiseFilterFreq (t808Kick const, Lfloat noiseFilterFreq);
    void    t808Kick_setNoiseFilterQ    (t808Kick const, Lfloat noiseFilterQ);
    void    t808kick_setSampleRate      (t808Kick const, Lfloat sr);
    
    //==============================================================================

     //==============================================================================
    /*!
     @defgroup t808KickSmall t808KickSmall
     @ingroup instruments
     @brief 808 drum machine kick.
     @{
     
     @fn void    t808KickSmall_init               (t808KickSmall* const, LEAF* const leaf)
     @brief Initialize a t808KickSmall to the default mempool of a LEAF instance.
     @param kick A pointer to the t808KickSmall to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    t808KickSmall_initToPool         (t808KickSmall* const, tMempool* const)
     @brief Initialize a t808KickSmall to a specified mempool.
     @param kick A pointer to the t808KickSmall to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    t808KickSmall_free               (t808KickSmall* const)
     @brief Free a t808KickSmall from its mempool.
     @param kick A pointer to the t808KickSmall to free.
     
     @fn Lfloat   t808KickSmall_tick               (t808KickSmall* const)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @fn void    t808KickSmall_on                 (t808KickSmall* const, Lfloat vel)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @fn void    t808KickSmall_setToneFreq        (t808KickSmall* const, Lfloat freq)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @fn void    t808KickSmall_setToneDecay       (t808KickSmall* const, Lfloat decay)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @fn void    t808KickSmall_setNoiseDecay      (t808KickSmall* const, Lfloat decay)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @fn void    t808KickSmall_setSighAmount      (t808KickSmall* const, Lfloat sigh)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @fn void    t808KickSmall_setChirpAmount     (t808KickSmall* const, Lfloat chirp)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @fn void    t808KickSmall_setToneNoiseMix    (t808KickSmall* const, Lfloat toneNoiseMix)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @fn void    t808KickSmall_setNoiseFilterFreq (t808KickSmall* const, Lfloat noiseFilterFreq)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @fn void    t808KickSmall_setNoiseFilterQ    (t808KickSmall* const, Lfloat noiseFilterQ)
     @brief
     @param kick A pointer to the relevant t808KickSmall.
     
     @} */
    
    typedef struct _t808KickSmall
    {
        
        tMempool mempool;
        
        tCycle tone; // Tri
        tNoise noiseOsc;
        tSVF toneLowpass;
        tADSRS toneEnvOscChirp;
        tADSRS toneEnvOscSigh;
        tADSRS toneEnvGain;
        tADSRS noiseEnvGain;
        tADSRS toneEnvFilter;
        
        Lfloat toneGain;
        Lfloat noiseGain;
        
        Lfloat toneInitialFreq;
        Lfloat sighAmountInHz;
        Lfloat chirpRatioMinusOne;
        Lfloat noiseFilterFreq;
    } _t808KickSmall;
    
    typedef _t808KickSmall* t808KickSmall;
    
    void    t808KickSmall_init               (t808KickSmall* const, LEAF* const leaf);
    void    t808KickSmall_initToPool         (t808KickSmall* const, tMempool* const);
    void    t808KickSmall_free               (t808KickSmall* const);
    
    Lfloat  t808KickSmall_tick               (t808KickSmall const);

    void    t808KickSmall_on                 (t808KickSmall const, Lfloat vel);
    void    t808KickSmall_setToneFreq        (t808KickSmall const, Lfloat freq);
    void    t808KickSmall_setToneDecay       (t808KickSmall const, Lfloat decay);
    void    t808KickSmall_setNoiseDecay      (t808KickSmall const, Lfloat decay);
    void    t808KickSmall_setSighAmount      (t808KickSmall const, Lfloat sigh);
    void    t808KickSmall_setChirpAmount     (t808KickSmall const, Lfloat chirp);
    void    t808KickSmall_setToneNoiseMix    (t808KickSmall const, Lfloat toneNoiseMix);
    void    t808KickSmall_setNoiseFilterFreq (t808KickSmall const, Lfloat noiseFilterFreq);
    void    t808KickSmall_setNoiseFilterQ    (t808KickSmall const, Lfloat noiseFilterQ);
    void    t808KickSmall_setSampleRate      (t808KickSmall const, Lfloat sr);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_INSTRUMENTS_H_INCLUDED

//==============================================================================



