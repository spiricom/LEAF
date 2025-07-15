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
#include "leaf-filters.h"
    
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

     The SampleReducer outputs each incoming sample and then “holds” it for a number of subsequent calls
    determined by the reduction ratio (0.0–1.0):
    A ratio of 1.0 means no reduction (pass-through)
    A ratio of 0.5 outputs the new input only every two calls, holding the previous value in between
    Lower ratios produce more aggressive sample-rate reduction and the bitcrush effect

    Example
    @code{.c}
    // initialize
    tSampleReducer* sr = NULL;
    tSampleReducer_init(&sr, leaf);

    // set when to output new sample by calls
    tSampleReducer_setRatio(sr, 0.25f);

    //audio loop
    for (int i = 0; i < numSamples; ++i) {
        float out = tSampleReducer_tick(sr, inputSamples[i]);
        outputSamples[i] = out;
    }

    //when done
    tSampleReducer_free(&sr);
    @endcode
     
     @fn void    tSampleReducer_init(tSampleReducer** const, LEAF* const leaf)
     @brief Initialize a tSampleReducer to the default mempool of a LEAF instance.
     @param reducer A pointer to the tSampleReducer to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSampleReducer_initToPool(tSampleReducer** const, tMempool** const)
     @brief Initialize a tSampleReducer to a specified mempool.
     @param reducer A pointer to the tSampleReducer to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSampleReducer_free(tSampleReducer** const)
     @brief Free a tSampleReducer from its mempool.
     @param reducer A pointer to the tSampleReducer to free.
     
     @fn Lfloat   tSampleReducer_tick    (tSampleReducer* const, Lfloat input)
     @brief
     @param reducer A pointer to the relevant tSampleReducer.
     
     @fn void    tSampleReducer_setRatio (tSampleReducer* const, Lfloat ratio)
     @brief
     @param reducer A pointer to the relevant tSampleReducer.
     ￼￼￼
     @} */
    
    typedef struct tSampleReducer
    {
        tMempool* mempool;
        Lfloat invRatio;
        Lfloat hold;
        uint32_t count;
    } tSampleReducer;

    void    tSampleReducer_init       (tSampleReducer** const, LEAF* const leaf);
    void    tSampleReducer_initToPool (tSampleReducer** const, tMempool** const);
    void    tSampleReducer_free       (tSampleReducer** const);
    
    Lfloat  tSampleReducer_tick       (tSampleReducer* const, Lfloat input);
    void    tSampleReducer_setRatio   (tSampleReducer* const, Lfloat ratio);
    
    //==============================================================================
    
    /*!
     @defgroup toversampler tOversampler
     @ingroup distortion
     @brief Oversampler.

    The Oversampler upsamples each input sample by integer `ratio`,
    applies a user‐supplied effect at the higher rate, then downsamples back to the
    original rate.  This reduces aliasing and improves quality for nonlinear processing

    Example
    @code{.c}
    //initialize
    tOversampler* os = NULL;
    tOversampler_init(&os,
                      8,    // filter order (taps per phase)
                      1,    // extraQuality flag
                      leaf);

    //set oversampling
    tOversampler_setRatio(os, 4);

    //audio loop
    float oversampleBuf[4];
    for (int i = 0; i < numSamples; ++i) {
        float out = tOversampler_tick(os,
                                      inputSamples[i],
                                      oversampleBuf,
                                      myDistortion);
        outputSamples[i] = out;
    }

    //when done
    tOversampler_free(&os);
    @endcode
     
     @fn void    tOversampler_init(tOversampler** const, int order, int extraQuality, LEAF* const leaf)
     @brief Initialize a tOversampler to the default mempool of a LEAF instance.
     @param oversampler A pointer to the tOversampler to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tOversampler_initToPool(tOversampler** const, int order, int extraQuality, tMempool** const)
     @brief Initialize a tOversampler to a specified mempool.
     @param oversampler A pointer to the tOversampler to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tOversampler_free(tOversampler** const)
     @brief Free a tOversampler from its mempool.
     @param oversampler A pointer to the tOversampler to free.
     
     @fn void    tOversampler_upsample       (tOversampler* const, Lfloat input, Lfloat* output)
     @brief
     @param oversampler A pointer to the relevant tOversampler.
     
     @fn Lfloat   tOversampler_downsample     (tOversampler* const os, Lfloat* input)
     @brief
     @param oversampler A pointer to the relevant tOversampler.
     
     @fn Lfloat   tOversampler_tick           (tOversampler* const, Lfloat input, Lfloat* oversample, Lfloat (*effectTick)(Lfloat))
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
    
    typedef struct tOversampler
    {
        tMempool* mempool;
        uint32_t maxRatio;
        uint8_t allowHighQuality;
        uint32_t ratio;
        uint32_t offset;
        Lfloat* pCoeffs;
        Lfloat* upState;
        Lfloat* downState;
        uint32_t numTaps;
        uint32_t phaseLength;
    } tOversampler;

    void    tOversampler_init           (tOversampler** const, int order, int extraQuality, LEAF* const leaf);
    void    tOversampler_initToPool     (tOversampler** const, int order, int extraQuality, tMempool** const);
    void    tOversampler_free           (tOversampler** const);

    Lfloat  tOversampler_tick           (tOversampler* const, Lfloat input, Lfloat* oversample, Lfloat (*effectTick)(Lfloat));

    void    tOversampler_upsample       (tOversampler* const, Lfloat input, Lfloat* output);
    Lfloat  tOversampler_downsample     (tOversampler* const, Lfloat* input);
    void    tOversampler_setRatio       (tOversampler* const, int ratio);
    void    tOversampler_setQuality     (tOversampler* const, int quality);
    int     tOversampler_getLatency     (tOversampler* const);
    
    //==============================================================================
/*!
 @defgroup tWavefolder tWavefolder
 @ingroup distortion
 @brief more efficient and simpler wavefolder.

 Example
@code{.c}
//initialize
tWavefolder* wf = NULL;
tWavefolder_init(&wf,
                 1.0f,   // feed-forward gain
                 0.5f,   // feedback gain
                 0.8f,   // fold depth limit
                 leaf);

//audio loop
for (int i = 0; i < numSamples; ++i) {
     float out = tWavefolder_tick(wf, inputSamples[i]);
     outputSamples[i] = out;
}

//tweak parameters
tWavefolder_setGain(wf, 2.0f);
tWavefolder_setOffset(wf, 0.1f);

//when done
tWavefolder_free(&wf);
@endcode
 
 @fn void    tWavefolder_init(tWavefolder** const, LEAF* const leaf)
 @brief Initialize a tWavefolder to the default mempool of a LEAF instance.
 @param wavefolder A pointer to the tWavefolder to initialize.
 @param leaf A pointer to the leaf instance.
 
 @fn void    tWavefolder_initToPool(tWavefolder** const, tMempool** const)
 @brief Initialize a tWavefolder to a specified mempool.
 @param wavefolder A pointer to the tWavefolder to initialize.
 @param mempool A pointer to the tMempool to use.
 
 @fn void    tWavefolder_free(tWavefolder** const)
 @brief Free a tWavefolder from its mempool.
 @param wavefolder A pointer to the tWavefolder to free.
 
 @fn Lfloat   tWavefolder_tick    (tWavefolder* const, Lfloat samp)
 @brief
 @param wavefolder A pointer to the relevant tWavefolder.
 ￼￼￼
 @} */

