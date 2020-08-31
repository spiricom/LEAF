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
     @brief
     @param
     
     @fn void    tPluck_initToPool    (tPluck* const, float lowestFrequency, tMempool* const)
     @brief
     @param
     
     @fn void    tPluck_free          (tPluck* const)
     @brief
     @param
     
     @fn float   tPluck_tick          (tPluck* const)
     @brief
     @param
     
     @fn void    tPluck_pluck         (tPluck* const, float amplitude)
     @brief Pluck the string.
     @param
     
     @fn void    tPluck_noteOn        (tPluck* const, float frequency, float amplitude )
     @brief Start a note with the given frequency and amplitude.
     @param
     
     @fn void    tPluck_noteOff       (tPluck* const, float amplitude )
     @brief Stop a note with the given amplitude (speed of decay)
     @param

     @fn void    tPluck_setFrequency  (tPluck* const, float frequency )
     @brief Set instrument parameters for a particular frequency.
     @param
     
     @fn void    tPluck_controlChange (tPluck* const, int number, float value)
     @brief Perform the control change specified by \e number and \e value (0.0 - 128.0).
     @param
    
     @fn float   tPluck_getLastOut    (tPluck* const)
     @brief
     @param
     
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
        
        float sr;
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
    
    //==============================================================================
    
    /*!
     @defgroup tkarplusstrong tKarplusStrong
     @ingroup physical
     @brief Stiff Karplus-Strong model
     @{
     
     @fn void    tKarplusStrong_init               (tKarplusStrong* const, float lowestFrequency, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tKarplusStrong_initToPool         (tKarplusStrong* const, float lowestFrequency, tMempool* const)
     @brief
     @param
     
     @fn void    tKarplusStrong_free               (tKarplusStrong* const)
     @brief
     @param
     
     @fn float   tKarplusStrong_tick               (tKarplusStrong* const)
     @brief
     @param
     
     @fn void    tKarplusStrong_pluck              (tKarplusStrong* const, float amplitude)
     @brief Pluck the string.
     @param
    
     @fn void    tKarplusStrong_noteOn             (tKarplusStrong* const, float frequency, float amplitude)
     @brief Start a note with the given frequency and amplitude.
     @param
     
     @fn void    tKarplusStrong_noteOff            (tKarplusStrong* const, float amplitude)
     @brief Stop a note with the given amplitude (speed of decay).
     @param
     
     @fn void    tKarplusStrong_setFrequency       (tKarplusStrong* const, float frequency)
     @brief Set instrument parameters for a particular frequency.
     @param
     
     @fn void    tKarplusStrong_controlChange      (tKarplusStrong* const, SKControlType type, float value)
     @brief Perform the control change specified by \e number and \e value (0.0 - 128.0). Use SKPickPosition, SKStringDamping, or SKDetune for type.
     @param
     
     @fn void    tKarplusStrong_setStretch         (tKarplusStrong* const, float stretch)
     @brief Set the stretch "factor" of the string (0.0 - 1.0).
     @param
     
     @fn void    tKarplusStrong_setPickupPosition  (tKarplusStrong* const, float position)
     @brief Set the pluck or "excitation" position along the string (0.0 - 1.0).
     @param
     
     @fn void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong* const, float aGain )
     @brief Set the base loop gain.
     @param
     
     @fn float   tKarplusStrong_getLastOut         (tKarplusStrong* const)
     @brief
     @param
     
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
     @brief
     @{
     
     @fn void    tSimpleLivingString_init   (tSimpleLivingString* const, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tSimpleLivingString_initToPool     (tSimpleLivingString* const, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, tMempool* const)
     @brief
     @param
     
     @fn void    tSimpleLivingString_free                (tSimpleLivingString* const)
     @brief
     @param
     
     @fn float   tSimpleLivingString_tick                (tSimpleLivingString* const, float input)
     @brief
     @param
     
     @fn float   tSimpleLivingString_sample              (tSimpleLivingString* const)
     @brief
     @param
     
     @fn void    tSimpleLivingString_setFreq             (tSimpleLivingString* const, float freq)
     @brief
     @param
     
     @fn void    tSimpleLivingString_setWaveLength       (tSimpleLivingString* const, float waveLength)
     @brief
     @param
     
     @fn void    tSimpleLivingString_setDampFreq         (tSimpleLivingString* const, float dampFreq)
     @brief
     @param
     
     @fn void    tSimpleLivingString_setDecay            (tSimpleLivingString* const, float decay)
     @brief
     @param
     
     @fn void    tSimpleLivingString_setTargetLev        (tSimpleLivingString* const, float targetLev)
     @brief
     @param
     
     @fn void    tSimpleLivingString_setLevSmoothFactor  (tSimpleLivingString* const, float levSmoothFactor)
     @brief
     @param
     
     @fn void    tSimpleLivingString_setLevStrength      (tSimpleLivingString* const, float levStrength)
     @brief
     @param
     
     @fn void    tSimpleLivingString_setLevMode          (tSimpleLivingString* const, int levMode)
     @brief
     @param
     
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
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tlivingstring tLivingString
     @ingroup physical
     @brief
     @{
     
     @fn void    tLivingString_init                  (tLivingString* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tLivingString_initToPool            (tLivingString* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, tMempool* const)
     @brief
     @param
     
     @fn void    tLivingString_free                  (tLivingString* const)
     @brief
     @param
     
     @fn float   tLivingString_tick                  (tLivingString* const, float input)
     @brief
     @param
     
     @fn float   tLivingString_sample                (tLivingString* const)
     @brief
     @param
     
     @fn void    tLivingString_setFreq               (tLivingString* const, float freq)
     @brief
     @param
     
     @fn void    tLivingString_setWaveLength         (tLivingString* const, float waveLength)
     @brief
     @param
     
     @fn void    tLivingString_setPickPos            (tLivingString* const, float pickPos)
     @brief
     @param
     
     @fn void    tLivingString_setPrepIndex          (tLivingString* const, float prepIndex)
     @brief
     @param
     
     @fn void    tLivingString_setDampFreq           (tLivingString* const, float dampFreq)
     @brief
     @param
     
     @fn void    tLivingString_setDecay              (tLivingString* const, float decay)
     @brief
     @param
     
     @fn void    tLivingString_setTargetLev          (tLivingString* const, float targetLev)
     @brief
     @param
     
     @fn void    tLivingString_setLevSmoothFactor    (tLivingString* const, float levSmoothFactor)
     @brief
     @param
     
     @fn void    tLivingString_setLevStrength        (tLivingString* const, float levStrength)
     @brief
     @param
     
     @fn void    tLivingString_setLevMode            (tLivingString* const, int levMode)
     @brief
     @param
     
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
    


        // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

    /*!
     @defgroup tcomplexlivingstring tComplexLivingString
     @ingroup physical
     @brief
     @{
     
     @fn void    tComplexLivingString_init  (tComplexLivingString* const, float freq, float pickPos, float prepPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tComplexLivingString_initToPool    (tComplexLivingString* const, float freq, float pickPos, float prepPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, tMempool* const)
     @brief
     @param
     
     @fn void    tComplexLivingString_free                  (tComplexLivingString* const)
     @brief
     @param
     
     @fn float   tComplexLivingString_tick                  (tComplexLivingString* const, float input)
     @brief
     @param
     
     @fn float   tComplexLivingString_sample                (tComplexLivingString* const)
     @brief
     @param
     
     @fn void    tComplexLivingString_setFreq               (tComplexLivingString* const, float freq)
     @brief
     @param
     
     @fn void    tComplexLivingString_setWaveLength         (tComplexLivingString* const, float waveLength)
     @brief
     @param
     
     @fn void    tComplexLivingString_setPickPos            (tComplexLivingString* const, float pickPos)
     @brief
     @param
     
     @fn void    tComplexLivingString_setPrepPos            (tComplexLivingString* const, float prepPos)
     @brief
     @param
     
     @fn void    tComplexLivingString_setPrepIndex          (tComplexLivingString* const, float prepIndex)
     @brief
     @param
     
     @fn void    tComplexLivingString_setDampFreq           (tComplexLivingString* const, float dampFreq)
     @brief
     @param
     
     @fn void    tComplexLivingString_setDecay              (tComplexLivingString* const, float decay)
     @brief
     @param
     
     @fn void    tComplexLivingString_setTargetLev          (tComplexLivingString* const, float targetLev)
     @brief
     @param
     
     @fn void    tComplexLivingString_setLevSmoothFactor    (tComplexLivingString* const, float levSmoothFactor)
     @brief
     @param
     
     @fn void    tComplexLivingString_setLevStrength        (tComplexLivingString* const, float levStrength)
     @brief
     @param
     
     @fn void    tComplexLivingString_setLevMode            (tComplexLivingString* const, int levMode)
     @brief
     @param
     
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
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

    
    /*!
     @defgroup treedtable tReedTable
     @ingroup physical
     @brief Reed Table - borrowed from STK
     @{
     
     @fn void    tReedTable_init         (tReedTable* const, float offset, float slope, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tReedTable_initToPool   (tReedTable* const, float offset, float slope, tMempool* const)
     @brief
     @param
     
     @fn void    tReedTable_free         (tReedTable* const)
     @brief
     @param
     
     @fn float   tReedTable_tick         (tReedTable* const, float input)
     @brief
     @param
     
     @fn float   tReedTable_tanh_tick    (tReedTable* const, float input)
     @brief
     @param
     
     @fn void    tReedTable_setOffset    (tReedTable* const, float offset)
     @brief
     @param
     
     @fn void    tReedTable_setSlope     (tReedTable* const, float slope)
     @brief
     @param
     
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

