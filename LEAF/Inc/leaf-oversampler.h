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
typedef struct _tOversamplerFilter
{
    float cutoff, Q;
    float ic1eq,ic2eq;
    float g,k,a1,a2,a3;
} tOversamplerFilter;

typedef struct _tOversampler2x
{
    tOversamplerFilter filters[2];
} tOversampler2x;

void        tOversampler2x_init(tOversampler2x* const);
float       tOversampler2x_tick(tOversampler2x* const, float input, float (*nonLinearTick)(float));

typedef struct _tOversampler4x
{
    tOversamplerFilter filters[4];
} tOversampler4x;

void        tOversampler4x_init(tOversampler4x* const);
float       tOversampler4x_tick(tOversampler4x* const, float input, float (*nonLinearTick)(float));
    
//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_OVERSAMPLER_H_INCLUDED

//==============================================================================
