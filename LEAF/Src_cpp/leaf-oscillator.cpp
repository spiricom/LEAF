/*
  ==============================================================================

    LEAFOscillator.c
    Created: 20 Jan 2017 12:00:58pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-wavetables.h"
#include "..\Inc\leaf-oscillator.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-wavetables.h"
#include "../Inc/leaf-oscillator.h"
#include "../leaf.h"

#endif

void     tNeuronSampleRateChanged(tNeuron* n)
{
    
}

void    tNeuron_init(tNeuron* const n)
{
    tPoleZero_init(&n->f);
    
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

void    tNeuron_free(tNeuron* const n)
{
    tPoleZero_free(&n->f);
    
    leaf_free(n);
}

void   tNeuron_reset(tNeuron* const n)
{
    
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


void        tNeuron_setV1(tNeuron* const n, float V1)
{
    n->V[0] = V1;
}


void        tNeuron_setV2(tNeuron* const n, float V2)
{
    n->V[1] = V2;
}

void        tNeuron_setV3(tNeuron* const n, float V3)
{
    n->V[2] = V3;
}

void        tNeuron_setTimeStep(tNeuron* const n, float timeStep)
{
    n->timeStep = timeStep;
}

void        tNeuron_setK(tNeuron* const n, float K)
{
    n->gK = K;
}

void        tNeuron_setL(tNeuron* const n, float L)
{
    n->gL = L;
    n->rate[2] = n->gL/n->C;
}

void        tNeuron_setN(tNeuron* const n, float N)
{
    n->gN = N;
}

void        tNeuron_setC(tNeuron* const n, float C)
{
    n->C = C;
    n->rate[2] = n->gL/n->C;
}

float   tNeuron_tick(tNeuron* const n)
{
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

void        tNeuron_setMode  (tNeuron* const n, NeuronMode mode)
{
    n->mode = mode;
}

void        tNeuron_setCurrent  (tNeuron* const n, float current)
{
    n->current = current;
}

// Cycle
void    tCycle_init(tCycle* const c)
{
    c->inc      =  0.0f;
    c->phase    =  0.0f;
    c->tsize     = 2048;
    
    tCycle_setTableSize(c, c->tsize);
}

void    tCycle_free(tCycle* const c)
{
    leaf_free(c->table);
    
    leaf_free(c);
}

void    tCycle_setTableSize(tCycle* const c, int size)
{
    leaf_free(c->table);
    
    c->tsize = size;
    
    c->table = (float*) leaf_alloc(c->tsize * sizeof(float));
    
    LEAF_generate_sine(c->table, c->tsize);
}

int     tCycle_setFreq(tCycle* const c, float freq)
{
    if (freq < 0.0f) freq = 0.0f;
    
    c->freq = freq;
    c->inc = freq * leaf.invSampleRate;
    
    return 0;
}

float   tCycle_tick(tCycle* const c)
{
    // Phasor increment
    c->phase += c->inc;
    if (c->phase >= 1.0f) c->phase -= 1.0f;
    
    // Wavetable synthesis
    float temp = c->tsize * c->phase;
    int intPart = (int)temp;
    float fracPart = temp - (float)intPart;
    float samp0 = c->table[intPart];
    if (++intPart >= c->tsize) intPart = 0;
    float samp1 = c->table[intPart];
    return (samp0 + (samp1 - samp0) * fracPart);
}

void     tCycleSampleRateChanged (tCycle* const c)
{
    c->inc = c->freq * leaf.invSampleRate;
}

/* Phasor */
void     tPhasorSampleRateChanged (tPhasor* const p)
{
    p->inc = p->freq * leaf.invSampleRate;
};

int     tPhasor_setFreq(tPhasor* const p, float freq)
{
    if (freq < 0.0f) freq = 0.0f;
    
    p->freq = freq;
    p->inc = freq * leaf.invSampleRate;
    
    return 0;
}

float   tPhasor_tick(tPhasor* const p)
{
    p->phase += p->inc;
    
    if (p->phase >= 1.0f) p->phase -= 1.0f;
    
    return p->phase;
}

void    tPhasor_init(tPhasor* const p)
{
    p->phase = 0.0f;
    p->inc = 0.0f;

}

