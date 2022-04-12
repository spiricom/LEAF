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
     
     @fn void    tAllpass_init           (tAllpass* const, float initDelay, uint32_t maxDelay, LEAF* const leaf)
     @brief Initialize a tAllpass to the default mempool of a LEAF instance.
     @param filter A pointer to the tAllpass to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tAllpass_initToPool     (tAllpass* const, float initDelay, uint32_t maxDelay, tMempool* const)
     @brief Initialize a tAllpass to a specified mempool.
     @param filter A pointer to the tAllpass to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tAllpass_free           (tAllpass* const)
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
    
    typedef struct _tAllpass
    {
        
        tMempool mempool;
        
        float gain;
        
        tLinearDelay delay;
        
        float lastOut;
    } _tAllpass;
    
    typedef _tAllpass* tAllpass;
    
    void    tAllpass_init           (tAllpass* const, float initDelay, uint32_t maxDelay, LEAF* const leaf);
    void    tAllpass_initToPool     (tAllpass* const, float initDelay, uint32_t maxDelay, tMempool* const);
    void    tAllpass_free           (tAllpass* const);
    
    float   tAllpass_tick           (tAllpass* const, float input);
    void    tAllpass_setGain        (tAllpass* const, float gain);
    void    tAllpass_setDelay       (tAllpass* const, float delay);
    
    
    //==============================================================================
    
    /*!
     @defgroup tonepole tOnePole
     @ingroup filters
     @brief OnePole filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tOnePole_init           (tOnePole* const, float thePole, LEAF* const leaf)
     @brief Initialize a tOnePole to the default mempool of a LEAF instance.
     @param filter A pointer to the tOnePole to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tOnePole_initToPool     (tOnePole* const, float thePole, tMempool* const)
     @brief Initialize a tOnePole to a specified mempool.
     @param filter A pointer to the tOnePole to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tOnePole_free           (tOnePole* const)
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
    
    typedef struct _tOnePole
    {
        
        tMempool mempool;
        float freq;
        float gain;
        float a0,a1;
        float b0,b1;
        float lastIn, lastOut;
        float twoPiTimesInvSampleRate;
    } _tOnePole;
    
    typedef _tOnePole* tOnePole;
    
    void    tOnePole_init           (tOnePole* const, float thePole, LEAF* const leaf);
    void    tOnePole_initToPool     (tOnePole* const, float thePole, tMempool* const);
    void    tOnePole_free           (tOnePole* const);
    
    float   tOnePole_tick           (tOnePole* const, float input);
    void    tOnePole_setB0          (tOnePole* const, float b0);
    void    tOnePole_setA1          (tOnePole* const, float a1);
    void    tOnePole_setPole        (tOnePole* const, float thePole);
    void    tOnePole_setFreq        (tOnePole* const, float freq);
    void    tOnePole_setCoefficients(tOnePole* const, float b0, float a1);
    void    tOnePole_setGain        (tOnePole* const, float gain);
    void    tOnePole_setSampleRate  (tOnePole* const, float sr);
    
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

    typedef struct _tTwoPole
    {
        
        tMempool mempool;
        
        float gain;
        float a0, a1, a2;
        float b0;
        
        float radius, frequency;
        int normalize;
        
        float lastOut[2];
        
        float sampleRate;
        float twoPiTimesInvSampleRate;
    } _tTwoPole;
    
    typedef _tTwoPole* tTwoPole;
    
    void    tTwoPole_init           (tTwoPole* const, LEAF* const leaf);
    void    tTwoPole_initToPool     (tTwoPole* const, tMempool* const);
    void    tTwoPole_free           (tTwoPole* const);
    
    float   tTwoPole_tick           (tTwoPole* const, float input);
    void    tTwoPole_setB0          (tTwoPole* const, float b0);
    void    tTwoPole_setA1          (tTwoPole* const, float a1);
    void    tTwoPole_setA2          (tTwoPole* const, float a2);
    void    tTwoPole_setResonance   (tTwoPole* const, float freq, float radius, int normalize);
    void    tTwoPole_setCoefficients(tTwoPole* const, float b0, float a1, float a2);
    void    tTwoPole_setGain        (tTwoPole* const, float gain);
    void    tTwoPole_setSampleRate  (tTwoPole* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tonezero tOneZero
     @ingroup filters
     @brief OneZero filter, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tOneZero_init           (tOneZero* const, float theZero, LEAF* const leaf)
     @brief Initialize a tOneZero to the default mempool of a LEAF instance.
     @param filter A pointer to the tSlide to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tOneZero_initToPool     (tOneZero* const, float theZero, tMempool* const)
     @brief Initialize a tOneZero to a specified mempool.
     @param filter A pointer to the tOneZero to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tOneZero_free           (tOneZero* const)
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
    
    typedef struct _tOneZero
    {
        tMempool mempool;
        float gain;
        float b0,b1;
        float lastIn, lastOut, frequency;
        float invSampleRate;
    } _tOneZero;
    
    typedef _tOneZero* tOneZero;
    
    void    tOneZero_init           (tOneZero* const, float theZero, LEAF* const leaf);
    void    tOneZero_initToPool     (tOneZero* const, float theZero, tMempool* const);
    void    tOneZero_free           (tOneZero* const);
    
    float   tOneZero_tick           (tOneZero* const, float input);
    void    tOneZero_setB0          (tOneZero* const, float b0);
    void    tOneZero_setB1          (tOneZero* const, float b1);
    void    tOneZero_setZero        (tOneZero* const, float theZero);
    void    tOneZero_setCoefficients(tOneZero* const, float b0, float b1);
    void    tOneZero_setGain        (tOneZero* const, float gain);
    float   tOneZero_getPhaseDelay  (tOneZero* const, float frequency);
    void    tOneZero_setSampleRate  (tOneZero* const, float sr);
    
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

    typedef struct _tTwoZero
    {
        tMempool mempool;
        
        float gain;
        float b0, b1, b2;
        float frequency, radius;
        float lastIn[2];
        float twoPiTimesInvSampleRate;
    } _tTwoZero;
    
    typedef _tTwoZero* tTwoZero;
    
    void    tTwoZero_init           (tTwoZero* const, LEAF* const leaf);
    void    tTwoZero_initToPool     (tTwoZero* const, tMempool* const);
    void    tTwoZero_free           (tTwoZero* const);
    
    float   tTwoZero_tick           (tTwoZero* const, float input);
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
    
    typedef struct _tPoleZero
    {
        
        tMempool mempool;
        
        float gain;
        float a0,a1;
        float b0,b1;
        
        float lastIn, lastOut;
    } _tPoleZero;
    
    typedef _tPoleZero* tPoleZero;
    
    void    tPoleZero_init              (tPoleZero* const, LEAF* const leaf);
    void    tPoleZero_initToPool        (tPoleZero* const, tMempool* const);
    void    tPoleZero_free              (tPoleZero* const);
    
    float   tPoleZero_tick              (tPoleZero* const, float input);
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
    
    typedef struct _tBiQuad
    {
        tMempool mempool;
        
        float gain;
        float a0, a1, a2;
        float b0, b1, b2;
        
        float lastIn[2];
        float lastOut[2];
        
        float frequency, radius;
        int normalize;
        
        float sampleRate;
        float twoPiTimesInvSampleRate;
    } _tBiQuad;
    
    typedef _tBiQuad* tBiQuad;
    
    void    tBiQuad_init           (tBiQuad* const, LEAF* const leaf);
    void    tBiQuad_initToPool     (tBiQuad* const, tMempool* const);
    void    tBiQuad_free           (tBiQuad* const);
    
    float   tBiQuad_tick           (tBiQuad* const, float input);
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
     
     @fn void    tSVF_init           (tSVF* const, SVFType type, float freq, float Q, LEAF* const leaf)
     @brief Initialize a tSVF to the default mempool of a LEAF instance.
     @param filter A pointer to the tSVF to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSVF_initToPool     (tSVF* const, SVFType type, float freq, float Q, tMempool* const)
     @brief Initialize a tSVF to a specified mempool.
     @param filter A pointer to the tSVF to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSVF_free           (tSVF* const)
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
        float cutoff, Q;
        float ic1eq,ic2eq;
        float g,k,a1,a2,a3,cH,cB,cL,cBK;
        float sampleRate;
        float invSampleRate;
    } _tSVF;
    
    typedef _tSVF* tSVF;
    
    void    tSVF_init           (tSVF* const, SVFType type, float freq, float Q, LEAF* const leaf);
    void    tSVF_initToPool     (tSVF* const, SVFType type, float freq, float Q, tMempool* const);
    void    tSVF_free           (tSVF* const);
    
    float   tSVF_tick           (tSVF* const, float v0);
    void    tSVF_setFreq        (tSVF* const, float freq);
    void    tSVF_setQ           (tSVF* const, float Q);
    void    tSVF_setFreqAndQ    (tSVF* const svff, float freq, float Q);
    void    tSVF_setSampleRate  (tSVF* const svff, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tefficientsvf tEfficientSVF
     @ingroup filters
     @brief Efficient State Variable Filter for 14-bit control input, [0, 4096).
     @{
     
     @fn void    tEfficientSVF_init          (tEfficientSVF* const, SVFType type, uint16_t input, float Q, LEAF* const leaf)
     @brief Initialize a tEfficientSVF to the default mempool of a LEAF instance.
     @param filter A pointer to the tEfficientSVF to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tEfficientSVF_initToPool    (tEfficientSVF* const, SVFType type, uint16_t input, float Q, tMempool* const)
     @brief Initialize a tEfficientSVF to a specified mempool.
     @param filter A pointer to the tEfficientSVF to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tEfficientSVF_free          (tEfficientSVF* const)
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
    
    typedef struct _tEfficientSVF
    {
        
        tMempool mempool;
        SVFType type;
        float cutoff, Q;
        float ic1eq,ic2eq;
        float g,k,a1,a2,a3;
    } _tEfficientSVF;
    
    typedef _tEfficientSVF* tEfficientSVF;
    
    void    tEfficientSVF_init          (tEfficientSVF* const, SVFType type, uint16_t input, float Q, LEAF* const leaf);
    void    tEfficientSVF_initToPool    (tEfficientSVF* const, SVFType type, uint16_t input, float Q, tMempool* const);
    void    tEfficientSVF_free          (tEfficientSVF* const);
    
    float   tEfficientSVF_tick          (tEfficientSVF* const, float v0);
    void    tEfficientSVF_setFreq       (tEfficientSVF* const, uint16_t controlFreq);
    void    tEfficientSVF_setQ          (tEfficientSVF* const, float Q);
    void    tEfficientSVF_setFreqAndQ   (tEfficientSVF* const, uint16_t controlFreq, float Q);
    
    //==============================================================================
    
    /*!
     @defgroup thighpass tHighpass
     @ingroup filters
     @brief Simple Highpass filter.
     @{
     
     @fn void    tHighpass_init          (tHighpass* const, float freq, LEAF* const leaf)
     @brief Initialize a tHighpass to the default mempool of a LEAF instance.
     @param filter A pointer to the tHighpass to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tHighpass_initToPool    (tHighpass* const, float freq, tMempool* const)
     @brief Initialize a tHighpass to a specified mempool.
     @param filter A pointer to the tHighpass to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tHighpass_free          (tHighpass* const)
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
    
    typedef struct _tHighpass
    {
        tMempool mempool;
        float xs, ys, R;
        float frequency;
        float twoPiTimesInvSampleRate;
    } _tHighpass;
    
    typedef _tHighpass* tHighpass;
    
    void    tHighpass_init          (tHighpass* const, float freq, LEAF* const leaf);
    void    tHighpass_initToPool    (tHighpass* const, float freq, tMempool* const);
    void    tHighpass_free          (tHighpass* const);
    
    float   tHighpass_tick          (tHighpass* const, float x);
    void    tHighpass_setFreq       (tHighpass* const, float freq);
    float   tHighpass_getFreq       (tHighpass* const);
    void    tHighpass_setSampleRate (tHighpass* const, float sr);
    
    //==============================================================================
    
    /*!
     @defgroup tbutterworth tButterworth
     @ingroup filters
     @brief Butterworth filter.
     @{
     
     @fn void    tButterworth_init           (tButterworth* const, int N, float f1, float f2, LEAF* const leaf, LEAF* const leaf)
     @brief Initialize a tButterworth to the default mempool of a LEAF instance.
     @param filter A pointer to the tButterworth to initialize.
     @param leaf A pointer to the leaf instance.
     @param order Order of the filter.
     @param lowCutoff Lower cutoff frequency.
     @param upperCutoff Upper cutoff frequency.
     
     @fn void    tButterworth_initToPool     (tButterworth* const, int N, float f1, float f2, tMempool* const)
     @brief Initialize a tButterworth to a specified mempool.
     @param filter A pointer to the tButterworth to initialize.
     @param mempool A pointer to the tMempool to use.
     @param order Order of the filter.
     @param lowCutoff Lower cutoff frequency.
     @param upperCutoff Upper cutoff frequency.
     
     @fn void    tButterworth_free           (tButterworth* const)
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
    typedef struct _tButterworth
    {
        tMempool mempool;
        
        float gain;
        int order;
        int numSVF;
        
        tSVF* svf;
        
        float f1,f2;
    } _tButterworth;
    
    typedef _tButterworth* tButterworth;
    
    void    tButterworth_init           (tButterworth* const, int N, float f1, float f2, LEAF* const leaf);
    void    tButterworth_initToPool     (tButterworth* const, int N, float f1, float f2, tMempool* const);
    void    tButterworth_free           (tButterworth* const);
    
    float   tButterworth_tick           (tButterworth* const, float input);
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
     
     @fn void    tFIR_init           (tFIR* const, float* coeffs, int numTaps, LEAF* const leaf)
     @brief Initialize a tFIR to the default mempool of a LEAF instance.
     @param filter A pointer to the tFIR to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tFIR_initToPool     (tFIR* const, float* coeffs, int numTaps, tMempool* const)
     @brief Initialize a tFIR to a specified mempool.
     @param filter A pointer to the tFIR to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tFIR_free           (tFIR* const)
     @brief Free a tFIR from its mempool.
     @param filter A pointer to the tFIR to free.
     
     @fn float   tFIR_tick           (tFIR* const, float input)
     @brief
     @param filter A pointer to the relevant tFIR.
     ￼￼￼
     @} */
    
    typedef struct _tFIR
    {
        
        tMempool mempool;
        float* past;
        float* coeff;
        int numTaps;
    } _tFIR;
    
    typedef _tFIR* tFIR;
    
    void    tFIR_init           (tFIR* const, float* coeffs, int numTaps, LEAF* const leaf);
    void    tFIR_initToPool     (tFIR* const, float* coeffs, int numTaps, tMempool* const);
    void    tFIR_free           (tFIR* const);
    
    float   tFIR_tick           (tFIR* const, float input);
    
    
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
     
     @fn float   tMedianFilter_tick           (tMedianFilter* const, float input)
     @brief
     @param filter A pointer to the relevant tMedianFilter.
     ￼￼￼
     @} */
    
    typedef struct _tMedianFilter
    {
        
        tMempool mempool;
        float* val;
        int* age;
        int m;
        int size;
        int middlePosition;
        int last;
        int pos;
    } _tMedianFilter;
    
    typedef _tMedianFilter* tMedianFilter;
    
    void    tMedianFilter_init           (tMedianFilter* const, int size, LEAF* const leaf);
    void    tMedianFilter_initToPool     (tMedianFilter* const, int size, tMempool* const);
    void    tMedianFilter_free           (tMedianFilter* const);
    
    float   tMedianFilter_tick           (tMedianFilter* const, float input);
    
    
    /*!
     @defgroup tvzfilter tVZFilter
     @ingroup filters
     @brief Vadim Zavalishin style from VA book (from implementation in RSlib posted to kvr forum)
     @{
     
     @fn void    tVZFilter_init           (tVZFilter* const, VZFilterType type, float freq, float Q, LEAF* const leaf)
     @brief Initialize a tVZFilter to the default mempool of a LEAF instance.
     @param filter A pointer to the tVZFilter to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tVZFilter_initToPool     (tVZFilter* const, VZFilterType type, float freq, float Q, tMempool* const)
     @brief Initialize a tVZFilter to a specified mempool.
     @param filter A pointer to the tVZFilter to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tVZFilter_free           (tVZFilter* const)
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
    
    
    typedef struct _tVZFilter
    {
        tMempool mempool;
        
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
    } _tVZFilter;
    
    typedef _tVZFilter* tVZFilter;
    
    void    tVZFilter_init           (tVZFilter* const, VZFilterType type, float freq, float Q, LEAF* const leaf);
    void    tVZFilter_initToPool     (tVZFilter* const, VZFilterType type, float freq, float Q, tMempool* const);
    void    tVZFilter_free           (tVZFilter* const);
    
    void    tVZFilter_setSampleRate  (tVZFilter* const, float sampleRate);
    float   tVZFilter_tick               (tVZFilter* const, float input);
    float   tVZFilter_tickEfficient               (tVZFilter* const vf, float in);
    void    tVZFilter_calcCoeffs           (tVZFilter* const);
    void    tVZFilter_calcCoeffsEfficientBP           (tVZFilter* const);
    void    tVZFilter_setBandwidth            (tVZFilter* const, float bandWidth);
    void    tVZFilter_setFreq           (tVZFilter* const, float freq);
    void    tVZFilter_setFreqAndBandwidth    (tVZFilter* const vf, float freq, float bw);
    void    tVZFilter_setFreqAndBandwidthEfficientBP    (tVZFilter* const vf, float freq, float bw);
    void    tVZFilter_setGain                  (tVZFilter* const, float gain);
    void    tVZFilter_setResonance                (tVZFilter* const vf, float res);
    void    tVZFilter_setFrequencyAndResonance (tVZFilter* const vf, float freq, float res);
    void    tVZFilter_setFrequencyAndResonanceAndGain (tVZFilter* const vf, float freq, float res, float gains);
    void    tVZFilter_setFrequencyAndBandwidthAndGain (tVZFilter* const vf, float freq, float BW, float gain);
    void    tVZFilter_setFrequencyAndResonanceAndMorph (tVZFilter* const vf, float freq, float res, float morph);
    void    tVZFilter_setMorphOnly               (tVZFilter* const vf, float morph);
    void    tVZFilter_setMorph               (tVZFilter* const vf, float morph);
    void    tVZFilter_setType                  (tVZFilter* const, VZFilterType type);
    float   tVZFilter_BandwidthToR        (tVZFilter* const vf, float B);
    float   tVZFilter_BandwidthToREfficientBP(tVZFilter* const vf, float B);
    
    /*!
     @defgroup tdiodefilter tDiodeFilter
     @ingroup filters
     @brief Diode filter.
     @{
     
     @fn void    tDiodeFilter_init           (tDiodeFilter* const, float freq, float Q, LEAF* const leaf)
     @brief Initialize a tDiodeFilter to the default mempool of a LEAF instance.
     @param filter A pointer to the tDiodeFilter to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tDiodeFilter_initToPool     (tDiodeFilter* const, float freq, float Q, tMempool* const)
     @brief Initialize a tDiodeFilter to a specified mempool.
     @param filter A pointer to the tDiodeFilter to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tDiodeFilter_free           (tDiodeFilter* const)
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
    typedef struct _tDiodeFilter
    {
        tMempool mempool;
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
    } _tDiodeFilter;
    
    typedef _tDiodeFilter* tDiodeFilter;
    
    void    tDiodeFilter_init           (tDiodeFilter* const, float freq, float Q, LEAF* const leaf);
    void    tDiodeFilter_initToPool     (tDiodeFilter* const, float freq, float Q, tMempool* const);
    void    tDiodeFilter_free           (tDiodeFilter* const);
    
    float   tDiodeFilter_tick               (tDiodeFilter* const, float input);
    void    tDiodeFilter_setFreq     (tDiodeFilter* const vf, float cutoff);
    void    tDiodeFilter_setQ     (tDiodeFilter* const vf, float resonance);
    void    tDiodeFilter_setSampleRate(tDiodeFilter* const vf, float sr);
    
    
    
    //transistor ladder filter by aciddose, based on mystran's method, KVR forums
    typedef struct _tLadderFilter
    {
        tMempool mempool;
        float cutoff;
        float invSampleRate;
        int oversampling;
        float c;
        float fb;
        float c2;
        float a;
        float s;
        float d;
        float b[4]; // stored states
    } _tLadderFilter;
    
    typedef _tLadderFilter* tLadderFilter;
    
    void    tLadderFilter_init           (tLadderFilter* const, float freq, float Q, LEAF* const leaf);
    void    tLadderFilter_initToPool     (tLadderFilter* const, float freq, float Q, tMempool* const);
    void    tLadderFilter_free           (tLadderFilter* const);
    
    float   tLadderFilter_tick               (tLadderFilter* const, float input);
    void    tLadderFilter_setFreq     (tLadderFilter* const vf, float cutoff);
    void    tLadderFilter_setQ     (tLadderFilter* const vf, float resonance);
    void    tLadderFilter_setSampleRate(tLadderFilter* const vf, float sr);
    



#ifdef __cplusplus
}
#endif

#endif  // LEAF_FILTERS_H_INCLUDED

//==============================================================================


