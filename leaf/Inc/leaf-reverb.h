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
     
     @fn void    tPRCReverb_init         (tPRCReverb* const, float t60, LEAF* const leaf)
     @brief Initialize a tPRCReverb to the default mempool of a LEAF instance.
     @param reverb A pointer to the tPRCReverb to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPRCReverb_initToPool   (tPRCReverb* const, float t60, tMempool* const)
     @brief Initialize a tPRCReverb to a specified mempool.
     @param reverb A pointer to the tPRCReverb to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPRCReverb_free         (tPRCReverb* const)
     @brief Free a tPRCReverb from its mempool.
     @param reverb A pointer to the tPRCReverb to free.
     
     @fn void    tPRCReverb_clear        (tPRCReverb* const)
     @brief
     @param reverb A pointer to the relevant tPRCReverb.
     
     @fn float   tPRCReverb_tick         (tPRCReverb* const, float input)
     @brief
     @param reverb A pointer to the relevant tPRCReverb.

     @fn void    tPRCReverb_setT60       (tPRCReverb* const, float t60)
     @brief Set reverb time in seconds.
     @param reverb A pointer to the relevant tPRCReverb.
    
     @fn void    tPRCReverb_setMix       (tPRCReverb* const, float mix)
     @brief Set mix between dry input and wet output signal.
     @param reverb A pointer to the relevant tPRCReverb.
     
     @} */
    
    typedef struct _tPRCReverb
    {
        
        tMempool mempool;
        
        float mix, t60;
        
        float inv_441;
        
        tDelay allpassDelays[2];
        tDelay combDelay;
        float allpassCoeff;
        float combCoeff;
        
        float lastIn, lastOut;
    } _tPRCReverb;
    
    typedef _tPRCReverb* tPRCReverb;
    
    void    tPRCReverb_init         (tPRCReverb* const, float t60, LEAF* const leaf);
    void    tPRCReverb_initToPool   (tPRCReverb* const, float t60, tMempool* const);
    void    tPRCReverb_free         (tPRCReverb* const);
    
    void    tPRCReverb_clear        (tPRCReverb* const);
    float   tPRCReverb_tick         (tPRCReverb* const, float input);
    void    tPRCReverb_setT60       (tPRCReverb* const, float t60);
    void    tPRCReverb_setMix       (tPRCReverb* const, float mix);
    
    //==============================================================================
    
    /*!
     @defgroup tnreverb tNReverb
     @ingroup reverb
     @brief Reverb, reimplemented from STK (Cook and Scavone).
     @{
     
     @fn void    tNReverb_init           (tNReverb* const, float t60, LEAF* const leaf)
     @brief Initialize a tNReverb to the default mempool of a LEAF instance.
     @param reverb A pointer to the tNReverb to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tNReverb_initToPool     (tNReverb* const, float t60, tMempool* const)
     @brief Initialize a tNReverb to a specified mempool.
     @param reverb A pointer to the tNReverb to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tNReverb_free           (tNReverb* const)
     @brief Free a tNReverb from its mempool.
     @param reverb A pointer to the tNReverb to free.
     
     @fn void    tNReverb_clear          (tNReverb* const)
     @brief
     @param reverb A pointer to the relevant tNReverb.
     
     @fn float   tNReverb_tick           (tNReverb* const, float input)
     @brief
     @param reverb A pointer to the relevant tNReverb.
     
     @fn void    tNReverb_tickStereo     (tNReverb* const rev, float input, float* output)
     @brief 
     @param reverb A pointer to the relevant tNReverb.
     
     @fn void    tNReverb_setT60         (tNReverb* const, float t60)
     @brief Set reverb time in seconds.
     @param reverb A pointer to the relevant tNReverb.
     
     @fn void    tNReverb_setMix         (tNReverb* const, float mix)
     @brief Set mix between dry input and wet output signal.
     @param reverb A pointer to the relevant tNReverb.
      
     @} */
    
    typedef struct _tNReverb
    {
        
        tMempool mempool;
        
        float mix, t60;
        
        float inv_sr, inv_441;
        
        tLinearDelay allpassDelays[8];
        tLinearDelay combDelays[6];
        float allpassCoeff;
        float combCoeffs[6];
        float lowpassState;
        
        float lastIn, lastOut;
    } _tNReverb;
    
    typedef _tNReverb* tNReverb;
    
    void    tNReverb_init           (tNReverb* const, float t60, LEAF* const leaf);
    void    tNReverb_initToPool     (tNReverb* const, float t60, tMempool* const);
    void    tNReverb_free           (tNReverb* const);
    
    void    tNReverb_clear          (tNReverb* const);
    float   tNReverb_tick           (tNReverb* const, float input);
    void    tNReverb_tickStereo     (tNReverb* const rev, float input, float* output);
    void    tNReverb_setT60         (tNReverb* const, float t60);
    void    tNReverb_setMix         (tNReverb* const, float mix);
    
    //==============================================================================
    
    /*!
     @defgroup tdattorroreverb tDattorroReverb
     @ingroup reverb
     @brief
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
     
     @fn float   tDattorroReverb_tick              (tDattorroReverb* const, float input)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_tickStereo        (tDattorroReverb* const rev, float input, float* output)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setMix            (tDattorroReverb* const, float mix)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setFreeze         (tDattorroReverb* const rev, uint32_t freeze)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setHP             (tDattorroReverb* const, float freq)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setSize           (tDattorroReverb* const, float size)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setInputDelay     (tDattorroReverb* const, float preDelay)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setInputFilter    (tDattorroReverb* const, float freq)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setFeedbackFilter (tDattorroReverb* const, float freq)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @fn void    tDattorroReverb_setFeedbackGain   (tDattorroReverb* const, float gain)
     @brief
     @param reverb A pointer to the relevant tDattorroReverb.
     
     @} */
    
    typedef struct _tDattorroReverb
    {
        
        tMempool mempool;
        
        float   predelay;
        float   input_filter;
        float   feedback_filter;
        float   feedback_gain;
        float   mix;
        uint32_t frozen;
        
        float   size, size_max, t;
        
        float   f1_delay_2_last,
        f2_delay_2_last;
        
        float   f1_last,
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
    float   tDattorroReverb_tick              (tDattorroReverb* const, float input);
    void    tDattorroReverb_tickStereo        (tDattorroReverb* const rev, float input, float* output);
    void    tDattorroReverb_setMix            (tDattorroReverb* const, float mix);
    void    tDattorroReverb_setFreeze         (tDattorroReverb* const rev, uint32_t freeze);
    void    tDattorroReverb_setHP             (tDattorroReverb* const, float freq);
    void    tDattorroReverb_setSize           (tDattorroReverb* const, float size);
    void    tDattorroReverb_setInputDelay     (tDattorroReverb* const, float preDelay);
    void    tDattorroReverb_setInputFilter    (tDattorroReverb* const, float freq);
    void    tDattorroReverb_setFeedbackFilter (tDattorroReverb* const, float freq);
    void    tDattorroReverb_setFeedbackGain   (tDattorroReverb* const, float gain);
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_REVERB_H_INCLUDED

//==============================================================================

