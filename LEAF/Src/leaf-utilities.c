/*
  ==============================================================================

    LEAFUtilities.c
    Created: 20 Jan 2017 12:02:17pm
    Author:  Michael R Mulshine

  ==============================================================================
*/


#if _WIN32 || _WIN64

#include "..\Inc\leaf-utilities.h"
#include "..\Inc\leaf-tables.h"
#include "..\leaf.h"
#include "..\Inc\d_fft_mayer.h"

#else

#include "../Inc/leaf-utilities.h"
#include "../Inc/leaf-tables.h"
#include "../leaf.h"
#include "../Externals/d_fft_mayer.h"

#endif

#define LOGTEN 2.302585092994

float mtof(float f)
{
    if (f <= -1500.0f) return(0);
    else if (f > 1499.0f) return(mtof(1499.0f));
    else return (8.17579891564f * expf(0.0577622650f * f));
}

float ftom(float f)
{
    return (f > 0 ? 17.3123405046f * logf(.12231220585f * f) : -1500.0f);
}

float powtodb(float f)
{
    if (f <= 0) return (0);
    else
    {
        float val = 100 + 10.f/LOGTEN * logf(f);
        return (val < 0 ? 0 : val);
    }
}

float rmstodb(float f)
{
    if (f <= 0) return (0);
    else
    {
        float val = 100 + 20.f/LOGTEN * log(f);
        return (val < 0 ? 0 : val);
    }
}

float dbtopow(float f)
{
    if (f <= 0)
        return(0);
    else
    {
        if (f > 870.0f)
            f = 870.0f;
        return (expf((LOGTEN * 0.1f) * (f-100.0f)));
    }
}

float dbtorms(float f)
{
    if (f <= 0)
        return(0);
    else
    {
        if (f > 485.0f)
            f = 485.0f;
    }
    return (expf((LOGTEN * 0.05f) * (f-100.0f)));
}

/* ---------------- env~ - simple envelope follower. ----------------- */
void tEnv_init(tEnv* const x, int ws, int hs, int bs)
{
    int period = hs, npoints = ws;
    
    int i;
    
    if (npoints < 1) npoints = 1024;
    if (period < 1) period = npoints/2;
    if (period < npoints / MAXOVERLAP + 1)
        period = npoints / MAXOVERLAP + 1;

    x->x_npoints = npoints;
    x->x_phase = 0;
    x->x_period = period;
    
    x->windowSize = npoints;
    x->hopSize = period;
    x->blockSize = bs;
    
    for (i = 0; i < MAXOVERLAP; i++) x->x_sumbuf[i] = 0;
    for (i = 0; i < npoints; i++)
        x->buf[i] = (1.0f - cosf((2 * PI * i) / npoints))/npoints;
    for (; i < npoints+INITVSTAKEN; i++) x->buf[i] = 0;
    
    x->x_f = 0;
    
    x->x_allocforvs = INITVSTAKEN;
    
    // ~ ~ ~ dsp ~ ~ ~
    if (x->x_period % x->blockSize)
    {
        x->x_realperiod = x->x_period + x->blockSize - (x->x_period % x->blockSize);
    }
    else
    {
        x->x_realperiod = x->x_period;
    }
    // ~ ~ ~ ~ ~ ~ ~ ~
}

void tEnv_free (tEnv* const x)
{
    leaf_free(x);
}

float tEnv_tick (tEnv* const x)
{
    return powtodb(x->x_result);
}

