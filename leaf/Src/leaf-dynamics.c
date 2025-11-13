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
void tCompressor_create(tMempool** const mp, tCompressor** const comp)
{
    ALLOC_FROM_POOL(tCompressor, comp, mp);
}

void tCompressor_init(LEAF* const leaf, tCompressor* const c)
{

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

void tCompressor_free (tCompressor** const comp)
{
    tCompressor* c = *comp;
    
    mpool_free((char*)c, c->mempool);
}

float tCompressor_tick(tCompressor* const c, float in)
{
    float slope, overshoot;
    
    float in_db = LEAF_clip(-90.0f, fasteratodb(fastabsf(in)), 0.0f);
    float out_db = 0.0f;
    
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
        float squareit = (overshoot + c->W);
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
    float attenuation = fasterdbtoa(c->M - c->y_T[0]);
    return attenuation * in;
}

//requires tables to be set with set function
//more efficient without soft knee calculation
float tCompressor_tickWithTable(tCompressor* const c, float in)
{
    float slope, overshoot;

    in = fastabsf(in);
    int inAmpIndex = LEAF_clip (0, (in * c->atodbScalar) - c->atodbOffset, c->atodbTableSizeMinus1);
    float in_db = c->atodbTable[inAmpIndex];
    float out_db = 0.0f;

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
        float squareit = (overshoot + c->W);
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
    float attenuationDb = c->M - c->y_T[0];
   // tempdbToA = dbtoa(attenuationDb);
    int attenuationDbIndex = LEAF_clip (0, (attenuationDb * c->dbtoaScalar) - c->dbtoaOffset, c->dbtoaTableSizeMinus1);
    float attenuation = c->dbtoaTable[attenuationDbIndex];
    return attenuation * in;
}

//requires tables to be set with set function
float tCompressor_tickWithTableHardKnee(tCompressor* const c, float in)
{
    float slope, overshoot;

    in = fastabsf(in);
    int inAmpIndex = LEAF_clip (0, (in * c->atodbScalar) - c->atodbOffset, c->atodbTableSizeMinus1);
    float in_db = c->atodbTable[inAmpIndex];
    float out_db = 0.0f;

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
    float attenuationDb = c->M - c->y_T[0];
   // tempdbToA = dbtoa(attenuationDb);
    int attenuationDbIndex = LEAF_clip (0, (attenuationDb * c->dbtoaScalar) - c->dbtoaOffset, c->dbtoaTableSizeMinus1);
    float attenuation = c->dbtoaTable[attenuationDbIndex];
    return attenuation * in;
}

void tCompressor_setTables(tCompressor* const c, float* atodb, float* dbtoa, float atodbMinIn, float atodbMaxIn, float dbtoaMinIn, float dbtoaMaxIn, int atodbTableSize, int dbtoaTableSize)
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
void tCompressor_setParams(tCompressor* const c, float thresh, float ratio, float knee, float makeup, float attack, float release)
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

void tCompressor_setSampleRate(tCompressor* const c, float sampleRate)
{
    c->sampleRate = sampleRate;
}
/* Feedback Leveler */

void tFeedbackLeveler_create(tMempool** const mp, tFeedbackLeveler** const fb)
{
    ALLOC_FROM_POOL(tFeedbackLeveler, fb, mp);
}

void tFeedbackLeveler_init(LEAF* const leaf, tFeedbackLeveler* const p, float targetLevel, float factor, float strength, int mode)
{

p->curr=0.0f;
    p->targetLevel=targetLevel;
    tPowerFollower_init(leaf,&p->pwrFlw, factor);
    p->mode=mode;
    p->strength=strength;

}

void tFeedbackLeveler_free (tFeedbackLeveler** const fb)
{
    tFeedbackLeveler* p = *fb;
    
    mpool_free((char*)p, p->mempool);
    p = NULL;
}

void     tFeedbackLeveler_setStrength(tFeedbackLeveler* const p, float strength)
{    // strength is how strongly level diff is affecting the amp ratio
    // try 0.125 for a start
    p->strength=strength;
}

void     tFeedbackLeveler_setFactor(tFeedbackLeveler* const p, float factor)
{
    tPowerFollower_setFactor(&p->pwrFlw,factor);
}

void     tFeedbackLeveler_setMode(tFeedbackLeveler* const p, int mode)
{    // 0 for decaying with upwards lev limiting, 1 for constrained absolute level (also downwards limiting)
    p->mode=mode;
}

float   tFeedbackLeveler_tick(tFeedbackLeveler* const p, float input)
{
    float levdiff=(tPowerFollower_tick(&p->pwrFlw, input)-p->targetLevel);
    if (p->mode==0 && levdiff<0.0f) levdiff=0.0f;
    p->curr=input*(1.0f-p->strength*levdiff);
    return p->curr;
}

float   tFeedbackLeveler_sample(tFeedbackLeveler* const p)
{
    return p->curr;
}


void     tFeedbackLeveler_setTargetLevel   (tFeedbackLeveler* const p, float TargetLevel)
{
    p->targetLevel=TargetLevel;
}


void tThreshold_create(tMempool** const mp, tThreshold** const th)
{
    ALLOC_FROM_POOL(tThreshold, th, mp);
}

void tThreshold_init(LEAF* const leaf, tThreshold* const t, float low, float high)
{

t->highThresh = high;
    t->lowThresh = low;

    t->currentValue = 0;

}

void tThreshold_free (tThreshold** const th)
{
    tThreshold* t = *th;

    mpool_free((char*)t, t->mempool);
}

int tThreshold_tick(tThreshold* const t, float in)
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

void tThreshold_setLow(tThreshold* const t, float low)
{
    t->lowThresh = low;
}

void tThreshold_setHigh(tThreshold* const t, float high)
{
    t->highThresh = high;
}
