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

tDattorro reverb;


tTapeDelay delay;

float size;
tRamp dtime;


void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, &randomNumberGenerator);

    tDattorro_init(&reverb);

    //tTapeDelay_init(&delay, 0.5f * leaf.sampleRate, leaf.sampleRate);
    
    tRamp_init(&dtime, 500.0f, 1);
    

    setSliderValue("mix", reverb.mix);
    setSliderValue("predelay", reverb.predelay / 200.0f);
    setSliderValue("input filter", reverb.input_filter / 20000.0f);
    setSliderValue("feedback filter", reverb.feedback_filter / 20000.0f);
    setSliderValue("feedback gain", reverb.feedback_gain);
    setSliderValue("size", reverb.size / 4.0f);
    
    
    leaf_pool_report();
}

int timer = 0;


#define CLICK 0

float   LEAFTest_tick            (float input)
{
#if CLICK
    input = 0.0f;
    
    timer++;
    if (timer == (1 * leaf.sampleRate))
    {
        timer = 0;
        
        input = 1.0f;
    }
#endif
    //tTapeDelay_setDelay(&delay, tRamp_tick(&dtime));
    
    //return tTapeDelay_tick(&delay, input);
    
    
    tDattorro_setSize(&reverb, tRamp_tick(&dtime));
    
    return tDattorro_tick(&reverb, input);
    
}

void    LEAFTest_block           (void)
{
  
    float val = getSliderValue("mix");
    tDattorro_setMix(&reverb, val);
    
    val = getSliderValue("predelay");
    tDattorro_setInputDelay(&reverb, val * 200.0f);
    
    val = getSliderValue("input filter");
    tDattorro_setInputFilter(&reverb, val * 20000.0f);
    
    val = getSliderValue("feedback filter");
    tDattorro_setFeedbackFilter(&reverb, val * 20000.0f);
    
    val = getSliderValue("feedback gain");
    tDattorro_setFeedbackGain(&reverb, val);
    
    val = getSliderValue("size");
    tRamp_setDest(&dtime, val * reverb.size_max);
    
    
    //float val = getSliderValue("size");
    //tRamp_setDest(&dtime, val * leaf.sampleRate + 1);
    
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
