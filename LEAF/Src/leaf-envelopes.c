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

void tADSR_free(tADSR* const adsr)
{
    
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

void tRamp_free(tRamp* const ramp)
{
    
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

void tExpSmooth_free(tExpSmooth* const smooth)
{
    
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

