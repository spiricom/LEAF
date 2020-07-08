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
     @brief
     @{
     */
    
    /*!
     @fn void    tEnvelopeFollower_init          (tEnvelopeFollower* const, float attackThreshold, float decayCoeff)
     @brief
     @param
     */
    
    /*!
     @fn void    tEnvelopeFollower_initToPool    (tEnvelopeFollower* const, float attackThreshold, float decayCoeff, tMempool* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tEnvelopeFollower_free          (tEnvelopeFollower* const)
     @brief
     @param
     */
    
    /*!
     @fn float   tEnvelopeFollower_tick          (tEnvelopeFollower* const, float x)
     @brief
     @param
     */
    
    /*!
     @fn int     tEnvelopeFollower_decayCoeff    (tEnvelopeFollower* const, float decayCoeff)
     @brief
     @param
     */
    
    /*!
     @fn int     tEnvelopeFollower_attackThresh  (tEnvelopeFollower* const, float attackThresh)
     @brief
     @param
     */
    
    /*! @} */
    
    typedef struct _tEnvelopeFollower
    {
        tMempool mempool;
        float y;
        float a_thresh;
        float d_coeff;
        
    } _tEnvelopeFollower;
    
    typedef _tEnvelopeFollower* tEnvelopeFollower;
    
    void    tEnvelopeFollower_init          (tEnvelopeFollower* const, float attackThreshold, float decayCoeff);
    void    tEnvelopeFollower_initToPool    (tEnvelopeFollower* const, float attackThreshold, float decayCoeff, tMempool* const);
    void    tEnvelopeFollower_free          (tEnvelopeFollower* const);
    
    float   tEnvelopeFollower_tick          (tEnvelopeFollower* const, float x);
    int     tEnvelopeFollower_decayCoeff    (tEnvelopeFollower* const, float decayCoeff);
    int     tEnvelopeFollower_attackThresh  (tEnvelopeFollower* const, float attackThresh);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tzerocrossing tZeroCrossing
     @ingroup analysis
     @brief
     @{
     */
    
    /*!
     @fn void    tZeroCrossing_init         (tZeroCrossing* const, int maxWindowSize)
     @brief
     @param
     */
    
    /*!
     @fn void    tZeroCrossing_initToPool   (tZeroCrossing* const, int maxWindowSize, tMempool* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tZeroCrossing_free         (tZeroCrossing* const)
     @brief
     @param
     */
    
    /*!
     @fn float   tZeroCrossing_tick         (tZeroCrossing* const, float input)
     @brief
     @param
     */
    
    /*!
     @fn void    tZeroCrossing_setWindow        (tZeroCrossing* const, float windowSize)
     @brief
     @param
     */
    
    /*! @} */
    
    /* Zero Crossing Detector */
    typedef struct _tZeroCrossing {
        tMempool mempool;
        int count;
        int maxWindowSize;
        int currentWindowSize;
        float invCurrentWindowSize;
        float* inBuffer;
        uint16_t* countBuffer;
        int prevPosition;
        int position;
    } _tZeroCrossing;
    
    typedef _tZeroCrossing* tZeroCrossing;
    
    void    tZeroCrossing_init         (tZeroCrossing* const, int maxWindowSize);
    void    tZeroCrossing_initToPool   (tZeroCrossing* const, int maxWindowSize, tMempool* const);
    void    tZeroCrossing_free         (tZeroCrossing* const);
    
    float   tZeroCrossing_tick         (tZeroCrossing* const, float input);
    void    tZeroCrossing_setWindow        (tZeroCrossing* const, float windowSize);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tpowerfollower tPowerFollower
     @ingroup analysis
     @brief
     @{
     */
    
    /*!
     @fn void    tPowerFollower_init         (tPowerFollower* const, float factor)
     @brief
     @param
     */
    
    /*!
     @fn void    tPowerFollower_initToPool   (tPowerFollower* const, float factor, tMempool* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tPowerFollower_free         (tPowerFollower* const)
     @brief
     @param
     */
    
    /*!
     @fn float   tPowerFollower_tick         (tPowerFollower* const, float input)
     @brief
     @param
     */
    
    /*!
     @fn float   tPowerFollower_sample       (tPowerFollower* const)
     @brief
     @param
     */
    
    /*!
     @fn int     tPowerFollower_setFactor    (tPowerFollower* const, float factor)
     @brief
     @param
     */
    
    /*! @} */
    
    /* PowerEnvelopeFollower */
    typedef struct _tPowerFollower
    {
        tMempool mempool;
        float factor, oneminusfactor;
        float curr;
        
    } _tPowerFollower;
    
    typedef _tPowerFollower* tPowerFollower;
    
    void    tPowerFollower_init         (tPowerFollower* const, float factor);
    void    tPowerFollower_initToPool   (tPowerFollower* const, float factor, tMempool* const);
    void    tPowerFollower_free         (tPowerFollower* const);
    
    float   tPowerFollower_tick         (tPowerFollower* const, float input);
    float   tPowerFollower_sample       (tPowerFollower* const);
    int     tPowerFollower_setFactor    (tPowerFollower* const, float factor);
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    /*!
     @defgroup tenvpd tEnvPD
     @ingroup analysis
     @brief
     @{
     */
    
    /*!
     @fn void    tEnvPD_init             (tEnvPD* const, int windowSize, int hopSize, int blockSize)
     @brief
     @param
     */
    
    /*!
     @fn void    tEnvPD_initToPool       (tEnvPD* const, int windowSize, int hopSize, int blockSize, tMempool* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tEnvPD_free             (tEnvPD* const)
     @brief
     @param
     */
    
    
    /*!
     @fn float   tEnvPD_tick             (tEnvPD* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tEnvPD_processBlock     (tEnvPD* const, float* in)
     @brief
     @param
     */
    
    /*! @} */
    
    // ENV~ from PD, modified for LEAF
#define MAXOVERLAP 32
#define INITVSTAKEN 64
#define ENV_WINDOW_SIZE 1024
#define ENV_HOP_SIZE 256
    
    typedef struct _tEnvPD
    {
        tMempool mempool;
        float buf[ENV_WINDOW_SIZE + INITVSTAKEN];
        uint16_t x_phase;                    /* number of points since last output */
        uint16_t x_period;                   /* requested period of output */
        uint16_t x_realperiod;               /* period rounded up to vecsize multiple */
        uint16_t x_npoints;                  /* analysis window size in samples */
        float x_result;                 /* result to output */
        float x_sumbuf[MAXOVERLAP];     /* summing buffer */
        float x_f;
        uint16_t windowSize, hopSize, blockSize;
        uint16_t x_allocforvs;               /* extra buffer for DSP vector size */
    } _tEnvPD;
    
    typedef _tEnvPD* tEnvPD;
    
    void    tEnvPD_init             (tEnvPD* const, int windowSize, int hopSize, int blockSize);
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
     */
    
    /*!
     @fn void    tAttackDetection_init           (tAttackDetection* const, int blocksize, int atk, int rel)
     @brief
     @param
     */
    
    /*!
     @fn void    tAttackDetection_initToPool     (tAttackDetection* const, int blocksize, int atk, int rel, tMempool* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tAttackDetection_free           (tAttackDetection* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tAttackDetection_setBlocksize   (tAttackDetection* const, int size)
     @brief Set expected input blocksize
     @param
     */
    
    /*!
     @fn void    tAttackDetection_setSamplerate  (tAttackDetection* const, int inRate)
     @brief Set attack detection sample rate
     @param
     */
    
    /*!
     @fn void    tAttackDetection_setAttack      (tAttackDetection* const, int inAtk)
     @brief Set attack time and coeff
     @param
     */
    
    /*!
     @fn void    tAttackDetection_setRelease     (tAttackDetection* const, int inRel)
     @brief Set release time and coeff
     @param
     */
    
    /*!
     @fn void    tAttackDetection_setThreshold   (tAttackDetection* const, float thres)
     @brief Set level above which values are identified as attacks
     @param
     */
    
    /*!
     @fn int     tAttackDetection_detect         (tAttackDetection* const, float *in)
     @brief Find the largest transient in input block, return index of attack
     @param
     */
    
    /*! @} */
    
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
    
    void    tAttackDetection_init           (tAttackDetection* const, int blocksize, int atk, int rel);
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
     */
    
    /*!
     @fn void    tSNAC_init          (tSNAC* const, int overlaparg)
     @brief
     @param
     */
    
    /*!
     @fn void    tSNAC_initToPool    (tSNAC* const, int overlaparg, tMempool* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tSNAC_free          (tSNAC* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tSNAC_ioSamples     (tSNAC *s, float *in, float *out, int size)
     @brief
     @param
     */
    
    /*!
     @fn void    tSNAC_setOverlap    (tSNAC *s, int lap)
     @brief
     @param
     */
    
    /*!
     @fn void    tSNAC_setBias       (tSNAC *s, float bias)
     @brief
     @param
     */
    
    /*!
     @fn void    tSNAC_setMinRMS     (tSNAC *s, float rms)
     @brief
     @param
     */
    
    /*!
     @fn float   tSNAC_getPeriod     (tSNAC *s)
     @brief
     @param
     */
    
    /*!
     @fn float   tSNAC_getFidelity   (tSNAC *s)
     @brief
     @param
     */
    
    /*! @} */
    
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
    
    void    tSNAC_init          (tSNAC* const, int overlaparg);
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
     @fn void    tPeriodDetection_init               (tPeriodDetection* const, float* in, float* out, int bufSize, int frameSize)
     @brief
     @param
     */
    
    /*!
     @fn void    tPeriodDetection_initToPool         (tPeriodDetection* const, float* in, float* out, int bufSize, int frameSize, tMempool* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tPeriodDetection_free               (tPeriodDetection* const)
     @brief
     @param
     */
    
    /*!
     @fn float   tPeriodDetection_tick               (tPeriodDetection* const, float sample)
     @brief
     @param
     */
    
    /*!
     @fn float   tPeriodDetection_getPeriod          (tPeriodDetection* const)
     @brief
     @param
     */
    
    /*!
     @fn void    tPeriodDetection_setHopSize         (tPeriodDetection* const, int hs)
     @brief
     @param
     */
    
    /*!
     @fn void    tPeriodDetection_setWindowSize      (tPeriodDetection* const, int ws)
     @brief
     @param
     */
    
    /*!
     @fn void    tPeriodDetection_setFidelityThreshold(tPeriodDetection* const, float threshold)
     @brief
     @param
     */
    
    /*!
     @fn void    tPeriodDetection_setAlpha           (tPeriodDetection* const, float alpha)
     @brief
     @param
     */
    
    /*!
     @fn void    tPeriodDetection_setTolerance       (tPeriodDetection* const, float tolerance)
     @brief
     @param
     */
    
    /*! @} */
    
#define DEFPITCHRATIO 2.0f
#define DEFTIMECONSTANT 100.0f
#define DEFHOPSIZE 64
#define DEFWINDOWSIZE 64
#define FBA 20
#define HPFREQ 40.0f
    
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
    
    void    tPeriodDetection_init               (tPeriodDetection* const, float* in, float* out, int bufSize, int frameSize);
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
    
    typedef struct _tZeroCrossing2
    {
        tMempool mempool;
        
        float _before_crossing;
        float _after_crossing;
        
        float             _peak;
        int               _leading_edge;// = undefined_edge; int_min
        int               _trailing_edge;// = undefined_edge;
        float             _width;// = 0.0f;
    } _tZeroCrossing2;
    
    typedef _tZeroCrossing2* tZeroCrossing2;
    
    void    tZeroCrossing2_init  (tZeroCrossing2* const);
    void    tZeroCrossing2_initToPool    (tZeroCrossing2* const, tMempool* const);
    void    tZeroCrossing2_free  (tZeroCrossing2* const);
    
    int     tZeroCrossing2_tick(tZeroCrossing2* const, float s);
    int     tZeroCrossing2_getState(tZeroCrossing2* const);
    void    tZeroCrossing2_updatePeak(tZeroCrossing2* const, float s, int pos);
    int     tZeroCrossing2_period(tZeroCrossing2* const, tZeroCrossing2* const next);
    float   tZeroCrossing2_fractionalPeriod(tZeroCrossing2* const, tZeroCrossing2* const next);
    int     tZeroCrossing2_getWidth(tZeroCrossing2* const);
    int     tZeroCrossing2_isSimilar(tZeroCrossing2* const, tZeroCrossing2* const next);
    
    //==============================================================================
    
#define PULSE_HEIGHT_DIFF 0.8
#define PULSE_WIDTH_DIFF 0.85
    
    typedef struct _tZeroCrossings
    {
        tMempool mempool;
        
        tZeroCrossing2* _info;
        unsigned int _size;
        unsigned int _pos;
        unsigned int _mask;
        
        float                _prev;// = 0.0f;
        float                _hysteresis;
        bool                 _state;// = false;
        int                  _num_edges;// = 0;
        int                  _window_size;
        int                  _frame;// = 0;
        bool                 _ready;// = false;
        float                _peak_update;// = 0.0f;
        float                _peak;// = 0.0f;
    } _tZeroCrossings;
    
    typedef _tZeroCrossings* tZeroCrossings;
    
    void    tZeroCrossings_init  (tZeroCrossings* const, int windowSize, float hysteresis);
    void    tZeroCrossings_initToPool    (tZeroCrossings* const, int windowSize, float hysteresis, tMempool* const);
    void    tZeroCrossings_free  (tZeroCrossings* const);
    
    int     tZeroCrossings_tick(tZeroCrossings* const, float s);
    int     tZeroCrossings_getState(tZeroCrossings* const);
    
    int     tZeroCrossings_getNumEdges(tZeroCrossings* const zc);
    int     tZeroCrossings_getCapacity(tZeroCrossings* const zc);
    int     tZeroCrossings_getFrame(tZeroCrossings* const zc);
    int     tZeroCrossings_getWindowSize(tZeroCrossings* const zc);
    
    int     tZeroCrossings_isReady(tZeroCrossings* const zc);
    float   tZeroCrossings_getPeak(tZeroCrossings* const zc);
    int     tZeroCrossings_isReset(tZeroCrossings* const zc);
    
    tZeroCrossing2 const tZeroCrossings_getCrossing(tZeroCrossings* const zc, int index);
    
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
    
    void    tBitset_init    (tBitset* const bitset, int numBits);
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
    
    void    tBACF_init  (tBACF* const bacf, tBitset* const bitset);
    void    tBACF_initToPool    (tBACF* const bacf, tBitset* const bitset, tMempool* const mempool);
    void    tBACF_free  (tBACF* const bacf);
    
    int    tBACF_tick  (tBACF* const bacf, int pos);
    
    //==============================================================================
    
#define PULSE_THRESHOLD 0.6f
#define HARMONIC_PERIODICITY_FACTOR 16
#define PERIODICITY_DIFF_FACTOR 0.008f
    
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
        tZeroCrossing           _zc;
        
        float             _harmonic_threshold;
        float             _periodicity_diff_threshold;
        int               _range;
    } _sub_collector;

    typedef struct _tPeriodDetector
    {
        tMempool mempool;
        
        tZeroCrossings          _zc;
        float                   _period;
        float                   _periodicity;
        unsigned int            _min_period;
        int                     _range;
        tBitset                 _bits;
        float                   _weight;
        unsigned int            _mid_point;
        float                   _periodicity_diff_threshold;
        float                   _predicted_period;// = -1.0f;
        unsigned int            _edge_mark;// = 0;
        unsigned int            _predict_edge;// = 0;
    } _tPeriodDetector;
    
    typedef _tPeriodDetector* tPeriodDetector;
    
    void    tPeriodDetector_init    (tPeriodDetector* const detector, float lowestFreq, float highestFreq, float hysteresis);
    void    tPeriodDetector_initToPool  (tPeriodDetector* const detector, float lowestFreq, float highestFreq, float hysteresis, tMempool* const mempool);
    void    tPeriodDetector_free    (tPeriodDetector* const detector);
    
    float   tPeriodDetector_tick    (tPeriodDetector* const detector, float sample);
    
    
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_ANALYSIS_H_INCLUDED

//==============================================================================



