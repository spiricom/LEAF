/*==============================================================================
 
 leaf-sampling.h
 Created: 23 Jan 2019 11:22:09am
 Author:  Mike Mulshine
 
 ==============================================================================*/


#ifndef LEAF_SAMPLING_H_INCLUDED
#define LEAF_SAMPLING_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-global.h"
#include "leaf-math.h"
#include "leaf-mempool.h"
#include "leaf-envelopes.h"
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
     @defgroup tbuffer tBuffer
     @ingroup sampling
     @brief
     @{
     
     @fn void  tBuffer_init                  (tBuffer* const, uint32_t length, LEAF* const leaf)
     @brief
     @param
     
     @fn void  tBuffer_initToPool            (tBuffer* const, uint32_t length, tMempool* const)
     @brief
     @param
     
     @fn void  tBuffer_free                  (tBuffer* const)
     @brief
     @param
     
     @fn void  tBuffer_tick                  (tBuffer* const, float sample)
     @brief
     @param
     
     @fn void  tBuffer_read                  (tBuffer* const, float* buff, uint32_t len)
     @brief
     @param
     
     @fn float tBuffer_get                   (tBuffer* const, int idx)
     @brief
     @param
     
     @fn void  tBuffer_record                (tBuffer* const)
     @brief
     @param
     
     @fn void  tBuffer_stop                  (tBuffer* const)
     @brief
     @param
     
     @fn int   tBuffer_getRecordPosition     (tBuffer* const)
     @brief
     @param
     
     @fn void   tBuffer_setRecordPosition    (tBuffer* const, int pos)
     @brief
     @param
     
     @fn void  tBuffer_setRecordMode         (tBuffer* const, RecordMode mode)
     @brief
     @param
     
     @fn void  tBuffer_clear                 (tBuffer* const)
     @brief
     @param
     
     @fn uint32_t tBuffer_getBufferLength    (tBuffer* const)
     @brief
     @param
     
     @fn uint32_t tBuffer_getRecordedLength  (tBuffer* const sb)
     @brief
     @param
     
     @fn void     tBuffer_setRecordedLength    (tBuffer* const sb, int length)
     @brief
     @param
     
     @fn int     tBuffer_isActive            (tBuffer* const sb)
     @brief
     @param
     
     @} */
    
    typedef enum RecordMode
    {
        RecordOneShot = 0,
        RecordLoop,
        RecordModeNil
    } RecordMode;
    
    typedef struct _tBuffer
    {
        
        tMempool mempool;
        
        float* buff;
        
        uint32_t idx;
        uint32_t bufferLength;
        uint32_t recordedLength;
        RecordMode mode;
        
        int active;
    } _tBuffer;
    
    typedef _tBuffer* tBuffer;
    
    void  tBuffer_init                  (tBuffer* const, uint32_t length, LEAF* const leaf);
    void  tBuffer_initToPool            (tBuffer* const, uint32_t length, tMempool* const);
    void  tBuffer_free                  (tBuffer* const);
    
    void  tBuffer_tick                  (tBuffer* const, float sample);
    void  tBuffer_read                  (tBuffer* const, float* buff, uint32_t len);
    float tBuffer_get                   (tBuffer* const, int idx);
    void  tBuffer_record                (tBuffer* const);
    void  tBuffer_stop                  (tBuffer* const);
    int   tBuffer_getRecordPosition     (tBuffer* const);
    void   tBuffer_setRecordPosition    (tBuffer* const, int pos);
    void  tBuffer_setRecordMode         (tBuffer* const, RecordMode mode);
    void  tBuffer_clear                 (tBuffer* const);
    uint32_t tBuffer_getBufferLength    (tBuffer* const);
    uint32_t tBuffer_getRecordedLength  (tBuffer* const sb);
    void 	tBuffer_setRecordedLength	(tBuffer* const sb, int length);
    int 	tBuffer_isActive			(tBuffer* const sb);
    
    //==============================================================================
    
    /*!
     @defgroup tsampler tSampler
     @ingroup sampling
     @brief
     @{
     
     @fn void    tSampler_init               (tSampler* const, tBuffer* const, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tSampler_initToPool         (tSampler* const, tBuffer* const, tMempool* const)
     @brief
     @param
     
     @fn void    tSampler_free               (tSampler* const)
     @brief
     @param
     
     @fn float   tSampler_tick               (tSampler* const)
     @brief
     @param
     
     @fn void    tSampler_setSample          (tSampler* const, tBuffer* const)
     @brief
     @param
     
     @fn void    tSampler_setMode            (tSampler* const, PlayMode mode)
     @brief
     @param
     
     @fn void    tSampler_play               (tSampler* const)
     @brief
     @param
     
     @fn void    tSampler_stop               (tSampler* const)
     @brief
     @param
     
     @fn void    tSampler_setStart           (tSampler* const, int32_t start)
     @brief
     @param
     
     @fn void    tSampler_setEnd             (tSampler* const, int32_t end)
     @brief
     @param
     
     @fn void    tSampler_setLength             (tSampler* const, int32_t length)
     @brief
     @param
     
     @fn void    tSampler_setCrossfadeLength (tSampler* const sp, uint32_t length)
     @brief
     @param
     
     @fn void    tSampler_setRate            (tSampler* const, float rate)
     @brief
     @param
     
     @} */
    
    typedef enum PlayMode
    {
        PlayNormal,
        PlayLoop,
        PlayBackAndForth,
        PlayModeNil
    } PlayMode;
    
    typedef struct _tSampler
    {
        
        tMempool mempool;
        
        tBuffer samp;
        
        tRamp gain;
        
        float idx;
        float inc;
        float last;
        float iinc;
        int8_t dir;
        int8_t flip;
        int8_t bnf;
        
        int32_t start, targetstart;
        int32_t end, targetend;
        
        uint32_t len;
        uint32_t cfxlen;
        float numticks;
        PlayMode mode;
        int retrigger;
        
        int active;
        
        uint8_t inCrossfade;
        
        float flipStart;
        float flipIdx;
    } _tSampler;
    
    typedef _tSampler* tSampler;
    
    void    tSampler_init               (tSampler* const, tBuffer* const, LEAF* const leaf);
    void    tSampler_initToPool         (tSampler* const, tBuffer* const, tMempool* const);
    void    tSampler_free               (tSampler* const);
    
    float   tSampler_tick               (tSampler* const);
    void    tSampler_setSample          (tSampler* const, tBuffer* const);
    void    tSampler_setMode            (tSampler* const, PlayMode mode);
    void    tSampler_play               (tSampler* const);
    void    tSampler_stop               (tSampler* const);
    void    tSampler_setStart           (tSampler* const, int32_t start);
    void    tSampler_setEnd             (tSampler* const, int32_t end);
    void    tSampler_setLength             (tSampler* const, int32_t length);
    void    tSampler_setCrossfadeLength (tSampler* const sp, uint32_t length);
    void    tSampler_setRate            (tSampler* const, float rate);
    
    //==============================================================================
    
    /*!
     @defgroup tautosampler tAutoSampler
     @ingroup sampling
     @brief
     @{
     
     @fn void    tAutoSampler_init               (tAutoSampler* const, tBuffer* const, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tAutoSampler_initToPool         (tAutoSampler* const, tBuffer* const, tMempool* const)
     @brief
     @param
     
     @fn void    tAutoSampler_free               (tAutoSampler* const)
     @brief
     @param
     
     @fn float   tAutoSampler_tick               (tAutoSampler* const, float input)
     @brief
     @param
     
     @fn void    tAutoSampler_setBuffer          (tAutoSampler* const, tBuffer* const)
     @brief
     @param
     
     @fn void    tAutoSampler_setMode            (tAutoSampler* const, PlayMode mode)
     @brief
     @param
     
     @fn void    tAutoSampler_play               (tAutoSampler* const)
     @brief
     @param
     
     @fn void    tAutoSampler_stop               (tAutoSampler* const)
     @brief
     @param
     
     @fn void    tAutoSampler_setThreshold       (tAutoSampler* const, float thresh)
     @brief
     @param
     
     @fn void    tAutoSampler_setWindowSize      (tAutoSampler* const, uint32_t size)
     @brief
     @param
     
     @fn void    tAutoSampler_setCrossfadeLength (tAutoSampler* const, uint32_t length)
     @brief
     @param
     
     @fn void    tAutoSampler_setRate            (tAutoSampler* const, float rate)
     @brief
     @param
     
     @} */
    
    typedef struct _tAutoSampler
    {
        
        tMempool mempool;
        tSampler sampler;
        tEnvelopeFollower ef;
        uint32_t windowSize;
        float threshold;
        float previousPower;
        uint32_t sampleCounter;
        uint32_t powerCounter;
        uint8_t sampleTriggered;
    } _tAutoSampler;
    
    typedef _tAutoSampler* tAutoSampler;
    
    void    tAutoSampler_init               (tAutoSampler* const, tBuffer* const, LEAF* const leaf);
    void    tAutoSampler_initToPool         (tAutoSampler* const, tBuffer* const, tMempool* const);
    void    tAutoSampler_free               (tAutoSampler* const);
    
    float   tAutoSampler_tick               (tAutoSampler* const, float input);
    void    tAutoSampler_setBuffer          (tAutoSampler* const, tBuffer* const);
    void    tAutoSampler_setMode            (tAutoSampler* const, PlayMode mode);
    void    tAutoSampler_play               (tAutoSampler* const);
    void    tAutoSampler_stop               (tAutoSampler* const);
    void    tAutoSampler_setThreshold       (tAutoSampler* const, float thresh);
    void    tAutoSampler_setWindowSize      (tAutoSampler* const, uint32_t size);
    void    tAutoSampler_setCrossfadeLength (tAutoSampler* const, uint32_t length);
    void    tAutoSampler_setRate            (tAutoSampler* const, float rate);


