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
     
     @fn void    tEnvelopeFollower_init          (tEnvelopeFollower* const follower, float attackThreshold, float decayCoeff, LEAF* const leaf)
     @brief Initialize a tEnvelopeFollower to the default LEAF mempool.
     @param follower A pointer to the tEnvelopeFollower to be initialized.
     @param attackThreshold Amplitude threshold for determining an envelope onset. 0.0 to 1.0
     @param decayCoefficient Multiplier to determine the envelope rate of decay. 0.0 to 1.0, above 0.95 recommended.
     
     @fn void    tEnvelopeFollower_initToPool    (tEnvelopeFollower* const follower, float attackThreshold, float decayCoeff, tMempool* const mempool)
     @brief Initialize a tEnvelopeFollower to a specified mempool.
     @param follower A pointer to the tEnvelopeFollower to be initialized.
     @param attackThreshold Amplitude threshold for determining an envelope onset. 0.0 to 1.0
     @param decayCoefficient Multiplier to determine the envelope rate of decay. 0.0 to 1.0, above 0.95 recommended.
     @param mempool A pointer to the tMempool to which the tEnvelopeFollower should be initialized.
     
     @fn void    tEnvelopeFollower_free          (tEnvelopeFollower* const follower)
     @brief Free a tEnvelopeFollower from its mempool.
     @param follower A pointer to the tEnvelopeFollower to be freed.
     
     @fn float   tEnvelopeFollower_tick          (tEnvelopeFollower* const follower, float input)
     @brief Tick the tEnvelopeFollower.
     @param follower A pointer to the relevant tEnvelopeFollower.
     @param input The input sample.
     @return The envelope value.
     
     @fn void    tEnvelopeFollower_setDecayCoefficient    (tEnvelopeFollower* const follower, float decayCoeff)
     @brief Set the envelope decay coefficient.
     @param follower A pointer to the relevant tEnvelopeFollower.
     @param decayCoefficient Multiplier to determine the envelope rate of decay. 0.0 to 1.0, above 0.95 recommended.
     
     @fn void    tEnvelopeFollower_setAttackThreshold  (tEnvelopeFollower* const follower, float attackThresh)
     @brief Set the envelope attack  threshold.
     @param follower A pointer to the relevant tEnvelopeFollower.
     ￼￼￼
     @} */
    
    typedef struct _tEnvelopeFollower
    {
        
        tMempool mempool;
        float y;
        float a_thresh;
        float d_coeff;
        
    } _tEnvelopeFollower;
    
    typedef _tEnvelopeFollower* tEnvelopeFollower;
    
    void    tEnvelopeFollower_init          (tEnvelopeFollower* const follower, float attackThreshold, float decayCoefficient, LEAF* const leaf);
    void    tEnvelopeFollower_initToPool    (tEnvelopeFollower* const follower, float attackThreshold, float decayCoefficient, tMempool* const mempool);
    void    tEnvelopeFollower_free          (tEnvelopeFollower* const follower);
    
    float   tEnvelopeFollower_tick          (tEnvelopeFollower* const follower, float sample);
    void    tEnvelopeFollower_setDecayCoefficient    (tEnvelopeFollower* const follower, float decayCoefficient);
    void    tEnvelopeFollower_setAttackThreshold  (tEnvelopeFollower* const follower, float attackThreshold);
    
    /*!
     @defgroup tzerocrossingcounter tZeroCrossingCounter
     @ingroup analysis
     @brief Counts the amount of zero crossings within a window of the input audio data
     @{
     
     @fn void    tZeroCrossingCounter_init         (tZeroCrossingCounter* const counter, int maxWindowSize, LEAF* const leaf)
     @brief Initialize a tZeroCrossingCounter to the default LEAF mempool.
     @param counter A pointer to the tZeroCrossingCounter to be initialized.
     @param maxWindowSize The max and initial size of the window.
     
     @fn void    tZeroCrossingCounter_initToPool   (tZeroCrossingCounter* const counter, int maxWindowSize, tMempool* const mempool)
     @brief Initialize a tZeroCrossingCounter to a specified mempool.
     @param counter A pointer to the tZeroCrossingCounter to be initialized.
     @param maxWindowSize The max and initial size of the window.
     @param mempool A pointer to the tMempool to which the tZeroCrossingCounter should be initialized.
     
     @fn void    tZeroCrossingCounter_free         (tZeroCrossingCounter* const counter)
     @brief Free a tZeroCrossingCounter from its mempool.
     @param counter A pointer to the tZeroCrossingCounter to be freed.
     
     @fn float   tZeroCrossingCounter_tick         (tZeroCrossingCounter* const counter, float input)
     @brief Tick the tZeroCrossingCounter.
     @param counter A pointer to the relevant tZeroCrossingCounter.
     @param input The input sample.
     @return The amount of zero crossings as a proportion of the window.
     
     @fn void    tZeroCrossingCounter_setWindowSize (tZeroCrossingCounter* const counter, float windowSize)
     @brief Set the size of the window. Cannot be greater than the max size given on initialization.
     @param counter A pointer to the relevant tZeroCrossingCounter.
     @param windowSize The new window size.
     ￼￼￼
     @} */
    
    /* Zero Crossing Detector */
    typedef struct _tZeroCrossingCounter
    {
        
        tMempool mempool;
        int count;
        int maxWindowSize;
        int currentWindowSize;
        float invCurrentWindowSize;
        float* inBuffer;
        uint16_t* countBuffer;
        int prevPosition;
        int position;
    } _tZeroCrossingCounter;
    
    typedef _tZeroCrossingCounter* tZeroCrossingCounter;
    
    void    tZeroCrossingCounter_init         (tZeroCrossingCounter* const, int maxWindowSize, LEAF* const leaf);
    void    tZeroCrossingCounter_initToPool   (tZeroCrossingCounter* const, int maxWindowSize, tMempool* const mempool);
    void    tZeroCrossingCounter_free         (tZeroCrossingCounter* const);
    
    float   tZeroCrossingCounter_tick         (tZeroCrossingCounter* const, float input);
    void    tZeroCrossingCounter_setWindowSize    (tZeroCrossingCounter* const, float windowSize);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tpowerfollower tPowerFollower
     @ingroup analysis
     @brief
     @{
     
     @fn void    tPowerFollower_init         (tPowerFollower* const, float factor, LEAF* const leaf)
     @brief Initialize a tPowerFollower to the default LEAF mempool.
     @param
     
     @fn void    tPowerFollower_initToPool   (tPowerFollower* const, float factor, tMempool* const)
     @brief Initialize a tPowerFollower to a specified mempool.
     @param
     
     @fn void    tPowerFollower_free         (tPowerFollower* const)
     @brief
     @param
     
     @fn float   tPowerFollower_tick         (tPowerFollower* const, float input)
     @brief
     @param
     
     @fn float   tPowerFollower_sample       (tPowerFollower* const)
     @brief
     @param
     
     @fn int     tPowerFollower_setFactor    (tPowerFollower* const, float factor)
     @brief
     @param
     ￼￼￼
     @} */
    
    /* PowerEnvelopeFollower */
    typedef struct _tPowerFollower
    {
        
        tMempool mempool;
        float factor, oneminusfactor;
        float curr;
        
    } _tPowerFollower;
    
    typedef _tPowerFollower* tPowerFollower;
    
    void    tPowerFollower_init         (tPowerFollower* const, float factor, LEAF* const leaf);
    void    tPowerFollower_initToPool   (tPowerFollower* const, float factor, tMempool* const);
    void    tPowerFollower_free         (tPowerFollower* const);
    
    float   tPowerFollower_tick         (tPowerFollower* const, float input);
    float   tPowerFollower_sample       (tPowerFollower* const);
    int     tPowerFollower_setFactor    (tPowerFollower* const, float factor);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tenvpd tEnvPD
     @ingroup analysis
     @brief ENV~ from PD, modified for LEAF
     @{
     
     @fn void    tEnvPD_init             (tEnvPD* const, int windowSize, int hopSize, int blockSize, LEAF* const leaf)
     @brief Initialize a tEnvPD to the default LEAF mempool.
     @param
     
     @fn void    tEnvPD_initToPool       (tEnvPD* const, int windowSize, int hopSize, int blockSize, tMempool* const)
     @brief Initialize a tEnvPD to a specified mempool.
     @param
     
     @fn void    tEnvPD_free             (tEnvPD* const)
     @brief
     @param
     
     @fn float   tEnvPD_tick             (tEnvPD* const)
     @brief
     @param
     
     @fn void    tEnvPD_processBlock     (tEnvPD* const, float* in)
     @brief
     @param
     ￼￼￼
     @} */
    
