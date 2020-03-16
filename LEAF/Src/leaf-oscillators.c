/*==============================================================================
 leaf-oscillators.c
 Created: 20 Jan 2017 12:00:58pm
 Author:  Michael R Mulshine
 ==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-oscillators.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-oscillators.h"
#include "../leaf.h"

#endif

// Cycle
void    tCycle_init(tCycle* const cy)
{
    tCycle_initToPool(cy, &leaf.mempool);
}

void    tCycle_free(tCycle* const cy)
{
    tCycle_freeFromPool(cy, &leaf.mempool);
}

void    tCycle_initToPool   (tCycle* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tCycle* c = *cy = (_tCycle*) mpool_alloc(sizeof(_tCycle), m);
    
    c->inc      =  0.0f;
    c->phase    =  0.0f;

}

void    tCycle_freeFromPool (tCycle* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tCycle* c = *cy;
    
    mpool_free(c, m);
}

void     tCycle_setFreq(tCycle* const cy, float freq)
{
    _tCycle* c = *cy;
    
    if (freq < 0.0f) c->freq = 0.0f;
    else if (freq > 20480.0f) c->freq = 20480.0f;
    else c->freq  = freq;

    c->inc = freq * leaf.invSampleRate;
}

//need to check bounds and wrap table properly to allow through-zero FM
float   tCycle_tick(tCycle* const cy)
{
    _tCycle* c = *cy;
    float temp;;
    int intPart;;
    float fracPart;
    float samp0;
    float samp1;
    
    // Phasor increment
    c->phase += c->inc;
    while (c->phase >= 1.0f) c->phase -= 1.0f;
    while (c->phase <= 0.0f) c->phase += 1.0f;

    // Wavetable synthesis

	temp = SINE_TABLE_SIZE * c->phase;
	intPart = (int)temp;
	fracPart = temp - (float)intPart;
	samp0 = __leaf_table_sinewave[intPart];
	if (++intPart >= SINE_TABLE_SIZE) intPart = 0;
	samp1 = __leaf_table_sinewave[intPart];

    return (samp0 + (samp1 - samp0) * fracPart);

}

void     tCycleSampleRateChanged (tCycle* const cy)
{
    _tCycle* c = *cy;
    
    c->inc = c->freq * leaf.invSampleRate;
}

//========================================================================
/* Triangle */
void   tTriangle_init(tTriangle* const cy)
{
    tTriangle_initToPool(cy, &leaf.mempool);
}

void   tTriangle_free(tTriangle* const cy)
{
    tTriangle_freeFromPool(cy, &leaf.mempool);
}

void    tTriangle_initToPool    (tTriangle* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTriangle* c = *cy = (_tTriangle*) mpool_alloc(sizeof(_tTriangle), m);
    
    c->inc      =  0.0f;
    c->phase    =  0.0f;
    tTriangle_setFreq(cy, 220);
}

void    tTriangle_freeFromPool  (tTriangle* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTriangle* c = *cy;
    
    mpool_free(c, m);
}

void tTriangle_setFreq(tTriangle* const cy, float freq)
{
    _tTriangle* c = *cy;
    
    if (freq < 0.0f) c->freq = 0.0f;
    else if (freq > 20480.0f) c->freq = 20480.0f;
    else c->freq  = freq;
    
    c->inc = c->freq * leaf.invSampleRate;
    
    c->w = c->freq * INV_20;
    for (c->oct = 0; c->w > 2.0f; c->oct++)
    {
        c->w = 0.5f * c->w;
    }
    c->w = 2.0f - c->w;
}


float   tTriangle_tick(tTriangle* const cy)
{
    _tTriangle* c = *cy;
    
    // Phasor increment
    c->phase += c->inc;
    while (c->phase >= 1.0f) c->phase -= 1.0f;
    
    float out = 0.0f;
    
    int idx = (int)(c->phase * TRI_TABLE_SIZE);
    
    // Wavetable synthesis
    out = __leaf_table_triangle[c->oct+1][idx] +
         (__leaf_table_triangle[c->oct][idx] - __leaf_table_triangle[c->oct+1][idx]) * c->w;
    
    return out;
}

void     tTriangleSampleRateChanged (tTriangle* const cy)
{
    _tTriangle* c = *cy;
    
    c->inc = c->freq * leaf.invSampleRate;
}

//========================================================================
/* Square */
void   tSquare_init(tSquare* const cy)
{
    tSquare_initToPool(cy, &leaf.mempool);
}

void   tSquare_free(tSquare* const cy)
{
    tSquare_freeFromPool(cy, &leaf.mempool);
}

