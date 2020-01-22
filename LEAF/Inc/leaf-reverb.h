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
#include "leaf-delay.h"
#include "leaf-filters.h"
#include "leaf-oscillators.h"
    
    //==============================================================================
    
    /* PRCReverb: Reverb, reimplemented from STK (Cook and Scavone). */
    typedef struct _tPRCReverb
    {
        float mix, t60;
        
        float inv_441;
        
        tDelay allpassDelays[2];
        tDelay combDelay;
        float allpassCoeff;
        float combCoeff;
        
        float lastIn, lastOut;
        
    } _tPRCReverb;
    
    typedef _tPRCReverb* tPRCReverb;
    
    void    tPRCReverb_init    (tPRCReverb* const, float t60);
    void    tPRCReverb_free    (tPRCReverb* const);
    
    float   tPRCReverb_tick    (tPRCReverb* const, float input);
    
    // Set reverb time in seconds.
    void    tPRCReverb_setT60  (tPRCReverb* const, float t60);
    
    // Set mix between dry input and wet output signal.
    void    tPRCReverb_setMix  (tPRCReverb* const, float mix);
    
    //==============================================================================
    
    /* NReverb: Reverb, reimplemented from STK (Cook and Scavone). */
    typedef struct _tNReverb
    {
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
    
    void    tNReverb_init      (tNReverb* const, float t60);
    void    tNReverb_free      (tNReverb* const);
    
    float   tNReverb_tick      (tNReverb* const, float input);
    void   tNReverb_tickStereo(tNReverb* const rev, float input, float* output);
    
    // Set reverb time in seconds.
    void    tNReverb_setT60    (tNReverb* const, float t60);
    
    // Set mix between dry input and wet output signal.
    void    tNReverb_setMix    (tNReverb*  const, float mix);
    
    //==============================================================================
    
    typedef struct _tDattorroReverb
    {
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
    void    tDattorroReverb_free              (tDattorroReverb* const);
    
    float   tDattorroReverb_tick              (tDattorroReverb* const, float input);
    void    tDattorroReverb_tickStereo        (tDattorroReverb* const rev, float input, float* output);
    void    tDattorroReverb_setMix            (tDattorroReverb* const, float mix);
    void    tDattorroReverb_setFreeze            (tDattorroReverb* const rev, uint32_t freeze);
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

