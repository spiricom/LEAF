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

tWDF r1;
tWDF r2;
tWDF c1;
tWDF c2;
tWDF rs1;
tWDF p1;
tWDF s1;
tWDF s2;
tWDF i;
tWDF d;
tNoise noise;
tCycle sine;

float gain;
bool buttonState;
int ratio = 2;

void    LEAFTest_init            (float sampleRate, int blockSize)
{
    LEAF_init(sampleRate, blockSize, &getRandomFloat);
    
    tNoise_init(&noise, WhiteNoise);
    tCycle_init(&sine);
    tCycle_setFreq(&sine, 200);
    
    //Bandpass circuit 1
    //use c1 for output voltage
//    tWDF_init(&r1, Resistor, 10000.0f, NULL, NULL);
//    tWDF_init(&r2, Resistor, 10000.0f, NULL, NULL);
//    tWDF_init(&c1, Capacitor, 0.000000159154943f, NULL, NULL);
//    tWDF_init(&c2, Capacitor, 0.000000159154943f, NULL, NULL);
//    tWDF_init(&p1, ParallelAdaptor, 10000.0f, &r1, &c1);
//    tWDF_init(&s1, SeriesAdaptor, 0.0f, &c2, &r2);
//    tWDF_init(&s2, SeriesAdaptor, 0.0f, &s1, &p1);
    
    //Bandpass circuit 2
//    tWDF_init(&r1, Resistor, 10000.0f, NULL, NULL);
//    tWDF_init(&c1, Capacitor, 0.000000159154943f, NULL, NULL);
//    tWDF_init(&s1, SeriesAdaptor, 0.0f, &c1, &r1);
//    tWDF_init(&r2, Resistor, 10000.0f, NULL, NULL);
//    tWDF_init(&p1, ParallelAdaptor, 10000.0f, &s1, &r2);
//    tWDF_init(&c2, Capacitor, 0.000000159154943f, NULL, NULL);
//    tWDF_init(&s2, SeriesAdaptor, 0.0f, &c2, &p1);
    
//    tWDF_setOutputPoint(&s2, &c1);
    
//    tWDFNonlinear_init(&i, IdealSource, &s2);
    
    tWDF_init(&r1, Resistor, 10000.0f, NULL, NULL);
    tWDF_init(&rs1, ResistiveSource, 1.0f, NULL, NULL);
    tWDF_init(&s1, SeriesAdaptor, 0.0f, &r1, &rs1);
    tWDF_init(&d, Diode, 0.0f, &s1, NULL);
    
    leaf_pool_report();
}

float   LEAFTest_tick            (float input)
{
    float sample = tCycle_tick(&sine) * gain;
    
    //tWDF_setValue(&rs1, sample*gain);
    sample = tWDF_tick(&d, sample, &d, 1);
    
    return sample;
}



bool lastState = false, lastPlayState = false;
void    LEAFTest_block           (void)
{
    float val = getSliderValue("mod freq");
    val = 1.0f + 10000.0f * val;
    
    tWDF_setValue(&r1, val);
    
    val = getSliderValue("mod depth");
    val = 10.0f * val;
    gain = val;
    //tWDF_setValue(&r2, val);
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