void tEnv_processBlock(tEnv* const x, float* in)
{
    int n = x->blockSize;
    
    int count;
    t_sample *sump;
    in += n;
    for (count = x->x_phase, sump = x->x_sumbuf;
         count < x->x_npoints; count += x->x_realperiod, sump++)
    {
        t_sample *hp = x->buf + count;
        t_sample *fp = in;
        t_sample sum = *sump;
        int i;
        
        for (i = 0; i < n; i++)
        {
            fp--;
            sum += *hp++ * (*fp * *fp);
        }
        *sump = sum;
    }
    sump[0] = 0;
    x->x_phase -= n;
    if (x->x_phase < 0)
    {
        x->x_result = x->x_sumbuf[0];
        for (count = x->x_realperiod, sump = x->x_sumbuf;
             count < x->x_npoints; count += x->x_realperiod, sump++)
            sump[0] = sump[1];
        sump[0] = 0;
        x->x_phase = x->x_realperiod - n;
    }
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Compressor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

/*
tCompressor*    tCompressorInit(int tauAttack, int tauRelease)
{
    tCompressor* c = &leaf.tCompressorRegistry[leaf.registryIndex[T_COMPRESSOR]++];
    
    c->tauAttack = tauAttack;
    c->tauRelease = tauRelease;
    
    c->x_G[0] = 0.0f, c->x_G[1] = 0.0f,
    c->y_G[0] = 0.0f, c->y_G[1] = 0.0f,
    c->x_T[0] = 0.0f, c->x_T[1] = 0.0f,
    c->y_T[0] = 0.0f, c->y_T[1] = 0.0f;
    
    c->T = 0.0f; // Threshold
    c->R = 1.0f; // compression Ratio
    c->M = 0.0f; // decibel Make-up gain
    c->W = 0.0f; // decibel Width of knee transition
    
    return c;
}
*/
void    tCompressor_init(tCompressor* const c)
{
    c->tauAttack = 100;
    c->tauRelease = 100;
	
    c->isActive = OFALSE;
    
    c->T = 0.0f; // Threshold
    c->R = 0.5f; // compression Ratio
    c->M = 3.0f; // decibel Width of knee transition
    c->W = 1.0f; // decibel Make-up gain
}

int ccount = 0;
float tCompressor_tick(tCompressor* const c, float in)
{
    float slope, overshoot;
    float alphaAtt, alphaRel;
    
    float in_db = 20.0f * log10f( fmaxf( fabsf( in), 0.000001f)), out_db = 0.0f;
    
    c->y_T[1] = c->y_T[0];
    
    slope = c->R - 1.0f; // feed-forward topology; was 1/C->R - 1 
    
    overshoot = in_db - c->T;

    
    if (overshoot <= -(c->W * 0.5f))
		{
        out_db = in_db;
			  c->isActive = OFALSE;
		}
    else if ((overshoot > -(c->W * 0.5f)) && (overshoot < (c->W * 0.5f)))
		{
        out_db = in_db + slope * (powf((overshoot + c->W*0.5f),2) / (2.0f * c->W)); // .^ 2 ???
			  c->isActive = OTRUE;
		}
    else if (overshoot >= (c->W * 0.5f))
		{
        out_db = in_db + slope * overshoot;
			  c->isActive = OTRUE;
		}
    
    
    
    c->x_T[0] = out_db - in_db;
    
    alphaAtt = expf(-1.0f/(0.001f * c->tauAttack * leaf.sampleRate));
    alphaRel = expf(-1.0f/(0.001f * c->tauRelease * leaf.sampleRate));
    
    if (c->x_T[0] > c->y_T[1])
        c->y_T[0] = alphaAtt * c->y_T[1] + (1-alphaAtt) * c->x_T[0];
    else
        c->y_T[0] = alphaRel * c->y_T[1] + (1-alphaRel) * c->x_T[0];
    
    float attenuation = powf(10.0f, ((c->M - c->y_T[0])/20.0f));
    
    /*
    if (++ccount > 5000)
    {
        
        ccount = 0;
        DBG(".5width: " + String(c->W * 0.5f));
        DBG("slope: " + String(slope) + " overshoot: " + String(overshoot));
        DBG("attenuation: " + String(attenuation));
    }
    */
    return attenuation * in;
    

}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Envelope ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tEnvelope_init(tEnvelope* const env, float attack, float decay, oBool loop)
{
    env->exp_buff = exp_decay;
    env->inc_buff = attack_decay_inc;
    env->buff_size = sizeof(exp_decay);
    
    env->loop = loop;
    
    if (attack > 8192.0f)
        attack = 8192.0f;
    if (attack < 0.0f)
        attack = 0.0f;
    
    if (decay > 8192.0f)
        decay = 8192.0f;
    if (decay < 0.0f)
        decay = 0.0f;
    
    int16_t attackIndex = ((int16_t)(attack * 8.0f))-1;
    int16_t decayIndex = ((int16_t)(decay * 8.0f))-1;
    int16_t rampIndex = ((int16_t)(2.0f * 8.0f))-1;
    
    if (attackIndex < 0)
        attackIndex = 0;
    if (decayIndex < 0)
        decayIndex = 0;
    if (rampIndex < 0)
        rampIndex = 0;
    
    env->inRamp = OFALSE;
    env->inAttack = OFALSE;
    env->inDecay = OFALSE;
    
    env->attackInc = env->inc_buff[attackIndex];
    env->decayInc = env->inc_buff[decayIndex];
    env->rampInc = env->inc_buff[rampIndex];

}

void tEnvelope_free(tEnvelope* const env)
{
    leaf_free(env);
}

int     tEnvelope_setAttack(tEnvelope* const env, float attack)
{
    int32_t attackIndex;
    
    if (attack < 0.0f) {
        attackIndex = 0.0f;
    } else if (attack < 8192.0f) {
        attackIndex = ((int32_t)(attack * 8.0f))-1;
    } else {
        attackIndex = ((int32_t)(8192.0f * 8.0f))-1;
    }
    
    env->attackInc = env->inc_buff[attackIndex];
    
    return 0;
}

int     tEnvelope_setDecay(tEnvelope* const env, float decay)
{
    int32_t decayIndex;
    
    if (decay < 0.0f) {
        decayIndex = 0.0f;
    } else if (decay < 8192.0f) {
        decayIndex = ((int32_t)(decay * 8.0f)) - 1;
    } else {
        decayIndex = ((int32_t)(8192.0f * 8.0f)) - 1; 
    }
    
    env->decayInc = env->inc_buff[decayIndex]; 
    
    return 0;
}

int     tEnvelope_loop(tEnvelope* const env, oBool loop)
{
    env->loop = loop;
    
    return 0;
}


int     tEnvelope_on(tEnvelope* const env, float velocity)
{
    if (env->inAttack || env->inDecay) // In case envelope retriggered while it is still happening.
    {
        env->rampPhase = 0;
        env->inRamp = OTRUE;
        env->rampPeak = env->next;
    }
    else // Normal start.
    {
        env->inAttack = OTRUE;
    }
    
    
    env->attackPhase = 0;
    env->decayPhase = 0;
    env->inDecay = OFALSE;
    env->gain = velocity;
    
    return 0;
}

float   tEnvelope_tick(tEnvelope* const env)
{
    if (env->inRamp)
    {
        if (env->rampPhase > UINT16_MAX)
        {
            env->inRamp = OFALSE;
            env->inAttack = OTRUE;
            env->next = 0.0f;
        }
        else
        {
            env->next = env->rampPeak * env->exp_buff[(uint32_t)env->rampPhase];
        }
        
        env->rampPhase += env->rampInc;
    }
    
    if (env->inAttack)
    {
        
        // If attack done, time to turn around.
        if (env->attackPhase > UINT16_MAX)
        {
            env->inDecay = OTRUE;
            env->inAttack = OFALSE;
            env->next = env->gain * 1.0f;
        }
        else
        {
            // do interpolation !
            env->next = env->gain * env->exp_buff[UINT16_MAX - (uint32_t)env->attackPhase]; // inverted and backwards to get proper rising exponential shape/perception
        }
        
        // Increment envelope attack.
        env->attackPhase += env->attackInc;
        
    }
    
    if (env->inDecay)
    {
        
        // If decay done, finish.
        if (env->decayPhase >= UINT16_MAX)
        {
            env->inDecay = OFALSE;
            
            if (env->loop)
            {
                env->attackPhase = 0;
                env->decayPhase = 0;
                env->inAttack = OTRUE;
            }
            else
            {
                env->next = 0.0f;
            }
            
        } else {
            
            env->next = env->gain * (env->exp_buff[(uint32_t)env->decayPhase]); // do interpolation !
        }
        
        // Increment envelope decay;
        env->decayPhase += env->decayInc;
    }
    
    return env->next;
}

/* ADSR */
void    tADSR_init(tADSR* const adsr, float attack, float decay, float sustain, float release)
{
    adsr->exp_buff = exp_decay;
    adsr->inc_buff = attack_decay_inc;
    adsr->buff_size = sizeof(exp_decay);
    
    if (attack > 8192.0f)
        attack = 8192.0f;
    if (attack < 0.0f)
        attack = 0.0f;
    
    if (decay > 8192.0f)
        decay = 8192.0f;
    if (decay < 0.0f)
        decay = 0.0f;
    
    if (sustain > 1.0f)
        sustain = 1.0f;
    if (sustain < 0.0f)
        sustain = 0.0f;
    
    if (release > 8192.0f)
        release = 8192.0f;
    if (release < 0.0f)
        release = 0.0f;
    
    int16_t attackIndex = ((int16_t)(attack * 8.0f))-1;
    int16_t decayIndex = ((int16_t)(decay * 8.0f))-1;
    int16_t releaseIndex = ((int16_t)(release * 8.0f))-1;
    int16_t rampIndex = ((int16_t)(2.0f * 8.0f))-1;
    
    if (attackIndex < 0)
        attackIndex = 0;
    if (decayIndex < 0)
        decayIndex = 0;
    if (releaseIndex < 0)
        releaseIndex = 0;
    if (rampIndex < 0)
        rampIndex = 0;
    
    adsr->inRamp = OFALSE;
    adsr->inAttack = OFALSE;
    adsr->inDecay = OFALSE;
    adsr->inSustain = OFALSE;
    adsr->inRelease = OFALSE;
    
    adsr->sustain = sustain;
    
    adsr->attackInc = adsr->inc_buff[attackIndex];
    adsr->decayInc = adsr->inc_buff[decayIndex];
    adsr->releaseInc = adsr->inc_buff[releaseIndex];
    adsr->rampInc = adsr->inc_buff[rampIndex];
    
}

int     tADSR_setAttack(tADSR* const adsr, float attack)
{
    int32_t attackIndex;
    
    if (attack < 0.0f) {
        attackIndex = 0.0f;
    } else if (attack < 8192.0f) {
        attackIndex = ((int32_t)(attack * 8.0f))-1;
    } else {
        attackIndex = ((int32_t)(8192.0f * 8.0f))-1;
    }
    
    adsr->attackInc = adsr->inc_buff[attackIndex];
    
    return 0;
}

int     tADSR_detDecay(tADSR* const adsr, float decay)
{
    int32_t decayIndex;
    
    if (decay < 0.0f) {
        decayIndex = 0.0f;
    } else if (decay < 8192.0f) {
        decayIndex = ((int32_t)(decay * 8.0f)) - 1;
    } else {
        decayIndex = ((int32_t)(8192.0f * 8.0f)) - 1;
    }
    
    adsr->decayInc = adsr->inc_buff[decayIndex];
    
    return 0;
}

int     tADSR_setSustain(tADSR *const adsr, float sustain)
{
    if (sustain > 1.0f)      adsr->sustain = 1.0f;
    else if (sustain < 0.0f) adsr->sustain = 0.0f;
    else                     adsr->sustain = sustain;
    
    return 0;
}

int     tADSR_setRelease(tADSR* const adsr, float release)
{
    int32_t releaseIndex;
    
    if (release < 0.0f) {
        releaseIndex = 0.0f;
    } else if (release < 8192.0f) {
        releaseIndex = ((int32_t)(release * 8.0f)) - 1;
    } else {
        releaseIndex = ((int32_t)(8192.0f * 8.0f)) - 1;
    }
    
    adsr->releaseInc = adsr->inc_buff[releaseIndex];
    
    return 0;
}

int tADSR_on(tADSR* const adsr, float velocity)
{
    if ((adsr->inAttack || adsr->inDecay) || (adsr->inSustain || adsr->inRelease)) // In case ADSR retriggered while it is still happening.
    {
        adsr->rampPhase = 0;
        adsr->inRamp = OTRUE;
        adsr->rampPeak = adsr->next;
    }
    else // Normal start.
    {
        adsr->inAttack = OTRUE;
    }
    
    adsr->attackPhase = 0;
    adsr->decayPhase = 0;
    adsr->releasePhase = 0;
    adsr->inDecay = OFALSE;
    adsr->inSustain = OFALSE;
    adsr->inRelease = OFALSE;
    adsr->gain = velocity;
    
    return 0;
}

int tADSR_off(tADSR* const adsr)
{
    if (adsr->inRelease) return 0;
    
    adsr->inAttack = OFALSE;
    adsr->inDecay = OFALSE;
    adsr->inSustain = OFALSE;
    adsr->inRelease = OTRUE;
    
    adsr->releasePeak = adsr->next;
    
    return 0;
}

float   tADSR_tick(tADSR* const adsr)
{
    if (adsr->inRamp)
    {
        if (adsr->rampPhase > UINT16_MAX)
        {
            adsr->inRamp = OFALSE;
            adsr->inAttack = OTRUE;
            adsr->next = 0.0f;
        }
        else
        {
            adsr->next = adsr->rampPeak * adsr->exp_buff[(uint32_t)adsr->rampPhase];
        }
        
        adsr->rampPhase += adsr->rampInc;
    }
    
    if (adsr->inAttack)
    {
        
        // If attack done, time to turn around.
        if (adsr->attackPhase > UINT16_MAX)
        {
            adsr->inDecay = OTRUE;
            adsr->inAttack = OFALSE;
            adsr->next = adsr->gain * 1.0f;
        }
        else
        {
            // do interpolation !
            adsr->next = adsr->gain * adsr->exp_buff[UINT16_MAX - (uint32_t)adsr->attackPhase]; // inverted and backwards to get proper rising exponential shape/perception
        }
        
        // Increment ADSR attack.
        adsr->attackPhase += adsr->attackInc;
        
    }
    
    if (adsr->inDecay)
    {
        
        // If decay done, sustain.
        if (adsr->decayPhase >= UINT16_MAX)
        {
            adsr->inDecay = OFALSE;
            adsr->inSustain = OTRUE;
            adsr->next = adsr->gain * adsr->sustain;
        }
        
        else
        {
            adsr->next = adsr->gain * (adsr->sustain + ((adsr->exp_buff[(uint32_t)adsr->decayPhase]) * (1 - adsr->sustain))); // do interpolation !
        }
        
        // Increment ADSR decay.
        adsr->decayPhase += adsr->decayInc;
    }

    if (adsr->inRelease)
    {
        // If release done, finish.
        if (adsr->releasePhase >= UINT16_MAX)
        {
            adsr->inRelease = OFALSE;
            adsr->next = 0.0f;
        }
        else {
            
            adsr->next = adsr->releasePeak * (adsr->exp_buff[(uint32_t)adsr->releasePhase]); // do interpolation !
        }
        
        // Increment envelope release;
        adsr->releasePhase += adsr->releaseInc;
    }

    return adsr->next;
}

/* Envelope Follower */
void    tEnvelopeFollower_init(tEnvelopeFollower* const e, float attackThreshold, float decayCoeff)
{
    e->y = 0.0f;
    e->a_thresh = attackThreshold;
    e->d_coeff = decayCoeff;
}

float   tEnvelopeFollower_tick(tEnvelopeFollower* const ef, float x)
{
    if (x < 0.0f ) x = -x;  /* Absolute value. */
    
    if ((x >= ef->y) && (x > ef->a_thresh)) ef->y = x;                      /* If we hit a peak, ride the peak to the top. */
    else                                    ef->y = ef->y * ef->d_coeff;    /* Else, exponential decay of output. */
    
    //ef->y = envelope_pow[(uint16_t)(ef->y * (float)UINT16_MAX)] * ef->d_coeff; //not quite the right behavior - too much loss of precision?
    //ef->y = powf(ef->y, 1.000009f) * ef->d_coeff;  // too expensive
    
    if( ef->y < VSF)   ef->y = 0.0f;
    
    return ef->y;
}

int     tEnvelopeFollower_decayCoeff(tEnvelopeFollower* const ef, float decayCoeff)
{
    return ef->d_coeff = decayCoeff;
}

int     tEnvelopeFollower_attackThresh(tEnvelopeFollower* const ef, float attackThresh)
{
    return ef->a_thresh = attackThresh;
}

/* Ramp */
void    tRamp_init(tRamp* const ramp, float time, int samples_per_tick)
{
    ramp->inv_sr_ms = 1.0f/(leaf.sampleRate*0.001f);
	ramp->minimum_time = ramp->inv_sr_ms * samples_per_tick;
    ramp->curr = 0.0f;
    ramp->dest = 0.0f;
    
    if (time < ramp->minimum_time)
    {
        ramp->time = ramp->minimum_time;
    }
    else
    {
        ramp->time = time;
    }
    
    ramp->samples_per_tick = samples_per_tick;
    ramp->inc = ((ramp->dest - ramp->curr) / ramp->time * ramp->inv_sr_ms) * (float)ramp->samples_per_tick;
}

int     tRamp_setTime(tRamp* const r, float time)
{
	if (time < r->minimum_time)
	{
		r->time = r->minimum_time;
	}
	else
	{
		r->time = time;
	}
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms) * ((float)r->samples_per_tick);
    return 0;
}

