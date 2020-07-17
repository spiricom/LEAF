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
     
     @fn void    tPRCReverb_init         (tPRCReverb* const, float t60)
     @brief
     @param
     
     @fn void    tPRCReverb_initToPool   (tPRCReverb* const, float t60, tMempool* const)
     @brief
     @param
     
     @fn void    tPRCReverb_free         (tPRCReverb* const)
     @brief
     @param
     
     @fn void    tPRCReverb_clear        (tPRCReverb* const)
     @brief
     @param
     
     @fn float   tPRCReverb_tick         (tPRCReverb* const, float input)
     @brief
     @param

     @fn void    tPRCReverb_setT60       (tPRCReverb* const, float t60)
     @brief Set reverb time in seconds.
     @param
    
     @fn void    tPRCReverb_setMix       (tPRCReverb* const, float mix)
     @brief Set mix between dry input and wet output signal.
     @param
     
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
    
    void    tPRCReverb_init         (tPRCReverb* const, float t60);
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
     
     @fn void    tNReverb_init           (tNReverb* const, float t60)
     @brief
     @param
     
     @fn void    tNReverb_initToPool     (tNReverb* const, float t60, tMempool* const)
     @brief
     @param
     
     @fn void    tNReverb_free           (tNReverb* const)
     @brief
     @param
     
     @fn void    tNReverb_clear          (tNReverb* const)
     @brief
     @param
     
     @fn float   tNReverb_tick           (tNReverb* const, float input)
     @brief
     @param
     
     @fn void    tNReverb_tickStereo     (tNReverb* const rev, float input, float* output)
     @brief
     @param
     
     @fn void    tNReverb_setT60         (tNReverb* const, float t60)
     @brief Set reverb time in seconds.
     @param
     
     @fn void    tNReverb_setMix         (tNReverb* const, float mix)
     @brief Set mix between dry input and wet output signal.
     @param
     
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
    
    void    tNReverb_init           (tNReverb* const, float t60);
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
     
     @fn void    tDattorroReverb_init              (tDattorroReverb* const)
     @brief
     @param
     
     @fn void    tDattorroReverb_initToPool        (tDattorroReverb* const, tMempool* const)
     @brief
     @param
     
     @fn void    tDattorroReverb_free              (tDattorroReverb* const)
     @brief
     @param
     
     @fn void    tDattorroReverb_clear             (tDattorroReverb* const)
     @brief
     @param
     
     @fn float   tDattorroReverb_tick              (tDattorroReverb* const, float input)
     @brief
     @param
     
     @fn void    tDattorroReverb_tickStereo        (tDattorroReverb* const rev, float input, float* output)
     @brief
     @param
     
     @fn void    tDattorroReverb_setMix            (tDattorroReverb* const, float mix)
     @brief
     @param
     
     @fn void    tDattorroReverb_setFreeze         (tDattorroReverb* const rev, uint32_t freeze)
     @brief
     @param
     
     @fn void    tDattorroReverb_setHP             (tDattorroReverb* const, float freq)
     @brief
     @param
     
     @fn void    tDattorroReverb_setSize           (tDattorroReverb* const, float size)
     @brief
     @param
     
     @fn void    tDattorroReverb_setInputDelay     (tDattorroReverb* const, float preDelay)
     @brief
     @param
     
     @fn void    tDattorroReverb_setInputFilter    (tDattorroReverb* const, float freq)
     @brief
     @param
     
     @fn void    tDattorroReverb_setFeedbackFilter (tDattorroReverb* const, float freq)
     @brief
     @param
     
     @fn void    tDattorroReverb_setFeedbackGain   (tDattorroReverb* const, float gain)
     @brief
     @param
     
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
    
    void    tDattorroReverb_init              (tDattorroReverb* const);
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

