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


typedef struct tPickupNonLinearity
{

    tMempool* mempool;
    float prev;
} tPickupNonLinearity;

void    tPickupNonLinearity_create               (tMempool** const mempool, tPickupNonLinearity** const);
void    tPickupNonLinearity_init                 (LEAF* const leaf, tPickupNonLinearity* const p);
void   tPickupNonLinearity_free          (tPickupNonLinearity** const p);

float tPickupNonLinearity_tick          (tPickupNonLinearity* const p, float in);

    //==============================================================================
    
    /*!
     @defgroup tpluck tPluck
     @ingroup physical
     @brief Karplus-Strong model
     @{
     
     @fn void    tPluck_init(tPluck* const, float lowestFrequency, LEAF* const leaf)
     @brief Initialize a tPluck to the default mempool of a LEAF instance.
     @param string A pointer to the tPluck to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPluck_init                 (tMempool** const)
     @brief Initialize a tPluck to a specified mempool.
     @param string A pointer to the tPluck to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPluck_free(tPluck** const)
     @brief Free a tPluck from its mempool.
     @param string A pointer to the tPluck to free.
     
     @fn float   tPluck_tick          (tPluck* const)
     @brief
     @param string A pointer to the relevant tPluck.
     
     @fn void    tPluck_pluck         (tPluck* const, tPluck** const, float lowestFrequency, float amplitude)
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
    
    typedef struct tPluck
    {

        tMempool* mempool;
        
        tAllpassDelay*     delayLine; // Allpass or Linear??  big difference...
        tOneZero*    loopFilter;
        tOnePole*    pickFilter;
        tNoise*      noise;
        
        float lastOut;
        float loopGain;
        float lastFreq;
        
        float sampleRate;
    } tPluck;

    void    tPluck_create               (tMempool** const mempool, tPluck** const); //float delayBuff[DELAY_LENGTH]);
    void    tPluck_init                 (LEAF* const leaf, tPluck* const, float lowestFrequency);
    void    tPluck_free          (tPluck** const);
    
    float  tPluck_tick          (tPluck* const);

    void    tPluck_pluck         (tPluck* const, float amplitude);
    void    tPluck_noteOn        (tPluck* const, float frequency, float amplitude );
    void    tPluck_noteOff       (tPluck* const, float amplitude );
    void    tPluck_setFrequency  (tPluck* const, float frequency );
    void    tPluck_controlChange (tPluck* const, int number, float value);
    float  tPluck_getLastOut    (tPluck* const);
    void    tPluck_setSampleRate (tPluck* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tkarplusstrong tKarplusStrong
     @ingroup physical
     @brief Stiff Karplus-Strong model
     @{
     
     @fn void    tKarplusStrong_init(tKarplusStrong* const, float lowestFrequency, LEAF* const leaf)
     @brief Initialize a tKarplusStrong to the default mempool of a LEAF instance.
     @param string A pointer to the tKarplusStrong to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tKarplusStrong_init                 (tMempool** const)
     @brief Initialize a tKarplusStrong to a specified mempool.
     @param string A pointer to the tKarplusStrong to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tKarplusStrong_free(tKarplusStrong** const)
     @brief Free a tKarplusStrong from its mempool.
     @param string A pointer to the tKarplusStrong to free.
     
     @fn float   tKarplusStrong_tick               (tKarplusStrong* const)
     @brief
     @param string A pointer to the relevant tKarplusStrong.
     
     @fn void    tKarplusStrong_pluck              (tKarplusStrong* const, tKarplusStrong** const, float lowestFrequency, float amplitude)
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
    
    typedef struct tKarplusStrong
    {

        tMempool* mempool;
        
        tAllpassDelay*  delayLine;
        tLinearDelay* combDelay;
        tOneZero* filter;
        tNoise*   noise;
        tBiQuad*  biquad[4];
        
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
    } tKarplusStrong;

    void    tKarplusStrong_create               (tMempool** const mempool, tKarplusStrong** const); // float delayBuff[2][DELAY_LENGTH]);
    void    tKarplusStrong_init                 (LEAF* const leaf, tKarplusStrong* const, float lowestFrequency);
    void    tKarplusStrong_free               (tKarplusStrong** const);
    
    float  tKarplusStrong_tick               (tKarplusStrong* const);

    void    tKarplusStrong_pluck              (tKarplusStrong* const, float amplitude);
    void    tKarplusStrong_noteOn             (tKarplusStrong* const, float frequency, float amplitude );
    void    tKarplusStrong_noteOff            (tKarplusStrong* const, float amplitude );
    void    tKarplusStrong_setFrequency       (tKarplusStrong* const, float frequency );
    void    tKarplusStrong_controlChange      (tKarplusStrong* const, SKControlType type, float value);
    void    tKarplusStrong_setStretch         (tKarplusStrong* const, float stretch );
    void    tKarplusStrong_setPickupPosition  (tKarplusStrong* const, float position );
    void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong* const, float aGain );
    float  tKarplusStrong_getLastOut         (tKarplusStrong* const);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tsimplelivingstring tSimpleLivingString
     @ingroup physical
     @brief Simplified string model.
     @{
     
     @fn void    tSimpleLivingString_init(tSimpleLivingString* const, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tSimpleLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tSimpleLivingString to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSimpleLivingString_init                 (tMempool** const)
     @brief Initialize a tSimpleLivingString to a specified mempool.
     @param string A pointer to the tSimpleLivingString to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSimpleLivingString_free(tSimpleLivingString** const)
     @brief Free a tSimpleLivingString from its mempool.
     @param string A pointer to the tSimpleLivingString to free.
     
     @fn float   tSimpleLivingString_tick                (tSimpleLivingString* const, tSimpleLivingString** const, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, float input)
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
    
    typedef struct tSimpleLivingString
    {

        tMempool* mempool;
        float freq, waveLengthInSamples;        // the frequency of the string, determining delay length
        float dampFreq;    // frequency for the bridge LP filter, in Hz
        float decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        float curr;
        tLinearDelay* delayLine;
        tOnePole* bridgeFilter;
        tHighpass* DCblocker;
        tFeedbackLeveler* fbLev;
        tExpSmooth* wlSmooth;
        float sampleRate;
    } tSimpleLivingString;

    void    tSimpleLivingString_init                (tSimpleLivingString* const, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf);
    void    tSimpleLivingString_init                 (LEAF* const leaf, tSimpleLivingString* const, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode);
    void    tSimpleLivingString_free                (tSimpleLivingString** const);
    
    float  tSimpleLivingString_tick                (tSimpleLivingString* const, float input);

    float  tSimpleLivingString_sample              (tSimpleLivingString* const);
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
    
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~


    typedef struct tSimpleLivingString2
    {

        tMempool* mempool;
        float freq, waveLengthInSamples;        // the frequency of the string, determining delay length
        float brightness;    // frequency for the bridge LP filter, in Hz
        float decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        float curr;
        tHermiteDelay*  delayLine;
        tTwoZero*  bridgeFilter;
        tHighpass*  DCblocker;
        tFeedbackLeveler*  fbLev;
        tExpSmooth*  wlSmooth;
        float sampleRate;
    } tSimpleLivingString2;

    void    tSimpleLivingString2_init                (tSimpleLivingString2* const, float freq, float brightness,
                                                     float decay, float targetLev, float levSmoothFactor,
                                                     float levStrength, int levMode, LEAF* const leaf);
    void    tSimpleLivingString2_init                 (LEAF* const leaf, tSimpleLivingString2* const, float freq, float brightness, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode);
    void    tSimpleLivingString2_free                (tSimpleLivingString2** const);

    float  tSimpleLivingString2_tick                (tSimpleLivingString2* const, float input);

    float  tSimpleLivingString2_sample              (tSimpleLivingString2* const);
    void    tSimpleLivingString2_setFreq             (tSimpleLivingString2* const, float freq);
    void    tSimpleLivingString2_setWaveLength       (tSimpleLivingString2* const, float waveLength); // in samples
    void    tSimpleLivingString2_setBrightness       (tSimpleLivingString2* const, float brightness);
    void    tSimpleLivingString2_setDecay            (tSimpleLivingString2* const, float decay); // should be near 1.0
    void    tSimpleLivingString2_setTargetLev        (tSimpleLivingString2* const, float targetLev);
    void    tSimpleLivingString2_setLevSmoothFactor  (tSimpleLivingString2* const, float levSmoothFactor);
    void    tSimpleLivingString2_setLevStrength      (tSimpleLivingString2* const, float levStrength);
    void    tSimpleLivingString2_setLevMode          (tSimpleLivingString2* const, int levMode);
    void    tSimpleLivingString2_setSampleRate       (tSimpleLivingString2* const, float sr);

    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~


typedef struct tSimpleLivingString3
{

    tMempool* mempool;
    float freq, waveLengthInSamples;        // the frequency of the string, determining delay length
    float dampFreq;    // frequency for the bridge LP filter, in Hz
    float decay, userDecay; // amplitude damping factor for the string (only active in mode 0)
    float prevDelayLength;
    float changeGainCompensator;
    int levMode;
    float curr;
    float Uout;
    float Lout;
    int maxLength;
    tLinearDelay*  delayLineU;
    tLinearDelay*  delayLineL;
    tOnePole*  bridgeFilter;
    tBiQuad*  bridgeFilter2;
    float temp1;
    float temp2;
    float pickupPoint;
    tHighpass*  DCblocker;
    tFeedbackLeveler*  fbLev;
    tExpSmooth*  wlSmooth;
    int oversampling;
    float sampleRate;
    float rippleGain;
    float rippleDelay;
    float invOnePlusr;
} tSimpleLivingString3;

void    tSimpleLivingString3_init               (tSimpleLivingString3* const, int oversampling, float freq, float dam,
                                                 float decay, float targetLev, float levSmoothF, float levStrength,
                                                 int levMode, LEAF* const leaf);
void    tSimpleLivingString3_init                 (LEAF* const leaf, tSimpleLivingString3* const pl, int oversampling, float freq, float dam, float decay, float targetLev, float levSmoothF, float levStrength, int levMode);
void    tSimpleLivingString3_free               (tSimpleLivingString3** const);

float  tSimpleLivingString3_tick               (tSimpleLivingString3* const, float input);

void    tSimpleLivingString3_pluck              (tSimpleLivingString3* const pl, float input, float position);
void    tSimpleLivingString3_setPickupPoint     (tSimpleLivingString3* const pl, float pickupPoint);
float  tSimpleLivingString3_sample             (tSimpleLivingString3* const);
void    tSimpleLivingString3_setFreq            (tSimpleLivingString3* const, float freq);
void    tSimpleLivingString3_setWaveLength      (tSimpleLivingString3* const, float waveLength); // in samples
void    tSimpleLivingString3_setDampFreq        (tSimpleLivingString3* const, float dampFreq);
void    tSimpleLivingString3_setDecay           (tSimpleLivingString3* const, float decay); // should be near 1.0
void    tSimpleLivingString3_setTargetLev       (tSimpleLivingString3* const, float targetLev);
void    tSimpleLivingString3_setLevSmoothFactor (tSimpleLivingString3* const, float levSmoothFactor);
void    tSimpleLivingString3_setLevStrength     (tSimpleLivingString3* const, float levStrength);
void    tSimpleLivingString3_setLevMode         (tSimpleLivingString3* const, int levMode);
void    tSimpleLivingString3_setSampleRate      (tSimpleLivingString3* const, float sr);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
typedef struct tSimpleLivingString4
{

    tMempool* mempool;
    float freq, waveLengthInSamples;        // the frequency of the string, determining delay length
    float dampFreq;    // frequency for the bridge LP filter, in Hz
    float decay, userDecay; // amplitude damping factor for the string (only active in mode 0)
    float prevDelayLength;
    float changeGainCompensator;
    int levMode;
    float curr;
    float Uout;
    float Lout;
    int maxLength;
    tLinearDelay*  delayLineU;
    tLinearDelay*  delayLineL;
    tOnePole*  bridgeFilter;
    tBiQuad*  bridgeFilter2;
    float temp1;
    float temp2;
    float pickupPoint;
    float pluckPosition;
    tHighpass*  DCblocker;
    tFeedbackLeveler*  fbLev;
    tExpSmooth*  wlSmooth;
    int oversampling;
    float sampleRate;
    float rippleGain;
    float rippleDelay;
} tSimpleLivingString4;

void    tSimpleLivingString4_init               (tSimpleLivingString4* const, int oversampling, float freq, float dampFreq,
                                                 float decay, float targetLev, float levSmoothFactor,float levStrength,
                                                 int levMode, LEAF* const leaf);
void    tSimpleLivingString4_init                 (LEAF* const leaf, tSimpleLivingString4* const pl, int oversampling, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode);
void    tSimpleLivingString4_free               (tSimpleLivingString4** const);

float  tSimpleLivingString4_tick               (tSimpleLivingString4* const, float input);

void    tSimpleLivingString4_pluck              (tSimpleLivingString4* const pl, float input, float position);
void    tSimpleLivingString4_pluckNoPosition    (tSimpleLivingString4* const pl, float input);
void    tSimpleLivingString4_setPluckPosition   (tSimpleLivingString4* const pl, float position);
void    tSimpleLivingString4_setPickupPoint     (tSimpleLivingString4* const pl, float pickupPoint);
float  tSimpleLivingString4_sample             (tSimpleLivingString4* const);
void    tSimpleLivingString4_setFreq            (tSimpleLivingString4* const, float freq);
void    tSimpleLivingString4_setWaveLength      (tSimpleLivingString4* const, float waveLength); // in samples
void    tSimpleLivingString4_setDampFreq        (tSimpleLivingString4* const, float dampFreq);
void    tSimpleLivingString4_setDecay           (tSimpleLivingString4* const, float decay); // should be near 1.0
void    tSimpleLivingString4_setTargetLev       (tSimpleLivingString4* const, float targetLev);
void    tSimpleLivingString4_setLevSmoothFactor (tSimpleLivingString4* const, float levSmoothFactor);
void    tSimpleLivingString4_setLevStrength     (tSimpleLivingString4* const, float levStrength);
void    tSimpleLivingString4_setLevMode         (tSimpleLivingString4* const, int levMode);
void    tSimpleLivingString4_setSampleRate      (tSimpleLivingString4* const, float sr);


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
typedef struct tSimpleLivingString5
{

    tMempool* mempool;
    float freq, waveLengthInSamples;        // the frequency of the string, determining delay length
    float dampFreq;    // frequency for the bridge LP filter, in Hz
    float decay, userDecay; // amplitude damping factor for the string (only active in mode 0)
    float prevDelayLength;
    float changeGainCompensator;
    int levMode;
    float curr;
    float Uout;
    float Lout;
    int maxLength;
    float prepIndex;
    float prepPos;
    tLagrangeDelay* delLF,*delUF,*delUB,*delLB;    // delay for lower/upper/forward/backward part of the waveguide model
    tOnePole* bridgeFilter, *nutFilter, *prepFilterU, *prepFilterL;
    float temp1;
    float temp2;
    float pickupPoint;
    float pluckPosition;
    tHighpass*  DCblocker;
    tHighpass*  DCblocker2;
    tFeedbackLeveler*  fbLev;
    tFeedbackLeveler*  fbLev2;

    tExpSmooth *wlSmooth, *prepPosSmooth, *prepIndexSmooth, *pluckPosSmooth, *pickupPointSmooth;
    int oversampling;
    float sampleRate;
    float rippleGain;
    float rippleDelay;
    float ff;
    float fb;
} tSimpleLivingString5;

void    tSimpleLivingString5_init               (tSimpleLivingString5* const, int oversampling, float freq, float dampFreq,
                                                 float decay, float prepPos, float prepIndex, float pluckPos, float targetLev,
                                                 float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf);
void    tSimpleLivingString5_init                 (LEAF* const leaf, tSimpleLivingString5* const pl, int oversampling, float freq, float dampFreq, float decay, float prepPos, float prepIndex, float pluckPos, float targetLev, float levSmoothFactor, float levStrength, int levMode);
void    tSimpleLivingString5_free               (tSimpleLivingString5** const);

float  tSimpleLivingString5_tick               (tSimpleLivingString5* const, float input);

void    tSimpleLivingString5_pluck              (tSimpleLivingString5* const pl, float input, float position);
void    tSimpleLivingString5_pluckNoPosition    (tSimpleLivingString5* const pl, float input);
void    tSimpleLivingString5_setPluckPosition   (tSimpleLivingString5* const pl, float position);
void    tSimpleLivingString5_setPrepPosition    (tSimpleLivingString5* const pl, float prepPosition);
void    tSimpleLivingString5_setPrepIndex       (tSimpleLivingString5* const pl, float prepIndex);
void    tSimpleLivingString5_setPickupPoint     (tSimpleLivingString5* const pl, float pickupPoint);
float  tSimpleLivingString5_sample             (tSimpleLivingString5* const);
void    tSimpleLivingString5_setFreq            (tSimpleLivingString5* const, float freq);
void    tSimpleLivingString5_setWaveLength      (tSimpleLivingString5* const, float waveLength); // in samples
void    tSimpleLivingString5_setDampFreq        (tSimpleLivingString5* const, float dampFreq);
void    tSimpleLivingString5_setDecay           (tSimpleLivingString5* const, float decay); // should be near 1.0
void    tSimpleLivingString5_setTargetLev       (tSimpleLivingString5* const, float targetLev);
void    tSimpleLivingString5_setLevSmoothFactor (tSimpleLivingString5* const, float levSmoothFactor);
void    tSimpleLivingString5_setLevStrength     (tSimpleLivingString5* const, float levStrength);
void    tSimpleLivingString5_setLevMode         (tSimpleLivingString5* const, int levMode);
void    tSimpleLivingString5_setSampleRate      (tSimpleLivingString5* const, float sr);
void    tSimpleLivingString5_setFFAmount        (tSimpleLivingString5* const pl, float ff);


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~




    /*!
     @defgroup tlivingstring tLivingString
     @ingroup physical
     @brief String model.
     @{
     
     @fn void    tLivingString_init(tLivingString* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tLivingString to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLivingString_init                 (tMempool** const)
     @brief Initialize a tLivingString to a specified mempool.
     @param string A pointer to the tLivingString to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tLivingString_free(tLivingString** const)
     @brief Free a tLivingString from its mempool.
     @param string A pointer to the tLivingString to free.
     
     @fn float   tLivingString_tick                  (tLivingString* const, tLivingString** const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, float input)
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
    
    typedef struct tLivingString
    {
        tMempool* mempool;
        float freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
        float pickPos;    // the pick position, dividing the string in two, in ratio
        float prepIndex;    // the amount of pressure on the pickpoint of the string (near 0=soft obj, near 1=hard obj)
        float dampFreq;    // frequency for the bridge LP filter, in Hz
        float decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        float curr;
        tLinearDelay* delLF,*delUF,*delUB,*delLB;    // delay for lower/upper/forward/backward part of the waveguide model
        tOnePole* bridgeFilter, *nutFilter,* prepFilterU, *prepFilterL;
        tHighpass* DCblockerL, *DCblockerU;
        tFeedbackLeveler* fbLevU, *fbLevL;
        tExpSmooth* wlSmooth, *ppSmooth;
        float sampleRate;
    } tLivingString;

    void    tLivingString_init                  (tLivingString* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf);
    void    tLivingString_init                 (LEAF* const leaf, tLivingString* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode);
    void    tLivingString_free                  (tLivingString** const);
    
    float  tLivingString_tick                  (tLivingString* const, float input);

    float  tLivingString_sample                (tLivingString* const);
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
     
     @fn void    tLivingString2_init(tLivingString2* const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tLivingString2 to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLivingString2_init                 (tMempool** const)
     @brief Initialize a tLivingString2 to a specified mempool.
     @param string A pointer to the tLivingString2 to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tLivingString2_free(tLivingString2** const)
     @brief Free a tLivingString2 from its mempool.
     @param string A pointer to the tLivingString2 to free.
     
     @fn float   tLivingString2_tick                  (tLivingString2* const, tLivingString2** const, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, float input)
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
    typedef struct tLivingString2
    {
        tMempool* mempool;
        float freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
        float pickPos;    // the pick position, dividing the string in two, in ratio
        float prepPos;    // the preparation position, dividing the string in two, in ratio
        float pickupPos;    // the preparation position, dividing the string in two, in ratio
        float prepIndex;    // the amount of pressure on the preparation position of the string (near 0=soft obj, near 1=hard obj)
        float decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        float brightness;
        float curr;
        tHermiteDelay* delLF,*delUF,*delUB,*delLB;    // delay for lower/upper/forward/backward part of the waveguide model
        tTwoZero* bridgeFilter, *nutFilter, *prepFilterU, *prepFilterL;
        tHighpass* DCblockerL, *DCblockerU;
        tFeedbackLeveler* fbLevU, *fbLevL;
        tExpSmooth* wlSmooth, *ppSmooth, *prpSmooth, *puSmooth;
        float sampleRate;
    } tLivingString2;

    void    tLivingString2_init               (tLivingString2* const, float freq, float pickPos, float prepPos, float pickupPos, float prepIndex, float brightness, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf);
    void    tLivingString2_init                 (LEAF* const leaf, tLivingString2* const, float freq, float pickPos, float prepPos, float pickupPos, float prepIndex, float brightness, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode);
    void    tLivingString2_free               (tLivingString2** const);
    
    float  tLivingString2_tick               (tLivingString2* const, float input);
    float  tLivingString2_tickEfficient      (tLivingString2* const, float input);

    void    tLivingString2_updateDelays       (tLivingString2* const pl); //necessary if using tickEfficient (so that parameter setting can be put in a slower process). included in standard tick.
    float  tLivingString2_sample             (tLivingString2* const);
    void    tLivingString2_setFreq            (tLivingString2* const, float freq);
    void    tLivingString2_setWaveLength      (tLivingString2* const, float waveLength); // in samples
    void    tLivingString2_setPickPos         (tLivingString2* const, float pickPos);
    void    tLivingString2_setPrepPos         (tLivingString2* const, float prepPos);
    void    tLivingString2_setPickupPos       (tLivingString2* const, float pickupPos);
    void    tLivingString2_setPrepIndex       (tLivingString2* const, float prepIndex);
    void    tLivingString2_setBrightness      (tLivingString2* const, float brightness);
    void    tLivingString2_setDecay           (tLivingString2* const, float decay); // from 0 to 1, gets converted to real decay factor
    void    tLivingString2_setTargetLev       (tLivingString2* const, float targetLev);
    void    tLivingString2_setLevSmoothFactor (tLivingString2* const, float levSmoothFactor);
    void    tLivingString2_setLevStrength     (tLivingString2* const, float levStrength);
    void    tLivingString2_setLevMode         (tLivingString2* const, int levMode);
    void    tLivingString2_setSampleRate      (tLivingString2* const, float sr);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tcomplexlivingstring tComplexLivingString
     @ingroup physical
     @brief Complex string model.
     @{
     
     @fn void    tComplexLivingString_init(tComplexLivingString* const, float freq, float pickPos, float prepPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf)
     @brief Initialize a tComplexLivingString to the default mempool of a LEAF instance.
     @param string A pointer to the tComplexLivingString to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tComplexLivingString_init                 (tMempool** const)
     @brief Initialize a tComplexLivingString to a specified mempool.
     @param string A pointer to the tComplexLivingString to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tComplexLivingString_free(tComplexLivingString** const)
     @brief Free a tComplexLivingString from its mempool.
     @param string A pointer to the tComplexLivingString to free.
     
     @fn float   tComplexLivingString_tick                  (tComplexLivingString* const, tComplexLivingString** const, float freq, float pickPos, float prepPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, float input)
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
    
    typedef struct tComplexLivingString
    {
        tMempool* mempool;
        float freq, waveLengthInSamples;        // the frequency of the whole string, determining delay length
        float pickPos;    // the pick position, dividing the string, in ratio
        float prepPos;    // preparation position, in ratio
        int prepLower;
        float prepIndex;    // the amount of pressure on the pickpoint of the string (near 0=soft obj, near 1=hard obj)
        float dampFreq;    // frequency for the bridge LP filter, in Hz
        float decay; // amplitude damping factor for the string (only active in mode 0)
        int levMode;
        float curr;
        tLinearDelay* delLF,*delUF, *delMF, *delMB, *delUB,*delLB;    // delay for lower/upper/forward/backward part of the waveguide model
        tOnePole* bridgeFilter, *nutFilter, *prepFilterU, *prepFilterL;
        tHighpass* DCblockerL, *DCblockerU;
        tFeedbackLeveler* fbLevU, *fbLevL;
        tExpSmooth* wlSmooth,* pickPosSmooth, *prepPosSmooth;
        float sampleRate;
    } tComplexLivingString;

    void    tComplexLivingString_init                  (tComplexLivingString* const, float freq, float pickPos, float prepPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode, LEAF* const leaf);
    void    tComplexLivingString_init                 (LEAF* const leaf, tComplexLivingString* const, float freq, float pickPos, float prepPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode);
    void    tComplexLivingString_free                  (tComplexLivingString** const);
    
    float  tComplexLivingString_tick                  (tComplexLivingString* const, float input);

    float  tComplexLivingString_sample                (tComplexLivingString* const);
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
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
//Bow Table
typedef struct tBowTable {
    tMempool* mempool;
    float offSet;
    float slope;
    float lastOutput;
} tBowTable;

void    tBowTable_create               (tMempool** const mempool, tBowTable** const);
void    tBowTable_init                 (LEAF* const leaf, tBowTable* const bt);
void    tBowTable_free                 (tBowTable** const bt);

float  tBowTable_lookup               (tBowTable* const bt, float sample);

typedef struct tBowed
{
    tMempool* mempool;
    int oversampling;
    // user controlled vars
    float x_bp;      // bow pressure
    float x_bpos;    // bow position
    float x_bv;      // bow velocity
    float x_fr;      // frequency

    float fr_save;

    // delay lines
    tLinearDelay*  neckDelay;
    tLinearDelay*  bridgeDelay;

    // one pole filter
    tCookOnePole*  reflFilt;

    tBowTable*  bowTabl;

    // stuff
    float maxVelocity, baseDelay, betaRatio;
    float sampleRate;
    float invSampleRate;
    tSVF*  lowpass;
    float output;
} tBowed;

void    tBowed_create               (tMempool** const mempool, tBowed** const);
void    tBowed_init                 (LEAF* const leaf, tBowed* const, int oversampling);
void    tBowed_free                  (tBowed** const);

float  tBowed_tick                  (tBowed* const);

void    tBowed_setFreq               (tBowed* const, float freq);
void    tBowed_setWaveLength         (tBowed* const, float waveLength); // in samples
void    tBowed_setSampleRate         (tBowed* const, float sr);


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
//from Plucked-string synthesis algorithms with tension modulation nonlinearity
//by Vesa Valimaki
typedef struct tTString
{
    tMempool* mempool;
    int oversampling;
    float invOversampling;
    float invOversamplingTimesTwo;
    float twoPiTimesInvSampleRate;
    // delay lines
    tLagrangeDelay*  delay;
    tLagrangeDelay*  delayP;
    tHighpass*  dcBlock;
    tHighpass*  dcBlockP;
    // one pole filter
    tCookOnePole*  reflFilt;
    tCookOnePole*  reflFiltP;
    float baseDelay;
    float sampleRate;
    float invSampleRate;
    float invSampleRateTimesTwoPi;
    float output;
    float outputP;
    float tensionGain;
    tSlide*  slide;
    tExpSmooth*  tensionSmoother;
    tExpSmooth*  pitchSmoother;
    tThiranAllpassSOCascade*  allpass;
    tThiranAllpassSOCascade*  allpassP;
    float allpassDelay;
    float allpassDelayP;
    float freq;
    tSVF*  lowpassP;
    tSVF*  highpassP;
    float filterFreq;
    float decayCoeff;
    float muteCoeff;
    float r;
    float rippleRate;
    float harmonic;
    float decayInSeconds;
    float invOnePlusr;
    float actualLowestFreq;
    float pickupOut;
    float pickupOutP;
    float pickupPos;

    float tensionAmount;
    tCycle*  tensionModOsc;
    float phantomGain;
    tCycle*  pickupModOsc;
    float pickupModOscFreq;
    float pickupModOscAmp;
    tSVF*  pickupFilter;
    tSVF*  pickupFilter2;
    float slideAmount;
    float absSlideAmount;
    float smoothedSlideAmount;
    tNoise*  noise;
    float slideNoise;
    float slideGain;
    uint32_t wound;
    tExpSmooth*  barPulse;
    float barPulseInc;
    uint32_t barPulsePhasor;
    tSVF*  barResonator;
    float barPosition;
    float prevBarPosition;
    float openStringLength;
    float pickupRatio;
    float lastBump;
    float timeSinceLastBump;
    uint32_t sampleCount;
    tSlide*  barSmooth;
    tHighpass*  barHP;
    tSVF* barLP;
    tSlide* barPulseSlide;
    tExpSmooth* barSmooth2;
    tExpSmooth* barSmoothVol;
    float prevBarPosSmoothVol;
    float prevBumpSmoothed;
    float prevBarPosSmoothed;
    float bumpSmoothed;
    float barDrive;
    uint32_t bumpOsc;
    uint32_t bumpCount;
    float phaseComp;
    float poleCoeff;
    float muted;
    uint32_t inharmonic;
    float inharmonicMult;
    uint32_t maxDelay;
    uint32_t tensionJumps;
    tFeedbackLeveler* feedback;
    tFeedbackLeveler* feedbackP;
    float feedbackNoise;
    float feedbackNoiseLevel;
    float quarterSampleRate;
    float windingsPerInch;
    uint32_t wavelength;
    float pluckRatio;
    float pickup_Ratio;
    tExpSmooth* pickNoise;
    tNoise* pickNoiseSource;
    float pluckPoint_forInput;
    tSVF* peakFilt;
    float pickupAmount;
    tPickupNonLinearity* p;
} tTString;

void    tTString_create               (tMempool** const mempool, tTString** const);
void    tTString_init                 (LEAF* const leaf, tTString* const, int oversampling, float lowestFreq);
void    tTString_free                     (tTString** const);

float  tTString_tick                     (tTString* const);

void    tTString_setDecay                 (tTString* const bw, float decay);
void    tTString_mute                     (tTString* const bw);
void    tTString_setFilter                (tTString* const bw, float filter);
void    tTString_setTensionGain           (tTString* const bw, float tensionGain);
void    tTString_setTensionSpeed          (tTString* const bw, float tensionSpeed);
void    tTString_setFreq                  (tTString* const, float freq);
void    tTString_pluck                    (tTString* const bw, float position, float amplitude);
void    tTString_setWavelength            (tTString* const, uint32_t waveLength); // in samples
void    tTString_setSampleRate            (tTString* const, float sr);
void    tTString_setHarmonicity           (tTString* const, float B, float freq);
void    tTString_setRippleDepth           (tTString* const bw, float depth);
void    tTString_setHarmonic              (tTString* const bw, float harmonic);
void    tTString_setPhantomHarmonicsGain  (tTString* const bw, float gain);
void    tTString_setSlideGain             (tTString* const bw, float gain);
void    tTString_setPickupPos             (tTString* const bw, float pos);
void    tTString_setPickupModAmp          (tTString* const bw, float amp);
void    tTString_setPickupModFreq         (tTString* const bw, float freq);
void    tTString_setBarPosition           (tTString* const bw, float barPosition);
void    tTString_setOpenStringFrequency   (tTString* const bw, float openStringFrequency);
void    tTString_setPickupRatio           (tTString* const bw, float ratio);
void    tTString_setBarDrive              (tTString* const bw, float drive);
void    tTString_setFeedbackStrength      (tTString* const bw, float strength);
void    tTString_setFeedbackReactionSpeed (tTString* const bw, float speed);
void    tTString_setInharmonic            (tTString* const bw, uint32_t onOrOff);
void    tTString_setWoundOrUnwound        (tTString* const bw, uint32_t wound);
void    tTString_setWindingsPerInch       (tTString* const bw, uint32_t windings);
void    tTString_setPickupFilterFreq      (tTString* const bw, float cutoff);
void    tTString_setPickupFilterQ         (tTString* const bw, float Q);
void    tTString_setPeakFilterFreq        (tTString* const bw, float freq);
void    tTString_setPeakFilterQ           (tTString* const bw, float Q);
void    tTString_setFilterFreqDirectly    (tTString* const bw, float freq);
void    tTString_setDecayInSeconds        (tTString* const bw, float decay);
void    tTString_setPickupAmount          (tTString* const bw, float amount);
/*!
 *   *
     @defgroup treedtable tReedTable
     @ingroup physical
     @brief Reed Table - borrowed from STK
     @{
     
     @fn void    tReedTable_init(tReedTable* const, float offset, float slope, LEAF* const leaf)
     @brief Initialize a tReedTable to the default mempool of a LEAF instance.
     @param reed A pointer to the tReedTable to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tReedTable_init                 (tMempool** const)
     @brief Initialize a tReedTable to a specified mempool.
     @param reed A pointer to the tReedTable to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tReedTable_free(tReedTable** const)
     @brief Free a tReedTable from its mempool.
     @param reed A pointer to the tReedTable to free.
     
     @fn float   tReedTable_tick         (tReedTable* const, tReedTable** const, float offset, float slope, float input)
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
    
    typedef struct tReedTable
    {

        tMempool* mempool;
        float offset, slope;
    } tReedTable;

    void    tReedTable_create               (tMempool** const mempool, tReedTable** const);
    void    tReedTable_init                 (LEAF* const leaf, tReedTable* const, float offset, float slope);
    void    tReedTable_free         (tReedTable** const);
    
    float  tReedTable_tick         (tReedTable* const, float input);
    float  tReedTable_tanh_tick    (tReedTable* const, float input); //tanh softclip version of reed table - replacing the hard clip in original stk code

    void    tReedTable_setOffset    (tReedTable* const, float offset);
    void    tReedTable_setSlope     (tReedTable* const, float slope);

//==============================================================================

typedef struct tStiffString
    {
        tMempool* mempool;
        int numModes;
        tCycle **oscs; // array of oscillators
        float *amplitudes;
        float *outputWeights;
        float freqHz;        // the frequency of the whole string, determining delay length

        float stiffness;
        float pluckPos;    // the pick position, dividing the string in two, in ratio
        float pickupPos;    // the preparation position, dividing the string in two, in ratio
        float decay;
        float decayHighFreq;
        float sampleRate;
        float twoPiTimesInvSampleRate;
        float *decayScalar;
        float *decayVal;
        float *nyquistCoeff;
        float nyquist;
        float nyquistScalingFactor;
        float muteDecay;
        float amp;
        float gainComp;
    } tStiffString;

    void    tStiffString_create               (tMempool** const mempool, tStiffString** const);
    void    tStiffString_init                 (LEAF* const leaf, tStiffString* const, int numModes);
    void    tStiffString_free                     (tStiffString** const);

    float  tStiffString_tick                     (tStiffString* const);
    void    tStiffString_setStiffness             (tStiffString* const, float newValue);
    void    tStiffString_setFreq                  (tStiffString* const, float newFreq);
    void    tStiffString_pluck                    (tStiffString* const, float amp);
    void    tStiffString_setPickupPos             (tStiffString* const, float pickuppos);
    void    tStiffString_setPluckPos              (tStiffString* const, float pluckpos);
    void    tStiffString_setDecay                 (tStiffString* const, float decay);
    void    tStiffString_setDecayHighFreq         (tStiffString* const, float decayHF);
    void    tStiffString_updateOscillators        (tStiffString* const pm);
    void    tStiffString_updateOutputWeights      (tStiffString* const pm);
    void    tStiffString_mute                     (tStiffString* const pm);
    void    tStiffString_setStiffnessNoUpdate     (tStiffString* const, float newValue);
    void    tStiffString_setFreqNoUpdate          (tStiffString* const, float newFreq);
    void    tStiffString_pluckNoUpdate            (tStiffString* const, float amp);
    void    tStiffString_setPickupPosNoUpdate     (tStiffString* const, float pickuppos);
    void    tStiffString_setPluckPosNoUpdate      (tStiffString* const, float pluckpos);
    void    tStiffString_setDecayNoUpdate         (tStiffString* const, float decay);
    void    tStiffString_setDecayHighFreqNoUpdate (tStiffString* const, float decayHF);





    typedef struct tStereoRotation
    {
        tMempool* mempool;
        float angle;
        float vcaoutx;
        float vcaouty;
        tHighpass* hip1;
        tHighpass* hip2;
        float rotGain;
        tLagrangeDelay* rotDelayx;
        tLagrangeDelay* rotDelayy;
        tOnePole* filtx;
        tOnePole* filty;
        float feedbackFactorx;
        float feedbackFactory;
    } tStereoRotation;

    void    tStereoRotation_create               (tMempool** const mempool, tStereoRotation** const);
    void    tStereoRotation_init                 (LEAF* const leaf, tStereoRotation* const rr);

    void    tStereoRotation_tick                    (tStereoRotation* const r, float* samples);
    void    tStereoRotation_tickIn                  (tStereoRotation* const r, float* samples);
    void    tStereoRotation_tickOut                 (tStereoRotation* const r, float* samples);
    void    tStereoRotation_setAngle                (tStereoRotation* const r, float input);
    void    tStereoRotation_setDelayX               (tStereoRotation* const r, float time);
    void    tStereoRotation_setDelayY               (tStereoRotation* const r, float time);
    void    tStereoRotation_setFeedbackX            (tStereoRotation* const r, float feedbackx);
    void    tStereoRotation_setFeedbackY            (tStereoRotation* const r, float feedbacky);
    void    tStereoRotation_setFilterX              (tStereoRotation* const r, float freq);
    void    tStereoRotation_setFilterY              (tStereoRotation* const r, float freq);
    void    tStereoRotation_setGain                 (tStereoRotation* const r, float gain);


#ifdef __cplusplus
}
#endif

#endif // LEAF_PHYSICAL_H_INCLUDED

//==============================================================================