void    tPhasor_free(tPhasor* const p)
{
    leaf_free(p);
}

void    tSawtooth_init(tSawtooth* const c)
{
    c->inc      = 0.0f;
    c->phase    = 0.0f;
    c->tsize    = 2048;
    
    tSawtooth_setTableSize(c, c->tsize);
}

void    tSawtooth_free(tSawtooth* const c)
{
    for (int i = 0; i < NUM_TABLES; i++)
    {
        leaf_free(c->table[i]);
    }
    
    leaf_free(c);
}

void    tSawtooth_setTableSize  (tSawtooth*  const c, int size)
{
    c->tsize = size;
    
    float freq = 20.0f;
    for (int i = 0; i < NUM_TABLES; i++)
    {
        leaf_free(c->table[i]);
        
        c->table[i] = (float*) leaf_alloc(c->tsize * sizeof(float));
        
        LEAF_generate_sawtooth(c->table[i], freq, c->tsize);
        
        freq *= 2.0f;
    }
}

int     tSawtooth_setFreq(tSawtooth* const c, float freq)
{
    if (freq < 0.0f) freq = 0.0f;
    
    c->freq = freq;
    c->inc = freq * leaf.invSampleRate;
    
    return 0;
}

float   tSawtooth_tick(tSawtooth* const c)
{
    // Phasor increment
    c->phase += c->inc;
    if (c->phase >= 1.0f) c->phase -= 1.0f;
    
    float out = 0.0f;
    float w;
    
    int idx = (int)(c->phase * c->tsize);
    
    // Wavetable synthesis
    
    if (c->freq <= 20.0f)
    {
        out = c->table[T20][idx];
    }
    else if (c->freq <= 40.0f)
    {
        w = ((40.0f - c->freq) * INV_20);
        out = (c->table[T20][idx] * w) + (c->table[T40][idx] * (1.0f - w));
    }
    else if (c->freq <= 80.0f)
    {
        w = ((80.0f - c->freq) * INV_40);
        out = (c->table[T40][idx] * w) + (c->table[T80][idx] * (1.0f - w));
    }
    else if (c->freq <= 160.0f)
    {
        w = ((160.0f - c->freq) * INV_80);
        out = (c->table[T80][idx] * w) + (c->table[T160][idx] * (1.0f - w));
    }
    else if (c->freq <= 320.0f)
    {
        w = ((320.0f - c->freq) * INV_160);
        out = (c->table[T160][idx] * w) + (c->table[T320][idx] * (1.0f - w));
    }
    else if (c->freq <= 640.0f)
    {
        w = ((640.0f - c->freq) * INV_320);
        out = (c->table[T320][idx] * w) + (c->table[T640][idx] * (1.0f - w));
    }
    else if (c->freq <= 1280.0f)
    {
        w = ((1280.0f - c->freq) * INV_640);
        out = (c->table[T640][idx] * w) + (c->table[T1280][idx] * (1.0f - w));
    }
    else if (c->freq <= 2560.0f)
    {
        w = ((2560.0f - c->freq) * INV_1280);
        out = (c->table[T1280][idx] * w) + (c->table[T2560][idx] * (1.0f - w));
    }
    else if (c->freq <= 5120.0f)
    {
        w = ((5120.0f - c->freq) * INV_2560);
        out = (c->table[T2560][idx] * w) + (c->table[T5120][idx] * (1.0f - w));
    }
    else if (c->freq <= 10240.0f)
    {
        w = ((10240.0f - c->freq) * INV_5120);
        out = (c->table[T5120][idx] * w) + (c->table[T10240][idx] * (1.0f - w));
    }
    else if (c->freq <= 20480.0f)
    {
        w = ((20480.0f - c->freq) * INV_10240);
        out = (c->table[T10240][idx] * w) + (c->table[T20480][idx] * (1.0f - w));
    }
    else if (c->freq <= 24000.0f)
    {
        out = c->table[T20480][idx];
    }
    
    return out;
}

void     tSawtoothSampleRateChanged (tSawtooth* const c)
{
    c->inc = c->freq * leaf.invSampleRate;
}

