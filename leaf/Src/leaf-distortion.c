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
#include "..\Inc\leaf-math.h"
#include "..\Inc\leaf-filters.h"
#else


#include "../Inc/leaf-distortion.h"
#include "../Inc/leaf-tables.h"
#include "../Inc/leaf-math.h"
#include "../Inc/leaf-filters.h"
#ifdef ARM_MATH_CM7
#include "arm_math.h"
#endif
#endif

//============================================================================================================
// Sample-Rate reducer
//============================================================================================================

void tSampleReducer_init (tSampleReducer* const sr, LEAF* const leaf)
{
    tSampleReducer_initToPool(sr, &leaf->mempool);
}

void tSampleReducer_initToPool (tSampleReducer* const sr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSampleReducer* s = *sr = (_tSampleReducer*) mpool_alloc(sizeof(_tSampleReducer), m);
    s->mempool = m;
    
    s->invRatio = 1.0f;
    s->hold = 0.0f;
    s->count = 0;
}

void tSampleReducer_free (tSampleReducer* const sr)
{
    _tSampleReducer* s = *sr;
    
    mpool_free((char*)s, s->mempool);
}

Lfloat tSampleReducer_tick(tSampleReducer const s, Lfloat input)
{
    if (s->count > s->invRatio)
    {
        s->hold = input;
        s->count = 0;
    }
    
    s->count++;
    return s->hold;
}


void tSampleReducer_setRatio(tSampleReducer const s, Lfloat ratio)
{
    if ((ratio <= 1.0f) && (ratio >= 0.0f))
        s->invRatio = 1.0f / ratio;
    
}

#if LEAF_INCLUDE_OVERSAMPLER_TABLES
//============================================================================================================
// Oversampler
//============================================================================================================
// Latency is equal to the phase length (numTaps / ratio)
void tOversampler_init (tOversampler* const osr, int ratio, int extraQuality, LEAF* const leaf)
{
    tOversampler_initToPool(osr, ratio, extraQuality, &leaf->mempool);
}

void tOversampler_initToPool (tOversampler* const osr, int maxRatio, int extraQuality, tMempool* const mp)
{
    _tMempool* m = *mp;
    int offset = 0;
    if (extraQuality) offset = 6;
    if (maxRatio == 2 || maxRatio == 4  || maxRatio == 8 ||
        maxRatio == 16 || maxRatio == 32 || maxRatio == 64)
    {
        _tOversampler* os = *osr = (_tOversampler*) mpool_alloc(sizeof(_tOversampler), m);
        os->mempool = m;
        
        os->offset = offset;
        os->maxRatio = maxRatio;
        os->allowHighQuality = extraQuality;
        os->ratio = os->maxRatio;
        uint_fast16_t idx = (int)(log2f(os->ratio))-1+os->offset;
        os->numTaps = __leaf_tablesize_firNumTaps[idx];
        os->phaseLength = os->numTaps / os->ratio;
        os->pCoeffs = (Lfloat*) __leaf_tableref_firCoeffs[idx];
        os->upState = (Lfloat*) mpool_alloc(sizeof(Lfloat) * os->numTaps * 2, m);
        os->downState = (Lfloat*) mpool_alloc(sizeof(Lfloat) * os->numTaps * 2, m);
    }
}

void tOversampler_free (tOversampler* const osr)
{
    _tOversampler* os = *osr;
    
    mpool_free((char*)os->upState, os->mempool);
    mpool_free((char*)os->downState, os->mempool);
    mpool_free((char*)os, os->mempool);
}

Lfloat tOversampler_tick(tOversampler const os, Lfloat input, Lfloat* oversample, Lfloat (*effectTick)(Lfloat))
{
    tOversampler_upsample(os, input, oversample);
    
    for (int i = 0; i < os->ratio; i++) {
        oversample[i] = effectTick(oversample[i]);
    }
    
    return tOversampler_downsample(os, oversample);
}

