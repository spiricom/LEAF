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

tOversampler os;
tNoise noise;
tCycle sine;
tFIR filter;

float gain;
bool buttonState;
int ratio = 16;

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, &getRandomFloat);
    
    tOversampler_init(&os, ratio, OTRUE);
    tNoise_init(&noise, WhiteNoise);
    tCycle_init(&sine);
    tCycle_setFreq(&sine, 220);
    leaf_pool_report();
}

float nothing(float val) {
    return val;
}

float   LEAFTest_tick            (float input)
{
    float sample = tCycle_tick(&sine);
    float output[ratio];
    
    if (buttonState) {
        tOversampler_upsample(&os, sample, output);
        for (int i = 0; i < ratio; ++i) {
            output[i] *= gain*10.0f;
            output[i] = LEAF_clip(-1.0f, output[i], 1.0f);
        }
        sample = tOversampler_downsample(&os, output);
    }
    else {
        sample *= gain*10.0f;
        sample = LEAF_clip(-1.0f, sample, 1.0f);
    }
    return sample * gain;
}



bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    
    buttonState = getSliderValue("on/off") > 0.5 ? true : false;
    
    float val = getSliderValue("mod freq");
    
    float freq = 200 + 20000 * val;
    tCycle_setFreq(&sine, freq);
    
    val = getSliderValue("mod depth");
    
    gain = val;
    
    
    

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
    int siz = leaf_pool_get_size();
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

