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


// INPUT
tDelayL     in_delay;
tOnePole    in_filter;
tDelayA     in_allpass[4];
float       in_allpass_delays[4] = { 4.771f, 3.595f, 12.73f, 9.307f };
float       in_allpass_gains[4] = { 0.75f, 0.75f, 0.625f, 0.625f };

// FEEDBACK 1
tDelayA     f1_allpass;
tDelayL     f1_delay_1;
tOnePole    f1_filter;
tDelayL     f1_delay_2;
tDelayL     f1_delay_3;

tCycle      f1_lfo;

// FEEDBACK 2
tDelayA     f2_allpass;
tDelayL     f2_delay_1;
tOnePole    f2_filter;
tDelayL     f2_delay_2;
tDelayL     f2_delay_3;

tCycle      f2_lfo;

float t;

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, &randomNumberGenerator);

    leaf_pool_report();
}

int timer = 0;


float f1_delay_2_last;
float f2_delay_2_last;

float f1_last;
float f2_last;


float mix;
float predelay = 0.f;
float input_filter = 1.;
float feedback_filter = 1.f;
float feedback_gain = 0.0f;



float   LEAFTest_tick            (float input)
{

    return 0.0f;
     
}

void    LEAFTest_block           (void)
{
    float val = getSliderValue("mix");
    
    mix = val;
    
    val = getSliderValue("predelay");
    
    predelay = (val * 200.f) * leaf.sampleRate * 0.001f;
    tDelayL_setDelay(&in_delay, predelay);
    
    val = getSliderValue("input filter");
    
    input_filter =  val * 20000.f;
    tOnePole_setFreq(&in_filter, input_filter);
    
    val = getSliderValue("feedback filter");
    
    feedback_filter = val * 20000.f;
    tOnePole_setFreq(&f1_filter, feedback_filter);
    tOnePole_setFreq(&f2_filter, feedback_filter);
    
    val = getSliderValue("feedback gain");
    
    feedback_gain = val;
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
    
    DBG("FREE BUFFER 2");
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
