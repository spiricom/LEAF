/*==============================================================================
 
 leaf-reverb.h
 Created: 20 Jan 2017 12:02:04pm
 Author:  Michael R Mulshine
 
 ==============================================================================*/

#ifndef LEAF_REVERB_H_INCLUDED
#define LEAF_REVERB_H_INCLUDED

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
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */

    //==============================================================================
    
    /*!
     @defgroup tprcreverb tPRCReverb
     @ingroup reverb
     @brief Reverb, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tPRCReverb_init         (tPRCReverb* const, Lfloat t60, LEAF* const leaf)
     @brief Initialize a tPRCReverb to the default mempool of a LEAF instance.
     @param reverb A pointer to the tPRCReverb to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPRCReverb_initToPool   (tPRCReverb* const, Lfloat t60, tMempool* const)
     @brief Initialize a tPRCReverb to a specified mempool.
     @param reverb A pointer to the tPRCReverb to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPRCReverb_free         (tPRCReverb* const)
     @brief Free a tPRCReverb from its mempool.
     @param reverb A pointer to the tPRCReverb to free.
     
     @fn void    tPRCReverb_clear        (tPRCReverb* const)
     @brief
     @param reverb A pointer to the relevant tPRCReverb.
     
     @fn Lfloat   tPRCReverb_tick         (tPRCReverb* const, Lfloat input)
     @brief
     @param reverb A pointer to the relevant tPRCReverb.

     @fn void    tPRCReverb_setT60       (tPRCReverb* const, Lfloat t60)
     @brief Set reverb time in seconds.
     @param reverb A pointer to the relevant tPRCReverb.
    
     @fn void    tPRCReverb_setMix       (tPRCReverb* const, Lfloat mix)
     @brief Set mix between dry input and wet output signal.
     @param reverb A pointer to the relevant tPRCReverb.
     
     @} */
    
    typedef struct _tPRCReverb
    {
        
        tMempool mempool;
        
        Lfloat mix, t60;
        
        Lfloat sampleRate;
        Lfloat invSampleRate;
        
        tDelay allpassDelays[2];
        tDelay combDelay;
        Lfloat allpassCoeff;
        Lfloat combCoeff;
        
        Lfloat lastIn, lastOut;
    } _tPRCReverb;
    
    typedef _tPRCReverb* tPRCReverb;
    
    void    tPRCReverb_init         (tPRCReverb* const, Lfloat t60, LEAF* const leaf);
    void    tPRCReverb_initToPool   (tPRCReverb* const, Lfloat t60, tMempool* const);
    void    tPRCReverb_free         (tPRCReverb* const);
    
    void    tPRCReverb_clear        (tPRCReverb* const);
    Lfloat   tPRCReverb_tick         (tPRCReverb* const, Lfloat input);
    void    tPRCReverb_setT60       (tPRCReverb* const, Lfloat t60);
    void    tPRCReverb_setMix       (tPRCReverb* const, Lfloat mix);
    void    tPRCReverb_setSampleRate(tPRCReverb* const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tnreverb tNReverb
     @ingroup reverb
     @brief Reverb, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tNReverb_init           (tNReverb* const, Lfloat t60, LEAF* const leaf)
     @brief Initialize a tNReverb to the default mempool of a LEAF instance.
     @param reverb A pointer to the tNReverb to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tNReverb_initToPool     (tNReverb* const, Lfloat t60, tMempool* const)
     @brief Initialize a tNReverb to a specified mempool.
     @param reverb A pointer to the tNReverb to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tNReverb_free           (tNReverb* const)
     @brief Free a tNReverb from its mempool.
     @param reverb A pointer to the tNReverb to free.
     
     @fn void    tNReverb_clear          (tNReverb* const)
     @brief
     @param reverb A pointer to the relevant tNReverb.
     
     @fn Lfloat   tNReverb_tick           (tNReverb* const, Lfloat input)
     @brief
     @param reverb A pointer to the relevant tNReverb.
     
     @fn void    tNReverb_tickStereo     (tNReverb* const rev, Lfloat input, Lfloat* output)
     @brief 
     @param reverb A pointer to the relevant tNReverb.
     
     @fn void    tNReverb_setT60         (tNReverb* const, Lfloat t60)
     @brief Set reverb time in seconds.
     @param reverb A pointer to the relevant tNReverb.
     
     @fn void    tNReverb_setMix         (tNReverb* const, Lfloat mix)
     @brief Set mix between dry input and wet output signal.
     @param reverb A pointer to the relevant tNReverb.
      
     @} */
    
    typedef struct _tNReverb
    {
        
        tMempool mempool;
        
        Lfloat mix, t60;
        
        Lfloat sampleRate;
        Lfloat invSampleRate;
        
        tLinearDelay allpassDelays[8];
        tLinearDelay combDelays[6];
        Lfloat allpassCoeff;
        Lfloat combCoeffs[6];
        Lfloat lowpassState;
        
        Lfloat lastIn, lastOut;
    } _tNReverb;
    
    typedef _tNReverb* tNReverb;
    
    void    tNReverb_init           (tNReverb* const, Lfloat t60, LEAF* const leaf);
    void    tNReverb_initToPool     (tNReverb* const, Lfloat t60, tMempool* const);
    void    tNReverb_free           (tNReverb* const);
    
    void    tNReverb_clear          (tNReverb* const);
    Lfloat   tNReverb_tick           (tNReverb* const, Lfloat input);
    void    tNReverb_tickStereo     (tNReverb* const rev, Lfloat input, Lfloat* output);
    void    tNReverb_setT60         (tNReverb* const, Lfloat t60);
    void    tNReverb_setMix         (tNReverb* const, Lfloat mix);
    void    tNReverb_setSampleRate  (tNReverb* const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tdattorroreverb tDattorroReverb
     @ingroup reverb
     @brief Dattorro plate reverb.
     @{
     
     @fn void    tDattorroReverb_init              (tDattorroReverb* const, LEAF* const leaf)
     @brief Initialize a tDattorroReverb to the default mempool of a LEAF instance.
     @param reverb A pointer to the tDattorroReverb to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tDattorroReverb_initToPool        (tDattorroReverb* const, tMempool* const)
     @brief Initialize a tDattorroReverb to a specified mempool.
     @param reverb A pointer to the tDattorroReverb to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tDattorroReverb_free              (tDattorroReverb* const)
     @brief Free a tDattorroReverb from its mempool.
     @param reverb A pointer to the tDattorroReverb to free.
     
     @fn void    tDattorroReverb_clear             (tDattorroReverb* const)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn Lfloat   tDattorroReverb_tick              (tDattorroReverb* const, Lfloat input)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_tickStereo        (tDattorroReverb* const rev, Lfloat input, Lfloat* output)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setMix            (tDattorroReverb* const, Lfloat mix)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setFreeze         (tDattorroReverb* const rev, uint32_t freeze)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setHP             (tDattorroReverb* const, Lfloat freq)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setSize           (tDattorroReverb* const, Lfloat size)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setInputDelay     (tDattorroReverb* const, Lfloat preDelay)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setInputFilter    (tDattorroReverb* const, Lfloat freq)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setFeedbackFilter (tDattorroReverb* const, Lfloat freq)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setFeedbackGain   (tDattorroReverb* const, Lfloat gain)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @} */
    
    typedef struct _tDattorroReverb
    {
        
        tMempool mempool;
        
        Lfloat   sampleRate;
        Lfloat   predelay;
        Lfloat   input_filter;
        Lfloat   feedback_filter;
        Lfloat   feedback_gain;
        Lfloat   mix;
        int frozen;
        
        Lfloat   size, size_max, t;
        
        Lfloat   f1_delay_2_last,
        f2_delay_2_last;
        
        Lfloat   f1_last,
        f2_last;
        
        // INPUT
        tTapeDelay  in_delay;
        tOnePole    in_filter;
        tAllpass    in_allpass[4];
        
        // FEEDBACK 1
        tAllpass    f1_allpass;
        tTapeDelay  f1_delay_1;
        tOnePole    f1_filter;
        tTapeDelay  f1_delay_2;
        tTapeDelay  f1_delay_3;
        tHighpass   f1_hp;
        
        tCycle      f1_lfo;
        
        // FEEDBACK 2
        tAllpass    f2_allpass;
        tTapeDelay  f2_delay_1;
        tOnePole    f2_filter;
        tTapeDelay  f2_delay_2;
        tTapeDelay  f2_delay_3;
        tHighpass   f2_hp;
        
        tCycle      f2_lfo;
    } _tDattorroReverb;
    
    typedef _tDattorroReverb* tDattorroReverb;
    
    void    tDattorroReverb_init              (tDattorroReverb* const, LEAF* const leaf);
    void    tDattorroReverb_initToPool        (tDattorroReverb* const, tMempool* const);
    void    tDattorroReverb_free              (tDattorroReverb* const);
    
    void    tDattorroReverb_clear             (tDattorroReverb* const);
    Lfloat   tDattorroReverb_tick              (tDattorroReverb* const, Lfloat input);
    void    tDattorroReverb_tickStereo        (tDattorroReverb* const rev, Lfloat input, Lfloat* output);
    void    tDattorroReverb_setMix            (tDattorroReverb* const, Lfloat mix);
    void    tDattorroReverb_setFreeze         (tDattorroReverb* const rev, int freeze);
    void    tDattorroReverb_setHP             (tDattorroReverb* const, Lfloat freq);
    void    tDattorroReverb_setSize           (tDattorroReverb* const, Lfloat size);
    void    tDattorroReverb_setInputDelay     (tDattorroReverb* const, Lfloat preDelay);
    void    tDattorroReverb_setInputFilter    (tDattorroReverb* const, Lfloat freq);
    void    tDattorroReverb_setFeedbackFilter (tDattorroReverb* const, Lfloat freq);
    void    tDattorroReverb_setFeedbackGain   (tDattorroReverb* const, Lfloat gain);
    void    tDattorroReverb_setSampleRate     (tDattorroReverb* const, Lfloat sr);
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_REVERB_H_INCLUDED

//==============================================================================