/* Triangle */
void    tTriangle_start(tTriangle* const c)
{
    
}

void   tTriangle_init(tTriangle* const c)
{
    c->inc      =  0.0f;
    c->phase    =  0.0f;
    c->tsize     = 2048;

    tTriangle_setTableSize(c, c->tsize);
}

void   tTriangle_free(tTriangle* const c)
{
    for (int i = 0; i < NUM_TABLES; i++)
    {
        leaf_free(c->table[i]);
    }
    
    leaf_free(c);
}

void    tTriangle_setTableSize  (tTriangle*  const c, int size)
{
    c->tsize = size;
    
    float freq = 20.0f;
    for (int i = 0; i < NUM_TABLES; i++)
    {
        leaf_free(c->table[i]);
        
        c->table[i] = (float*) leaf_alloc(c->tsize * sizeof(float));
        
        LEAF_generate_triangle(c->table[i], freq, c->tsize);
        
        freq *= 2.0f;
    }
}

int tTriangle_setFreq(tTriangle* const c, float freq)
{
    if (freq < 0.0f) freq = 0.0f;
    
    c->freq = freq;
    c->inc = freq * leaf.invSampleRate;
    
    return 0;
}


float   tTriangle_tick(tTriangle* const c)
{
    // Phasor increment
    c->phase += c->inc;
    if (c->phase >= 1.0f) c->phase -= 1.0f;
    
    float out = 0.0f;
    float w;
    
    int idx = (int)(c->phase * c->tsize);
    
    // Wavetable synthesis
    
    if (c->freq <= 20.0f)
    {
        out = c->table[T20][idx];
    }
    else if (c->freq <= 40.0f)
    {
        w = ((40.0f - c->freq) * INV_20);
        out = (c->table[T20][idx] * w) + (c->table[T40][idx] * (1.0f - w));
    }
    else if (c->freq <= 80.0f)
    {
        w = ((80.0f - c->freq) * INV_40);
        out = (c->table[T40][idx] * w) + (c->table[T80][idx] * (1.0f - w));
    }
    else if (c->freq <= 160.0f)
    {
        w = ((160.0f - c->freq) * INV_80);
        out = (c->table[T80][idx] * w) + (c->table[T160][idx] * (1.0f - w));
    }
    else if (c->freq <= 320.0f)
    {
        w = ((320.0f - c->freq) * INV_160);
        out = (c->table[T160][idx] * w) + (c->table[T320][idx] * (1.0f - w));
    }
    else if (c->freq <= 640.0f)
    {
        w = ((640.0f - c->freq) * INV_320);
        out = (c->table[T320][idx] * w) + (c->table[T640][idx] * (1.0f - w));
    }
    else if (c->freq <= 1280.0f)
    {
        w = ((1280.0f - c->freq) * INV_640);
        out = (c->table[T640][idx] * w) + (c->table[T1280][idx] * (1.0f - w));
    }
    else if (c->freq <= 2560.0f)
    {
        w = ((2560.0f - c->freq) * INV_1280);
        out = (c->table[T1280][idx] * w) + (c->table[T2560][idx] * (1.0f - w));
    }
    else if (c->freq <= 5120.0f)
    {
        w = ((5120.0f - c->freq) * INV_2560);
        out = (c->table[T2560][idx] * w) + (c->table[T5120][idx] * (1.0f - w));
    }
    else if (c->freq <= 10240.0f)
    {
        w = ((10240.0f - c->freq) * INV_5120);
        out = (c->table[T5120][idx] * w) + (c->table[T10240][idx] * (1.0f - w));
    }
    else if (c->freq <= 20480.0f)
    {
        w = ((20480.0f - c->freq) * INV_10240);
        out = (c->table[T10240][idx] * w) + (c->table[T20480][idx] * (1.0f - w));
    }
    else if (c->freq <= 24000.0f)
    {
        out = c->table[T20480][idx];
    }
    
    return out;
}

void     tTriangleSampleRateChanged (tTriangle*  const c)
{
    c->inc = c->freq * leaf.invSampleRate;
}

/* Square */
void   tSquare_init(tSquare* const c)
{
    c->inc      =  0.0f;
    c->phase    =  0.0f;
    c->tsize     = 2048;

    tSquare_setTableSize(c, c->tsize);
}

