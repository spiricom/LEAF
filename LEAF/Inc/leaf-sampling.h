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
#include "leaf-envelopes.h"

//==============================================================================

typedef enum RecordMode
{
    RecordOneShot = 0,
    RecordLoop,
    RecordModeNil
} RecordMode;

typedef struct _tBuffer
{
    float* buff;
    
    uint32_t idx;
    uint32_t length;
    
    RecordMode mode;
    
    int active;
    
} tBuffer;

void  tBuffer_init (tBuffer* const, uint32_t length);
void  tBuffer_free (tBuffer* const);

void  tBuffer_tick (tBuffer* const, float sample);

void  tBuffer_read(tBuffer* const, float* buff, uint32_t len);

float tBuffer_get (tBuffer* const, int idx);

void  tBuffer_record (tBuffer* const);
void  tBuffer_stop (tBuffer* const);

void  tBuffer_setRecordMode (tBuffer* const, RecordMode mode);

void  tBuffer_clear (tBuffer* const);

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
    tBuffer* samp;
    
    tRamp gain;
    
    float idx;
    float inc;
    float last;
    float iinc;
    int8_t dir;
    int8_t flip;
    int8_t bnf;
    
    int32_t start;
    int32_t end;
    uint32_t len;
    uint32_t cfxlen;

    PlayMode mode;
    int retrigger;
    
    int active;

} tSampler;

void    tSampler_init      (tSampler* const, tBuffer* const);
void    tSampler_free      (tSampler* const);

float   tSampler_tick      (tSampler* const);

void    tSampler_setSample (tSampler* const, tBuffer* s);

void    tSampler_setMode   (tSampler* const, PlayMode mode);

void    tSampler_play      (tSampler* const);
void    tSampler_stop      (tSampler* const);

void    tSampler_setStart  (tSampler* const, int32_t start);
void    tSampler_setEnd    (tSampler* const, int32_t end);

void    tSampler_setCrossfadeLength  (tSampler* const p, uint32_t length);

void    tSampler_setRate   (tSampler* const, float rate);

//==============================================================================

#ifdef __cplusplus
}
#endif

#endif // LEAF_SAMPLING_H_INCLUDED

//==============================================================================