typedef struct tWavefolder
{

    tMempool* mempool;
    
    Lfloat FBsample;
    Lfloat gain;
    Lfloat offset;
    Lfloat foldDepth;
    Lfloat FBAmount;
    Lfloat invFBAmount;
    Lfloat FFAmount;
    tHighpass*  dcBlock;
} tWavefolder;

void    tWavefolder_init         (tWavefolder** const, Lfloat ffAmount, Lfloat fbAmount, Lfloat foldDepth, LEAF* const leaf);
void    tWavefolder_initToPool   (tWavefolder** const, Lfloat ffAmount, Lfloat fbAmount, Lfloat foldDepth, tMempool** const);
void    tWavefolder_free         (tWavefolder** const);

Lfloat  tWavefolder_tick         (tWavefolder* const, Lfloat samp);

void    tWavefolder_setFFAmount  (tWavefolder* const wf, Lfloat ffAmount);
void    tWavefolder_setFBAmount  (tWavefolder* const wf, Lfloat fbAmount);
void    tWavefolder_setFoldDepth (tWavefolder* const wf, Lfloat foldDepth);
void    tWavefolder_setOffset    (tWavefolder* const wf, Lfloat offset);
void    tWavefolder_setGain      (tWavefolder* const wf, Lfloat gain);


