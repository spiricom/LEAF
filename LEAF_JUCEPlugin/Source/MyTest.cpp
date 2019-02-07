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

tCrusher crusher;

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, &randomNumberGenerator);

    tCrusher_init(&crusher);
    
    leaf_pool_report();
}

int timer = 0;

float   LEAFTest_tick            (float input)
{
    float sample = 0.f;
    
    sample = (mix * tCrusher_tick(&crusher, input)) + ((1.f - mix) * input);
    
    return sample;
}

bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float val = getSliderValue("mix");
    
    mix = val;
    
    val = getSliderValue("sr");
    
    tCrusher_setSamplingRatio(&crusher, val * 3.99f + 0.01f);
    
    val = getSliderValue("rnd");
    
    tCrusher_setRound(&crusher, val);
    
    val = getSliderValue("qual");
    
    tCrusher_setQuality(&crusher, val);
    
    val = getSliderValue("op");
    
    tCrusher_setOperation(&crusher, (int)(val * 8.0f));
    
    /*
    float samp = -1.f + 2.f * val;
    
    union unholy_t bw;
    bw.f = samp;
    bw.i = (bw.i | (op << 23));
    
    DBG(String(samp) + " " + String(bw.f));
     */
    
    // rounding test
    /*
    val = getSliderValue("rnd");
    
    float to_rnd = -1.f + 2.f * val;
    
    float rnd = LEAF_round(to_rnd, 0.3f);
    
    DBG("to_rnd: " + String(to_rnd) + " rnd: " + String(rnd));
     */
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
