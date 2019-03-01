//
//  leaf-oversampler.c
//  LEAF
//
//  Created by Matthew Wang and Joshua Becker on 2/28/19.
//  Copyright © 2019 Princeton University. All rights reserved.
//

#if _WIN32 || _WIN64

#include "..\Inc\leaf-oversampler.h"
#else

#include "../Inc/leaf-oversampler.h"

#endif

// Currently just using a double sample rate version of SVF from leaf-filter.c, may want to implement better filter for oversampling.
// Quick testing seems to indicate that this filter sufficiently handles most aliasing.
void tOversamplerFilter_init(tOversamplerFilter* const lpf, float freq, float Q, float sampleRateMultiplier)
{
    lpf->ic1eq = 0;
    lpf->ic2eq = 0;
    
    float a1,a2,a3,g,k;
    float scaledInverseSampleRate = leaf.invSampleRate * (1 / sampleRateMultiplier);
    
    g = tanf(PI * freq * scaledInverseSampleRate);
    k = 1.0f/Q;
    a1 = 1.0f/(1.0f+g*(g+k));
    a2 = g*a1;
    a3 = g*a2;
    
    lpf->g = g;
    lpf->k = k;
    lpf->a1 = a1;
    lpf->a2 = a2;
    lpf->a3 = a3;
}

float tOversamplerFilter_tick(tOversamplerFilter* const lpf, float v0)
{
    float v1,v2,v3;
    v3 = v0 - lpf->ic2eq;
    v1 = (lpf->a1 * lpf->ic1eq) + (lpf->a2 * v3);
    v2 = lpf->ic2eq + (lpf->a2 * lpf->ic1eq) + (lpf->a3 * v3);
    lpf->ic1eq = (2.0f * v1) - lpf->ic1eq;
    lpf->ic2eq = (2.0f * v2) - lpf->ic2eq;
    
    return v2;
}

// 2X Oversampler
void tOversampler2x_init(tOversampler2x* const os)
{
    tOversamplerFilter_init(&os->filters[0], leaf.sampleRate*0.5f, 0.1f, 2.f);
    tOversamplerFilter_init(&os->filters[1], leaf.sampleRate*0.5f, 0.1f, 2.f);
}

float tOversampler2x_tick(tOversampler2x* const os, float input, float (*nonLinearTick)(float))
{
    float sample = input;
    float oversample = 0.f;
    
    sample = tOversamplerFilter_tick(&os->filters[0], sample);
    oversample = tOversamplerFilter_tick(&os->filters[0], oversample);
    
    sample = nonLinearTick(sample);
    oversample = nonLinearTick(oversample);
    
    sample = tOversamplerFilter_tick(&os->filters[1], sample);
    oversample = tOversamplerFilter_tick(&os->filters[1], oversample);
    
    return sample;
}

// 4X Oversampler
void tOversampler4x_init(tOversampler4x* const os)
{
    tOversamplerFilter_init(&os->filters[0], leaf.sampleRate*0.5f, 0.1f, 2.f);
    tOversamplerFilter_init(&os->filters[1], leaf.sampleRate*0.5f, 0.1f, 4.f);
    tOversamplerFilter_init(&os->filters[2], leaf.sampleRate*0.5f, 0.1f, 4.f);
    tOversamplerFilter_init(&os->filters[3] , leaf.sampleRate*0.5f, 0.1f, 2.f);
}

float tOversampler4x_tick(tOversampler4x* const os, float input, float (*nonLinearTick)(float))
{
    float sample = input;
    float oversample1 = 0.f;
    float oversample2 = 0.f;
    float oversample3 = 0.f;
    // Phase 1:
    //  x = [sample, oversample2]
    //  lpf(x)
    // Phase 2:
    //  x = [sample, oversample1, oversample2, oversample3]
    //  lpf(dist(lpf(x)))
    // Phase 3:
    //  x = [sample, oversample2]
    //  lpf(x)
    
    sample = tOversamplerFilter_tick(&os->filters[0], sample);
    oversample2 = tOversamplerFilter_tick(&os->filters[0], oversample2);
    
    sample = tOversamplerFilter_tick(&os->filters[1], sample);
    oversample1 = tOversamplerFilter_tick(&os->filters[1], oversample1);
    oversample2 = tOversamplerFilter_tick(&os->filters[1], oversample2);
    oversample3 = tOversamplerFilter_tick(&os->filters[1], oversample3);
    
    sample = nonLinearTick(sample);
    oversample1 = nonLinearTick(oversample1);
    oversample2 = nonLinearTick(oversample2);
    oversample3 = nonLinearTick(oversample3);
    
    sample = tOversamplerFilter_tick(&os->filters[2], sample);
    oversample1 = tOversamplerFilter_tick(&os->filters[2], oversample1);
    oversample2 = tOversamplerFilter_tick(&os->filters[2], oversample2);
    oversample3 = tOversamplerFilter_tick(&os->filters[2], oversample3);
    
    sample = tOversamplerFilter_tick(&os->filters[3], sample);
    oversample2 = tOversamplerFilter_tick(&os->filters[3], oversample2);
    
    return sample;
}
