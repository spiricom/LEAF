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
     
     @fn void    tAllpass_init           (tAllpass* const, Lfloat initDelay, uint32_t maxDelay, LEAF* const leaf)
     @brief Initialize a tAllpass to the default mempool of a LEAF instance.
     @param filter A pointer to the tAllpass to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tAllpass_initToPool     (tAllpass* const, Lfloat initDelay, uint32_t maxDelay, tMempool* const)
     @brief Initialize a tAllpass to a specified mempool.
     @param filter A pointer to the tAllpass to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tAllpass_free           (tAllpass* const)
     @brief Free a tAllpass from its mempool.
     @param filter A pointer to the tAllpass to free.
     
     @fn Lfloat   tAllpass_tick           (tAllpass* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tAllpass.
     
     @fn void    tAllpass_setGain        (tAllpass* const, Lfloat gain)
     @brief
     @param filter A pointer to the relevant tAllpass.
     
     @fn void    tAllpass_setDelay       (tAllpass* const, Lfloat delay)
     @brief
     @param filter A pointer to the relevant tAllpass.
     ￼￼￼
     @} */
    
    typedef struct _tAllpass
    {
        
        tMempool mempool;
        
        Lfloat gain;
        
        tLinearDelay delay;
        
        Lfloat lastOut;
    } _tAllpass;
    
    typedef _tAllpass* tAllpass;

    // Memory handlers for `tAllpass`
    void    tAllpass_init           (tAllpass* const, Lfloat initDelay, uint32_t maxDelay, LEAF* const leaf);
    void    tAllpass_initToPool     (tAllpass* const, Lfloat initDelay, uint32_t maxDelay, tMempool* const);
    void    tAllpass_free           (tAllpass* const);

    // Tick function for `tAllpass`
    Lfloat   tAllpass_tick          (tAllpass const, Lfloat input);

    // Setter functions for `tAllpass`
    void    tAllpass_setGain        (tAllpass const, Lfloat gain);
    void    tAllpass_setDelay       (tAllpass const, Lfloat delay);
    
    //==============================================================================

    /*!
    @defgroup tallpass tAllpassSO
    @ingroup filters
    @brief Schroeder allpass. Comb-filter with feedforward and feedback.
    @{

    @fn void    tAllpassSO_init           (tAllpassSO* const, Lfloat initDelay, uint32_t maxDelay, LEAF* const leaf)
    @brief Initialize a tAllpassSO to the default mempool of a LEAF instance.
    @param filter A pointer to the tAllpassSO to initialize.
    @param leaf A pointer to the leaf instance.
 
    @fn void    tAllpassSO_initToPool     (tAllpassSO* const, Lfloat initDelay, uint32_t maxDelay, tMempool* const)
    @brief Initialize a tAllpassSO to a specified mempool.
    @param filter A pointer to the tAllpassSO to initialize.
    @param mempool A pointer to the tMempool to use.

    @fn void    tAllpassSO_free           (tAllpassSO* const)
    @brief Free a tAllpassSO from its mempool.
    @param filter A pointer to the tAllpassSO to free.

    @fn Lfloat   tAllpassSO_tick           (tAllpassSO* const, Lfloat input)
    @brief
    @param filter A pointer to the relevant tAllpassSO.
 
    @fn void    tAllpassSO_setGain        (tAllpassSO* const, Lfloat gain)
    @brief
    @param filter A pointer to the relevant tAllpassSO.

    @fn void    tAllpassSO_setDelay       (tAllpassSO* const, Lfloat delay)
    @brief
    @param filter A pointer to the relevant tAllpassSO.
    ￼￼￼
    @} */

    typedef struct _tAllpassSO
    {

        tMempool mempool;

        Lfloat prevSamp;
        Lfloat prevPrevSamp;
        Lfloat prevSamp2;
        Lfloat prevPrevSamp2;
        Lfloat a1;
        Lfloat a2;

    } _tAllpassSO;

    typedef _tAllpassSO* tAllpassSO;

    // Memory handlers for `tAllpassSO`
    void    tAllpassSO_init           (tAllpassSO* const, LEAF* const leaf);
    void    tAllpassSO_initToPool     (tAllpassSO* const, tMempool* const);
    void    tAllpassSO_free           (tAllpassSO* const);

    // Tick function for `tAllpassSO`
    Lfloat  tAllpassSO_tick           (tAllpassSO const, Lfloat input);

    // Setter functions for `tAllpassSO`
    void    tAllpassSO_setCoeff       (tAllpassSO const ft, Lfloat a1, Lfloat a2);
    //==============================================================================


    typedef struct _tThiranAllpassSOCascade
    {

        tMempool mempool;

        int numFilts;
        tAllpassSO* filters;
        Lfloat B;
        Lfloat iKey;
        Lfloat a[2];
    
        Lfloat k1[2];
        Lfloat k2[2];
        Lfloat k3[2];
        Lfloat C1[2];
        Lfloat C2[2];
        int numActiveFilters;
        int numFiltsMap[2];
        int isHigh;
        Lfloat D;
    } _tThiranAllpassSOCascade;

    typedef _tThiranAllpassSOCascade* tThiranAllpassSOCascade;

    // Memory handlers for `tThiranAllpassSOCascade`
    void    tThiranAllpassSOCascade_init           (tThiranAllpassSOCascade* const, int order, LEAF* const leaf);
    void    tThiranAllpassSOCascade_initToPool     (tThiranAllpassSOCascade* const, int order, tMempool* const);
    void    tThiranAllpassSOCascade_free           (tThiranAllpassSOCascade* const);

    // Tick function for `tThiranAllpassSOCascade`
    Lfloat  tThiranAllpassSOCascade_tick           (tThiranAllpassSOCascade const, Lfloat input);

    // Setter functions for `tThiranAllpassSOCascade`
    Lfloat  tThiranAllpassSOCascade_setCoeff       (tThiranAllpassSOCascade const ft, Lfloat dispersionCoeff, Lfloat freq, Lfloat invOversampling);
    void    tThiranAllpassSOCascade_clear          (tThiranAllpassSOCascade const ft);
    //==============================================================================

    /*!
     @defgroup tonepole tOnePole
     @ingroup filters
     @brief OnePole filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tOnePole_init           (tOnePole* const, Lfloat thePole, LEAF* const leaf)
     @brief Initialize a tOnePole to the default mempool of a LEAF instance.
     @param filter A pointer to the tOnePole to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tOnePole_initToPool     (tOnePole* const, Lfloat thePole, tMempool* const)
     @brief Initialize a tOnePole to a specified mempool.
     @param filter A pointer to the tOnePole to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tOnePole_free           (tOnePole* const)
     @brief Free a tOnePole from its mempool.
     @param filter A pointer to the tOnePole to free.
     
     @fn Lfloat   tOnePole_tick           (tOnePole* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setB0          (tOnePole* const, Lfloat b0)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setA1          (tOnePole* const, Lfloat a1)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setPole        (tOnePole* const, Lfloat thePole)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setFreq        (tOnePole* const, Lfloat freq)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setCoefficients(tOnePole* const, Lfloat b0, Lfloat a1)
     @brief
     @param filter A pointer to the relevant tOnePole.
     
     @fn void    tOnePole_setGain        (tOnePole* const, Lfloat gain)
     @brief
     @param filter A pointer to the relevant tOnePole.
     ￼￼￼
     @} */
    
    typedef struct _tOnePole
    {
        
        tMempool mempool;
        Lfloat freq;
        Lfloat gain;
        Lfloat a0,a1;
        Lfloat b0,b1;
        Lfloat lastIn, lastOut;
        Lfloat twoPiTimesInvSampleRate;
    } _tOnePole;
    
    typedef _tOnePole* tOnePole;

    // Memory handlers for `tOnePole`
    void    tOnePole_init            (tOnePole* const, Lfloat thePole, LEAF* const leaf);
    void    tOnePole_initToPool      (tOnePole* const, Lfloat thePole, tMempool* const);
    void    tOnePole_free            (tOnePole* const);

    // Tick function for `tOnePole`
    Lfloat   tOnePole_tick           (tOnePole const, Lfloat input);

    // Setter functions for `tOnePole`
    void    tOnePole_setB0           (tOnePole const, Lfloat b0);
    void    tOnePole_setA1           (tOnePole const, Lfloat a1);
    void    tOnePole_setPole         (tOnePole const, Lfloat thePole);
    void    tOnePole_setFreq         (tOnePole const, Lfloat freq);
    void    tOnePole_setCoefficients (tOnePole const, Lfloat b0, Lfloat a1);
    void    tOnePole_setGain         (tOnePole const, Lfloat gain);
    void    tOnePole_setSampleRate   (tOnePole const, Lfloat sr);
    
    //==============================================================================

    /*!
    @defgroup tonepole tOnePole
    @ingroup filters
    @brief OnePole filter, reimplemented from STK (Cook and Scavone).
    @{

    @fn void    tOnePole_init           (tOnePole* const, Lfloat thePole, LEAF* const leaf)
    @brief Initialize a tOnePole to the default mempool of a LEAF instance.
    @param filter A pointer to the tOnePole to initialize.
    @param leaf A pointer to the leaf instance.

    @fn void    tOnePole_initToPool     (tOnePole* const, Lfloat thePole, tMempool* const)
    @brief Initialize a tOnePole to a specified mempool.
    @param filter A pointer to the tOnePole to initialize.
    @param mempool A pointer to the tMempool to use.

    @fn void    tOnePole_free           (tOnePole* const)
    @brief Free a tOnePole from its mempool.
    @param filter A pointer to the tOnePole to free.

    @fn Lfloat   tOnePole_tick           (tOnePole* const, Lfloat input)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setB0          (tOnePole* const, Lfloat b0)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setA1          (tOnePole* const, Lfloat a1)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setPole        (tOnePole* const, Lfloat thePole)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setFreq        (tOnePole* const, Lfloat freq)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setCoefficients(tOnePole* const, Lfloat b0, Lfloat a1)
    @brief
    @param filter A pointer to the relevant tOnePole.

    @fn void    tOnePole_setGain        (tOnePole* const, Lfloat gain)
    @brief
    @param filter A pointer to the relevant tOnePole.
    ￼￼￼
    @} */

    typedef struct _tCookOnePole
    {

        tMempool mempool;
        Lfloat poleCoeff, sgain, output;
        Lfloat twoPiTimesInvSampleRate;
        Lfloat gain;
    } _tCookOnePole;

    typedef _tCookOnePole* tCookOnePole;

    // Memory handlers for `tCookOnePole`
    void    tCookOnePole_init           (tCookOnePole* const, LEAF* const leaf);
    void    tCookOnePole_initToPool     (tCookOnePole* const, tMempool* const);
    void    tCookOnePole_free           (tCookOnePole* const);

    // Tick function for `tCookOnePole`
    Lfloat  tCookOnePole_tick           (tCookOnePole const, Lfloat input);

    // Setter functions for `tCookOnePole`
    void    tCookOnePole_setPole        (tCookOnePole const, Lfloat thePole);
    void    tCookOnePole_setGain        (tCookOnePole const, Lfloat gain);
    void    tCookOnePole_setGainAndPole (tCookOnePole const ft, Lfloat gain, Lfloat pole);
    void    tCookOnePole_setSampleRate  (tCookOnePole const, Lfloat sr);

    //==============================================================================

    /*!
     @defgroup ttwopole tTwoPole
     @ingroup filters
     @brief TwoPole filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tTwoPole_init           (tTwoPole* const, LEAF* const leaf)
     @brief Initialize a tTwoPole to the default mempool of a LEAF instance.
     @param filter A pointer to the tTwoPole to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTwoPole_initToPool     (tTwoPole* const, tMempool* const)
     @brief Initialize a tTwoPole to a specified mempool.
     @param filter A pointer to the tTwoPole to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTwoPole_free           (tTwoPole* const)
     @brief Free a tTwoPole from its mempool.
     @param filter A pointer to the tTwoPole to free.
     
     @fn Lfloat   tTwoPole_tick           (tTwoPole* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setB0          (tTwoPole* const, Lfloat b0)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setA1          (tTwoPole* const, Lfloat a1)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setA2          (tTwoPole* const, Lfloat a2)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setResonance   (tTwoPole* const, Lfloat freq, Lfloat radius, int normalize)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setCoefficients(tTwoPole* const, Lfloat b0, Lfloat a1, Lfloat a2)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     
     @fn void    tTwoPole_setGain        (tTwoPole* const, Lfloat gain)
     @brief
     @param filter A pointer to the relevant tTwoPole.
     ￼￼￼
     @} */

    typedef struct _tTwoPole
    {
        
        tMempool mempool;
        
        Lfloat gain;
        Lfloat a0, a1, a2;
        Lfloat b0;
        
        Lfloat radius, frequency;
        int normalize;
        
        Lfloat lastOut[2];
        
        Lfloat sampleRate;
        Lfloat twoPiTimesInvSampleRate;
    } _tTwoPole;
    
    typedef _tTwoPole* tTwoPole;

    // Memory handlers for `tTwoPole`
    void    tTwoPole_init            (tTwoPole* const, LEAF* const leaf);
    void    tTwoPole_initToPool      (tTwoPole* const, tMempool* const);
    void    tTwoPole_free            (tTwoPole* const);

    // Tick function for `tTwoPole`
    Lfloat  tTwoPole_tick            (tTwoPole const, Lfloat input);

    // Setter functions for `tTwoPole`
    void    tTwoPole_setB0           (tTwoPole const, Lfloat b0);
    void    tTwoPole_setA1           (tTwoPole const, Lfloat a1);
    void    tTwoPole_setA2           (tTwoPole const, Lfloat a2);
    void    tTwoPole_setResonance    (tTwoPole const, Lfloat freq, Lfloat radius, int normalize);
    void    tTwoPole_setCoefficients (tTwoPole const, Lfloat b0, Lfloat a1, Lfloat a2);
    void    tTwoPole_setGain         (tTwoPole const, Lfloat gain);
    void    tTwoPole_setSampleRate   (tTwoPole const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tonezero tOneZero
     @ingroup filters
     @brief OneZero filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tOneZero_init           (tOneZero* const, Lfloat theZero, LEAF* const leaf)
     @brief Initialize a tOneZero to the default mempool of a LEAF instance.
     @param filter A pointer to the tSlide to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tOneZero_initToPool     (tOneZero* const, Lfloat theZero, tMempool* const)
     @brief Initialize a tOneZero to a specified mempool.
     @param filter A pointer to the tOneZero to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tOneZero_free           (tOneZero* const)
     @brief Free a tOneZero from its mempool.
     @param filter A pointer to the tOneZero to free.
     
     @fn Lfloat   tOneZero_tick           (tOneZero* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setB0          (tOneZero* const, Lfloat b0)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setB1          (tOneZero* const, Lfloat b1)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setZero        (tOneZero* const, Lfloat theZero)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setCoefficients(tOneZero* const, Lfloat b0, Lfloat b1)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn void    tOneZero_setGain        (tOneZero* const, Lfloat gain)
     @brief
     @param filter A pointer to the relevant tOneZero.
     
     @fn Lfloat   tOneZero_getPhaseDelay  (tOneZero *f, Lfloat frequency)
     @brief
     @param filter A pointer to the relevant tOneZero.
     ￼￼￼
     @} */
    
    typedef struct _tOneZero
    {
        tMempool mempool;
        Lfloat gain;
        Lfloat b0,b1;
        Lfloat lastIn, lastOut, frequency;
        Lfloat invSampleRate;
    } _tOneZero;
    
    typedef _tOneZero* tOneZero;

    // Memory handlers for `tOneZero`
    void    tOneZero_init           (tOneZero* const, Lfloat theZero, LEAF* const leaf);
    void    tOneZero_initToPool     (tOneZero* const, Lfloat theZero, tMempool* const);
    void    tOneZero_free           (tOneZero* const);

    // Tick function for `tOneZero`
    Lfloat  tOneZero_tick            (tOneZero const, Lfloat input);

    // Setter functions for `tOneZero`
    void    tOneZero_setB0           (tOneZero const, Lfloat b0);
    void    tOneZero_setB1           (tOneZero const, Lfloat b1);
    void    tOneZero_setZero         (tOneZero const, Lfloat theZero);
    void    tOneZero_setCoefficients (tOneZero const, Lfloat b0, Lfloat b1);
    void    tOneZero_setGain         (tOneZero const, Lfloat gain);
    Lfloat  tOneZero_getPhaseDelay   (tOneZero const, Lfloat frequency);
    void    tOneZero_setSampleRate   (tOneZero const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup ttwozero tTwoZero
     @ingroup filters
     @brief TwoZero filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tTwoZero_init           (tTwoZero* const, LEAF* const leaf)
     @brief Initialize a tTwoZero to the default mempool of a LEAF instance.
     @param filter A pointer to the tTwoZero to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tTwoZero_initToPool     (tTwoZero* const, tMempool* const)
     @brief Initialize a tTwoZero to a specified mempool.
     @param filter A pointer to the tTwoZero to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tTwoZero_free           (tTwoZero* const)
     @brief Free a tTwoZero from its mempool.
     @param filter A pointer to the tTwoZero to free.
     
     @fn Lfloat   tTwoZero_tick           (tTwoZero* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setB0          (tTwoZero* const, Lfloat b0)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setB1          (tTwoZero* const, Lfloat b1)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setB2          (tTwoZero* const, Lfloat b2)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setNotch       (tTwoZero* const, Lfloat frequency, Lfloat radius)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setCoefficients(tTwoZero* const, Lfloat b0, Lfloat b1, Lfloat b2)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     @fn void    tTwoZero_setGain        (tTwoZero* const, Lfloat gain)
     @brief
     @param filter A pointer to the relevant tTwoZero.
     
     ￼￼￼
     @} */

    typedef struct _tTwoZero
    {
        tMempool mempool;
        
        Lfloat gain;
        Lfloat b0, b1, b2;
        Lfloat frequency, radius;
        Lfloat lastIn[2];
        Lfloat twoPiTimesInvSampleRate;
    } _tTwoZero;
    
    typedef _tTwoZero* tTwoZero;

    // Memory handlers for `tTwoZero`
    void    tTwoZero_init           (tTwoZero* const, LEAF* const leaf);
    void    tTwoZero_initToPool     (tTwoZero* const, tMempool* const);
    void    tTwoZero_free           (tTwoZero* const);

    // Tick function for `tTwoZero`
    Lfloat  tTwoZero_tick           (tTwoZero const, Lfloat input);

    // Setter functions for `tTwoZero`
    void    tTwoZero_setB0          (tTwoZero const, Lfloat b0);
    void    tTwoZero_setB1          (tTwoZero const, Lfloat b1);
    void    tTwoZero_setB2          (tTwoZero const, Lfloat b2);
    void    tTwoZero_setNotch       (tTwoZero const, Lfloat frequency, Lfloat radius);
    void    tTwoZero_setCoefficients(tTwoZero const, Lfloat b0, Lfloat b1, Lfloat b2);
    void    tTwoZero_setGain        (tTwoZero const, Lfloat gain);
    void    tTwoZero_setSampleRate  (tTwoZero const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tpolezero tPoleZero
     @ingroup filters
     @brief PoleZero filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tPoleZero_init              (tPoleZero* const, LEAF* const leaf)
     @brief Initialize a tPoleZero to the default mempool of a LEAF instance.
     @param filter A pointer to the tPoleZero to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPoleZero_initToPool        (tPoleZero* const, tMempool* const)
     @brief Initialize a tPoleZero to a specified mempool.
     @param filter A pointer to the tPoleZero to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPoleZero_free              (tPoleZero* const)
     @brief Free a tPoleZero from its mempool.
     @param filter A pointer to the tPoleZero to free.
     
     
     @fn Lfloat   tPoleZero_tick              (tPoleZero* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setB0             (tPoleZero* const, Lfloat b0)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setB1             (tPoleZero* const, Lfloat b1)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setA1             (tPoleZero* const, Lfloat a1)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setCoefficients   (tPoleZero* const, Lfloat b0, Lfloat b1, Lfloat a1)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setAllpass        (tPoleZero* const, Lfloat coeff)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setBlockZero      (tPoleZero* const, Lfloat thePole)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     @fn void    tPoleZero_setGain           (tPoleZero* const, Lfloat gain)
     @brief
     @param filter A pointer to the relevant tPoleZero.
     
     ￼￼￼
     @} */
    
    typedef struct _tPoleZero
    {
        
        tMempool mempool;
        
        Lfloat gain;
        Lfloat a1;
        Lfloat b0,b1;
        
        Lfloat lastIn, lastOut;
    } _tPoleZero;
    
    typedef _tPoleZero* tPoleZero;

    // Memory handlers for `tPoleZero`
    void    tPoleZero_init              (tPoleZero* const, LEAF* const leaf);
    void    tPoleZero_initToPool        (tPoleZero* const, tMempool* const);
    void    tPoleZero_free              (tPoleZero* const);

    // Tick function for `tPoleZero`
    Lfloat  tPoleZero_tick              (tPoleZero const, Lfloat input);

    // Setter functions for `tPoleZero`
    void    tPoleZero_setB0             (tPoleZero const, Lfloat b0);
    void    tPoleZero_setB1             (tPoleZero const, Lfloat b1);
    void    tPoleZero_setA1             (tPoleZero const, Lfloat a1);
    void    tPoleZero_setCoefficients   (tPoleZero const, Lfloat b0, Lfloat b1, Lfloat a1);
    void    tPoleZero_setAllpass        (tPoleZero const, Lfloat coeff);
    void    tPoleZero_setBlockZero      (tPoleZero const, Lfloat thePole);
    void    tPoleZero_setGain           (tPoleZero const, Lfloat gain);
    
    //==============================================================================
    
    /*!
     @defgroup tbiquad tBiQuad
     @ingroup filters
     @brief BiQuad filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tBiQuad_init           (tBiQuad* const, LEAF* const leaf)
     @brief Initialize a tBiQuad to the default mempool of a LEAF instance.
     @param filter A pointer to the tBiQuad to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tBiQuad_initToPool     (tBiQuad* const, tMempool* const)
     @brief Initialize a tBiQuad to a specified mempool.
     @param filter A pointer to the tBiQuad to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tBiQuad_free           (tBiQuad* const)
     @brief Free a tBiQuad from its mempool.
     @param filter A pointer to the tBiQuad to free.
     
     
     @fn Lfloat   tBiQuad_tick           (tBiQuad* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setB0          (tBiQuad* const, Lfloat b0)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setB1          (tBiQuad* const, Lfloat b1)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setB2          (tBiQuad* const, Lfloat b2)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setA1          (tBiQuad* const, Lfloat a1)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setA2          (tBiQuad* const, Lfloat a2)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setNotch       (tBiQuad* const, Lfloat freq, Lfloat radius)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setResonance   (tBiQuad* const, Lfloat freq, Lfloat radius, int normalize)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setCoefficients(tBiQuad* const, Lfloat b0, Lfloat b1, Lfloat b2, Lfloat a1, Lfloat a2)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     @fn void    tBiQuad_setGain        (tBiQuad* const, Lfloat gain)
     @brief
     @param filter A pointer to the relevant tBiQuad.
     
     ￼￼￼
     @} */
    
    typedef struct _tBiQuad
    {
        tMempool mempool;
        
        Lfloat gain;
        Lfloat a0, a1, a2;
        Lfloat b0, b1, b2;
        
        Lfloat lastIn[2];
        Lfloat lastOut[2];
        
        Lfloat frequency, radius;
        int normalize;
        
        Lfloat sampleRate;
        Lfloat twoPiTimesInvSampleRate;
    } _tBiQuad;
    
    typedef _tBiQuad* tBiQuad;

    // Memory handlers for `tBiQuad`
    void    tBiQuad_init           (tBiQuad* const, LEAF* const leaf);
    void    tBiQuad_initToPool     (tBiQuad* const, tMempool* const);
    void    tBiQuad_free           (tBiQuad* const);

    // Tick function for `tBiQuad`
    Lfloat  tBiQuad_tick           (tBiQuad const, Lfloat input);

    // Setter functions for `tBiQuad`
    void    tBiQuad_setB0          (tBiQuad const, Lfloat b0);
    void    tBiQuad_setB1          (tBiQuad const, Lfloat b1);
    void    tBiQuad_setB2          (tBiQuad const, Lfloat b2);
    void    tBiQuad_setA1          (tBiQuad const, Lfloat a1);
    void    tBiQuad_setA2          (tBiQuad const, Lfloat a2);
    void    tBiQuad_setNotch       (tBiQuad const, Lfloat freq, Lfloat radius);
    void    tBiQuad_setResonance   (tBiQuad const, Lfloat freq, Lfloat radius, int normalize);
    void    tBiQuad_setCoefficients(tBiQuad const, Lfloat b0, Lfloat b1, Lfloat b2, Lfloat a1, Lfloat a2);
    void    tBiQuad_setGain        (tBiQuad const, Lfloat gain);
    void    tBiQuad_setSampleRate  (tBiQuad const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tsvf tSVF
     @ingroup filters
     @brief State Variable Filter, algorithm from Andy Simper.
     @{
     
     @fn void    tSVF_init           (tSVF* const, SVFType type, Lfloat freq, Lfloat Q, LEAF* const leaf)
     @brief Initialize a tSVF to the default mempool of a LEAF instance.
     @param filter A pointer to the tSVF to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSVF_initToPool     (tSVF* const, SVFType type, Lfloat freq, Lfloat Q, tMempool* const)
     @brief Initialize a tSVF to a specified mempool.
     @param filter A pointer to the tSVF to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSVF_free           (tSVF* const)
     @brief Free a tSVF from its mempool.
     @param filter A pointer to the tSVF to free.
     
     @fn Lfloat   tSVF_tick           (tSVF* const, Lfloat v0)
     @brief
     @param filter A pointer to the relevant tSVF.
     
     @fn void    tSVF_setFreq        (tSVF* const, Lfloat freq)
     @brief
     @param filter A pointer to the relevant tSVF.
     
     @fn void    tSVF_setQ           (tSVF* const, Lfloat Q)
     @brief
     @param filter A pointer to the relevant tSVF.
     
     @fn void    tSVF_setFreqAndQ    (tSVF* const svff, Lfloat freq, Lfloat Q)
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
    
    typedef struct _tSVF
    {
        tMempool mempool;
        SVFType type;
        Lfloat cutoff, Q, cutoffMIDI;
        Lfloat ic1eq,ic2eq;
        Lfloat g,k,a1,a2,a3,cH,cB,cL,cBK;
        Lfloat sampleRate;
        Lfloat invSampleRate;
        Lfloat phaseComp;
        Lfloat sampleRatio;
        const Lfloat *table;
    } _tSVF;
    
    typedef _tSVF* tSVF;

    // Memory handlers for `tSVF`
    void    tSVF_init                (tSVF* const, SVFType type, Lfloat freq, Lfloat Q, LEAF* const leaf);
    void    tSVF_initToPool          (tSVF* const, SVFType type, Lfloat freq, Lfloat Q, tMempool* const);
    void    tSVF_free                (tSVF* const);

    // Tick functions for `tSVF`
    Lfloat  tSVF_tick                (tSVF const, Lfloat v0);
    Lfloat  tSVF_tickLP              (tSVF const, Lfloat v0);
    Lfloat  tSVF_tickHP              (tSVF const, Lfloat v0);
    Lfloat  tSVF_tickBP              (tSVF const, Lfloat v0);

    // Setter functions for `tSVF`
    void    tSVF_setFreq             (tSVF const, Lfloat freq);
    void    tSVF_setFreqFast         (tSVF const vf, Lfloat cutoff);
    void    tSVF_setQ                (tSVF const, Lfloat Q);
    void    tSVF_setFreqAndQ         (tSVF const svff, Lfloat freq, Lfloat Q);
    void    tSVF_setFreqAndQFast     (tSVF const svff, Lfloat cutoff, Lfloat Q);
    void    tSVF_setFilterType       (tSVF const svff, SVFType type);
    void    tSVF_setSampleRate       (tSVF const svff, Lfloat sr);
    Lfloat  tSVF_getPhaseAtFrequency (tSVF const svff, Lfloat freq);
    
    //==============================================================================

    typedef struct _tSVF_LP
    {
        tMempool mempool;
        Lfloat ic1eq,ic2eq;
        Lfloat g,onePlusg,k,a0,a1,a2,a3,a4,a5;
        Lfloat sampleRate;
        Lfloat invSampleRate;
        Lfloat phaseComp;
        Lfloat sampleRatio;
        uint32_t nan;
        const Lfloat *table;
    } _tSVF_LP;

    typedef _tSVF_LP* tSVF_LP;

    // Memory handlers for `tSVF_LP`
    void    tSVF_LP_init                (tSVF_LP* const, Lfloat freq, Lfloat Q, LEAF* const leaf);
    void    tSVF_LP_initToPool          (tSVF_LP* const, Lfloat freq, Lfloat Q, tMempool* const);
    void    tSVF_LP_free                (tSVF_LP* const);

    // Tick function for `tSVF_LP`
    Lfloat  tSVF_LP_tick                (tSVF_LP const, Lfloat v0);

    // Setter functions for `tSVF_LP`
    void    tSVF_LP_setFreq             (tSVF_LP const, Lfloat freq);
    void    tSVF_LP_setFreqFast         (tSVF_LP const vf, Lfloat cutoff);
    void    tSVF_LP_setQ                (tSVF_LP const, Lfloat Q);
    void    tSVF_LP_setFreqAndQ         (tSVF_LP const svff, Lfloat freq, Lfloat Q);
    void    tSVF_LP_setFreqAndQFast     (tSVF_LP const svff, Lfloat cutoff, Lfloat Q);
    void    tSVF_LP_setSampleRate       (tSVF_LP const svff, Lfloat sr);
    Lfloat  tSVF_LP_getPhaseAtFrequency (tSVF_LP const svff, Lfloat freq);
    //==============================================================================
    
    /*!
     @defgroup tefficientsvf tEfficientSVF
     @ingroup filters
     @brief Efficient State Variable Filter for 14-bit control input, [0, 4096).
     @{
     
     @fn void    tEfficientSVF_init          (tEfficientSVF* const, SVFType type, uint16_t input, Lfloat Q, LEAF* const leaf)
     @brief Initialize a tEfficientSVF to the default mempool of a LEAF instance.
     @param filter A pointer to the tEfficientSVF to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tEfficientSVF_initToPool    (tEfficientSVF* const, SVFType type, uint16_t input, Lfloat Q, tMempool* const)
     @brief Initialize a tEfficientSVF to a specified mempool.
     @param filter A pointer to the tEfficientSVF to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tEfficientSVF_free          (tEfficientSVF* const)
     @brief Free a tEfficientSVF from its mempool.
     @param filter A pointer to the tEfficientSVF to free.
     
     @fn Lfloat   tEfficientSVF_tick          (tEfficientSVF* const, Lfloat v0)
     @brief
     @param filter A pointer to the relevant tEfficientSVF.
     
     @fn void    tEfficientSVF_setFreq       (tEfficientSVF* const, uint16_t controlFreq)
     @brief
     @param filter A pointer to the relevant tEfficientSVF.
     
     @fn void    tEfficientSVF_setQ          (tEfficientSVF* const, Lfloat Q)
     @brief
     @param filter A pointer to the relevant tEfficientSVF.
     ￼￼￼
     @} */
    
    typedef struct _tEfficientSVF
    {
        
        tMempool mempool;
        SVFType type;
        Lfloat cutoff, Q;
        Lfloat ic1eq,ic2eq;
        Lfloat g,k,a1,a2,a3;
        const Lfloat *table;
    } _tEfficientSVF;
    
    typedef _tEfficientSVF* tEfficientSVF;

    // Memory handlers for `tEfficientSVF`
    void    tEfficientSVF_init          (tEfficientSVF* const, SVFType type, uint16_t input, Lfloat Q, LEAF* const leaf);
    void    tEfficientSVF_initToPool    (tEfficientSVF* const, SVFType type, uint16_t input, Lfloat Q, tMempool* const);
    void    tEfficientSVF_free          (tEfficientSVF* const);

    // Tick function for `tEfficientSVF`
    Lfloat  tEfficientSVF_tick          (tEfficientSVF const, Lfloat v0);

    // Setter functions for `tEfficientSVF`
    void    tEfficientSVF_setFreq       (tEfficientSVF const svff, Lfloat cutoff);
    void    tEfficientSVF_setQ          (tEfficientSVF const, Lfloat Q);
    void    tEfficientSVF_setFreqAndQ   (tEfficientSVF const, uint16_t controlFreq, Lfloat Q);
    void    tEfficientSVF_setSampleRate (tEfficientSVF const, Lfloat sampleRate);
    //==============================================================================
    
    /*!
     @defgroup thighpass tHighpass
     @ingroup filters
     @brief Simple Highpass filter.
     @{
     
     @fn void    tHighpass_init          (tHighpass* const, Lfloat freq, LEAF* const leaf)
     @brief Initialize a tHighpass to the default mempool of a LEAF instance.
     @param filter A pointer to the tHighpass to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tHighpass_initToPool    (tHighpass* const, Lfloat freq, tMempool* const)
     @brief Initialize a tHighpass to a specified mempool.
     @param filter A pointer to the tHighpass to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tHighpass_free          (tHighpass* const)
     @brief Free a tHighpass from its mempool.
     @param filter A pointer to the tHighpass to free.
     
     @fn Lfloat   tHighpass_tick          (tHighpass* const, Lfloat x)
     @brief
     @param filter A pointer to the relevant tHighpass.
     
     @fn void    tHighpass_setFreq       (tHighpass* const, Lfloat freq)
     @brief
     @param filter A pointer to the relevant tHighpass.
     
     @fn Lfloat   tHighpass_getFreq       (tHighpass* const)
     @brief
     @param filter A pointer to the relevant tHighpass.
     ￼￼￼
     @} */
    
    typedef struct _tHighpass
    {
        tMempool mempool;
        Lfloat xs, ys, R;
        Lfloat frequency;
        Lfloat twoPiTimesInvSampleRate;
    } _tHighpass;
    
    typedef _tHighpass* tHighpass;

    // Memory handlers for `tHighpass`
    void    tHighpass_init          (tHighpass* const, Lfloat freq, LEAF* const leaf);
    void    tHighpass_initToPool    (tHighpass* const, Lfloat freq, tMempool* const);
    void    tHighpass_free          (tHighpass* const);

    // Tick function for `tHighpass`
    Lfloat  tHighpass_tick          (tHighpass const, Lfloat x);

    // Setter functions for `tHighpass`
    void    tHighpass_setFreq       (tHighpass const, Lfloat freq);
    Lfloat  tHighpass_getFreq       (tHighpass const);
    void    tHighpass_setSampleRate (tHighpass const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tbutterworth tButterworth
     @ingroup filters
     @brief Butterworth filter.
     @{
     
     @fn void    tButterworth_init           (tButterworth* const, int N, Lfloat f1, Lfloat f2, LEAF* const leaf, LEAF* const leaf)
     @brief Initialize a tButterworth to the default mempool of a LEAF instance.
     @param filter A pointer to the tButterworth to initialize.
     @param leaf A pointer to the leaf instance.
     @param order Order of the filter.
     @param lowCutoff Lower cutoff frequency.
     @param upperCutoff Upper cutoff frequency.
     
     @fn void    tButterworth_initToPool     (tButterworth* const, int N, Lfloat f1, Lfloat f2, tMempool* const)
     @brief Initialize a tButterworth to a specified mempool.
     @param filter A pointer to the tButterworth to initialize.
     @param mempool A pointer to the tMempool to use.
     @param order Order of the filter.
     @param lowCutoff Lower cutoff frequency.
     @param upperCutoff Upper cutoff frequency.
     
     @fn void    tButterworth_free           (tButterworth* const)
     @brief Free a tButterworth from its mempool.
     @param filter A pointer to the tButterworth to free.
     
     @fn Lfloat   tButterworth_tick           (tButterworth* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tButterworth.
     
     @fn void    tButterworth_setF1          (tButterworth* const, Lfloat in)
     @brief
     @param filter A pointer to the relevant tButterworth.
     
     @fn void    tButterworth_setF2          (tButterworth* const, Lfloat in)
     @brief
     @param filter A pointer to the relevant tButterworth.
     
     @fn void    tButterworth_setFreqs       (tButterworth* const, Lfloat f1, Lfloat f2)
     @brief
     @param filter A pointer to the relevant tButterworth.
     ￼￼￼
     @} */
    
#define NUM_SVF_BW 16
    typedef struct _tButterworth
    {
        tMempool mempool;
        
        Lfloat gain;
        int order;
        int numSVF;
        
        tSVF* svf;
        
        Lfloat f1,f2;
    } _tButterworth;
    
    typedef _tButterworth* tButterworth;

    // Memory handlers for `tButterworth`
    void    tButterworth_init           (tButterworth* const, int N, Lfloat f1, Lfloat f2, LEAF* const leaf);
    void    tButterworth_initToPool     (tButterworth* const, int N, Lfloat f1, Lfloat f2, tMempool* const);
    void    tButterworth_free           (tButterworth* const);

    // Tick function for `tButterworth`
    Lfloat  tButterworth_tick           (tButterworth const, Lfloat input);

    // Setter functions for `tButterworth`
    void    tButterworth_setF1          (tButterworth const, Lfloat in);
    void    tButterworth_setF2          (tButterworth const, Lfloat in);
    void    tButterworth_setFreqs       (tButterworth const, Lfloat f1, Lfloat f2);
    void    tButterworth_setSampleRate  (tButterworth const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tfir tFIR
     @ingroup filters
     @brief Finite impulse response filter.
     @{
     
     @fn void    tFIR_init           (tFIR* const, Lfloat* coeffs, int numTaps, LEAF* const leaf)
     @brief Initialize a tFIR to the default mempool of a LEAF instance.
     @param filter A pointer to the tFIR to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tFIR_initToPool     (tFIR* const, Lfloat* coeffs, int numTaps, tMempool* const)
     @brief Initialize a tFIR to a specified mempool.
     @param filter A pointer to the tFIR to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tFIR_free           (tFIR* const)
     @brief Free a tFIR from its mempool.
     @param filter A pointer to the tFIR to free.
     
     @fn Lfloat   tFIR_tick           (tFIR* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tFIR.
     ￼￼￼
     @} */
    
    typedef struct _tFIR
    {
        
        tMempool mempool;
        Lfloat* past;
        Lfloat* coeff;
        int numTaps;
    } _tFIR;
    
    typedef _tFIR* tFIR;

    // Memory handlers for `tFIR`
    void    tFIR_init           (tFIR* const, Lfloat* coeffs, int numTaps, LEAF* const leaf);
    void    tFIR_initToPool     (tFIR* const, Lfloat* coeffs, int numTaps, tMempool* const);
    void    tFIR_free           (tFIR* const);

    // Tick function for `tFIR`
    Lfloat  tFIR_tick           (tFIR const, Lfloat input);
    
    
    //==============================================================================
    
    /*!
     @defgroup tmedianfilter tMedianFilter
     @ingroup filters
     @brief Median filter.
     @{
     
     @fn void    tMedianFilter_init           (tMedianFilter* const, int size, LEAF* const leaf)
     @brief Initialize a tMedianFilter to the default mempool of a LEAF instance.
     @param filter A pointer to the tMedianFilter to initialize.
     @param leaf A pointer to the leaf instance.f
     
     @fn void    tMedianFilter_initToPool     (tMedianFilter* const, int size, tMempool* const)
     @brief Initialize a tMedianFilter to a specified mempool.
     @param filter A pointer to the tMedianFilter to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tMedianFilter_free           (tMedianFilter* const)
     @brief Free a tMedianFilter from its mempool.
     @param filter A pointer to the tMedianFilter to free.
     
     @fn Lfloat   tMedianFilter_tick           (tMedianFilter* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tMedianFilter.
     ￼￼￼
     @} */
    
    typedef struct _tMedianFilter
    {
        
        tMempool mempool;
        Lfloat* val;
        int* age;
        int m;
        int size;
        int middlePosition;
        int last;
        int pos;
    } _tMedianFilter;
    
    typedef _tMedianFilter* tMedianFilter;

    // Memory handlers for `tMedianFilter`
    void    tMedianFilter_init           (tMedianFilter* const, int size, LEAF* const leaf);
    void    tMedianFilter_initToPool     (tMedianFilter* const, int size, tMempool* const);
    void    tMedianFilter_free           (tMedianFilter* const);

    // Tick function for `tMedianFilter`
    Lfloat  tMedianFilter_tick           (tMedianFilter const, Lfloat input);
    
    
    /*!
     @defgroup tvzfilter tVZFilter
     @ingroup filters
     @brief Vadim Zavalishin style from VA book (from implementation in RSlib posted to kvr forum)
     @{
     
     @fn void    tVZFilter_init           (tVZFilter* const, VZFilterType type, Lfloat freq, Lfloat Q, LEAF* const leaf)
     @brief Initialize a tVZFilter to the default mempool of a LEAF instance.
     @param filter A pointer to the tVZFilter to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tVZFilter_initToPool     (tVZFilter* const, VZFilterType type, Lfloat freq, Lfloat Q, tMempool* const)
     @brief Initialize a tVZFilter to a specified mempool.
     @param filter A pointer to the tVZFilter to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tVZFilter_free           (tVZFilter* const)
     @brief Free a tVZFilter from its mempool.
     @param filter A pointer to the tVZFilter to free.
     
     @fn Lfloat   tVZFilter_tick               (tVZFilter* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn Lfloat   tVZFilter_tickEfficient               (tVZFilter* const vf, Lfloat in)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_calcCoeffs           (tVZFilter* const)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setBandwidth            (tVZFilter* const, Lfloat bandWidth)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setFreq           (tVZFilter* const, Lfloat freq)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setFreqAndBandwidth    (tVZFilter* const vf, Lfloat freq, Lfloat bw)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setGain                  (tVZFilter* const, Lfloat gain)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setType                  (tVZFilter* const, VZFilterType type)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn Lfloat   tVZFilter_BandwidthToR        (tVZFilter* const vf, Lfloat B)
     @brief
     @param filter A pointer to the relevant tVZFilter.
     
     @fn void    tVZFilter_setSampleRate  (tVZFilter* const, Lfloat sampleRate)
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
    
    
    typedef struct _tVZFilter
    {
        tMempool mempool;
        
        VZFilterType type;
        // state:
        Lfloat s1, s2;
        
        // filter coefficients:
        Lfloat g;          // embedded integrator gain
        Lfloat R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        Lfloat h;          // factor for feedback (== 1/(1+2*R*g+g*g))
        Lfloat cL, cB, cH; // coefficients for low-, band-, and highpass signals
        
        // parameters:
        Lfloat fc;    // characteristic frequency
        Lfloat G;     // gain
        Lfloat invG;        //1/gain
        Lfloat Q; //q of filter
        Lfloat B;     // bandwidth (in octaves)
        Lfloat m;     // morph parameter (0...1)
        Lfloat R2Plusg; //precomputed for the tick
        Lfloat sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        Lfloat invSampleRate;
        Lfloat sampRatio;
        Lfloat cutoffMIDI;
        const Lfloat *table;
    } _tVZFilter;
    
    typedef _tVZFilter* tVZFilter;

    // Memory handlers for `tVZFilter`
    void    tVZFilter_init                                (tVZFilter* const, VZFilterType type, Lfloat freq, Lfloat Q,
                                                           LEAF* const leaf);
    void    tVZFilter_initToPool                          (tVZFilter* const, VZFilterType type, Lfloat freq, Lfloat Q,
                                                           tMempool* const);
    void    tVZFilter_free                                (tVZFilter* const);

    // Tick functions for `tVZFilter`
    Lfloat  tVZFilter_tick                                (tVZFilter const, Lfloat input);
    Lfloat  tVZFilter_tickEfficient                       (tVZFilter const vf, Lfloat in);

    // Setter functions for `tVZFilter`
    void    tVZFilter_setSampleRate                       (tVZFilter const, Lfloat sampleRate);
    void    tVZFilter_calcCoeffs                          (tVZFilter const);
    void    tVZFilter_calcCoeffsEfficientBP               (tVZFilter const);
    void    tVZFilter_setBandwidth                        (tVZFilter const, Lfloat bandWidth);
    void    tVZFilter_setFreq                             (tVZFilter const, Lfloat freq);
    void    tVZFilter_setFreqFast                         (tVZFilter const vf, Lfloat cutoff);
    void    tVZFilter_setFreqAndBandwidth                 (tVZFilter const vf, Lfloat freq, Lfloat bw);
    void    tVZFilter_setFreqAndBandwidthEfficientBP      (tVZFilter const vf, Lfloat freq, Lfloat bw);
    void    tVZFilter_setGain                             (tVZFilter const, Lfloat gain);
    void    tVZFilter_setResonance                        (tVZFilter const vf, Lfloat res);
    void    tVZFilter_setFrequencyAndResonance            (tVZFilter const vf, Lfloat freq, Lfloat res);
    void    tVZFilter_setFrequencyAndResonanceAndGain     (tVZFilter const vf, Lfloat freq, Lfloat res, Lfloat gains);
    void    tVZFilter_setFastFrequencyAndResonanceAndGain (tVZFilter const vf, Lfloat freq, Lfloat res, Lfloat gain);
    void    tVZFilter_setFrequencyAndBandwidthAndGain     (tVZFilter const vf, Lfloat freq, Lfloat BW, Lfloat gain);
    void    tVZFilter_setFrequencyAndResonanceAndMorph    (tVZFilter const vf, Lfloat freq, Lfloat res, Lfloat morph);
    void    tVZFilter_setMorphOnly                        (tVZFilter const vf, Lfloat morph);
    void    tVZFilter_setMorph                            (tVZFilter const vf, Lfloat morph);
    void    tVZFilter_setType                             (tVZFilter const, VZFilterType type);
    Lfloat  tVZFilter_BandwidthToR                        (tVZFilter const vf, Lfloat B);
    Lfloat  tVZFilter_BandwidthToREfficientBP             (tVZFilter const vf, Lfloat B);
    

    typedef struct _tVZFilterLS
    {
        tMempool mempool;
        // state:
        Lfloat s1, s2;

        // filter coefficients:
        Lfloat g;          // embedded integrator gain
        Lfloat R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        Lfloat h;          // factor for feedback (== 1/(1+2*R*g+g*g))

        // parameters:
        Lfloat fc;    // characteristic frequency
        Lfloat G;     // gain
        Lfloat Q; //q of filter
        Lfloat B;     // bandwidth (in octaves)
        Lfloat R2Plusg; //precomputed for the tick
        Lfloat sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        Lfloat invSampleRate;
        Lfloat gPreDiv;
        Lfloat invSqrtA;
        Lfloat sampRatio; // ratio of the sample rate to 48000 (which is what the tanf table was calculated for)
        const Lfloat *table;
        Lfloat cutoffMIDI;
    } _tVZFilterLS;

    typedef _tVZFilterLS* tVZFilterLS;

    // Memory handlers for `tVZFilterLS`
    void    tVZFilterLS_init                           (tVZFilterLS* const,Lfloat freq, Lfloat Q, Lfloat gain,
                                                        LEAF* const leaf);
    void    tVZFilterLS_initToPool                     (tVZFilterLS* const, Lfloat freq, Lfloat Q, Lfloat gain,
                                                        tMempool* const);
    void    tVZFilterLS_free                           (tVZFilterLS* const);

    // Tick function for `tVZFilterLS`
    Lfloat  tVZFilterLS_tick                           (tVZFilterLS const, Lfloat input);

    // Setter functions for `tVZFilterLS`
    void    tVZFilterLS_setSampleRate                  (tVZFilterLS const, Lfloat sampleRate);
    void    tVZFilterLS_setBandwidthSlow               (tVZFilterLS const, Lfloat bandWidth);
    void    tVZFilterLS_setFreq                        (tVZFilterLS const, Lfloat freq);
    void    tVZFilterLS_setFreqFast                    (tVZFilterLS const vf, Lfloat cutoff);
    void    tVZFilterLS_setGain                        (tVZFilterLS const, Lfloat gain);
    void    tVZFilterLS_setResonance                   (tVZFilterLS const vf, Lfloat res);
    void    tVZFilterLS_setFreqFastAndResonanceAndGain (tVZFilterLS const vf, Lfloat cutoff, Lfloat res, Lfloat gain);


    typedef struct _tVZFilterHS
    {
        tMempool mempool;
        // state:
        Lfloat s1, s2;

        // filter coefficients:
        Lfloat g;          // embedded integrator gain
        Lfloat R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        Lfloat h;          // factor for feedback (== 1/(1+2*R*g+g*g))


        // parameters:
        Lfloat fc;    // characteristic frequency
        Lfloat G;     // gain
        Lfloat Q; //q of filter
        Lfloat B;     // bandwidth (in octaves)
        Lfloat R2Plusg; //precomputed for the tick
        Lfloat sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        Lfloat invSampleRate;
        Lfloat gPreDiv;
        Lfloat sqrtA;
        Lfloat sampRatio; // ratio of the sample rate to 48000 (which is what the tanf table was calculated for)
        const Lfloat *table;
        Lfloat cutoffMIDI;
    } _tVZFilterHS;

    typedef _tVZFilterHS* tVZFilterHS;

    // Memory handlers for `tVZFilterHS`
    void    tVZFilterHS_init                           (tVZFilterHS* const,Lfloat freq, Lfloat Q, Lfloat gain,
                                                        LEAF* const leaf);
    void    tVZFilterHS_initToPool                     (tVZFilterHS* const, Lfloat freq, Lfloat Q, Lfloat gain,
                                                        tMempool* const);
    void    tVZFilterHS_free                           (tVZFilterHS* const);

    // Tick function for `tVZFilterHS`
    Lfloat  tVZFilterHS_tick                           (tVZFilterHS const, Lfloat input);

    // Setter functions for `tVZFilterHS`
    void    tVZFilterHS_setSampleRate                  (tVZFilterHS const, Lfloat sampleRate);
    void    tVZFilterHS_setBandwidthSlow               (tVZFilterHS const, Lfloat bandWidth);
    void    tVZFilterHS_setFreq                        (tVZFilterHS const, Lfloat freq);
    void    tVZFilterHS_setFreqFast                    (tVZFilterHS const vf, Lfloat cutoff);
    void    tVZFilterHS_setGain                        (tVZFilterHS const, Lfloat gain);
    void    tVZFilterHS_setResonance                   (tVZFilterHS const vf, Lfloat res);
    void    tVZFilterHS_setFreqFastAndResonanceAndGain (tVZFilterHS const vf, Lfloat cutoff, Lfloat res, Lfloat gain);

    typedef struct _tVZFilterBell
    {
        tMempool mempool;
        // state:
        Lfloat s1, s2;

        // filter coefficients:
        Lfloat g;          // embedded integrator gain
        Lfloat R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        Lfloat h;          // factor for feedback (== 1/(1+2*R*g+g*g))


        // parameters:
        Lfloat fc;    // characteristic frequency
        Lfloat G;     // gain
        Lfloat B;     // bandwidth (in octaves)
        Lfloat R2Plusg; //precomputed for the tick
        Lfloat sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        Lfloat invSampleRate;
        Lfloat rToUse;
        Lfloat sampRatio; // ratio of the sample rate to 48000 (which is what the tanf table was calculated for)
        const Lfloat *table;
        Lfloat cutoffMIDI;
    } _tVZFilterBell;

    typedef _tVZFilterBell* tVZFilterBell;

    // Memory handlers for `tVZFilterBell`
    void    tVZFilterBell_init                            (tVZFilterBell* const,Lfloat freq, Lfloat BW, Lfloat gain,
                                                           LEAF* const leaf);
    void    tVZFilterBell_initToPool                      (tVZFilterBell* const, Lfloat freq, Lfloat BW, Lfloat gain,
                                                           tMempool* const);
    void    tVZFilterBell_free                            (tVZFilterBell* const);

    // Tick function for `tVZFilterBell`
    Lfloat  tVZFilterBell_tick                           (tVZFilterBell const, Lfloat input);

    // Setter functions for `tVZFilterBell`
    void    tVZFilterBell_setSampleRate                   (tVZFilterBell const, Lfloat sampleRate);
    void    tVZFilterBell_setBandwidth                    (tVZFilterBell const, Lfloat bandWidth);
    void    tVZFilterBell_setFreq                         (tVZFilterBell const, Lfloat freq);
    void    tVZFilterBell_setFreqFast                     (tVZFilterBell const vf, Lfloat cutoff);
    void    tVZFilterBell_setFreqAndGainFast              (tVZFilterBell const, Lfloat freq, Lfloat gain);
    void    tVZFilterBell_setFrequencyAndGain             (tVZFilterBell const, Lfloat freq, Lfloat gain);
    void    tVZFilterBell_setFrequencyAndBandwidthAndGain (tVZFilterBell const vf, Lfloat freq, Lfloat bandwidth, Lfloat gain);
    void    tVZFilterBell_setFreqAndBWAndGainFast         (tVZFilterBell const vf, Lfloat cutoff, Lfloat BW, Lfloat gain);
    void    tVZFilterBell_setGain                         (tVZFilterBell const, Lfloat gain);

    typedef struct _tVZFilterBR
    {
        tMempool mempool;
        // state:
        Lfloat s1, s2;

        // filter coefficients:
        Lfloat g;          // embedded integrator gain
        Lfloat R2;         // twice the damping coefficient (R2 == 2*R == 1/Q)
        Lfloat h;          // factor for feedback (== 1/(1+2*R*g+g*g))
        // parameters:
        Lfloat R2Plusg; //precomputed for the tick
        Lfloat cutoffMIDI;
        Lfloat G;
        Lfloat sampleRate;    //local sampling rate of filter (may be different from leaf sr if oversampled)
        Lfloat invSampleRate;
        Lfloat sampRatio; // ratio of the sample rate to 48000 (which is what the tanf table was calculated for)
        const Lfloat *table;
    } _tVZFilterBR;

    typedef _tVZFilterBR* tVZFilterBR;

    // Memory handlers for `tVZFilterBR`
    void    tVZFilterBR_init                    (tVZFilterBR* const,Lfloat freq, Lfloat Q, LEAF* const leaf);
    void    tVZFilterBR_initToPool              (tVZFilterBR* const, Lfloat freq, Lfloat Q, tMempool* const);
    void    tVZFilterBR_free                    (tVZFilterBR* const);

    // Tick function for `tVZFilterBR`
    Lfloat  tVZFilterBR_tick                    (tVZFilterBR const, Lfloat input);

    // Setter functions for `tVZFilterBR`
    void    tVZFilterBR_setSampleRate           (tVZFilterBR const, Lfloat sampleRate);
    void    tVZFilterBR_setGain                 (tVZFilterBR const, Lfloat gain);
    void    tVZFilterBR_setFreq                 (tVZFilterBR const, Lfloat freq);
    void    tVZFilterBR_setFreqFast             (tVZFilterBR const vf, Lfloat cutoff);
    void    tVZFilterBR_setResonance            (tVZFilterBR const vf, Lfloat res);
    void    tVZFilterBR_setFreqAndResonanceFast (tVZFilterBR const vf, Lfloat cutoff, Lfloat res);

    /*!
     @defgroup tdiodefilter tDiodeFilter
     @ingroup filters
     @brief Diode filter.
     @{
     
     @fn void    tDiodeFilter_init           (tDiodeFilter* const, Lfloat freq, Lfloat Q, LEAF* const leaf)
     @brief Initialize a tDiodeFilter to the default mempool of a LEAF instance.
     @param filter A pointer to the tDiodeFilter to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tDiodeFilter_initToPool     (tDiodeFilter* const, Lfloat freq, Lfloat Q, tMempool* const)
     @brief Initialize a tDiodeFilter to a specified mempool.
     @param filter A pointer to the tDiodeFilter to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tDiodeFilter_free           (tDiodeFilter* const)
     @brief Free a tDiodeFilter from its mempool.
     @param filter A pointer to the tDiodeFilter to free.
     
     @fn Lfloat   tDiodeFilter_tick               (tDiodeFilter* const, Lfloat input)
     @brief
     @param filter A pointer to the relevant tDiodeFilter.
     
     @fn void    tDiodeFilter_setFreq     (tDiodeFilter* const vf, Lfloat cutoff)
     @brief
     @param filter A pointer to the relevant tDiodeFilter.
     
     @fn void    tDiodeFilter_setQ     (tDiodeFilter* const vf, Lfloat resonance)
     @brief
     @param filter A pointer to the relevant tDiodeFilter.
     ￼￼￼
     @} */
    
    //diode ladder filter by Ivan C, based on mystran's method
    typedef struct _tDiodeFilter
    {
        tMempool mempool;
        Lfloat cutoff;
        Lfloat f;
        Lfloat r;
        Lfloat Vt;
        Lfloat n;
        Lfloat gamma;
        Lfloat zi;
        Lfloat g0inv;
        Lfloat g1inv;
        Lfloat g2inv;
        Lfloat s0, s1, s2, s3;
        Lfloat invSampleRate;
        Lfloat sampRatio;
        const Lfloat *table;
        Lfloat cutoffMIDI;
    } _tDiodeFilter;
    
    typedef _tDiodeFilter* tDiodeFilter;

    // Memory handlers for `tDiodeFilter`
    void    tDiodeFilter_init           (tDiodeFilter* const, Lfloat freq, Lfloat Q, LEAF* const leaf);
    void    tDiodeFilter_initToPool     (tDiodeFilter* const, Lfloat freq, Lfloat Q, tMempool* const);
    void    tDiodeFilter_free           (tDiodeFilter* const);

    // Tick functions for `tDiodeFilter`
    Lfloat  tDiodeFilter_tick           (tDiodeFilter const, Lfloat input);
    Lfloat  tDiodeFilter_tickEfficient  (tDiodeFilter const vf, Lfloat in);

    // Setter functions for `tDiodeFilter`
    void    tDiodeFilter_setFreq        (tDiodeFilter const vf, Lfloat cutoff);
    void    tDiodeFilter_setFreqFast    (tDiodeFilter const vf, Lfloat cutoff);
    void    tDiodeFilter_setQ           (tDiodeFilter const vf, Lfloat resonance);
    void    tDiodeFilter_setSampleRate  (tDiodeFilter const vf, Lfloat sr);
    
    
    
    //transistor ladder filter by aciddose, based on mystran's method, KVR forums
    typedef struct _tLadderFilter
    {
        tMempool mempool;
        Lfloat cutoff;
        Lfloat invSampleRate;
        Lfloat sampleRatio;
        int oversampling;
        Lfloat invOS;
        Lfloat c;
        Lfloat fb;
        Lfloat c2;
        Lfloat a;
        Lfloat s;
        Lfloat d;
        Lfloat b[4]; // stored states
        const Lfloat *table;
        Lfloat cutoffMIDI;
    } _tLadderFilter;
    
    typedef _tLadderFilter* tLadderFilter;

    // Memory handlers for `tLadderFilter`
    void    tLadderFilter_init            (tLadderFilter* const, Lfloat freq, Lfloat Q, LEAF* const leaf);
    void    tLadderFilter_initToPool      (tLadderFilter* const, Lfloat freq, Lfloat Q, tMempool* const);
    void    tLadderFilter_free            (tLadderFilter* const);

    // Tick function for `tLadderFilter`
    Lfloat  tLadderFilter_tick            (tLadderFilter const, Lfloat input);

    // Setter functions for `tLadderFilter`
    void    tLadderFilter_setFreq         (tLadderFilter const vf, Lfloat cutoff);
    void    tLadderFilter_setFreqFast     (tLadderFilter const vf, Lfloat cutoff);
    void    tLadderFilter_setQ            (tLadderFilter const vf, Lfloat resonance);
    void    tLadderFilter_setSampleRate   (tLadderFilter const vf, Lfloat sr);
    void    tLadderFilter_setOversampling (tLadderFilter const vf, int os);


    //tilt filter
    typedef struct _tTiltFilter
    {
        tMempool mempool;
        Lfloat cutoff;
        Lfloat sr3;
        Lfloat gfactor;
        Lfloat a0;
        Lfloat b1;
        Lfloat lp_out;
        Lfloat lgain;
        Lfloat hgain;
        Lfloat invAmp;
    } _tTiltFilter;

    typedef _tTiltFilter* tTiltFilter;

    // Memory handlers for `tTiltFilter`
    void    tTiltFilter_init          (tTiltFilter* const, Lfloat freq, LEAF* const leaf);
    void    tTiltFilter_initToPool    (tTiltFilter* const, Lfloat freq, tMempool* const);
    void    tTiltFilter_free          (tTiltFilter* const);

    // Tick function for `tTiltFilter`
    Lfloat  tTiltFilter_tick          (tTiltFilter const, Lfloat input);

    // Setter functions for `tTiltFilter`
    void    tTiltFilter_setTilt       (tTiltFilter const vf, Lfloat tilt);
    void    tTiltFilter_setSampleRate (tTiltFilter const vf, Lfloat sr);


#ifdef __cplusplus
}
#endif

#endif  // LEAF_FILTERS_H_INCLUDED

//==============================================================================