#define MAXOVERLAP 32
#define INITVSTAKEN 64
#define ENV_WINDOW_SIZE 1024
#define ENV_HOP_SIZE 256
    
    typedef struct _tEnvPD
    {
        
        tMempool mempool;
        float buf[ENV_WINDOW_SIZE + INITVSTAKEN];
        int x_phase;                    /* number of points since last output */
        int x_period;                   /* requested period of output */
        int x_realperiod;               /* period rounded up to vecsize multiple */
        int x_npoints;                  /* analysis window size in samples */
        float x_result;                 /* result to output */
        float x_sumbuf[MAXOVERLAP];     /* summing buffer */
        float x_f;
        int windowSize, hopSize, blockSize;
        int x_allocforvs;               /* extra buffer for DSP vector size */
    } _tEnvPD;
    
    typedef _tEnvPD* tEnvPD;
    
    void    tEnvPD_init             (tEnvPD* const, int windowSize, int hopSize, int blockSize, LEAF* const leaf);
    void    tEnvPD_initToPool       (tEnvPD* const, int windowSize, int hopSize, int blockSize, tMempool* const);
    void    tEnvPD_free             (tEnvPD* const);
    
    float   tEnvPD_tick             (tEnvPD* const);
    void    tEnvPD_processBlock     (tEnvPD* const, float* in);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tattackdetection tAttackDetection
     @ingroup analysis
     @brief
     @{
     
     @fn void    tAttackDetection_init           (tAttackDetection* const, int blocksize, int atk, int rel, LEAF* const leaf)
     @brief Initialize a tAttackDetection to the default LEAF mempool.
     @param
     
     @fn void    tAttackDetection_initToPool     (tAttackDetection* const, int blocksize, int atk, int rel, tMempool* const)
     @brief Initialize a tAttackDetection to a specified mempool.
     @param
     
     @fn void    tAttackDetection_free           (tAttackDetection* const)
     @brief
     @param
     
     @fn void    tAttackDetection_setBlocksize   (tAttackDetection* const, int size)
     @brief Set expected input blocksize
     @param
     
     @fn void    tAttackDetection_setSamplerate  (tAttackDetection* const, int inRate)
     @brief Set attack detection sample rate
     @param
     
     @fn void    tAttackDetection_setAttack      (tAttackDetection* const, int inAtk)
     @brief Set attack time and coeff
     @param
     
     @fn void    tAttackDetection_setRelease     (tAttackDetection* const, int inRel)
     @brief Set release time and coeff
     @param
     
     @fn void    tAttackDetection_setThreshold   (tAttackDetection* const, float thres)
     @brief Set level above which values are identified as attacks
     @param
     
     @fn int     tAttackDetection_detect         (tAttackDetection* const, float *in)
     @brief Find the largest transient in input block, return index of attack
     @param
     ￼￼￼
     @} */
    
