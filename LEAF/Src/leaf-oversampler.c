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
        int idx = (int)(log2f(os->ratio))-1+offset;
        os->numTaps = firNumTaps[idx];
        os->phaseLength = os->numTaps / os->ratio;
        os->pCoeffs = firCoeffs[idx];
        os->upState = leaf_alloc(sizeof(float) * os->phaseLength);
        os->downState = leaf_alloc(sizeof(float) * os->phaseLength);
    }
}

float tOversampler_tick(tOversampler* const os, float input, float (*effectTick)(float))
{
    float buf[os->ratio];
    
    tOversampler_upsample(os, input, buf);
    
    for (int i = 0; i < os->ratio; ++i) {
        buf[i] = effectTick(buf[i]);
    }
    
    return tOversampler_downsample(os, buf);
}

// From CMSIS DSP Library
void tOversampler_upsample(tOversampler* const os, float input, float* output)
{
    float *pState = os->upState;                 /* State pointer */
    const float *pCoeffs = os->pCoeffs;               /* Coefficient pointer */
    float *pStateCur;
    float *ptr1;                               /* Temporary pointer for state buffer */
    const float *ptr2;                               /* Temporary pointer for coefficient buffer */
    float sum0;                                /* Accumulators */
    uint32_t i, tapCnt;                    /* Loop counters */
    uint32_t phaseLen = os->phaseLength;            /* Length of each polyphase filter component */
    uint32_t j;
    
    /* os->pState buffer contains previous frame (phaseLen - 1) samples */
    /* pStateCur points to the location where the new input data should be written */
    pStateCur = os->upState + (phaseLen - 1U);
    
    /* Copy new input sample into the state buffer */
    *pStateCur = input;
    
    /* Address modifier index of coefficient buffer */
    j = 1U;
    
    /* Loop over the Interpolation factor. */
    i = os->ratio;
    
    while (i > 0U)
    {
        /* Set accumulator to zero */
        sum0 = 0.0f;
        
        /* Initialize state pointer */
        ptr1 = pState;
        
        /* Initialize coefficient pointer */
        ptr2 = pCoeffs + (os->ratio - j);
        
        /* Loop over the polyPhase length.
         Repeat until we've computed numTaps-(4*os->L) coefficients. */
        
        /* Initialize tapCnt with number of samples */
        tapCnt = phaseLen;
        
        while (tapCnt > 0U)
        {
            /* Perform the multiply-accumulate */
            sum0 += *ptr1++ * *ptr2;
            
            /* Upsampling is done by stuffing L-1 zeros between each sample.
             * So instead of multiplying zeros with coefficients,
             * Increment the coefficient pointer by interpolation factor times. */
            ptr2 += os->ratio;
            
            /* Decrement loop counter */
            tapCnt--;
        }
        
        /* The result is in the accumulator, store in the destination buffer. */
        *output++ = sum0 * os->ratio;
        
        /* Increment the address modifier index of coefficient buffer */
        j++;
        
        /* Decrement the loop counter */
        i--;
    }
    
    /* Advance the state pointer by 1
     * to process the next group of interpolation factor number samples */
    pState = pState + 1;
    
    /* Processing is complete.
     Now copy the last phaseLen - 1 samples to the satrt of the state buffer.
     This prepares the state buffer for the next function call. */
    
    /* Points to the start of the state buffer */
    pStateCur = os->upState;

    /* Initialize tapCnt with number of samples */
    tapCnt = (phaseLen - 1U);
    
    /* Copy data */
    while (tapCnt > 0U)
    {
        *pStateCur++ = *pState++;
        
        /* Decrement loop counter */
        tapCnt--;
    }
}

// From CMSIS DSP Library
float tOversampler_downsample(tOversampler *const os, float* input)
{
    float *pState = os->downState;                 /* State pointer */
    const float *pCoeffs = os->pCoeffs;               /* Coefficient pointer */
    float *pStateCur;                          /* Points to the current sample of the state */
    float *px0;                                /* Temporary pointer for state buffer */
    const float *pb;                                 /* Temporary pointer for coefficient buffer */
    float x0, c0;                              /* Temporary variables to hold state and coefficient values */
    float acc0;                                /* Accumulator */
    uint32_t numTaps = os->numTaps;                 /* Number of filter coefficients in the filter */
    uint32_t i, tapCnt;
    float output;
    
    /* os->pState buffer contains previous frame (numTaps - 1) samples */
    /* pStateCur points to the location where the new input data should be written */
    pStateCur = os->downState + (numTaps - 1U);
    
    /* Copy decimation factor number of new input samples into the state buffer */
    i = os->ratio;
    
    do
    {
        *pStateCur++ = *input++;
        
    } while (--i);
    
    /* Set accumulator to zero */
    acc0 = 0.0f;
    
    /* Initialize state pointer */
    px0 = pState;
    
    /* Initialize coeff pointer */
    pb = pCoeffs;
    
    /* Initialize tapCnt with number of taps */
    tapCnt = numTaps;
    
    while (tapCnt > 0U)
    {
        /* Read coefficients */
        c0 = *pb++;
        
        /* Fetch 1 state variable */
        x0 = *px0++;
        
        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;
        
        /* Decrement loop counter */
        tapCnt--;
    }
    
    /* Advance the state pointer by the decimation factor
     * to process the next group of decimation factor number samples */
    pState = pState + os->ratio;
    
    /* The result is in the accumulator, store in the destination buffer. */
    output = acc0;
    
    /* Processing is complete.
     Now copy the last numTaps - 1 samples to the satrt of the state buffer.
     This prepares the state buffer for the next function call. */
    
    /* Points to the start of the state buffer */
    pStateCur = os->downState;
    
    /* Initialize tapCnt with number of taps */
    tapCnt = (numTaps - 1U);
    
    /* Copy data */
    while (tapCnt > 0U)
    {
        *pStateCur++ = *pState++;
        
        /* Decrement loop counter */
        tapCnt--;
    }
    
    return output;
}
