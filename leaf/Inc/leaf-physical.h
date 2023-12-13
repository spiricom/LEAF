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
     
     @fn void    tPluck_init          (tPluck* const, Lfloat lowestFrequency, LEAF* const leaf)
     @brief Initialize a tPluck to the default mempool of a LEAF instance.
     @param string A pointer to the tPluck to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPluck_initToPool    (tPluck* const, Lfloat lowestFrequency, tMempool* const)
     @brief Initialize a tPluck to a specified mempool.
     @param string A pointer to the tPluck to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPluck_free          (tPluck* const)
     @brief Free a tPluck from its mempool.
     @param string A pointer to the tPluck to free.
     
     @fn Lfloat   tPluck_tick          (tPluck* const)
     @brief
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_pluck         (tPluck* const, Lfloat amplitude)
     @brief Pluck the string.
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_noteOn        (tPluck* const, Lfloat frequency, Lfloat amplitude )
     @brief Start a note with the given frequency and amplitude.
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_noteOff       (tPluck* const, Lfloat amplitude )
     @brief Stop a note with the given amplitude (speed of decay)
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_setFrequency  (tPluck* const, Lfloat frequency )
     @brief Set instrument parameters for a particular frequency.
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_controlChange (tPluck* const, int number, Lfloat value)
     @brief Perform the control change specified by \e number and \e value (0.0 - 128.0).
     @param string A pointer to the relevant tPluck.
     
     @fn Lfloat   tPluck_getLastOut    (tPluck* const)
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
        
        Lfloat lastOut;
        Lfloat loopGain;
        Lfloat lastFreq;
        
        Lfloat sampleRate;
    } _tPluck;
    
    typedef _tPluck* tPluck;
    
    void    tPluck_init          (tPluck* const, Lfloat lowestFrequency, LEAF* const leaf); //Lfloat delayBuff[DELAY_LENGTH]);
    void    tPluck_initToPool    (tPluck* const, Lfloat lowestFrequency, tMempool* const);
    void    tPluck_free          (tPluck* const);
    
    Lfloat   tPluck_tick          (tPluck* const);
    void    tPluck_pluck         (tPluck* const, Lfloat amplitude);
    void    tPluck_noteOn        (tPluck* const, Lfloat frequency, Lfloat amplitude );
    void    tPluck_noteOff       (tPluck* const, Lfloat amplitude );
    void    tPluck_setFrequency  (tPluck* const, Lfloat frequency );
    void    tPluck_controlChange (tPluck* const, int number, Lfloat value);
    Lfloat   tPluck_getLastOut    (tPluck* const);
    void    tPluck_setSampleRate (tPluck* const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tkarplusstrong tKarplusStrong
     @ingroup physical
     @brief Stiff Karplus-Strong model
     @{
     
     @fn void    tKarplusStrong_init               (tKarplusStrong* const, Lfloat lowestFrequency, LEAF* const leaf)
     @brief Initialize a tKarplusStrong to the default mempool of a LEAF instance.
     @param string A pointer to the tKarplusStrong to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tKarplusStrong_initToPool         (tKarplusStrong* const, Lfloat lowestFrequency, tMempool* const)
     @brief Initialize a tKarplusStrong to a specified mempool.
     @param string A pointer to the tKarplusStrong to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tKarplusStrong_free               (tKarplusStrong* const)
     @brief Free a tKarplusStrong from its mempool.
     @param string A pointer to the tKarplusStrong to free.
     
     @fn Lfloat   tKarplusStrong_tick               (tKarplusStrong* const)
     @brief
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_pluck              (tKarplusStrong* const, Lfloat amplitude)
     @brief Pluck the string.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_noteOn             (tKarplusStrong* const, Lfloat frequency, Lfloat amplitude)
     @brief Start a note with the given frequency and amplitude.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_noteOff            (tKarplusStrong* const, Lfloat amplitude)
     @brief Stop a note with the given amplitude (speed of decay).
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_setFrequency       (tKarplusStrong* const, Lfloat frequency)
     @brief Set instrument parameters for a particular frequency.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_controlChange      (tKarplusStrong* const, SKControlType type, Lfloat value)
     @brief Perform the control change specified by \e number and \e value (0.0 - 128.0). Use SKPickPosition, SKStringDamping, or SKDetune for type.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_setStretch         (tKarplusStrong* const, Lfloat stretch)
     @brief Set the stretch "factor" of the string (0.0 - 1.0).
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_setPickupPosition  (tKarplusStrong* const, Lfloat position)
     @brief Set the pluck or "excitation" position along the string (0.0 - 1.0).
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong* const, Lfloat aGain )
     @brief Set the base loop gain.
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn Lfloat   tKarplusStrong_getLastOut         (tKarplusStrong* const)
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
        Lfloat loopGain;
        Lfloat baseLoopGain;
        Lfloat lastFrequency;
        Lfloat lastLength;
        Lfloat stretching;
        Lfloat pluckAmplitude;
        Lfloat pickupPosition;
        
        Lfloat lastOut;
        
        Lfloat sampleRate;
    } _tKarplusStrong;
    
    typedef _tKarplusStrong* tKarplusStrong;
    
    void    tKarplusStrong_init               (tKarplusStrong* const, Lfloat lowestFrequency, LEAF* const leaf); // Lfloat delayBuff[2][DELAY_LENGTH]);
    void    tKarplusStrong_initToPool         (tKarplusStrong* const, Lfloat lowestFrequency, tMempool* const);
    void    tKarplusStrong_free               (tKarplusStrong* const);
    
    Lfloat   tKarplusStrong_tick               (tKarplusStrong* const);
    void    tKarplusStrong_pluck              (tKarplusStrong* const, Lfloat amplitude);
    void    tKarplusStrong_noteOn             (tKarplusStrong* const, Lfloat frequency, Lfloat amplitude );
    void    tKarplusStrong_noteOff            (tKarplusStrong* const, Lfloat amplitude );
    void    tKarplusStrong_setFrequency       (tKarplusStrong* const, Lfloat frequency );
    void    tKarplusStrong_controlChange      (tKarplusStrong* const, SKControlType type, Lfloat value);
    void    tKarplusStrong_setStretch         (tKarplusStrong* const, Lfloat stretch );
    void    tKarplusStrong_setPickupPosition  (tKarplusStrong* const, Lfloat position );
    void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong* const, Lfloat aGain );
    Lfloat   tKarplusStrong_getLastOut         (tKarplusStrong* const);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tsimplelivingstring tSimpleLivingString
     @ingroup physical
     @brief Simplified string model.
     @{
     
     @fn void    tSimpleLivingString_init   (tSimpleLivingString* const, Lfloat freq, Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor, Lfloat levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tSimpleLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tSimpleLivingString to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSimpleLivingString_initToPool     (tSimpleLivingString* const, Lfloat freq, Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor, Lfloat levStrength, int levMode, tMempool* const)
     @brief Initialize a tSimpleLivingString to a specified mempool.
     @param string A pointer to the tSimpleLivingString to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSimpleLivingString_free                (tSimpleLivingString* const)
     @brief Free a tSimpleLivingString from its mempool.
     @param string A pointer to the tSimpleLivingString to free.
     
     @fn Lfloat   tSimpleLivingString_tick                (tSimpleLivingString* const, Lfloat input)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn Lfloat   tSimpleLivingString_sample              (tSimpleLivingString* const)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setFreq             (tSimpleLivingString* const, Lfloat freq)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setWaveLength       (tSimpleLivingString* const, Lfloat waveLength)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setDampFreq         (tSimpleLivingString* const, Lfloat dampFreq)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setDecay            (tSimpleLivingString* const, Lfloat decay)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setTargetLev        (tSimpleLivingString* const, Lfloat targetLev)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setLevSmoothFactor  (tSimpleLivingString* const, Lfloat levSmoothFactor)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setLevStrength      (tSimpleLivingString* const, Lfloat levStrength)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @fn void    tSimpleLivingString_setLevMode          (tSimpleLivingString* const, int levMode)
     @brief
     @param string A pointer to the relevant tSimpleLivingString.
     
     @} */
    
    typedef struct _tSimpleLivingString
    {
        
        tMempool mempool;
        Lfloat freq, waveLengthInSamples;        // the frequency of the string, determining delay length
        Lfloat dampFreq;    // frequency for the bridge LP filter, in Hz
        Lfloat decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        Lfloat curr;
        tLinearDelay delayLine;
        tOnePole bridgeFilter;
        tHighpass DCblocker;
        tFeedbackLeveler fbLev;
        tExpSmooth wlSmooth;
        Lfloat sampleRate;
    } _tSimpleLivingString;
    
    typedef _tSimpleLivingString* tSimpleLivingString;
    
    void    tSimpleLivingString_init                (tSimpleLivingString* const, Lfloat freq, Lfloat dampFreq,
                                                     Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                     Lfloat levStrength, int levMode, LEAF* const leaf);
    void    tSimpleLivingString_initToPool          (tSimpleLivingString* const, Lfloat freq, Lfloat dampFreq,
                                                     Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                     Lfloat levStrength, int levMode, tMempool* const);
    void    tSimpleLivingString_free                (tSimpleLivingString* const);
    
    Lfloat   tSimpleLivingString_tick                (tSimpleLivingString* const, Lfloat input);
    Lfloat   tSimpleLivingString_sample              (tSimpleLivingString* const);
    void    tSimpleLivingString_setFreq             (tSimpleLivingString* const, Lfloat freq);
    void    tSimpleLivingString_setWaveLength       (tSimpleLivingString* const, Lfloat waveLength); // in samples
    void    tSimpleLivingString_setDampFreq         (tSimpleLivingString* const, Lfloat dampFreq);
    void    tSimpleLivingString_setDecay            (tSimpleLivingString* const, Lfloat decay); // should be near 1.0
    void    tSimpleLivingString_setTargetLev        (tSimpleLivingString* const, Lfloat targetLev);
    void    tSimpleLivingString_setLevSmoothFactor  (tSimpleLivingString* const, Lfloat levSmoothFactor);
    void    tSimpleLivingString_setLevStrength      (tSimpleLivingString* const, Lfloat levStrength);
    void    tSimpleLivingString_setLevMode          (tSimpleLivingString* const, int levMode);
    void    tSimpleLivingString_setSampleRate       (tSimpleLivingString* const, Lfloat sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~


    typedef struct _tSimpleLivingString2
    {

        tMempool mempool;
        Lfloat freq, waveLengthInSamples;        // the frequency of the string, determining delay length
        Lfloat brightness;    // frequency for the bridge LP filter, in Hz
        Lfloat decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        Lfloat curr;
        tHermiteDelay delayLine;
        tTwoZero bridgeFilter;
        tHighpass DCblocker;
        tFeedbackLeveler fbLev;
        tExpSmooth wlSmooth;
        Lfloat sampleRate;
    } _tSimpleLivingString2;

    typedef _tSimpleLivingString2* tSimpleLivingString2;

    void    tSimpleLivingString2_init                (tSimpleLivingString2* const, Lfloat freq, Lfloat brightness,
                                                     Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                     Lfloat levStrength, int levMode, LEAF* const leaf);
    void    tSimpleLivingString2_initToPool          (tSimpleLivingString2* const, Lfloat freq, Lfloat brightness,
                                                     Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                     Lfloat levStrength, int levMode, tMempool* const);
    void    tSimpleLivingString2_free                (tSimpleLivingString2* const);

    Lfloat   tSimpleLivingString2_tick                (tSimpleLivingString2* const, Lfloat input);
    Lfloat   tSimpleLivingString2_sample              (tSimpleLivingString2* const);
    void    tSimpleLivingString2_setFreq             (tSimpleLivingString2* const, Lfloat freq);
    void    tSimpleLivingString2_setWaveLength       (tSimpleLivingString2* const, Lfloat waveLength); // in samples
    void    tSimpleLivingString2_setBrightness         (tSimpleLivingString2* const, Lfloat brightness);
    void    tSimpleLivingString2_setDecay            (tSimpleLivingString2* const, Lfloat decay); // should be near 1.0
    void    tSimpleLivingString2_setTargetLev        (tSimpleLivingString2* const, Lfloat targetLev);
    void    tSimpleLivingString2_setLevSmoothFactor  (tSimpleLivingString2* const, Lfloat levSmoothFactor);
    void    tSimpleLivingString2_setLevStrength      (tSimpleLivingString2* const, Lfloat levStrength);
    void    tSimpleLivingString2_setLevMode          (tSimpleLivingString2* const, int levMode);
    void    tSimpleLivingString2_setSampleRate       (tSimpleLivingString2* const, Lfloat sr);

    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~


typedef struct _tSimpleLivingString3
{
    
    tMempool mempool;
    Lfloat freq, waveLengthInSamples;        // the frequency of the string, determining delay length
    Lfloat dampFreq;    // frequency for the bridge LP filter, in Hz
    Lfloat decay, userDecay; // amplitude damping factor for the string (only active in mode 0)
    Lfloat prevDelayLength;
    Lfloat changeGainCompensator;
    int levMode;
    Lfloat curr;
    Lfloat Uout;
    Lfloat Lout;
    int maxLength;
    tLinearDelay delayLineU;
    tLinearDelay delayLineL;
    tOnePole bridgeFilter;
    tBiQuad bridgeFilter2;
    Lfloat temp1;
    Lfloat temp2;
    Lfloat pickupPoint;
    tHighpass DCblocker;
    tFeedbackLeveler fbLev;
    tExpSmooth wlSmooth;
    int oversampling;
    Lfloat sampleRate;
    Lfloat rippleGain;
    Lfloat rippleDelay;
} _tSimpleLivingString3;

typedef _tSimpleLivingString3* tSimpleLivingString3;

void    tSimpleLivingString3_init                (tSimpleLivingString3* const, int oversampling, Lfloat freq, Lfloat dampFreq,
                                                 Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                 Lfloat levStrength, int levMode, LEAF* const leaf);
void    tSimpleLivingString3_initToPool  (tSimpleLivingString3* const pl, int oversampling, Lfloat freq, Lfloat dampFreq,
                                         Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                          Lfloat levStrength, int levMode, tMempool* const mp);
void    tSimpleLivingString3_free                (tSimpleLivingString3* const);

void   tSimpleLivingString3_pluck              (tSimpleLivingString3* const pl, Lfloat input, Lfloat position);
Lfloat   tSimpleLivingString3_tick                (tSimpleLivingString3* const, Lfloat input);
void   tSimpleLivingString3_setPickupPoint(tSimpleLivingString3* const pl, Lfloat pickupPoint);
Lfloat   tSimpleLivingString3_sample              (tSimpleLivingString3* const);
void    tSimpleLivingString3_setFreq             (tSimpleLivingString3* const, Lfloat freq);
void    tSimpleLivingString3_setWaveLength       (tSimpleLivingString3* const, Lfloat waveLength); // in samples
void    tSimpleLivingString3_setDampFreq         (tSimpleLivingString3* const, Lfloat dampFreq);
void    tSimpleLivingString3_setDecay            (tSimpleLivingString3* const, Lfloat decay); // should be near 1.0
void    tSimpleLivingString3_setTargetLev        (tSimpleLivingString3* const, Lfloat targetLev);
void    tSimpleLivingString3_setLevSmoothFactor  (tSimpleLivingString3* const, Lfloat levSmoothFactor);
void    tSimpleLivingString3_setLevStrength      (tSimpleLivingString3* const, Lfloat levStrength);
void    tSimpleLivingString3_setLevMode          (tSimpleLivingString3* const, int levMode);
void    tSimpleLivingString3_setSampleRate       (tSimpleLivingString3* const, Lfloat sr);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
typedef struct _tSimpleLivingString4
{
    
    tMempool mempool;
    Lfloat freq, waveLengthInSamples;        // the frequency of the string, determining delay length
    Lfloat dampFreq;    // frequency for the bridge LP filter, in Hz
    Lfloat decay, userDecay; // amplitude damping factor for the string (only active in mode 0)
    Lfloat prevDelayLength;
    Lfloat changeGainCompensator;
    int levMode;
    Lfloat curr;
    Lfloat Uout;
    Lfloat Lout;
    int maxLength;
    tLinearDelay delayLineU;
    tLinearDelay delayLineL;
    tOnePole bridgeFilter;
    tBiQuad bridgeFilter2;
    Lfloat temp1;
    Lfloat temp2;
    Lfloat pickupPoint;
    Lfloat pluckPosition;
    tHighpass DCblocker;
    tFeedbackLeveler fbLev;
    tExpSmooth wlSmooth;
    int oversampling;
    Lfloat sampleRate;
    Lfloat rippleGain;
    Lfloat rippleDelay;
} _tSimpleLivingString4;

typedef _tSimpleLivingString4* tSimpleLivingString4;

void    tSimpleLivingString4_init                (tSimpleLivingString4* const, int oversampling, Lfloat freq, Lfloat dampFreq,
                                                 Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                 Lfloat levStrength, int levMode, LEAF* const leaf);
void    tSimpleLivingString4_initToPool  (tSimpleLivingString4* const pl, int oversampling, Lfloat freq, Lfloat dampFreq,
                                         Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                          Lfloat levStrength, int levMode, tMempool* const mp);
void    tSimpleLivingString4_free                (tSimpleLivingString4* const);

void   tSimpleLivingString4_pluck              (tSimpleLivingString4* const pl, Lfloat input, Lfloat position);
void   tSimpleLivingString4_pluckNoPosition             (tSimpleLivingString4* const pl, Lfloat input);
void   tSimpleLivingString4_setPluckPosition(tSimpleLivingString4* const pl, Lfloat position);
Lfloat   tSimpleLivingString4_tick                (tSimpleLivingString4* const, Lfloat input);
void   tSimpleLivingString4_setPickupPoint(tSimpleLivingString4* const pl, Lfloat pickupPoint);
Lfloat   tSimpleLivingString4_sample              (tSimpleLivingString4* const);
void    tSimpleLivingString4_setFreq             (tSimpleLivingString4* const, Lfloat freq);
void    tSimpleLivingString4_setWaveLength       (tSimpleLivingString4* const, Lfloat waveLength); // in samples
void    tSimpleLivingString4_setDampFreq         (tSimpleLivingString4* const, Lfloat dampFreq);
void    tSimpleLivingString4_setDecay            (tSimpleLivingString4* const, Lfloat decay); // should be near 1.0
void    tSimpleLivingString4_setTargetLev        (tSimpleLivingString4* const, Lfloat targetLev);
void    tSimpleLivingString4_setLevSmoothFactor  (tSimpleLivingString4* const, Lfloat levSmoothFactor);
void    tSimpleLivingString4_setLevStrength      (tSimpleLivingString4* const, Lfloat levStrength);
void    tSimpleLivingString4_setLevMode          (tSimpleLivingString4* const, int levMode);
void    tSimpleLivingString4_setSampleRate       (tSimpleLivingString4* const, Lfloat sr);


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
typedef struct _tSimpleLivingString5
{
    
    tMempool mempool;
    Lfloat freq, waveLengthInSamples;        // the frequency of the string, determining delay length
    Lfloat dampFreq;    // frequency for the bridge LP filter, in Hz
    Lfloat decay, userDecay; // amplitude damping factor for the string (only active in mode 0)
    Lfloat prevDelayLength;
    Lfloat changeGainCompensator;
    int levMode;
    Lfloat curr;
    Lfloat Uout;
    Lfloat Lout;
    int maxLength;
    Lfloat prepIndex;
    Lfloat prepPos;
    tLinearDelay delLF,delUF,delUB,delLB;    // delay for lower/upper/forward/backward part of the waveguide model
    tOnePole bridgeFilter, nutFilter, prepFilterU, prepFilterL;
    Lfloat temp1;
    Lfloat temp2;
    Lfloat pickupPoint;
    Lfloat pluckPosition;
    tHighpass DCblocker;
    tHighpass DCblocker2;
    tFeedbackLeveler fbLev;
    tFeedbackLeveler fbLev2;
    tWavefolder wf1;
    tWavefolder wf2;
    tWavefolder wf3;
    tWavefolder wf4;
    tExpSmooth wlSmooth, prepPosSmooth, prepIndexSmooth;
    int oversampling;
    Lfloat sampleRate;
    Lfloat rippleGain;
    Lfloat rippleDelay;
    Lfloat ff;
    Lfloat fb;
} _tSimpleLivingString5;

typedef _tSimpleLivingString5* tSimpleLivingString5;

void    tSimpleLivingString5_init                (tSimpleLivingString5* const, int oversampling, Lfloat freq, Lfloat dampFreq,
                                                 Lfloat decay, Lfloat prepPos, Lfloat prepIndex, Lfloat pluckPos, Lfloat targetLev, Lfloat levSmoothFactor,
                                                 Lfloat levStrength, int levMode, LEAF* const leaf);
void    tSimpleLivingString5_initToPool  (tSimpleLivingString5* const pl, int oversampling, Lfloat freq, Lfloat dampFreq,
                                         Lfloat decay, Lfloat prepPos, Lfloat prepIndex, Lfloat pluckPos, Lfloat targetLev, Lfloat levSmoothFactor,
                                          Lfloat levStrength, int levMode, tMempool* const mp);
void    tSimpleLivingString5_free                (tSimpleLivingString5* const);

void   tSimpleLivingString5_pluck              (tSimpleLivingString5* const pl, Lfloat input, Lfloat position);
void   tSimpleLivingString5_pluckNoPosition             (tSimpleLivingString5* const pl, Lfloat input);
void   tSimpleLivingString5_setPluckPosition(tSimpleLivingString5* const pl, Lfloat position);
void   tSimpleLivingString5_setPrepPosition(tSimpleLivingString5* const pl, Lfloat prepPosition);
void   tSimpleLivingString5_setPrepIndex(tSimpleLivingString5* const pl, Lfloat prepIndex);
Lfloat   tSimpleLivingString5_tick                (tSimpleLivingString5* const, Lfloat input);
void   tSimpleLivingString5_setPickupPoint(tSimpleLivingString5* const pl, Lfloat pickupPoint);
Lfloat   tSimpleLivingString5_sample              (tSimpleLivingString5* const);
void    tSimpleLivingString5_setFreq             (tSimpleLivingString5* const, Lfloat freq);
void    tSimpleLivingString5_setWaveLength       (tSimpleLivingString5* const, Lfloat waveLength); // in samples
void    tSimpleLivingString5_setDampFreq         (tSimpleLivingString5* const, Lfloat dampFreq);
void    tSimpleLivingString5_setDecay            (tSimpleLivingString5* const, Lfloat decay); // should be near 1.0
void    tSimpleLivingString5_setTargetLev        (tSimpleLivingString5* const, Lfloat targetLev);
void    tSimpleLivingString5_setLevSmoothFactor  (tSimpleLivingString5* const, Lfloat levSmoothFactor);
void    tSimpleLivingString5_setLevStrength      (tSimpleLivingString5* const, Lfloat levStrength);
void    tSimpleLivingString5_setLevMode          (tSimpleLivingString5* const, int levMode);
void    tSimpleLivingString5_setSampleRate       (tSimpleLivingString5* const, Lfloat sr);
void   tSimpleLivingString5_setFBAmount(tSimpleLivingString5* const pl, Lfloat fb);
void   tSimpleLivingString5_setFFAmount(tSimpleLivingString5* const pl, Lfloat ff);
void   tSimpleLivingString5_setFoldDepth(tSimpleLivingString5* const pl, Lfloat depth);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~




    /*!
     @defgroup tlivingstring tLivingString
     @ingroup physical
     @brief String model.
     @{
     
     @fn void    tLivingString_init                  (tLivingString* const, Lfloat freq, Lfloat pickPos, Lfloat prepIndex, Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor, Lfloat levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tLivingString to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLivingString_initToPool            (tLivingString* const, Lfloat freq, Lfloat pickPos, Lfloat prepIndex, Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor, Lfloat levStrength, int levMode, tMempool* const)
     @brief Initialize a tLivingString to a specified mempool.
     @param string A pointer to the tLivingString to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tLivingString_free                  (tLivingString* const)
     @brief Free a tLivingString from its mempool.
     @param string A pointer to the tLivingString to free.
     
     @fn Lfloat   tLivingString_tick                  (tLivingString* const, Lfloat input)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn Lfloat   tLivingString_sample                (tLivingString* const)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setFreq               (tLivingString* const, Lfloat freq)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setWaveLength         (tLivingString* const, Lfloat waveLength)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setPickPos            (tLivingString* const, Lfloat pickPos)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setPrepIndex          (tLivingString* const, Lfloat prepIndex)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setDampFreq           (tLivingString* const, Lfloat dampFreq)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setDecay              (tLivingString* const, Lfloat decay)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setTargetLev          (tLivingString* const, Lfloat targetLev)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setLevSmoothFactor    (tLivingString* const, Lfloat levSmoothFactor)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setLevStrength        (tLivingString* const, Lfloat levStrength)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @fn void    tLivingString_setLevMode            (tLivingString* const, int levMode)
     @brief
     @param string A pointer to the relevant tLivingString.
     
     @} */
    
    typedef struct _tLivingString
    {
        tMempool mempool;
        Lfloat freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
        Lfloat pickPos;    // the pick position, dividing the string in two, in ratio
        Lfloat prepIndex;    // the amount of pressure on the pickpoint of the string (near 0=soft obj, near 1=hard obj)
        Lfloat dampFreq;    // frequency for the bridge LP filter, in Hz
        Lfloat decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        Lfloat curr;
        tLinearDelay delLF,delUF,delUB,delLB;    // delay for lower/upper/forward/backward part of the waveguide model
        tOnePole bridgeFilter, nutFilter, prepFilterU, prepFilterL;
        tHighpass DCblockerL, DCblockerU;
        tFeedbackLeveler fbLevU, fbLevL;
        tExpSmooth wlSmooth, ppSmooth;
        Lfloat sampleRate;
    } _tLivingString;
    
    typedef _tLivingString* tLivingString;
    
    void    tLivingString_init                  (tLivingString* const, Lfloat freq, Lfloat pickPos, Lfloat prepIndex,
                                                 Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                 Lfloat levStrength, int levMode, LEAF* const leaf);
    void    tLivingString_initToPool            (tLivingString* const, Lfloat freq, Lfloat pickPos, Lfloat prepIndex,
                                                 Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                 Lfloat levStrength, int levMode, tMempool* const);
    void    tLivingString_free                  (tLivingString* const);
    
    Lfloat   tLivingString_tick                  (tLivingString* const, Lfloat input);
    Lfloat   tLivingString_sample                (tLivingString* const);
    void    tLivingString_setFreq               (tLivingString* const, Lfloat freq);
    void    tLivingString_setWaveLength         (tLivingString* const, Lfloat waveLength); // in samples
    void    tLivingString_setPickPos            (tLivingString* const, Lfloat pickPos);
    void    tLivingString_setPrepIndex          (tLivingString* const, Lfloat prepIndex);
    void    tLivingString_setDampFreq           (tLivingString* const, Lfloat dampFreq);
    void    tLivingString_setDecay              (tLivingString* const, Lfloat decay); // should be near 1.0
    void    tLivingString_setTargetLev          (tLivingString* const, Lfloat targetLev);
    void    tLivingString_setLevSmoothFactor    (tLivingString* const, Lfloat levSmoothFactor);
    void    tLivingString_setLevStrength        (tLivingString* const, Lfloat levStrength);
    void    tLivingString_setLevMode            (tLivingString* const, int levMode);
    void    tLivingString_setSampleRate         (tLivingString* const, Lfloat sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    
       /*!
     @defgroup tlivingstring2 tLivingString2
     @ingroup physical
     @brief String model with preparation and pick position separated.
     @{
     
     @fn void    tLivingString2_init                  (tLivingString2* const, Lfloat freq, Lfloat pickPos, Lfloat prepIndex, Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor, Lfloat levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tLivingString2 to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLivingString2_initToPool            (tLivingString2* const, Lfloat freq, Lfloat pickPos, Lfloat prepIndex, Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor, Lfloat levStrength, int levMode, tMempool* const)
     @brief Initialize a tLivingString2 to a specified mempool.
     @param string A pointer to the tLivingString2 to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tLivingString2_free                  (tLivingString2* const)
     @brief Free a tLivingString2 from its mempool.
     @param string A pointer to the tLivingString2 to free.
     
     @fn Lfloat   tLivingString2_tick                  (tLivingString2* const, Lfloat input)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn Lfloat   tLivingString2_sample                (tLivingString2* const)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setFreq               (tLivingString2* const, Lfloat freq)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setWaveLength         (tLivingString2* const, Lfloat waveLength)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setPickPos            (tLivingString2* const, Lfloat pickPos)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setPrepPosition          (tLivingString2* const, Lfloat prepPos)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     
     @fn void    tLivingString2_setPrepIndex          (tLivingString2* const, Lfloat prepIndex)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setBrightness           (tLivingString2* const, Lfloat brightness)
     @brief
     @param string A pointer to the relevant tLivingString2.
     @param Lfloat The brightness parameter from 0 to 1.
     
     @fn void    tLivingString2_setDecay              (tLivingString2* const, Lfloat decay)
     @brief
     @param string A pointer to the relevant tLivingString2.
     @param Lfloat The decay parameter from 0 to 1.
     
     @fn void    tLivingString2_setTargetLev          (tLivingString2* const, Lfloat targetLev)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setLevSmoothFactor    (tLivingString2* const, Lfloat levSmoothFactor)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setLevStrength        (tLivingString2* const, Lfloat levStrength)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @fn void    tLivingString2_setLevMode            (tLivingString2* const, int levMode)
     @brief
     @param string A pointer to the relevant tLivingString2.
     
     @} */
    typedef struct _tLivingString2
    {
        tMempool mempool;
        Lfloat freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
        Lfloat pickPos;    // the pick position, dividing the string in two, in ratio
        Lfloat prepPos;    // the preparation position, dividing the string in two, in ratio
        Lfloat pickupPos;    // the preparation position, dividing the string in two, in ratio
        Lfloat prepIndex;    // the amount of pressure on the preparation position of the string (near 0=soft obj, near 1=hard obj)
        Lfloat decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        Lfloat brightness;
        Lfloat curr;
        tHermiteDelay delLF,delUF,delUB,delLB;    // delay for lower/upper/forward/backward part of the waveguide model
        tTwoZero bridgeFilter, nutFilter, prepFilterU, prepFilterL;
        tHighpass DCblockerL, DCblockerU;
        tFeedbackLeveler fbLevU, fbLevL;
        tExpSmooth wlSmooth, ppSmooth, prpSmooth, puSmooth;
        Lfloat sampleRate;
    } _tLivingString2;
    
    typedef _tLivingString2* tLivingString2;
    
    void    tLivingString2_init                  (tLivingString2* const, Lfloat freq, Lfloat pickPos, Lfloat prepPos, Lfloat pickupPos, Lfloat prepIndex,
                                                  Lfloat brightness, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                  Lfloat levStrength, int levMode, LEAF* const leaf);
    void    tLivingString2_initToPool            (tLivingString2* const, Lfloat freq, Lfloat pickPos, Lfloat prepPos, Lfloat pickupPos, Lfloat prepIndex,
                                                  Lfloat brightness, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                  Lfloat levStrength, int levMode, tMempool* const);
    void    tLivingString2_free                  (tLivingString2* const);
    
    Lfloat   tLivingString2_tick                  (tLivingString2* const, Lfloat input);
    Lfloat   tLivingString2_tickEfficient                 (tLivingString2* const, Lfloat input);

    void   tLivingString2_updateDelays(tLivingString2* const pl); //necessary if using tickEfficient (so that parameter setting can be put in a slower process). included in standard tick.
    Lfloat   tLivingString2_sample                (tLivingString2* const);
    void    tLivingString2_setFreq               (tLivingString2* const, Lfloat freq);
    void    tLivingString2_setWaveLength         (tLivingString2* const, Lfloat waveLength); // in samples
    void    tLivingString2_setPickPos            (tLivingString2* const, Lfloat pickPos);
    void    tLivingString2_setPrepPos            (tLivingString2* const, Lfloat prepPos);
    void    tLivingString2_setPickupPos            (tLivingString2* const, Lfloat pickupPos);
    void    tLivingString2_setPrepIndex          (tLivingString2* const, Lfloat prepIndex);
    void    tLivingString2_setBrightness         (tLivingString2* const, Lfloat brightness);
    void    tLivingString2_setDecay              (tLivingString2* const, Lfloat decay); // from 0 to 1, gets converted to real decay factor
    void    tLivingString2_setTargetLev          (tLivingString2* const, Lfloat targetLev);
    void    tLivingString2_setLevSmoothFactor    (tLivingString2* const, Lfloat levSmoothFactor);
    void    tLivingString2_setLevStrength        (tLivingString2* const, Lfloat levStrength);
    void    tLivingString2_setLevMode            (tLivingString2* const, int levMode);
    void    tLivingString2_setSampleRate         (tLivingString2* const, Lfloat sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tcomplexlivingstring tComplexLivingString
     @ingroup physical
     @brief Complex string model.
     @{
     
     @fn void    tComplexLivingString_init  (tComplexLivingString* const, Lfloat freq, Lfloat pickPos, Lfloat prepPos, Lfloat prepIndex, Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor, Lfloat levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tComplexLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tComplexLivingString to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tComplexLivingString_initToPool    (tComplexLivingString* const, Lfloat freq, Lfloat pickPos, Lfloat prepPos, Lfloat prepIndex, Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor, Lfloat levStrength, int levMode, tMempool* const)
     @brief Initialize a tComplexLivingString to a specified mempool.
     @param string A pointer to the tComplexLivingString to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tComplexLivingString_free                  (tComplexLivingString* const)
     @brief Free a tComplexLivingString from its mempool.
     @param string A pointer to the tComplexLivingString to free.
     
     @fn Lfloat   tComplexLivingString_tick                  (tComplexLivingString* const, Lfloat input)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn Lfloat   tComplexLivingString_sample                (tComplexLivingString* const)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setFreq               (tComplexLivingString* const, Lfloat freq)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setWaveLength         (tComplexLivingString* const, Lfloat waveLength)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setPickPos            (tComplexLivingString* const, Lfloat pickPos)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setPrepPos            (tComplexLivingString* const, Lfloat prepPos)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setPrepIndex          (tComplexLivingString* const, Lfloat prepIndex)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setDampFreq           (tComplexLivingString* const, Lfloat dampFreq)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setDecay              (tComplexLivingString* const, Lfloat decay)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setTargetLev          (tComplexLivingString* const, Lfloat targetLev)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setLevSmoothFactor    (tComplexLivingString* const, Lfloat levSmoothFactor)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setLevStrength        (tComplexLivingString* const, Lfloat levStrength)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @fn void    tComplexLivingString_setLevMode            (tComplexLivingString* const, int levMode)
     @brief
     @param string A pointer to the relevant tComplexLivingString.
     
     @} */
    
    typedef struct _tComplexLivingString
    {
        tMempool mempool;
        Lfloat freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
        Lfloat pickPos;    // the pick position, dividing the string, in ratio
        Lfloat prepPos;    // preparation position, in ratio
        int prepLower;
        Lfloat prepIndex;    // the amount of pressure on the pickpoint of the string (near 0=soft obj, near 1=hard obj)
        Lfloat dampFreq;    // frequency for the bridge LP filter, in Hz
        Lfloat decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        Lfloat curr;
        tLinearDelay delLF,delUF, delMF, delMB, delUB,delLB;    // delay for lower/upper/forward/backward part of the waveguide model
        tOnePole bridgeFilter, nutFilter, prepFilterU, prepFilterL;
        tHighpass DCblockerL, DCblockerU;
        tFeedbackLeveler fbLevU, fbLevL;
        tExpSmooth wlSmooth, pickPosSmooth, prepPosSmooth;
        Lfloat sampleRate;
    } _tComplexLivingString;
    
    typedef _tComplexLivingString* tComplexLivingString;
    
    void    tComplexLivingString_init                  (tComplexLivingString* const, Lfloat freq, Lfloat pickPos,
                                                        Lfloat prepPos, Lfloat prepIndex, Lfloat dampFreq,
                                                        Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                        Lfloat levStrength, int levMode, LEAF* const leaf);
    void    tComplexLivingString_initToPool            (tComplexLivingString* const, Lfloat freq, Lfloat pickPos,
                                                        Lfloat prepPos, Lfloat prepIndex, Lfloat dampFreq,
                                                        Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                                        Lfloat levStrength, int levMode, tMempool* const);
    void    tComplexLivingString_free                  (tComplexLivingString* const);
    
    Lfloat   tComplexLivingString_tick                  (tComplexLivingString* const, Lfloat input);
    Lfloat   tComplexLivingString_sample                (tComplexLivingString* const);
    void    tComplexLivingString_setFreq               (tComplexLivingString* const, Lfloat freq);
    void    tComplexLivingString_setWaveLength         (tComplexLivingString* const, Lfloat waveLength); // in samples
    void    tComplexLivingString_setPickPos            (tComplexLivingString* const, Lfloat pickPos);
    void    tComplexLivingString_setPrepPos            (tComplexLivingString* const, Lfloat prepPos);
    void    tComplexLivingString_setPrepIndex          (tComplexLivingString* const, Lfloat prepIndex);
    void    tComplexLivingString_setDampFreq           (tComplexLivingString* const, Lfloat dampFreq);
    void    tComplexLivingString_setDecay              (tComplexLivingString* const, Lfloat decay); // should be near 1.0
    void    tComplexLivingString_setTargetLev          (tComplexLivingString* const, Lfloat targetLev);
    void    tComplexLivingString_setLevSmoothFactor    (tComplexLivingString* const, Lfloat levSmoothFactor);
    void    tComplexLivingString_setLevStrength        (tComplexLivingString* const, Lfloat levStrength);
    void    tComplexLivingString_setLevMode            (tComplexLivingString* const, int levMode);
    void    tComplexLivingString_setSampleRate         (tComplexLivingString* const, Lfloat sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    
    /*!
     @defgroup treedtable tReedTable
     @ingroup physical
     @brief Reed Table - borrowed from STK
     @{
     
     @fn void    tReedTable_init         (tReedTable* const, Lfloat offset, Lfloat slope, LEAF* const leaf)
     @brief Initialize a tReedTable to the default mempool of a LEAF instance.
     @param reed A pointer to the tReedTable to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tReedTable_initToPool   (tReedTable* const, Lfloat offset, Lfloat slope, tMempool* const)
     @brief Initialize a tReedTable to a specified mempool.
     @param reed A pointer to the tReedTable to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tReedTable_free         (tReedTable* const)
     @brief Free a tReedTable from its mempool.
     @param reed A pointer to the tReedTable to free.
     
     @fn Lfloat   tReedTable_tick         (tReedTable* const, Lfloat input)
     @brief
     @param reed A pointer to the relevant tReedTable.
     
     @fn Lfloat   tReedTable_tanh_tick    (tReedTable* const, Lfloat input)
     @brief
     @param reed A pointer to the relevant tReedTable.
     
     @fn void    tReedTable_setOffset    (tReedTable* const, Lfloat offset)
     @brief
     @param reed A pointer to the relevant tReedTable.
     
     @fn void    tReedTable_setSlope     (tReedTable* const, Lfloat slope)
     @brief
     @param reed A pointer to the relevant tReedTable.
     
     @} */
    
    typedef struct _tReedTable
    {
        
        tMempool mempool;
        Lfloat offset, slope;
    } _tReedTable;
    
    typedef _tReedTable* tReedTable;
    
    void    tReedTable_init         (tReedTable* const, Lfloat offset, Lfloat slope, LEAF* const leaf);
    void    tReedTable_initToPool   (tReedTable* const, Lfloat offset, Lfloat slope, tMempool* const);
    void    tReedTable_free         (tReedTable* const);
    
    Lfloat   tReedTable_tick         (tReedTable* const, Lfloat input);
    Lfloat   tReedTable_tanh_tick    (tReedTable* const, Lfloat input); //tanh softclip version of reed table - replacing the hard clip in original stk code
    void    tReedTable_setOffset    (tReedTable* const, Lfloat offset);
    void    tReedTable_setSlope     (tReedTable* const, Lfloat slope);

//==============================================================================

typedef struct _tStiffString
    {
        tMempool mempool;
        int numModes;
        tCycle *osc; // array of oscillators
        Lfloat *amplitudes;
        Lfloat *outputWeights;
        Lfloat freqHz;        // the frequency of the whole string, determining delay length

        Lfloat stiffness;
        Lfloat pluckPos;    // the pick position, dividing the string in two, in ratio
        Lfloat pickupPos;    // the preparation position, dividing the string in two, in ratio
        Lfloat decay;
        Lfloat decayHighFreq;
        Lfloat sampleRate;
        Lfloat twoPiTimesInvSampleRate;
    } _tStiffString;

    typedef _tStiffString* tStiffString;

    void tStiffString_init        (tStiffString* const, int numModes, LEAF* const leaf);
    void tStiffString_initToPool  (tStiffString* const, int numModes, tMempool* const);
    void tStiffString_free        (tStiffString* const);

    Lfloat tStiffString_tick(tStiffString* const);
    void tStiffString_setStiffness(tStiffString* const, Lfloat newValue);
    void tStiffString_setFreq(tStiffString* const, Lfloat newFreq);
    void tStiffString_setFreqFast(tStiffString* const, Lfloat newFreq);
    void tStiffString_setInitialAmplitudes(tStiffString* const);

#ifdef __cplusplus
}
#endif

#endif // LEAF_PHYSICAL_H_INCLUDED

//==============================================================================