int     tRamp_setDest(tRamp* const r, float dest)
{
    r->dest = dest;
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms) * ((float)r->samples_per_tick);
    return 0;
}

int     tRamp_setVal(tRamp* const r, float val)
{
    r->curr = val;
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms) * ((float)r->samples_per_tick);
    return 0;
}

float   tRamp_tick(tRamp* const r) {
    
    r->curr += r->inc;
    
    if (((r->curr >= r->dest) && (r->inc > 0.0f)) || ((r->curr <= r->dest) && (r->inc < 0.0f))) r->inc = 0.0f;
    // Palle: There is a slight risk that you overshoot here and stay on dest+inc, which with a large inc value could be a real problem
    // I suggest you add: r->curr=r->dest in the true if case
    
    return r->curr;
}

float   tRamp_sample(tRamp* const r) {
  
    return r->curr;
}

void    tRampSampleRateChanged(tRamp* const r)
{
    r->inv_sr_ms = 1.0f / (leaf.sampleRate * 0.001f);
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms)*((float)r->samples_per_tick);
}


/* Exponential Smoother */
void    tExpSmooth_init(tExpSmooth* const smooth, float val, float factor)
{	// factor is usually a value between 0 and 0.1. Lower value is slower. 0.01 for example gives you a smoothing time of about 10ms
	smooth->curr=val;
	smooth->dest=val;
	if (factor<0) factor=0;
	if (factor>1) factor=1;
	smooth->factor=factor;
	smooth->oneminusfactor=1.0f-factor;
}

int     tExpSmooth_setFactor(tExpSmooth* const smooth, float factor)
{	// factor is usually a value between 0 and 0.1. Lower value is slower. 0.01 for example gives you a smoothing time of about 10ms
	if (factor<0)
		factor=0;
	else
		if (factor>1) factor=1;
	smooth->factor=factor;
	smooth->oneminusfactor=1.0f-factor;
    return 0;
}

int     tExpSmooth_setDest(tExpSmooth* const smooth, float dest)
{
	smooth->dest=dest;
    return 0;
}

int     tExpSmooth_setVal(tExpSmooth* const smooth, float val)
{
	smooth->curr=val;
    return 0;
}

float   tExpSmooth_tick(tExpSmooth* const smooth)
{
    smooth->curr = smooth->factor*smooth->dest+smooth->oneminusfactor*smooth->curr;
    return smooth->curr;
}

float   tExpSmooth_sample(tExpSmooth* const smooth)
{
    return smooth->curr;
}


/* Power Follower */

void    tPwrFollow_init(tPwrFollow* const p, float factor)
{
	p->curr=0.0f;
	p->factor=factor;
	p->oneminusfactor=1.0f-factor;
}

int     tPwrFollow_setFactor(tPwrFollow* const p, float factor)
{
	if (factor<0) factor=0;
	if (factor>1) factor=1;
	p->factor=factor;
	p->oneminusfactor=1.0f-factor;
    return 0;
}

float   tPwrFollow_tick(tPwrFollow* const p, float input)
{
    p->curr = p->factor*input*input+p->oneminusfactor*p->curr;
    return p->curr;
}