#define DEFBLOCKSIZE 1024
#define DEFTHRESHOLD 6
#define DEFATTACK    10
#define DEFRELEASE    10
    
    typedef struct _tAttackDetection
    {
        
        tMempool mempool;
        
        float env;
        
        //Attack & Release times in msec
        int atk;
        int rel;
        
        //Attack & Release coefficients based on times
        float atk_coeff;
        float rel_coeff;
        
        int blocksize;
        int samplerate;
        
        //RMS amplitude of previous block - used to decide if attack is present
        float prevAmp;
        
        float threshold;
    } _tAttackDetection;
    
    typedef _tAttackDetection* tAttackDetection;
    
    void    tAttackDetection_init           (tAttackDetection* const, int blocksize, int atk, int rel, LEAF* const leaf);
    void    tAttackDetection_initToPool     (tAttackDetection* const, int blocksize, int atk, int rel, tMempool* const);
    void    tAttackDetection_free           (tAttackDetection* const);
    
    void    tAttackDetection_setBlocksize   (tAttackDetection* const, int size);
    void    tAttackDetection_setSamplerate  (tAttackDetection* const, int inRate);
    void    tAttackDetection_setAttack      (tAttackDetection* const, int inAtk);
    void    tAttackDetection_setRelease     (tAttackDetection* const, int inRel);
    void    tAttackDetection_setThreshold   (tAttackDetection* const, float thres);
    int     tAttackDetection_detect         (tAttackDetection* const, float *in);
    
    //==============================================================================
    
    /*!
     @defgroup tsnac tSNAC
     @ingroup analysis
     @brief Period detection algorithm from Katja Vetters http://www.katjaas.nl/helmholtz/helmholtz.html
     @{
     
     @fn void    tSNAC_init          (tSNAC* const, int overlaparg, LEAF* const leaf)
     @brief Initialize a tSNAC to the default LEAF mempool.
     @param
     
     @fn void    tSNAC_initToPool    (tSNAC* const, int overlaparg, tMempool* const)
     @brief Initialize a tSNAC to a specified mempool.
     @param
     
     @fn void    tSNAC_free          (tSNAC* const)
     @brief
     @param
     
     @fn void    tSNAC_ioSamples     (tSNAC *s, float *in, float *out, int size)
     @brief
     @param
     
     @fn void    tSNAC_setOverlap    (tSNAC *s, int lap)
     @brief
     @param
     
     @fn void    tSNAC_setBias       (tSNAC *s, float bias)
     @brief
     @param
     
     @fn void    tSNAC_setMinRMS     (tSNAC *s, float rms)
     @brief
     @param
     
     @fn float   tSNAC_getPeriod     (tSNAC *s)
     @brief
     @param
     
     @fn float   tSNAC_getFidelity   (tSNAC *s)
     @brief
     @param
     ￼￼￼
     @} */
    
