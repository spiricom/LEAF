//
//  leaf-distortion.c
//  LEAF
//
//  Created by Jeff Snyder, Matthew Wang, Michael Mulshine, and Joshua Becker
//  Copyright © 2019 Princeton University. All rights reserved.
//

#if _WIN32 || _WIN64

#include "..\Inc\leaf-distortion.h"
#include "..\Inc\leaf-tables.h"
#else


#include "../Inc/leaf-distortion.h"
#include "../Inc/leaf-tables.h"

//testing
//#include "gpio.h"

#endif

//============================================================================================================
// Sample-Rate reducer
//============================================================================================================


void tSampleReducer_init(tSampleReducer* const sr)
{
    _tSampleReducer* s = *sr = (_tSampleReducer*) leaf_alloc(sizeof(_tSampleReducer));
    
    s->invRatio = 1.0f;
    s->hold = 0.0f;
    s->count = 0;
}

void    tSampleReducer_free    (tSampleReducer* const sr)
{
    _tSampleReducer* s = *sr;
    
    leaf_free(s);
}

void    tSampleReducer_initToPool   (tSampleReducer* const sr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSampleReducer* s = *sr = (_tSampleReducer*) mpool_alloc(sizeof(_tSampleReducer), &m->pool);
    
    s->invRatio = 1.0f;
    s->hold = 0.0f;
    s->count = 0;
}

void    tSampleReducer_freeFromPool (tSampleReducer* const sr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSampleReducer* s = *sr;
    
    mpool_free(s, &m->pool);
}

float tSampleReducer_tick(tSampleReducer* const sr, float input)
{
    _tSampleReducer* s = *sr;
    if (s->count > s->invRatio)
    {
        s->hold = input;
        s->count = 0;
    }
    
    s->count++;
    return s->hold;
}


void tSampleReducer_setRatio(tSampleReducer* const sr, float ratio)
{
    _tSampleReducer* s = *sr;
    if ((ratio <= 1.0f) && (ratio >= 0.0f))
        s->invRatio = 1.0f / ratio;
    
}

//============================================================================================================
// Oversampler
//============================================================================================================
// Latency is equal to the phase length (numTaps / ratio)
void tOversampler_init(tOversampler* const osr, int ratio, oBool extraQuality)
{
    _tOversampler* os = *osr = (_tOversampler*) leaf_alloc(sizeof(_tOversampler));
    
    uint8_t offset = 0;
    if (extraQuality) offset = 6;
    if (ratio == 2 || ratio == 4  ||
        ratio == 8 || ratio == 16 ||
        ratio == 32 || ratio == 64) {
        os->ratio = ratio;
        int idx = (int)(log2f(os->ratio))-1+offset;
        os->numTaps = firNumTaps[idx];
        os->phaseLength = os->numTaps / os->ratio;
        os->pCoeffs = (float*) firCoeffs[idx];
        os->upState = leaf_alloc(sizeof(float) * os->numTaps * 2);
        os->downState = leaf_alloc(sizeof(float) * os->numTaps * 2);
    }
}

void tOversampler_free(tOversampler* const osr)
{
    _tOversampler* os = *osr;
    
    leaf_free(os->upState);
    leaf_free(os->downState);
    leaf_free(os);
}

void    tOversampler_initToPool     (tOversampler* const osr, int ratio, oBool extraQuality, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tOversampler* os = *osr = (_tOversampler*) mpool_alloc(sizeof(_tOversampler), &m->pool);
    
    uint8_t offset = 0;
    if (extraQuality) offset = 6;
    if (ratio == 2 || ratio == 4  ||
        ratio == 8 || ratio == 16 ||
        ratio == 32 || ratio == 64) {
        os->ratio = ratio;
        int idx = (int)(log2f(os->ratio))-1+offset;
        os->numTaps = firNumTaps[idx];
        os->phaseLength = os->numTaps / os->ratio;
        os->pCoeffs = (float*) firCoeffs[idx];
        os->upState = mpool_alloc(sizeof(float) * os->numTaps * 2, &m->pool);
        os->downState = mpool_alloc(sizeof(float) * os->numTaps * 2, &m->pool);
    }
}

void    tOversampler_freeFromPool   (tOversampler* const osr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tOversampler* os = *osr;
    
    mpool_free(os->upState, &m->pool);
    mpool_free(os->downState, &m->pool);
    mpool_free(os, &m->pool);
}

