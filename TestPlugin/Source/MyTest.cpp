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

tNoise noise;
tSVF bp1;
tSVF bp2;
tFormantShifter fs;

tSampler samp;
tBuffer buff;
tEnvelope env;

tAutotune at;

tPhasor phasor;

tHighpass hipass;

tMBSaw bsaw;
tMBTriangle btri;
tMBPulse bpulse;

tPitchDetector detector;

float gain;
float freq;
float dtime;
bool buttonState;
int ratio = 2;
float x = 0.0f;
float y = 0.0f;
float a, b, c, d;

#define MSIZE 500000
char memory[MSIZE];

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, memory, MSIZE, &getRandomFloat);
    
    tMBSaw_init(&bsaw);
    tMBTriangle_init(&btri);
    tMBPulse_init(&bpulse);
    
    tPitchDetector_init(&detector, 1000.0f, 5000.0f, 0.0f);
}

inline double getSawFall(double angle) {
    
    angle = fmod(angle + double_Pi, 2*double_Pi); // shift x
    double sample = angle/double_Pi - double(1); // computer as remainder
    
    return sample;
    
}

float   LEAFTest_tick            (float input)
{
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
    
    tPitchDetector_tick(&detector, input);

    tMBTriangle_setFreq(&btri, tPitchDetector_getFrequency(&detector));
    
    return input + tMBTriangle_tick(&btri) * 0.25;
}

int firstFrame = 1;
bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    DBG(tPitchDetector_getFrequency(&detector));
    DBG(tPitchDetector_getPeriodicity(&detector));
    //    if (firstFrame == 1)
    //    {
    //        tBuffer_record(&buff); // starts recording
    //        tSampler_play(&samp); // start spitting samples out
    //        firstFrame = 0;
    //    }
    
    float val = getSliderValue("mod freq");
    
    x = val * 20000.0f + 220.0f;
    
    //    a = val * tBuffer_getBufferLength(&buff);
    
//    DBG("start: " + String(a));
    
    val = getSliderValue("mod depth");
    
    y = val * 2.0f - 1.0f;
//    DBG(String(y));
    //    b = val * tBuffer_getBufferLength(&buff);
    
//    DBG("rate: " + String(b));
    //
    //    tSampler_setStart(&samp, a);
    //    tSampler_setEnd(&samp, b);
    //    tSampler_setRate(&samp, b);
    
    //    tFormantShifter_setShiftFactor(&fs, x);
    //    tFormantShifter_setIntensity(&fs, y);
    
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
