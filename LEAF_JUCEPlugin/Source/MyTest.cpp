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
tCycle sine;
tDelay delay;
tRetune retune;
tAutotune autotune;

float gain;
float freq;
float dtime;
bool buttonState;
int ratio = 2;

#define MSIZE 500000
char memory[MSIZE];

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, memory, MSIZE, &getRandomFloat);
    
    tNoise_init(&noise, WhiteNoise);
    tCycle_init(&sine);
    tCycle_setFreq(&sine, 200);
    tDelay_init(&delay, 44100, 44100);
    tDelay_free(&delay); //most basic free test
    tDelay_init(&delay, 44100, 44100);
    
    tRetune_init(&retune, 1, 2048, 1024);
    tAutotune_init(&autotune, 8, 2048, 1024);
}

float   LEAFTest_tick            (float input)
{
    float sample = tCycle_tick(&sine);
    float* retuneOut = tRetune_tick(&retune, sample);
    float* autotuneOut = tAutotune_tick(&autotune, sample);
    float r1 = retuneOut[0];
    //float r2 = retuneOut[1];
    float a1 = autotuneOut[6];
    float a2 = autotuneOut[7];
    sample = a1 + a2;
    
    return sample * 0.25f;
}



bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float val = getSliderValue("mod freq");
    tRetune_setPitchFactor(&retune, val*4.0f + 0.5f, 0);
    tAutotune_setFreq(&autotune, val*5000.0f + 50.0f, 7);
    
    val = getSliderValue("mod depth");
    tRetune_setPitchFactor(&retune, val*4.0f + 0.5f, 1);
    tAutotune_setFreq(&autotune, val*5000.0f + 50.0f, 6);
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
    leaf_free(buffer);
    
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
