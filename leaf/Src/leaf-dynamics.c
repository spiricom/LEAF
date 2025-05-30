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
void tCompressor_init (tCompressor** const comp, LEAF* const leaf)
{
    tCompressor_initToPool(comp, &leaf->mempool);
}

void tCompressor_initToPool (tCompressor** const comp, tMempool* const mp)
{
    _tMempool* m = *mp;
    tCompressor* c = *comp = (tCompressor*) mpool_alloc(sizeof(tCompressor), m);
    c->mempool = m;
    LEAF* leaf = c->mempool->leaf;
    c->sampleRate = leaf->sampleRate;
    
    c->tauAttack = expf(-1.0f/(0.001f * 50.0f * c->sampleRate));
    c->tauRelease = expf(-1.0f/(0.001f * 100.0f * c->sampleRate));
    
    c->isActive = 0;
    c->x_G[0] = 0.0f;
    c->x_G[1] = 0.0f;
    c->y_G[0] = 0.0f;
    c->y_G[1] = 0.0f;
    c->x_T[0] = 0.0f;
    c->x_T[1] = 0.0f;
    c->y_T[0] = 0.0f;
    c->y_T[1] = 0.0f;
    c->T = 0.0f; // Threshold
    c->R = 0.5f; // compression Ratio
    c->invR = 2.0f; //inverse of ratio
    c->M = 3.0f; // decibel Make-up gain
    c->W = 1.5f; // decibel Width of knee transition
    c->inv4W = 1.0f / (4.0f * c->W) ;
    

}

void tCompressor_free (tCompressor* const comp)
{
    tCompressor* c = &*comp;
    
    mpool_free((char*)c, c->mempool);
}

Lfloat tCompressor_tick(tCompressor* const c, Lfloat in)
{
    Lfloat slope, overshoot;
    
    Lfloat in_db = LEAF_clip(-90.0f, fasteratodb(fastabsf(in)), 0.0f);
    Lfloat out_db = 0.0f;
    
    c->y_T[1] = c->y_T[0];
    
    slope = 1.0f - c->invR; // feed-forward topology;
    
    overshoot = in_db - c->T;
    

    if (overshoot <= -(c->W))
    {
        out_db = in_db;
        c->isActive = 0;
    }
    else if ((overshoot > -(c->W)) && (overshoot < (c->W)))
    {
        Lfloat squareit = (overshoot + c->W);
        out_db = in_db + slope * ((squareit * squareit) * c->inv4W); // .^ 2 ???
        c->isActive = 1;
    }
    else
    {
        out_db = in_db + slope * overshoot;
        c->isActive = 1;
    }
/*  // simpler hard-knee version
    if (overshoot <= 0.0f)
    {
        out_db = in_db;
        c->isActive = 0;
    }
    else
    {
        out_db = in_db + slope * overshoot;
        c->isActive = 1;
    }
    */
    c->x_T[0] = out_db - in_db;
    if (c->x_T[0] > c->y_T[1])
        c->y_T[0] = c->tauAttack * c->y_T[1] + (1.0f-c->tauAttack) * c->x_T[0];
    else
        c->y_T[0] = c->tauRelease * c->y_T[1] + (1.0f-c->tauRelease) * c->x_T[0];
    Lfloat attenuation = fasterdbtoa(c->M - c->y_T[0]);
    return attenuation * in;
}