void   tSquare_free(tSquare* const c)
{
    for (int i = 0; i < NUM_TABLES; i++)
    {
        leaf_free(c->table[i]);
    }
    
    leaf_free(c);
}

void    tSquare_setTableSize  (tSquare*  const c, int size)
{
    c->tsize = size;
    
    float freq = 20.0f;
    for (int i = 0; i < NUM_TABLES; i++)
    {
        leaf_free(c->table[i]);
        
        c->table[i] = (float*) leaf_alloc(c->tsize * sizeof(float));
        
        LEAF_generate_square(c->table[i], freq, c->tsize);
        
        freq *= 2.0f;
    }
}

int     tSquare_setFreq(tSquare*  const c, float freq)
{
    if (freq < 0.0f) freq = 0.0f;
    
    c->freq = freq;
    c->inc = freq * leaf.invSampleRate;
    
    return 0;
}

float   tSquare_tick(tSquare* const c)
{
    // Phasor increment
    c->phase += c->inc;
    if (c->phase >= 1.0f) c->phase -= 1.0f;
    
    float out = 0.0f;
    float w = 0.0f;

    int idx = (int)(c->phase * c->tsize);

    // Wavetable synthesis
    
    if (c->freq <= 20.0f)
    {
        out = c->table[T20][idx];
    }
    else if (c->freq <= 40.0f)
    {
        w = ((40.0f - c->freq) * INV_20);
        out = (c->table[T20][idx] * w) + (c->table[T40][idx] * (1.0f - w));
    }
    else if (c->freq <= 80.0f)
    {
        w = ((80.0f - c->freq) * INV_40);
        out = (c->table[T40][idx] * w) + (c->table[T80][idx] * (1.0f - w));
    }
    else if (c->freq <= 160.0f)
    {
        w = ((160.0f - c->freq) * INV_80);
        out = (c->table[T80][idx] * w) + (c->table[T160][idx] * (1.0f - w));
    }
    else if (c->freq <= 320.0f)
    {
        w = ((320.0f - c->freq) * INV_160);
        out = (c->table[T160][idx] * w) + (c->table[T320][idx] * (1.0f - w));
    }
    else if (c->freq <= 640.0f)
    {
        w = ((640.0f - c->freq) * INV_320);
        out = (c->table[T320][idx] * w) + (c->table[T640][idx] * (1.0f - w));
    }
    else if (c->freq <= 1280.0f)
    {
        w = ((1280.0f - c->freq) * INV_640);
        out = (c->table[T640][idx] * w) + (c->table[T1280][idx] * (1.0f - w));
    }
    else if (c->freq <= 2560.0f)
    {
        w = ((2560.0f - c->freq) * INV_1280);
        out = (c->table[T1280][idx] * w) + (c->table[T2560][idx] * (1.0f - w));
    }
    else if (c->freq <= 5120.0f)
    {
        w = ((5120.0f - c->freq) * INV_2560);
        out = (c->table[T2560][idx] * w) + (c->table[T5120][idx] * (1.0f - w));
    }
    else if (c->freq <= 10240.0f)
    {
        w = ((10240.0f - c->freq) * INV_5120);
        out = (c->table[T5120][idx] * w) + (c->table[T10240][idx] * (1.0f - w));
    }
    else if (c->freq <= 20480.0f)
    {
        w = ((20480.0f - c->freq) * INV_10240);
        out = (c->table[T10240][idx] * w) + (c->table[T20480][idx] * (1.0f - w));
    }
    else if (c->freq <= 24000.0f)
    {
        out = c->table[T20480][idx];
    }
    
    return out;
}

void     tSquareSampleRateChanged (tSquare*  const c)
{
    c->inc = c->freq * leaf.invSampleRate;
}

/* Noise */
void    tNoise_init(tNoise* const n, NoiseType type)
{
    n->type = type;
    n->rand = leaf.random;
}

void    tNoise_free(tNoise* const n)
{
    leaf_free(n);
}

float   tNoise_tick(tNoise* const n)
{
    float rand = n->rand();
    
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
