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

tDualPitchDetector detector;

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

#define MSIZE 2048000
char memory[MSIZE];

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, memory, MSIZE, &getRandomFloat);
    
    tMBSaw_init(&bsaw);
    tMBTriangle_init(&btri);
    tMBPulse_init(&bpulse);
    
    bufIn = (float*) leaf_alloc(sizeof(float) * 4096);
    bufOut = (float*) leaf_alloc(sizeof(float) * 4096);
    
    tDualPitchDetector_init(&detector, mtof(53), mtof(77));
    
    tCompressor_init(&compressor);
    
    tSVF_init(&lp, SVFTypeLowpass, mtof(77) * 2.0f, 1.0f);
    tSVF_init(&hp, SVFTypeHighpass, mtof(48) * 0.5f, 1.0f);
    
    tPeriodDetection_init(&pd, bufIn, bufOut, 4096, 1024);
    
    tZeroCrossingCounter_init(&zc, 128);
    tEnvelopeFollower_init(&ef, 0.02f, 0.9999f);
    
    tTriangle_init(&tri);
    tTriangle_setFreq(&tri, 100);
    
    tBuffer_init(&samp, 5.0f * leaf.sampleRate);
    tMBSampler_init(&sampler, &samp);
    
    tMBSampler_setMode(&sampler, PlayLoop);
    tMBSampler_setEnd(&sampler, samp->bufferLength);
}

inline double getSawFall(double angle) {
    
    angle = fmod(angle + double_Pi, 2*double_Pi); // shift x
    double sample = angle/double_Pi - double(1); // computer as remainder
    
    return sample;
    
}

float   LEAFTest_tick            (float input)
{
//    tBuffer_tick(&samp, input);
//
//    return tMBSampler_tick(&sampler);
    
    
//    tMBSaw_setFreq(&bsaw, x);
//    tMBTriangle_setFreq(&btri, x);
//    tMBPulse_setFreq(&bpulse, x);
//
//    tMBTriangle_setWidth(&btri, y);
//    tMBPulse_setWidth(&bpulse, y);
//
////    return tMBSaw_tick(&bsaw);
////    return tMBTriangle_tick(&btri);
//    return tMBPulse_tick(&bpulse);
    
    input = tSVF_tick(&hp, tSVF_tick(&lp, tCompressor_tick(&compressor, input)));
    
//    float freq = 1.0f/tPeriodDetection_tick(&pd, input) * leaf.sampleRate;
    tDualPitchDetector_tick(&detector, input);
    float altFreq = tDualPitchDetector_getFrequency(&detector);

//    if (fabsf(1.0f - (freq / altFreq)) < 0.05f)
//    if (tZeroCrossingCounter_tick(&zc, input) < 0.05 && freq > 0.0f)
    if (altFreq > 0.0f)
    {
        tTriangle_setFreq(&tri, altFreq);
    }

    float g = tEnvelopeFollower_tick(&ef, input);

    return tTriangle_tick(&tri) * g;
}

int firstFrame = 1;
bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float periodicity = tDualPitchDetector_getPeriodicity(&detector);
    if (periodicity > 0.99f)
    {
        DBG(tDualPitchDetector_getFrequency(&detector));
        DBG(tDualPitchDetector_getPeriodicity(&detector));
    }
    
    float val = getSliderValue("on/off");
    
    if (val > 0.5f && !sampler->active)
    {
        tBuffer_record(&samp);
        tMBSampler_play(&sampler);
    }
    else if (val < 0.5f && sampler->active)
    {
        tMBSampler_stop(&sampler);
    }
    
    val = getSliderValue("mod freq");
    
    tMBSampler_setStart(&sampler, val * 5.0f * leaf.sampleRate);

    
    val = getSliderValue("mod depth");
    
    tMBSampler_setRate(&sampler, val * 8.0f - 4.0f);
    
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
    DBG(String(leaf_pool_get_used()) + " of  " + String(leaf_pool_get_size()));
}

void leaf_pool_dump(void)
{
    float* buff = (float*)leaf_pool_get_pool();
    unsigned long siz = leaf_pool_get_size();
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
    buffer = (float*) leaf_alloc(sizeof(float) * size);
    
    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)i;
        
    }
    
    leaf_pool_report();
    
    DBG("ALLOC BUFFER 2");
    size = 25;
    
    buffer = (float*) leaf_alloc(sizeof(float) * size);
    
    leaf_pool_report();
    
    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)(i*2);
    }
    leaf_free((char*)buffer);
    
    leaf_pool_report();
    
    DBG("ALLOC BUFFER 3");
    size = 15;
    
    buffer = (float*) leaf_alloc(sizeof(float) * size);
    
    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)(i*3);
    }
    
    leaf_pool_report();
    
    leaf_pool_dump();
}