void    tSquare_initToPool  (tSquare* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSquare* c = *cy = (_tSquare*) mpool_alloc(sizeof(_tSquare), m);
    
    c->inc      =  0.0f;
    c->phase    =  0.0f;
    tSquare_setFreq(cy, 220);
}

void    tSquare_freeFromPool(tSquare* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSquare* c = *cy;
    
    mpool_free(c, m);
}

void    tSquare_setFreq(tSquare* const cy, float freq)
{
    _tSquare* c = *cy;
    
    if (freq < 0.0f) c->freq = 0.0f;
    else if (freq > 20480.0f) c->freq = 20480.0f;
    else c->freq  = freq;
    
    c->inc = c->freq * leaf.invSampleRate;
    
    c->w = c->freq * INV_20;
    for (c->oct = 0; c->w > 2.0f; c->oct++)
    {
        c->w = 0.5f * c->w;
    }
    c->w = 2.0f - c->w;
}

float   tSquare_tick(tSquare* const cy)
{
    _tSquare* c = *cy;
    
    // Phasor increment
    c->phase += c->inc;
    while (c->phase >= 1.0f) c->phase -= 1.0f;
    
    float out = 0.0f;
    
    int idx = (int)(c->phase * TRI_TABLE_SIZE);
    
    // Wavetable synthesis
    out = __leaf_table_squarewave[c->oct+1][idx] +
         (__leaf_table_squarewave[c->oct][idx] - __leaf_table_squarewave[c->oct+1][idx]) * c->w;
    
    return out;
}

void     tSquareSampleRateChanged (tSquare* const cy)
{
    _tSquare* c = *cy;
    
    c->inc = c->freq * leaf.invSampleRate;
}

//=====================================================================
// Sawtooth
void    tSawtooth_init(tSawtooth* const cy)
{
    tSawtooth_initToPool(cy, &leaf.mempool);
}

void    tSawtooth_free(tSawtooth* const cy)
{
    tSawtooth_freeFromPool(cy, &leaf.mempool);
}

void    tSawtooth_initToPool    (tSawtooth* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSawtooth* c = *cy = (_tSawtooth*) mpool_alloc(sizeof(_tSawtooth), m);
    
    c->inc      = 0.0f;
    c->phase    = 0.0f;
    tSawtooth_setFreq(cy, 220);
}

void    tSawtooth_freeFromPool  (tSawtooth* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSawtooth* c = *cy;
    
    mpool_free(c, m);
}

void    tSawtooth_setFreq(tSawtooth* const cy, float freq)
{
    _tSawtooth* c = *cy;
    
    if (freq < 0.0f) c->freq = 0.0f;
    else if (freq > 20480.0f) c->freq = 20480.0f;
    else c->freq  = freq;
    
    c->inc = c->freq * leaf.invSampleRate;
    
    c->w = c->freq * INV_20;
    for (c->oct = 0; c->w > 2.0f; c->oct++)
    {
        c->w = 0.5f * c->w;
    }
    c->w = 2.0f - c->w;
}

float   tSawtooth_tick(tSawtooth* const cy)
{
    _tSawtooth* c = *cy;
    
    // Phasor increment
    c->phase += c->inc;
    while (c->phase >= 1.0f) c->phase -= 1.0f;
    
    float out = 0.0f;
    
    int idx = (int)(c->phase * TRI_TABLE_SIZE);
    
    // Wavetable synthesis
    out = __leaf_table_sawtooth[c->oct+1][idx] +
         (__leaf_table_sawtooth[c->oct][idx] - __leaf_table_sawtooth[c->oct+1][idx]) * c->w;
    
    return out;
}

void     tSawtoothSampleRateChanged (tSawtooth* const cy)
{
    _tSawtooth* c = *cy;
    
    c->inc = c->freq * leaf.invSampleRate;
}

//========================================================================
/* Phasor */
void     tPhasorSampleRateChanged (tPhasor* const ph)
{
    _tPhasor* p = *ph;
    
    p->inc = p->freq * leaf.invSampleRate;
};

void    tPhasor_init(tPhasor* const ph)
{
    tPhasor_initToPool(ph, &leaf.mempool);
}

void    tPhasor_free(tPhasor* const ph)
{
    tPhasor_freeFromPool(ph, &leaf.mempool);
}

void    tPhasor_initToPool  (tPhasor* const ph, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPhasor* p = *ph = (_tPhasor*) mpool_alloc(sizeof(_tPhasor), m);
    
    p->phase = 0.0f;
    p->inc = 0.0f;
}

