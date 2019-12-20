/*==============================================================================
 
 leaf-distortion.h
 Created: 25 Oct 2019 10:23:28am
 Author:  Matthew Wang
 
 ==============================================================================*/

#ifndef LEAF_DISTORTION_H_INCLUDED
#define LEAF_DISTORTION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-global.h"
#include "leaf-mempool.h"
#include "leaf-math.h"
    
    //==============================================================================
    
    /* tLockhartWavefolder */
    
    typedef struct _tLockhartWavefolder
    {
        double Ln1;
        double Fn1;
        float xn1;
        
    } _tLockhartWavefolder;
    
    typedef _tLockhartWavefolder* tLockhartWavefolder;
    
    void    tLockhartWavefolder_init    (tLockhartWavefolder* const);
    void    tLockhartWavefolder_free    (tLockhartWavefolder* const);
    
    float   tLockhartWavefolder_tick    (tLockhartWavefolder* const, float samp);
    
    //==============================================================================
    
    typedef struct _tCrusher
    {
        float srr;
        float mult, div;
        float rnd;
        
        uint32_t  op; //which bitwise operation (0-7)
        
        float gain;
        
    } _tCrusher;
    
    typedef _tCrusher* tCrusher;
    
    void    tCrusher_init    (tCrusher* const);
    void    tCrusher_free    (tCrusher* const);
    
    float   tCrusher_tick    (tCrusher* const, float input);
    
    // 0.0 - 1.0
    void    tCrusher_setOperation (tCrusher* const, float op);
    
    // 0.0 - 1.0
    void    tCrusher_setQuality (tCrusher* const, float val);
    
    // what division to round to
    void    tCrusher_setRound (tCrusher* const, float rnd);
    
    // sampling ratio
    void    tCrusher_setSamplingRatio (tCrusher* const, float ratio);
    
    //==============================================================================
    
    typedef struct _tOversampler
    {
        int ratio;
        float* pCoeffs;
        float* upState;
        float* downState;
        int numTaps;
        int phaseLength;
    } tOversampler;
    
//    typedef _tOversampler* tOversampler;
    
    void        tOversampler_init(tOversampler* const, int order, oBool extraQuality);
    void        tOversampler_free(tOversampler* const);
    void        tOversampler_upsample(tOversampler* const, float input, float* output);
    float       tOversampler_downsample(tOversampler* const os, float* input);
    float       tOversampler_tick(tOversampler* const, float input, float (*effectTick)(float));
    int         tOversampler_getLatency(tOversampler* const os);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_DISTORTION_H_INCLUDED

//==============================================================================


