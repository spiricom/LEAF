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
    
    /*!
     @defgroup tsamplereducer tSampleReducer
     @ingroup distortion
     @brief
     @{
     
     @fn void    tSampleReducer_init    (tSampleReducer* const)
     @brief
     @param
     
     @fn void    tSampleReducer_initToPool   (tSampleReducer* const, tMempool* const)
     @brief
     @param
     
     @fn void    tSampleReducer_free    (tSampleReducer* const)
     @brief
     @param
     
     @fn float   tSampleReducer_tick    (tSampleReducer* const, float input)
     @brief
     @param
     
     @fn void    tSampleReducer_setRatio (tSampleReducer* const, float ratio)
     @brief
     @param
     ￼￼￼
     @} */
    
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
    void    tSampleReducer_setRatio (tSampleReducer* const, float ratio);
    
    //==============================================================================
    
    /*!
     @defgroup toversampler tOversampler
     @ingroup distortion
     @brief
     @{
     
     @fn void    tOversampler_init           (tOversampler* const, int order, int extraQuality)
     @brief
     @param
     
     @fn void    tOversampler_initToPool     (tOversampler* const, int order, int extraQuality, tMempool* const)
     @brief
     @param
     
     @fn void    tOversampler_free           (tOversampler* const)
     @brief
     @param
     
     @fn void    tOversampler_upsample       (tOversampler* const, float input, float* output)
     @brief
     @param
     
     @fn float   tOversampler_downsample     (tOversampler* const os, float* input)
     @brief
     @param
     
     @fn float   tOversampler_tick           (tOversampler* const, float input, float* oversample, float (*effectTick)(float))
     @brief
     @param
     
     @fn int     tOversampler_getLatency     (tOversampler* const os)
     @brief
     @param
     ￼￼￼
     @} */
    
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
    
    void    tOversampler_init           (tOversampler* const, int order, int extraQuality);
    void    tOversampler_initToPool     (tOversampler* const, int order, int extraQuality, tMempool* const);
    void    tOversampler_free           (tOversampler* const);
    
    void    tOversampler_upsample       (tOversampler* const, float input, float* output);
    float   tOversampler_downsample     (tOversampler* const os, float* input);
    float   tOversampler_tick           (tOversampler* const, float input, float* oversample, float (*effectTick)(float));
    int     tOversampler_getLatency     (tOversampler* const os);
    
    //==============================================================================
    
    /*!
     @defgroup tlockhartwavefolder tLockhartWavefolder
     @ingroup distortion
     @brief
     @{
     
     @fn void    tLockhartWavefolder_init    (tLockhartWavefolder* const)
     @brief
     @param
     
     @fn void    tLockhartWavefolder_initToPool   (tLockhartWavefolder* const, tMempool* const)
     @brief
     @param
     
     @fn void    tLockhartWavefolder_free    (tLockhartWavefolder* const)
     @brief
     @param
     
     @fn float   tLockhartWavefolder_tick    (tLockhartWavefolder* const, float samp)
     @brief
     @param
     ￼￼￼
     @} */
    
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

    /*!
     @defgroup tcrusher tCrusher
     @ingroup distortion
     @brief
     @{
     
     @fn void    tCrusher_init    (tCrusher* const)
     @brief
     @param
     
     @fn void    tCrusher_initToPool   (tCrusher* const, tMempool* const)
     @brief
     @param
     
     @fn void    tCrusher_free    (tCrusher* const)
     @brief
     @param
     
     @fn float   tCrusher_tick    (tCrusher* const, float input)
     @brief
     @param
     
     @fn void    tCrusher_setOperation (tCrusher* const, float op)
     @brief
     @param 0.0 - 1.0
     
     @fn void    tCrusher_setQuality (tCrusher* const, float val)
     @brief
     @param 0.0 - 1.0
     
     @fn void    tCrusher_setRound (tCrusher* const, float rnd)
     @brief
     @param what division to round to
     
     @fn void    tCrusher_setSamplingRatio (tCrusher* const, float ratio)
     @brief
     @param sampling ratio
     ￼￼￼
     @} */
    
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
    void    tCrusher_setOperation (tCrusher* const, float op);
    void    tCrusher_setQuality (tCrusher* const, float val);
    void    tCrusher_setRound (tCrusher* const, float rnd);
    void    tCrusher_setSamplingRatio (tCrusher* const, float ratio);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_DISTORTION_H_INCLUDED

//==============================================================================


