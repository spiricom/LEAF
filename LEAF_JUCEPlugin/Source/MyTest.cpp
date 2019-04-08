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



#define MAX_GRAINS 50

tBuffer buff;
tSampler samp[MAX_GRAINS];

tTapeDelay delay;

float feedback = 0.f;
float mix = 0.f;
int numGrains = 10;

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, &getRandomFloat);
    
    // Init and set record loop
    tBuffer_init (&buff, leaf.sampleRate * 1.f); // 1.0-second buffers
    tBuffer_setRecordMode (&buff, RecordLoop);
    tBuffer_record(&buff);
    
    for (int i = 0; i < MAX_GRAINS; i++)
    {
        // Init and set play loop
        tSampler_init (&samp[i], &buff);
        tSampler_setMode (&samp[i], PlayLoop);

        /*
        float speed = ((getRandomFloat() < 0.5f) ? 0.5f : 1.f);
        float dir = (getRandomFloat() < 0.5f) ? -1 : 1;
        
        tSampler_setRate(&samp[i], speed * dir);
         */
        
        tSampler_setRate(&samp[i], 1.f);
        
        // Record and play
        tSampler_play(&samp[i]);
    }
    
    tTapeDelay_init(&delay, leaf.sampleRate * 0.05f, leaf.sampleRate * 1.f); // 1 second delay, starts out at 50 ms
    
    leaf_pool_report();
}

int timer = 0;

float lastOut;
float lastDelayOut;
float makeup;

float   LEAFTest_tick            (float input)
{
    float delaySample = input + lastDelayOut * feedback;
    
    delaySample = tTapeDelay_tick( &delay, 0.5f * delaySample);
    
    lastDelayOut = delaySample;
    
    tBuffer_tick(&buff, lastDelayOut); // read delayed samples in to buffer
    
    float sample = 0.f;
    
    for (int i = 0; i < numGrains; i++)
    {
        sample += tSampler_tick(&samp[i]);
    }
    
    sample /= (float)numGrains;
    
    sample *= makeup;
    
    lastOut = sample;

    return  (sample * mix) +
            (input * (1.f - mix));
}

bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float val = getSliderValue("mix");
    mix = val;
    
    
    val = getSliderValue("delay");
    tTapeDelay_setDelay(&delay, val * leaf.sampleRate);
    
    
    val = getSliderValue("feedback");
    feedback = val;
    
    
    val = getSliderValue("density");
    numGrains = LEAF_clip(1, val * MAX_GRAINS, 50);

    
    val = getSliderValue("makeup");
    makeup = 1.f + val * 3.f;
    
    
    for (int i = 0; i < numGrains; i++)
    {
        if (getRandomFloat() < 0.01f)
        {
            tSampler_setStart(&samp[i], leaf.sampleRate * 0.05f + leaf.sampleRate * getRandomFloat() * 0.95f);
            
            uint64_t end = (leaf.sampleRate * 0.05f + getRandomFloat() * leaf.sampleRate * 0.45f + samp[i].start);
            
            tSampler_setEnd(&samp[i],  end % buff.length); // 10 - 1010 ms
        }
    }
    
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
