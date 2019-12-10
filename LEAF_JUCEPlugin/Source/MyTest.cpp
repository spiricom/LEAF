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

float gain;
float freq;
float dtime;
bool buttonState;
int ratio = 2;
float x = 0.0f;
float y = 0.0f;

#define MSIZE 500000
char memory[MSIZE];

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, memory, MSIZE, &getRandomFloat);
    
    tNoise_init(&noise, WhiteNoise);
    
    tSVF_init(&bp1, SVFTypeBandpass, 100, 4.0f);
    tSVF_init(&bp2, SVFTypeBandpass, 1000, 4.0f);
    
    tFormantShifter_init(&fs, 2048, 20);
}

float   LEAFTest_tick            (float input)
{
    float sample = tNoise_tick(&noise);
    sample *= 0.5f;
    float b = tSVF_tick(&bp1, sample);
    b += tSVF_tick(&bp2, sample);
    
    return (tFormantShifter_tick(&fs, input));
}


bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float val = getSliderValue("mod freq");
    
    x = val * 3.5f + 0.5f;
    
    DBG("fwarp: " + String(x));
    
    val = getSliderValue("mod depth");
    
    y = val * 49.0f + 1.0f;
    
    DBG("intensity: " + String(y));
    
    tFormantShifter_setShiftFactor(&fs, x);
    tFormantShifter_setIntensity(&fs, y);
    
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
