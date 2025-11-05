/*==============================================================================
 
 leaf-filters.h
 Created: 20 Jan 2017 12:01:10pm
 Author:  Michael R Mulshine
 
 ==============================================================================*/

#ifndef LEAF_FILTERS_H_INCLUDED
#define LEAF_FILTERS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-delay.h"
#include "leaf-tables.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    /*!
     @defgroup tallpass tAllpass
     @ingroup filters
     @brief Schroeder allpass. Comb-filter with feedforward and feedback.
     @{
     
     @fn void    tAllpass_init(tAllpass** const, float initDelay, uint32_t maxDelay, LEAF* const leaf)
     @brief Initialize a tAllpass to the default mempool of a LEAF instance.
     @param filter A pointer to the tAllpass to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tAllpass_initToPool(tAllpass** const, float initDelay, uint32_t maxDelay, tMempool** const)
     @brief Initialize a tAllpass to a specified mempool.
     @param filter A pointer to the tAllpass to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tAllpass_free(tAllpass** const)
     @brief Free a tAllpass from its mempool.
     @param filter A pointer to the tAllpass to free.
     
     @fn float   tAllpass_tick           (tAllpass* const, float input)
     @brief
     @param filter A pointer to the relevant tAllpass.
     
     @fn void    tAllpass_setGain        (tAllpass* const, float gain)
     @brief
     @param filter A pointer to the relevant tAllpass.
     
     @fn void    tAllpass_setDelay       (tAllpass* const, float delay)
     @brief
     @param filter A pointer to the relevant tAllpass.
     ￼￼￼
     @} */
    
    typedef struct tAllpass
    {
        tMempool* mempool;
        
        float gain;
        
        tLinearDelay* delay;
        
        float lastOut;
    } tAllpass;

    // Memory handlers for `tAllpass`
    void    tAllpass_init           (tAllpass** const, float initDelay, uint32_t maxDelay, LEAF* const leaf);
    void    tAllpass_initToPool     (tAllpass** const, float initDelay, uint32_t maxDelay, tMempool** const);
    void    tAllpass_free           (tAllpass** const);

    // Tick function for `tAllpass`
    float   tAllpass_tick          (tAllpass* const, float input);

    // Setter functions for `tAllpass`
    void    tAllpass_setGain        (tAllpass* const, float gain);
    void    tAllpass_setDelay       (tAllpass* const, float delay);
    
    //==============================================================================

    /*!
    @defgroup tallpass tAllpassSO
    @ingroup filters
    @brief Schroeder allpass. Comb-filter with feedforward and feedback.
    @{

    @fn void    tAllpassSO_init(tAllpassSO** const, float initDelay, uint32_t maxDelay, LEAF* const leaf)
    @brief Initialize a tAllpassSO to the default mempool of a LEAF instance.
    @param filter A pointer to the tAllpassSO to initialize.
    @param leaf A pointer to the leaf instance.
 
    @fn void    tAllpassSO_initToPool(tAllpassSO** const, float initDelay, uint32_t maxDelay, tMempool** const)
    @brief Initialize a tAllpassSO to a specified mempool.
    @param filter A pointer to the tAllpassSO to initialize.
    @param mempool A pointer to the tMempool to use.

    @fn void    tAllpassSO_free(tAllpassSO** const)
    @brief Free a tAllpassSO from its mempool.
    @param filter A pointer to the tAllpassSO to free.

    @fn float   tAllpassSO_tick           (tAllpassSO* const, float input)
    @brief
    @param filter A pointer to the relevant tAllpassSO.
 
    @fn void    tAllpassSO_setGain        (tAllpassSO* const, float gain)
    @brief
    @param filter A pointer to the relevant tAllpassSO.

    @fn void    tAllpassSO_setDelay       (tAllpassSO* const, float delay)
    @brief
    @param filter A pointer to the relevant tAllpassSO.
    ￼￼￼
    @} */

    typedef struct tAllpassSO
    {

        tMempool* mempool;

        float prevSamp;
        float prevPrevSamp;
        float prevSamp2;
        float prevPrevSamp2;
        float a1;
        float a2;

    } tAllpassSO;

    // Memory handlers for `tAllpassSO`
    void    tAllpassSO_init           (tAllpassSO** const, LEAF* const leaf);
    void    tAllpassSO_initToPool     (tAllpassSO** const, tMempool** const);
    void    tAllpassSO_free           (tAllpassSO** const);

    // Tick function for `tAllpassSO`
    float  tAllpassSO_tick           (tAllpassSO* const, float input);

    // Setter functions for `tAllpassSO`
    void    tAllpassSO_setCoeff       (tAllpassSO* const ft, float a1, float a2);
    //==============================================================================


    typedef struct tThiranAllpassSOCascade
    {

        tMempool* mempool;

        int numFilts;
        tAllpassSO** filters;
        float B;
        float iKey;
        float a[2];
    
        float k1[2];
        float k2[2];
        float k3[2];
        float C1[2];
        float C2[2];
        int numActiveFilters;
        int numFiltsMap[2];
        int isHigh;
        float D;
    } tThiranAllpassSOCascade;

    // Memory handlers for `tThiranAllpassSOCascade`
    void    tThiranAllpassSOCascade_init           (tThiranAllpassSOCascade** const, int order, LEAF* const leaf);
    void    tThiranAllpassSOCascade_initToPool     (tThiranAllpassSOCascade** const, int order, tMempool** const);
    void    tThiranAllpassSOCascade_free           (tThiranAllpassSOCascade** const);

    // Tick function for `tThiranAllpassSOCascade`
    float  tThiranAllpassSOCascade_tick           (tThiranAllpassSOCascade* const, float input);

    // Setter functions for `tThiranAllpassSOCascade`
    float  tThiranAllpassSOCascade_setCoeff       (tThiranAllpassSOCascade* const ft, float dispersionCoeff, float freq, float invOversampling);
    void    tThiranAllpassSOCascade_clear          (tThiranAllpassSOCascade* const ft);
    //==============================================================================

    /*!
     @defgroup tonepole tOnePole
     @ingroup filters
     @brief OnePole filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tOnePole_init(tOnePole** const, float thePole, LEAF* const leaf)
     @brief Initialize a tOnePole to the default mempool of a LEAF instance.
     @param filter A pointer to the tOnePole to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tOnePole_initToPool(tOnePole** const, float thePole, tMempool** const)
     @brief Initialize a tOnePole to a specified mempool.
     @param filter A pointer to the tOnePole to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tOnePole_free(tOnePole** const)
     @brief Free a tOnePole from its mempool.
     @param filter A pointer to the tOnePole to free.
     
     @fn float   tOnePole_tick           (tOnePole* const, float input)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setB0          (tOnePole* const, float b0)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setA1          (tOnePole* const, float a1)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setPole        (tOnePole* const, float thePole)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setFreq        (tOnePole* const, float freq)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setCoefficients(tOnePole* const, float b0, float a1)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setGain        (tOnePole* const, float gain)
     @brief
     @param filter A pointer to the relevant tOnePole.
     ￼￼￼
     @} */
    
    typedef struct tOnePole
    {

        tMempool* mempool;
        float freq;
        float gain;
        float a0,a1;
        float b0,b1;
        float lastIn, lastOut;
        float twoPiTimesInvSampleRate;
    } tOnePole;

    // Memory handlers for `tOnePole`
    void    tOnePole_init            (tOnePole** const, float freq, LEAF* const leaf);
    void    tOnePole_initToPool      (tOnePole** const, float freq, tMempool** const);
    void    tOnePole_free            (tOnePole** const);

    // Tick function for `tOnePole`
    float   tOnePole_tick           (tOnePole* const, float input);

    // Setter functions for `tOnePole`
    void    tOnePole_setB0           (tOnePole* const, float b0);
    void    tOnePole_setA1           (tOnePole* const, float a1);
    void    tOnePole_setPole         (tOnePole* const, float thePole);
    void    tOnePole_setFreq         (tOnePole* const, float freq);
    void    tOnePole_setCoefficients (tOnePole* const, float b0, float a1);
    void    tOnePole_setGain         (tOnePole* const, float gain);
    void    tOnePole_setSampleRate   (tOnePole* const, float sr);
    
    //==============================================================================

    /*!
    @defgroup tonepole tOnePole
    @ingroup filters
    @brief OnePole filter, reimplemented from STK (Cook and Scavone).
    @{

    @fn void    tOnePole_init(tOnePole** const, float thePole, LEAF* const leaf)
    @brief Initialize a tOnePole to the default mempool of a LEAF instance.
    @param filter A pointer to the tOnePole to initialize.
    @param leaf A pointer to the leaf instance.

    @fn void    tOnePole_initToPool(tOnePole** const, float thePole, tMempool** const)
    @brief Initialize a tOnePole to a specified mempool.
    @param filter A pointer to the tOnePole to initialize.
    @param mempool A pointer to the tMempool to use.

    @fn void    tOnePole_free(tOnePole** const)
    @brief Free a tOnePole from its mempool.
    @param filter A pointer to the tOnePole to free.

    @fn float   tOnePole_tick           (tOnePole* const, float input)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setB0          (tOnePole* const, float b0)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setA1          (tOnePole* const, float a1)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setPole        (tOnePole* const, float thePole)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setFreq        (tOnePole* const, float freq)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setCoefficients(tOnePole* const, float b0, float a1)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setGain        (tOnePole* const, float gain)
    @brief
    @param filter A pointer to the relevant tOnePole.
    ￼￼￼
    @} */

    typedef struct tCookOnePole
    {

        tMempool* mempool;
        float poleCoeff, sgain, output;
        float twoPiTimesInvSampleRate;
        float gain;
    } tCookOnePole;

    // Memory handlers for `tCookOnePole`
    void    tCookOnePole_init           (tCookOnePole** const, LEAF* const leaf);
    void    tCookOnePole_initToPool     (tCookOnePole** const, tMempool** const);
    void    tCookOnePole_free           (tCookOnePole** const);

    // Tick function for `tCookOnePole`
    float  tCookOnePole_tick           (tCookOnePole* const, float input);

    // Setter functions for `tCookOnePole`
    void    tCookOnePole_setPole        (tCookOnePole* const, float thePole);
    void    tCookOnePole_setGain        (tCookOnePole* const, float gain);
    void    tCookOnePole_setGainAndPole (tCookOnePole* const ft, float gain, float pole);
    void    tCookOnePole_setSampleRate  (tCookOnePole* const, float sr);

    //==============================================================================

    /*!
     @defgroup ttwopole tTwoPole
     @ingroup filters
     @brief TwoPole filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tTwoPole_init(tTwoPole** const, LEAF* const leaf)
     @brief Initialize a tTwoPole to the default mempool of a LEAF instance.
     @param filter A pointer to the tTwoPole to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTwoPole_initToPool(tTwoPole** const, tMempool** const)
     @brief Initialize a tTwoPole to a specified mempool.
     @param filter A pointer to the tTwoPole to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTwoPole_free(tTwoPole** const)
     @brief Free a tTwoPole from its mempool.
     @param filter A pointer to the tTwoPole to free.
     
     @fn float   tTwoPole_tick           (tTwoPole* const, float input)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setB0          (tTwoPole* const, float b0)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setA1          (tTwoPole* const, float a1)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setA2          (tTwoPole* const, float a2)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setResonance   (tTwoPole* const, float freq, float radius, int normalize)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setCoefficients(tTwoPole* const, float b0, float a1, float a2)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setGain        (tTwoPole* const, float gain)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     ￼￼￼
     @} */

    typedef struct tTwoPole
    {

        tMempool* mempool;
        
        float gain;
        float a0, a1, a2;
        float b0;
        
        float radius, frequency;
        int normalize;
        
        float lastOut[2];
        
        float sampleRate;
        float twoPiTimesInvSampleRate;
    } tTwoPole;

    // Memory handlers for `tTwoPole`
    void    tTwoPole_init            (tTwoPole** const, LEAF* const leaf);
    void    tTwoPole_initToPool      (tTwoPole** const, tMempool** const);
    void    tTwoPole_free            (tTwoPole** const);

    // Tick function for `tTwoPole`
    float  tTwoPole_tick            (tTwoPole* const, float input);

    // Setter functions for `tTwoPole`
    void    tTwoPole_setB0           (tTwoPole* const, float b0);
    void    tTwoPole_setA1           (tTwoPole* const, float a1);
    void    tTwoPole_setA2           (tTwoPole* const, float a2);
    void    tTwoPole_setResonance    (tTwoPole* const, float freq, float radius, int normalize);
    void    tTwoPole_setCoefficients (tTwoPole* const, float b0, float a1, float a2);
    void    tTwoPole_setGain         (tTwoPole* const, float gain);
    void    tTwoPole_setSampleRate   (tTwoPole* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tonezero tOneZero
     @ingroup filters
     @brief OneZero filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tOneZero_init(tOneZero** const, float theZero, LEAF* const leaf)
     @brief Initialize a tOneZero to the default mempool of a LEAF instance.
     @param filter A pointer to the tSlide to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tOneZero_initToPool(tOneZero** const, float theZero, tMempool** const)
     @brief Initialize a tOneZero to a specified mempool.
     @param filter A pointer to the tOneZero to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tOneZero_free(tOneZero** const)
     @brief Free a tOneZero from its mempool.
     @param filter A pointer to the tOneZero to free.
     
     @fn float   tOneZero_tick           (tOneZero* const, float input)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setB0          (tOneZero* const, float b0)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setB1          (tOneZero* const, float b1)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setZero        (tOneZero* const, float theZero)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setCoefficients(tOneZero* const, float b0, float b1)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setGain        (tOneZero* const, float gain)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn float   tOneZero_getPhaseDelay  (tOneZero *f, float frequency)
     @brief
     @param filter A pointer to the relevant tOneZero.
     ￼￼￼
     @} */
    
    typedef struct tOneZero
    {
        tMempool* mempool;
        float gain;
        float b0,b1;
        float lastIn, lastOut, frequency;
        float invSampleRate;
    } tOneZero;

    // Memory handlers for `tOneZero`
    void    tOneZero_init           (tOneZero** const, float theZero, LEAF* const leaf);
    void    tOneZero_initToPool     (tOneZero** const, float theZero, tMempool** const);
    void    tOneZero_free           (tOneZero** const);

    // Tick function for `tOneZero`
    float  tOneZero_tick            (tOneZero* const, float input);

    // Setter functions for `tOneZero`
    void    tOneZero_setB0           (tOneZero* const, float b0);
    void    tOneZero_setB1           (tOneZero* const, float b1);
    void    tOneZero_setZero         (tOneZero* const, float theZero);
    void    tOneZero_setCoefficients (tOneZero* const, float b0, float b1);
    void    tOneZero_setGain         (tOneZero* const, float gain);
    float  tOneZero_getPhaseDelay   (tOneZero* const, float frequency);
    void    tOneZero_setSampleRate   (tOneZero* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup ttwozero tTwoZero
     @ingroup filters
     @brief TwoZero filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tTwoZero_init(tTwoZero** const, LEAF* const leaf)
     @brief Initialize a tTwoZero to the default mempool of a LEAF instance.
     @param filter A pointer to the tTwoZero to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTwoZero_initToPool(tTwoZero** const, tMempool** const)
     @brief Initialize a tTwoZero to a specified mempool.
     @param filter A pointer to the tTwoZero to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTwoZero_free(tTwoZero** const)
     @brief Free a tTwoZero from its mempool.
     @param filter A pointer to the tTwoZero to free.
     
     @fn float   tTwoZero_tick           (tTwoZero* const, float input)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setB0          (tTwoZero* const, float b0)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setB1          (tTwoZero* const, float b1)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setB2          (tTwoZero* const, float b2)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setNotch       (tTwoZero* const, float frequency, float radius)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setCoefficients(tTwoZero* const, float b0, float b1, float b2)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setGain        (tTwoZero* const, float gain)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     ￼￼￼
     @} */

    typedef struct tTwoZero
    {
        tMempool* mempool;
        
        float gain;
        float b0, b1, b2;
        float frequency, radius;
        float lastIn[2];
        float twoPiTimesInvSampleRate;
    } tTwoZero;

    // Memory handlers for `tTwoZero`
    void    tTwoZero_init           (tTwoZero** const, LEAF* const leaf);
    void    tTwoZero_initToPool     (tTwoZero** const, tMempool** const);
    void    tTwoZero_free           (tTwoZero** const);

    // Tick function for `tTwoZero`
    float  tTwoZero_tick           (tTwoZero* const, float input);

    // Setter functions for `tTwoZero`
    void    tTwoZero_setB0          (tTwoZero* const, float b0);
    void    tTwoZero_setB1          (tTwoZero* const, float b1);
    void    tTwoZero_setB2          (tTwoZero* const, float b2);
    void    tTwoZero_setNotch       (tTwoZero* const, float frequency, float radius);
    void    tTwoZero_setCoefficients(tTwoZero* const, float b0, float b1, float b2);
    void    tTwoZero_setGain        (tTwoZero* const, float gain);
    void    tTwoZero_setSampleRate  (tTwoZero* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tpolezero tPoleZero
     @ingroup filters
     @brief PoleZero filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tPoleZero_init(tPoleZero** const, LEAF* const leaf)
     @brief Initialize a tPoleZero to the default mempool of a LEAF instance.
     @param filter A pointer to the tPoleZero to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPoleZero_initToPool(tPoleZero** const, tMempool** const)
     @brief Initialize a tPoleZero to a specified mempool.
     @param filter A pointer to the tPoleZero to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPoleZero_free(tPoleZero** const)
     @brief Free a tPoleZero from its mempool.
     @param filter A pointer to the tPoleZero to free.
     
     
     @fn float   tPoleZero_tick              (tPoleZero* const, float input)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setB0             (tPoleZero* const, float b0)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setB1             (tPoleZero* const, float b1)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setA1             (tPoleZero* const, float a1)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setCoefficients   (tPoleZero* const, float b0, float b1, float a1)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setAllpass        (tPoleZero* const, float coeff)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setBlockZero      (tPoleZero* const, float thePole)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setGain           (tPoleZero* const, float gain)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     ￼￼￼
     @} */
    
    typedef struct tPoleZero
    {

        tMempool* mempool;
        
        float gain;
        float a1;
        float b0,b1;
        
        float lastIn, lastOut;
    } tPoleZero;

    // Memory handlers for `tPoleZero`
    void    tPoleZero_init              (tPoleZero** const, LEAF* const leaf);
    void    tPoleZero_initToPool        (tPoleZero** const, tMempool** const);
    void    tPoleZero_free              (tPoleZero** const);

    // Tick function for `tPoleZero`
    float  tPoleZero_tick              (tPoleZero* const, float input);

    // Setter functions for `tPoleZero`
    void    tPoleZero_setB0             (tPoleZero* const, float b0);
    void    tPoleZero_setB1             (tPoleZero* const, float b1);
    void    tPoleZero_setA1             (tPoleZero* const, float a1);
    void    tPoleZero_setCoefficients   (tPoleZero* const, float b0, float b1, float a1);
    void    tPoleZero_setAllpass        (tPoleZero* const, float coeff);
    void    tPoleZero_setBlockZero      (tPoleZero* const, float thePole);
    void    tPoleZero_setGain           (tPoleZero* const, float gain);
    
    //==============================================================================
    
    /*!
     @defgroup tbiquad tBiQuad
     @ingroup filters
     @brief BiQuad filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tBiQuad_init(tBiQuad** const, LEAF* const leaf)
     @brief Initialize a tBiQuad to the default mempool of a LEAF instance.
     @param filter A pointer to the tBiQuad to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tBiQuad_initToPool(tBiQuad** const, tMempool** const)
     @brief Initialize a tBiQuad to a specified mempool.
     @param filter A pointer to the tBiQuad to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tBiQuad_free(tBiQuad** const)
     @brief Free a tBiQuad from its mempool.
     @param filter A pointer to the tBiQuad to free.
     
     
     @fn float   tBiQuad_tick           (tBiQuad* const, float input)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setB0          (tBiQuad* const, float b0)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setB1          (tBiQuad* const, float b1)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setB2          (tBiQuad* const, float b2)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setA1          (tBiQuad* const, float a1)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setA2          (tBiQuad* const, float a2)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setNotch       (tBiQuad* const, float freq, float radius)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setResonance   (tBiQuad* const, float freq, float radius, int normalize)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setCoefficients(tBiQuad* const, float b0, float b1, float b2, float a1, float a2)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setGain        (tBiQuad* const, float gain)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     ￼￼￼
     @} */
    
    typedef struct tBiQuad
    {
        tMempool* mempool;
        
        float gain;
        float a0, a1, a2;
        float b0, b1, b2;
        
        float lastIn[2];
        float lastOut[2];
        
        float frequency, radius;
        int normalize;
        
        float sampleRate;
        float twoPiTimesInvSampleRate;
    } tBiQuad;

    // Memory handlers for `tBiQuad`
    void    tBiQuad_init           (tBiQuad** const, LEAF* const leaf);
    void    tBiQuad_initToPool     (tBiQuad** const, tMempool** const);
    void    tBiQuad_free           (tBiQuad** const);

    // Tick function for `tBiQuad`
    float  tBiQuad_tick           (tBiQuad* const, float input);

    // Setter functions for `tBiQuad`
    void    tBiQuad_setB0          (tBiQuad* const, float b0);
    void    tBiQuad_setB1          (tBiQuad* const, float b1);
    void    tBiQuad_setB2          (tBiQuad* const, float b2);
    void    tBiQuad_setA1          (tBiQuad* const, float a1);
    void    tBiQuad_setA2          (tBiQuad* const, float a2);
    void    tBiQuad_setNotch       (tBiQuad* const, float freq, float radius);
    void    tBiQuad_setResonance   (tBiQuad* const, float freq, float radius, int normalize);
    void    tBiQuad_setCoefficients(tBiQuad* const, float b0, float b1, float b2, float a1, float a2);
    void    tBiQuad_setGain        (tBiQuad* const, float gain);
    void    tBiQuad_setSampleRate  (tBiQuad* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tsvf tSVF
     @ingroup filters
     @brief State Variable Filter, algorithm from Andy Simper.
     @{
     
     @fn void    tSVF_init(tSVF** const, SVFType type, float freq, float Q, LEAF* const leaf)
     @brief Initialize a tSVF to the default mempool of a LEAF instance.
     @param filter A pointer to the tSVF to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSVF_initToPool(tSVF** const, SVFType type, float freq, float Q, tMempool** const)
     @brief Initialize a tSVF to a specified mempool.
     @param filter A pointer to the tSVF to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSVF_free(tSVF** const)
     @brief Free a tSVF from its mempool.
     @param filter A pointer to the tSVF to free.
     
     @fn float   tSVF_tick           (tSVF* const, float v0)
     @brief
     @param filter A pointer to the relevant tSVF.
     
     @fn void    tSVF_setFreq        (tSVF* const, float freq)
     @brief
     @param filter A pointer to the relevant tSVF.
     
     @fn void    tSVF_setQ           (tSVF* const, float Q)
     @brief
     @param filter A pointer to the relevant tSVF.
     
     @fn void    tSVF_setFreqAndQ    (tSVF* const svff, float freq, float Q)
     @brief
     @param filter A pointer to the relevant tSVF.
     ￼￼￼
     @fn void    tSVF_setFilterType    (tSVF* const svff, SVFType type)
     @brief
     @param filter A pointer to the relevant tSVF.
     @param type which kind of filter do you want to change the SVF to?
     @} */
    
    typedef enum SVFType
    {
        SVFTypeHighpass = 0,
        SVFTypeLowpass,
        SVFTypeBandpass,
        SVFTypeNotch,
        SVFTypePeak,
        SVFTypeLowShelf,
        SVFTypeHighShelf
    } SVFType;
    
    typedef struct tSVF
    {
        tMempool* mempool;
        SVFType type;
        float cutoff, Q, cutoffMIDI;
        float ic1eq,ic2eq;
        float g,k,a1,a2,a3,cH,cB,cL,cBK;
        float sampleRate;
        float invSampleRate;
        float phaseComp;
        float sampleRatio;
        const float *table;
    } tSVF;

    // Memory handlers for `tSVF`
    void    tSVF_init                (tSVF** const, SVFType type, float freq, float Q, LEAF* const leaf);
    void    tSVF_initToPool          (tSVF** const, SVFType type, float freq, float Q, tMempool** const);
    void    tSVF_free                (tSVF** const);

    // Tick functions for `tSVF`
    float  tSVF_tick                (tSVF* const, float v0);
    float  tSVF_tickLP              (tSVF* const, float v0);
    float  tSVF_tickHP              (tSVF* const, float v0);
    float  tSVF_tickBP              (tSVF* const, float v0);

    // Setter functions for `tSVF`
    void    tSVF_setFreq             (tSVF* const, float freq);
    void    tSVF_setFreqFast         (tSVF* const vf, float cutoff);
    void    tSVF_setQ                (tSVF* const, float Q);
    void    tSVF_setFreqAndQ         (tSVF* const svff, float freq, float Q);
    void    tSVF_setFreqAndQFast     (tSVF* const svff, float cutoff, float Q);
    void    tSVF_setFilterType       (tSVF* const svff, SVFType type);
    void    tSVF_setSampleRate       (tSVF* const svff, float sr);
    float  tSVF_getPhaseAtFrequency (tSVF* const svff, float freq);
    
    //==============================================================================

    typedef struct tSVF_LP
    {
        tMempool* mempool;
        float ic1eq,ic2eq;
        float g,onePlusg,k,a0,a1,a2,a3,a4,a5;
        float sampleRate;
        float invSampleRate;
        float phaseComp;
        float sampleRatio;
        uint32_t nan;
        const float *table;
    } tSVF_LP;

    // Memory handlers for `tSVF_LP`
    void    tSVF_LP_init                (tSVF_LP** const, float freq, float Q, LEAF* const leaf);
    void    tSVF_LP_initToPool          (tSVF_LP** const, float freq, float Q, tMempool** const);
    void    tSVF_LP_free                (tSVF_LP** const);

    // Tick function for `tSVF_LP`
    float  tSVF_LP_tick                (tSVF_LP* const, float v0);

    // Setter functions for `tSVF_LP`
    void    tSVF_LP_setFreq             (tSVF_LP* const, float freq);
    void    tSVF_LP_setFreqFast         (tSVF_LP* const vf, float cutoff);
    void    tSVF_LP_setQ                (tSVF_LP* const, float Q);
    void    tSVF_LP_setFreqAndQ         (tSVF_LP* const svff, float freq, float Q);
    void    tSVF_LP_setFreqAndQFast     (tSVF_LP* const svff, float cutoff, float Q);
    void    tSVF_LP_setSampleRate       (tSVF_LP* const svff, float sr);
    float  tSVF_LP_getPhaseAtFrequency (tSVF_LP* const svff, float freq);
    //==============================================================================
    
    /*!
     @defgroup tefficientsvf tEfficientSVF
     @ingroup filters
     @brief Efficient State Variable Filter for 14-bit control input, [0, 4096).
     @{
     
     @fn void    tEfficientSVF_init(tEfficientSVF** const, SVFType type, uint16_t input, float Q, LEAF* const leaf)
     @brief Initialize a tEfficientSVF to the default mempool of a LEAF instance.
     @param filter A pointer to the tEfficientSVF to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tEfficientSVF_initToPool(tEfficientSVF** const, SVFType type, uint16_t input, float Q, tMempool** const)
     @brief Initialize a tEfficientSVF to a specified mempool.
     @param filter A pointer to the tEfficientSVF to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tEfficientSVF_free(tEfficientSVF** const)
     @brief Free a tEfficientSVF from its mempool.
     @param filter A pointer to the tEfficientSVF to free.
     
     @fn float   tEfficientSVF_tick          (tEfficientSVF* const, float v0)
     @brief
     @param filter A pointer to the relevant tEfficientSVF.
     
     @fn void    tEfficientSVF_setFreq       (tEfficientSVF* const, uint16_t controlFreq)
     @brief
     @param filter A pointer to the relevant tEfficientSVF.
     
     @fn void    tEfficientSVF_setQ          (tEfficientSVF* const, float Q)
     @brief
     @param filter A pointer to the relevant tEfficientSVF.
     ￼￼￼
     @} */
    
    typedef struct tEfficientSVF
    {

        tMempool* mempool;
        SVFType type;
        float cutoff, Q;
        float ic1eq,ic2eq;
        float g,k,a1,a2,a3;
        const float *table;
    } tEfficientSVF;

    // Memory handlers for `tEfficientSVF`
    void    tEfficientSVF_init          (tEfficientSVF** const, SVFType type, uint16_t input, float Q, LEAF* const leaf);
    void    tEfficientSVF_initToPool    (tEfficientSVF** const, SVFType type, uint16_t input, float Q, tMempool** const);
    void    tEfficientSVF_free          (tEfficientSVF** const);

    // Tick function for `tEfficientSVF`
    float  tEfficientSVF_tick          (tEfficientSVF* const, float v0);

    // Setter functions for `tEfficientSVF`
    void    tEfficientSVF_setFreq       (tEfficientSVF* const svff, float cutoff);
    void    tEfficientSVF_setQ          (tEfficientSVF* const, float Q);
    void    tEfficientSVF_setFreqAndQ   (tEfficientSVF* const, uint16_t controlFreq, float Q);
    void    tEfficientSVF_setSampleRate (tEfficientSVF* const, float sampleRate);
    //==============================================================================
    
    /*!
     @defgroup thighpass tHighpass
     @ingroup filters
     @brief Simple Highpass filter.
     @{
     
     @fn void    tHighpass_init(tHighpass** const, float freq, LEAF* const leaf)
     @brief Initialize a tHighpass to the default mempool of a LEAF instance.
     @param filter A pointer to the tHighpass to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tHighpass_initToPool(tHighpass** const, float freq, tMempool** const)
     @brief Initialize a tHighpass to a specified mempool.
     @param filter A pointer to the tHighpass to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tHighpass_free(tHighpass** const)
     @brief Free a tHighpass from its mempool.
     @param filter A pointer to the tHighpass to free.
     
     @fn float   tHighpass_tick          (tHighpass* const, float x)
     @brief
     @param filter A pointer to the relevant tHighpass.
     
     @fn void    tHighpass_setFreq       (tHighpass* const, float freq)
     @brief
     @param filter A pointer to the relevant tHighpass.
     
     @fn float   tHighpass_getFreq       (tHighpass* const)
     @brief
     @param filter A pointer to the relevant tHighpass.
     ￼￼￼
     @} */
    
    typedef struct tHighpass
    {
        tMempool* mempool;
        float xs, ys, R;
        float frequency;
        float twoPiTimesInvSampleRate;
    } tHighpass;

    // Memory handlers for `tHighpass`
    void    tHighpass_init          (tHighpass** const, float freq, LEAF* const leaf);
    void    tHighpass_initToPool    (tHighpass** const, float freq, tMempool** const);
    void    tHighpass_free          (tHighpass** const);

    // Tick function for `tHighpass`
    float  tHighpass_tick          (tHighpass* const, float x);

    // Setter functions for `tHighpass`
    void    tHighpass_setFreq       (tHighpass* const, float freq);
    float  tHighpass_getFreq       (tHighpass* const);
    void    tHighpass_setSampleRate (tHighpass* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tbutterworth tButterworth
     @ingroup filters
     @brief Butterworth filter.
     @{
     
     @fn void    tButterworth_init(tButterworth** const, int N, float f1, float f2, LEAF* const leaf, LEAF* const leaf)
     @brief Initialize a tButterworth to the default mempool of a LEAF instance.
     @param filter A pointer to the tButterworth to initialize.
     @param leaf A pointer to the leaf instance.
     @param order Order of the filter.
     @param lowCutoff Lower cutoff frequency.
     @param upperCutoff Upper cutoff frequency.
     
     @fn void    tButterworth_initToPool(tButterworth** const, int N, float f1, float f2, tMempool** const)
     @brief Initialize a tButterworth to a specified mempool.
     @param filter A pointer to the tButterworth to initialize.
     @param mempool A pointer to the tMempool to use.
     @param order Order of the filter.
     @param lowCutoff Lower cutoff frequency.
     @param upperCutoff Upper cutoff frequency.
     
     @fn void    tButterworth_free(tButterworth** const)
     @brief Free a tButterworth from its mempool.
     @param filter A pointer to the tButterworth to free.
     
     @fn float   tButterworth_tick           (tButterworth* const, float input)
     @brief
     @param filter A pointer to the relevant tButterworth.
     
     @fn void    tButterworth_setF1          (tButterworth* const, float in)
     @brief
     @param filter A pointer to the relevant tButterworth.
     
     @fn void    tButterworth_setF2          (tButterworth* const, float in)
     @brief
     @param filter A pointer to the relevant tButterworth.
     
     @fn void    tButterworth_setFreqs       (tButterworth* const, float f1, float f2)
     @brief
     @param filter A pointer to the relevant tButterworth.
     ￼￼￼
     @} */
    
#define NUM_SVF_BW 16
    typedef struct tButterworth
    {
        tMempool* mempool;
        
        float gain;
        int order;
        int numSVF;
        
        tSVF** svfs;
        
        float f1,f2;
    } tButterworth;

    // Memory handlers for `tButterworth`
    void    tButterworth_init           (tButterworth** const, int N, float f1, float f2, LEAF* const leaf);
    void    tButterworth_initToPool     (tButterworth** const, int N, float f1, float f2, tMempool** const);
    void    tButterworth_free           (tButterworth** const);

    // Tick function for `tButterworth`
    float  tButterworth_tick           (tButterworth* const, float input);

    // Setter functions for `tButterworth`
    void    tButterworth_setF1          (tButterworth* const, float in);
    void    tButterworth_setF2          (tButterworth* const, float in);
    void    tButterworth_setFreqs       (tButterworth* const, float f1, float f2);
    void    tButterworth_setSampleRate  (tButterworth* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tfir tFIR
     @ingroup filters
     @brief Finite impulse response filter.
     @{
     
     @fn void    tFIR_init(tFIR** const, float* coeffs, int numTaps, LEAF* const leaf)
     @brief Initialize a tFIR to the default mempool of a LEAF instance.
     @param filter A pointer to the tFIR to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tFIR_initToPool(tFIR** const, float* coeffs, int numTaps, tMempool** const)
     @brief Initialize a tFIR to a specified mempool.
     @param filter A pointer to the tFIR to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tFIR_free(tFIR** const)
     @brief Free a tFIR from its mempool.
     @param filter A pointer to the tFIR to free.
     
     @fn float   tFIR_tick           (tFIR* const, float input)
     @brief
     @param filter A pointer to the relevant tFIR.
     ￼￼￼
     @} */
    
    typedef struct tFIR
    {

        tMempool* mempool;
        float* past;
        float* coeff;
        int numTaps;
    } tFIR;

    // Memory handlers for `tFIR`
    void    tFIR_init           (tFIR** const, float* coeffs, int numTaps, LEAF* const leaf);
    void    tFIR_initToPool     (tFIR** const, float* coeffs, int numTaps, tMempool** const);
    void    tFIR_free           (tFIR** const);

    // Tick function for `tFIR`
    float  tFIR_tick           (tFIR* const, float input);
    
    
    //==============================================================================
    
    /*!
     @defgroup tmedianfilter tMedianFilter
     @ingroup filters
     @brief Median filter.
     @{
     
     @fn void    tMedianFilter_init(tMedianFilter** const, int size, LEAF* const leaf)
     @brief Initialize a tMedianFilter to the default mempool of a LEAF instance.
     @param filter A pointer to the tMedianFilter to initialize.
     @param leaf A pointer to the leaf instance.f
     
     @fn void    tMedianFilter_initToPool(tMedianFilter** const, int size, tMempool** const)
     @brief Initialize a tMedianFilter to a specified mempool.
     @param filter A pointer to the tMedianFilter to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tMedianFilter_free(tMedianFilter** const)
     @brief Free a tMedianFilter from its mempool.
     @param filter A pointer to the tMedianFilter to free.
     
     @fn float   tMedianFilter_tick           (tMedianFilter* const, float input)
     @brief
     @param filter A pointer to the relevant tMedianFilter.
     ￼￼￼
     @} */
    
    typedef struct tMedianFilter
    {

        tMempool* mempool;
        float* val;
        int* age;
        int m;
        int size;
        int middlePosition;
        int last;
        int pos;
    } tMedianFilter;

    // Memory handlers for `tMedianFilter`
    void    tMedianFilter_init           (tMedianFilter** const, int size, LEAF* const leaf);
    void    tMedianFilter_initToPool     (tMedianFilter** const, int size, tMempool** const);
    void    tMedianFilter_free           (tMedianFilter** const);

    // Tick function for `tMedianFilter`
    float  tMedianFilter_tick           (tMedianFilter* const, float input);
    
    
    /*!
     @defgroup tvzfilter tVZFilter
     @ingroup filters
     @brief Vadim Zavalishin style from VA book (from implementation in RSlib posted to kvr forum)
     @{
     
     @fn void    tVZFilter_init(tVZFilter** const, VZFilterType type, float freq, float Q, LEAF* const leaf)
     @brief Initialize a tVZFilter to the default mempool of a LEAF instance.
     @param filter A pointer to the tVZFilter to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tVZFilter_initToPool(tVZFilter** const, VZFilterType type, float freq, float Q, tMempool** const)
     @brief Initialize a tVZFilter to a specified mempool.
     @param filter A pointer to the tVZFilter to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tVZFilter_free(tVZFilter** const)
     @brief Free a tVZFilter from its mempool.
     @param filter A pointer to the tVZFilter to free.
     
     @fn float   tVZFilter_tick               (tVZFilter* const, float input)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn float   tVZFilter_tickEfficient               (tVZFilter* const vf, float in)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_calcCoeffs           (tVZFilter* const)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setBandwidth            (tVZFilter* const, float bandWidth)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setFreq           (tVZFilter* const, float freq)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setFreqAndBandwidth    (tVZFilter* const vf, float freq, float bw)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setGain                  (tVZFilter* const, float gain)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setType                  (tVZFilter* const, VZFilterType type)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn float   tVZFilter_BandwidthToR        (tVZFilter* const vf, float B)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setSampleRate  (tVZFilter* const, float sampleRate)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     ￼￼￼
     @} */
    
    typedef enum VZFilterType
    {
        Highpass = 0,
        Lowpass,
        BandpassSkirt,
        BandpassPeak,
        BandReject,
        Bell,
        Lowshelf,
        Highshelf,
        Morph,
        Bypass,
        Allpass
    } VZFilterType;
    
    
    typedef struct tVZFilter
    {
        tMempool* mempool;
        
        VZFilterType type;
        // state:
        float s1, s2;
        
        // filter coefficients:
        float g;          // embedded integrator gain
        float R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        float h;          // factor for feedback (== 1/(1+2*R*g+g*g))
        float cL, cB, cH; // coefficients for low-, band-, and highpass signals
        
        // parameters:
        float fc;    // characteristic frequency
        float G;     // gain
        float invG;        //1/gain
        float Q; //q of filter
        float B;     // bandwidth (in octaves)
        float m;     // morph parameter (0...1)
        float R2Plusg; //precomputed for the tick
        float sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        float invSampleRate;
        float sampRatio;
        float cutoffMIDI;
        const float *table;
    } tVZFilter;

    // Memory handlers for `tVZFilter`
    void    tVZFilter_init                                (tVZFilter** const, VZFilterType type, float freq, float Q,
                                                           LEAF* const leaf);
    void    tVZFilter_initToPool                          (tVZFilter** const, VZFilterType type, float freq, float Q,
                                                           tMempool** const);
    void    tVZFilter_free                                (tVZFilter** const);

    // Tick functions for `tVZFilter`
    float  tVZFilter_tick                                (tVZFilter* const, float input);
    float  tVZFilter_tickEfficient                       (tVZFilter* const vf, float in);

    // Setter functions for `tVZFilter`
    void    tVZFilter_setSampleRate                       (tVZFilter* const, float sampleRate);
    void    tVZFilter_calcCoeffs                          (tVZFilter* const);
    void    tVZFilter_calcCoeffsEfficientBP               (tVZFilter* const);
    void    tVZFilter_setBandwidth                        (tVZFilter* const, float bandWidth);
    void    tVZFilter_setFreq                             (tVZFilter* const, float freq);
    void    tVZFilter_setFreqFast                         (tVZFilter* const vf, float cutoff);
    void    tVZFilter_setFreqAndBandwidth                 (tVZFilter* const vf, float freq, float bw);
    void    tVZFilter_setFreqAndBandwidthEfficientBP      (tVZFilter* const vf, float freq, float bw);
    void    tVZFilter_setGain                             (tVZFilter* const, float gain);
    void    tVZFilter_setResonance                        (tVZFilter* const vf, float res);
    void    tVZFilter_setFrequencyAndResonance            (tVZFilter* const vf, float freq, float res);
    void    tVZFilter_setFrequencyAndResonanceAndGain     (tVZFilter* const vf, float freq, float res, float gains);
    void    tVZFilter_setFastFrequencyAndResonanceAndGain (tVZFilter* const vf, float freq, float res, float gain);
    void    tVZFilter_setFrequencyAndBandwidthAndGain     (tVZFilter* const vf, float freq, float BW, float gain);
    void    tVZFilter_setFrequencyAndResonanceAndMorph    (tVZFilter* const vf, float freq, float res, float morph);
    void    tVZFilter_setMorphOnly                        (tVZFilter* const vf, float morph);
    void    tVZFilter_setMorph                            (tVZFilter* const vf, float morph);
    void    tVZFilter_setType                             (tVZFilter* const, VZFilterType type);
    float  tVZFilter_BandwidthToR                        (tVZFilter* const vf, float B);
    float  tVZFilter_BandwidthToREfficientBP             (tVZFilter* const vf, float B);
    

    typedef struct tVZFilterLS
    {
        tMempool* mempool;
        // state:
        float s1, s2;

        // filter coefficients:
        float g;          // embedded integrator gain
        float R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        float h;          // factor for feedback (== 1/(1+2*R*g+g*g))

        // parameters:
        float fc;    // characteristic frequency
        float G;     // gain
        float Q; //q of filter
        float B;     // bandwidth (in octaves)
        float R2Plusg; //precomputed for the tick
        float sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        float invSampleRate;
        float gPreDiv;
        float invSqrtA;
        float sampRatio; // ratio of the sample rate to 48000 (which is what the tanf table was calculated for)
        const float *table;
        float cutoffMIDI;
    } tVZFilterLS;

    // Memory handlers for `tVZFilterLS`
    void    tVZFilterLS_init                           (tVZFilterLS** const,float freq, float Q, float gain,
                                                        LEAF* const leaf);
    void    tVZFilterLS_initToPool                     (tVZFilterLS** const, float freq, float Q, float gain,
                                                        tMempool** const);
    void    tVZFilterLS_free                           (tVZFilterLS** const);

    // Tick function for `tVZFilterLS`
    float  tVZFilterLS_tick                           (tVZFilterLS* const, float input);

    // Setter functions for `tVZFilterLS`
    void    tVZFilterLS_setSampleRate                  (tVZFilterLS* const, float sampleRate);
    void    tVZFilterLS_setBandwidthSlow               (tVZFilterLS* const, float bandWidth);
    void    tVZFilterLS_setFreq                        (tVZFilterLS* const, float freq);
    void    tVZFilterLS_setFreqFast                    (tVZFilterLS* const vf, float cutoff);
    void    tVZFilterLS_setGain                        (tVZFilterLS* const, float gain);
    void    tVZFilterLS_setResonance                   (tVZFilterLS* const vf, float res);
    void    tVZFilterLS_setFreqFastAndResonanceAndGain (tVZFilterLS* const vf, float cutoff, float res, float gain);


    typedef struct tVZFilterHS
    {
        tMempool* mempool;
        // state:
        float s1, s2;

        // filter coefficients:
        float g;          // embedded integrator gain
        float R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        float h;          // factor for feedback (== 1/(1+2*R*g+g*g))


        // parameters:
        float fc;    // characteristic frequency
        float G;     // gain
        float Q; //q of filter
        float B;     // bandwidth (in octaves)
        float R2Plusg; //precomputed for the tick
        float sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        float invSampleRate;
        float gPreDiv;
        float sqrtA;
        float sampRatio; // ratio of the sample rate to 48000 (which is what the tanf table was calculated for)
        const float *table;
        float cutoffMIDI;
    } tVZFilterHS;

    // Memory handlers for `tVZFilterHS`
    void    tVZFilterHS_init                           (tVZFilterHS** const,float freq, float Q, float gain,
                                                        LEAF* const leaf);
    void    tVZFilterHS_initToPool                     (tVZFilterHS** const, float freq, float Q, float gain,
                                                        tMempool** const);
    void    tVZFilterHS_free                           (tVZFilterHS** const);

    // Tick function for `tVZFilterHS`
    float  tVZFilterHS_tick                           (tVZFilterHS* const, float input);

    // Setter functions for `tVZFilterHS`
    void    tVZFilterHS_setSampleRate                  (tVZFilterHS* const, float sampleRate);
    void    tVZFilterHS_setBandwidthSlow               (tVZFilterHS* const, float bandWidth);
    void    tVZFilterHS_setFreq                        (tVZFilterHS* const, float freq);
    void    tVZFilterHS_setFreqFast                    (tVZFilterHS* const vf, float cutoff);
    void    tVZFilterHS_setGain                        (tVZFilterHS* const, float gain);
    void    tVZFilterHS_setResonance                   (tVZFilterHS* const vf, float res);
    void    tVZFilterHS_setFreqFastAndResonanceAndGain (tVZFilterHS* const vf, float cutoff, float res, float gain);

    typedef struct tVZFilterBell
    {
        tMempool* mempool;
        // state:
        float s1, s2;

        // filter coefficients:
        float g;          // embedded integrator gain
        float R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        float h;          // factor for feedback (== 1/(1+2*R*g+g*g))


        // parameters:
        float fc;    // characteristic frequency
        float G;     // gain
        float B;     // bandwidth (in octaves)
        float R2Plusg; //precomputed for the tick
        float sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        float invSampleRate;
        float rToUse;
        float sampRatio; // ratio of the sample rate to 48000 (which is what the tanf table was calculated for)
        const float *table;
        float cutoffMIDI;
    } tVZFilterBell;

    // Memory handlers for `tVZFilterBell`
    void    tVZFilterBell_init                            (tVZFilterBell** const,float freq, float BW, float gain,
                                                           LEAF* const leaf);
    void    tVZFilterBell_initToPool                      (tVZFilterBell** const, float freq, float BW, float gain,
                                                           tMempool** const);
    void    tVZFilterBell_free                            (tVZFilterBell** const);

    // Tick function for `tVZFilterBell`
    float  tVZFilterBell_tick                           (tVZFilterBell* const, float input);

    // Setter functions for `tVZFilterBell`
    void    tVZFilterBell_setSampleRate                   (tVZFilterBell* const, float sampleRate);
    void    tVZFilterBell_setBandwidth                    (tVZFilterBell* const, float bandWidth);
    void    tVZFilterBell_setFreq                         (tVZFilterBell* const, float freq);
    void    tVZFilterBell_setFreqFast                     (tVZFilterBell* const vf, float cutoff);
    void    tVZFilterBell_setFreqAndGainFast              (tVZFilterBell* const, float freq, float gain);
    void    tVZFilterBell_setFrequencyAndGain             (tVZFilterBell* const, float freq, float gain);
    void    tVZFilterBell_setFrequencyAndBandwidthAndGain (tVZFilterBell* const vf, float freq, float bandwidth, float gain);
    void    tVZFilterBell_setFreqAndBWAndGainFast         (tVZFilterBell* const vf, float cutoff, float BW, float gain);
    void    tVZFilterBell_setGain                         (tVZFilterBell* const, float gain);

    typedef struct tVZFilterBR
    {
        tMempool* mempool;
        // state:
        float s1, s2;

        // filter coefficients:
        float g;          // embedded integrator gain
        float R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        float h;          // factor for feedback (== 1/(1+2*R*g+g*g))
        // parameters:
        float R2Plusg; //precomputed for the tick
        float cutoffMIDI;
        float G;
        float sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        float invSampleRate;
        float sampRatio; // ratio of the sample rate to 48000 (which is what the tanf table was calculated for)
        const float *table;
    } tVZFilterBR;

    // Memory handlers for `tVZFilterBR`
    void    tVZFilterBR_init                    (tVZFilterBR** const,float freq, float Q, LEAF* const leaf);
    void    tVZFilterBR_initToPool              (tVZFilterBR** const, float freq, float Q, tMempool** const);
    void    tVZFilterBR_free                    (tVZFilterBR** const);

    // Tick function for `tVZFilterBR`
    float  tVZFilterBR_tick                    (tVZFilterBR* const, float input);

    // Setter functions for `tVZFilterBR`
    void    tVZFilterBR_setSampleRate           (tVZFilterBR* const, float sampleRate);
    void    tVZFilterBR_setGain                 (tVZFilterBR* const, float gain);
    void    tVZFilterBR_setFreq                 (tVZFilterBR* const, float freq);
    void    tVZFilterBR_setFreqFast             (tVZFilterBR* const vf, float cutoff);
    void    tVZFilterBR_setResonance            (tVZFilterBR* const vf, float res);
    void    tVZFilterBR_setFreqAndResonanceFast (tVZFilterBR* const vf, float cutoff, float res);

    /*!
     @defgroup tdiodefilter tDiodeFilter
     @ingroup filters
     @brief Diode filter.
     @{
     
     @fn void    tDiodeFilter_init(tDiodeFilter** const, float freq, float Q, LEAF* const leaf)
     @brief Initialize a tDiodeFilter to the default mempool of a LEAF instance.
     @param filter A pointer to the tDiodeFilter to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tDiodeFilter_initToPool(tDiodeFilter** const, float freq, float Q, tMempool** const)
     @brief Initialize a tDiodeFilter to a specified mempool.
     @param filter A pointer to the tDiodeFilter to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tDiodeFilter_free(tDiodeFilter** const)
     @brief Free a tDiodeFilter from its mempool.
     @param filter A pointer to the tDiodeFilter to free.
     
     @fn float   tDiodeFilter_tick               (tDiodeFilter* const, float input)
     @brief
     @param filter A pointer to the relevant tDiodeFilter.
     
     @fn void    tDiodeFilter_setFreq     (tDiodeFilter* const vf, float cutoff)
     @brief
     @param filter A pointer to the relevant tDiodeFilter.
     
     @fn void    tDiodeFilter_setQ     (tDiodeFilter* const vf, float resonance)
     @brief
     @param filter A pointer to the relevant tDiodeFilter.
     ￼￼￼
     @} */
    
    //diode ladder filter by Ivan C, based on mystran's method
    typedef struct tDiodeFilter
    {
        tMempool* mempool;
        float cutoff;
        float f;
        float r;
        float Vt;
        float n;
        float gamma;
        float zi;
        float g0inv;
        float g1inv;
        float g2inv;
        float s0, s1, s2, s3;
        float invSampleRate;
        float sampRatio;
        const float *table;
        float cutoffMIDI;
    } tDiodeFilter;

    // Memory handlers for `tDiodeFilter`
    void    tDiodeFilter_init           (tDiodeFilter** const, float freq, float Q, LEAF* const leaf);
    void    tDiodeFilter_initToPool     (tDiodeFilter** const, float freq, float Q, tMempool** const);
    void    tDiodeFilter_free           (tDiodeFilter** const);

    // Tick functions for `tDiodeFilter`
    float  tDiodeFilter_tick           (tDiodeFilter* const, float input);
    float  tDiodeFilter_tickEfficient  (tDiodeFilter* const vf, float in);

    // Setter functions for `tDiodeFilter`
    void    tDiodeFilter_setFreq        (tDiodeFilter* const vf, float cutoff);
    void    tDiodeFilter_setFreqFast    (tDiodeFilter* const vf, float cutoff);
    void    tDiodeFilter_setQ           (tDiodeFilter* const vf, float resonance);
    void    tDiodeFilter_setSampleRate  (tDiodeFilter* const vf, float sr);
    
    
    
    //transistor ladder filter by aciddose, based on mystran's method, KVR forums
    typedef struct tLadderFilter
    {
        tMempool* mempool;
        float cutoff;
        float invSampleRate;
        float sampleRatio;
        int oversampling;
        float invOS;
        float c;
        float fb;
        float c2;
        float a;
        float s;
        float d;
        float b[4]; // stored states
        const float *table;
        float cutoffMIDI;
    } tLadderFilter;

    // Memory handlers for `tLadderFilter`
    void    tLadderFilter_init            (tLadderFilter** const, float freq, float Q, LEAF* const leaf);
    void    tLadderFilter_initToPool      (tLadderFilter** const, float freq, float Q, tMempool** const);
    void    tLadderFilter_free            (tLadderFilter** const);

    // Tick function for `tLadderFilter`
    float  tLadderFilter_tick            (tLadderFilter* const, float input);

    // Setter functions for `tLadderFilter`
    void    tLadderFilter_setFreq         (tLadderFilter* const vf, float cutoff);
    void    tLadderFilter_setFreqFast     (tLadderFilter* const vf, float cutoff);
    void    tLadderFilter_setQ            (tLadderFilter* const vf, float resonance);
    void    tLadderFilter_setSampleRate   (tLadderFilter* const vf, float sr);
    void    tLadderFilter_setOversampling (tLadderFilter* const vf, int os);


    //tilt filter
    typedef struct tTiltFilter
    {
        tMempool* mempool;
        float cutoff;
        float sr3;
        float gfactor;
        float a0;
        float b1;
        float lp_out;
        float lgain;
        float hgain;
        float invAmp;
    } tTiltFilter;

    // Memory handlers for `tTiltFilter`
    void    tTiltFilter_init          (tTiltFilter** const, float freq, LEAF* const leaf);
    void    tTiltFilter_initToPool    (tTiltFilter** const, float freq, tMempool** const);
    void    tTiltFilter_free          (tTiltFilter** const);

    // Tick function for `tTiltFilter`
    float  tTiltFilter_tick          (tTiltFilter* const, float input);

    // Setter functions for `tTiltFilter`
    void    tTiltFilter_setTilt       (tTiltFilter* const vf, float tilt);
    void    tTiltFilter_setSampleRate (tTiltFilter* const vf, float sr);


#ifdef __cplusplus
}
#endif

#endif  // LEAF_FILTERS_H_INCLUDED

//==============================================================================