//==============================================================================
    
    /*!
     @defgroup tlockhartwavefolder tLockhartWavefolder
     @ingroup distortion
     @brief Analog model wavefolder.

    Example
    @code{.c}
    // initialize
    tLockhartWavefolder* lfw = NULL;
    tLockhartWavefolder_init(&lfw, leaf);

    //audio loop
    for (int i = 0; i < numSamples; ++i) {
        float out = tLockhartWavefolder_tick(lfw, inputSamples[i]);
        outputSamples[i] = out;
    }

    //when done
    tLockhartWavefolder_free(&lfw);
    @endcode
     
     @fn void    tLockhartWavefolder_init(tLockhartWavefolder** const, LEAF* const leaf)
     @brief Initialize a tLockhartWavefolder to the default mempool of a LEAF instance.
     @param wavefolder A pointer to the tLockhartWavefolder to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tLockhartWavefolder_initToPool(tLockhartWavefolder** const, tMempool** const)
     @brief Initialize a tLockhartWavefolder to a specified mempool.
     @param wavefolder A pointer to the tLockhartWavefolder to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tLockhartWavefolder_free(tLockhartWavefolder** const)
     @brief Free a tLockhartWavefolder from its mempool.
     @param wavefolder A pointer to the tLockhartWavefolder to free.
     
     @fn Lfloat   tLockhartWavefolder_tick    (tLockhartWavefolder* const, Lfloat samp)
     @brief
     @param wavefolder A pointer to the relevant tLockhartWavefolder.
     ￼￼￼
     @} */
    
    typedef struct tLockhartWavefolder
    {

        tMempool* mempool;
        
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

    } tLockhartWavefolder;

    void    tLockhartWavefolder_init       (tLockhartWavefolder** const, LEAF* const leaf);
    void    tLockhartWavefolder_initToPool (tLockhartWavefolder** const, tMempool** const);
    void    tLockhartWavefolder_free       (tLockhartWavefolder** const);
    
    Lfloat  tLockhartWavefolder_tick       (tLockhartWavefolder* const, Lfloat samp);

    //==============================================================================

    /*!
     @defgroup tcrusher tCrusher
     @ingroup distortion
     @brief Bit depth and sampler rate degrader.

    Example
    @code{.c}
    // initialize
    tCrusher* cb = NULL;
    tCrusher_init(&cb, leaf);

    // configure bitcrush
    tCrusher_setOperation(cb, 4.0f);
    tCrusher_setQuality(cb, 0.5f);
    tCrusher_setRound(cb, 0.1f);
    tCrusher_setSamplingRatio(cb, 0.25f);

    //audio loop
    for (int i = 0; i < numSamples; ++i) {
        float out = tCrusher_tick(cb, inputSamples[i]);
        outputSamples[i] = out;
    }

    //when done
    tCrusher_free(&cb);
    @endcode
     
     @fn void    tCrusher_init(tCrusher** const, LEAF* const leaf)
     @brief Initialize a tCrusher to the default mempool of a LEAF instance.
     @param crusher A pointer to the tCrusher to initialize.
     @param leaf A pointer to the leaf instance.f
     
     @fn void    tCrusher_initToPool(tCrusher** const, tMempool** const)
     @brief Initialize a tCrusher to a specified mempool.
     @param crusher A pointer to the tCrusher to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tCrusher_free(tCrusher** const)
     @brief Free a tCrusher from its mempool.
     @param crusher A pointer to the tCrusher to free.
     
     @fn Lfloat   tCrusher_tick    (tCrusher* const, Lfloat input)
     @brief
     @param crusher A pointer to the relevant tCrusher.
     
     @fn void    tCrusher_setOperation (tCrusher* const, Lfloat op)
     @brief
     @param crusher A pointer to the relevant tCrusher.
     @param operation 0.0 - 1.0
     
     @fn void    tCrusher_setQuality (tCrusher* const, Lfloat val)
     @brief
     @param crusher A pointer to the relevant tCrusher.
     @param quality 0.0 - 1.0
     
     @fn void    tCrusher_setRound (tCrusher* const, Lfloat rnd)
     @brief
     @param crusher A pointer to the relevant tCrusher.
     @param what division to round to
     
     @fn void    tCrusher_setSamplingRatio (tCrusher* const, Lfloat ratio)
     @brief
     @param crusher A pointer to the relevant tCrusher.
     @param sampling ratio
     ￼￼￼
     @} */
    
    typedef struct tCrusher
    {

        tMempool* mempool;
        
        Lfloat srr;
        Lfloat mult, div;
        Lfloat rnd;
        
        uint32_t  op; //which bitwise operation (0-7)
        
        Lfloat gain;
        tSampleReducer*  sReducer;
        
    } tCrusher;

    void    tCrusher_init             (tCrusher** const, LEAF* const leaf);
    void    tCrusher_initToPool       (tCrusher** const, tMempool** const);
    void    tCrusher_free             (tCrusher** const);
    
    Lfloat  tCrusher_tick             (tCrusher* const, Lfloat input);

    void    tCrusher_setOperation     (tCrusher* const, Lfloat op);
    void    tCrusher_setQuality       (tCrusher* const, Lfloat val);
    void    tCrusher_setRound         (tCrusher* const, Lfloat rnd);
    void    tCrusher_setSamplingRatio (tCrusher* const, Lfloat ratio);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_DISTORTION_H_INCLUDED

//==============================================================================


