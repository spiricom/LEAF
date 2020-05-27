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

    adsr->exp_buff = __leaf_table_exp_decay;
    adsr->inc_buff = __leaf_table_attack_decay_inc;
    adsr->buff_size = sizeof(__leaf_table_exp_decay);

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

    adsr->next = 0.0f;

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

    int32_t attackIndex;

    if (attack < 0.0f) {
        attackIndex = 0.0f;
    } else if (attack < 8192.0f) {
        attackIndex = ((int32_t)(attack * 8.0f))-1;
    } else {
        attackIndex = ((int32_t)(8192.0f * 8.0f))-1;
    }

    adsr->attackInc = adsr->inc_buff[attackIndex];
}

void     tADSR_setDecay(tADSR* const adsrenv, float decay)
{
    _tADSR* adsr = *adsrenv;

    int32_t decayIndex;

    if (decay < 0.0f) {
        decayIndex = 0.0f;
    } else if (decay < 8192.0f) {
        decayIndex = ((int32_t)(decay * 8.0f)) - 1;
    } else {
        decayIndex = ((int32_t)(8192.0f * 8.0f)) - 1;
    }

    adsr->decayInc = adsr->inc_buff[decayIndex];
}

void     tADSR_setSustain(tADSR* const adsrenv, float sustain)
{
    _tADSR* adsr = *adsrenv;

    if (sustain > 1.0f)      adsr->sustain = 1.0f;
    else if (sustain < 0.0f) adsr->sustain = 0.0f;
    else                     adsr->sustain = sustain;
}

void     tADSR_setRelease(tADSR* const adsrenv, float release)
{
    _tADSR* adsr = *adsrenv;

    int32_t releaseIndex;

    if (release < 0.0f) {
        releaseIndex = 0.0f;
    } else if (release < 8192.0f) {
        releaseIndex = ((int32_t)(release * 8.0f)) - 1;
    } else {
        releaseIndex = ((int32_t)(8192.0f * 8.0f)) - 1;
    }

    adsr->releaseInc = adsr->inc_buff[releaseIndex];
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
}

void tADSR_off(tADSR* const adsrenv)
{
    _tADSR* adsr = *adsrenv;

    if (adsr->inRelease) return;

    adsr->inAttack = OFALSE;
    adsr->inDecay = OFALSE;
    adsr->inSustain = OFALSE;
    adsr->inRelease = OTRUE;

    adsr->releasePeak = adsr->next;
}