/*!
     @defgroup tMBSampler tMBSampler
     @ingroup sampling
     @brief
     @{
     
     @fn void    tMBSampler_init               (tMBSampler* const, tBuffer* const, LEAF* const leaf)
     @brief
     @param
     
     @fn void    tMBSampler_initToPool         (tMBSampler* const, tBuffer* const, tMempool* const)
     @brief
     @param
     
     @fn void    tMBSampler_free               (tMBSampler* const)
     @brief
     @param
     
     @fn float   tMBSampler_tick               (tMBSampler* const)
     @brief
     @param
     
     @fn void    tMBSampler_setSample          (tMBSampler* const, tBuffer* const)
     @brief
     @param
     
     @fn void    tMBSampler_setMode            (tMBSampler* const, PlayMode mode)
     @brief
     @param
     
     @fn void    tMBSampler_play               (tMBSampler* const)
     @brief
     @param
     
     @fn void    tMBSampler_stop               (tMBSampler* const)
     @brief
     @param
     
     @fn void    tMBSampler_setStart           (tMBSampler* const, int32_t start)
     @brief
     @param
     
     @fn void    tMBSampler_setEnd             (tMBSampler* const, int32_t end)
     @brief
     @param
     
     @fn void    tMBSampler_setLength             (tMBSampler* const, int32_t length)
     @brief
     @param
     
     @fn void    tMBSampler_setCrossfadeLength (tMBSampler* const sp, uint32_t length)
     @brief
     @param
     
     @fn void    tMBSampler_setRate            (tMBSampler* const, float rate)
     @brief
     @param
     
     @} */
    
