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

tWaveTable wt;
tCompactWaveTable cwt;
tWaveSynth ws;

tBuffer samp;
tMBSampler sampler;

const int numWavetables = 1;
tWaveTable wavetables[numWavetables];

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

int lastLoadedAudioSize = 0;

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(&leaf, sampleRate, blockSize, memory, MSIZE, &getRandomFloat);
    
//    tWaveTable_init(&wt, __leaf_table_sawtooth[0], 2048, 10000.f, &leaf);
//    tCompactWaveTable_init(&cwt, __leaf_table_sawtooth[0], 2048, 10000.f, &leaf);
    
    tMBTriangle_init(&btri, &leaf);
    tMBPulse_init(&bpulse, &leaf);
    tMBSaw_init(&bsaw, &leaf);
    tMBSaw_setSyncMode(&bsaw, 1);
    tMBTriangle_setSyncMode(&btri, 0);
    tMBPulse_setSyncMode(&bpulse, 0);
    
    tPhasor_init(&phasor, &leaf);
    tPhasor_setFreq(&phasor, 220.f);
    
    float* set[4];
    set[0] = (float*)__leaf_table_sinewave;
    set[1] = (float*)__leaf_table_triangle[0];
    set[2] = (float*)__leaf_table_squarewave[0];
    set[3] = (float*)__leaf_table_sawtooth[0];
    int sizes[4];
    for (int i = 0; i < 4; i++) sizes[i] = 2048;
    
    tWaveSynth_init(&ws, 1, set, sizes, 4, 10000.f, &leaf);
    
    lastLoadedAudioSize = 0;
    loadedAudio.clear();
}

inline double getSawFall(double angle) {
    
    angle = fmod(angle + double_Pi, 2*double_Pi); // shift x
    double sample = angle/double_Pi - double(1); // computer as remainder
    
    return sample;
    
}

float   LEAFTest_tick            (float input)
{
    float out = 0.0f;
    for (int i = 0; i < fmin(loadedAudio.size(), numWavetables); ++i)
    {
//        out += tWaveTable_tick(&wavetables[i]);
    }
    return out;
}

int firstFrame = 1;
bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float val = getSliderValue("slider1");
    tMBTriangle_setFreq(&btri, val * 440.f);
    tMBPulse_setFreq(&bpulse, val * 160000.f - 80000.0f);
    tMBSaw_setFreq(&bsaw, val * 10000.f);
//    tWaveTable_setFreq(&wt, val * 160000.f - 80000.0f);
//    tCompactWaveTable_setFreq(&cwt, val * 10000.);
    tWaveSynth_setFreq(&ws, 0, val * 10000.f);
//    tRetune_tuneVoice(&retune, 0, val * 3.0f + 0.5f);
//    tSimpleRetune_tuneVoice(&sretune, 0, 300);

    val = getSliderValue("slider2");
    tWaveSynth_setIndex(&ws, val);
//    tRetune_setPitchFactor(&retune, val * 3.0f + 0.5f, 1);
    
    val = getSliderValue("slider3");
//    tRetune_setPitchFactor(&retune, val * 3.0f + 0.5f, 2);
        
    if (lastLoadedAudioSize < loadedAudio.size())
    {
        int i = (loadedAudio.size() - 1) % numWavetables;
        if (loadedAudio.size() - 1 >= numWavetables) tWaveTable_free(&wavetables[i]);
        tWaveTable_init(&wavetables[i], (float*)loadedAudio[loadedAudio.size() - 1].getReadPointer(0), loadedAudio[loadedAudio.size() - 1].getNumSamples(), 20000, &leaf);
        
        lastLoadedAudioSize = loadedAudio.size();
    }
//    if (loadedAudio.size() > 0) tWaveTable_setFreq(&wavetables[0], 220);//val * 10000.f);
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