float   tADSR_tick(tADSR* const adsrenv)
{
    _tADSR* adsr = *adsrenv;


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
            adsr->next = (adsr->gain * (adsr->sustain + ((adsr->exp_buff[(uint32_t)adsr->decayPhase]) * (1.0f - adsr->sustain)))) * adsr->leakFactor; // do interpolation !
        }

        // Increment ADSR decay.
        adsr->decayPhase += adsr->decayInc;
    }

    if (adsr->inSustain)
    {
        adsr->next = adsr->next * adsr->leakFactor;
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




/* ADSR 2*/
//This one is adapted from the VCV Rack code
//-JS



const float ADSR2_MIN_TIME = 1e-3f;
const float ADSR2_INV_MIN_TIME = 1000.0f;
const float ADSR2_MAX_TIME = 10.f;
const float ADSR2_LAMBDA_BASE = 10000.0f;


void    tADSR2_init(tADSR2* const adsrenv, float attack, float decay, float sustain, float release)
{
    tADSR2_initToPool(adsrenv, attack, decay, sustain, release, &leaf.mempool);
}

void tADSR2_free(tADSR2* const adsrenv)
{
    tADSR2_freeFromPool(adsrenv, &leaf.mempool);
}

void    tADSR2_initToPool    (tADSR2* const adsrenv, float attack, float decay, float sustain, float release, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tADSR2* adsr = *adsrenv = (_tADSR2*) mpool_alloc(sizeof(_tADSR2), m);
    adsr->sampleRateInMs =  leaf.sampleRate * 0.001f;
    adsr->attack = LEAF_clip(0.0f, attack * 0.001f, 1.0f);
    adsr->attackLambda = powf(ADSR2_LAMBDA_BASE, -adsr->attack) * ADSR2_INV_MIN_TIME;


    adsr->decay = LEAF_clip(0.0f, decay * 0.001f, 1.0f);
    adsr->decayLambda = powf(ADSR2_LAMBDA_BASE, -adsr->decay) * ADSR2_INV_MIN_TIME;


    adsr->sustain= sustain;


    adsr->release = LEAF_clip(0.0f, release * 0.001f, 1.0f);
    adsr->releaseLambda = powf(ADSR2_LAMBDA_BASE, -adsr->release) * ADSR2_INV_MIN_TIME;

    adsr->attacking = 0;
    adsr->gate = 0;
    adsr->gain = 1.0f;
    adsr->targetGainSquared = 1.0f;
    adsr->factor = 0.01f;
    adsr->oneMinusFactor = 0.99f;
    adsr->env = 0.0f;
    adsr->leakFactor = 1.0f;
}

void    tADSR2_freeFromPool  (tADSR2* const adsrenv, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tADSR2* adsr = *adsrenv;
    mpool_free(adsr, m);
}

void     tADSR2_setAttack(tADSR2* const adsrenv, float attack)
{
    _tADSR2* adsr = *adsrenv;
    adsr->attack = LEAF_clip(0.0f, attack * 0.001f, 1.0f);
    adsr->attackLambda = fastPow(ADSR2_LAMBDA_BASE, -adsr->attack) * ADSR2_INV_MIN_TIME;

}

void     tADSR2_setDecay(tADSR2* const adsrenv, float decay)
{
    _tADSR2* adsr = *adsrenv;
    adsr->decay = LEAF_clip(0.0f, decay * 0.001f, 1.0f);
    adsr->decayLambda = fastPow(ADSR2_LAMBDA_BASE, -adsr->decay) * ADSR2_INV_MIN_TIME;

}

void     tADSR2_setSustain(tADSR2* const adsrenv, float sustain)
{
    _tADSR2* adsr = *adsrenv;
    adsr->sustain = sustain;

    if (adsr->gate)
    {
        if (adsr->attacking == 0)
        {
            adsr->envTarget = sustain;
        }
    }
}

void     tADSR2_setRelease(tADSR2* const adsrenv, float release)
{
    _tADSR2* adsr = *adsrenv;
    adsr->release = LEAF_clip(0.0f, release * 0.001f, 1.0f);
    adsr->releaseLambda = fastPow(ADSR2_LAMBDA_BASE, -adsr->release) * ADSR2_INV_MIN_TIME;
}

// 0.999999 is slow leak, 0.9 is fast leak
void     tADSR2_setLeakFactor(tADSR2* const adsrenv, float leakFactor)
{
    _tADSR2* adsr = *adsrenv;

    adsr->leakFactor = leakFactor;
}

void tADSR2_on(tADSR2* const adsrenv, float velocity)
{
    _tADSR2* adsr = *adsrenv;
    adsr->targetGainSquared = velocity * velocity;
    adsr->envTarget = 1.2f;
    adsr->leakGain = 1.0f;
    adsr->gate = 1;
    adsr->attacking = 1;
}

void tADSR2_off(tADSR2* const adsrenv)
{
    _tADSR2* adsr = *adsrenv;
    adsr->gate = 0;
    adsr->envTarget = 0.0f;
}

float   tADSR2_tick(tADSR2* const adsrenv)
{
    _tADSR2* adsr = *adsrenv;
    float lambda;

    if (adsr->gate)
    {
        if (adsr->attacking)
        {
            lambda = adsr->attackLambda;
        }
        else
        {
            lambda = adsr->decayLambda;
        }
    }
    else
    {
        lambda = adsr->releaseLambda;
    }


    // Adjust env
    adsr->env += (adsr->envTarget - adsr->env) * lambda * leaf.invSampleRate;

    // Turn off attacking state if envelope is HIGH
    if (adsr->env >= 1.0f)
    {
        adsr->attacking = 0;
        adsr->envTarget = adsr->sustain;
    }

    //smooth the gain value   -- this is not ideal, a retrigger while the envelope is still going with a new gain will cause a jump, although it will be smoothed quickly. Maybe doing the math so the range is computed based on the gain rather than 0.->1. is preferable? But that's harder to get the exponential curve right without a lookup.
    adsr->gain = ((adsr->factor*adsr->targetGainSquared)+(adsr->oneMinusFactor*adsr->gain));
    adsr->leakGain *= adsr->leakFactor;
    return adsr->env * adsr->gain * adsr->leakGain;
}

/* ADSR 3*/
//This one doesn't use any lookup table - by Nigel Redmon from his blog. Thanks, Nigel!
//-JS

float calcADSR3Coef(double rate, double targetRatio)
{
    return (rate <= 0.0f) ? 0.0f : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}


void    tADSR3_init(tADSR3* const adsrenv, float attack, float decay, float sustain, float release)
{
    tADSR3_initToPool(adsrenv, attack, decay, sustain, release, &leaf.mempool);
}

void tADSR3_free(tADSR3* const adsrenv)
{
    tADSR3_freeFromPool(adsrenv, &leaf.mempool);
}

void    tADSR3_initToPool    (tADSR3* const adsrenv, float attack, float decay, float sustain, float release, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tADSR3* adsr = *adsrenv = (_tADSR3*) mpool_alloc(sizeof(_tADSR3), m);
    adsr->sampleRateInMs =  leaf.sampleRate * 0.001f;
    adsr->targetRatioA = 0.3f;
    adsr->targetRatioDR = 0.0001f;
    adsr->attackRate = attack * adsr->sampleRateInMs;
    adsr->attackCoef = calcADSR3Coef(attack * adsr->sampleRateInMs, adsr->targetRatioA);
    adsr->attackBase = (1.0f + adsr->targetRatioA) * (1.0f - adsr->attackCoef);

    adsr->decayRate = decay * adsr->sampleRateInMs;
    adsr->decayCoef = calcADSR3Coef(decay * adsr->sampleRateInMs,adsr-> targetRatioDR);
    adsr->decayBase = (adsr->sustainLevel - adsr->targetRatioDR) * (1.0f - adsr->decayCoef);

    adsr->sustainLevel = sustain;
    adsr->decayBase = (adsr->sustainLevel - adsr->targetRatioDR) * (1.0f - adsr->decayCoef);

    adsr->releaseRate = release * adsr->sampleRateInMs;
    adsr->releaseCoef = calcADSR3Coef(release * adsr->sampleRateInMs, adsr->targetRatioDR);
    adsr->releaseBase = -adsr->targetRatioDR * (1.0f - adsr->releaseCoef);

    adsr->state = env_idle;
    adsr->gain = 1.0f;
    adsr->targetGainSquared = 1.0f;
    adsr->factor = 0.01f;
    adsr->oneMinusFactor = 0.99f;
    adsr->output = 0.0f;
    adsr->leakFactor = 1.0f;
}

void    tADSR3_freeFromPool  (tADSR3* const adsrenv, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tADSR3* adsr = *adsrenv;
    mpool_free(adsr, m);
}

void     tADSR3_setAttack(tADSR3* const adsrenv, float attack)
{
    _tADSR3* adsr = *adsrenv;

    adsr->attackRate = attack * adsr->sampleRateInMs;
    adsr->attackCoef = calcADSR3Coef(adsr->attackRate, adsr->targetRatioA);
    adsr->attackBase = (1.0f + adsr->targetRatioA) * (1.0f - adsr->attackCoef);
}

void     tADSR3_setDecay(tADSR3* const adsrenv, float decay)
{
    _tADSR3* adsr = *adsrenv;

    adsr->decayRate = decay * adsr->sampleRateInMs;
    adsr->decayCoef = calcADSR3Coef(adsr->decayRate,adsr-> targetRatioDR);
    adsr->decayBase = (adsr->sustainLevel - adsr->targetRatioDR) * (1.0f - adsr->decayCoef);
}

void     tADSR3_setSustain(tADSR3* const adsrenv, float sustain)
{
    _tADSR3* adsr = *adsrenv;

    adsr->sustainLevel = sustain;
    adsr->decayBase = (adsr->sustainLevel - adsr->targetRatioDR) * (1.0f - adsr->decayCoef);
}

void     tADSR3_setRelease(tADSR3* const adsrenv, float release)
{
    _tADSR3* adsr = *adsrenv;

    adsr->releaseRate = release * adsr->sampleRateInMs;
    adsr->releaseCoef = calcADSR3Coef(adsr->releaseRate, adsr->targetRatioDR);
    adsr->releaseBase = -adsr->targetRatioDR * (1.0f - adsr->releaseCoef);
}

// 0.999999 is slow leak, 0.9 is fast leak
void     tADSR3_setLeakFactor(tADSR3* const adsrenv, float leakFactor)
{
    _tADSR3* adsr = *adsrenv;

    adsr->leakFactor = leakFactor;
}

void tADSR3_on(tADSR3* const adsrenv, float velocity)
{
    _tADSR3* adsr = *adsrenv;
    adsr->state = env_attack;
    adsr->targetGainSquared = velocity * velocity;
}

void tADSR3_off(tADSR3* const adsrenv)
{
    _tADSR3* adsr = *adsrenv;

    if (adsr->state != env_idle)
    {
        adsr->state = env_release;
    }
}

float   tADSR3_tick(tADSR3* const adsrenv)
{
    _tADSR3* adsr = *adsrenv;


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
    //smooth the gain value   -- this is not ideal, a retrigger while the envelope is still going with a new gain will cause a jump, although it will be smoothed quickly. Maybe doing the math so the range is computed based on the gain rather than 0.->1. is preferable? But that's harder to get the exponential curve right without a lookup.
    adsr->gain = (adsr->factor*adsr->targetGainSquared)+(adsr->oneMinusFactor*adsr->gain);
    return adsr->output * adsr->gain;
}


/* ADSR 4 */ // new version of our original table-based ADSR but with the table passed in by the user
// use this if the size of the big ADSR tables is too much.
void    tADSR4_init    (tADSR4* const adsrenv, float attack, float decay, float sustain, float release, float* expBuffer, int bufferSize)
{
    tADSR4_initToPool    (adsrenv, attack, decay, sustain, release, expBuffer, bufferSize, &leaf.mempool);
}

void tADSR4_free(tADSR4* const adsrenv)
{
    tADSR4_freeFromPool(adsrenv, &leaf.mempool);
}

//initialize with an exponential function that decays -- i.e. a call to LEAF_generate_exp(expBuffer, 0.001f, 0.0f, 1.0f, -0.0008f, EXP_BUFFER_SIZE);
//times are in ms
void    tADSR4_initToPool    (tADSR4* const adsrenv, float attack, float decay, float sustain, float release, float* expBuffer, int bufferSize, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tADSR4* adsr = *adsrenv = (_tADSR4*) mpool_alloc(sizeof(_tADSR4), m);

    adsr->exp_buff = expBuffer;
    adsr->buff_size = bufferSize;
    adsr->buff_sizeMinusOne = bufferSize - 1;

    adsr->bufferSizeDividedBySampleRateInMs = bufferSize / (leaf.sampleRate * 0.001f);

    if (attack < 0.0f)
        attack = 0.0f;

    if (decay < 0.0f)
        decay = 0.0f;

    if (sustain > 1.0f)
        sustain = 1.0f;
    if (sustain < 0.0f)
        sustain = 0.0f;

    if (release < 0.0f)
        release = 0.0f;

    adsr->next = 0.0f;

    adsr->whichStage = env_idle;

    adsr->sustain = sustain;

    adsr->attackInc = adsr->bufferSizeDividedBySampleRateInMs / attack;
    adsr->decayInc = adsr->bufferSizeDividedBySampleRateInMs / decay;
    adsr->releaseInc = adsr->bufferSizeDividedBySampleRateInMs / release;
    adsr->rampInc = adsr->bufferSizeDividedBySampleRateInMs / 8.0f;

    adsr->leakFactor = 1.0f;
}

void    tADSR4_freeFromPool  (tADSR4* const adsrenv, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tADSR4* adsr = *adsrenv;
    mpool_free(adsr, m);
}

void     tADSR4_setAttack(tADSR4* const adsrenv, float attack)
{
    _tADSR4* adsr = *adsrenv;

    if (attack < 0.0f)
    {
        attack = 0.0f;
    }

    adsr->attackInc = adsr->bufferSizeDividedBySampleRateInMs / attack;
}

void     tADSR4_setDecay(tADSR4* const adsrenv, float decay)
{
    _tADSR4* adsr = *adsrenv;

    if (decay < 0.0f)
    {
        decay = 0.0f;
    }
    adsr->decayInc = adsr->bufferSizeDividedBySampleRateInMs / decay;
}

void     tADSR4_setSustain(tADSR4* const adsrenv, float sustain)
{
    _tADSR4* adsr = *adsrenv;

    if (sustain > 1.0f)      adsr->sustain = 1.0f;
    else if (sustain < 0.0f) adsr->sustain = 0.0f;
    else                     adsr->sustain = sustain;
}

void     tADSR4_setRelease(tADSR4* const adsrenv, float release)
{
    _tADSR4* adsr = *adsrenv;

    if (release < 0.0f)
    {
        release = 0.0f;
    }
    adsr->releaseInc = adsr->bufferSizeDividedBySampleRateInMs / release;
}

// 0.999999 is slow leak, 0.9 is fast leak
void     tADSR4_setLeakFactor(tADSR4* const adsrenv, float leakFactor)
{
    _tADSR4* adsr = *adsrenv;


    adsr->leakFactor = leakFactor;
}

void tADSR4_on(tADSR4* const adsrenv, float velocity)
{
    _tADSR4* adsr = *adsrenv;

    if (adsr->whichStage != env_idle) // In case ADSR retriggered while it is still happening.
    {
        adsr->rampPhase = 0;
        adsr->whichStage = env_ramp;
        adsr->rampPeak = adsr->next;
    }
    else // Normal start.
    {
        adsr->whichStage = env_attack;
    }

    adsr->attackPhase = 0;
    adsr->decayPhase = 0;
    adsr->releasePhase = 0;
    adsr->gain = velocity;
}

void tADSR4_off(tADSR4* const adsrenv)
{
    _tADSR4* adsr = *adsrenv;

    if (adsr->whichStage == env_idle)
    {
        return;
    }
    else
    {
        adsr->whichStage = env_release;
        adsr->releasePeak = adsr->next;
    }
}

float   tADSR4_tick(tADSR4* const adsrenv)
{
    _tADSR4* adsr = *adsrenv;

    switch (adsr->whichStage)
    {
        case env_ramp:
            if (adsr->rampPhase > adsr->buff_sizeMinusOne)
            {
                adsr->whichStage = env_attack;
                adsr->next = 0.0f;
            }
            else
            {
                uint32_t intPart = (uint32_t)adsr->rampPhase;
                float floatPart = adsr->rampPhase - intPart;
                float secondValue;
                if (adsr->rampPhase + 1.0f > adsr->buff_sizeMinusOne)
                {
                    secondValue = 0.0f;
                }
                else
                {
                    secondValue = adsr->exp_buff[(uint32_t)((adsr->rampPhase)+1)];
                }
                adsr->next = adsr->rampPeak * LEAF_interpolation_linear(adsr->exp_buff[intPart], secondValue, floatPart);
            }

            adsr->rampPhase += adsr->rampInc;
            break;


        case env_attack:

            // If attack done, time to turn around.
            if (adsr->attackPhase > adsr->buff_sizeMinusOne)
            {
                adsr->whichStage = env_decay;
                adsr->next = adsr->gain;
            }
            else
            {
                // do interpolation !
                uint32_t intPart = (uint32_t)adsr->attackPhase;
                float floatPart = adsr->attackPhase - intPart;
                float secondValue;
                if (adsr->attackPhase + 1.0f > adsr->buff_sizeMinusOne)
                {
                    secondValue = 0.0f;
                }
                else
                {
                    secondValue = adsr->exp_buff[(uint32_t)((adsr->attackPhase)+1)];
                }

                adsr->next = adsr->gain * (1.0f - LEAF_interpolation_linear(adsr->exp_buff[intPart], secondValue, floatPart)); // inverted and backwards to get proper rising exponential shape/perception
            }

            // Increment ADSR attack.
            adsr->attackPhase += adsr->attackInc;
            break;

        case env_decay:

            // If decay done, sustain.
            if (adsr->decayPhase > adsr->buff_sizeMinusOne)
            {
                adsr->whichStage = env_sustain;
                adsr->next = adsr->gain * adsr->sustain;
            }

            else
            {
                uint32_t intPart = (uint32_t)adsr->decayPhase;
                float floatPart = adsr->decayPhase - intPart;
                float secondValue;
                if (adsr->decayPhase + 1.0f > adsr->buff_sizeMinusOne)
                {
                    secondValue = 0.0f;
                }
                else
                {
                    secondValue = adsr->exp_buff[(uint32_t)((adsr->decayPhase)+1)];
                }
                float interpValue = (LEAF_interpolation_linear(adsr->exp_buff[intPart], secondValue, floatPart));
                adsr->next = (adsr->gain * (adsr->sustain + (interpValue * (1.0f - adsr->sustain)))) * adsr->leakFactor; // do interpolation !
            }

            // Increment ADSR decay.
            adsr->decayPhase += adsr->decayInc;
            break;

        case env_sustain:
            adsr->next = adsr->next * adsr->leakFactor;
            break;

        case env_release:
            // If release done, finish.
            if (adsr->releasePhase > adsr->buff_sizeMinusOne)
            {
                adsr->whichStage = env_idle;
                adsr->next = 0.0f;
            }
            else {
                uint32_t intPart = (uint32_t)adsr->releasePhase;
                float floatPart = adsr->releasePhase - intPart;
                float secondValue;
                if (adsr->releasePhase + 1.0f > adsr->buff_sizeMinusOne)
                {
                    secondValue = 0.0f;
                }
                else
                {
                    secondValue = adsr->exp_buff[(uint32_t)((adsr->releasePhase)+1)];
                }
                adsr->next = adsr->releasePeak * (LEAF_interpolation_linear(adsr->exp_buff[intPart], secondValue, floatPart)); // do interpolation !
            }

            // Increment envelope release;
            adsr->releasePhase += adsr->releaseInc;
            break;
    }
    return adsr->next;
}

float   tADSR4_tickNoInterp(tADSR4* const adsrenv)
{
    _tADSR4* adsr = *adsrenv;

    switch (adsr->whichStage)
    {
        case env_ramp:
            if (adsr->rampPhase > adsr->buff_sizeMinusOne)
            {
                adsr->whichStage = env_attack;
                adsr->next = 0.0f;
            }
            else
            {
                adsr->next = adsr->rampPeak * adsr->exp_buff[(uint32_t)adsr->rampPhase];
            }

            adsr->rampPhase += adsr->rampInc;
            break;


        case env_attack:

            // If attack done, time to turn around.
            if (adsr->attackPhase > adsr->buff_sizeMinusOne)
            {
                adsr->whichStage = env_decay;
                adsr->next = adsr->gain;
            }
            else
            {
                adsr->next = adsr->gain * (1.0f - adsr->exp_buff[(uint32_t)adsr->attackPhase]); // inverted and backwards to get proper rising exponential shape/perception
            }

            // Increment ADSR attack.
            adsr->attackPhase += adsr->attackInc;
            break;

        case env_decay:

            // If decay done, sustain.
            if (adsr->decayPhase > adsr->buff_sizeMinusOne)
            {
                adsr->whichStage = env_sustain;
                adsr->next = adsr->gain * adsr->sustain;
            }

            else
            {
                adsr->next = (adsr->gain * (adsr->sustain + (adsr->exp_buff[(uint32_t)adsr->decayPhase] * (1.0f - adsr->sustain)))) * adsr->leakFactor;
            }

            // Increment ADSR decay.
            adsr->decayPhase += adsr->decayInc;
            break;

        case env_sustain:
            adsr->next = adsr->next * adsr->leakFactor;
            break;

        case env_release:
            // If release done, finish.
            if (adsr->releasePhase > adsr->buff_sizeMinusOne)
            {
                adsr->whichStage = env_idle;
                adsr->next = 0.0f;
            }
            else {
                adsr->next = adsr->releasePeak * adsr->exp_buff[(uint32_t)adsr->releasePhase];
            }

            // Increment envelope release;
            adsr->releasePhase += adsr->releaseInc;
            break;
    }
    return adsr->next;
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

void     tExpSmooth_setValAndDest(tExpSmooth* const expsmooth, float val)
{
    _tExpSmooth* smooth = *expsmooth;
	smooth->curr=val;
	smooth->dest=val;
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

