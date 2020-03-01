/*==============================================================================

    leaf-dynamics.c
    Created: 30 Nov 2018 11:56:49am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-dynamics.h"

#else

#include "../Inc/leaf-dynamics.h"

#endif

//==============================================================================

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
void    tCompressor_init(tCompressor* const comp)
{
    _tCompressor* c = *comp = (_tCompressor*) leaf_alloc(sizeof(_tCompressor));
    
    c->tauAttack = 100;
    c->tauRelease = 100;
    
    c->isActive = OFALSE;
    
    c->T = 0.0f; // Threshold
    c->R = 0.5f; // compression Ratio
    c->M = 3.0f; // decibel Width of knee transition
    c->W = 1.0f; // decibel Make-up gain
}

void tCompressor_free(tCompressor* const comp)
{
    _tCompressor* c = *comp;
    
    leaf_free(c);
}

void    tCompressor_initToPool  (tCompressor* const comp, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tCompressor* c = *comp = (_tCompressor*) mpool_alloc(sizeof(_tCompressor), &m->pool);
    
    c->tauAttack = 100;
    c->tauRelease = 100;
    
    c->isActive = OFALSE;
    
    c->T = 0.0f; // Threshold
    c->R = 0.5f; // compression Ratio
    c->M = 3.0f; // decibel Width of knee transition
    c->W = 1.0f; // decibel Make-up gain
}

void    tCompressor_freeFromPool(tCompressor* const comp, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tCompressor* c = *comp;
    
    mpool_free(c, &m->pool);
}

float tCompressor_tick(tCompressor* const comp, float in)
{
    _tCompressor* c = *comp;
    
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
    
    return attenuation * in;
}

/* Feedback Leveler */

void    tFeedbackLeveler_init(tFeedbackLeveler* const fb, float targetLevel, float factor, float strength, int mode)
{
    _tFeedbackLeveler* p = *fb = (_tFeedbackLeveler*) leaf_alloc(sizeof(_tFeedbackLeveler));
    
    p->curr=0.0f;
    p->targetLevel=targetLevel;
    tPowerFollower_init(&p->pwrFlw,factor);
    p->mode=mode;
    p->strength=strength;
}

void tFeedbackLeveler_free(tFeedbackLeveler* const fb)
{
    _tFeedbackLeveler* p = *fb;
    
    tPowerFollower_free(&p->pwrFlw);
    leaf_free(p);
}

void    tFeedbackLeveler_initToPool     (tFeedbackLeveler* const fb, float targetLevel, float factor, float strength, int mode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tFeedbackLeveler* p = *fb = (_tFeedbackLeveler*) mpool_alloc(sizeof(_tFeedbackLeveler), &m->pool);
    
    p->curr=0.0f;
    p->targetLevel=targetLevel;
    tPowerFollower_initToPool(&p->pwrFlw,factor, mp);
    p->mode=mode;
    p->strength=strength;
}

void    tFeedbackLeveler_freeFromPool   (tFeedbackLeveler* const fb, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tFeedbackLeveler* p = *fb;
    
    tPowerFollower_freeFromPool(&p->pwrFlw, mp);
    mpool_free(p, &m->pool);
}

void     tFeedbackLeveler_setStrength(tFeedbackLeveler* const fb, float strength)
{    // strength is how strongly level diff is affecting the amp ratio
    // try 0.125 for a start
    _tFeedbackLeveler* p = *fb;
    p->strength=strength;
}

void     tFeedbackLeveler_setFactor(tFeedbackLeveler* const fb, float factor)
{
    _tFeedbackLeveler* p = *fb;
    tPowerFollower_setFactor(&p->pwrFlw,factor);
}

void     tFeedbackLeveler_setMode(tFeedbackLeveler* const fb, int mode)
{    // 0 for decaying with upwards lev limiting, 1 for constrained absolute level (also downwards limiting)
    _tFeedbackLeveler* p = *fb;
    p->mode=mode;
}

float   tFeedbackLeveler_tick(tFeedbackLeveler* const fb, float input)
{
    _tFeedbackLeveler* p = *fb;
    float levdiff=(tPowerFollower_tick(&p->pwrFlw, input)-p->targetLevel);
    if (p->mode==0 && levdiff<0.0f) levdiff=0.0f;
    p->curr=input*(1.0f-p->strength*levdiff);
    return p->curr;
}

float   tFeedbackLeveler_sample(tFeedbackLeveler* const fb)
{
    _tFeedbackLeveler* p = *fb;
    return p->curr;
}


void     tFeedbackLeveler_setTargetLevel   (tFeedbackLeveler* const fb, float TargetLevel)
{
    _tFeedbackLeveler* p = *fb;
    p->targetLevel=TargetLevel;
}