#define SNAC_FRAME_SIZE 1024           // default analysis framesize // should be the same as (or smaller than?) PS_FRAME_SIZE
#define DEFOVERLAP 1                // default overlap
#define DEFBIAS 0.2f        // default bias
#define DEFMINRMS 0.003f   // default minimum RMS
#define SEEK 0.85f       // seek-length as ratio of framesize
    
    typedef struct _tSNAC
    {
        
        tMempool mempool;
        
        float* inputbuf;
        float* processbuf;
        float* spectrumbuf;
        float* biasbuf;
        uint16_t timeindex;
        uint16_t framesize;
        uint16_t overlap;
        uint16_t periodindex;
        
        float periodlength;
        float fidelity;
        float biasfactor;
        float minrms;
        
    } _tSNAC;
    
    typedef _tSNAC* tSNAC;
    
    void    tSNAC_init          (tSNAC* const, int overlaparg, LEAF* const leaf);
    void    tSNAC_initToPool    (tSNAC* const, int overlaparg, tMempool* const);
    void    tSNAC_free          (tSNAC* const);
    
    void    tSNAC_ioSamples     (tSNAC *s, float *in, float *out, int size);
    void    tSNAC_setOverlap    (tSNAC *s, int lap);
    void    tSNAC_setBias       (tSNAC *s, float bias);
    void    tSNAC_setMinRMS     (tSNAC *s, float rms);
    
    /*To get freq, perform SAMPLE_RATE/snac_getperiod() */
    float   tSNAC_getPeriod     (tSNAC *s);
    float   tSNAC_getFidelity   (tSNAC *s);
    
    
    /*!
     @defgroup tperioddetection tPeriodDetection
     @ingroup analysis
     @brief
     @{
     */
    
    
    /*!
     @fn void    tPeriodDetection_init               (tPeriodDetection* const, float* in, float* out, int bufSize, int frameSize, LEAF* const leaf)
     @brief Initialize a tPeriodDetection to the default LEAF mempool.
     @param
     
     @fn void    tPeriodDetection_initToPool  (tPeriodDetection* const, float* in, float* out, int bufSize, int frameSize, tMempool* const)
     @brief Initialize a tPeriodDetection to a specified mempool.
     @param
     
     @fn void    tPeriodDetection_free               (tPeriodDetection* const)
     @brief
     @param
     
     @fn float   tPeriodDetection_tick               (tPeriodDetection* const, float sample)
     @brief
     @param
     
     @fn float   tPeriodDetection_getPeriod          (tPeriodDetection* const)
     @brief
     @param
     
     @fn void    tPeriodDetection_setHopSize         (tPeriodDetection* const, int hs)
     @brief
     @param
     
     @fn void    tPeriodDetection_setWindowSize      (tPeriodDetection* const, int ws)
     @brief
     @param
     
     @fn void    tPeriodDetection_setFidelityThreshold(tPeriodDetection* const, float threshold)
     @brief
     @param
     
     @fn void    tPeriodDetection_setAlpha           (tPeriodDetection* const, float alpha)
     @brief
     @param
     
     @fn void    tPeriodDetection_setTolerance       (tPeriodDetection* const, float tolerance)
     @brief
     @param
     ￼￼￼
     @} */
    