float tOversampler_tick(tOversampler* const osr, float input, float (*effectTick)(float))
{
    _tOversampler* os = *osr;
    
    float buf[os->ratio];
    
    tOversampler_upsample(osr, input, buf);
    
    for (int i = 0; i < os->ratio; ++i) {
        buf[i] = effectTick(buf[i]);
    }
    
    return tOversampler_downsample(osr, buf);
}

// From CMSIS DSP Library
void tOversampler_upsample(tOversampler* const osr, float input, float* output)
{
    _tOversampler* os = *osr;
    
    float *pState = os->upState;                 /* State pointer */
    float *pCoeffs = os->pCoeffs;               /* Coefficient pointer */
    float *pStateCur;
    float *ptr1;                               /* Temporary pointer for state buffer */
    float *ptr2;                               /* Temporary pointer for coefficient buffer */
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
float tOversampler_downsample(tOversampler *const osr, float* input)
{
    _tOversampler* os = *osr;
    
    float *pState = os->downState;                 /* State pointer */
    float *pCoeffs = os->pCoeffs;               /* Coefficient pointer */
    float *pStateCur;                          /* Points to the current sample of the state */
    float *px0;                                /* Temporary pointer for state buffer */
    float *pb;                                 /* Temporary pointer for coefficient buffer */
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
     Now copy the last numTaps - 1 samples to the start of the state buffer.
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

int tOversampler_getLatency(tOversampler* const osr)
{
    _tOversampler* os = *osr;
    return os->phaseLength;
}

//============================================================================================================
// WAVEFOLDER
//============================================================================================================


//from the paper: Virtual Analog Model of the Lockhart Wavefolder
//by Fabián Esqueda, Henri Pöntynen, Julian D. Parker and Stefan Bilbao

void tLockhartWavefolder_init(tLockhartWavefolder* const wf)
{
    _tLockhartWavefolder* w = *wf = (_tLockhartWavefolder*) leaf_alloc(sizeof(_tLockhartWavefolder));
    
    w->Ln1 = 0.0;
    w->Fn1 = 0.0;
    w->xn1 = 0.0f;

    w->RL = 7.5e3;
    w->R = 15e3;
    w->VT = 26e-3;
    w->Is = 10e-16;

    w->a = 2.0*w->RL/w->R;
    w->b = (w->R+2.0*w->RL)/(w->VT*w->R);
    w->d = (w->RL*w->Is)/w->VT;
    w->half_a = 0.5 * w->a;
    w->longthing = (0.5*w->VT/w->b);


    // Antialiasing error threshold
    w->thresh = 10e-10;
}

void tLockhartWavefolder_free(tLockhartWavefolder* const wf)
{
    _tLockhartWavefolder* w = *wf;
    
    leaf_free(w);
}

void    tLockhartWavefolder_initToPool   (tLockhartWavefolder* const wf, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tLockhartWavefolder* w = *wf = (_tLockhartWavefolder*) mpool_alloc(sizeof(_tLockhartWavefolder), &m->pool);
    
    w->Ln1 = 0.0;
    w->Fn1 = 0.0;
    w->xn1 = 0.0f;
    
    w->RL = 7.5e3;
    w->R = 15e3;
    w->VT = 26e-3;
    w->Is = 10e-16;
    
    w->a = 2.0*w->RL/w->R;
    w->b = (w->R+2.0*w->RL)/(w->VT*w->R);
    w->d = (w->RL*w->Is)/w->VT;
    w->half_a = 0.5 * w->a;
    w->longthing = (0.5*w->VT/w->b);
    
    
    // Antialiasing error threshold
    w->thresh = 10e-10;
}

void    tLockhartWavefolder_freeFromPool (tLockhartWavefolder* const wf, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tLockhartWavefolder* w = *wf;
    
    mpool_free(w, &m->pool);
}

double tLockhartWavefolderLambert(double x, double ln)
{
    double thresh, w, expw, p, r, s, err;
    // Error threshold
    thresh = 10e-12;
    // Initial guess (use previous value)
    w = ln;
    
    // Haley's method (Sec. 4.2 of the paper)
    for(int i=0; i<1000; i+=1) {
        
        expw = exp(w);
        
        p = w*expw - x;
        r = (w+1.0)*expw;
        s = (w+2.0)/(2.0*(w+1.0));
        err = (p/(r-(p*s)));
        
        if (fabs(err)<thresh) {

        	break;
        }
        
        w = w - err;
        if (i == 999)
        {
        	//HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
        }

    }
    return w;
}

float tLockhartWavefolder_tick(tLockhartWavefolder* const wf, float samp)
{
    _tLockhartWavefolder* w = *wf;
    
    float out = 0.0f;
    
    // Compute Antiderivative
    int l = (samp > 0.0f) - (samp < 0.0f);
    double u = w->d*exp(l*w->b*samp);
    double Ln = tLockhartWavefolderLambert(u,w->Ln1);
    double Fn = w->longthing*(Ln*(Ln + 2.0)) - w->half_a*samp*samp;
    
    // Check for ill-conditioning
    if (fabs(samp-w->xn1)<w->thresh) {
        
        // Compute Averaged Wavefolder Output
        float xn = 0.5f*(samp+w->xn1);
        u = w->d*exp(l*w->b*xn);
        Ln = tLockhartWavefolderLambert(u,w->Ln1);
        out = (float) (l*w->VT*Ln - w->a*xn);

    }
    else {
        
        // Apply AA Form
        out = (float) ((Fn-w->Fn1)/(samp-w->xn1));
    }
    
    // Update States
    w->Ln1 = Ln;
    w->Fn1 = Fn;
    w->xn1 = samp;
    
    return out;
}

//============================================================================================================
// CRUSHER
//============================================================================================================
#define SCALAR 5000.f

void    tCrusher_init    (tCrusher* const cr)
{
    _tCrusher* c = *cr = (_tCrusher*) leaf_alloc(sizeof(_tCrusher));
    
    c->op = 4;
    c->div = SCALAR;
    c->rnd = 0.25f;
    c->srr = 0.25f;
    tSampleReducer_init(&c->sReducer);
    c->gain = (c->div / SCALAR) * 0.7f + 0.3f;
}

void    tCrusher_free    (tCrusher* const cr)
{
    _tCrusher* c = *cr;
    tSampleReducer_free(&c->sReducer);
    leaf_free(c);
}

void    tCrusher_initToPool   (tCrusher* const cr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tCrusher* c = *cr = (_tCrusher*) mpool_alloc(sizeof(_tCrusher), &m->pool);
    
    c->op = 4;
    c->div = SCALAR;
    c->rnd = 0.25f;
    c->srr = 0.25f;
    tSampleReducer_initToPool(&c->sReducer, mp);
    c->gain = (c->div / SCALAR) * 0.7f + 0.3f;
}

void    tCrusher_freeFromPool (tCrusher* const cr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tCrusher* c = *cr;
    tSampleReducer_freeFromPool(&c->sReducer, mp);
    mpool_free(c, &m->pool);
}

float   tCrusher_tick    (tCrusher* const cr, float input)
{
    _tCrusher* c = *cr;
    
    float sample = input;
    
    sample *= SCALAR; // SCALAR is 5000 by default
    
    sample = (int32_t) sample;
    
    sample /= c->div;
    
    sample = LEAF_bitwise_xor(sample, c->op << 23);
    
    sample = LEAF_clip(-1.f, sample, 1.f);
    
    sample = LEAF_round(sample, c->rnd);
    
    sample = tSampleReducer_tick(&c->sReducer, sample);
    
    return sample * c->gain;
    
}

void    tCrusher_setOperation (tCrusher* const cr, float op)
{
    _tCrusher* c = *cr;
    c->op = (uint32_t) (op * 8.0f);
}

// 0.0 - 1.0
void    tCrusher_setQuality (tCrusher* const cr, float val)
{
    _tCrusher* c = *cr;
    
    val = LEAF_clip(0.0f, val, 1.0f);
    
    c->div = 0.01f + val * SCALAR;
    
    c->gain = (c->div / SCALAR) * 0.7f + 0.3f;
}

// what decimal to round to
void    tCrusher_setRound (tCrusher* const cr, float rnd)
{
    _tCrusher* c = *cr;
    c->rnd = fabsf(rnd);
}

void    tCrusher_setSamplingRatio (tCrusher* const cr, float ratio)
{
    _tCrusher* c = *cr;
    c->srr = ratio;
    tSampleReducer_setRatio(&c->sReducer, ratio);

}
