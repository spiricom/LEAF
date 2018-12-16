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

t808Snare snare;

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAFInit(sampleRate, blockSize, &randomNumberGenerator);
    
    t808Snare_init(&snare);
    
    leaf_pool_report();
}

int timer = 0;

float prev = 0.0;

float   LEAFTest_tick            (float input)
{
    float sample = 0.0f;
    
    timer++;
    if (timer == (1 * leaf.sampleRate))
    {
        timer = 0;
        t808Snare_on(&snare, 1.0);
    }

    sample = t808Snare_tick(&snare);
    
    return sample;
     
}

void    LEAFTest_block           (void)
{
    float val = getSliderValue("mix");
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
