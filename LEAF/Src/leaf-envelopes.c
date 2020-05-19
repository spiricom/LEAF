/*
  ==============================================================================

    leaf-envelopes.c
    Created: 20 Jan 2017 12:02:17pm
    Author:  Michael R Mulshine

  ==============================================================================
*/


#if _WIN32 || _WIN64

#include "..\Inc\leaf-envelopes.h"
#include "..\Inc\leaf-tables.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-envelopes.h"
#include "../Inc/leaf-tables.h"
#include "../leaf.h"

#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Envelope ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tEnvelope_init(tEnvelope* const envlp, float attack, float decay, oBool loop)
{
    _tEnvelope* env = *envlp = (_tEnvelope*) leaf_alloc(sizeof(_tEnvelope));
    
    env->exp_buff = __leaf_table_exp_decay;
    env->inc_buff = __leaf_table_attack_decay_inc;
    env->buff_size = sizeof(__leaf_table_exp_decay);
    
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

void tEnvelope_free(tEnvelope* const envlp)
{
    _tEnvelope* env = *envlp;
    leaf_free(env);
}

void    tEnvelope_initToPool    (tEnvelope* const envlp, float attack, float decay, oBool loop, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tEnvelope* env = *envlp = (_tEnvelope*) mpool_alloc(sizeof(_tEnvelope), m);
    
    env->exp_buff = __leaf_table_exp_decay;
    env->inc_buff = __leaf_table_attack_decay_inc;
    env->buff_size = sizeof(__leaf_table_exp_decay);
    
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

void    tEnvelope_freeFromPool  (tEnvelope* const envlp, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tEnvelope* env = *envlp;
    mpool_free(env, m);
}

void     tEnvelope_setAttack(tEnvelope* const envlp, float attack)
{
    _tEnvelope* env = *envlp;
    
    int32_t attackIndex;
    
    if (attack < 0.0f) {
        attackIndex = 0.0f;
    } else if (attack < 8192.0f) {
        attackIndex = ((int32_t)(attack * 8.0f))-1;
    } else {
        attackIndex = ((int32_t)(8192.0f * 8.0f))-1;
    }
    
    env->attackInc = env->inc_buff[attackIndex];
}

void     tEnvelope_setDecay(tEnvelope* const envlp, float decay)
{
    _tEnvelope* env = *envlp;
    
    int32_t decayIndex;
    
    if (decay < 0.0f) {
        decayIndex = 0;
    } else if (decay < 8192.0f) {
        decayIndex = ((int32_t)(decay * 8.0f)) - 1;
    } else {
        decayIndex = ((int32_t)(8192.0f * 8.0f)) - 1; 
    }
    
    env->decayInc = env->inc_buff[decayIndex];
}

void     tEnvelope_loop(tEnvelope* const envlp, oBool loop)
{
    _tEnvelope* env = *envlp;
    env->loop = loop;
}


void     tEnvelope_on(tEnvelope* const envlp, float velocity)
{
    _tEnvelope* env = *envlp;
    
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
}

float   tEnvelope_tick(tEnvelope* const envlp)
{
    _tEnvelope* env = *envlp;
    
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
//replaced our older ADSR that relied on a large lookup table with this one by Nigel Redmon from his blog. Thanks, Nigel!
//-JS



float calcADSRCoef(double rate, double targetRatio)
{
    return (rate <= 0.0f) ? 0.0f : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}


void    tADSR_init(tADSR* const adsrenv, float attack, float decay, float sustain, float release)
{
    tADSR_initToPool(adsrenv, attack, decay, sustain, release, &leaf.mempool);
}

void tADSR_free(tADSR* const adsrenv)
{
    tADSR_freeFromPool(adsrenv, &leaf.mempool);
}

void    tADSR_initToPool    (tADSR* const adsrenv, float attack, float decay, float sustain, float release, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tADSR* adsr = *adsrenv = (_tADSR*) mpool_alloc(sizeof(_tADSR), m);
    adsr->sampleRateInMs =  leaf.sampleRate * 0.001f;
    adsr->targetRatioA = 0.3f;
    adsr->targetRatioDR = 0.0001f;
    adsr->attackRate = attack * adsr->sampleRateInMs;
    adsr->attackCoef = calcADSRCoef(attack * adsr->sampleRateInMs, adsr->targetRatioA);
    adsr->attackBase = (1.0f + adsr->targetRatioA) * (1.0f - adsr->attackCoef);
    
    adsr->decayRate = decay * adsr->sampleRateInMs;
    adsr->decayCoef = calcADSRCoef(decay * adsr->sampleRateInMs,adsr-> targetRatioDR);
    adsr->decayBase = (adsr->sustainLevel - adsr->targetRatioDR) * (1.0f - adsr->decayCoef);
    
    adsr->sustainLevel = sustain;
    adsr->decayBase = (adsr->sustainLevel - adsr->targetRatioDR) * (1.0f - adsr->decayCoef);
    
    adsr->releaseRate = release * adsr->sampleRateInMs;
    adsr->releaseCoef = calcADSRCoef(release * adsr->sampleRateInMs, adsr->targetRatioDR);
    adsr->releaseBase = -adsr->targetRatioDR * (1.0f - adsr->releaseCoef);

    adsr->state = env_idle;
    adsr->gain = 1.0f;
    adsr->output = 0.0f;
    adsr->leakFactor = 1.0f;
}

void    tADSR_freeFromPool  (tADSR* const adsrenv, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tADSR* adsr = *adsrenv;
    mpool_free(adsr, m);
}

void     tADSR_setAttack(tADSR* const adsrenv, float attack)
{
    _tADSR* adsr = *adsrenv;

    adsr->attackRate = attack * adsr->sampleRateInMs;
    adsr->attackCoef = calcADSRCoef(adsr->attackRate, adsr->targetRatioA);
    adsr->attackBase = (1.0f + adsr->targetRatioA) * (1.0f - adsr->attackCoef);
}

void     tADSR_setDecay(tADSR* const adsrenv, float decay)
{
    _tADSR* adsr = *adsrenv;
    
    adsr->decayRate = decay * adsr->sampleRateInMs;
    adsr->decayCoef = calcADSRCoef(adsr->decayRate,adsr-> targetRatioDR);
    adsr->decayBase = (adsr->sustainLevel - adsr->targetRatioDR) * (1.0f - adsr->decayCoef);
}

void     tADSR_setSustain(tADSR* const adsrenv, float sustain)
{
    _tADSR* adsr = *adsrenv;
    
    adsr->sustainLevel = sustain;
    adsr->decayBase = (adsr->sustainLevel - adsr->targetRatioDR) * (1.0f - adsr->decayCoef);
}

void     tADSR_setRelease(tADSR* const adsrenv, float release)
{
    _tADSR* adsr = *adsrenv;
    
    adsr->releaseRate = release * adsr->sampleRateInMs;
    adsr->releaseCoef = calcADSRCoef(adsr->releaseRate, adsr->targetRatioDR);
    adsr->releaseBase = -adsr->targetRatioDR * (1.0f - adsr->releaseCoef);
}

// 0.999999 is slow leak, 0.9 is fast leak
void     tADSR_setLeakFactor(tADSR* const adsrenv, float leakFactor)
{
    _tADSR* adsr = *adsrenv;

    adsr->leakFactor = leakFactor;
}

void tADSR_on(tADSR* const adsrenv, float velocity)
{
    _tADSR* adsr = *adsrenv;
    adsr->state = env_attack;
    adsr->gain = velocity;
}

void tADSR_off(tADSR* const adsrenv)
{
    _tADSR* adsr = *adsrenv;

    if (adsr->state != env_idle)
    {
        adsr->state = env_release;
    }
}

float   tADSR_tick(tADSR* const adsrenv)
{
    _tADSR* adsr = *adsrenv;
    

    switch (adsr->state) {
        case env_idle:
            break;
        case env_attack:
            adsr->output = adsr->attackBase + adsr->output * adsr->attackCoef;
            if (adsr->output >= 1.0f) {
                adsr->output = 1.0f;
                adsr->state = env_decay;
            }
            break;
        case env_decay:
            adsr->output = adsr->decayBase + adsr->output * adsr->decayCoef * adsr->leakFactor;
            if (adsr->output <= adsr->sustainLevel) {
                adsr->output = adsr->sustainLevel;
                adsr->state = env_sustain;
            }
            break;
        case env_sustain:
            adsr->output = adsr->output * adsr->leakFactor;
            break;
        case env_release:
            adsr->output = adsr->releaseBase + adsr->output * adsr->releaseCoef;
            if (adsr->output <= 0.0f) {
                adsr->output = 0.0f;
                adsr->state = env_idle;
            }
    }
    return adsr->output * adsr->gain;
}






/////-----------------
/* Ramp */
void    tRamp_init(tRamp* const r, float time, int samples_per_tick)
{
    _tRamp* ramp = *r = (_tRamp*) leaf_alloc(sizeof(_tRamp));
    
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
    ramp->factor = (1.0f / ramp->time) * ramp->inv_sr_ms;
    ramp->samples_per_tick = samples_per_tick;
    ramp->inc = ((ramp->dest - ramp->curr) / ramp->time * ramp->inv_sr_ms) * (float)ramp->samples_per_tick;
}

void tRamp_free(tRamp* const r)
{
    _tRamp* ramp = *r;
    
    leaf_free(ramp);
}

void    tRamp_initToPool    (tRamp* const r, float time, int samples_per_tick, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tRamp* ramp = *r = (_tRamp*) mpool_alloc(sizeof(_tRamp), m);
    
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
    ramp->factor = (1.0f / ramp->time) * ramp->inv_sr_ms * (float)ramp->samples_per_tick;
    ramp->inc = (ramp->dest - ramp->curr) * ramp->factor;
}

void    tRamp_freeFromPool  (tRamp* const r, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tRamp* ramp = *r;
    
    mpool_free(ramp, m);
}

void     tRamp_setTime(tRamp* const ramp, float time)
{
    _tRamp* r = *ramp;
    
	if (time < r->minimum_time)
	{
		r->time = r->minimum_time;
	}
	else
	{
		r->time = time;
	}
    r->factor = (1.0f / r->time) * r->inv_sr_ms * (float)r->samples_per_tick;
    r->inc = (r->dest - r->curr) * r->factor;

}

void     tRamp_setDest(tRamp* const ramp, float dest)
{
    _tRamp* r = *ramp;
    r->dest = dest;
    r->inc = (r->dest - r->curr) * r->factor;
}

void     tRamp_setVal(tRamp* const ramp, float val)
{
    _tRamp* r = *ramp;
    r->curr = val;
    r->inc = (r->dest - r->curr) * r->factor;
}

float   tRamp_tick(tRamp* const ramp)
{
    _tRamp* r = *ramp;
    
    r->curr += r->inc;
    
    if (((r->curr >= r->dest) && (r->inc > 0.0f)) || ((r->curr <= r->dest) && (r->inc < 0.0f)))
	{
		r->inc = 0.0f;
		r->curr=r->dest;
	}
    
    return r->curr;
}

float   tRamp_sample(tRamp* const ramp)
{
    _tRamp* r = *ramp;
    return r->curr;
}

void    tRampSampleRateChanged(tRamp* const ramp)
{
    _tRamp* r = *ramp;
    r->inv_sr_ms = 1.0f / (leaf.sampleRate * 0.001f);
    r->factor = (1.0f / r->time) * r->inv_sr_ms * (float)r->samples_per_tick;
    r->inc = (r->dest - r->curr) * r->factor;
}



/* RampUpDown */
void    tRampUpDown_init(tRampUpDown* const r, float upTime, float downTime, int samples_per_tick)
{
	tRampUpDown_initToPool(r, upTime, downTime, samples_per_tick, &leaf.mempool);
}

void tRampUpDown_free(tRampUpDown* const r)
{
	tRampUpDown_freeFromPool(r, &leaf.mempool);
}

void    tRampUpDown_initToPool(tRampUpDown* const r, float upTime, float downTime, int samples_per_tick, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tRampUpDown* ramp = *r = (_tRampUpDown*) mpool_alloc(sizeof(_tRampUpDown), m);

    ramp->inv_sr_ms = 1.0f/(leaf.sampleRate*0.001f);
    ramp->minimum_time = ramp->inv_sr_ms * samples_per_tick;
    ramp->curr = 0.0f;
    ramp->dest = 0.0f;

    if (upTime < ramp->minimum_time)
    {
        ramp->upTime = ramp->minimum_time;
    }
    else
    {
        ramp->upTime = upTime;
    }

    if (downTime < ramp->minimum_time)
    {
        ramp->downTime = ramp->minimum_time;
    }
    else
    {
        ramp->downTime = downTime;
    }

    ramp->samples_per_tick = samples_per_tick;
    ramp->upInc = ((ramp->dest - ramp->curr) / ramp->upTime * ramp->inv_sr_ms) * (float)ramp->samples_per_tick;
    ramp->downInc = ((ramp->dest - ramp->curr) / ramp->downTime * ramp->inv_sr_ms) * (float)ramp->samples_per_tick;
}

void    tRampUpDown_freeFromPool  (tRampUpDown* const r, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tRampUpDown* ramp = *r;

    mpool_free(ramp, m);
}

void     tRampUpDown_setUpTime(tRampUpDown* const ramp, float upTime)
{
    _tRampUpDown* r = *ramp;

    if (upTime < r->minimum_time)
    {
        r->upTime = r->minimum_time;
    }
    else
    {
        r->upTime = upTime;
    }
    r->upInc = ((r->dest - r->curr) / r->upTime * r->inv_sr_ms) * (float)r->samples_per_tick;
}


void     tRampUpDown_setDownTime(tRampUpDown* const ramp, float downTime)
{
    _tRampUpDown* r = *ramp;

    if (downTime < r->minimum_time)
    {
        r->downTime = r->minimum_time;
    }
    else
    {
        r->downTime = downTime;
    }
    r->downInc = ((r->dest - r->curr) / r->downTime * r->inv_sr_ms) * (float)r->samples_per_tick;
}

void     tRampUpDown_setDest(tRampUpDown* const ramp, float dest)
{
    _tRampUpDown* r = *ramp;
    r->dest = dest;
    r->upInc = ((r->dest - r->curr) / r->upTime * r->inv_sr_ms) * (float)r->samples_per_tick;
    r->downInc = ((r->dest - r->curr) / r->downTime * r->inv_sr_ms) * (float)r->samples_per_tick;
}

void     tRampUpDown_setVal(tRampUpDown* const ramp, float val)
{
    _tRampUpDown* r = *ramp;
    r->curr = val;
    r->upInc = ((r->dest - r->curr) / r->upTime * r->inv_sr_ms) * (float)r->samples_per_tick;
    r->downInc = ((r->dest - r->curr) / r->downTime * r->inv_sr_ms) * (float)r->samples_per_tick;
}

float   tRampUpDown_tick(tRampUpDown* const ramp)
{
    _tRampUpDown* r = *ramp;
    float test;

    if (r->dest < r->curr)
    {
    	test = r->curr + r->downInc;
    	if (test > r->dest)
    	{
    		r->curr = test;
    	}
    	else
    	{
    		r->downInc = 0.0f;
    		r->curr = r->dest;
    	}
    }
    else if (r->dest > r->curr)
    {
    	test = r->curr + r->upInc;
    	if (test < r->dest)
    	{
    		r->curr = test;
    	}
    	else
    	{
    		r->upInc = 0.0f;
    		r->curr = r->dest;
    	}
    }
    return r->curr;
}

float   tRampUpDown_sample(tRampUpDown* const ramp)
{
    _tRampUpDown* r = *ramp;
    return r->curr;
}






/* Exponential Smoother */
void    tExpSmooth_init(tExpSmooth* const expsmooth, float val, float factor)
{	// factor is usually a value between 0 and 0.1. Lower value is slower. 0.01 for example gives you a smoothing time of about 10ms
    _tExpSmooth* smooth = *expsmooth = (_tExpSmooth*) leaf_alloc(sizeof(_tExpSmooth));
    
	smooth->curr=val;
	smooth->dest=val;
	if (factor<0) factor=0;
	if (factor>1) factor=1;
	smooth->factor=factor;
	smooth->oneminusfactor=1.0f-factor;
}

void tExpSmooth_free(tExpSmooth* const expsmooth)
{
    _tExpSmooth* smooth = *expsmooth;
    
    leaf_free(smooth);
}

void    tExpSmooth_initToPool   (tExpSmooth* const expsmooth, float val, float factor, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tExpSmooth* smooth = *expsmooth = (_tExpSmooth*) mpool_alloc(sizeof(_tExpSmooth), m);
    
    smooth->curr=val;
    smooth->dest=val;
    if (factor<0) factor=0;
    if (factor>1) factor=1;
    smooth->factor=factor;
    smooth->oneminusfactor=1.0f-factor;
}

void    tExpSmooth_freeFromPool (tExpSmooth* const expsmooth, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tExpSmooth* smooth = *expsmooth;
    
    mpool_free(smooth, m);
}

void     tExpSmooth_setFactor(tExpSmooth* const expsmooth, float factor)
{	// factor is usually a value between 0 and 0.1. Lower value is slower. 0.01 for example gives you a smoothing time of about 10ms
    _tExpSmooth* smooth = *expsmooth;
    
	if (factor<0)
		factor=0;
	else
		if (factor>1) factor=1;
	smooth->factor=factor;
	smooth->oneminusfactor=1.0f-factor;
}

void     tExpSmooth_setDest(tExpSmooth* const expsmooth, float dest)
{
    _tExpSmooth* smooth = *expsmooth;
	smooth->dest=dest;
}

void     tExpSmooth_setVal(tExpSmooth* const expsmooth, float val)
{
    _tExpSmooth* smooth = *expsmooth;
	smooth->curr=val;
}

float   tExpSmooth_tick(tExpSmooth* const expsmooth)
{
    _tExpSmooth* smooth = *expsmooth;
    smooth->curr = smooth->factor*smooth->dest+smooth->oneminusfactor*smooth->curr;
    return smooth->curr;
}

float   tExpSmooth_sample(tExpSmooth* const expsmooth)
{
    _tExpSmooth* smooth = *expsmooth;
    return smooth->curr;
}

//tSlide is based on the max/msp slide~ object
////

void    tSlide_init          (tSlide* const sl, float upSlide, float downSlide)
{
	tSlide_initToPool    (sl, upSlide, downSlide, &leaf.mempool);
}
void    tSlide_free          (tSlide* const sl)
{
	tSlide_freeFromPool    (sl, &leaf.mempool);
}
void    tSlide_initToPool    (tSlide* const sl, float upSlide, float downSlide, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSlide* s = *sl = (_tSlide*) mpool_alloc(sizeof(_tSlide), m);
    s->prevIn = 0.0f;
    s->currentOut = 0.0f;
    s->prevOut = 0.0f;
    if (upSlide < 1.0f)
    {
    	upSlide = 1.0f;
    }

    if (downSlide < 1.0f)
    {
    	downSlide = 1.0f;
    }
    s->invUpSlide = 1.0f / upSlide;
    s->invDownSlide = 1.0f / downSlide;
}

void    tSlide_freeFromPool  (tSlide* const sl, tMempool* const mp)
{
	_tMempool* m = *mp;
	    _tSlide* s = *sl;

	    mpool_free(s, m);
}

float tSlide_tick(tSlide* const sl, float in)
{
	_tSlide* s = *sl;


	if (in >= s->prevOut)
	{
		s->currentOut = s->prevOut + ((in - s->prevOut) * s->invUpSlide);
	}
	else
	{
		s->currentOut = s->prevOut + ((in - s->prevOut) * s->invDownSlide);
	}
	if (s->currentOut < VSF) s->currentOut = 0.0f;
	s->prevIn = in;
	s->prevOut = s->currentOut;
	return s->currentOut;
}

