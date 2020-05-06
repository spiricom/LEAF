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
// Sine
void    tSine_init(tSine* const cy, int size)
{
    tSine_initToPool(cy, size, &leaf.mempool);
}

void    tSine_free(tSine* const cy)
{
    tSine_freeFromPool(cy, &leaf.mempool);
}

void    tSine_initToPool   (tSine* const cy, int size, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSine* c = *cy = (_tSine*) mpool_alloc(sizeof(_tSine), m);
    
    c->size = size;
    c->sine = (float*) mpool_alloc(sizeof(float) * c->size, m);
    LEAF_generate_sine(c->sine, size);
    c->inc      =  0.0f;
    c->phase    =  0.0f;
    
}

void    tSine_freeFromPool (tSine* const cy, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSine* c = *cy;
    
    mpool_free(c->sine, m);
    mpool_free(c, m);
}

void     tSine_setFreq(tSine* const cy, float freq)
{
    _tSine* c = *cy;
    

    c->freq  = freq;
    
    c->inc = freq * leaf.invSampleRate;
}

float   tSine_tick(tSine* const cy)
{
    _tSine* c = *cy;
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
    samp0 = c->sine[intPart];
    if (++intPart >= c->size) intPart = 0;
    samp1 = c->sine[intPart];
    
    return (samp0 + (samp1 - samp0) * fracPart);
    
}

