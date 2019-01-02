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
    
    t = leaf.sampleRate * 0.001f;
    
    // ==================DATTORRO===================
    // INPUT
    tDelayL_init(&in_delay, 0.f, 200.f*t);
    tOnePole_init(&in_filter, 1.f);
    
    for (int i = 0; i < 4; i++)
    {
        tDelayA_init(&in_allpass[i], in_allpass_delays[i], 20.f*t);
        tDelayA_setGain(&in_allpass[i], in_allpass_gains[i]);
    }
    
    // FEEDBACK 1
    tDelayA_init(&f1_allpass, 30.51f*t, 100.f*t);
    tDelayA_setGain(&f1_allpass, 0.7f);
    
    tDelayL_init(&f1_delay_1, 141.69f*t, 200.0f*t);
    tDelayL_init(&f1_delay_2, 89.24f*t, 100.0f*t);
    tDelayL_init(&f1_delay_3, 125.f*t, 200.0f*t);
    
    tOnePole_init(&f1_filter, 1.f);
    
    tCycle_init(&f1_lfo);
    tCycle_setFreq(&f1_lfo, 0.1f);
    
    // FEEDBACK 2
    tDelayA_init(&f2_allpass, 22.58f*t, 100.f*t);
    tDelayA_setGain(&f2_allpass, 0.7f);
    
    tDelayL_init(&f2_delay_1, 149.62f*t, 200.0f*t);
    tDelayL_init(&f2_delay_2, 60.48f*t, 100.0f*t);
    tDelayL_init(&f2_delay_3, 106.28f*t, 200.0f*t);
    
    tOnePole_init(&f2_filter, 1.f);
    
    tCycle_init(&f2_lfo);
    tCycle_setFreq(&f2_lfo, 0.07f);
    
    // =============================================
    
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
    // ==================DATTORRO===================
    
    // INPUT
    float in_sample = tDelayL_tick(&in_delay, input);
    
    in_sample = tOnePole_tick(&in_filter, in_sample);
    
    for (int i = 0; i < 4; i++)
    {
        in_sample = tDelayA_tick(&in_allpass[i], in_sample);
    }

    // FEEDBACK 1
    float f1_sample = in_sample + f2_last; // + f2_last_out;
    
    tDelayA_setDelay(&f1_allpass, 30.51f*t + tCycle_tick(&f1_lfo) * 4.0f);
    
    f1_sample = tDelayA_tick(&f1_allpass, f1_sample);
    
    f1_sample = tDelayL_tick(&f1_delay_1, f1_sample);
    
    f1_sample = tOnePole_tick(&f1_filter, f1_sample);
    
    f1_sample = f1_sample + f1_delay_2_last * 0.5f;
    
    float f1_delay_2_sample = tDelayL_tick(&f1_delay_2, f1_sample * 0.5f);
    
    f1_delay_2_last = f1_delay_2_sample;
    
    f1_sample = f1_delay_2_last + f1_sample;
    
    f1_sample *= feedback_gain;
    
    f1_last = tDelayL_tick(&f1_delay_3, f1_sample);

    // FEEDBACK 2
    float f2_sample = in_sample + f1_last;
    
    tDelayA_setDelay(&f2_allpass, 22.58f*t + tCycle_tick(&f2_lfo) * 4.0f);
    
    f2_sample = tDelayA_tick(&f2_allpass, f2_sample);
    
    f2_sample = tDelayL_tick(&f2_delay_1, f2_sample);
    
    f2_sample = tOnePole_tick(&f2_filter, f2_sample);
    
    f2_sample = f2_sample + f2_delay_2_last * 0.5f;
    
    float f2_delay_2_sample = tDelayL_tick(&f2_delay_2, f2_sample * 0.5f);
    
    f2_delay_2_last = f2_delay_2_sample;
    
    f2_sample = f2_delay_2_last + f2_sample;
    
    f2_sample *= feedback_gain;
    
    f2_last = tDelayL_tick(&f2_delay_3, f2_sample);
    
    // TAP OUT 1
    f1_sample =     tDelayL_tapOut(&f1_delay_1, 8.9f*t) +
                    tDelayL_tapOut(&f1_delay_1, 99.8f*t);
    
    f1_sample -=    tDelayL_tapOut(&f1_delay_2, 64.2f*t);
    
    f1_sample +=    tDelayL_tapOut(&f1_delay_3, 67.f*t);
    
    f1_sample -=    tDelayL_tapOut(&f2_delay_1, 66.8f*t);
    
    f1_sample -=    tDelayL_tapOut(&f2_delay_2, 6.3f*t);
    
    f1_sample -=    tDelayL_tapOut(&f2_delay_3, 35.8f*t);
    
    f1_sample *=    0.14;
    
    // TAP OUT 2
    f2_sample =     tDelayL_tapOut(&f2_delay_1, 11.8f*t) +
                    tDelayL_tapOut(&f2_delay_1, 121.7f*t);
    
    f2_sample -=    tDelayL_tapOut(&f2_delay_2, 6.3f*t);
    
    f2_sample +=    tDelayL_tapOut(&f2_delay_3, 89.7f*t);
    
    f2_sample -=    tDelayL_tapOut(&f1_delay_1, 70.8f*t);
    
    f2_sample -=    tDelayL_tapOut(&f1_delay_2, 11.2f*t);
    
    f2_sample -=    tDelayL_tapOut(&f1_delay_3, 4.1f*t);
    
    f2_sample *=    0.14f;
    
    float sample = (f1_sample + f2_sample) * 0.5f;
    
    // =============================================
    
    return (input * (1.0f - mix) + sample * mix);
     
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