//requires tables to be set with set function
//more efficient without soft knee calculation
Lfloat tCompressor_tickWithTable(tCompressor* const c, Lfloat in)
{
    Lfloat slope, overshoot;

    in = fastabsf(in);
    int inAmpIndex = LEAF_clip (0, (in * c->atodbScalar) - c->atodbOffset, c->atodbTableSizeMinus1);
    Lfloat in_db = c->atodbTable[inAmpIndex];
    Lfloat out_db = 0.0f;

    c->y_T[1] = c->y_T[0];

    slope = 1.0f - c->invR; // feed-forward topology;

    overshoot = in_db - c->T;


    if (overshoot <= -(c->W))
    {
        out_db = in_db;
        c->isActive = 0;
    }
    else if ((overshoot > -(c->W)) && (overshoot < (c->W)))
    {
        Lfloat squareit = (overshoot + c->W);
        out_db = in_db + slope * ((squareit * squareit) * c->inv4W); // .^ 2 ???
        c->isActive = 1;
    }
    else
    {
        out_db = in_db + slope * overshoot;
        c->isActive = 1;
    }

    c->x_T[0] = out_db - in_db;
    if (c->x_T[0] > c->y_T[1])
        c->y_T[0] = c->tauAttack * c->y_T[1] + (1.0f-c->tauAttack) * c->x_T[0];
    else
        c->y_T[0] = c->tauRelease * c->y_T[1] + (1.0f-c->tauRelease) * c->x_T[0];
    Lfloat attenuationDb = c->M - c->y_T[0];
   // tempdbToA = dbtoa(attenuationDb);
    int attenuationDbIndex = LEAF_clip (0, (attenuationDb * c->dbtoaScalar) - c->dbtoaOffset, c->dbtoaTableSizeMinus1);
    Lfloat attenuation = c->dbtoaTable[attenuationDbIndex];
    return attenuation * in;
}

//requires tables to be set with set function
Lfloat tCompressor_tickWithTableHardKnee(tCompressor* const c, Lfloat in)
{
    Lfloat slope, overshoot;

    in = fastabsf(in);
    int inAmpIndex = LEAF_clip (0, (in * c->atodbScalar) - c->atodbOffset, c->atodbTableSizeMinus1);
    Lfloat in_db = c->atodbTable[inAmpIndex];
    Lfloat out_db = 0.0f;

    c->y_T[1] = c->y_T[0];

    slope = 1.0f - c->invR; // feed-forward topology;

    overshoot = in_db - c->T;

  // simpler hard-knee version (more efficient)
    if (overshoot <= 0.0f)
    {
        out_db = in_db;
        c->isActive = 0;
    }
    else
    {
        out_db = in_db + slope * overshoot;
        c->isActive = 1;
    }

    c->x_T[0] = out_db - in_db;
    if (c->x_T[0] > c->y_T[1])
        c->y_T[0] = c->tauAttack * c->y_T[1] + (1.0f-c->tauAttack) * c->x_T[0];
    else
        c->y_T[0] = c->tauRelease * c->y_T[1] + (1.0f-c->tauRelease) * c->x_T[0];
    Lfloat attenuationDb = c->M - c->y_T[0];
   // tempdbToA = dbtoa(attenuationDb);
    int attenuationDbIndex = LEAF_clip (0, (attenuationDb * c->dbtoaScalar) - c->dbtoaOffset, c->dbtoaTableSizeMinus1);
    Lfloat attenuation = c->dbtoaTable[attenuationDbIndex];
    return attenuation * in;
}

void tCompressor_setTables(tCompressor* const c, Lfloat* atodb, Lfloat* dbtoa, Lfloat atodbMinIn, Lfloat atodbMaxIn, Lfloat dbtoaMinIn, Lfloat dbtoaMaxIn, int atodbTableSize, int dbtoaTableSize)
{
	c->atodbTable = atodb;
	c->dbtoaTable = dbtoa;

	c->atodbTableSizeMinus1 = atodbTableSize-1;
	c->dbtoaTableSizeMinus1 = dbtoaTableSize-1;

	c->atodbScalar = c->atodbTableSizeMinus1/(atodbMaxIn-atodbMinIn);
	c->atodbOffset = atodbMinIn * c->atodbScalar;


	c->dbtoaScalar = c->dbtoaTableSizeMinus1/(dbtoaMaxIn-dbtoaMinIn);
	c->dbtoaOffset = dbtoaMinIn * c->dbtoaScalar;

}

////c->tauAttack = 100;
//c->tauRelease = 100;
//
//c->isActive = 0;
//
//c->T = 0.0f; // Threshold
//c->R = 0.5f; // compression Ratio
//c->W = 3.0f; // decibel Width of knee transition
//c->M = 1.0f; // decibel Make-up gain
void tCompressor_setParams(tCompressor* const c, Lfloat thresh, Lfloat ratio, Lfloat knee, Lfloat makeup, Lfloat attack, Lfloat release)
{
    c->T = thresh;
    c->R = ratio;
    c->invR = 1.0f / ratio;
    c->W = knee * 0.5f;
    c->inv4W = 1.0f / (2.0f * knee);
    c->M = makeup;
    c->tauAttack = fastExp4(-1.0f/(0.001f * attack * c->sampleRate));
    c->tauRelease = fastExp4(-1.0f/(0.001f * release * c->sampleRate));
}

