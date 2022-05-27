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
     @brief Sample rate reducer.
     @{
     
     @fn void    tSampleReducer_init    (tSampleReducer* const, LEAF* const leaf)
     @brief Initialize a tSampleReducer to the default mempool of a LEAF instance.
     @param reducer A pointer to the tSampleReducer to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSampleReducer_initToPool   (tSampleReducer* const, tMempool* const)
     @brief Initialize a tSampleReducer to a specified mempool.
     @param reducer A pointer to the tSampleReducer to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSampleReducer_free    (tSampleReducer* const)
     @brief Free a tSampleReducer from its mempool.
     @param reducer A pointer to the tSampleReducer to free.
     
     @fn float   tSampleReducer_tick    (tSampleReducer* const, float input)
     @brief
     @param reducer A pointer to the relevant tSampleReducer.
     
     @fn void    tSampleReducer_setRatio (tSampleReducer* const, float ratio)
     @brief
     @param reducer A pointer to the relevant tSampleReducer.
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
    
    void    tSampleReducer_init    (tSampleReducer* const, LEAF* const leaf);
    void    tSampleReducer_initToPool   (tSampleReducer* const, tMempool* const);
    void    tSampleReducer_free    (tSampleReducer* const);
    
    float   tSampleReducer_tick    (tSampleReducer* const, float input);
    void    tSampleReducer_setRatio (tSampleReducer* const, float ratio);
    
    //==============================================================================
    
    /*!
     @defgroup toversampler tOversampler
     @ingroup distortion
     @brief Oversampler.
     @{
     
     @fn void    tOversampler_init           (tOversampler* const, int order, int extraQuality, LEAF* const leaf)
     @brief Initialize a tOversampler to the default mempool of a LEAF instance.
     @param oversampler A pointer to the tOversampler to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tOversampler_initToPool     (tOversampler* const, int order, int extraQuality, tMempool* const)
     @brief Initialize a tOversampler to a specified mempool.
     @param oversampler A pointer to the tOversampler to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tOversampler_free           (tOversampler* const)
     @brief Free a tOversampler from its mempool.
     @param oversampler A pointer to the tOversampler to free.
     
     @fn void    tOversampler_upsample       (tOversampler* const, float input, float* output)
     @brief
     @param oversampler A pointer to the relevant tOversampler.
     
     @fn float   tOversampler_downsample     (tOversampler* const os, float* input)
     @brief
     @param oversampler A pointer to the relevant tOversampler.
     
     @fn float   tOversampler_tick           (tOversampler* const, float input, float* oversample, float (*effectTick)(float))
     @brief
     @param oversampler A pointer to the relevant tOversampler.
     
     @fn void    tOversampler_setRatio       (tOversampler* const, int ratio)
     @brief
     @param oversampler A pointer to the relevant tOversampler.
     @param ratio
     
     @fn int     tOversampler_getLatency     (tOversampler* const os)
     @brief
     @param oversampler A pointer to the relevant tOversampler.
     ￼￼￼
     @} */
    
    typedef struct _tOversampler
    {
        tMempool mempool;
        int maxRatio;
        int allowHighQuality;
        int ratio;
        int offset;
        float* pCoeffs;
        float* upState;
        float* downState;
        int numTaps;
        int phaseLength;
    } _tOversampler;
    
    typedef _tOversampler* tOversampler;
    
    void    tOversampler_init           (tOversampler* const, int order, int extraQuality, LEAF* const leaf);
    void    tOversampler_initToPool     (tOversampler* const, int order, int extraQuality, tMempool* const);
    void    tOversampler_free           (tOversampler* const);
    
    void    tOversampler_upsample       (tOversampler* const, float input, float* output);
    float   tOversampler_downsample     (tOversampler* const, float* input);
    float   tOversampler_tick           (tOversampler* const, float input, float* oversample,
                                         float (*effectTick)(float));

    void    tOversampler_setRatio       (tOversampler* const, int ratio);
    void    tOversampler_setQuality     (tOversampler* const, int quality);
    int     tOversampler_getLatency     (tOversampler* const);
    
    //==============================================================================
    
    /*!
     @defgroup tlockhartwavefolder tLockhartWavefolder
     @ingroup distortion
     @brief Analog model wavefolder.
     @{
     
     @fn void    tLockhartWavefolder_init    (tLockhartWavefolder* const, LEAF* const leaf)
     @brief Initialize a tLockhartWavefolder to the default mempool of a LEAF instance.
     @param wavefolder A pointer to the tLockhartWavefolder to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLockhartWavefolder_initToPool   (tLockhartWavefolder* const, tMempool* const)
     @brief Initialize a tLockhartWavefolder to a specified mempool.
     @param wavefolder A pointer to the tLockhartWavefolder to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tLockhartWavefolder_free    (tLockhartWavefolder* const)
     @brief Free a tLockhartWavefolder from its mempool.
     @param wavefolder A pointer to the tLockhartWavefolder to free.
     
     @fn float   tLockhartWavefolder_tick    (tLockhartWavefolder* const, float samp)
     @brief
     @param wavefolder A pointer to the relevant tLockhartWavefolder.
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
    
    void    tLockhartWavefolder_init    (tLockhartWavefolder* const, LEAF* const leaf);
    void    tLockhartWavefolder_initToPool   (tLockhartWavefolder* const, tMempool* const);
    void    tLockhartWavefolder_free    (tLockhartWavefolder* const);
    
    float   tLockhartWavefolder_tick    (tLockhartWavefolder* const, float samp);

    //==============================================================================

    /*!
     @defgroup tcrusher tCrusher
     @ingroup distortion
     @brief Bit depth and sampler rate degrader.
     @{
     
     @fn void    tCrusher_init    (tCrusher* const, LEAF* const leaf)
     @brief Initialize a tCrusher to the default mempool of a LEAF instance.
     @param crusher A pointer to the tCrusher to initialize.
     @param leaf A pointer to the leaf instance.f
     
     @fn void    tCrusher_initToPool   (tCrusher* const, tMempool* const)
     @brief Initialize a tCrusher to a specified mempool.
     @param crusher A pointer to the tCrusher to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tCrusher_free    (tCrusher* const)
     @brief Free a tCrusher from its mempool.
     @param crusher A pointer to the tCrusher to free.
     
     @fn float   tCrusher_tick    (tCrusher* const, float input)
     @brief
     @param crusher A pointer to the relevant tCrusher.
     
     @fn void    tCrusher_setOperation (tCrusher* const, float op)
     @brief
     @param crusher A pointer to the relevant tCrusher.
     @param operation 0.0 - 1.0
     
     @fn void    tCrusher_setQuality (tCrusher* const, float val)
     @brief
     @param crusher A pointer to the relevant tCrusher.
     @param quality 0.0 - 1.0
     
     @fn void    tCrusher_setRound (tCrusher* const, float rnd)
     @brief
     @param crusher A pointer to the relevant tCrusher.
     @param what division to round to
     
     @fn void    tCrusher_setSamplingRatio (tCrusher* const, float ratio)
     @brief
     @param crusher A pointer to the relevant tCrusher.
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
    
    void    tCrusher_init    (tCrusher* const, LEAF* const leaf);
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