#define FILLEN 256

    typedef struct _tMBSampler
    {
        
        tMempool mempool;
        
        tBuffer samp;
        PlayMode mode;
        int active;
        
        tExpSmooth gain;
        
        float    out;
        float    last, beforeLast;
        float    amp;
        float    last_amp;
        float    syncin;
        float   _p, _w, _z;
        float   _last_w;
        float   _f [FILLEN + STEP_DD_PULSE_LENGTH];
        int     _j;
        
        int     start, end;
        int     currentLoopLength;
    } _tMBSampler;
    
    typedef _tMBSampler* tMBSampler;
    
    void    tMBSampler_init               (tMBSampler* const, tBuffer* const, LEAF* const leaf);
    void    tMBSampler_initToPool         (tMBSampler* const, tBuffer* const, tMempool* const);
    void    tMBSampler_free               (tMBSampler* const);
    
    float   tMBSampler_tick               (tMBSampler* const);
    void    tMBSampler_setSample          (tMBSampler* const, tBuffer* const);
    void    tMBSampler_setMode            (tMBSampler* const, PlayMode mode);
    void    tMBSampler_play               (tMBSampler* const);
    void    tMBSampler_stop               (tMBSampler* const);
    void    tMBSampler_setStart           (tMBSampler* const, int32_t start);
    void    tMBSampler_setEnd             (tMBSampler* const, int32_t end);
    void    tMBSampler_setLength          (tMBSampler* const, int32_t length);
    void    tMBSampler_setRate            (tMBSampler* const, float rate);
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_SAMPLING_H_INCLUDED

//==============================================================================
