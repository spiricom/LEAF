/*==============================================================================
 
 leaf-analysis.h
 Created: 25 Oct 2019 10:30:52am
 Author:  Matthew Wang
 
 ==============================================================================*/

#ifndef LEAF_ANALYSIS_H_INCLUDED
#define LEAF_ANALYSIS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-global.h"
#include "leaf-mempool.h"
#include "leaf-distortion.h"
#include "leaf-math.h"
#include "leaf-filters.h"
#include "leaf-envelopes.h"
#include "leaf-delay.h"
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    /*!
     @defgroup tenvelopefollower tEnvelopeFollower
     @ingroup analysis
     @brief Detects and returns the basic envelope of incoming audio data.
     @{
     
     @fn void    tEnvelopeFollower_init(tEnvelopeFollower** const follower, Lfloat attackThreshold, Lfloat decayCoeff, LEAF* const leaf)
     @brief Initialize a tEnvelopeFollower to the default mempool of a LEAF instance.
     @param follower A pointer to the tEnvelopeFollower to initialize.
     @param attackThreshold Amplitude threshold for determining an envelope onset. 0.0 to 1.0
     @param decayCoefficient Multiplier to determine the envelope rate of decay. 0.0 to 1.0, above 0.95 recommended.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tEnvelopeFollower_initToPool(tEnvelopeFollower** const follower, Lfloat attackThreshold, Lfloat decayCoeff, tMempool** const mempool)
     @brief Initialize a tEnvelopeFollower to a specified mempool.
     @param follower A pointer to the tEnvelopeFollower to initialize.
     @param attackThreshold Amplitude threshold for determining an envelope onset. 0.0 to 1.0
     @param decayCoefficient Multiplier to determine the envelope rate of decay. 0.0 to 1.0, above 0.95 recommended.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tEnvelopeFollower_free(tEnvelopeFollower** const follower)
     @brief Free a tEnvelopeFollower from its mempool.
     @param follower A pointer to the tEnvelopeFollower to free.
     
     @fn Lfloat   tEnvelopeFollower_tick          (tEnvelopeFollower* const follower, Lfloat input)
     @brief Tick the tEnvelopeFollower.
     @param follower A pointer to the relevant tEnvelopeFollower.
     @param input The input sample.
     @return The envelope value.
     
     @fn void    tEnvelopeFollower_setDecayCoefficient    (tEnvelopeFollower* const follower, Lfloat decayCoeff)
     @brief Set the envelope decay coefficient.
     @param follower A pointer to the relevant tEnvelopeFollower.
     @param decayCoefficient Multiplier to determine the envelope rate of decay. 0.0 to 1.0, above 0.95 recommended.
     
     @fn void    tEnvelopeFollower_setAttackThreshold  (tEnvelopeFollower* const follower, Lfloat attackThresh)
     @brief Set the envelope attack threshold.
     @param follower A pointer to the relevant tEnvelopeFollower.
     @param attackThreshold The new threshold to determine envelope onset as an amplitude from 0.0 to 1.0
     ￼￼￼
     @} */
    
    typedef struct tEnvelopeFollower
    {

        tMempool* mempool;
        Lfloat y;
        Lfloat a_thresh;
        Lfloat d_coeff;
        
    } tEnvelopeFollower;

    void    tEnvelopeFollower_init                 (tEnvelopeFollower** const follower, Lfloat attackThreshold, Lfloat decayCoefficient, LEAF* const leaf);
    void    tEnvelopeFollower_initToPool           (tEnvelopeFollower** const follower, Lfloat attackThreshold, Lfloat decayCoefficient, tMempool** const mempool);
    void    tEnvelopeFollower_free                 (tEnvelopeFollower** const follower);
    
    Lfloat  tEnvelopeFollower_tick                 (tEnvelopeFollower* const follower, Lfloat sample);

    void    tEnvelopeFollower_setDecayCoefficient  (tEnvelopeFollower* const follower, Lfloat decayCoefficient);
    void    tEnvelopeFollower_setAttackThreshold   (tEnvelopeFollower* const follower, Lfloat attackThreshold);
    
    /*!
     @defgroup tzerocrossingcounter tZeroCrossingCounter
     @ingroup analysis
     @brief Count the amount of zero crossings within a window of the input audio data
     @{
     
     @fn void    tZeroCrossingCounter_init(tZeroCrossingCounter** const counter, int maxWindowSize, LEAF* const leaf)
     @brief Initialize a tZeroCrossingCounter to the default mempool of a LEAF instance.
     @param counter A pointer to the tZeroCrossingCounter to initialize.
     @param maxWindowSize The max and initial size of the window.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tZeroCrossingCounter_initToPool(tZeroCrossingCounter** const counter, int maxWindowSize, tMempool** const mempool)
     @brief Initialize a tZeroCrossingCounter to a specified mempool.
     @param counter A pointer to the tZeroCrossingCounter to initialize.
     @param maxWindowSize The max and initial size of the window.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tZeroCrossingCounter_free(tZeroCrossingCounter** const counter)
     @brief Free a tZeroCrossingCounter from its mempool.
     @param counter A pointer to the tZeroCrossingCounter to free.
     
     @fn Lfloat   tZeroCrossingCounter_tick         (tZeroCrossingCounter* const counter, Lfloat input)
     @brief Tick the tZeroCrossingCounter.
     @param counter A pointer to the relevant tZeroCrossingCounter.
     @param input The input sample.
     @return The amount of zero crossings as a proportion of the window.
     
     @fn void    tZeroCrossingCounter_setWindowSize (tZeroCrossingCounter* const counter, Lfloat windowSize)
     @brief Set the size of the window. Cannot be greater than the max size given on initialization.
     @param counter A pointer to the relevant tZeroCrossingCounter.
     @param windowSize The new window size.
     ￼￼￼
     @} */
    
    /* Zero Crossing Detector */
    typedef struct tZeroCrossingCounter
    {

        tMempool* mempool;
        int count;
        int maxWindowSize;
        int currentWindowSize;
        Lfloat invCurrentWindowSize;
        Lfloat* inBuffer;
        uint16_t* countBuffer;
        int prevPosition;
        int position;
    } tZeroCrossingCounter;

    void    tZeroCrossingCounter_init          (tZeroCrossingCounter** const, int maxWindowSize, LEAF* const leaf);
    void    tZeroCrossingCounter_initToPool    (tZeroCrossingCounter** const, int maxWindowSize, tMempool** const mempool);
    void    tZeroCrossingCounter_free          (tZeroCrossingCounter** const);
    
    Lfloat  tZeroCrossingCounter_tick          (tZeroCrossingCounter* const, Lfloat input);

    void    tZeroCrossingCounter_setWindowSize (tZeroCrossingCounter* const, Lfloat windowSize);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tpowerfollower tPowerFollower
     @ingroup analysis
     @brief Measure and follow the power of an input signal using an exponential moving average for smoothing.
     @{
     
     @fn void    tPowerFollower_init(tPowerFollower** const, Lfloat factor, LEAF* const leaf)
     @brief Initialize a tPowerFollower to the default mempool of a LEAF instance.
     @param follower A pointer to the tPowerFollower to initialize.
     @param factor Smoothing factor of the moving average. 0.0-1.0, with a higher value discounting older inputs more quickly.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPowerFollower_initToPool(tPowerFollower** const, Lfloat factor, tMempool** const)
     @brief Initialize a tPowerFollower to a specified mempool.
     @param follower A pointer to the tPowerFollower to initialize.
     @param factor Smoothing factor of the moving average. 0.0-1.0, with a higher value discounting older inputs more quickly.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPowerFollower_free(tPowerFollower** const)
     @brief Free a tPowerFollower from its mempool.
     @param follower A pointer to the tPowerFollower to free.
     
     @fn Lfloat   tPowerFollower_tick         (tPowerFollower* const, Lfloat input)
     @brief Pass a sample into the power follower and return the current power.
     @param follower A pointer to the relevant tPowerFollower.
     @param input The input sample
     @return The current power.
     
     @fn Lfloat   tPowerFollower_getPower       (tPowerFollower* const)
     @brief Get the current power.
     @param follower A pointer to the relevant tPowerFollower.
     @return The current power.
     
     @fn int     tPowerFollower_setFactor    (tPowerFollower* const, Lfloat factor)
     @brief Set the smoothing factor for the moving average.
     @param follower A pointer to the relevant tPowerFollower.
     @param factor Smoothing factor of the moving average. 0.0-1.0, with a higher value discounting older inputs more quickly.
     ￼￼￼
     @} */
    
    /* PowerEnvelopeFollower */
    typedef struct tPowerFollower
    {

        tMempool* mempool;
        Lfloat factor, oneminusfactor;
        Lfloat curr;
        
    } tPowerFollower;

    void    tPowerFollower_init         (tPowerFollower** const, Lfloat factor, LEAF* const leaf);
    void    tPowerFollower_initToPool   (tPowerFollower** const, Lfloat factor, tMempool** const);
    void    tPowerFollower_free         (tPowerFollower** const);
    
    Lfloat  tPowerFollower_tick         (tPowerFollower* const, Lfloat input);

    Lfloat  tPowerFollower_getPower     (tPowerFollower* const);
    void    tPowerFollower_setFactor    (tPowerFollower* const, Lfloat factor);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tenvpd tEnvPD
     @ingroup analysis
     @brief ENV~ from PD, modified for LEAF
     @{
     
     @fn void    tEnvPD_init(tEnvPD** const, int windowSize, int hopSize, int blockSize, LEAF* const leaf)
     @brief Initialize a tEnvPD to the default mempool of a LEAF instance.
     @param env A pointer to the tEnvPD to initialize.
     @param windowSize
     @param hopSize
     @param blockSize
     @param leaf A pointer to the leaf instance.
     
     @fn void    tEnvPD_initToPool(tEnvPD** const, int windowSize, int hopSize, int blockSize, tMempool** const)
     @brief Initialize a tEnvPD to a specified mempool.
     @param env A pointer to the tEnvPD to initialize.
     @param windowSize
     @param hopSize
     @param blockSize
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tEnvPD_free(tEnvPD** const)
     @brief Free a tEnvPD from its mempool.
     @param env  A pointer to the tEnvPD to free.
     
     @fn Lfloat   tEnvPD_tick             (tEnvPD* const)
     @brief
     @param env
     
     @fn void    tEnvPD_processBlock     (tEnvPD* const, Lfloat* in)
     @brief
     @param env
     @param inputBlock
     ￼￼￼
     @} */
    