float   tPwrFollow_sample(tPwrFollow* const p)
{
    return p->curr;
}

/* Feedback Leveler */

void    tFBleveller_init(tFBleveller* const p, float targetLevel, float factor, float strength, int mode)
{
	p->curr=0.0f;
	p->targetLevel=targetLevel;
	tPwrFollow_init(&p->pwrFlw,factor);
	p->mode=mode;
	p->strength=strength;
}

int     tFBleveller_setStrength(tFBleveller* const p, float strength)
{	// strength is how strongly level diff is affecting the amp ratio
	// try 0.125 for a start
	p->strength=strength;
    return 0;
}

int     tFBleveller_setFactor(tFBleveller* const p, float factor)
{
	tPwrFollow_setFactor(&p->pwrFlw,factor);
    return 0;
}

int     tFBleveller_setMode(tFBleveller* const p, int mode)
{	// 0 for decaying with upwards lev limiting, 1 for constrained absolute level (also downwards limiting)
	p->mode=mode;
    return 0;
}

float   tFBleveller_tick(tFBleveller* const p, float input)
{
	float levdiff=(tPwrFollow_tick(&p->pwrFlw, input)-p->targetLevel);
	if (p->mode==0 && levdiff<0) levdiff=0;
	p->curr=input*(1-p->strength*levdiff);
	return p->curr;
}

float   tFBleveller_sample(tFBleveller* const p)
{
    return p->curr;
}


int     tFBleveller_setTargetLevel   (tFBleveller* const p, float TargetLevel)
{
	p->targetLevel=TargetLevel;
}

///Reed Table model
//default values from STK are 0.6 offset and -0.8 slope

void    tReedTable_init      (tReedTable* const p, float offset, float slope)
{
	p->offset = offset;
	p->slope = slope;
}

void    tReedTable_free      (tReedTable* const p)
{
	;
}

float   tReedTable_tick      (tReedTable* const p, float input)
{
	// The input is differential pressure across the reed.
	float output = p->offset + (p->slope * input);

	// If output is > 1, the reed has slammed shut and the
	// reflection function value saturates at 1.0.
	if ( output > 1.0f) output = 1.0f;

	// This is nearly impossible in a physical system, but
	// a reflection function value of -1.0 corresponds to
	// an open end (and no discontinuity in bore profile).
	if ( output < -1.0f) output = -1.0f;

	return output;
}

float   tReedTable_tanh_tick     (tReedTable* const p, float input)
{
	// The input is differential pressure across the reed.
	float output = p->offset + (p->slope * input);

	// If output is > 1, the reed has slammed shut and the
	// reflection function value saturates at 1.0.
	return tanhf(output);
}

void     tReedTable_setOffset   (tReedTable* const p, float offset)
{
	p->offset = offset;
}

void     tReedTable_setSlope   (tReedTable* const p, float slope)
{
	p->slope = slope;
}



/* Simple Living String*/

void    tSimpleLivingString_init(tSimpleLivingString* const p, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode)
{
	p->curr=0.0f;
	tExpSmooth_init(&p->wlSmooth, leaf.sampleRate/freq, 0.01); // smoother for string wavelength (not freq, to avoid expensive divisions)
	tSimpleLivingString_setFreq(p, freq);
	tDelayL_init(&p->delayLine,p->waveLengthInSamples, 2400);
	tOnePole_init(&p->bridgeFilter, dampFreq);
	tHighpass_init(&p->DCblocker,13);
	p->decay=decay;
	tFBleveller_init(&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode);
	p->levMode=levMode;
}

int     tSimpleLivingString_setFreq(tSimpleLivingString* const p, float freq)
{
	if (freq<20) freq=20;
	else if (freq>10000) freq=10000;
	p->waveLengthInSamples = leaf.sampleRate/freq;
	tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    return 0;
}

int     tSimpleLivingString_setWaveLength(tSimpleLivingString* const p, float waveLength)
{
	if (waveLength<4.8) waveLength=4.8;
	else if (waveLength>2400) waveLength=2400;
	p->waveLengthInSamples = waveLength;
	tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    return 0;
}

int     tSimpleLivingString_setDampFreq(tSimpleLivingString* const p, float dampFreq)
{
	tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    return 0;
}

int     tSimpleLivingString_setDecay(tSimpleLivingString* const p, float decay)
{
	p->decay=decay;
    return 0;
}

int     tSimpleLivingString_setTargetLev(tSimpleLivingString* const p, float targetLev)
{
	tFBleveller_setTargetLevel(&p->fbLev, targetLev);
    return 0;
}

int     tSimpleLivingString_setLevSmoothFactor(tSimpleLivingString* const p, float levSmoothFactor)
{
	tFBleveller_setFactor(&p->fbLev, levSmoothFactor);
    return 0;
}

int     tSimpleLivingString_setLevStrength(tSimpleLivingString* const p, float levStrength)
{
	tFBleveller_setStrength(&p->fbLev, levStrength);
    return 0;
}

int     tSimpleLivingString_setLevMode(tSimpleLivingString* const p, int levMode)
{
	tFBleveller_setMode(&p->fbLev, levMode);
	p->levMode=levMode;
    return 0;
}

