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
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    typedef struct _tSampleReducer
    {
        tMempool mempool;
        float invRatio;
        float hold;
        uint32_t count;
    } _tSampleReducer;
    
    typedef _tSampleReducer* tSampleReducer;
    
    void    tSampleReducer_init    (tSampleReducer* const);
    void    tSampleReducer_initToPool   (tSampleReducer* const, tMempool* const);
    void    tSampleReducer_free    (tSampleReducer* const);
    
    float   tSampleReducer_tick    (tSampleReducer* const, float input);
    
    // sampling ratio
    void    tSampleReducer_setRatio (tSampleReducer* const, float ratio);
    
    //==============================================================================
    
    typedef struct _tOversampler
    {
        tMempool mempool;
        int ratio;
        float* pCoeffs;
        float* upState;
        float* downState;
        int numTaps;
        int phaseLength;
    } _tOversampler;
    
    typedef _tOversampler* tOversampler;
    
    void    tOversampler_init           (tOversampler* const, int order, oBool extraQuality);
    void    tOversampler_initToPool     (tOversampler* const, int order, oBool extraQuality, tMempool* const);
    void    tOversampler_free           (tOversampler* const);
    
    void    tOversampler_upsample       (tOversampler* const, float input, float* output);
    float   tOversampler_downsample     (tOversampler* const os, float* input);
    float   tOversampler_tick           (tOversampler* const, float input, float* oversample, float (*effectTick)(float));
    int     tOversampler_getLatency     (tOversampler* const os);
    
    //==============================================================================
    
    /* tLockhartWavefolder */
    
    typedef struct _tLockhartWavefolder
    {
        tMempool mempool;
        
        double Ln1;
        double Fn1;
        double xn1;

        double RL;
        double R;
        double VT;
        double Is;

        double a;
        double b;
        double d;

        // Antialiasing error threshold
        double AAthresh;
        double half_a;
        double longthing;

        double LambertThresh, w, expw, p, r, s, myerr;
        double l;
        double u, Ln, Fn;
        double tempsDenom;
        double tempErrDenom;
        double tempOutDenom;

    } _tLockhartWavefolder;
    
    typedef _tLockhartWavefolder* tLockhartWavefolder;
    
    void    tLockhartWavefolder_init    (tLockhartWavefolder* const);
    void    tLockhartWavefolder_initToPool   (tLockhartWavefolder* const, tMempool* const);
    void    tLockhartWavefolder_free    (tLockhartWavefolder* const);
    
    float   tLockhartWavefolder_tick    (tLockhartWavefolder* const, float samp);

    //==============================================================================

    typedef struct _tCrusher
    {
        tMempool mempool;
        
        float srr;
        float mult, div;
        float rnd;
        
        uint32_t  op; //which bitwise operation (0-7)
        
        float gain;
        tSampleReducer sReducer;
        
    } _tCrusher;
    
    typedef _tCrusher* tCrusher;
    
    void    tCrusher_init    (tCrusher* const);
    void    tCrusher_initToPool   (tCrusher* const, tMempool* const);
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

    
#ifdef __cplusplus
}
#endif

#endif // LEAF_DISTORTION_H_INCLUDED

//==============================================================================