#define MAXOVERLAP 32
#define INITVSTAKEN 64
#define ENV_WINDOW_SIZE 1024
#define ENV_HOP_SIZE 256
    
    typedef struct tEnvPD
    {

        tMempool* mempool;
        Lfloat buf[ENV_WINDOW_SIZE + INITVSTAKEN];
        int x_phase;                    /* number of points since last output */
        int x_period;                   /* requested period of output */
        int x_realperiod;               /* period rounded up to vecsize multiple */
        int x_npoints;                  /* analysis window size in samples */
        Lfloat x_result;                 /* result to output */
        Lfloat x_sumbuf[MAXOVERLAP];     /* summing buffer */
        Lfloat x_f;
        int windowSize, hopSize, blockSize;
        int x_allocforvs;               /* extra buffer for DSP vector size */
    } tEnvPD;
    

    void    tEnvPD_init             (tEnvPD** const, int windowSize, int hopSize, int blockSize, LEAF* const leaf);
    void    tEnvPD_initToPool       (tEnvPD** const, int windowSize, int hopSize, int blockSize, tMempool** const);
    void    tEnvPD_free             (tEnvPD** const);
    
    Lfloat  tEnvPD_tick             (tEnvPD* const);

    void    tEnvPD_processBlock     (tEnvPD* const, Lfloat* in);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tattackdetection tAttackDetection
     @ingroup analysis
     @brief Detect attacks in an input signal
     @{
     
     @fn void    tAttackDetection_init(tAttackDetection** const, int blocksize, int atk, int rel, LEAF* const leaf)
     @brief Initialize a tAttackDetection to the default mempool of a LEAF instance.
     @param detection A pointer to the tAttackDetection to initialize.
     @param blockSize
     @param attack
     @param release
     @param leaf A pointer to the leaf instance.
     
     @fn void    tAttackDetection_initToPool(tAttackDetection** const, int blocksize, int atk, int rel, tMempool** const)
     @brief Initialize a tAttackDetection to a specified mempool.
     @param detection A pointer to the tAttackDetection to initialize.
     @param blockSize
     @param attack
     @param release
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tAttackDetection_free(tAttackDetection** const)
     @brief Free a tAttackDetection from its mempool.
     @param detection A pointer to the tAttackDetection to free.
     
     @fn void    tAttackDetection_setBlocksize   (tAttackDetection* const, int size)
     @brief Set expected input blocksize
     @param detection A pointer to the relevant tAttackDetection.
     @param blockSize
     
     @fn void    tAttackDetection_setSamplerate  (tAttackDetection* const, int inRate)
     @brief Set attack detection sample rate
     @param detection A pointer to the relevant tAttackDetection.
     @param sampleRate
     
     @fn void    tAttackDetection_setAttack      (tAttackDetection* const, int inAtk)
     @brief Set attack time and coeff
     @param detection A pointer to the relevant tAttackDetection.
     @param attack
     
     @fn void    tAttackDetection_setRelease     (tAttackDetection* const, int inRel)
     @brief Set release time and coeff
     @param detection A pointer to the relevant tAttackDetection.
     @param release
     
     @fn void    tAttackDetection_setThreshold   (tAttackDetection* const, Lfloat thres)
     @brief Set level above which values are identified as attacks
     @param detection A pointer to the relevant tAttackDetection.
     @param threshold
     
     @fn int     tAttackDetection_detect         (tAttackDetection* const, Lfloat *in)
     @brief Find the largest transient in input block, return index of attack
     @param detection A pointer to the relevant tAttackDetection.
     @param inputBlock
     @return Index of the largest transient in the input.
     ￼￼￼
     @} */
    
