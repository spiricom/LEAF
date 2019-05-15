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

float mix = 0.f;
float fx = 1.f;

#define NUM_GRAINS 20



tTapeDelay delay;

float feedback = 0.f;

tBuffer buff;
tSampler samp;

tCycle osc;

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, &getRandomFloat);
    
    // Init and set record
    tBuffer_init (&buff, leaf.sampleRate * 1.f); // init, 1 second buffer
    tBuffer_setRecordMode (&buff, RecordOneShot); // RecordOneShot records once through
    tBuffer_record(&buff); // starts recording

    // Init and set play
    tSampler_init (&samp, &buff); // init, give address of record buffer
    tSampler_setMode (&samp, PlayLoop); //set in Loop Mode
    tSampler_setRate(&samp, 1.f); // Rate of 1.0
    tSampler_play(&samp); // start spitting samples out
    
    tCycle_init(&osc);
    tCycle_setFreq(&osc, 110.0f);
    leaf_pool_report();
}

float depth = 1.0f;

float   LEAFTest_tick            (float input)
{
    float sample = 0.f;
    
    tBuffer_tick(&buff, tCycle_tick(&osc)); // ticking the buffer records in to buffer

    // dont tick sampler if buffer not active (not recording)
    if (buff.active == 0)
    {
        sample = tSampler_tick(&samp); // ticking sampler loops sample
    }
    
    
    return sample;
}

bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    bool state = getButtonState("record");
    
    if (state)
    {
        setButtonState("record", false);
        tBuffer_record(&buff);
    }
    
    float val = getSliderValue("rate");
    
    float rate = 8.0 * val - 4.0;
    
    tSampler_setRate(&samp, rate);;
    
    DBG("rate: " + String(rate));
    
    val = getSliderValue("start");
    
    float start = buff.length * val;
    
    tSampler_setStart(&samp, start);;
    
    DBG("start: " + String(start));
    
    val = getSliderValue("end");
    
    float end = buff.length * val;
    
    tSampler_setEnd(&samp, end);;
    
    DBG("end: " + String(end));
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

