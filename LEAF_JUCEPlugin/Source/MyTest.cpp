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

tBuffer sample;

tSampler player;

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, &randomNumberGenerator);

    tBuffer_init(&sample, leaf.sampleRate * 2);
    
    tSampler_init(&player, &sample);
    tSampler_setMode(&player, BackAndForth);
    
    leaf_pool_report();
    
    tBuffer_record(&sample);
}

int timer = 0;

float   LEAFTest_tick            (float input)
{
    tBuffer_tick(&sample, input);
    
    return tSampler_tick(&player);
}

bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float val = getSliderValue("rate");
    
    float rate = val * 16.0f - 8.0f;
    tSampler_setRate(&player, rate);
    //DBG("rate: " + String(rate));
    
    val = getSliderValue("start");
    float start = val * sample.length;
    tSampler_setStart(&player,  start);
    //DBG("start: " + String(start));
    
    val = getSliderValue("end");
    float end = val * sample.length;
    tSampler_setEnd(&player, end);
    //DBG("end: " + String(end));
    
    val = getSliderValue("cfxlen");
    uint32_t cfxlen = val * 500;
    tSampler_setCrossfadeLength(&player, cfxlen);
    DBG("cfxlen: " + String(cfxlen));
    
    bool state = getButtonState("sample");
    
    if (state && !lastState)
    {
        timer = 0;
        tBuffer_record(&sample);
    }
    
    lastState = state;
    
    state = getButtonState("play");
    
    if (state && !lastPlayState)
    {
        tSampler_play(&player);
    }
    
    lastPlayState = state;
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