#define DEFPITCHRATIO 1.0f
#define DEFTIMECONSTANT 100.0f
#define DEFHOPSIZE 64
#define DEFWINDOWSIZE 64
#define FBA 20
#define HPFREQ 20.0f
    
    typedef struct _tPeriodDetection
    {
        
        tMempool mempool;
        
        tEnvPD env;
        tSNAC snac;
        float* inBuffer;
        float* outBuffer;
        int frameSize;
        int bufSize;
        int framesPerBuffer;
        int curBlock;
        int lastBlock;
        int i;
        int indexstore;
        int iLast;
        int index;
        float period;
        
        uint16_t hopSize;
        uint16_t windowSize;
        uint8_t fba;
        
        float timeConstant;
        float radius;
        float max;
        float lastmax;
        float deltamax;
        
        float fidelityThreshold;
        
        float history;
        float alpha;
        float tolerance;
        
    } _tPeriodDetection;
    
    typedef _tPeriodDetection* tPeriodDetection;
    
    void    tPeriodDetection_init               (tPeriodDetection* const, float* in, float* out, int bufSize, int frameSize, LEAF* const leaf);
    void    tPeriodDetection_initToPool         (tPeriodDetection* const, float* in, float* out, int bufSize, int frameSize, tMempool* const);
    void    tPeriodDetection_free               (tPeriodDetection* const);
    
    float   tPeriodDetection_tick               (tPeriodDetection* const, float sample);
    float   tPeriodDetection_getPeriod          (tPeriodDetection* const);
    void    tPeriodDetection_setHopSize         (tPeriodDetection* const, int hs);
    void    tPeriodDetection_setWindowSize      (tPeriodDetection* const, int ws);
    void    tPeriodDetection_setFidelityThreshold(tPeriodDetection* const, float threshold);
    void    tPeriodDetection_setAlpha           (tPeriodDetection* const, float alpha);
    void    tPeriodDetection_setTolerance       (tPeriodDetection* const, float tolerance);
    
    //==============================================================================
    
    // Maybe keep these up to PeriodDetector internal?
    
    typedef struct _tZeroCrossingInfo
    {
        
        tMempool mempool;
        
        float _before_crossing;
        float _after_crossing;
        
        float             _peak;
        int               _leading_edge;// = undefined_edge; int_min
        int               _trailing_edge;// = undefined_edge;
        float             _width;// = 0.0f;
    } _tZeroCrossingInfo;
    
    typedef _tZeroCrossingInfo* tZeroCrossingInfo;
    
    void    tZeroCrossingInfo_init  (tZeroCrossingInfo* const, LEAF* const leaf);
    void    tZeroCrossingInfo_initToPool    (tZeroCrossingInfo* const, tMempool* const);
    void    tZeroCrossingInfo_free  (tZeroCrossingInfo* const);
    
    int     tZeroCrossingInfo_tick(tZeroCrossingInfo* const, float s);
    int     tZeroCrossingInfo_getState(tZeroCrossingInfo* const);
    void    tZeroCrossingInfo_updatePeak(tZeroCrossingInfo* const, float s, int pos);
    int     tZeroCrossingInfo_period(tZeroCrossingInfo* const, tZeroCrossingInfo* const next);
    float   tZeroCrossingInfo_fractionalPeriod(tZeroCrossingInfo* const, tZeroCrossingInfo* const next);
    int     tZeroCrossingInfo_getWidth(tZeroCrossingInfo* const);
    
    //==============================================================================
    
    typedef struct _tZeroCrossingCollector
    {
        
        tMempool mempool;
        
        tZeroCrossingInfo* _info;
        unsigned int _size;
        unsigned int _pos;
        unsigned int _mask;
        
        float                _prev;// = 0.0f;
        float                _hysteresis;
        int                  _state;// = false;
        int                  _num_edges;// = 0;
        int                  _window_size;
        int                  _frame;// = 0;
        int                  _ready;// = false;
        float                _peak_update;// = 0.0f;
        float                _peak;// = 0.0f;
    } _tZeroCrossingCollector;
    
    typedef _tZeroCrossingCollector* tZeroCrossingCollector;
    
    void    tZeroCrossingCollector_init  (tZeroCrossingCollector* const, int windowSize, float hysteresis, LEAF* const leaf);
    void    tZeroCrossingCollector_initToPool    (tZeroCrossingCollector* const, int windowSize, float hysteresis, tMempool* const);
    void    tZeroCrossingCollector_free  (tZeroCrossingCollector* const);
    
    int     tZeroCrossingCollector_tick(tZeroCrossingCollector* const, float s);
    int     tZeroCrossingCollector_getState(tZeroCrossingCollector* const);
    
    int     tZeroCrossingCollector_getNumEdges(tZeroCrossingCollector* const zc);
    int     tZeroCrossingCollector_getCapacity(tZeroCrossingCollector* const zc);
    int     tZeroCrossingCollector_getFrame(tZeroCrossingCollector* const zc);
    int     tZeroCrossingCollector_getWindowSize(tZeroCrossingCollector* const zc);
    
    int     tZeroCrossingCollector_isReady(tZeroCrossingCollector* const zc);
    float   tZeroCrossingCollector_getPeak(tZeroCrossingCollector* const zc);
    int     tZeroCrossingCollector_isReset(tZeroCrossingCollector* const zc);
    
    tZeroCrossingInfo const tZeroCrossingCollector_getCrossing(tZeroCrossingCollector* const zc, int index);

    void    tZeroCrossingCollector_setHysteresis(tZeroCrossingCollector* const zc, float hysteresis);
    
    //==============================================================================
    
    typedef struct _tBitset
    {
        
        tMempool mempool;
        
        unsigned int _value_size;
        unsigned int _size;
        unsigned int _bit_size;
        unsigned int* _bits;
    } _tBitset;
    
    typedef _tBitset* tBitset;
    
    void    tBitset_init    (tBitset* const bitset, int numBits, LEAF* const leaf);
    void    tBitset_initToPool  (tBitset* const bitset, int numBits, tMempool* const mempool);
    void    tBitset_free    (tBitset* const bitset);
    
    int     tBitset_get     (tBitset* const bitset, int index);
    unsigned int*   tBitset_getData   (tBitset* const bitset);
    
    void    tBitset_set     (tBitset* const bitset, int index, unsigned int val);
    void    tBitset_setMultiple (tBitset* const bitset, int index, int n, unsigned int val);

    int     tBitset_getSize (tBitset* const bitset);
    void    tBitset_clear   (tBitset* const bitset);

    
    //==============================================================================
    
    typedef struct _tBACF
    {
        
        tMempool mempool;
        
        tBitset _bitset;
        unsigned int _mid_array;
    } _tBACF;
    
    typedef _tBACF* tBACF;
    
    void    tBACF_init  (tBACF* const bacf, tBitset* const bitset, LEAF* const leaf);
    void    tBACF_initToPool    (tBACF* const bacf, tBitset* const bitset, tMempool* const mempool);
    void    tBACF_free  (tBACF* const bacf);
    
    int     tBACF_getCorrelation    (tBACF* const bacf, int pos);
    void    tBACF_set  (tBACF* const bacf, tBitset* const bitset);
    
    //==============================================================================
    
    /*!
     @defgroup tperioddetector tPeriodDetector
     @ingroup analysis
     @brief
     @{
     
     @fn void    tPeriodDetector_init    (tPeriodDetector* const detector, float lowestFreq, float highestFreq, float hysteresis, LEAF* const leaf)
     @brief Initialize a tPeriodDetector to the default LEAF mempool.
     @param
     
     @fn void    tPeriodDetector_initToPool  (tPeriodDetector* const detector, float lowestFreq, float highestFreq, float hysteresis, tMempool* const mempool)
     @brief Initialize a tPeriodDetector to a specified mempool.
     @param
     
     @fn void    tPeriodDetector_free    (tPeriodDetector* const detector)
     @brief
     @param
     
     @fn int     tPeriodDetector_tick    (tPeriodDetector* const detector, float sample)
     @brief
     @param
     
     @fn float   tPeriodDetector_getPeriod   (tPeriodDetector* const detector)
     @brief Get the periodicity for a given harmonic of the detected pitch.
     @param
     
     @fn float   tPeriodDetector_getPeriodicity  (tPeriodDetector* const detector)
     @brief
     @param
     
     @fn float   tPeriodDetector_harmonic    (tPeriodDetector* const detector, int harmonicIndex)
     @brief
     @param
     
     @fn float   tPeriodDetector_predictPeriod   (tPeriodDetector* const detector)
     @brief
     @param
     
     @fn int     tPeriodDetector_isReady (tPeriodDetector* const detector)
     @brief
     @param
     
     @fn int     tPeriodDetector_isReset (tPeriodDetector* const detector)
     @brief
     @param
     ￼￼￼
     @fn void    tPeriodDetector_setHysteresis    (tPeriodDetector* const detector, float hysteresis)
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
        float             _periodicity;// = 0.0f;
        unsigned int      _harmonic;
    } _auto_correlation_info;
    
    typedef struct _sub_collector
    {
        
        tMempool mempool;
        
        float             _first_period;
        
        _auto_correlation_info _fundamental;
        
        // passed in, not initialized
        tZeroCrossingCollector    _zc;
        
        float             _harmonic_threshold;
        float             _periodicity_diff_threshold;
        int               _range;
    } _sub_collector;

    typedef struct _period_info
    {
        float period; // -1.0f
        float periodicity;
    } _period_info;

    typedef struct _tPeriodDetector
    {
        tMempool mempool;
        
        tZeroCrossingCollector          _zc;
        _period_info            _fundamental;
        unsigned int            _min_period;
        int                     _range;
        tBitset                 _bits;
        float                   _weight;
        unsigned int            _mid_point;
        float                   _periodicity_diff_threshold;
        float                   _predicted_period;// = -1.0f;
        unsigned int            _edge_mark;// = 0;
        unsigned int            _predict_edge;// = 0;
        unsigned int            _num_pulses; // = 0;
        int                     _half_empty; // 0;
        
        tBACF                   _bacf;
        
    } _tPeriodDetector;
    
    typedef _tPeriodDetector* tPeriodDetector;
    
    void    tPeriodDetector_init    (tPeriodDetector* const detector, float lowestFreq, float highestFreq, float hysteresis, LEAF* const leaf);
    void    tPeriodDetector_initToPool  (tPeriodDetector* const detector, float lowestFreq, float highestFreq, float hysteresis, tMempool* const mempool);
    void    tPeriodDetector_free    (tPeriodDetector* const detector);
    
    int     tPeriodDetector_tick    (tPeriodDetector* const detector, float sample);
    
    // get the periodicity for a given harmonic
    float   tPeriodDetector_getPeriod   (tPeriodDetector* const detector);
    float   tPeriodDetector_getPeriodicity  (tPeriodDetector* const detector);
    float   tPeriodDetector_harmonic    (tPeriodDetector* const detector, int harmonicIndex);
    float   tPeriodDetector_predictPeriod   (tPeriodDetector* const detector);
    int     tPeriodDetector_isReady (tPeriodDetector* const detector);
    int     tPeriodDetector_isReset (tPeriodDetector* const detector);

    void    tPeriodDetector_setHysteresis   (tPeriodDetector* const detector, float hysteresis);
    
    //==============================================================================
    
    /*!
     @defgroup tpitchdetector tPitchDetector
     @ingroup analysis
     @brief
     @{
     
     @fn void    tPitchDetector_init (tPitchDetector* const detector, float lowestFreq, float highestFreq, LEAF* const leaf)
     @brief Initialize a tPitchDetector to the default LEAF mempool.
     @param detector A pointer to the relevant tPitchDetector.
     @param lowestFreq
     @param highestFreq
     
     
     @fn void    tPitchDetector_initToPool   (tPitchDetector* const detector, float lowestFreq, float highestFreq, tMempool* const mempool)
     @brief Initialize a tPitchDetector to a specified mempool.
     @param detector A pointer to the relevant tPitchDetector.
     @param lowestFreq
     @param highestFreq
     @param mempool
     
     @fn void    tPitchDetector_free (tPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn int     tPitchDetector_tick    (tPitchDetector* const detector, float sample)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn float   tPitchDetector_getFrequency    (tPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn float   tPitchDetector_getPeriodicity  (tPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn float   tPitchDetector_harmonic    (tPitchDetector* const detector, int harmonicIndex)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn float   tPitchDetector_predictFrequency (tPitchDetector* const detector)
     @brief
     @param detector A pointer to the relevant tPitchDetector.
     
     @fn void    tPitchDetector_setHysteresis    (tPitchDetector* const detector, float hysteresis)
     @brief Set the hysteresis used in zero crossing detection.
     @param detector A pointer to the relevant tPitchDetector.
     @param hysteresis The hysteresis in decibels. Defaults to -40db.
     ￼￼￼
     @} */
    
