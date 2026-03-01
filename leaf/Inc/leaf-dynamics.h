/*==============================================================================
 
 leaf-dynamics.h
 Created: 30 Nov 2018 11:57:05am
 Author:  airship
 
 ==============================================================================*/

#ifndef LEAF_DYNAMICS_H_INCLUDED
#define LEAF_DYNAMICS_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-global.h"
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-analysis.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    /*!
     @defgroup tcompressor tCompressor
     @ingroup dynamics
     @brief Standard compressor.

    Example
    @code{.c}
    //initialize
    tCompressor* comp = NULL;
    tCompressor_init(&comp, leaf);

    //configure parameters
    tCompressor_setParams(comp,
                          -12.0f,  //threshold (dB)
                          4.0f,    //compression ratio
                          6.0f,    //knee width (dB)
                          3.0f,    //makeup gain (dB)
                          0.010f,  //attack time (s)
                          0.100f); //release time (s)

    //audio loop
    for (int i = 0; i < numSamples; ++i) {
        float in  = inputSamples[i];
        // tick processes the sample and applies compression
        float out = tCompressor_tick(comp, in);
        outputSamples[i] = out;
    }

    //tweak parameters
    tCompressor_setParams(comp,
                          -6.0f,  //new threshold
                          4.0f,
                          6.0f,
                          3.0f,
                          0.010f,
                          0.100f);

    //when done
    tCompressor_free(&comp);
    @endcode

     @fn void    tCompressor_init(tCompressor** const, LEAF* const leaf)
     @brief Initialize a tCompressor to the default mempool of a LEAF instance.
     @param compressor A pointer to the tCompressor to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tCompressor_initToPool(tCompressor** const, tMempool** const)
     @brief Initialize a tCompressor to a specified mempool.
     @param compressor A pointer to the tCompressor to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tCompressor_free(tCompressor** const)
     @brief Free a tCompressor from its mempool.
     @param compressor A pointer to the tCompressor to free.
     
     @fn Lfloat   tCompressor_tick        (tCompressor* const, Lfloat input)
     @brief
     @param compressor A pointer to the relevant tCompressor.
     
     @} */
   
    typedef struct tCompressor
    {
        
        tMempool* mempool;
        
        Lfloat tauAttack, tauRelease;
        Lfloat T, R, W, M; // Threshold, compression Ratio, decibel Width of knee transition, decibel Make-up gain
        Lfloat invR;
        Lfloat inv4W;
        Lfloat x_G[2], y_G[2], x_T[2], y_T[2];
        
        int isActive;
        
        Lfloat sampleRate;
        Lfloat* atodbTable;
        Lfloat* dbtoaTable;
        Lfloat atodbScalar;
        Lfloat dbtoaScalar;
        Lfloat atodbOffset;
        Lfloat dbtoaOffset;
        int atodbTableSizeMinus1;
        int dbtoaTableSizeMinus1;
        
    } tCompressor;

    void    tCompressor_init                  (tCompressor** const, LEAF* const leaf);
    void    tCompressor_initToPool            (tCompressor** const, tMempool** const);
    void    tCompressor_free                  (tCompressor** const);
    
    Lfloat  tCompressor_tick                  (tCompressor* const, Lfloat input);
    Lfloat  tCompressor_tickWithTable         (tCompressor* const comp, Lfloat in);
    Lfloat  tCompressor_tickWithTableHardKnee (tCompressor* const comp, Lfloat in);

    void    tCompressor_setTables             (tCompressor* const comp, Lfloat* atodb, Lfloat* dbtoa, Lfloat atodbMinIn, Lfloat atodbMaxIn, Lfloat dbtoaMinIn, Lfloat dbtoaMaxIn, int atodbTableSize, int dbtoaTableSize);
    void    tCompressor_setParams             (tCompressor* const comp, Lfloat thresh, Lfloat ratio, Lfloat knee, Lfloat makeup, Lfloat attack, Lfloat release);
    void    tCompressor_setSampleRate         (tCompressor* const comp, Lfloat sampleRate);
    
    /*!
     @defgroup tfeedbackleveler tFeedbackLeveler
     @ingroup dynamics
     @brief An auto VCA that you put into a feedback circuit to make it stay at the same level.
     @details An auto VCA that you put into a feedback circuit to make it stay at the same level. It can enforce level bidirectionally (amplifying and attenuating as needed) or just attenutating. The former option allows for infinite sustain strings, for example, while the latter option allows for decaying strings, which can never exceed a specific level.

    Example
    @code{.c}
    //initialize
    tFeedbackLeveler* fl = NULL;
    tFeedbackLeveler_init(&fl,
                          0.1f,   //targetLevel (linear scale)
                          1.0f,   //factor for smoothing
                          0.5f,   //strength of correction
                          1,      //mode: 0 = attenuate only, 1 = bidirectional
                          leaf);

    //audio loop
    float fbBuffer[1024] = {0};
    int idx = 0;

    for (int n = 0; n < totalSamples; ++n) {
        float input = inputSamples[n];
        // apply leveler before feeding back
        float leveled = tFeedbackLeveler_tick(fl, fbBuffer[idx]);
        //mix leveled feedback with new input
        float out = input + leveled;
        outputSamples[n] = out;
        //write back into circular feedback buffer
        fbBuffer[idx] = out;
        idx = (idx + 1) & 1023;
    }

    //monitor current gain applied
    float currentGain = tFeedbackLeveler_sample(fl);

    //tweak target level
    tFeedbackLeveler_setTargetLevel(fl, 0.2f);

    //when done
    tFeedbackLeveler_free(&fl);
    @endcode

     @fn void tFeedbackLeveler_init(tFeedbackLeveler** const, Lfloat targetLevel, Lfloat factor, Lfloat strength, int mode, LEAF* const leaf)
     @brief Initialize a tFeedbackLeveler to the default mempool of a LEAF instance.
     @param leveler A pointer to the tFeedbackLeveler to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void tFeedbackLeveler_initToPool(tFeedbackLeveler** const, Lfloat targetLevel, Lfloat factor, Lfloat strength, int mode, tMempool** const)
     @brief Initialize a tFeedbackLeveler to a specified mempool.
     @param leveler A pointer to the tFeedbackLeveler to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tFeedbackLeveler_free(tFeedbackLeveler** const)
     @brief Free a tFeedbackLeveler from its mempool.
     @param leveler A pointer to the tFeedbackLeveler to free.
     
     @fn Lfloat   tFeedbackLeveler_tick           (tFeedbackLeveler* const, Lfloat input)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     
     @fn Lfloat   tFeedbackLeveler_sample         (tFeedbackLeveler* const)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     
     @fn void    tFeedbackLeveler_setTargetLevel (tFeedbackLeveler* const, Lfloat TargetLevel)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     
     @fn void    tFeedbackLeveler_setFactor      (tFeedbackLeveler* const, Lfloat factor)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     
     @fn void    tFeedbackLeveler_setMode        (tFeedbackLeveler* const, int mode)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     @param mode 0 for upwards limiting only, 1 for biderctional limiting
    
     @fn void    tFeedbackLeveler_setStrength    (tFeedbackLeveler* const, Lfloat strength)
     @brief
     @param leveler A pointer to the relevant tFeedbackLeveler.
     
     @} */
    
    typedef struct tFeedbackLeveler
    {

        tMempool* mempool;
        Lfloat targetLevel;    // target power level
        Lfloat strength;        // how strongly level difference affects the VCA
        int      mode;            // 0 for upwards limiting only, 1 for biderctional limiting
        Lfloat curr;
        tPowerFollower* pwrFlw;    // internal power follower needed for level tracking
        
    } tFeedbackLeveler;

    void    tFeedbackLeveler_init           (tFeedbackLeveler** const, Lfloat targetLevel, Lfloat factor, Lfloat strength, int mode, LEAF* const leaf);
    void    tFeedbackLeveler_initToPool     (tFeedbackLeveler** const, Lfloat targetLevel, Lfloat factor, Lfloat strength, int mode, tMempool** const);
    void    tFeedbackLeveler_free           (tFeedbackLeveler** const);
    
    Lfloat  tFeedbackLeveler_tick           (tFeedbackLeveler* const, Lfloat input);

    Lfloat  tFeedbackLeveler_sample         (tFeedbackLeveler* const);
    void    tFeedbackLeveler_setTargetLevel (tFeedbackLeveler* const, Lfloat TargetLevel);
    void    tFeedbackLeveler_setFactor      (tFeedbackLeveler* const, Lfloat factor);
    void    tFeedbackLeveler_setMode        (tFeedbackLeveler* const, int mode); // 0 for upwards limiting only, 1 for biderctional limiting
    void    tFeedbackLeveler_setStrength    (tFeedbackLeveler* const, Lfloat strength);
    
    //==============================================================================
    

    /*!
     @defgroup tthreshold tThreshold
     @ingroup dynamics
     @brief Threshold with hysteresis (like Max/MSP thresh~ object)

    Example
    @code{.c}
    //initialize
    tThreshold* th = NULL;
    tThreshold_init(&th,
                    0.2f,   //low threshold
                    0.8f,   //high threshold
                    leaf);

    //audio loop
    for (int i = 0; i < numSamples; ++i) {
        float in = inputSamples[i];
        int state = tThreshold_tick(th, in);
        if (state) {
            //above hysteresis upper bound
            outputSamples[i] = 1.0f;
        } else {
            //below hysteresis lower bound
            outputSamples[i] = 0.0f;
        }
    }

    //tweak parameters
    tThreshold_setLow(th, 0.3f);
    tThreshold_setHigh(th, 0.7f);

    //when done
    tThreshold_free(&th);
    @endcode

     
     @fn void    tThreshold_init(tThreshold** const, Lfloat low, Lfloat high, LEAF* const leaf)
     @brief Initialize a tThreshold to the default mempool of a LEAF instance.
     @param threshold A pointer to the tThreshold to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tThreshold_initToPool(tThreshold** const, Lfloat low, Lfloat high, tMempool** const)
     @brief Initialize a tThreshold to a specified mempool.
     @param threshold A pointer to the tThreshold to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tThreshold_free(tThreshold** const)
     @brief Free a tThreshold from its mempool.
     @param threshold A pointer to the tThreshold to free.
     
     @fn int   tThreshold_tick        (tThreshold* const, Lfloat input)
     @brief
     @param threshold A pointer to the relevant tThreshold.
     
     @fn void   tThreshold_setLow        (tThreshold* const, Lfloat low)
     @brief
     @param threshold A pointer to the relevant tThreshold.
     
     @fn void   tThreshold_setHigh       (tThreshold* const, Lfloat high)
     @brief
     @param threshold A pointer to the relevant tThreshold.
     
     @} */

    typedef struct tThreshold
    {

        tMempool* mempool;
        Lfloat highThresh, lowThresh;
		int currentValue;
    } tThreshold;

    void    tThreshold_init        (tThreshold** const, Lfloat low, Lfloat high, LEAF* const leaf);
    void    tThreshold_initToPool  (tThreshold** const, Lfloat low, Lfloat high, tMempool** const);
    void    tThreshold_free        (tThreshold** const);

    int    tThreshold_tick         (tThreshold* const, Lfloat input);

    void   tThreshold_setLow       (tThreshold* const, Lfloat low);
    void   tThreshold_setHigh      (tThreshold* const, Lfloat high);


    //////======================================================================

#ifdef __cplusplus
}
#endif

#endif // LEAF_DYNAMICS_H_INCLUDED

//==============================================================================

