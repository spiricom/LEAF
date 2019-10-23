//
//  leaf-oversampler.h
//  LEAF
//
//  Created by Matthew Wang and Joshua Becker on 2/28/19.
//  Copyright © 2019 Princeton University. All rights reserved.
//
//==============================================================================

#ifndef LEAF_OVERSAMPLER_H_INCLUDED
#define LEAF_OVERSAMPLER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
//==============================================================================
    
#include "leaf-globals.h"
#include "leaf-math.h"
#include "leaf-filter.h"
    
//==============================================================================

typedef struct _tOversampler
{
    int ratio;
    float* pCoeffs;
    float* upState;
    float* downState;
    int numTaps;
    int phaseLength;
} tOversampler;

void        tOversampler_init(tOversampler* const, int order, oBool extraQuality);
void        tOversampler_free(tOversampler* const);
void        tOversampler_upsample(tOversampler* const, float input, float* output);
float       tOversampler_downsample(tOversampler *const os, float* input);
float       tOversampler_tick(tOversampler* const, float input, float (*effectTick)(float));
int         tOversampler_getLatency(tOversampler* const os);

//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_OVERSAMPLER_H_INCLUDED

//==============================================================================
