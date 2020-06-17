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
    
    void  tBuffer_init                  (tBuffer* const, uint32_t length);
    void  tBuffer_initToPool            (tBuffer* const, uint32_t length, tMempool* const);
    void  tBuffer_free                  (tBuffer* const);
    
    void  tBuffer_tick                  (tBuffer* const, float sample);
    
    void  tBuffer_read                  (tBuffer* const, float* buff, uint32_t len);
    
    float tBuffer_get                   (tBuffer* const, int idx);
    
    void  tBuffer_record                (tBuffer* const);
    void  tBuffer_stop                  (tBuffer* const);
    int   tBuffer_getRecordPosition     (tBuffer* const);
    void   tBuffer_setRecordPosition     (tBuffer* const, int pos);
    
    void  tBuffer_setRecordMode         (tBuffer* const, RecordMode mode);
    
    void  tBuffer_clear                 (tBuffer* const);
    
    uint32_t tBuffer_getBufferLength    (tBuffer* const);
    uint32_t tBuffer_getRecordedLength  (tBuffer* const sb);
    void 	tBuffer_setRecordedLength	(tBuffer* const sb, int length);
    int 	tBuffer_isActive			(tBuffer* const sb);
    
    //==============================================================================
    
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
    
    void    tSampler_init               (tSampler* const, tBuffer* const);
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
    
    void    tAutoSampler_init               (tAutoSampler* const, tBuffer* const);
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
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_SAMPLING_H_INCLUDED

//==============================================================================