void    tPhasor_freeFromPool(tPhasor* const ph, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPhasor* p = *ph;
    
    mpool_free(p, m);
}

void    tPhasor_setFreq(tPhasor* const ph, float freq)
{
    _tPhasor* p = *ph;
    
    if (freq < 0.0f) p->freq = 0.0f;
    else if (freq > 20480.0f) p->freq = 20480.0f;
    else p->freq  = freq;
    
    p->inc = freq * leaf.invSampleRate;
}

float   tPhasor_tick(tPhasor* const ph)
{
    _tPhasor* p = *ph;
    
    p->phase += p->inc;
    
    if (p->phase >= 1.0f) p->phase -= 1.0f;
    
    return p->phase;
}

/* Noise */
void    tNoise_init(tNoise* const ns, NoiseType type)
{
    tNoise_initToPool(ns, type, &leaf.mempool);
}

void    tNoise_free(tNoise* const ns)
{
    tNoise_freeFromPool(ns, &leaf.mempool);
}

void    tNoise_initToPool   (tNoise* const ns, NoiseType type, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNoise* n = *ns = (_tNoise*) mpool_alloc(sizeof(_tNoise), m);
    
    n->type = type;
    n->rand = leaf.random;
}

void    tNoise_freeFromPool (tNoise* const ns, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNoise* n = *ns;
    
    mpool_free(n, m);
}

float   tNoise_tick(tNoise* const ns)
{
    _tNoise* n = *ns;
    
    float rand = (n->rand() * 2.0f) - 1.0f;
    
    if (n->type == PinkNoise)
    {
        float tmp;
        n->pinkb0 = 0.99765f * n->pinkb0 + rand * 0.0990460f;
        n->pinkb1 = 0.96300f * n->pinkb1 + rand * 0.2965164f;
        n->pinkb2 = 0.57000f * n->pinkb2 + rand * 1.0526913f;
        tmp = n->pinkb0 + n->pinkb1 + n->pinkb2 + rand * 0.1848f;
        return (tmp * 0.05f);
    }
    else // WhiteNoise
    {
        return rand;
    }
}

//=================================================================================
/* Neuron */

void     tNeuronSampleRateChanged(tNeuron* nr)
{
    
}

void    tNeuron_init(tNeuron* const nr)
{
    tNeuron_initToPool(nr, &leaf.mempool);
}

void    tNeuron_free(tNeuron* const nr)
{
    tNeuron_freeFromPool(nr, &leaf.mempool);
}

void    tNeuron_initToPool  (tNeuron* const nr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNeuron* n = *nr = (_tNeuron*) mpool_alloc(sizeof(_tNeuron), m);
    
    tPoleZero_initToPool(&n->f, mp);
    
    tPoleZero_setBlockZero(&n->f, 0.99f);
    
    n->timeStep = 1.0f / 50.0f;
    
    n->current = 0.0f; // 100.0f for sound
    n->voltage = 0.0f;
    
    n->mode = NeuronNormal;
    
    n->P[0] = 0.0f;
    n->P[1] = 0.0f;
    n->P[2] = 1.0f;
    
    n->V[0] = -12.0f;
    n->V[1] = 115.0f;
    n->V[2] = 10.613f;
    
    n->gK = 36.0f;
    n->gN = 120.0f;
    n->gL = 0.3f;
    n->C = 1.0f;
    
    n->rate[2] = n->gL/n->C;
}

void    tNeuron_freeFromPool(tNeuron* const nr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNeuron* n = *nr;
    
    tPoleZero_free(&n->f);
    mpool_free(n, m);
}

void   tNeuron_reset(tNeuron* const nr)
{
    _tNeuron* n = *nr;
    
    tPoleZero_setBlockZero(&n->f, 0.99f);
    
    n->timeStep = 1.0f / 50.0f;
    
    n->current = 0.0f; // 100.0f for sound
    n->voltage = 0.0f;
    
    n->mode = NeuronNormal;
    
    n->P[0] = 0.0f;
    n->P[1] = 0.0f;
    n->P[2] = 1.0f;
    
    n->V[0] = -12.0f;
    n->V[1] = 115.0f;
    n->V[2] = 10.613f;
    
    n->gK = 36.0f;
    n->gN = 120.0f;
    n->gL = 0.3f;
    n->C = 1.0f;
    
    n->rate[2] = n->gL/n->C;
}

void        tNeuron_setV1(tNeuron* const nr, float V1)
{
    _tNeuron* n = *nr;
    n->V[0] = V1;
}


void        tNeuron_setV2(tNeuron* const nr, float V2)
{
    _tNeuron* n = *nr;
    n->V[1] = V2;
}