void tCompressor_setSampleRate(tCompressor* const c, Lfloat sampleRate)
{
    c->sampleRate = sampleRate;
}
/* Feedback Leveler */

void tFeedbackLeveler_init (tFeedbackLeveler* const fb, Lfloat targetLevel, Lfloat factor, Lfloat strength, int mode, LEAF* const leaf)
{
    tFeedbackLeveler_initToPool(fb, targetLevel, factor, strength, mode, &leaf->mempool);
}

void tFeedbackLeveler_initToPool (tFeedbackLeveler** const fb, Lfloat targetLevel, Lfloat factor, Lfloat strength, int mode, tMempool* const mp)
{
    _tMempool* m = *mp;
    tFeedbackLeveler* p = *fb = (tFeedbackLeveler*) mpool_alloc(sizeof(tFeedbackLeveler), m);
    p->mempool = m;
    
    p->curr=0.0f;
    p->targetLevel=targetLevel;
    tPowerFollower_initToPool(&p->pwrFlw, factor, mp);
    p->mode=mode;
    p->strength=strength;
}

void tFeedbackLeveler_free (tFeedbackLeveler** const fb)
{
    tFeedbackLeveler* p = *fb;
    
    tPowerFollower_free(&p->pwrFlw);
    mpool_free((char*)p, p->mempool);
}

void     tFeedbackLeveler_setStrength(tFeedbackLeveler* const p, Lfloat strength)
{    // strength is how strongly level diff is affecting the amp ratio
    // try 0.125 for a start
    p->strength=strength;
}

void     tFeedbackLeveler_setFactor(tFeedbackLeveler* const p, Lfloat factor)
{
    tPowerFollower_setFactor(&p->pwrFlw,factor);
}

void     tFeedbackLeveler_setMode(tFeedbackLeveler* const p, int mode)
{    // 0 for decaying with upwards lev limiting, 1 for constrained absolute level (also downwards limiting)
    p->mode=mode;
}

Lfloat   tFeedbackLeveler_tick(tFeedbackLeveler* const p, Lfloat input)
{
    Lfloat levdiff=(tPowerFollower_tick(&p->pwrFlw, input)-p->targetLevel);
    if (p->mode==0 && levdiff<0.0f) levdiff=0.0f;
    p->curr=input*(1.0f-p->strength*levdiff);
    return p->curr;
}

Lfloat   tFeedbackLeveler_sample(tFeedbackLeveler* const p)
{
    return p->curr;
}


void     tFeedbackLeveler_setTargetLevel   (tFeedbackLeveler* const p, Lfloat TargetLevel)
{
    p->targetLevel=TargetLevel;
}


void tThreshold_init (tThreshold* const th, Lfloat low, Lfloat high, LEAF* const leaf)
{
	tThreshold_initToPool(th, low, high, &leaf->mempool);
}

void tThreshold_initToPool (tThreshold** const th, Lfloat low, Lfloat high, tMempool* const mp)
{
    _tMempool* m = *mp;
    tThreshold* t = *th = (tThreshold*) mpool_alloc(sizeof(tThreshold), m);
    t->mempool = m;
    
    t->highThresh = high;
    t->lowThresh = low;

    t->currentValue = 0;
}

void tThreshold_free (tThreshold** const th)
{
    tThreshold* t = *th;

    mpool_free((char*)t, t->mempool);
}

int tThreshold_tick(tThreshold* const t, Lfloat in)
{
    if (in >= t->highThresh)
    {
    	t->currentValue = 1;
    }
    else if (in <= t->lowThresh)
	{
    	t->currentValue = 0;
	}

    return t->currentValue;
}

void tThreshold_setLow(tThreshold* const t, Lfloat low)
{
    t->lowThresh = low;
}

void tThreshold_setHigh(tThreshold* const t, Lfloat high)
{
    t->highThresh = high;
}
