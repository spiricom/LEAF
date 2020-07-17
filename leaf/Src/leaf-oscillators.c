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

// WaveTable
void    tTable_init(tTable* const cy, float* waveTable, int size)
{
    tTable_initToPool(cy, waveTable, size, &leaf.mempool);
}

void    tTable_initToPool(tTable* const cy, float* waveTable, int size, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTable* c = *cy = (_tTable*)mpool_alloc(sizeof(_tTable), m);
    c->mempool = m;
    
    c->waveTable = waveTable;
    c->size = size;
    c->inc = 0.0f;
    c->phase = 0.0f;
}

void    tTable_free(tTable* const cy)
{
    _tTable* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

void     tTable_setFreq(tTable* const cy, float freq)
{
    _tTable* c = *cy;
    
    c->freq = freq;
    c->inc = freq * leaf.invSampleRate;
}

float   tTable_tick(tTable* const cy)
{
    _tTable* c = *cy;
    float temp;
    int intPart;
    float fracPart;
    float samp0;
    float samp1;
    
    // Phasor increment
    c->phase += c->inc;
    while (c->phase >= 1.0f) c->phase -= 1.0f;
    while (c->phase < 0.0f) c->phase += 1.0f;
    
    // Wavetable synthesis
    
    temp = c->size * c->phase;
    intPart = (int)temp;
    fracPart = temp - (float)intPart;
    samp0 = c->waveTable[intPart];
    if (++intPart >= c->size) intPart = 0;
    samp1 = c->waveTable[intPart];
    
    return (samp0 + (samp1 - samp0) * fracPart);
    
}

void     tTableSampleRateChanged(tTable* const cy)
{
    _tTable* c = *cy;
    
    c->inc = c->freq * leaf.invSampleRate;
}

// Cycle
void    tCycle_init(tCycle* const cy)
{
    tCycle_initToPool(cy, &leaf.mempool);
}

void    tCycle_initToPool   (tCycle* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tCycle* c = *cy = (_tCycle*) mpool_alloc(sizeof(_tCycle), m);
    c->mempool = m;
    
    c->inc      =  0.0f;
    c->phase    =  0.0f;
}

void    tCycle_free (tCycle* const cy)
{
    _tCycle* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

void     tCycle_setFreq(tCycle* const cy, float freq)
{
    _tCycle* c = *cy;
    
    c->freq  = freq;

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
    while (c->phase < 0.0f) c->phase += 1.0f;

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

void    tTriangle_initToPool    (tTriangle* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTriangle* c = *cy = (_tTriangle*) mpool_alloc(sizeof(_tTriangle), m);
    c->mempool = m;
    
    c->inc      =  0.0f;
    c->phase    =  0.0f;
    tTriangle_setFreq(cy, 220);
}

void    tTriangle_free  (tTriangle* const cy)
{
    _tTriangle* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

void tTriangle_setFreq(tTriangle* const cy, float freq)
{
    _tTriangle* c = *cy;
    
    c->freq  = freq;
    
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
    while (c->phase < 0.0f) c->phase += 1.0f;
    
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

void    tSquare_initToPool  (tSquare* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSquare* c = *cy = (_tSquare*) mpool_alloc(sizeof(_tSquare), m);
    c->mempool = m;
    
    c->inc      =  0.0f;
    c->phase    =  0.0f;
    tSquare_setFreq(cy, 220);
}

void    tSquare_free (tSquare* const cy)
{
    _tSquare* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

void    tSquare_setFreq(tSquare* const cy, float freq)
{
    _tSquare* c = *cy;

    c->freq  = freq;
    
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
    while (c->phase < 0.0f) c->phase += 1.0f;
    
    float out = 0.0f;
    
    int idx = (int)(c->phase * SQR_TABLE_SIZE);
    
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

void    tSawtooth_initToPool    (tSawtooth* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSawtooth* c = *cy = (_tSawtooth*) mpool_alloc(sizeof(_tSawtooth), m);
    c->mempool = m;
    
    c->inc      = 0.0f;
    c->phase    = 0.0f;
    tSawtooth_setFreq(cy, 220);
}

void    tSawtooth_free (tSawtooth* const cy)
{
    _tSawtooth* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

void    tSawtooth_setFreq(tSawtooth* const cy, float freq)
{
    _tSawtooth* c = *cy;
    
    c->freq  = freq;
    
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
    while (c->phase < 0.0f) c->phase += 1.0f;
    
    float out = 0.0f;
    
    int idx = (int)(c->phase * SAW_TABLE_SIZE);
    
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

//==============================================================================

/* tTri: Anti-aliased Triangle waveform. */
void    tTri_init          (tTri* const osc)
{
    tTri_initToPool(osc, &leaf.mempool);
}

void    tTri_initToPool    (tTri* const osc, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTri* c = *osc = (_tTri*) mpool_alloc(sizeof(_tTri), m);
    c->mempool = m;

    c->inc      =  0.0f;
    c->phase    =  0.0f;
    c->skew     =  0.5f;
    c->lastOut  =  0.0f;
}

void    tTri_free (tTri* const cy)
{
    _tTri* c = *cy;
    
    mpool_free((char*)c, c->mempool);
}

float   tTri_tick          (tTri* const osc)
{
    _tTri* c = *osc;
    
    float out;
    float skew;
    
    if (c->phase < c->skew)
    {
        out = 1.0f;
        skew = (1.0f - c->skew) * 2.0f;
    }
    else
    {
        out = -1.0f;
        skew = c->skew * 2.0f;
    }
    
    out += LEAF_poly_blep(c->phase, c->inc);
    out -= LEAF_poly_blep(fmod(c->phase + (1.0f - c->skew), 1.0f), c->inc);
    
    out = (skew * c->inc * out) + ((1 - c->inc) * c->lastOut);
    c->lastOut = out;
    
    c->phase += c->inc;
    if (c->phase >= 1.0f)
    {
        c->phase -= 1.0f;
    }
    
    return out;
}

void    tTri_setFreq       (tTri* const osc, float freq)
{
    _tTri* c = *osc;
    
    c->freq  = freq;
    c->inc = freq * leaf.invSampleRate;
}

void    tTri_setSkew       (tTri* const osc, float skew)
{
    _tTri* c = *osc;
    c->skew = (skew + 1.0f) * 0.5f;
}


//==============================================================================

/* tPulse: Anti-aliased pulse waveform. */
void    tPulse_init        (tPulse* const osc)
{
    tPulse_initToPool(osc, &leaf.mempool);
}

void    tPulse_initToPool  (tPulse* const osc, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPulse* c = *osc = (_tPulse*) mpool_alloc(sizeof(_tPulse), m);
    c->mempool = m;
    
    c->inc      =  0.0f;
    c->phase    =  0.0f;
    c->width     =  0.5f;
}

void    tPulse_free (tPulse* const osc)
{
    _tPulse* c = *osc;
    
    mpool_free((char*)c, c->mempool);
}

float   tPulse_tick        (tPulse* const osc)
{
    _tPulse* c = *osc;
    
    float out;
    if (c->phase < c->width) out = 1.0f;
    else out = -1.0f;
    out += LEAF_poly_blep(c->phase, c->inc);
    out -= LEAF_poly_blep(fmod(c->phase + (1.0f - c->width), 1.0f), c->inc);
    
    c->phase += c->inc;
    if (c->phase >= 1.0f)
    {
        c->phase -= 1.0f;
    }
    
    return out;
}

void    tPulse_setFreq     (tPulse* const osc, float freq)
{
    _tPulse* c = *osc;
    
    c->freq  = freq;
    c->inc = freq * leaf.invSampleRate;
}

void    tPulse_setWidth    (tPulse* const osc, float width)
{
    _tPulse* c = *osc;
    c->width = width;
}


//==============================================================================

/* tSawtooth: Anti-aliased Sawtooth waveform. */
void    tSaw_init          (tSaw* const osc)
{
    tSaw_initToPool(osc, &leaf.mempool);
}

void    tSaw_initToPool    (tSaw* const osc, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSaw* c = *osc = (_tSaw*) mpool_alloc(sizeof(_tSaw), m);
    c->mempool = m;
    
    c->inc      =  0.0f;
    c->phase    =  0.0f;
}

void    tSaw_free  (tSaw* const osc)
{
    _tSaw* c = *osc;
    
    mpool_free((char*)c, c->mempool);
}

float   tSaw_tick          (tSaw* const osc)
{
    _tSaw* c = *osc;
    
    float out = (c->phase * 2.0f) - 1.0f;
    out -= LEAF_poly_blep(c->phase, c->inc);
    
    c->phase += c->inc;
    if (c->phase >= 1.0f)
    {
        c->phase -= 1.0f;
    }
    
    return out;
}

void    tSaw_setFreq       (tSaw* const osc, float freq)
{
    _tSaw* c = *osc;
    
    c->freq  = freq;
    
    c->inc = freq * leaf.invSampleRate;
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

void    tPhasor_initToPool  (tPhasor* const ph, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPhasor* p = *ph = (_tPhasor*) mpool_alloc(sizeof(_tPhasor), m);
    p->mempool = m;
    
    p->phase = 0.0f;
    p->inc = 0.0f;
    p->phaseDidReset = 0;
}

void    tPhasor_free (tPhasor* const ph)
{
    _tPhasor* p = *ph;
    
    mpool_free((char*)p, p->mempool);
}

void    tPhasor_setFreq(tPhasor* const ph, float freq)
{
    _tPhasor* p = *ph;

    p->freq  = freq;
    
    p->inc = freq * leaf.invSampleRate;
}

float   tPhasor_tick(tPhasor* const ph)
{
    _tPhasor* p = *ph;
    
    p->phase += p->inc;
    
    
    p->phaseDidReset = 0;
    if (p->phase >= 1.0f)
    {
        p->phaseDidReset = 1;
        p->phase -= 1.0f;
    }
    
    return p->phase;
}

/* Noise */
void    tNoise_init(tNoise* const ns, NoiseType type)
{
    tNoise_initToPool(ns, type, &leaf.mempool);
}

void    tNoise_initToPool   (tNoise* const ns, NoiseType type, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNoise* n = *ns = (_tNoise*) mpool_alloc(sizeof(_tNoise), m);
    n->mempool = m;
    
    n->type = type;
    n->rand = leaf.random;
}

void    tNoise_free (tNoise* const ns)
{
    _tNoise* n = *ns;
    
    mpool_free((char*)n, n->mempool);
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

void    tNeuron_initToPool  (tNeuron* const nr, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNeuron* n = *nr = (_tNeuron*) mpool_alloc(sizeof(_tNeuron), m);
    n->mempool = m;
    
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

void    tNeuron_free (tNeuron* const nr)
{
    _tNeuron* n = *nr;
    
    tPoleZero_free(&n->f);
    mpool_free((char*)n, n->mempool);
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


/// MINBLEPS

// https://github.com/MrBlueXav/Dekrispator_v2 blepvco.c


static void place_step_dd(float *buffer, int index, float phase, float w, float scale)
{
    float r;
    int i;
    
    r = MINBLEP_PHASES * phase / w;
    i = rintf(r - 0.5f);
    r -= (float)i;
    i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
    /* this would be better than the above, but more expensive:
     *  while (i < 0) {
     *    i += MINBLEP_PHASES;
     *    index++;
     *  }
     */
    
    while (i < MINBLEP_PHASES * STEP_DD_PULSE_LENGTH) {
        buffer[index] += scale * (step_dd_table[i].value + r * step_dd_table[i].delta);
        i += MINBLEP_PHASES;
        index++;
    }
}
//----------------------------------------------------------------------------------------------------------

static void place_slope_dd(float *buffer, int index, float phase, float w, float slope_delta)
{
    float r;
    int i;
    
    r = MINBLEP_PHASES * phase / w;
    i = rintf(r - 0.5f);
    r -= (float)i;
    i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */
    
    slope_delta *= w;
    
    while (i < MINBLEP_PHASES * SLOPE_DD_PULSE_LENGTH) {
        buffer[index] += slope_delta * (slope_dd_table[i] + r * (slope_dd_table[i + 1] - slope_dd_table[i]));
        i += MINBLEP_PHASES;
        index++;
    }
}
//----------------------------------------------------------------------------------------------------------

void tMBPulse_init(tMBPulse* const osc)
{
    tMBPulse_initToPool(osc, &leaf.mempool);
}
                          
void tMBPulse_initToPool(tMBPulse* const osc, tMempool* const pool)
{
    _tMempool* m = *pool;
    _tMBPulse* c = *osc = (_tMBPulse*) mpool_alloc(sizeof(_tMBPulse), m);
    c->mempool = m;
    
    c->_init = true;
    c->amp = 1.0f;
    c->freq = 440.f;
    c->syncin = 0.0f;
    c->waveform = 0.0f;
    c->_z = 0.0f;
    c->_j = 0;
    memset (c->_f, 0, (FILLEN + STEP_DD_PULSE_LENGTH) * sizeof (float));
}

void tMBPulse_free(tMBPulse* const osc)
{
    _tMBPulse* c = *osc;
    mpool_free((char*)c, c->mempool);
}

float tMBPulse_tick(tMBPulse* const osc)
{
    _tMBPulse* c = *osc;
    
    int    j, k;
    float  freq, syncin;
    float  a, b, db, p, t, w, dw, x, z;
    
    syncin  = c->syncin;
    freq = c->freq;
    p = c->_p;  /* phase [0, 1) */
    w = c->_w;  /* phase increment */
    b = c->_b;  /* duty cycle (0, 1) */
    x = c->_x;  /* temporary output variable */
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */
    k = c->_k;  /* output state, 0 = high (0.5f), 1 = low (-0.5f) */
    //
    if (c->_init) {
        p = 0.0f;
        
        w = freq / leaf.sampleRate;
        if (w < 1e-5) w = 1e-5;
        if (w > 0.5) w = 0.5;
        b = 0.5 * (1.0 + c->waveform );
        if (b < w) b = w;
        if (b > 1.0f - w) b = 1.0f - w;
        /* for variable-width rectangular wave, we could do DC compensation with:
         *     x = 1.0f - b;
         * but that doesn't work well with highly modulated hard sync.  Instead,
         * we keep things in the range [-0.5f, 0.5f]. */
        x = 0.5f;
        /* if we valued alias-free startup over low startup time, we could do:
         *   p -= w;
         *   place_step_dd(_f, j, 0.0f, w, 0.5f); */
        k = 0;
        c->_init = false;
    }
    //
    //    a = 0.2 + 0.8 * vco->_port [FILT];
    a = 0.5f; // when a = 1, LPfilter is disabled
    
    t = freq / leaf.sampleRate;
    if (t < 1e-5) t = 1e-5;
    if (t > 0.5) t = 0.5;
    dw = (t - w) ;
    t = 0.5 * (1.0 + c->waveform );
    if (t < w) t = w;
    if (t > 1.0f - w) t = 1.0f - w;
    db = (t - b) ;
    
    w += dw;
    b += db;
    p += w;
    
    if (syncin >= 1e-20f) {  /* sync to master */
        //
        float eof_offset = (syncin - 1e-20f) * w;
        float p_at_reset = p - eof_offset;
        p = eof_offset;
        
        /* place any DDs that may have occurred in subsample before reset */
        if (!k) {
            if (p_at_reset >= b) {
                place_step_dd(c->_f, j, p_at_reset - b + eof_offset, w, -1.0f);
                k = 1;
                x = -0.5f;
            }
            if (p_at_reset >= 1.0f) {
                p_at_reset -= 1.0f;
                place_step_dd(c->_f, j, p_at_reset + eof_offset, w, 1.0f);
                k = 0;
                x = 0.5f;
            }
        } else {
            if (p_at_reset >= 1.0f) {
                p_at_reset -= 1.0f;
                place_step_dd(c->_f, j, p_at_reset + eof_offset, w, 1.0f);
                k = 0;
                x = 0.5f;
            }
            if (!k && p_at_reset >= b) {
                place_step_dd(c->_f, j, p_at_reset - b + eof_offset, w, -1.0f);
                k = 1;
                x = -0.5f;
            }
        }
        
        /* now place reset DD */
        if (k) {
            place_step_dd(c->_f, j, p, w, 1.0f);
            k = 0;
            x = 0.5f;
        }
        if (p >= b) {
            place_step_dd(c->_f, j, p - b, w, -1.0f);
            k = 1;
            x = -0.5f;
        }
        
        c->syncout = syncin;  /* best we can do is pass on upstream sync */
        
    } else if (!k) {  /* normal operation, signal currently high */
        
        if (p >= b) {
            place_step_dd(c->_f, j, p - b, w, -1.0f);
            k = 1;
            x = -0.5f;
        }
        if (p >= 1.0f) {
            p -= 1.0f;
            c->syncout = p / w + 1e-20f;
            place_step_dd(c->_f, j, p, w, 1.0f);
            k = 0;
            x = 0.5f;
        } else {
            c->syncout = 0.0f;
        }
        
    } else {  /* normal operation, signal currently low */
        
        if (p >= 1.0f) {
            p -= 1.0f;
            c->syncout = p / w + 1e-20f;
            place_step_dd(c->_f, j, p, w, 1.0f);
            k = 0;
            x = 0.5f;
        } else {
            c->syncout = 0.0f;
        }
        if (!k && p >= b) {
            place_step_dd(c->_f, j, p - b, w, -1.0f);
            k = 1;
            x = -0.5f;
        }
    }
    c->_f[j + DD_SAMPLE_DELAY] += x;
    
    z += a * (c->_f[j] - z);
    c->out = c->amp * z;
    
    if (++j == FILLEN)
    {
        j = 0;
        memcpy (c->_f, c->_f + FILLEN, STEP_DD_PULSE_LENGTH * sizeof (float));
        memset (c->_f + STEP_DD_PULSE_LENGTH, 0,  FILLEN * sizeof (float));
    }
    
    c->_p = p;
    c->_w = w;
    c->_b = b;
    c->_x = x;
    c->_z = z;
    c->_j = j;
    c->_k = k;
    
    return c->out;
}

void tMBPulse_setFreq(tMBPulse* const osc, float f)
{
    _tMBPulse* c = *osc;
    c->freq = f;
}

void tMBPulse_setWidth(tMBPulse* const osc, float w)
{
    _tMBPulse* c = *osc;
    c->waveform = w;
}

void tMBPulse_syncIn(tMBPulse* const osc, float sync)
{
    _tMBPulse* c = *osc;
    c->syncin = sync;
}

float tMBPulse_syncOut(tMBPulse* const osc)
{
    _tMBPulse* c = *osc;
    return c->syncout;
}

//----------------------------------------------------------------------------------------------------------

void tMBTriangle_init(tMBTriangle* const osc)
{
    tMBTriangle_initToPool(osc, &leaf.mempool);
}

void tMBTriangle_initToPool(tMBTriangle* const osc, tMempool* const pool)
{
    _tMempool* m = *pool;
    _tMBTriangle* c = *osc = (_tMBTriangle*) mpool_alloc(sizeof(_tMBTriangle), m);
    c->mempool = m;
    
    c->amp = 1.0f;
    c->freq = 440.f;
    c->syncin = 0.0f;
    c->waveform = 0.0f;
    c->_init = true;
    c->_z = 0.0f;
    c->_j = 0;
    memset (c->_f, 0, (FILLEN + STEP_DD_PULSE_LENGTH) * sizeof (float));
}

void tMBTriangle_free(tMBTriangle* const osc)
{
    _tMBTriangle* c = *osc;
    mpool_free((char*)c, c->mempool);
}

float tMBTriangle_tick(tMBTriangle* const osc)
{
    _tMBTriangle* c = *osc;
    
    int    j, k;
    float  freq, syncin;
    float  a, b, b1, db, p, t, w, dw, x, z;
    
    syncin  = c->syncin;
    freq = c->freq;
    p = c->_p;  /* phase [0, 1) */
    w = c->_w;  /* phase increment */
    b = c->_b;  /* duty cycle (0, 1) */
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */
    k = c->_k;  /* output state, 0 = positive slope, 1 = negative slope */
    
    if (c->_init) {
        //        w = (exp2ap (freq[1] + vco->_port[OCTN] + vco->_port[TUNE] + expm[1] * vco->_port[EXPG] + 8.03136)
        //                + 1e3 * linm[1] * vco->_port[LING]) / SAMPLERATE;
        w = freq / leaf.sampleRate;
        if (w < 1e-5) w = 1e-5;
        if (w > 0.5) w = 0.5;
        b = 0.5 * (1.0 + c->waveform);
        if (b < w) b = w;
        if (b > 1.0f - w) b = 1.0f - w;
        p = 0.5f * b;
        /* if we valued alias-free startup over low startup time, we could do:
         *   p -= w;
         *   place_slope_dd(_f, j, 0.0f, w, 1.0f / b); */
        k = 0;
        c->_init = false;
    }
    
    //    a = 0.2 + 0.8 * vco->_port [FILT];
    a = 0.5f; // when a = 1, LPfilter is disabled
    
    t = freq / leaf.sampleRate;
    if (t < 1e-5) t = 1e-5;
    if (t > 0.5) t = 0.5;
    dw = (t - w) ;
    t = 0.5 * (1.0 + c->waveform );
    if (t < w) t = w;
    if (t > 1.0f - w) t = 1.0f - w;
    db = (t - b) ;
    
    w += dw;
    b += db;
    b1 = 1.0f - b;
    p += w;
    
    if (syncin >= 1e-20f) {  /* sync to master */
        
        float eof_offset = (syncin - 1e-20f) * w;
        float p_at_reset = p - eof_offset;
        p = eof_offset;
        //
        /* place any DDs that may have occurred in subsample before reset */
        if (!k) {
            x = -0.5f + p_at_reset / b;
            if (p_at_reset >= b) {
                x = 0.5f - (p_at_reset - b) / b1;
                place_slope_dd(c->_f, j, p_at_reset - b + eof_offset, w, -1.0f / b1 - 1.0f / b);
                k = 1;
            }
            if (p_at_reset >= 1.0f) {
                p_at_reset -= 1.0f;
                x = -0.5f + p_at_reset / b;
                place_slope_dd(c->_f, j, p_at_reset + eof_offset, w, 1.0f / b + 1.0f / b1);
                k = 0;
            }
        } else {
            x = 0.5f - (p_at_reset - b) / b1;
            if (p_at_reset >= 1.0f) {
                p_at_reset -= 1.0f;
                x = -0.5f + p_at_reset / b;
                place_slope_dd(c->_f, j, p_at_reset + eof_offset, w, 1.0f / b + 1.0f / b1);
                k = 0;
            }
            if (!k && p_at_reset >= b) {
                x = 0.5f - (p_at_reset - b) / b1;
                place_slope_dd(c->_f, j, p_at_reset - b + eof_offset, w, -1.0f / b1 - 1.0f / b);
                k = 1;
            }
        }
        
        /* now place reset DDs */
        if (k)
            place_slope_dd(c->_f, j, p, w, 1.0f / b + 1.0f / b1);
        place_step_dd(c->_f, j, p, w, -0.5f - x);
        x = -0.5f + p / b;
        k = 0;
        if (p >= b) {
            x = 0.5f - (p - b) / b1;
            place_slope_dd(c->_f, j, p - b, w, -1.0f / b1 - 1.0f / b);
            k = 1;
        }
        c->syncout = syncin;  /* best we can do is pass on upstream sync */
        
    } else if (!k) {  /* normal operation, slope currently up */
        
        x = -0.5f + p / b;
        if (p >= b) {
            x = 0.5f - (p - b) / b1;
            place_slope_dd(c->_f, j, p - b, w, -1.0f / b1 - 1.0f / b);
            k = 1;
        }
        if (p >= 1.0f) {
            p -= 1.0f;
            c->syncout = p / w + 1e-20f;
            x = -0.5f + p / b;
            place_slope_dd(c->_f, j, p, w, 1.0f / b + 1.0f / b1);
            k = 0;
        } else {
            c->syncout = 0.0f;
        }
        
    } else {  /* normal operation, slope currently down */
        
        x = 0.5f - (p - b) / b1;
        if (p >= 1.0f) {
            p -= 1.0f;
            c->syncout = p / w + 1e-20f;
            x = -0.5f + p / b;
            place_slope_dd(c->_f, j, p, w, 1.0f / b + 1.0f / b1);
            k = 0;
        } else {
            c->syncout = 0.0f;
        }
        if (!k && p >= b) {
            x = 0.5f - (p - b) / b1;
            place_slope_dd(c->_f, j, p - b, w, -1.0f / b1 - 1.0f / b);
            k = 1;
        }
    }
    c->_f[j + DD_SAMPLE_DELAY] += x;
    
    z += a * (c->_f[j] - z);
    c->out = c->amp * z;
    
    if (++j == FILLEN)
    {
        j = 0;
        memcpy (c->_f, c->_f + FILLEN, STEP_DD_PULSE_LENGTH * sizeof (float));
        memset (c->_f + STEP_DD_PULSE_LENGTH, 0,  FILLEN * sizeof (float));
    }
    
    c->_p = p;
    c->_w = w;
    c->_b = b;
    c->_z = z;
    c->_j = j;
    c->_k = k;
    
    return c->out;
}

void tMBTriangle_setFreq(tMBTriangle* const osc, float f)
{
    _tMBTriangle* c = *osc;
    c->freq = f;
}

void tMBTriangle_setWidth(tMBTriangle* const osc, float w)
{
    _tMBTriangle* c = *osc;
    c->waveform = w;
}

void tMBTriangle_syncIn(tMBTriangle* const osc, float sync)
{
    _tMBTriangle* c = *osc;
    c->syncin = sync;
}

float tMBTriangle_syncOut(tMBTriangle* const osc)
{
    _tMBTriangle* c = *osc;
    return c->syncout;
}


//----------------------------------------------------------------------------------------------------------

void tMBSaw_init(tMBSaw* const osc)
{
    tMBSaw_initToPool(osc, &leaf.mempool);
}

void tMBSaw_initToPool(tMBSaw* const osc, tMempool* const pool)
{
    _tMempool* m = *pool;
    _tMBSaw* c = *osc = (_tMBSaw*) mpool_alloc(sizeof(_tMBSaw), m);
    c->mempool = m;
    
    c->_init = true;
    c->amp = 1.0f;
    c->freq = 440.f;
    c->syncin = 0.0f;
    c->_z = 0.0f;
    c->_j = 0;
    memset (c->_f, 0, (FILLEN + STEP_DD_PULSE_LENGTH) * sizeof (float));
}

void tMBSaw_free(tMBSaw* const osc)
{
    _tMBSaw* c = *osc;
    mpool_free((char*)c, c->mempool);
}

float tMBSaw_tick(tMBSaw* const osc)
{
    _tMBSaw* c = *osc;
    
    int    j;
    float  freq, syncin;
    float  a, p, t, w, dw, z;
    syncin  = c->syncin;
    freq = c->freq;
    
    p = c->_p;  /* phase [0, 1) */
    w = c->_w;  /* phase increment */
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */
    
    if (c->_init) {
        p = 0.5f;
        w = freq / leaf.sampleRate;
        if (w < 1e-5) w = 1e-5;
        if (w > 0.5) w = 0.5;
        /* if we valued alias-free startup over low startup time, we could do:
         *   p -= w;
         *   place_slope_dd(_f, j, 0.0f, w, -1.0f); */
        c->_init = false;
    }
    
    //a = 0.2 + 0.8 * vco->_port [FILT];
    a = 0.5f; // when a = 1, LPfilter is disabled
    
    t = freq / leaf.sampleRate;
    if (t < 1e-5) t = 1e-5;
    if (t > 0.5) t = 0.5;
    dw = (t - w); // n= 1
    w += dw;
    p += w;
    
    if (syncin >= 1e-20f) {  /* sync to master */
        
        float eof_offset = (syncin - 1e-20f) * w;
        float p_at_reset = p - eof_offset;
        p = eof_offset;
        
        /* place any DD that may have occurred in subsample before reset */
        if (p_at_reset >= 1.0f) {
            p_at_reset -= 1.0f;
            place_step_dd(c->_f, j, p_at_reset + eof_offset, w, 1.0f);
        }
        
        /* now place reset DD */
        place_step_dd(c->_f, j, p, w, p_at_reset);
        
        c->syncout = syncin;  /* best we can do is pass on upstream sync */
        
    } else if (p >= 1.0f) {  /* normal phase reset */
        
        p -= 1.0f;
        c->syncout = p / w + 1e-20f;
        place_step_dd(c->_f, j, p, w, 1.0f);
        
    } else {
        
        c->syncout = 0.0f;
    }
    c->_f[j + DD_SAMPLE_DELAY] += 0.5f - p;
    
    z += a * (c->_f[j] - z); // LP filtering
    c->out = c->amp * z;
    
    if (++j == FILLEN)
    {
        j = 0;
        memcpy (c->_f, c->_f + FILLEN, STEP_DD_PULSE_LENGTH * sizeof (float));
        memset (c->_f + STEP_DD_PULSE_LENGTH, 0,  FILLEN * sizeof (float));
    }
    
    c->_p = p;
    c->_w = w;
    c->_z = z;
    c->_j = j;
    
    return c->out;
}

void tMBSaw_setFreq(tMBSaw* const osc, float f)
{
    _tMBSaw* c = *osc;
    c->freq = f;
}

void tMBSaw_syncIn(tMBSaw* const osc, float sync)
{
    _tMBSaw* c = *osc;
    c->syncin = sync;
}

float tMBSaw_syncOut(tMBSaw* const osc)
{
    _tMBSaw* c = *osc;
    return c->syncout;
}
