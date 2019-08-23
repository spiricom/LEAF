//
//  leaf-oversampler.c
//  LEAF
//
//  Created by Matthew Wang and Joshua Becker on 2/28/19.
//  Copyright © 2019 Princeton University. All rights reserved.
//

#if _WIN32 || _WIN64

#include "..\Inc\leaf-oversampler.h"
#include "..\Inc\leaf-tables.h"
#else


#include "../Inc/leaf-oversampler.h"
#include "../Inc/leaf-tables.h"

#endif

// Oversampler
void tOversampler_init(tOversampler* const os, int ratio, oBool extraQuality)
{
    uint8_t offset = 0;
    if (extraQuality) offset = 6;
    if (ratio == 2 || ratio == 4  ||
        ratio == 8 || ratio == 16 ||
        ratio == 32 || ratio == 64) {
        os->ratio = ratio;
        tFIR_init(&os->firUp, firCoeffs[(int)(os->ratio*0.5f)-1+offset]);
        tFIR_init(&os->firDown, firCoeffs[(int)(os->ratio*0.5f)-1+offset]);
    }
}

void tOversampler_upsample(tOversampler* const os, float input, float* output)
{
    output[0] = input;
    for (int i = 0; i < os->ratio; ++i) {
        if (i > 0) output[i] = 0;
        output[i] = tFIR_tick(&os->firUp, output[i]);
    }
}

float tOversampler_downsample(tOversampler* const os, float* input)
{
    float output = tFIR_tick(&os->firDown, input[0]);
    for (int i = 1; i < os->ratio; ++i) {
        tFIR_tick(&os->firDown, input[i]);
    }
    return output;
}

float tOversampler_tick(tOversampler* const os, float input, float (*effectTick)(float))
{
    float buf[os->ratio];
    
    tOversampler_upsample(os, input, buf);
    
    for (int i = 0; i < os->ratio; ++i) {
        buf[i] = effectTick(buf[i]);
    }
    
    return tOversampler_downsample(os, buf);
    
//    float samples[os->ratio];
//    samples[0] = input;
//    for (int i = 1; i < os->ratio; ++i) {
//        samples[i]= 0.f;
//    }
//
//    int whichFilter = 0;
//    int i = 0;
//    int j = 0;
//    for (; i < os->order; ++i) {
//        for (; j < exp2(i+1); j += os->order-i) {
//            samples[j] = tFIR_tick(&os->filters[whichFilter], samples[j]);
//        }
//        whichFilter++;
//    }
//
//    for (int s = 0; s < os->ratio; ++s) {
//        samples[s] = effectTick(samples[s]);
//    }
//
//    for (; i >= 0; --i) {
//        for (; j >= 0; j -= os->order-i) {
//            samples[j] = tFIR_tick(&os->filters[whichFilter], samples[j]);
//        }
//        whichFilter++;
//    }
}