void     tSineSampleRateChanged (tSine* const cy)
{
    _tSine* c = *cy;
    
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
    p->phaseDidReset = 0;
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



void    tMinBLEP_init           (tMinBLEP* const minblep)
{
    tMinBLEP_initToPool(minblep, &leaf.mempool);
}

void    tMinBLEP_free           (tMinBLEP* const minblep)
{
    tMinBLEP_freeFromPool(minblep, &leaf.mempool);
}

void    tMinBLEP_initToPool     (tMinBLEP* const minblep, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tMinBLEP* mb = *minblep = (_tMinBLEP*) mpool_alloc(sizeof(_tMinBLEP), m);
    
    mb->overSamplingRatio = 16;
    mb->zeroCrossings = 16;
    mb->returnDerivative = 0;
    mb->proportionalBlepFreq = 0.5; // defaults to NyQuist ....
    
    mb->lastValue = 0;
    mb->lastDelta = 0;
    
//     float* x1, x2, y1, y2;
    // AA FILTER
//    zeromem (coefficients, sizeof (coefficients));
    
    mb->minBlepSize = (mb->zeroCrossings * 2 * mb->overSamplingRatio) + 1;
    
    mb->ratio = 1;
    mb->lastRatio = 1;
    
//    createLowPass (ratio);
//    resetFilters();
    
    mb->blepIndex = 0;
    mb->numActiveBleps = 0;
    //currentActiveBlepOffsets;
    mb->offset = (float*) mpool_alloc(sizeof(float) * mb->minBlepSize, m);
    mb->freqMultiple = (float*) mpool_alloc(sizeof(float) * mb->minBlepSize, m);
    mb->pos_change_magnitude = (float*) mpool_alloc(sizeof(float) * mb->minBlepSize, m);
    mb->vel_change_magnitude = (float*) mpool_alloc(sizeof(float) * mb->minBlepSize, m);
    
    mb->minBlepArray = (float*) mpool_alloc(sizeof(float) * mb->minBlepSize, m);
    mb->minBlepDerivArray = (float*) mpool_alloc(sizeof(float) * mb->minBlepSize, m);
    
    tMinBLEP_buildBLEP(minblep);
}

void    tMinBLEP_freeFromPool   (tMinBLEP* const minblep, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tMinBLEP* mb = *minblep;
    
    
    mpool_free(mb, m);
}

// SINC Function
static float SINC(float x)
{
    float pix;
    
    if(x == 0.0f)
        return 1.0f;
    else
    {
        pix = PI * x;
        return sinf(pix) / pix;
    }
}

// Generate Blackman Window
static void BlackmanWindow(int n, float *w)
{
    int m = n - 1;
    int i;
    float f1, f2, fm;
    
    fm = (float)m;
    for(i = 0; i <= m; i++)
    {
        f1 = (2.0f * PI * (float)i) / fm;
        f2 = 2.0f * f1;
        w[i] = 0.42f - (0.5f * cosf(f1)) + (0.08f * cosf(f2));
    }
}

// Discrete Fourier Transform
static void DFT(int n, float *realTime, float *imagTime, float *realFreq, float *imagFreq)
{
    int k, i;
    float sr, si, p;
    
    for(k = 0; k < n; k++)
    {
        realFreq[k] = 0.0f;
        imagFreq[k] = 0.0f;
    }
    
    for(k = 0; k < n; k++)
        for(i = 0; i < n; i++)
        {
            p = (2.0f * PI * (float)(k * i)) / n;
            sr = cosf(p);
            si = -sinf(p);
            realFreq[k] += (realTime[i] * sr) - (imagTime[i] * si);
            imagFreq[k] += (realTime[i] * si) + (imagTime[i] * sr);
        }
}

// Inverse Discrete Fourier Transform
static void InverseDFT(int n, float *realTime, float *imagTime, float *realFreq, float *imagFreq)
{
    int k, i;
    float sr, si, p;
    
    for(k = 0; k < n; k++)
    {
        realTime[k] = 0.0f;
        imagTime[k] = 0.0f;
    }
    
    for(k = 0; k < n; k++)
    {
        for(i = 0; i < n; i++)
        {
            p = (2.0f * PI * (float)(k * i)) / n;
            sr = cosf(p);
            si = -sinf(p);
            realTime[k] += (realFreq[i] * sr) + (imagFreq[i] * si);
            imagTime[k] += (realFreq[i] * si) - (imagFreq[i] * sr);
        }
        realTime[k] /= n;
        imagTime[k] /= n;
    }
}

// Complex Absolute Value
static float complexabs(float x, float y)
{
    return sqrtf((x * x) + (y * y));
}

// Complex Exponential
static void complexexp(float x, float y, float *zx, float *zy)
{
    float expx;
    
    expx = expf(x);
    *zx = expx * cosf(y);
    *zy = expx * sinf(y);
}

// Compute Real Cepstrum Of Signal
static void RealCepstrum(int n, float *signal, float *realCepstrum)
{
    int i;
    
    float realTime[n];
    float imagTime[n];
    float realFreq[n];
    float imagFreq[n];
    
    // Compose Complex FFT Input
    
    for(i = 0; i < n; i++)
    {
        realTime[i] = signal[i];
        imagTime[i] = 0.0f;
    }
    
    // Perform DFT
    
    DFT(n, realTime, imagTime, realFreq, imagFreq);
    
    // Calculate Log Of Absolute Value
    
    for(i = 0; i < n; i++)
    {
        realFreq[i] = logf(complexabs(realFreq[i], imagFreq[i]));
        imagFreq[i] = 0.0f;
    }
    
    // Perform Inverse FFT
    
    InverseDFT(n, realTime, imagTime, realFreq, imagFreq);
    
    // Output Real Part Of FFT
    for(i = 0; i < n; i++)
        realCepstrum[i] = realTime[i];
}

// Compute Minimum Phase Reconstruction Of Signal
static void MinimumPhase(int n, float *realCepstrum, float *minimumPhase)
{
    int i, nd2;
    float realTime[n];
    float imagTime[n];
    float realFreq[n];
    float imagFreq[n];
    
    nd2 = n / 2;
    
    if((n % 2) == 1)
    {
        realTime[0] = realCepstrum[0];
        for(i = 1; i < nd2; i++)
            realTime[i] = 2.0f * realCepstrum[i];
        for(i = nd2; i < n; i++)
            realTime[i] = 0.0f;
    }
    else
    {
        realTime[0] = realCepstrum[0];
        for(i = 1; i < nd2; i++)
            realTime[i] = 2.0f * realCepstrum[i];
        realTime[nd2] = realCepstrum[nd2];
        for(i = nd2 + 1; i < n; i++)
            realTime[i] = 0.0f;
    }
    
    for(i = 0; i < n; i++)
        imagTime[i] = 0.0f;
    
    DFT(n, realTime, imagTime, realFreq, imagFreq);
    
    for(i = 0; i < n; i++)
        complexexp(realFreq[i], imagFreq[i], &realFreq[i], &imagFreq[i]);
    
    InverseDFT(n, realTime, imagTime, realFreq, imagFreq);
    
    for(i = 0; i < n; i++)
        minimumPhase[i] = realTime[i];
}


// Generate the MinBLEP
void tMinBLEP_buildBLEP(tMinBLEP* const minblep)
{
    _tMinBLEP* m = *minblep;
    
    int i, n;
    float r, a, b;
    
    n = m->minBlepSize;
    
    // Generate Sinc
    
    a = -m->overSamplingRatio;
    b = m->overSamplingRatio;
    for(i = 0; i < n; i++)
    {
        r = ((float)i) / ((float)(n - 1));
        m->minBlepArray[i] = SINC(a + (r * (b - a)));
        m->minBlepDerivArray[i] = 0;
    }
    
    // Window Sinc
    
    BlackmanWindow(n, m->minBlepDerivArray);
    for(i = 0; i < n; i++)
        m->minBlepArray[i] *= m->minBlepDerivArray[i];
    
    // Minimum Phase Reconstruction
    
    RealCepstrum(n, m->minBlepArray, m->minBlepDerivArray);
    MinimumPhase(n, m->minBlepDerivArray, m->minBlepArray);
    
    // Integrate Into MinBLEP
    a = 0.0f;
    float secondInt = 0.0f;
    for(i = 0; i < n; i++)
    {
        a += m->minBlepArray[i];
        m->minBlepArray[i] = a;
        
        secondInt += a;
        m->minBlepDerivArray[i] = secondInt;
    }
    
    // Normalize
    a = m->minBlepArray[n - 1];
    a = 1.0f / a;
    b = 0.0f;
    for(i = 0; i < n; i++)
    {
        m->minBlepArray[i] *= a;
        b = fmaxf(b, m->minBlepDerivArray[i]);
    }
    
    // Normalize ...
    b = 1.0f/b;
    for (i = 0; i < n; i++)
    {
        m->minBlepDerivArray[i] *= b;
        m->minBlepDerivArray[i] -= ((float)i) / ((float) n-1);
        
        // SUBTRACT 1 and invert so the signal (so it goes 1->0)
        m->minBlepArray[i] -= 1.0f;
        m->minBlepArray[i] = -m->minBlepArray[i];
    }
}

void    tMinBLEP_addBLEP        (tMinBLEP* const minblep, float offset, float posChange, float velChange)
{
    _tMinBLEP* m = *minblep;
    
    int n = m->minBlepSize;
    
    m->offset[m->blepIndex] = offset;
    m->freqMultiple[m->blepIndex] = m->overSamplingRatio*m->proportionalBlepFreq;
    m->pos_change_magnitude[m->blepIndex] = posChange;
    m->vel_change_magnitude[m->blepIndex] = velChange;
    
    m->blepIndex++;
    if (m->blepIndex >= n) m->blepIndex = 0;
    
    m->numActiveBleps++;
}

float   tMinBLEP_tick           (tMinBLEP* const minblep, float input)
{
    _tMinBLEP* m = *minblep;
    // PROCESS ALL BLEPS -
    /// for each offset, copy a portion of the blep array to the output ....
    
    float sample = input;
    
    int n = m->minBlepSize;

    for (int blep = 1; blep <= m->numActiveBleps; blep++)
    {
        int i = (m->blepIndex - blep + n) % n;
        float adjusted_Freq = m->freqMultiple[i];
        float exactPosition = m->offset[i];
        
        double blepPosExact = adjusted_Freq*(exactPosition + 1); // +1 because this needs to trigger on the LOW SAMPLE
        double blepPosSample = 0;
        double fraction = modf(blepPosExact, &blepPosSample);
        
        // LIMIT the scaling on the derivative array
        // otherwise, it can get TOO large
        double depthLimited = m->proportionalBlepFreq; //jlimit<double>(.1, 1, proportionalBlepFreq);
        double blepDeriv_PosExact = depthLimited*m->overSamplingRatio*(exactPosition + 1);
        double blepDeriv_Sample = 0;
        double fraction_Deriv = modf(blepDeriv_PosExact, &blepDeriv_Sample);
        
        
        // DONE ... we reached the end ...
        if (((int) blepPosExact > n) && ((int) blepDeriv_PosExact > n))
            break;
        
        // BLEP has not yet occurred ...
        if (blepPosExact < 0)
            continue;
        
        
        // 0TH ORDER COMPENSATION ::::
        /// add the BLEP to compensate for discontinuties in the POSITION
        if ( fabs(m->pos_change_magnitude[i]) > 0 && blepPosSample < n)
        {
            // LINEAR INTERPOLATION ::::
            float lowValue = m->minBlepArray[(int) blepPosSample];
            float hiValue = lowValue;
            
            if ((int) blepPosSample + 1 < n)
                hiValue = m->minBlepArray[(int) blepPosSample + 1];
            
            float delta = hiValue - lowValue;
            float exactValue = lowValue + fraction*delta;
            
            // SCALE by the discontinuity magnitude
            exactValue *= m->pos_change_magnitude[i];
            
            // ADD to the thruput
            sample += exactValue;
        }
        
        
        // 1ST ORDER COMPENSATION ::::
        /// add the BLEP DERIVATIVE to compensate for discontinuties in the VELOCITY
        if ( fabs(m->vel_change_magnitude[i]) > 0 && blepDeriv_PosExact < n)
        {
            
            // LINEAR INTERPOLATION ::::
            double lowValue = m->minBlepDerivArray[(int) blepDeriv_PosExact];
            double hiValue = lowValue;
            
            if ((int) blepDeriv_PosExact + 1 < n)
                hiValue = m->minBlepDerivArray[(int) blepDeriv_PosExact + 1];
            
            double delta = hiValue - lowValue;
            double exactValue = lowValue + fraction_Deriv*delta;
            
            // SCALE by the discontinuity magnitude`
            exactValue *= m->vel_change_magnitude[i];
            
            // ADD to the thruput
            sample += exactValue;
            
        }
    
        // UPDATE ::::
        m->offset[i] += 1.0f;
        if (m->offset[i] * adjusted_Freq > n)
        {
            m->numActiveBleps--;
        }
    }
    return sample;
}
