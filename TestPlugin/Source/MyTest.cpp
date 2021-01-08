/*
 ==============================================================================
 FM.c
 Created: 23 Jan 2017 9:39:38am
 Author:  Michael R Mulshine
 ==============================================================================
 */

#include "LEAFTest.h"
#include "MyTest.h"

static void leaf_pool_report(void);
static void leaf_pool_dump(void);
static void run_pool_test(void);

tMBSaw bsaw;
tMBTriangle btri;
tMBPulse bpulse;

tPhasor phasor;

tSVF lp, hp;

tPeriodDetection pd;

tZeroCrossingCounter zc;
tEnvelopeFollower ef;

tTriangle tri;

tNoise noise;
tButterworth bw;

tWavetable wt;
tCompactWavetable cwt;
tWaveset ws;

tBuffer samp;
tMBSampler sampler;

float gain;
float dtime;
bool buttonState;
int ratio = 2;
float x = 0.0f;
float y = 0.0f;
float a, b, c, d;

float* bufIn;
float* bufOut;

LEAF leaf;

#define MSIZE 2048000
char memory[MSIZE];

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(&leaf, sampleRate, blockSize, memory, MSIZE, &getRandomFloat);
    
//    tWavetable_init(&wt, __leaf_table_sawtooth[0], 2048, 10000.f, &leaf);
//    tCompactWavetable_init(&cwt, __leaf_table_sawtooth[0], 2048, 10000.f, &leaf);
    
    tMBTriangle_init(&btri, &leaf);
    tMBPulse_init(&bpulse, &leaf);
    tMBSaw_init(&bsaw, &leaf);
    tMBSaw_setSyncMode(&bsaw, 1);
    tMBTriangle_setSyncMode(&btri, 0);
    tMBPulse_setSyncMode(&bpulse, 0);
    
    tPhasor_init(&phasor, &leaf);
    tPhasor_setFreq(&phasor, 220.f);
    
    float const* set[4];
    set[0] = __leaf_table_sinewave;
    set[1] = __leaf_table_triangle[0];
    set[2] = __leaf_table_squarewave[0];
    set[3] = __leaf_table_sawtooth[0];
    
    tWaveset_init(&ws, set, 4, 2048, 10000.f, &leaf);
    tWaveset_setIndexGain(&ws, 0, -1.0f);
}

inline double getSawFall(double angle) {
    
    angle = fmod(angle + double_Pi, 2*double_Pi); // shift x
    double sample = angle/double_Pi - double(1); // computer as remainder
    
    return sample;
    
}

float   LEAFTest_tick            (float input)
{
//    return tRetune_tick(&retune, input)[0];
//    return tSimpleRetune_tick(&sretune, input);
//    tMBPulse_sync(&bpulse, tPhasor_tick(&phasor) * 2.f - 1.f);
//    return tMBPulse_tick(&bpulse);
//    return tWavetable_tick(&wt);
    return tWaveset_tick(&ws);
}

int firstFrame = 1;
bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float val = getSliderValue("slider1");
    tMBTriangle_setFreq(&btri, val * 440.f);
    tMBPulse_setFreq(&bpulse, val * 160000.f - 80000.0f);
    tMBSaw_setFreq(&bsaw, val * 10000.f);
//    tWavetable_setFreq(&wt, val * 160000.f - 80000.0f);
//    tCompactWavetable_setFreq(&cwt, val * 10000.);
    tWaveset_setFreq(&ws, val * 10000.f);
//    tRetune_tuneVoice(&retune, 0, val * 3.0f + 0.5f);
//    tSimpleRetune_tuneVoice(&sretune, 0, 300);

    val = getSliderValue("slider2");
    tWaveset_setIndex(&ws, val);
//    tRetune_setPitchFactor(&retune, val * 3.0f + 0.5f, 1);
    
    val = getSliderValue("slider3");
//    tRetune_setPitchFactor(&retune, val * 3.0f + 0.5f, 2);
        
}

void    LEAFTest_controllerInput (int cnum, float cval)
{
    
}

void    LEAFTest_pitchBendInput  (int pitchBend)
{
    
}

int lastNote;
void    LEAFTest_noteOn          (int note, float velocity)
{
}


void    LEAFTest_noteOff         (int note)
{
}



void    LEAFTest_end             (void)
{
    
}

// LEAF POOL UTILITIES

void leaf_pool_report(void)
{
    DBG(String(leaf_pool_get_used(&leaf)) + " of  " + String(leaf_pool_get_size(&leaf)));
}

void leaf_pool_dump(void)
{
    float* buff = (float*)leaf_pool_get_pool(&leaf);
    unsigned long siz = leaf_pool_get_size(&leaf);
    siz /= sizeof(float);
    for (int i = 0; i < siz; i++)
    {
        DBG(String(buff[i]));
    }
}

static void run_pool_test(void)
{
    leaf_pool_report();

    DBG("ALLOC BUFFER 1");
    int size = 50;
    float* buffer;
    buffer = (float*) leaf_alloc(&leaf, sizeof(float) * size);

    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)i;

    }

    leaf_pool_report();

    DBG("ALLOC BUFFER 2");
    size = 25;

    buffer = (float*) leaf_alloc(&leaf, sizeof(float) * size);

    leaf_pool_report();

    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)(i*2);
    }
    leaf_free(&leaf, (char*)buffer);

    leaf_pool_report();

    DBG("ALLOC BUFFER 3");
    size = 15;

    buffer = (float*) leaf_alloc(&leaf, sizeof(float) * size);

    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)(i*3);
    }

    leaf_pool_report();

    leaf_pool_dump();
}