#define DEFBLOCKSIZE 1024
#define DEFTHRESHOLD 6
#define DEFATTACK    10
#define DEFRELEASE    10
    
    typedef struct tAttackDetection
    {
        tMempool* mempool;
        Lfloat env;
        
        //Attack & Release times in msec
        int atk;
        int rel;
        
        //Attack & Release coefficients based on times
        Lfloat atk_coeff;
        Lfloat rel_coeff;
        
        int blockSize;
        int sampleRate;
        
        //RMS amplitude of previous block - used to decide if attack is present
        Lfloat prevAmp;
        
        Lfloat threshold;
    } tAttackDetection;
    

    void    tAttackDetection_init           (tAttackDetection** const, int blocksize, int atk, int rel, LEAF* const leaf);
    void    tAttackDetection_initToPool     (tAttackDetection** const, int blocksize, int atk, int rel, tMempool** const);
    void    tAttackDetection_free           (tAttackDetection** const);
    
    void    tAttackDetection_setBlocksize   (tAttackDetection* const, int size);
    void    tAttackDetection_setSamplerate  (tAttackDetection* const, int inRate);
    void    tAttackDetection_setAttack      (tAttackDetection* const, int inAtk);
    void    tAttackDetection_setRelease     (tAttackDetection* const, int inRel);
    void    tAttackDetection_setThreshold   (tAttackDetection* const, Lfloat thres);
    int     tAttackDetection_detect         (tAttackDetection* const, Lfloat *in);
    void    tAttackDetection_setSampleRate  (tAttackDetection* const, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tsnac tSNAC
     @ingroup analysis
     @brief Component of period detection algorithm from Katja Vetters http://www.katjaas.nl/helmholtz/helmholtz.html
     @{
     
     @fn void    tSNAC_init(tSNAC** const, int overlaparg, LEAF* const leaf)
     @brief Initialize a tSNAC to the default mempool of a LEAF instance.
     @param snac A pointer to the tSNAC to initialize.
     @param overlap
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSNAC_initToPool(tSNAC** const, int overlaparg, tMempool** const)
     @brief Initialize a tSNAC to a specified mempool.
     @param snac A pointer to the tSNAC to initialize.
     @param overlap
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSNAC_free(tSNAC** const)
     @brief Free a tSNAC from its mempool.
     @param snac A pointer to the tSNAC to free.
     
     @fn void    tSNAC_ioSamples     (tSNAC *s, Lfloat *in, Lfloat *out, int size)
     @brief
     @param snac A pointer to the relevant tSNAC.
     @param input
     @param output
     @param size
     
     @fn void    tSNAC_setOverlap    (tSNAC *s, int lap)
     @brief
     @param snac A pointer to the relevant tSNAC.
     @param overlap
     
     @fn void    tSNAC_setBias       (tSNAC *s, Lfloat bias)
     @brief
     @param snac A pointer to the relevant tSNAC.
     @param bias
     
     @fn void    tSNAC_setMinRMS     (tSNAC *s, Lfloat rms)
     @brief
     @param snac A pointer to the relevant tSNAC.
     @param rms
     
     @fn Lfloat   tSNAC_getPeriod     (tSNAC *s)
     @brief
     @param snac A pointer to the relevant tSNAC.
     @return The detected period of the input
     
     @fn Lfloat   tSNAC_getFidelity   (tSNAC *s)
     @brief
     @param snac A pointer to the relevant tSNAC.
     @return The periodic fidelity of the input
     ￼￼￼
     @} */
    
#define SNAC_FRAME_SIZE 1024           // default analysis framesize // should be the same as (or smaller than?) PS_FRAME_SIZE
#define DEFOVERLAP 1                // default overlap
#define DEFBIAS 0.2f        // default bias
#define DEFMINRMS 0.003f   // default minimum RMS
#define SEEK 0.85f       // seek-length as ratio of framesize
    
    typedef struct tSNAC
    {
        tMempool* mempool;
        
        Lfloat* inputbuf;
        Lfloat* processbuf;
        Lfloat* spectrumbuf;
        Lfloat* biasbuf;
        uint16_t timeindex;
        uint16_t framesize;
        uint16_t overlap;
        uint16_t periodindex;
        
        Lfloat periodlength;
        Lfloat fidelity;
        Lfloat biasfactor;
        Lfloat minrms;
        
    } tSNAC;

    void    tSNAC_init          (tSNAC** const, int overlaparg, LEAF* const leaf);
    void    tSNAC_initToPool    (tSNAC** const, int overlaparg, tMempool** const);
    void    tSNAC_free          (tSNAC** const);
    
    void    tSNAC_ioSamples     (tSNAC* s, Lfloat *in, int size);
    void    tSNAC_setOverlap    (tSNAC* s, int lap);
    void    tSNAC_setBias       (tSNAC* s, Lfloat bias);
    void    tSNAC_setMinRMS     (tSNAC* s, Lfloat rms);
    
    /*To get freq, perform SAMPLE_RATE/snac_getperiod() */
    Lfloat   tSNAC_getPeriod     (tSNAC* s);
    Lfloat   tSNAC_getFidelity   (tSNAC* s);
    
    /*!
     @defgroup tperioddetection tPeriodDetection
     @ingroup analysis
     @brief Period detection algorithm from Katja Vetters http://www.katjaas.nl/helmholtz/helmholtz.html
     @{

     @fn void    tPeriodDetection_init(tPeriodDetection** const, Lfloat* in, Lfloat* out, int bufSize, int frameSize, LEAF* const leaf)
     @brief Initialize a tPeriodDetection to the default mempool of a LEAF instance.
     @param detection A pointer to the tPeriodDetection to initialize.
     @param in
     @param out
     @param bufferSize
     @param frameSize
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPeriodDetection_initToPool(tPeriodDetection** const, Lfloat* in, Lfloat* out, int bufSize, int frameSize, tMempool** const)
     @brief Initialize a tPeriodDetection to a specified mempool.
     @param detection A pointer to the tPeriodDetection to initialize.
     @param in
     @param out
     @param bufferSize
     @param frameSize
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPeriodDetection_free(tPeriodDetection** const)
     @brief Free a tPeriodDetection from its mempool.
     @param detection A pointer to the tPeriodDetection to free.
     
     @fn Lfloat   tPeriodDetection_tick               (tPeriodDetection* const, Lfloat sample)
     @brief
     @param detection A pointer to the relevant tPeriodDetection.
     @param input
     @return
     
     @fn Lfloat   tPeriodDetection_getPeriod          (tPeriodDetection* const)
     @brief
     @param detection A pointer to the relevant tPeriodDetection.
     @return The detected period.
     
     @fn void    tPeriodDetection_setHopSize         (tPeriodDetection* const, int hs)
     @brief
     @param detection A pointer to the relevant tPeriodDetection.
     @param hopSize
     
     @fn void    tPeriodDetection_setWindowSize      (tPeriodDetection* const, int ws)
     @brief
     @param detection A pointer to the relevant tPeriodDetection.
     @param windowSize
     
     @fn void    tPeriodDetection_setFidelityThreshold(tPeriodDetection* const, Lfloat threshold)
     @brief
     @param detection A pointer to the relevant tPeriodDetection.
     @param threshold
     
     @fn void    tPeriodDetection_setAlpha           (tPeriodDetection* const, Lfloat alpha)
     @brief
     @param detection A pointer to the relevant tPeriodDetection.
     @param alpha
     
     @fn void    tPeriodDetection_setTolerance       (tPeriodDetection* const, Lfloat tolerance)
     @brief
     @param detection A pointer to the relevant tPeriodDetection.
     @param tolerance
     ￼￼￼
     @} */
    
#define DEFPITCHRATIO 1.0f
#define DEFTIMECONSTANT 100.0f
#define DEFHOPSIZE 64
#define DEFWINDOWSIZE 64
#define FBA 20
#define HPFREQ 20.0f
    
    typedef struct tPeriodDetection
    {
        tMempool* mempool;
        
        tEnvPD* env;
        tSNAC* snac;
        Lfloat* inBuffer;
        Lfloat* outBuffer;
        int frameSize;
        int bufSize;
        int framesPerBuffer;
        int curBlock;
        int lastBlock;
        int i;
        int indexstore;
        int iLast;
        int index;
        Lfloat period;
        
        uint16_t hopSize;
        uint16_t windowSize;
        uint8_t fba;
        
        Lfloat timeConstant;
        Lfloat radius;
        Lfloat max;
        Lfloat lastmax;
        Lfloat deltamax;
        
        Lfloat fidelityThreshold;
        
        Lfloat history;
        Lfloat alpha;
        Lfloat tolerance;
        
        Lfloat invSampleRate;
    } tPeriodDetection;

    void    tPeriodDetection_init                 (tPeriodDetection** const, Lfloat* in, int bufSize, int frameSize, LEAF* const leaf);
    void    tPeriodDetection_initToPool           (tPeriodDetection** const, Lfloat* in, int bufSize, int frameSize, tMempool** const);
    void    tPeriodDetection_free                 (tPeriodDetection** const);
    
    Lfloat  tPeriodDetection_tick                 (tPeriodDetection* const, Lfloat sample);

    Lfloat  tPeriodDetection_getPeriod            (tPeriodDetection* const);
    Lfloat  tPeriodDetection_getFidelity          (tPeriodDetection* const);
    void    tPeriodDetection_setHopSize           (tPeriodDetection* const, int hs);
    void    tPeriodDetection_setWindowSize        (tPeriodDetection* const, int ws);
    void    tPeriodDetection_setFidelityThreshold (tPeriodDetection* const, Lfloat threshold);
    void    tPeriodDetection_setAlpha             (tPeriodDetection* const, Lfloat alpha);
    void    tPeriodDetection_setTolerance         (tPeriodDetection* const, Lfloat tolerance);
    void    tPeriodDetection_setSampleRate        (tPeriodDetection* const, Lfloat sr);
    
    //==============================================================================
    
    // Maybe keep from here to tPeriodDetector internal?
    typedef struct tZeroCrossingInfo
    {

        tMempool* mempool;
        
        Lfloat _before_crossing;
        Lfloat _after_crossing;
        
        Lfloat             _peak;
        int               _leading_edge;// = undefined_edge; int_min
        int               _trailing_edge;// = undefined_edge;
        Lfloat             _width;// = 0.0f;
    } tZeroCrossingInfo;

    void    tZeroCrossingInfo_init              (tZeroCrossingInfo** const, LEAF* const leaf);
    void    tZeroCrossingInfo_initToPool        (tZeroCrossingInfo** const, tMempool** const);
    void    tZeroCrossingInfo_free              (tZeroCrossingInfo** const);
    
    int     tZeroCrossingInfo_tick              (tZeroCrossingInfo* const, Lfloat s);

    int     tZeroCrossingInfo_getState          (tZeroCrossingInfo* const);
    void    tZeroCrossingInfo_updatePeak        (tZeroCrossingInfo* const, Lfloat s, int pos);
    int     tZeroCrossingInfo_period            (tZeroCrossingInfo* const, tZeroCrossingInfo* const next);
    Lfloat  tZeroCrossingInfo_fractionalPeriod  (tZeroCrossingInfo* const, tZeroCrossingInfo* const next);
    int     tZeroCrossingInfo_getWidth          (tZeroCrossingInfo* const);
    
    //==============================================================================
    
    typedef struct tZeroCrossingCollector
    {
        tMempool* mempool;
        
        tZeroCrossingInfo** _info;
        unsigned int _size;
        unsigned int _pos;
        unsigned int _mask;
        
        Lfloat                _prev;// = 0.0f;
        Lfloat                _hysteresis;
        int                  _state;// = false;
        int                  _num_edges;// = 0;
        int                  _window_size;
        int                  _frame;// = 0;
        int                  _ready;// = false;
        Lfloat                _peak_update;// = 0.0f;
        Lfloat                _peak;// = 0.0f;
    } tZeroCrossingCollector;

    void    tZeroCrossingCollector_init          (tZeroCrossingCollector** const, int windowSize, Lfloat hysteresis, LEAF* const leaf);
    void    tZeroCrossingCollector_initToPool    (tZeroCrossingCollector** const, int windowSize, Lfloat hysteresis, tMempool** const);
    void    tZeroCrossingCollector_free          (tZeroCrossingCollector** const);
    
    int     tZeroCrossingCollector_tick          (tZeroCrossingCollector* const, Lfloat s);
    int     tZeroCrossingCollector_getState      (tZeroCrossingCollector* const);
    
    int     tZeroCrossingCollector_getNumEdges   (tZeroCrossingCollector* const zc);
    int     tZeroCrossingCollector_getCapacity   (tZeroCrossingCollector* const zc);
    int     tZeroCrossingCollector_getFrame      (tZeroCrossingCollector* const zc);
    int     tZeroCrossingCollector_getWindowSize (tZeroCrossingCollector* const zc);

    int     tZeroCrossingCollector_isReady       (tZeroCrossingCollector* const zc);
    Lfloat  tZeroCrossingCollector_getPeak       (tZeroCrossingCollector* const zc);
    int     tZeroCrossingCollector_isReset       (tZeroCrossingCollector* const zc);
    
    tZeroCrossingInfo* const tZeroCrossingCollector_getCrossing   (tZeroCrossingCollector* const zc, int index);
    void                    tZeroCrossingCollector_setHysteresis (tZeroCrossingCollector* const zc, Lfloat hysteresis);
    
    //==============================================================================
    
    typedef struct tBitset
    {
        tMempool* mempool;
        
        unsigned int _value_size;
        unsigned int _size;
        unsigned int _bit_size;
        unsigned int* _bits;
    } tBitset;

    void    tBitset_init            (tBitset** const bitset, int numBits, LEAF* const leaf);
    void    tBitset_initToPool      (tBitset** const bitset, int numBits, tMempool** const mempool);
    void    tBitset_free            (tBitset** const bitset);
    
    int     tBitset_get             (tBitset* const bitset, int index);
    void    tBitset_set             (tBitset* const bitset, int index, unsigned int val);
    void    tBitset_setMultiple     (tBitset* const bitset, int index, int n, unsigned int val);
    int     tBitset_getSize         (tBitset* const bitset);
    void    tBitset_clear           (tBitset* const bitset);

    unsigned int* tBitset_getData   (tBitset* const bitset);

    
    //==============================================================================
    
    typedef struct tBACF
    {
        tMempool* mempool;
        
        tBitset* _bitset;
        unsigned int _mid_array;
    } tBACF;

    void    tBACF_init           (tBACF** const bacf, tBitset** const bitset, LEAF* const leaf);
    void    tBACF_initToPool     (tBACF** const bacf, tBitset** const bitset, tMempool** const mempool);
    void    tBACF_free           (tBACF** const bacf);
    
    int     tBACF_getCorrelation (tBACF* const bacf, int pos);
    void    tBACF_set            (tBACF* const bacf, tBitset** const bitset);
    
    //==============================================================================
    
    /*!
     @defgroup tperioddetector tPeriodDetector
     @ingroup analysis
     @brief Period detection algorithm from Joel de Guzman's Q Audio DSP Library
     @{
     
     @fn void    tPeriodDetector_init(tPeriodDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, Lfloat hysteresis, LEAF* const leaf)
     @brief Initialize a tPeriodDetector to the default mempool of a LEAF instance.
     @param
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPeriodDetector_initToPool(tPeriodDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, Lfloat hysteresis, tMempool** const mempool)
     @brief Initialize a tPeriodDetector to a specified mempool.
     @param
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPeriodDetector_free(tPeriodDetector** const detector)
     @brief Free a tPeriodDetector from its mempool.
     @param detector A pointer to the tPeriodDetector to free.
     
     @fn int     tPeriodDetector_tick    (tPeriodDetector* const detector, Lfloat sample)
     @brief
     @param
     
     @fn Lfloat   tPeriodDetector_getPeriod   (tPeriodDetector* const detector)
     @brief Get the periodicity for a given harmonic of the detected pitch.
     @param
     
     @fn Lfloat   tPeriodDetector_getPeriodicity  (tPeriodDetector* const detector)
     @brief
     @param
     
     @fn Lfloat   tPeriodDetector_harmonic    (tPeriodDetector* const detector, int harmonicIndex)
     @brief
     @param
     
     @fn Lfloat   tPeriodDetector_predictPeriod   (tPeriodDetector* const detector)
     @brief
     @param
     
     @fn int     tPeriodDetector_isReady (tPeriodDetector* const detector)
     @brief
     @param
     
     @fn int     tPeriodDetector_isReset (tPeriodDetector* const detector)
     @brief
     @param
     ￼￼￼
     @fn void    tPeriodDetector_setHysteresis    (tPeriodDetector* const detector, Lfloat hysteresis)
     @brief Set the hysteresis used in zero crossing detection.
     @param detector A pointer to the relevant tPeriodDetector.
     @param hysteresis The hysteresis in decibels. Defaults to -40db.
     
     @} */
    
#define PULSE_THRESHOLD 0.6f
#define HARMONIC_PERIODICITY_FACTOR 16 //16
#define PERIODICITY_DIFF_FACTOR 0.008f //0.008f
    
    typedef struct _auto_correlation_info
    {
        int               _i1;// = -1;
        int               _i2;// = -1;
        int               _period;// = -1;
        Lfloat             _periodicity;// = 0.0f;
        unsigned int      _harmonic;
    } _auto_correlation_info;
    
    typedef struct _sub_collector
    {
        tMempool* mempool;
        
        Lfloat             _first_period;
        
        _auto_correlation_info _fundamental;
        
        // passed in, not initialized
        tZeroCrossingCollector*    _zc;
        
        Lfloat             _harmonic_threshold;
        Lfloat             _periodicity_diff_threshold;
        int               _range;
    } _sub_collector;

    typedef struct _period_info
    {
        Lfloat period; // -1.0f
        Lfloat periodicity;
    } _period_info;

    typedef struct tPeriodDetector
    {
        tMempool* mempool;
        
        tZeroCrossingCollector*          _zc;
        _period_info            _fundamental;
        unsigned int            _min_period;
        int                     _range;
        tBitset*                 _bits;
        Lfloat                   _weight;
        unsigned int            _mid_point;
        Lfloat                   _periodicity_diff_threshold;
        Lfloat                   _predicted_period;// = -1.0f;
        unsigned int            _edge_mark;// = 0;
        unsigned int            _predict_edge;// = 0;
        unsigned int            _num_pulses; // = 0;
        int                     _half_empty; // 0;
        
        Lfloat                   sampleRate;
        Lfloat                   lowestFreq;
        Lfloat                   highestFreq;
        
        tBACF*                   _bacf;
        
    } tPeriodDetector;

    void    tPeriodDetector_init    (tPeriodDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, Lfloat hysteresis, LEAF* const leaf);
    void    tPeriodDetector_initToPool  (tPeriodDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, Lfloat hysteresis, tMempool** const mempool);
    void    tPeriodDetector_free    (tPeriodDetector** const detector);
    
    int     tPeriodDetector_tick           (tPeriodDetector* const detector, Lfloat sample);
    
    // get the periodicity for a given harmonic
    Lfloat  tPeriodDetector_getPeriod      (tPeriodDetector* const detector);
    Lfloat  tPeriodDetector_getPeriodicity (tPeriodDetector* const detector);
    Lfloat  tPeriodDetector_harmonic       (tPeriodDetector* const detector, int harmonicIndex);
    Lfloat  tPeriodDetector_predictPeriod  (tPeriodDetector* const detector);
    int     tPeriodDetector_isReady        (tPeriodDetector* const detector);
    int     tPeriodDetector_isReset        (tPeriodDetector* const detector);
    void    tPeriodDetector_setHysteresis  (tPeriodDetector* const detector, Lfloat hysteresis);
    void    tPeriodDetector_setSampleRate  (tPeriodDetector* const detector, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tpitchdetector tPitchDetector
     @ingroup analysis
     @brief Pitch detection algorithm from Joel de Guzman's Q Audio DSP Library
     @{
     
     @fn void    tPitchDetector_init(tPitchDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, LEAF* const leaf)
     @brief Initialize a tPitchDetector to the default mempool of a LEAF instance.
     @param detector A pointer to the relevant tPitchDetector.
     @param lowestFreq
     @param highestFreq
     @param leaf A pointer to the leaf instance.
     
     
     @fn void    tPitchDetector_initToPool(tPitchDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, tMempool** const mempool)
     @brief Initialize a tPitchDetector to a specified mempool.
     @param detector A pointer to the relevant tPitchDetector.
     @param lowestFreq
     @param highestFreq
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPitchDetector_free(tPitchDetector** const detector)
     @brief Free a tPitchDetector from its mempool.
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn int     tPitchDetector_tick    (tPitchDetector* const detector, Lfloat sample)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     @param input
     
     @fn Lfloat   tPitchDetector_getFrequency    (tPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn Lfloat   tPitchDetector_getPeriodicity  (tPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn Lfloat   tPitchDetector_harmonic    (tPitchDetector* const detector, int harmonicIndex)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     @param harmonicIndex
     
     @fn Lfloat   tPitchDetector_predictFrequency (tPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn void    tPitchDetector_setHysteresis    (tPitchDetector* const detector, Lfloat hysteresis)
     @brief Set the hysteresis used in zero crossing detection.
     @param detector A pointer to the relevant tPitchDetector.
     @param hysteresis The hysteresis in decibels. Defaults to -40db.
     ￼￼￼
     @} */
    
#define ONSET_PERIODICITY 0.95f
#define MIN_PERIODICITY 0.9f
#define DEFAULT_HYSTERESIS -200.0f

    typedef struct _pitch_info
    {
        Lfloat frequency;
        Lfloat periodicity;
    } _pitch_info;
    
    typedef struct tPitchDetector
    {

        tMempool* mempool;
        
        tPeriodDetector* _pd;
        _pitch_info _current;
        int _frames_after_shift;// = 0;
        
        Lfloat sampleRate;
        
    } tPitchDetector;

    void    tPitchDetector_init              (tPitchDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, LEAF* const leaf);
    void    tPitchDetector_initToPool        (tPitchDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, tMempool** const mempool);
    void    tPitchDetector_free              (tPitchDetector** const detector);
    
    int     tPitchDetector_tick              (tPitchDetector* const detector, Lfloat sample);

    Lfloat  tPitchDetector_getFrequency      (tPitchDetector* const detector);
    Lfloat  tPitchDetector_getPeriodicity    (tPitchDetector* const detector);
    Lfloat  tPitchDetector_harmonic          (tPitchDetector* const detector, int harmonicIndex);
    Lfloat  tPitchDetector_predictFrequency  (tPitchDetector* const detector);
    int     tPitchDetector_indeterminate     (tPitchDetector* const detector);
    
    void    tPitchDetector_setHysteresis     (tPitchDetector* const detector, Lfloat hysteresis);
    void    tPitchDetector_setSampleRate     (tPitchDetector* const detector, Lfloat sr);
    
    //==============================================================================
    
    /*!
     @defgroup tdualpitchdetector tDualPitchDetector
     @ingroup analysis
     @brief Combined pitch detection algorithm using both Joel de Guzman's Q Audio DSP Library and Katya Vetters algorithms
     @{
     
     @fn void tDualPitchDetector_init(tDualPitchDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, Lfloat* inBuffer, int bufSize, LEAF* const leaf)
     @brief Initialize a tDualPitchDetector to the default mempool of a LEAF instance.
     @param detector A pointer to the relevant tDualPitchDetector.
     @param lowestFreq
     @param highestFreq
     @param inputBuffer A buffer of Lfloat to store input to the pitch detector.
     @param bufferSize Size of the input buffer.
     @param leaf A pointer to the leaf instance.
     
     @fn void tDualPitchDetector_initToPool(tDualPitchDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, Lfloat* inBuffer, int bufSize, tMempool** const mempool)
     @brief Initialize a tDualPitchDetector to a specified mempool.
     @param detector A pointer to the relevant tPitchDualDetector.
     @param lowestFreq
     @param highestFreq
     @param inputBuffer A buffer of Lfloat to store input to the pitch detector.
     @param bufferSize Size of the input buffer.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tDualPitchDetector_free(tDualPitchDetector** const detector)
     @brief Free a tDualPitchDetector from its mempool.
     @param detector A pointer to the relevant tDualPitchDetector.
     
     @fn int     tDualPitchDetector_tick    (tDualPitchDetector* const detector, Lfloat sample)
     @brief
     @param detector A pointer to the relevant tDualPitchDetector.
     @param input
     
     @fn Lfloat   tDualPitchDetector_getFrequency    (tDualPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tDualPitchDetector.
     @return The detected frequency.
     
     @fn Lfloat   tDualPitchDetector_getPeriodicity  (tDualPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tDualPitchDetector.
     @return The periodicity of the input.
     
     @fn Lfloat   tDualPitchDetector_harmonic    (tDualPitchDetector* const detector, int harmonicIndex)
     @brief
     @param detector A pointer to the relevant tDualPitchDetector.
     @param harmonicIndex
     @return
     
     @fn Lfloat tDualPitchDetector_predictFrequency (tDualPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tDualPitchDetector.
     @return The predicted frequency in Hz.
     
     @fn void    tDualPitchDetector_setHysteresis    (tDualPitchDetector* const detector, Lfloat hysteresis)
     @brief Set the hysteresis used in zero crossing detection.
     @param detector A pointer to the relevant tDualPitchDetector.
     @param hysteresis The hysteresis in decibels. Defaults to -40db.
     
     @fn void    tDualPitchDetector_setPeriodicityThreshold (tDualPitchDetector* const detector, Lfloat thresh)
     @brief Set the threshold for periodicity of a signal to be considered as pitched.
     @param detector A pointer to the relevant tDualPitchDetector.
     @param threshold The periodicity threshold from 0.0 to 1.0 with 1.0 being perfectly periodic.
     ￼￼￼
     @} */

    typedef struct tDualPitchDetector
    {
        tMempool* mempool;
        
        tPeriodDetection* _pd1;
        tPitchDetector* _pd2;
        _pitch_info _current;
        Lfloat _mean;
        Lfloat _predicted_frequency;
        int _first;
        
        Lfloat highest, lowest;
        Lfloat thresh;
        
        Lfloat sampleRate;

    } tDualPitchDetector;

    void    tDualPitchDetector_init (tDualPitchDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, Lfloat* inBuffer, int bufSize, LEAF* const leaf);
    void    tDualPitchDetector_initToPool   (tDualPitchDetector** const detector, Lfloat lowestFreq, Lfloat highestFreq, Lfloat* inBuffer, int bufSize, tMempool** const mempool);
    void    tDualPitchDetector_free (tDualPitchDetector** const detector);
    
    int     tDualPitchDetector_tick                    (tDualPitchDetector* const detector, Lfloat sample);

    Lfloat  tDualPitchDetector_getFrequency            (tDualPitchDetector* const detector);
    Lfloat  tDualPitchDetector_getPeriodicity          (tDualPitchDetector* const detector);
    Lfloat  tDualPitchDetector_harmonic                (tDualPitchDetector* const detector, int harmonicIndex);
    Lfloat  tDualPitchDetector_predictFrequency        (tDualPitchDetector* const detector);
    
    void    tDualPitchDetector_setHysteresis           (tDualPitchDetector* const detector, Lfloat hysteresis);
    void    tDualPitchDetector_setPeriodicityThreshold (tDualPitchDetector* const detector, Lfloat thresh);
    void    tDualPitchDetector_setSampleRate           (tDualPitchDetector* const detector, Lfloat sr);
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_ANALYSIS_H_INCLUDED

//==============================================================================