float   tSimpleLivingString_tick(tSimpleLivingString* const p, float input)
{
	float stringOut=tOnePole_tick(&p->bridgeFilter,tDelayL_tickOut(&p->delayLine));
	float stringInput=tHighpass_tick(&p->DCblocker, tFBleveller_tick(&p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
	tDelayL_tickIn(&p->delayLine, stringInput);
	tDelayL_setDelay(&p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    p->curr = stringOut;
    return p->curr;
}

float   tSimpleLivingString_sample(tSimpleLivingString* const p)
{
    return p->curr;
}

/* Living String*/

void    tLivingString_init(tLivingString* const p, float freq, float pickPos, float prepIndex, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode)
{
	p->curr=0.0f;
	tExpSmooth_init(&p->wlSmooth, leaf.sampleRate/freq, 0.01); // smoother for string wavelength (not freq, to avoid expensive divisions)
	tLivingString_setFreq(p, freq);
	tExpSmooth_init(&p->ppSmooth, pickPos, 0.01); // smoother for pick position
	tLivingString_setPickPos(p, pickPos);
	p->prepIndex=prepIndex;
	tDelayL_init(&p->delLF,p->waveLengthInSamples, 2400);
	tDelayL_init(&p->delUF,p->waveLengthInSamples, 2400);
	tDelayL_init(&p->delUB,p->waveLengthInSamples, 2400);
	tDelayL_init(&p->delLB,p->waveLengthInSamples, 2400);
	tOnePole_init(&p->bridgeFilter, dampFreq);
	tOnePole_init(&p->nutFilter, dampFreq);
	tOnePole_init(&p->prepFilterU, dampFreq);
	tOnePole_init(&p->prepFilterL, dampFreq);
	tHighpass_init(&p->DCblockerU,13);
	tHighpass_init(&p->DCblockerL,13);
	p->decay=decay;
	tFBleveller_init(&p->fbLevU, targetLev, levSmoothFactor, levStrength, levMode);
	tFBleveller_init(&p->fbLevL, targetLev, levSmoothFactor, levStrength, levMode);
	p->levMode=levMode;
}

int     tLivingString_setFreq(tLivingString* const p, float freq)
{	// NOTE: It is faster to set wavelength in samples directly
	if (freq<20) freq=20;
	else if (freq>10000) freq=10000;
	p->waveLengthInSamples = leaf.sampleRate/freq;
	tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    return 0;
}

int     tLivingString_setWaveLength(tLivingString* const p, float waveLength)
{
	if (waveLength<4.8) waveLength=4.8;
	else if (waveLength>2400) waveLength=2400;
	p->waveLengthInSamples = waveLength;
	tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    return 0;
}

int     tLivingString_setPickPos(tLivingString* const p, float pickPos)
{	// between 0 and 1
	if (pickPos<0.f) pickPos=0.f;
	else if (pickPos>1.f) pickPos=1.f;
	p->pickPos = pickPos;
	tExpSmooth_setDest(&p->ppSmooth, p->pickPos);
    return 0;
}

int     tLivingString_setPrepIndex(tLivingString* const p, float prepIndex)
{	// between 0 and 1
	if (prepIndex<0.f) prepIndex=0.f;
	else if (prepIndex>1.f) prepIndex=1.f;
	p->prepIndex = prepIndex;
    return 0;
}

int     tLivingString_setDampFreq(tLivingString* const p, float dampFreq)
{
	tOnePole_setFreq(&p->bridgeFilter, dampFreq);
	tOnePole_setFreq(&p->nutFilter, dampFreq);
	tOnePole_setFreq(&p->prepFilterU, dampFreq);
	tOnePole_setFreq(&p->prepFilterL, dampFreq);
    return 0;
}

int     tLivingString_setDecay(tLivingString* const p, float decay)
{
	p->decay=decay;
    return 0;
}

int     tLivingString_setTargetLev(tLivingString* const p, float targetLev)
{
	tFBleveller_setTargetLevel(&p->fbLevU, targetLev);
	tFBleveller_setTargetLevel(&p->fbLevL, targetLev);
    return 0;
}

int     tLivingString_setLevSmoothFactor(tLivingString* const p, float levSmoothFactor)
{
	tFBleveller_setFactor(&p->fbLevU, levSmoothFactor);
	tFBleveller_setFactor(&p->fbLevL, levSmoothFactor);
    return 0;
}

int     tLivingString_setLevStrength(tLivingString* const p, float levStrength)
{
	tFBleveller_setStrength(&p->fbLevU, levStrength);
	tFBleveller_setStrength(&p->fbLevL, levStrength);
    return 0;
}

int     tLivingString_setLevMode(tLivingString* const p, int levMode)
{
	tFBleveller_setMode(&p->fbLevU, levMode);
	tFBleveller_setMode(&p->fbLevL, levMode);
	p->levMode=levMode;
    return 0;
}

float   tLivingString_tick(tLivingString* const p, float input)
{
	// from pickPos upwards=forwards
	float fromLF=tDelayL_tickOut(&p->delLF);
	float fromUF=tDelayL_tickOut(&p->delUF);
	float fromUB=tDelayL_tickOut(&p->delUB);
	float fromLB=tDelayL_tickOut(&p->delLB);
	// into upper half of string, from nut, going backwards
	float fromNut=-tFBleveller_tick(&p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(&p->DCblockerU, tOnePole_tick(&p->nutFilter, fromUF)));
	tDelayL_tickIn(&p->delUB, fromNut);
	// into lower half of string, from pickpoint, going backwards
	float fromLowerPrep=-tOnePole_tick(&p->prepFilterL, fromLF);
	float intoLower=p->prepIndex*fromLowerPrep+(1-p->prepIndex)*fromUB+input;
	tDelayL_tickIn(&p->delLB, intoLower);
	// into lower half of string, from bridge
	float fromBridge=-tFBleveller_tick(&p->fbLevL, (p->levMode==0?p->decay:1)*tHighpass_tick(&p->DCblockerL, tOnePole_tick(&p->bridgeFilter, fromLB)));
	tDelayL_tickIn(&p->delLF, fromBridge);
	// into upper half of string, from pickpoint, going forwards/upwards
	float fromUpperPrep=-tOnePole_tick(&p->prepFilterU, fromUB);
	float intoUpper=p->prepIndex*fromUpperPrep+(1-p->prepIndex)*fromLF+input;
	tDelayL_tickIn(&p->delUF, intoUpper);
	// update all delay lengths
	float pickP=tExpSmooth_tick(&p->ppSmooth);
	float wLen=tExpSmooth_tick(&p->wlSmooth);
	float lowLen=pickP*wLen;
	float upLen=(1-pickP)*wLen;
	tDelayL_setDelay(&p->delLF, lowLen);
	tDelayL_setDelay(&p->delLB, lowLen);
	tDelayL_setDelay(&p->delUF, upLen);
	tDelayL_setDelay(&p->delUB, upLen);
    p->curr = fromBridge;
    return p->curr;
}

float   tLivingString_sample(tLivingString* const p)
{
    return p->curr;
}


/* Stack */

// If stack contains note, returns index. Else returns -1;
int tStack_contains(tStack* const ns, uint16_t noteVal)
{
    for (int i = 0; i < ns->size; i++)
    {
        if (ns->data[i] == noteVal)    return i;
    }
    return -1;
}

void tStack_add(tStack* const ns, uint16_t noteVal)
{
    uint8_t j;
    
    int whereToInsert = 0;
    if (ns->ordered)
    {
        for (j = 0; j < ns->size; j++)
        {
            if (noteVal > ns->data[j])
            {
                if ((noteVal < ns->data[j+1]) || (ns->data[j+1] == -1))
                {
                    whereToInsert = j+1;
                    break;
                }
            }
        }
    }
    
    //first move notes that are already in the stack one position to the right
    for (j = ns->size; j > whereToInsert; j--)
    {
        ns->data[j] = ns->data[(j - 1)];
    }
    
    //then, insert the new note into the front of the stack
    ns->data[whereToInsert] = noteVal;
    
    ns->size++;
}

int tStack_addIfNotAlreadyThere(tStack* const ns, uint16_t noteVal)
{
    uint8_t j;

    int added = 0;
    
    if (tStack_contains(ns, noteVal) == -1)
    {
        int whereToInsert = 0;
        if (ns->ordered)
        {
            for (j = 0; j < ns->size; j++)
            {
                if (noteVal > ns->data[j])
                {
                    if ((noteVal < ns->data[j+1]) || (ns->data[j+1] == -1))
                    {
                        whereToInsert = j+1;
                        break;
                    }
                }
            }
        }
        
        //first move notes that are already in the stack one position to the right
        for (j = ns->size; j > whereToInsert; j--)
        {
            ns->data[j] = ns->data[(j - 1)];
        }
        
        //then, insert the new note into the front of the stack
        ns->data[whereToInsert] = noteVal;
        
        ns->size++;
        
        added = 1;
    }
    
    return added;
}


// Remove noteVal. return 1 if removed, 0 if not
int tStack_remove(tStack* const ns, uint16_t noteVal)
{
    uint8_t k;
    int foundIndex = tStack_contains(ns, noteVal);
    int removed = 0;
    
    if (foundIndex >= 0)
    {
        for (k = 0; k < (ns->size - foundIndex); k++)
        {
            if ((k+foundIndex) >= (ns->capacity - 1))
            {
                ns->data[k + foundIndex] = -1;
            }
            else
            {
                ns->data[k + foundIndex] = ns->data[k + foundIndex + 1];
                if ((k + foundIndex) == (ns->size - 1))
                {
                    ns->data[k + foundIndex + 1] = -1;
                }
            }
            
        }
        // in case it got put on the stack multiple times
        foundIndex--;
        ns->size--;
        removed = 1;
    }
    
    
    
    return removed;
}

// Doesn't change size of data types
void tStack_setCapacity(tStack* const ns, uint16_t cap)
{
    if (cap <= 0)
        ns->capacity = 1;
    else if (cap <= STACK_SIZE)
        ns->capacity = cap;
    else
        ns->capacity = STACK_SIZE;
    
    for (int i = cap; i < STACK_SIZE; i++)
    {
        if ((int)ns->data[i] != -1)
        {
            ns->data[i] = -1;
            ns->size -= 1;
        }
    }
    
    if (ns->pos >= cap)
    {
        ns->pos = 0;
    }
}

int tStack_getSize(tStack* const ns)
{
    return ns->size;
}

void tStack_clear(tStack* const ns)
{
    for (int i = 0; i < STACK_SIZE; i++)
    {
        ns->data[i] = -1;
    }
    ns->pos = 0;
    ns->size = 0;
}

// Next item in order of addition to stack. Return 0-31 if there is a next item to move to. Returns -1 otherwise.
int tStack_next(tStack* const ns)
{
    int step = 0;
    if (ns->size != 0) // if there is at least one note in the stack
    {
        if (ns->pos > 0) // if you're not at the most recent note (first one), then go backward in the array (moving from earliest to latest)
        {
            ns->pos--;
        }
        else
        {
            ns->pos = (ns->size - 1); // if you are the most recent note, go back to the earliest note in the array
        }
        
        step = ns->data[ns->pos];
        return step;
    }
    else
    {
        return -1;
    }
}

int tStack_get(tStack* const ns, int which)
{
    return ns->data[which];
}

int tStack_first(tStack* const ns)
{
    return ns->data[0];
}

void tStack_init(tStack* const ns)
{
    ns->ordered = OFALSE;
    ns->size = 0;
    ns->pos = 0;
    ns->capacity = STACK_SIZE;
    
    for (int i = 0; i < STACK_SIZE; i++) ns->data[i] = -1;
}

/******************************************************************************/
/***************** static function declarations *******************************/
/******************************************************************************/

static void solad_init(tSOLAD *w);
static inline float read_sample(tSOLAD *w, float floatindex);
static void pitchdown(tSOLAD *w, float *out);
static void pitchup(tSOLAD *w, float *out);

/******************************************************************************/
/***************** public access functions ************************************/
/******************************************************************************/

// init
void     tSOLAD_init(tSOLAD* const w)
{
    w->pitchfactor = 1.;
    
    solad_init(w);
}

void tSOLAD_free(tSOLAD* const w)
{
    leaf_free(w);
}

// send one block of input samples, receive one block of output samples
void tSOLAD_ioSamples(tSOLAD* const w, float* in, float* out, int blocksize)
{
    int i = w->timeindex;
    int n = w->blocksize = blocksize;
    
    if(!i) w->delaybuf[LOOPSIZE] = in[0];   // copy one sample for interpolation
    while(n--) w->delaybuf[i++] = *in++;    // copy one input block to delay buffer
    
    if(w->pitchfactor > 1) pitchup(w, out);
    else pitchdown(w, out);
    
    w->timeindex += blocksize;
    w->timeindex &= LOOPMASK;
}

// set periodicity analysis data
void tSOLAD_setPeriod(tSOLAD* const w, float period)
{
    if(period > MAXPERIOD) period = MAXPERIOD;
    if(period > MINPERIOD) w->period = period;  // ignore period when too small
}

// set pitch factor between 0.25 and 4
void tSOLAD_setPitchFactor(tSOLAD* const w, float pitchfactor)
{
    if(pitchfactor < 0.25) pitchfactor = 0.25;
    else if(pitchfactor > 4.) pitchfactor = 4.;
    w->pitchfactor = pitchfactor;
}

// force readpointer lag
void tSOLAD_setReadLag(tSOLAD* const w, float readlag)
{
    if(readlag < 0) readlag = 0;
    if(readlag < w->readlag)               // do not jump backward, only forward
    {
        w->jump = w->readlag - readlag;
        w->readlag = readlag;
        w->xfadelength = readlag;
        w->xfadevalue = 1;
    }
}

// reset state variables
void tSOLAD_resetState(tSOLAD* const w)
{
    int n = LOOPSIZE + 1;
    float *buf = w->delaybuf;
    
    while(n--) *buf++ = 0;
    solad_init(w);
}

/******************************************************************************/
/******************** private procedures **************************************/
/******************************************************************************/

/*
 Function pitchdown() is called to read samples from the delay buffer when pitch
 factor is between 0.25 and 1. The read pointer lags behind because of the slowed
 down speed, and it must jump forward towards the write pointer soon as there is
 sufficient space to jump. That is, if there is at least one period of the input
 signal between read pointer and write pointer.  When short periods follow up on
 long periods, the read pointer may have space to jump over more than one period
 lenghts. Jump length must be [periodlength ^ 2] in any case.
 
 A linear crossfade function joins the jump-from point with the jump-to point.
 The crossfade must be completed before another read pointer jump is allowed.
 Length of the crossfade function is stored as a number of samples in terms of
 the input sample rate. This length is dynamically translated
 to a crossfade length expressed in output reading rate, according to pitch
 factor which can change before the crossfade is completed. Crossfade length does
 not cover an invariable length in periods for all pitch transposition factors.
 For pitch factors from 0.5 till 1, crossfade length is stretched in the
 output just as much as the signal itself, as crossfade speed is set to equal
 pitch factor. For pitch factors below 0.5, the read pointer wants to jump
 forward before one period is read, therefore the crossfade length as expressed
 in output periods must be shorter. Crossfade speed is set to [1 - pitchfactor]
 for those cases. Pitch factor 0.5 is the natural switch point between crossfade
 speeds [pitchfactor] and [1 - pitchfactor] because 0.5 == 1 - 0.5. The crossfade
 speed modification for pitch factors below 0.5 also means that much of the
 original signal content will be skipped.
 */


static void pitchdown(tSOLAD* const w, float *out)
{
    int n = w->blocksize;
    float refindex = (float)(w->timeindex + LOOPSIZE); // no negative values!
    float pitchfactor = w->pitchfactor;
    float period = w->period;
    float readlag = w->readlag;
    float readlagstep = 1 - pitchfactor;
    float jump = w->jump;
    float xfadevalue = w->xfadevalue;
    float xfadelength = w->xfadelength;
    float xfadespeed, xfadestep, readindex, outputsample;
    
    if(pitchfactor > 0.5) xfadespeed = pitchfactor;
    else xfadespeed = 1 - pitchfactor;
    xfadestep = xfadespeed / xfadelength;
    
    while(n--)
    {
        if(readlag > period)        // check if read pointer may jump forward...
        {
            if(xfadevalue <= 0)      // ...but do not interrupt crossfade
            {
                jump = period;                           // jump forward
                while((jump * 2) < readlag) jump *= 2;   // use available space
                readlag -= jump;                         // reduce read pointer lag
                xfadevalue = 1;                          // start crossfade
                xfadelength = period - 1;
                xfadestep = xfadespeed / xfadelength;
            }
        }
        
        readindex = refindex - readlag;
        outputsample = read_sample(w, readindex);
        
        if(xfadevalue > 0)
        {
            outputsample *= (1 - xfadevalue);                               // fadein
            outputsample += read_sample(w, readindex - jump) * xfadevalue;  // fadeout
            xfadevalue -= xfadestep;
        }
        
        *out++ = outputsample;
        refindex += 1;
        readlag += readlagstep;
    }
    
    w->jump = jump;                 // state variables
    w->readlag = readlag;
    w->xfadevalue = xfadevalue;
    w->xfadelength = xfadelength;
}


/*
 Function pitchup() for pitch factors above 1 is more complicated than
 pitchdown(). The read pointer increments faster than the write pointer and a
 backward jump must happen in time, reckoning with the crossfade region. The read
 pointer backward jump length is always one period. In order to minimize the area
 of signal duplicates, crossfade length is aimed at [period / pitchfactor].
 This leads to a crossfade speed of [pitchfactor * pitchfactor].
 
 Some samples for the fade out (but not all of them) must already be in the
 buffer, otherwise we will run out of input samples before the crossfade is
 completed. The ratio of past samples and future samples for a crossfade of any
 length is as follows:
 
 past samples: xfadelength * (1 - 1 / pitchfactor)
 future samples: xfadelength * (1 / pitchfactor)
 
 For example in the case of pitch factor 1.5 this would be:
 
 past samples: xfadelength * (1 - 1 / 1.5) = xfadelength * 1 / 3
 future samples: xfadelength * (1 / 1.5) = xfadelength * 2 / 3
 
 In the case of pitch factor 4 this would be:
 
 past samples: xfadelength * (1 - 1 / 4) = xfadelength * 3 / 4
 future samples: xfadelength * (1 / 4) = xfadelength * 1 / 4
 
 The read pointer lag must therefore preserve a minimum dependent on pitch
 factor. The minimum is called 'limit' here:
 
 limit = period * (pitchfactor - 1) / pitchfactor * pitchfactor
 
 Components of this expression are combined to reuse them in operations, while
 (pitchfactor - 1) is changed to (pitchfactor - 0.99) to avoid numerical
 resolution issues for pitch factors slightly above 1:
 
 xfadespeed = pitchfactor * pitchfactor
 limitfactor = (pitchfactor - 0.99) / xfadespeed
 limit = period * limitfactor
 
 When read lag is smaller than this limit, the read pointer must preferably
 jump backward, unless a previous crossfade is not yet completed. Crossfades must
 preferably be completed, unless the read pointer lag becomes smaller than zero.
 With fluctuating period lengths and pitch factors, the readpointer lag limit may
 change from one input block to the next in such a way that the actual lag is
 suddenly much smaller than the limit, and the intended crossfade length can not
 be applied. Therefore the crossfade length is simply calculated from the
 available amount of samples for all cases, like so:
 
 xfadelength = readlag / limitfactor
 
 For most occurrences, this will amount to a crossfade length reduced to
 [period / pitchfactor] in the output for pitch factors above 1, while in some
 cases it will be considerably shorter. Fortunately, an incidental aberration of
 the intended crossfade length hardly ever creates an audible artifact. The
 reason to specify preferred crossfade length according to pitch factor is to
 minimize the impression of echoes without sacrificing too much of the signal
 content. The readpointer jump length remains one period in any case.
 
 Sometimes, the input signal periodicity may decrease substantially between one
 signal block and the next. In such cases it may be possible for the read pointer
 to jump forward and reduce latency. For every signal block, a check on this
 possibility is done. A previous crossfade must be completed before a forward
 jump is allowed.
 */
static void pitchup(tSOLAD* const w, float *out)
{
    int n = w->blocksize;
    float refindex = (float)(w->timeindex + LOOPSIZE); // no negative values
    float pitchfactor = w->pitchfactor;
    float period = w->period;
    float readlag = w->readlag;
    float jump = w->jump;
    float xfadevalue = w->xfadevalue;
    float xfadelength = w->xfadelength;
    
    float readlagstep = pitchfactor - 1;
    float xfadespeed = pitchfactor * pitchfactor;
    float xfadestep = xfadespeed / xfadelength;
    float limitfactor = (pitchfactor - (float)0.99) / xfadespeed;
    float limit = period * limitfactor;
    float readindex, outputsample;
    
    if((readlag > (period + 2 * limit)) & (xfadevalue < 0))
    {
        jump = period;                                        // jump forward
        while((jump * 2) < (readlag - 2 * limit)) jump *= 2;  // use available space
        readlag -= jump;                                      // reduce read pointer lag
        xfadevalue = 1;                                       // start crossfade
        xfadelength = period - 1;
        xfadestep = xfadespeed / xfadelength;
    }
    
    while(n--)
    {
        if(readlag < limit)  // check if read pointer should jump backward...
        {
            if((xfadevalue < 0) | (readlag < 0)) // ...but try not to interrupt crossfade
            {
                xfadelength = readlag / limitfactor;
                if(xfadelength < 1) xfadelength = 1;
                xfadestep = xfadespeed / xfadelength;
                
                jump = -period;         // jump backward
                readlag += period;      // increase read pointer lag
                xfadevalue = 1;         // start crossfade
            }
        }
        
        readindex = refindex - readlag;
        outputsample = read_sample(w, readindex);
        
        if(xfadevalue > 0)
        {
            outputsample *= (1 - xfadevalue);
            outputsample += read_sample(w, readindex - jump) * xfadevalue;
            xfadevalue -= xfadestep;
        }
        
        *out++ = outputsample;
        refindex += 1;
        readlag -= readlagstep;
    }
    
    w->readlag = readlag;               // state variables
    w->jump = jump;
    w->xfadelength = xfadelength;
    w->xfadevalue = xfadevalue;
}

// read one sample from delay buffer, with linear interpolation
static inline float read_sample(tSOLAD* const w, float floatindex)
{
    int index = (int)floatindex;
    float fraction = floatindex - (float)index;
    float *buf = w->delaybuf;
    index &= LOOPMASK;
    
    return (buf[index] + (fraction * (buf[index+1] - buf[index])));
}

static void solad_init(tSOLAD* const w)
{
    w->timeindex = 0;
    w->xfadevalue = -1;
    w->period = INITPERIOD;
    w->readlag = INITPERIOD;
    w->blocksize = INITPERIOD;
}

/******************************************************************************/
/***************************** private procedures *****************************/
/******************************************************************************/

#define REALFFT mayer_realfft
#define REALIFFT mayer_realifft

static void snac_analyzeframe(tSNAC* const s);
static void snac_autocorrelation(tSNAC* const s);
static void snac_normalize(tSNAC* const s);
static void snac_pickpeak(tSNAC* const s);
static void snac_periodandfidelity(tSNAC* const s);
static void snac_biasbuf(tSNAC* const s);
static float snac_spectralpeak(tSNAC* const s, float periodlength);


/******************************************************************************/
/******************************** constructor, destructor *********************/
/******************************************************************************/


void tSNAC_init(tSNAC* const s, int overlaparg)
{
    s->biasfactor = DEFBIAS;
    s->timeindex = 0;
    s->periodindex = 0;
    s->periodlength = 0.;
    s->fidelity = 0.;
    s->minrms = DEFMINRMS;
    s->framesize = SNAC_FRAME_SIZE;
    
    snac_biasbuf(s);
    tSNAC_setOverlap(s, overlaparg);
}

void tSNAC_free(tSNAC* const s)
{
    leaf_free(s);
}
/******************************************************************************/
/************************** public access functions****************************/
/******************************************************************************/


void tSNAC_ioSamples(tSNAC* const s, float *in, float *out, int size)
{
    int timeindex = s->timeindex;
    int mask = s->framesize - 1;
    int outindex = 0;
    float *inputbuf = s->inputbuf;
    float *processbuf = s->processbuf;
    
    // call analysis function when it is time
    if(!(timeindex & (s->framesize / s->overlap - 1))) snac_analyzeframe(s);
    
    while(size--)
    {
        inputbuf[timeindex] = *in++;
        out[outindex++] = processbuf[timeindex++];
        timeindex &= mask;
    }
    s->timeindex = timeindex;
}

void tSNAC_setOverlap(tSNAC* const s, int lap)
{
    if(!((lap==1)|(lap==2)|(lap==4)|(lap==8))) lap = DEFOVERLAP;
    s->overlap = lap;
}


void tSNAC_setBias(tSNAC* const s, float bias)
{
    if(bias > 1.) bias = 1.;
    if(bias < 0.) bias = 0.;
    s->biasfactor = bias;
    snac_biasbuf(s);
    return;
}


void tSNAC_setMinRMS(tSNAC* const s, float rms)
{
    if(rms > 1.) rms = 1.;
    if(rms < 0.) rms = 0.;
    s->minrms = rms;
    return;
}


float tSNAC_getPeriod(tSNAC* const s)
{
    return(s->periodlength);
}


float tSNAC_getFidelity(tSNAC* const s)
{
    return(s->fidelity);
}


/******************************************************************************/
/***************************** private procedures *****************************/
/******************************************************************************/


// main analysis function
static void snac_analyzeframe(tSNAC* const s)
{
    int n, tindex = s->timeindex;
    int framesize = s->framesize;
    int mask = framesize - 1;
    float norm = 1. / sqrt((float)(framesize * 2));
    
    float *inputbuf = s->inputbuf;
    float *processbuf = s->processbuf;
    
    // copy input to processing buffers
    for(n=0; n<framesize; n++)
    {
        processbuf[n] = inputbuf[tindex] * norm;
        tindex++;
        tindex &= mask;
    }
    
    // zeropadding
    for(n=framesize; n<(framesize<<1); n++) processbuf[n] = 0.;
    
    // call analysis procedures
    snac_autocorrelation(s);
    snac_normalize(s);
    snac_pickpeak(s);
    snac_periodandfidelity(s);
}


static void snac_autocorrelation(tSNAC* const s)
{
    int n, m;
    int framesize = s->framesize;
    int fftsize = framesize * 2;
    float *processbuf = s->processbuf;
    float *spectrumbuf = s->spectrumbuf;
    
    REALFFT(fftsize, processbuf);
    
    // compute power spectrum
    processbuf[0] *= processbuf[0];                      // DC
    processbuf[framesize] *= processbuf[framesize];      // Nyquist
    
    for(n=1; n<framesize; n++)
    {
        processbuf[n] = processbuf[n] * processbuf[n]
        + processbuf[fftsize-n] * processbuf[fftsize-n]; // imag coefficients appear reversed
        processbuf[fftsize-n] = 0.;
    }
    
    // store power spectrum up to SR/4 for possible later use
    for(m=0; m<(framesize>>1); m++)
    {
        spectrumbuf[m] = processbuf[m];
    }
    
    // transform power spectrum to autocorrelation function
    REALIFFT(fftsize, processbuf);
    return;
}


static void snac_normalize(tSNAC* const s)
{
    int framesize = s->framesize;
    int framesizeplustimeindex = s->framesize + s->timeindex;
    int timeindexminusone = s->timeindex - 1;
    int n, m;
    int mask = framesize - 1;
    int seek = framesize * SEEK;
    float *inputbuf = s->inputbuf;
    float *processbuf= s->processbuf;
    float signal1, signal2;
    
    // minimum RMS implemented as minimum autocorrelation at index 0
    // functionally equivalent to white noise floor
    float rms = s->minrms / sqrt(1.0f / (float)framesize);
    float minrzero = rms * rms;
    float rzero = processbuf[0];
    if(rzero < minrzero) rzero = minrzero;
    double normintegral = (double)rzero * 2.;
    
    // normalize biased autocorrelation function
    // inputbuf is circular buffer: timeindex may be non-zero when overlap > 1
    processbuf[0] = 1;
    for(n=1, m=s->timeindex+1; n<seek; n++, m++)
    {
        signal1 = inputbuf[(n + timeindexminusone)&mask];
        signal2 = inputbuf[(framesizeplustimeindex - n)&mask];
        normintegral -= (double)(signal1 * signal1 + signal2 * signal2);
        processbuf[n] /= (float)normintegral * 0.5f;
    }
    
    // flush instable function tail
    for(n = seek; n<framesize; n++) processbuf[n] = 0.;
    return;
}


static void snac_periodandfidelity(tSNAC* const s)
{
    float periodlength;
    
    if(s->periodindex)
    {
        periodlength = (float)s->periodindex +
        interpolate3phase(s->processbuf, s->periodindex);
        if(periodlength < 8) periodlength = snac_spectralpeak(s, periodlength);
        s->periodlength = periodlength;
        s->fidelity = interpolate3max(s->processbuf, s->periodindex);
    }
    return;
}

// select the peak which most probably represents period length
static void snac_pickpeak(tSNAC* const s)
{
    int n, peakindex=0;
    int seek = s->framesize * SEEK;
    float *processbuf= s->processbuf;
    float maxvalue = 0.;
    float biasedpeak;
    float *biasbuf = s->biasbuf;
    
    // skip main lobe
    for(n=1; n<seek; n++)
    {
        if(processbuf[n] < 0.) break;
    }
    
    // find interpolated / biased maximum in SNAC function
    // interpolation finds the 'real maximum'
    // biasing favours the first candidate
    for(; n<seek-1; n++)
    {
        if(processbuf[n] >= processbuf[n-1])
        {
            if(processbuf[n] > processbuf[n+1])     // we have a local peak
            {
                biasedpeak = interpolate3max(processbuf, n) * biasbuf[n];
                
                if(biasedpeak > maxvalue)
                {
                    maxvalue = biasedpeak;
                    peakindex = n;
                }
            }
        }
    }
    s->periodindex = peakindex;
    return;
}


// verify period length via frequency domain (up till SR/4)
// frequency domain is more precise than lag domain for period lengths < 8
// argument 'periodlength' is initial estimation from autocorrelation
static float snac_spectralpeak(tSNAC* const s, float periodlength)
{
    if(periodlength < 4.) return periodlength;
    
    float max = 0.;
    int n, startbin, stopbin, peakbin = 0;
    int spectrumsize = s->framesize>>1;
    float *spectrumbuf = s->spectrumbuf;
    float peaklocation = (float)(s->framesize * 2.) / periodlength;
    
    startbin = (int)(peaklocation * 0.8 + 0.5);
    if(startbin < 1) startbin = 1;
    stopbin = (int)(peaklocation * 1.25 + 0.5);
    if(stopbin >= spectrumsize - 1) stopbin = spectrumsize - 1;
    
    for(n=startbin; n<stopbin; n++)
    {
        if(spectrumbuf[n] >= spectrumbuf[n-1])
        {
            if(spectrumbuf[n] > spectrumbuf[n+1])
            {
                if(spectrumbuf[n] > max)
                {
                    max = spectrumbuf[n];
                    peakbin = n;
                }
            }
        }
    }
    
    // calculate amplitudes in peak region
    for(n=(peakbin-1); n<(peakbin+2); n++)
    {
        spectrumbuf[n] = sqrt(spectrumbuf[n]);
    }
    
    peaklocation = (float)peakbin + interpolate3phase(spectrumbuf, peakbin);
    periodlength = (float)(s->framesize * 2.0f) / peaklocation;
    
    return periodlength;
}


// modified logarithmic bias function
static void snac_biasbuf(tSNAC* const s)
{
    int n;
    int maxperiod = (int)(s->framesize * (float)SEEK);
    float bias = s->biasfactor / log((float)(maxperiod - 4));
    float *biasbuf = s->biasbuf;
    
    for(n=0; n<5; n++)    // periods < 5 samples can't be tracked
    {
        biasbuf[n] = 0.;
    }
    
    for(n=5; n<maxperiod; n++)
    {
        biasbuf[n] = 1.0f - (float)log(n - 4) * bias;
    }
}

/********Private function prototypes**********/
static void atkdtk_init(tAtkDtk* const a, int blocksize, int atk, int rel);
static void atkdtk_envelope(tAtkDtk* const a, float *in);

/********Constructor/Destructor***************/

void tAtkDtk_init(tAtkDtk* const a, int blocksize)
{
    atkdtk_init(a, blocksize, DEFATTACK, DEFRELEASE);
}

void tAtkDtk_init_expanded(tAtkDtk* const a, int blocksize, int atk, int rel)
{
    atkdtk_init(a, blocksize, atk, rel);
}

void tAtkDtk_free(tAtkDtk *a)
{
    leaf_free(a);
}

/*******Public Functions***********/


void tAtkDtk_setBlocksize(tAtkDtk* const a, int size)
{
    
    if(!((size==64)|(size==128)|(size==256)|(size==512)|(size==1024)|(size==2048)))
        size = DEFBLOCKSIZE;
    a->blocksize = size;
    
    return;
    
}

void tAtkDtk_setSamplerate(tAtkDtk* const a, int inRate)
{
    a->samplerate = inRate;
    
    //Reset atk and rel to recalculate coeff
    tAtkDtk_setAtk(a, a->atk);
    tAtkDtk_setRel(a, a->rel);
    
    return;
}

void tAtkDtk_setThreshold(tAtkDtk* const a, float thres)
{
    a->threshold = thres;
    return;
}

void tAtkDtk_setAtk(tAtkDtk* const a, int inAtk)
{
    a->atk = inAtk;
    a->atk_coeff = pow(0.01, 1.0/(a->atk * a->samplerate * 0.001));
    
    return;
}

void tAtkDtk_setRel(tAtkDtk* const a, int inRel)
{
    a->rel = inRel;
    a->rel_coeff = pow(0.01, 1.0/(a->rel * a->samplerate * 0.001));
    
    return;
}


int tAtkDtk_detect(tAtkDtk* const a, float *in)
{
    int result;
    
    atkdtk_envelope(a, in);
    
    if(a->env >= a->prevAmp*2) //2 times greater = 6dB increase
        result = 1;
    else
        result = 0;
    
    a->prevAmp = a->env;
    
    return result;
    
}

/*******Private Functions**********/

static void atkdtk_init(tAtkDtk* const a, int blocksize, int atk, int rel)
{
    a->env = 0;
    a->blocksize = blocksize;
    a->threshold = DEFTHRESHOLD;
    a->samplerate = leaf.sampleRate;
    a->prevAmp = 0;
    
    a->env = 0;
    
    tAtkDtk_setAtk(a, atk);
    tAtkDtk_setRel(a, rel);
}

static void atkdtk_envelope(tAtkDtk* const a, float *in)
{
    int i = 0;
    float tmp;
    for(i = 0; i < a->blocksize; ++i){
        tmp = fastabs(in[i]);
        
        if(tmp > a->env)
            a->env = a->atk_coeff * (a->env - tmp) + tmp;
        else
            a->env = a->rel_coeff * (a->env - tmp) + tmp;
    }
    
}