#define ONSET_PERIODICITY 0.95f
#define MIN_PERIODICITY 0.9f
#define DEFAULT_HYSTERESIS -40.0f

    typedef struct _pitch_info
    {
        float frequency;
        float periodicity;
    } _pitch_info;
    
    typedef struct _tPitchDetector
    {
        
        tMempool mempool;
        
        tPeriodDetector _pd;
        _pitch_info _current;
        int _frames_after_shift;// = 0;
        
    } _tPitchDetector;
    
    typedef _tPitchDetector* tPitchDetector;
    
    void    tPitchDetector_init (tPitchDetector* const detector, float lowestFreq, float highestFreq, LEAF* const leaf);
    void    tPitchDetector_initToPool   (tPitchDetector* const detector, float lowestFreq, float highestFreq, tMempool* const mempool);
    void    tPitchDetector_free (tPitchDetector* const detector);
    
    int     tPitchDetector_tick    (tPitchDetector* const detector, float sample);
    float   tPitchDetector_getFrequency    (tPitchDetector* const detector);
    float   tPitchDetector_getPeriodicity  (tPitchDetector* const detector);
    float   tPitchDetector_harmonic    (tPitchDetector* const detector, int harmonicIndex);
    float   tPitchDetector_predictFrequency (tPitchDetector* const detector);
    int     tPitchDetector_indeterminate    (tPitchDetector* const detector);
    
    void    tPitchDetector_setHysteresis    (tPitchDetector* const detector, float hysteresis);
    
    

    typedef struct _tDualPitchDetector
    {
        
        tMempool mempool;
        
        tPitchDetector _pd1;
        tPitchDetector _pd2;
        _pitch_info _current;
        float _mean;
        float _predicted_frequency;
        int _first;
        
    } _tDualPitchDetector;
    
    typedef _tDualPitchDetector* tDualPitchDetector;
    
    void    tDualPitchDetector_init (tDualPitchDetector* const detector, float lowestFreq, float highestFreq, LEAF* const leaf);
    void    tDualPitchDetector_initToPool   (tDualPitchDetector* const detector, float lowestFreq, float highestFreq, tMempool* const mempool);
    void    tDualPitchDetector_free (tDualPitchDetector* const detector);
    
    int     tDualPitchDetector_tick    (tDualPitchDetector* const detector, float sample);
    float   tDualPitchDetector_getFrequency    (tDualPitchDetector* const detector);
    float   tDualPitchDetector_getPeriodicity  (tDualPitchDetector* const detector);
    float   tDualPitchDetector_harmonic    (tDualPitchDetector* const detector, int harmonicIndex);
    float   tDualPitchDetector_predictFrequency (tDualPitchDetector* const detector);
    
    void    tDualPitchDetector_setHysteresis    (tDualPitchDetector* const detector, float hysteresis);
    
    
    
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_ANALYSIS_H_INCLUDED

//==============================================================================