// From CMSIS DSP Library
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tOversampler_upsample(tOversampler* const osr, Lfloat input, Lfloat* output)
#else
void tOversampler_upsample(tOversampler const os, Lfloat input, Lfloat* output)
#endif
{
    if (os->ratio == 1)
    {
        output[0] = input;
        return;
    }
    
    Lfloat *pState = os->upState;                 /* State pointer */
    Lfloat *pCoeffs = os->pCoeffs;               /* Coefficient pointer */
    Lfloat *pStateCur;
    Lfloat *ptr1;                               /* Temporary pointer for state buffer */
    Lfloat *ptr2;                               /* Temporary pointer for coefficient buffer */
    Lfloat sum0;                                /* Accumulators */
    uint_fast16_t i, tapCnt;                    /* Loop counters */
    uint_fast16_t phaseLen = os->phaseLength;            /* Length of each polyphase filter component */
    uint_fast16_t j;
    
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
#ifdef ITCMRAM
Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) tOversampler_downsample(tOversampler *const osr, Lfloat* input)
#else
Lfloat tOversampler_downsample(tOversampler const os, Lfloat* input)
#endif
{
    if (os->ratio == 1) return input[0];
    
    Lfloat *pState = os->downState;                 /* State pointer */
    Lfloat *pCoeffs = os->pCoeffs;               /* Coefficient pointer */
    Lfloat *pStateCur;                          /* Points to the current sample of the state */
    Lfloat *px0;                                /* Temporary pointer for state buffer */
    Lfloat *pb;                                 /* Temporary pointer for coefficient buffer */
    Lfloat x0, c0;                              /* Temporary variables to hold state and coefficient values */
    Lfloat acc0;                                /* Accumulator */
    uint32_t numTaps = os->numTaps;                 /* Number of filter coefficients in the filter */
    uint32_t i, tapCnt;
    Lfloat output;
    
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

void    tOversampler_setRatio       (tOversampler const os, int ratio)
{
    if (ratio == os->ratio) return;
    if (ratio > os->maxRatio) ratio = os->maxRatio;
    if (ratio == 1) os->ratio = ratio;
    else if (ratio == 2 || ratio == 4  || ratio == 8 ||
        ratio == 16 || ratio == 32 || ratio == 64)
    {
        os->ratio = ratio;
        int idx = (int)(log2f(os->ratio))-1+os->offset;
        os->numTaps = __leaf_tablesize_firNumTaps[idx];
        os->phaseLength = os->numTaps / os->ratio;
        os->pCoeffs = (Lfloat*) __leaf_tableref_firCoeffs[idx];
    }
}

void    tOversampler_setQuality     (tOversampler const os, int quality)
{
    if (!os->allowHighQuality) return;
    int offset = 0;
    if (quality > 0) offset = 6;
    os->offset = offset;
    
    if (os->ratio == 1) return;
    
    int idx = (int)(log2f(os->ratio))-1+os->offset;
    os->numTaps = __leaf_tablesize_firNumTaps[idx];
    os->phaseLength = os->numTaps / os->ratio;
    os->pCoeffs = (Lfloat*) __leaf_tableref_firCoeffs[idx];
}

int tOversampler_getLatency(tOversampler const os)
{
    return os->phaseLength;
}
#endif // LEAF_INCLUDE_OVERSAMPLER_TABLES

//============================================================================================================
// SIMPLER WAVEFOLDER
//============================================================================================================

void tWavefolder_init (tWavefolder* const wf, Lfloat ffAmount, Lfloat fbAmount, Lfloat foldDepth, LEAF* const leaf)
{
    tWavefolder_initToPool   (wf, ffAmount, fbAmount, foldDepth, &leaf->mempool);
}

void tWavefolder_initToPool (tWavefolder* const wf, Lfloat ffAmount, Lfloat fbAmount, Lfloat foldDepth, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tWavefolder* w = *wf = (_tWavefolder*) mpool_alloc(sizeof(_tWavefolder), m);
    w->mempool = m;
    tHighpass_initToPool(&w->dcBlock, 1.0f, mp);
    w->FBsample = 0.0f;
    w->offset = 0.0f;
    w->gain = 1.0f;
    w->foldDepth = foldDepth;
    w->FBAmount = fbAmount;
    w->FFAmount = ffAmount;
    w->invFBAmount = 1.0f / (1.0f + fbAmount);
}

void tWavefolder_free (tWavefolder* const wf)
{
    _tWavefolder* w = *wf;
    
    mpool_free((char*)w, w->mempool);
}

void tWavefolder_setFFAmount(tWavefolder const w, Lfloat ffAmount)
{
    w->FFAmount = ffAmount;
}
void tWavefolder_setFBAmount(tWavefolder const w, Lfloat fbAmount)
{
    w->FBAmount = fbAmount;
    
    w->invFBAmount = 1.0f / (1.0f + fbAmount);
}

void tWavefolder_setFoldDepth(tWavefolder const w, Lfloat foldDepth)
{
    w->foldDepth = foldDepth;
}

void tWavefolder_setOffset(tWavefolder const w, Lfloat offset)
{
    w->offset = offset;
}

void tWavefolder_setGain(tWavefolder const w, Lfloat gain)
{
    w->gain = gain;
}

Lfloat tWavefolder_tick(tWavefolder const w, Lfloat in)
{
    //Lfloat sample = in * w->offset + (w->gain * w->offset);
    Lfloat sample = in;
    float curFB = w->FBAmount;
    float curFF = w->FFAmount;

    //softclip approx for tanh saturation in original code
    float ffSample = sample;
    if (ffSample <= -1.0f)
    {
        ffSample = -1.0f;
    } else if (ffSample >= 1.0f)
    {
        ffSample = 1.0f;
    }
    ffSample = ffSample - ((ffSample * ffSample * ffSample)* 0.3333333f);
    ffSample *= 1.499999f;
    float ff = (curFF * ffSample) + ((1.0f - curFF) * sample);

    //softclip approx for tanh saturation in original code
    float fbSample = w->FBsample;
    if (fbSample <= -1.0f)
    {
        fbSample = -1.0f;
    } else if (fbSample >= 1.0f)
    {
        fbSample = 1.0f;
    }
    fbSample = fbSample - ((fbSample * fbSample * fbSample)* 0.3333333f);
    fbSample *= 1.499999f;
    float fb = curFB * fbSample;

    Lfloat tempVal = 0.0f;
#ifdef ARM_MATH_CM7
    tempVal =arm_sin_f32(TWO_PI * sample);
#else
    tempVal =sinf(TWO_PI * sample);
#endif
    w->FBsample = (ff + fb) - w->foldDepth * tempVal;
    sample = w->FBsample * w->invFBAmount;
    sample = tHighpass_tick(w->dcBlock, sample);
    return sample;
}

//============================================================================================================
// WAVEFOLDER
//============================================================================================================


//from the paper: Virtual Analog Model of the Lockhart Wavefolder
//by Fabián Esqueda, Henri Pöntynen, Julian D. Parker and Stefan Bilbao

void tLockhartWavefolder_init (tLockhartWavefolder* const wf, LEAF* const leaf)
{
	tLockhartWavefolder_initToPool   (wf, &leaf->mempool);
}

void tLockhartWavefolder_initToPool (tLockhartWavefolder* const wf, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tLockhartWavefolder* w = *wf = (_tLockhartWavefolder*) mpool_alloc(sizeof(_tLockhartWavefolder), m);
    w->mempool = m;
    
    w->Ln1 = 0.0;
    w->Fn1 = 0.0;
    w->xn1 = 0.0;
    
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
    w->AAthresh = 10e-10; //10

	w->LambertThresh = 10e-12; //12  //was 8


    w->w = 0.0f;
    w->expw = 0.0f;
    w->p = 0.0f;
    w->r = 0.0f;
    w->s= 0.0f;
    w->myerr = 0.0f;
    w->l = 0.0f;
    w->u = 0.0f;
    w->Ln = 0.0f;
	w->Fn = 0.0f;
    w->tempsDenom = 0.0f;
    w->tempErrDenom = 0.0f;
    w->tempOutDenom = 0.0f;


}

void tLockhartWavefolder_free (tLockhartWavefolder* const wf)
{
    _tLockhartWavefolder* w = *wf;
    
    mpool_free((char*)w, w->mempool);
}



double tLockhartWavefolderLambert(tLockhartWavefolder const mwf, double x, double ln)
{
    // Initial guess (use previous value)
	mwf->w = ln;
    
    // Haley's method (Sec. 4.2 of the paper)
    for(int i=0; i<6000; i+=1) { //1000
        
    	mwf->expw = exp(mwf->w);
    	
        if (isinf(mwf->expw) || isnan(mwf->expw))
        {
        	mwf->expw = 10e-5;

        }
        
    	mwf->p = mwf->w*mwf->expw - x;
    	
        if (isinf(mwf->p) || isnan(mwf->p))
        {
        	mwf->p = 10e-5;

        }
        
    	mwf->r = (mwf->w+1.0)*mwf->expw;
    	
        if (isinf(mwf->r) || isnan(mwf->r))
        {
        	mwf->r = 10e-5;

        }
        
    	mwf->tempsDenom = (2.0*(mwf->w+1.0));
    	
        if ((mwf->tempsDenom == 0.0) || isinf(mwf->tempsDenom) || isnan(mwf->tempsDenom))
        {
        	mwf->tempsDenom = 10e-5;
        }
        
        mwf->s = (mwf->w+2.0)/mwf->tempsDenom;
        
        if (isnan(mwf->s) || isinf(mwf->s))
        {
        	mwf->s = 10e-5;
        }
        
        mwf->tempErrDenom = (mwf->r-(mwf->p*mwf->s));
         
        if ((mwf->tempErrDenom == 0.0) || isinf(mwf->tempErrDenom) || isnan(mwf->tempErrDenom))
        {
        	mwf->tempErrDenom = 10e-5;
        }
        
        mwf->myerr = (mwf->p/mwf->tempErrDenom);
         
        if (isnan(mwf->myerr) || isinf(mwf->myerr))
        {
        	mwf->myerr = 10e-5;
        }
        

        if ((fabs(mwf->myerr))<mwf->LambertThresh) {

        	break;
        }

        mwf->w = mwf->w - mwf->myerr;

         /*
        if (isinf(mwf->w) || isnan(mwf->w))
        {
        	mwf->w = 10e-5;
        	LEAF_error();
        }
*/

    }
    return mwf->w;
}

Lfloat tLockhartWavefolder_tick(tLockhartWavefolder const w, Lfloat in)
{
    Lfloat out = 0.0f;
    
    // Compute Antiderivative
    w->l = (in > 0.0) - (in < 0.0);
    double tempbin = w->b*in;
    if (tempbin > 600.0)
    {
        tempbin = 600.0;
    }
    else if (tempbin < -600.0)
    {
        tempbin = -600.0;
    }
    w->u = w->d*exp(w->l*tempbin);
    
    if ((w->u == 0.0) || isinf(w->u) || isnan(w->u))
    {
    	w->u = 10e-5;

    }
    

    w->Ln = tLockhartWavefolderLambert(w,w->u,w->Ln1);
    /*
    if ((w->Ln == 0.0) || isinf(w->Ln) || isnan(w->Ln))
	{
		w->Ln = 10e-5;
		LEAF_error();
	}
*/
    w->Fn = (w->longthing*(w->Ln*(w->Ln + 2.0))) - (w->half_a*in*in);
    /*
    if ((w->Fn == 0.0) || isinf(w->Fn) || isnan(w->Fn))
	{
		w->Fn = 10e-5;
		LEAF_error();
	}
	*/
    // Check for ill-conditioning

    if (fabs(in-w->xn1)<w->AAthresh)
    {
        
        // Compute Averaged Wavefolder Output
    	double xn = 0.5*(in+w->xn1);
        double tempbxn = xn * w->b;
        if (tempbxn > 600.0)
        {
            tempbxn = 600.0;
        }
        else if (tempbxn < -600.0)
        {
            tempbxn = -600.0;
        }
    	w->u = w->d*exp(w->l*tempbxn);
        
    	if ((w->u == 0.0) || isinf(w->u) || isnan(w->u))
        {
        	w->u = 10e-5;


        }
        
    	w->Ln = tLockhartWavefolderLambert(w,w->u,w->Ln1);
    	/*
        if ((w->Ln == 0.0) || isinf(w->Ln) || isnan(w->Ln))
    	{
    		w->Ln = 10e-5;
    		LEAF_error();
    	}
    	*/
        out = (Lfloat)((w->l*w->VT*w->Ln) - (w->a*xn));

    }
    else
    {

        // Apply AA Form
    	w->tempOutDenom = (in-w->xn1);
    	
    	if ((w->tempOutDenom == 0.0) || isinf(w->tempOutDenom))
    	{
    		w->tempOutDenom = 10e-5;
    	}
    	
        out = (Lfloat)((w->Fn-w->Fn1)/w->tempOutDenom);
        /*
        if (isinf(out) || isnan(out))
		{
        	out = 10e-5;
        	LEAF_error();
		}
		*/

    }

    // Update States
    w->Ln1 = w->Ln;
    w->Fn1 = w->Fn;
    w->xn1 = (double)in;
    
    return out;
}

//============================================================================================================
// CRUSHER
//============================================================================================================
#define SCALAR 5000.f
#define INV_SCALAR 0.0002f

void tCrusher_init (tCrusher* const cr, LEAF* const leaf)
{
    tCrusher_initToPool(cr, &leaf->mempool);
}

void tCrusher_initToPool (tCrusher* const cr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tCrusher* c = *cr = (_tCrusher*) mpool_alloc(sizeof(_tCrusher), m);
    c->mempool = m;
    
    c->op = 4;
    c->div = 1.0f / SCALAR;
    c->rnd = 0.25f;
    c->srr = 0.25f;
    tSampleReducer_initToPool(&c->sReducer, mp);
    c->gain = (c->div / SCALAR) * 0.7f + 0.3f;
}

void tCrusher_free (tCrusher* const cr)
{
    _tCrusher* c = *cr;
    
    tSampleReducer_free(&c->sReducer);
    mpool_free((char*)c, c->mempool);
}

Lfloat tCrusher_tick (tCrusher const c, Lfloat input)
{
    Lfloat sample = input;
    
    sample *= SCALAR; // SCALAR is 5000 by default
    
    sample = (int32_t) sample;
    
    sample *= c->div;
    
    sample = LEAF_bitwise_xor(sample, c->op << 23);
    
    sample = LEAF_clip(-1.f, sample, 1.f);
    
    sample = LEAF_round(sample, c->rnd);
    
    sample = tSampleReducer_tick(&c->sReducer, sample);
    
    return sample * c->gain;
    
}

void    tCrusher_setOperation (tCrusher const c, Lfloat op)
{
    c->op = (uint32_t) (op * 8.0f);
}

// 0.0 - 1.0
void    tCrusher_setQuality (tCrusher const c, Lfloat val)
{
    val = LEAF_clip(0.0f, val, 1.0f);
    
    c->div = 0.01f + val * SCALAR;
    
    c->gain = (c->div * INV_SCALAR) * 0.7f + 0.3f;
}

// what decimal to round to
void    tCrusher_setRound (tCrusher const c, Lfloat rnd)
{
    c->rnd = rnd;
}

void    tCrusher_setSamplingRatio (tCrusher const c, Lfloat ratio)
{
    c->srr = ratio;
    tSampleReducer_setRatio(c->sReducer, ratio);
}
