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

tRetune retune;
tSimpleRetune sretune;

tCompressor compressor;

tSVF lp, hp;

tPeriodDetection pd;

tZeroCrossingCounter zc;
tEnvelopeFollower ef;

tTriangle tri;

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
    
    tRetune_init(&retune, 1, mtof(48), mtof(72), 2048, &leaf);
    tSimpleRetune_init(&sretune, 1, mtof(48), mtof(72), 2048, &leaf);
    tSimpleRetune_setMode(&sretune, 1);
}

inline double getSawFall(double angle) {
    
    angle = fmod(angle + double_Pi, 2*double_Pi); // shift x
    double sample = angle/double_Pi - double(1); // computer as remainder
    
    return sample;
    
}

float   LEAFTest_tick            (float input)
{
//    return tRetune_tick(&retune, input)[0];
    return tSimpleRetune_tick(&sretune, input);
}

int firstFrame = 1;
bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float val = getSliderValue("slider1");
    tRetune_tuneVoice(&retune, 0, val * 3.0f + 0.5f);
    tSimpleRetune_tuneVoice(&sretune, 0, 300);

    val = getSliderValue("slider2");
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