void        tNeuron_setV3(tNeuron* const nr, float V3)
{
    _tNeuron* n = *nr;
    n->V[2] = V3;
}

void        tNeuron_setTimeStep(tNeuron* const nr, float timeStep)
{
    _tNeuron* n = *nr;
    n->timeStep = timeStep;
}

void        tNeuron_setK(tNeuron* const nr, float K)
{
    _tNeuron* n = *nr;
    n->gK = K;
}

void        tNeuron_setL(tNeuron* const nr, float L)
{
    _tNeuron* n = *nr;
    n->gL = L;
    n->rate[2] = n->gL/n->C;
}

void        tNeuron_setN(tNeuron* const nr, float N)
{
    _tNeuron* n = *nr;
    n->gN = N;
}

void        tNeuron_setC(tNeuron* const nr, float C)
{
    _tNeuron* n = *nr;
    n->C = C;
    n->rate[2] = n->gL/n->C;
}

float   tNeuron_tick(tNeuron* const nr)
{
    _tNeuron* n = *nr;
    
    float output = 0.0f;
    float voltage = n->voltage;
    
    n->alpha[0] = (0.01f * (10.0f - voltage)) / (expf((10.0f - voltage)/10.0f) - 1.0f);
    n->alpha[1] = (0.1f * (25.0f-voltage)) / (expf((25.0f-voltage)/10.0f) - 1.0f);
    n->alpha[2] = (0.07f * expf((-1.0f * voltage)/20.0f));
    
    n->beta[0] = (0.125f * expf((-1.0f* voltage)/80.0f));
    n->beta[1] = (4.0f * expf((-1.0f * voltage)/18.0f));
    n->beta[2] = (1.0f / (expf((30.0f-voltage)/10.0f) + 1.0f));
    
    for (int i = 0; i < 3; i++)
    {
        n->P[i] = (n->alpha[i] * n->timeStep) + ((1.0f - ((n->alpha[i] + n->beta[i]) * n->timeStep)) * n->P[i]);
        
        if (n->P[i] > 1.0f)         n->P[i] = 0.0f;
        else if (n->P[i] < -1.0f)   n->P[i] = 0.0f;
    }
    // rate[0]= k ; rate[1] = Na ; rate[2] = l
    n->rate[0] = ((n->gK * powf(n->P[0], 4.0f)) / n->C);
    n->rate[1] = ((n->gN * powf(n->P[1], 3.0f) * n->P[2]) / n->C);
    
    //calculate the final membrane voltage based on the computed variables
    n->voltage = voltage +
    (n->timeStep * n->current / n->C) -
    (n->timeStep * ( n->rate[0] * (voltage - n->V[0]) + n->rate[1] * (voltage - n->V[1]) + n->rate[2] * (voltage - n->V[2])));
    
    if (n->mode == NeuronTanh)
    {
        n->voltage = 100.0f * tanhf(0.01f * n->voltage);
    }
    else if (n->mode == NeuronAaltoShaper)
    {
        float shapeVoltage = 0.01f * n->voltage;
        
        float w, c, xc, xc2, xc4;
        
        float sqrt8 = 2.82842712475f;
        
        float wscale = 1.30612244898f;
        float m_drive = 1.0f;
        
        xc = LEAF_clip(-sqrt8, shapeVoltage, sqrt8);
        
        xc2 = xc*xc;
        
        c = 0.5f * shapeVoltage * (3.0f - (xc2));
        
        xc4 = xc2 * xc2;
        
        w = (1.0f - xc2 * 0.25f + xc4 * 0.015625f) * wscale;
        
        shapeVoltage = w * (c + 0.05f * xc2) * (m_drive + 0.75f);
        
        n->voltage = 100.0f * shapeVoltage;
    }
    
    
    if (n->voltage > 100.0f)  n->voltage = 100.0f;
    else if (n->voltage < -100.) n->voltage = -100.0f;
    
    //(inputCurrent + (voltage - ((voltage * timeStep) / timeConstant)) + P[0] + P[1] + P[2]) => voltage;
    // now we should have a result
    //set the output voltage to the "step" ugen, which controls the DAC.
    output = n->voltage * 0.01f; // volts
    
    output = tPoleZero_tick(&n->f, output);
    
    return output;
    
}

void        tNeuron_setMode  (tNeuron* const nr, NeuronMode mode)
{
    _tNeuron* n = *nr;
    n->mode = mode;
}

void        tNeuron_setCurrent  (tNeuron* const nr, float current)
{
    _tNeuron* n = *nr;
    n->current = current;
}
