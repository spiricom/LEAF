/*
 ==============================================================================
 
 leaf-physical.h
 Created: 30 Nov 2018 10:41:55am
 Author:  airship
 
 ==============================================================================
 */

#ifndef LEAF_PHYSICAL_H_INCLUDED
#define LEAF_PHYSICAL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-global.h"
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-delay.h"
#include "leaf-filters.h"
#include "leaf-oscillators.h"
#include "leaf-envelopes.h"
#include "leaf-dynamics.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    /*!
     @defgroup tpluck tPluck
     @ingroup physical
     @brief Karplus-Strong model
     @{
     
     @fn void    tPluck_init          (tPluck* const, float lowestFrequency, LEAF* const leaf)
     @brief Initialize a tPluck to the default mempool of a LEAF instance.
     @param string A pointer to the tPluck to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPluck_initToPool    (tPluck* const, float lowestFrequency, tMempool* const)
     @brief Initialize a tPluck to a specified mempool.
     @param string A pointer to the tPluck to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPluck_free          (tPluck* const)
     @brief Free a tPluck from its mempool.
     @param string A pointer to the tPluck to free.
     
     @fn float   tPluck_tick          (tPluck* const)
     @brief
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_pluck         (tPluck* const, float amplitude)
     @brief Pluck the string.
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_noteOn        (tPluck* const, float frequency, float amplitude )
     @brief Start a note with the given frequency and amplitude.
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_noteOff       (tPluck* const, float amplitude )
     @brief Stop a note with the given amplitude (speed of decay)
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_setFrequency  (tPluck* const, float frequency )
     @brief Set instrument parameters for a particular frequency.
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_controlChange (tPluck* const, int number, float value)
     @brief Perform the control change specified by \e number and \e value (0.0 - 128.0).
     @param string A pointer to the relevant tPluck.
     
     @fn float   tPluck_getLastOut    (tPluck* const)
     @brief
     @param string A pointer to the relevant tPluck.
     
     @} */
    
    typedef struct _tPluck
    {
        
        tMempool mempool;
        
        tAllpassDelay     delayLine; // Allpass or Linear??  big difference...
        tOneZero    loopFilter;
        tOnePole    pickFilter;
        tNoise      noise;
        
        float lastOut;
        float loopGain;
        float lastFreq;
        
        float sampleRate;
    } _tPluck;
    
    typedef _tPluck* tPluck;
    
    void    tPluck_init          (tPluck* const, float lowestFrequency, LEAF* const leaf); //float delayBuff[DELAY_LENGTH]);
    void    tPluck_initToPool    (tPluck* const, float lowestFrequency, tMempool* const);
    void    tPluck_free          (tPluck* const);
    
    float   tPluck_tick          (tPluck* const);
    void    tPluck_pluck         (tPluck* const, float amplitude);
    void    tPluck_noteOn        (tPluck* const, float frequency, float amplitude );
    void    tPluck_noteOff       (tPluck* const, float amplitude );
    void    tPluck_setFrequency  (tPluck* const, float frequency );
    void    tPluck_controlChange (tPluck* const, int number, float value);
    float   tPluck_getLastOut    (tPluck* const);
    void    tPluck_setSampleRate (tPluck* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tkarplusstrong tKarplusStrong
     @ingroup physical
     @brief Stiff Karplus-Strong model
     @{
     
     @fn void    tKarplusStrong_init               (tKarplusStrong* const, float lowestFrequency, LEAF* const leaf)
     @brief Initialize a tKarplusStrong to the default mempool of a LEAF instance.
     @param string A pointer to the tKarplusStrong to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tKarplusStrong_initToPool         (tKarplusStrong* const, float lowestFrequency, tMempool* const)
     @brief Initialize a tKarplusStrong to a specified mempool.
     @param string A pointer to the tKarplusStrong to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tKarplusStrong_free               (tKarplusStrong* const)
     @brief Free a tKarplusStrong from its mempool.
     @param string A pointer to the tKarplusStrong to free.
     
     @fn float   tKarplusStrong_tick               (tKarplusStrong* const)
     @brief
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_pluck              (tKarplusStrong* const, float amplitude)
     @brief Pluck the string.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_noteOn             (tKarplusStrong* const, float frequency, float amplitude)
     @brief Start a note with the given frequency and amplitude.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_noteOff            (tKarplusStrong* const, float amplitude)
     @brief Stop a note with the given amplitude (speed of decay).
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_setFrequency       (tKarplusStrong* const, float frequency)
     @brief Set instrument parameters for a particular frequency.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_controlChange      (tKarplusStrong* const, SKControlType type, float value)
     @brief Perform the control change specified by \e number and \e value (0.0 - 128.0). Use SKPickPosition, SKStringDamping, or SKDetune for type.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_setStretch         (tKarplusStrong* const, float stretch)
     @brief Set the stretch "factor" of the string (0.0 - 1.0).
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_setPickupPosition  (tKarplusStrong* const, float position)
     @brief Set the pluck or "excitation" position along the string (0.0 - 1.0).
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong* const, float aGain )
     @brief Set the base loop gain.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn float   tKarplusStrong_getLastOut         (tKarplusStrong* const)
     @brief
     @param string A pointer to the relevant tKarplusStrong.
     
     @} */
    
    typedef enum SKControlType
    {
        SKPickPosition = 0,
        SKStringDamping,
        SKDetune,
        SKControlTypeNil
    } SKControlType;
    
    typedef struct _tKarplusStrong
    {
        
        tMempool mempool;
        
        tAllpassDelay  delayLine;
        tLinearDelay combDelay;
        tOneZero filter;
        tNoise   noise;
        tBiQuad  biquad[4];
        
        uint32_t length;
        float loopGain;
        float baseLoopGain;
        float lastFrequency;
        float lastLength;
        float stretching;
        float pluckAmplitude;
        float pickupPosition;
        
        float lastOut;
        
        float sampleRate;
    } _tKarplusStrong;
    
    typedef _tKarplusStrong* tKarplusStrong;
    
    void    tKarplusStrong_init               (tKarplusStrong* const, float lowestFrequency, LEAF* const leaf); // float delayBuff[2][DELAY_LENGTH]);
    void    tKarplusStrong_initToPool         (tKarplusStrong* const, float lowestFrequency, tMempool* const);
    void    tKarplusStrong_free               (tKarplusStrong* const);
    
    float   tKarplusStrong_tick               (tKarplusStrong* const);
    void    tKarplusStrong_pluck              (tKarplusStrong* const, float amplitude);
    void    tKarplusStrong_noteOn             (tKarplusStrong* const, float frequency, float amplitude );
    void    tKarplusStrong_noteOff            (tKarplusStrong* const, float amplitude );
    void    tKarplusStrong_setFrequency       (tKarplusStrong* const, float frequency );
    void    tKarplusStrong_controlChange      (tKarplusStrong* const, SKControlType type, float value);
    void    tKarplusStrong_setStretch         (tKarplusStrong* const, float stretch );
    void    tKarplusStrong_setPickupPosition  (tKarplusStrong* const, float position );
    void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong* const, float aGain );
    float   tKarplusStrong_getLastOut         (tKarplusStrong* const);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tsimplelivingstring tSimpleLivingString
     @ingroup physical
     @brief Simplified string model.
     @{
     
     @fn void    tSimpleLivingString_init   (tSimpleLivingString* const, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tSimpleLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tSimpleLivingString to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSimpleLivingString_initToPool     (tSimpleLivingString* const, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, tMempool* const)
     @brief Initialize a tSimpleLivingString to a specified mempool.
     @param string A pointer to the tSimpleLivingString to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSimpleLivingString_free                (tSimpleLivingString* const)
     @brief Free a tSimpleLivingString from its mempool.
     @param string A pointer to the tSimpleLivingString to free.
     
     @fn float   tSimpleLivingString_tick                (tSimpleLivingString* const, float input)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn float   tSimpleLivingString_sample              (tSimpleLivingString* const)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setFreq             (tSimpleLivingString* const, float freq)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setWaveLength       (tSimpleLivingString* const, float waveLength)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setDampFreq         (tSimpleLivingString* const, float dampFreq)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setDecay            (tSimpleLivingString* const, float decay)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setTargetLev        (tSimpleLivingString* const, float targetLev)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setLevSmoothFactor  (tSimpleLivingString* const, float levSmoothFactor)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setLevStrength      (tSimpleLivingString* const, float levStrength)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setLevMode          (tSimpleLivingString* const, int levMode)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @} */
    
    typedef struct _tSimpleLivingString
    {
        
        tMempool mempool;
        float freq, waveLengthInSamples;        // the frequency of the string, determining delay length
        float dampFreq;    // frequency for the bridge LP filter, in Hz
        float decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        float curr;
        tLinearDelay delayLine;
        tOnePole bridgeFilter;
        tHighpass DCblocker;
        tFeedbackLeveler fbLev;
        tExpSmooth wlSmooth;
        float sampleRate;
    } _tSimpleLivingString;
    
    typedef _tSimpleLivingString* tSimpleLivingString;
    
    void    tSimpleLivingString_init                (tSimpleLivingString* const, float freq, float dampFreq,
                                                     float decay, float targetLev, float levSmoothFactor,
                                                     float levStrength, int levMode, LEAF* const leaf);
    void    tSimpleLivingString_initToPool          (tSimpleLivingString* const, float freq, float dampFreq,
                                                     float decay, float targetLev, float levSmoothFactor,
                                                     float levStrength, int levMode, tMempool* const);
    void    tSimpleLivingString_free                (tSimpleLivingString* const);
    
    float   tSimpleLivingString_tick                (tSimpleLivingString* const, float input);
    float   tSimpleLivingString_sample              (tSimpleLivingString* const);
    void    tSimpleLivingString_setFreq             (tSimpleLivingString* const, float freq);
    void    tSimpleLivingString_setWaveLength       (tSimpleLivingString* const, float waveLength); // in samples
    void    tSimpleLivingString_setDampFreq         (tSimpleLivingString* const, float dampFreq);
    void    tSimpleLivingString_setDecay            (tSimpleLivingString* const, float decay); // should be near 1.0
    void    tSimpleLivingString_setTargetLev        (tSimpleLivingString* const, float targetLev);
    void    tSimpleLivingString_setLevSmoothFactor  (tSimpleLivingString* const, float levSmoothFactor);
    void    tSimpleLivingString_setLevStrength      (tSimpleLivingString* const, float levStrength);
    void    tSimpleLivingString_setLevMode          (tSimpleLivingString* const, int levMode);
    void    tSimpleLivingString_setSampleRate       (tSimpleLivingString* const, float sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tlivingstring tLivingString
     @ingroup physical
     @brief String model.
     @{
     
     @fn void    tLivingString_init                  (tLivingString* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tLivingString to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLivingString_initToPool            (tLivingString* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, tMempool* const)
     @brief Initialize a tLivingString to a specified mempool.
     @param string A pointer to the tLivingString to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tLivingString_free                  (tLivingString* const)
     @brief Free a tLivingString from its mempool.
     @param string A pointer to the tLivingString to free.
     
     @fn float   tLivingString_tick                  (tLivingString* const, float input)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn float   tLivingString_sample                (tLivingString* const)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setFreq               (tLivingString* const, float freq)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setWaveLength         (tLivingString* const, float waveLength)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setPickPos            (tLivingString* const, float pickPos)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setPrepIndex          (tLivingString* const, float prepIndex)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setDampFreq           (tLivingString* const, float dampFreq)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setDecay              (tLivingString* const, float decay)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setTargetLev          (tLivingString* const, float targetLev)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setLevSmoothFactor    (tLivingString* const, float levSmoothFactor)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setLevStrength        (tLivingString* const, float levStrength)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setLevMode            (tLivingString* const, int levMode)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @} */
    
    typedef struct _tLivingString
    {
        tMempool mempool;
        float freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
        float pickPos;    // the pick position, dividing the string in two, in ratio
        float prepIndex;    // the amount of pressure on the pickpoint of the string (near 0=soft obj, near 1=hard obj)
        float dampFreq;    // frequency for the bridge LP filter, in Hz
        float decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        float curr;
        tLinearDelay delLF,delUF,delUB,delLB;    // delay for lower/upper/forward/backward part of the waveguide model
        tOnePole bridgeFilter, nutFilter, prepFilterU, prepFilterL;
        tHighpass DCblockerL, DCblockerU;
        tFeedbackLeveler fbLevU, fbLevL;
        tExpSmooth wlSmooth, ppSmooth;
        float sampleRate;
    } _tLivingString;
    
    typedef _tLivingString* tLivingString;
    
    void    tLivingString_init                  (tLivingString* const, float freq, float pickPos, float prepIndex,
                                                 float dampFreq, float decay, float targetLev, float levSmoothFactor,
                                                 float levStrength, int levMode, LEAF* const leaf);
    void    tLivingString_initToPool            (tLivingString* const, float freq, float pickPos, float prepIndex,
                                                 float dampFreq, float decay, float targetLev, float levSmoothFactor,
                                                 float levStrength, int levMode, tMempool* const);
    void    tLivingString_free                  (tLivingString* const);
    
    float   tLivingString_tick                  (tLivingString* const, float input);
    float   tLivingString_sample                (tLivingString* const);
    void    tLivingString_setFreq               (tLivingString* const, float freq);
    void    tLivingString_setWaveLength         (tLivingString* const, float waveLength); // in samples
    void    tLivingString_setPickPos            (tLivingString* const, float pickPos);
    void    tLivingString_setPrepIndex          (tLivingString* const, float prepIndex);
    void    tLivingString_setDampFreq           (tLivingString* const, float dampFreq);
    void    tLivingString_setDecay              (tLivingString* const, float decay); // should be near 1.0
    void    tLivingString_setTargetLev          (tLivingString* const, float targetLev);
    void    tLivingString_setLevSmoothFactor    (tLivingString* const, float levSmoothFactor);
    void    tLivingString_setLevStrength        (tLivingString* const, float levStrength);
    void    tLivingString_setLevMode            (tLivingString* const, int levMode);
    void    tLivingString_setSampleRate         (tLivingString* const, float sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    
       /*!
     @defgroup tlivingstring2 tLivingString2
     @ingroup physical
     @brief String model with preparation and pick position separated.
     @{
     
     @fn void    tLivingString2_init                  (tLivingString2* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tLivingString2 to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLivingString2_initToPool            (tLivingString2* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, tMempool* const)
     @brief Initialize a tLivingString2 to a specified mempool.
     @param string A pointer to the tLivingString2 to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tLivingString2_free                  (tLivingString2* const)
     @brief Free a tLivingString2 from its mempool.
     @param string A pointer to the tLivingString2 to free.
     
     @fn float   tLivingString2_tick                  (tLivingString2* const, float input)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn float   tLivingString2_sample                (tLivingString2* const)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setFreq               (tLivingString2* const, float freq)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setWaveLength         (tLivingString2* const, float waveLength)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setPickPos            (tLivingString2* const, float pickPos)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setPrepPosition          (tLivingString2* const, float prepPos)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     
     @fn void    tLivingString2_setPrepIndex          (tLivingString2* const, float prepIndex)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setBrightness           (tLivingString2* const, float brightness)
     @brief
     @param string A pointer to the relevant tLivingString2.
     @param float The brightness parameter from 0 to 1.
     
     @fn void    tLivingString2_setDecay              (tLivingString2* const, float decay)
     @brief
     @param string A pointer to the relevant tLivingString2.
     @param float The decay parameter from 0 to 1.
     
     @fn void    tLivingString2_setTargetLev          (tLivingString2* const, float targetLev)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setLevSmoothFactor    (tLivingString2* const, float levSmoothFactor)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setLevStrength        (tLivingString2* const, float levStrength)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setLevMode            (tLivingString2* const, int levMode)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @} */
    typedef struct _tLivingString2
    {
        tMempool mempool;
        float freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
        float pickPos;    // the pick position, dividing the string in two, in ratio
        float prepPos;    // the preparation position, dividing the string in two, in ratio
        float pickupPos;    // the preparation position, dividing the string in two, in ratio
        float prepIndex;    // the amount of pressure on the preparation position of the string (near 0=soft obj, near 1=hard obj)
        float decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        float brightness;
        float curr;
        tHermiteDelay delLF,delUF,delUB,delLB;    // delay for lower/upper/forward/backward part of the waveguide model
        tTwoZero bridgeFilter, nutFilter, prepFilterU, prepFilterL;
        tHighpass DCblockerL, DCblockerU;
        tFeedbackLeveler fbLevU, fbLevL;
        tExpSmooth wlSmooth, ppSmooth, prpSmooth, puSmooth;
        float sampleRate;
    } _tLivingString2;
    
    typedef _tLivingString2* tLivingString2;
    
    void    tLivingString2_init                  (tLivingString2* const, float freq, float pickPos, float prepPos, float pickupPos, float prepIndex,
                                                  float brightness, float decay, float targetLev, float levSmoothFactor,
                                                  float levStrength, int levMode, LEAF* const leaf);
    void    tLivingString2_initToPool            (tLivingString2* const, float freq, float pickPos, float prepPos, float pickupPos, float prepIndex,
                                                  float brightness, float decay, float targetLev, float levSmoothFactor,
                                                  float levStrength, int levMode, tMempool* const);
    void    tLivingString2_free                  (tLivingString2* const);
    
    float   tLivingString2_tick                  (tLivingString2* const, float input);
    float   tLivingString2_tickEfficient                 (tLivingString2* const, float input);
    float   tLivingString2_udpateDelays(tLivingString2* const pl); //necessary if using tickEfficient (so that parameter setting can be put in a slower process). included in standard tick.
    float   tLivingString2_sample                (tLivingString2* const);
    void    tLivingString2_setFreq               (tLivingString2* const, float freq);
    void    tLivingString2_setWaveLength         (tLivingString2* const, float waveLength); // in samples
    void    tLivingString2_setPickPos            (tLivingString2* const, float pickPos);
    void    tLivingString2_setPrepPos            (tLivingString2* const, float prepPos);
    void    tLivingString2_setPickupPos            (tLivingString2* const, float pickupPos);
    void    tLivingString2_setPrepIndex          (tLivingString2* const, float prepIndex);
    void    tLivingString2_setBrightness         (tLivingString2* const, float brightness);
    void    tLivingString2_setDecay              (tLivingString2* const, float decay); // from 0 to 1, gets converted to real decay factor
    void    tLivingString2_setTargetLev          (tLivingString2* const, float targetLev);
    void    tLivingString2_setLevSmoothFactor    (tLivingString2* const, float levSmoothFactor);
    void    tLivingString2_setLevStrength        (tLivingString2* const, float levStrength);
    void    tLivingString2_setLevMode            (tLivingString2* const, int levMode);
    void    tLivingString2_setSampleRate         (tLivingString2* const, float sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tcomplexlivingstring tComplexLivingString
     @ingroup physical
     @brief Complex string model.
     @{
     
     @fn void    tComplexLivingString_init  (tComplexLivingString* const, float freq, float pickPos, float prepPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tComplexLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tComplexLivingString to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tComplexLivingString_initToPool    (tComplexLivingString* const, float freq, float pickPos, float prepPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, tMempool* const)
     @brief Initialize a tComplexLivingString to a specified mempool.
     @param string A pointer to the tComplexLivingString to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tComplexLivingString_free                  (tComplexLivingString* const)
     @brief Free a tComplexLivingString from its mempool.
     @param string A pointer to the tComplexLivingString to free.
     
     @fn float   tComplexLivingString_tick                  (tComplexLivingString* const, float input)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn float   tComplexLivingString_sample                (tComplexLivingString* const)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setFreq               (tComplexLivingString* const, float freq)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setWaveLength         (tComplexLivingString* const, float waveLength)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setPickPos            (tComplexLivingString* const, float pickPos)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setPrepPos            (tComplexLivingString* const, float prepPos)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setPrepIndex          (tComplexLivingString* const, float prepIndex)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setDampFreq           (tComplexLivingString* const, float dampFreq)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setDecay              (tComplexLivingString* const, float decay)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setTargetLev          (tComplexLivingString* const, float targetLev)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setLevSmoothFactor    (tComplexLivingString* const, float levSmoothFactor)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setLevStrength        (tComplexLivingString* const, float levStrength)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setLevMode            (tComplexLivingString* const, int levMode)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @} */
    
    typedef struct _tComplexLivingString
    {
        tMempool mempool;
        float freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
        float pickPos;    // the pick position, dividing the string, in ratio
        float prepPos;    // preparation position, in ratio
        int prepLower;
        float prepIndex;    // the amount of pressure on the pickpoint of the string (near 0=soft obj, near 1=hard obj)
        float dampFreq;    // frequency for the bridge LP filter, in Hz
        float decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        float curr;
        tLinearDelay delLF,delUF, delMF, delMB, delUB,delLB;    // delay for lower/upper/forward/backward part of the waveguide model
        tOnePole bridgeFilter, nutFilter, prepFilterU, prepFilterL;
        tHighpass DCblockerL, DCblockerU;
        tFeedbackLeveler fbLevU, fbLevL;
        tExpSmooth wlSmooth, pickPosSmooth, prepPosSmooth;
        float sampleRate;
    } _tComplexLivingString;
    
    typedef _tComplexLivingString* tComplexLivingString;
    
    void    tComplexLivingString_init                  (tComplexLivingString* const, float freq, float pickPos,
                                                        float prepPos, float prepIndex, float dampFreq,
                                                        float decay, float targetLev, float levSmoothFactor,
                                                        float levStrength, int levMode, LEAF* const leaf);
    void    tComplexLivingString_initToPool            (tComplexLivingString* const, float freq, float pickPos,
                                                        float prepPos, float prepIndex, float dampFreq,
                                                        float decay, float targetLev, float levSmoothFactor,
                                                        float levStrength, int levMode, tMempool* const);
    void    tComplexLivingString_free                  (tComplexLivingString* const);
    
    float   tComplexLivingString_tick                  (tComplexLivingString* const, float input);
    float   tComplexLivingString_sample                (tComplexLivingString* const);
    void    tComplexLivingString_setFreq               (tComplexLivingString* const, float freq);
    void    tComplexLivingString_setWaveLength         (tComplexLivingString* const, float waveLength); // in samples
    void    tComplexLivingString_setPickPos            (tComplexLivingString* const, float pickPos);
    void    tComplexLivingString_setPrepPos            (tComplexLivingString* const, float prepPos);
    void    tComplexLivingString_setPrepIndex          (tComplexLivingString* const, float prepIndex);
    void    tComplexLivingString_setDampFreq           (tComplexLivingString* const, float dampFreq);
    void    tComplexLivingString_setDecay              (tComplexLivingString* const, float decay); // should be near 1.0
    void    tComplexLivingString_setTargetLev          (tComplexLivingString* const, float targetLev);
    void    tComplexLivingString_setLevSmoothFactor    (tComplexLivingString* const, float levSmoothFactor);
    void    tComplexLivingString_setLevStrength        (tComplexLivingString* const, float levStrength);
    void    tComplexLivingString_setLevMode            (tComplexLivingString* const, int levMode);
    void    tComplexLivingString_setSampleRate         (tComplexLivingString* const, float sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    
    /*!
     @defgroup treedtable tReedTable
     @ingroup physical
     @brief Reed Table - borrowed from STK
     @{
     
     @fn void    tReedTable_init         (tReedTable* const, float offset, float slope, LEAF* const leaf)
     @brief Initialize a tReedTable to the default mempool of a LEAF instance.
     @param reed A pointer to the tReedTable to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tReedTable_initToPool   (tReedTable* const, float offset, float slope, tMempool* const)
     @brief Initialize a tReedTable to a specified mempool.
     @param reed A pointer to the tReedTable to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tReedTable_free         (tReedTable* const)
     @brief Free a tReedTable from its mempool.
     @param reed A pointer to the tReedTable to free.
     
     @fn float   tReedTable_tick         (tReedTable* const, float input)
     @brief
     @param reed A pointer to the relevant tReedTable.
     
     @fn float   tReedTable_tanh_tick    (tReedTable* const, float input)
     @brief
     @param reed A pointer to the relevant tReedTable.
     
     @fn void    tReedTable_setOffset    (tReedTable* const, float offset)
     @brief
     @param reed A pointer to the relevant tReedTable.
     
     @fn void    tReedTable_setSlope     (tReedTable* const, float slope)
     @brief
     @param reed A pointer to the relevant tReedTable.
     
     @} */
    
    typedef struct _tReedTable
    {
        
        tMempool mempool;
        float offset, slope;
    } _tReedTable;
    
    typedef _tReedTable* tReedTable;
    
    void    tReedTable_init         (tReedTable* const, float offset, float slope, LEAF* const leaf);
    void    tReedTable_initToPool   (tReedTable* const, float offset, float slope, tMempool* const);
    void    tReedTable_free         (tReedTable* const);
    
    float   tReedTable_tick         (tReedTable* const, float input);
    float   tReedTable_tanh_tick    (tReedTable* const, float input); //tanh softclip version of reed table - replacing the hard clip in original stk code
    void    tReedTable_setOffset    (tReedTable* const, float offset);
    void    tReedTable_setSlope     (tReedTable* const, float slope);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_PHYSICAL_H_INCLUDED

//==============================================================================


