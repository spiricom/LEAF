/*==============================================================================

    leaf-formant.h
    Created: 30 Nov 2018 11:03:37am
    Author:  airship

==============================================================================*/

#ifndef LEAF_FORMANT_H_INCLUDED
#define LEAF_FORMANT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
    
#include "leaf-global.h"
#include "leaf-mempool.h"
#include "leaf-math.h"

//==============================================================================
    
#define FORD 7
#define FORMANT_BUFFER_SIZE 2048

typedef struct _tFormantShifter
{
    int ford;
    int bufsize;
    float falph;
    float flamb;
    float* fk;
    float* fb;
    float* fc;
    float* frb;
    float* frc;
    float* fsig;
    float* fsmooth;
    float fhp;
    float flp;
    float flpa;
    float** fbuff;
    float* ftvec;
    float fmute;
    float fmutealph;
    unsigned int cbi;
    
} tFormantShifter;

void        tFormantShifter_init            (tFormantShifter* const, int bufsize, int order);
void        tFormantShifter_free            (tFormantShifter* const);

float       tFormantShifter_tick            (tFormantShifter* const, float input, float fwarp);
float       tFormantShifter_remove          (tFormantShifter* const, float input);
float       tFormantShifter_add             (tFormantShifter* const, float input, float fwarp);
void        tFormantShifter_ioSamples       (tFormantShifter* const, float* in, float* out, int size, float fwarp);
    
//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_FORMANT_H_INCLUDED

//==============================================================================

