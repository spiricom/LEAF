/*==============================================================================
 
 leaf-math.c
 Created: 22 Jan 2017 7:02:56pm
 
 ==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-math.h"
#include "..\Inc\leaf-tables.h"

#else

#include "../Inc/leaf-math.h"
#include "../Inc/leaf-tables.h"

#endif


#define EXPONENTIAL_TABLE_SIZE 65536

void LEAF_generate_sine(float* buffer, int size)
{
    float phase;
    for (int i = 0; i < size; i++)
    {
        phase = (float) i / (float) size;
        buffer[i] = sinf(phase * TWO_PI);
    }
}

void LEAF_generate_sawtooth(float* buffer, float basefreq, int size, LEAF* const leaf)
{
    int harmonic = 1;
    float phase = 0.0f;
    float freq = harmonic * basefreq;
    float amp;
    
    while (freq < (leaf->sampleRate * 0.5))
    {
        amp = 1.0f / harmonic;
        for (int i = 0; i < size; i++)
        {
            phase = (float) i / (float) size;
            buffer[i] += (amp * sinf(harmonic * phase * TWO_PI));
        }
        
        harmonic++;
        freq = harmonic * basefreq;
    }
}


void LEAF_generate_triangle(float* buffer, float basefreq, int size, LEAF* const leaf)
{
    int harmonic = 1;
    float phase = 0.0f;
    float freq = harmonic * basefreq;
    float amp = 1.0f;
    
    int count = 0;
    float mult = 1.0f;
    
    while (freq < (leaf->sampleRate * 0.5))
    {
        amp = 1.0f / (float)(harmonic * harmonic);
        
        if (count % 2)  mult = -1.0f;
        else            mult =  1.0f;
        
        for (int i = 0; i < size; i++)
        {
            phase = (float) i / (float) size;
            buffer[i] += (mult * amp * sinf(harmonic * phase * TWO_PI));
        }
        
        count++;
        harmonic += 2;
        freq = harmonic * basefreq;
    }
}

void LEAF_generate_square(float* buffer, float basefreq, int size, LEAF* const leaf)
{
    int harmonic = 1;
    float phase = 0.0f;
    float freq = harmonic * basefreq;
    float amp = 1.0f;
    
    while (freq < (leaf->sampleRate * 0.5))
    {
        amp = 1.0f / (float)(harmonic);
        
        for (int i = 0; i < size; i++)
        {
            phase = (float) i / (float) size;
            buffer[i] += (amp * sinf(harmonic * phase * TWO_PI));
        }
        
        harmonic += 2;
        freq = harmonic * basefreq;
    }
}


//0.001 base gives a good curve that goes from 1 to near zero
void LEAF_generate_exp(float* buffer, float base, float start, float end, float offset, int size)
{
    float increment = (end - start) / (float)(size-1);
    float x = start;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = powf(base, x) + offset;
        x += increment;
    }
}


void LEAF_generate_table_skew_non_sym_double(float* buffer, float start, float end, float center, int size)
{
    double skew = log (0.5) / log ((center - start) / (end - start));
    double increment = 1.0 / (double)(size-1);
    double x = 0.0000000001;
    double proportion = 0.0;
    for (int i = 0; i < size; i++)
    {
        proportion = exp (log(x) / skew);
        buffer[i] = (float)(start + (end - start) * proportion);
        x += increment;
    }
}


void LEAF_generate_table_skew_non_sym(float* buffer, float start, float end, float center, int size)
{
    float skew = logf (0.5) / logf ((center - start) / (end - start));
    float increment = 1.0f / (float)(size-1);
    float x = 0.0000000001f;
    float proportion = 0.0;
    for (int i = 0; i < size; i++)
    {
        proportion = expf (logf(x) / skew);
        buffer[i] = (float)(start + (end - start) * proportion);
        x += increment;
    }
}


void LEAF_generate_atodb(float* buffer, int size, float min, float max)
{
    float increment = (max-min) / (float)(size-1);
    float x = min;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = atodb(x);
        x += increment;
    }
}

void LEAF_generate_dbtoa(float* buffer, int size, float minDb, float maxDb)
{
    float increment = (maxDb-minDb) / (float)(size-1);
    float x = minDb;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = dbtoa(x);
        x += increment;
    }
}


void LEAF_generate_atodbPositiveClipped(float* buffer, float lowerThreshold, float range, int size)
{
    float increment = 1.0f / (float)(size-1);
    float x = 0.000000001f;
    float scalar = range / fastabsf(lowerThreshold);
    for (int i = 0; i < size; i++)
    {
        float temp = atodb(x);
        temp = LEAF_clip(lowerThreshold, temp, 0.0f);
        buffer[i] = (temp-lowerThreshold) * scalar;
        x += increment;
    }
}


void LEAF_generate_mtof(float* buffer, float startMIDI, float endMIDI, int size)
{
    float increment = 1.0f / (float)(size-1);
    float x = 0.0f;
    float scalar = (endMIDI-startMIDI);
    for (int i = 0; i < size; i++)
    {
        float midiVal = (x * scalar) + startMIDI;
        buffer[i] = mtof(midiVal);
        x += increment;
    }
}

void LEAF_generate_ftom(float* buffer, float startFreq, float endFreq, int size)
{
    float increment = 1.0f / (float)(size-1);
    float x = 0.0f;
    float scalar = (endFreq-startFreq);
    for (int i = 0; i < size; i++)
    {
        float midiVal = (x * scalar) + startFreq;
        buffer[i] = ftom(midiVal);
        x += increment;
    }
}

//not sure that this works
float fast_sinf2(float x)
{
    float invert = 1.0f;
    float out;

    x = x * INV_TWO_PI_TIMES_SINE_TABLE_SIZE;

    if (x < 0.0f)
    {
        x *= -1.0f;
        invert = -1.0f;
    }
    int intX = ((int)x) & 2047;
    int intXNext = (intX + 1) & 2047;
    float floatX = x-intX;

    out = __leaf_table_sinewave[intX] * (1.0f - floatX);
    out += __leaf_table_sinewave[intXNext] * floatX;
    out *= invert;
    return out;
}
#if LEAF_INCLUDE_MINBLEP_TABLES
/// MINBLEPS
// https://github.com/MrBlueXav/Dekrispator_v2 blepvco.c
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) place_step_dd(float *buffer, int index, float phase, float w, float scale)
#else
void place_step_dd(float *buffer, int index, float phase, float w, float scale)
#endif
{
    float r;
    long i;

    r = MINBLEP_PHASES * phase / w;
    i = lrintf(r - 0.5f);
    r -= (float)i;
    i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */

    while (i < MINBLEP_PHASES * STEP_DD_PULSE_LENGTH) {
        buffer[index] += scale * (step_dd_table[i].value + r * step_dd_table[i].delta);
        i += MINBLEP_PHASES;
        index++;
    }
}




#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) place_slope_dd(float *buffer, int index, float phase, float w, float slope_delta)
#else
void place_slope_dd(float *buffer, int index, float phase, float w, float slope_delta)
#endif
{
    float r;
    long i;

    r = MINBLEP_PHASES * phase / w;
    i = lrintf(r - 0.5f);
    r -= (float)i;
    i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */

    slope_delta *= w;

    while (i < MINBLEP_PHASES * SLOPE_DD_PULSE_LENGTH) {
        buffer[index] += slope_delta * (slope_dd_table[i] + r * (slope_dd_table[i + 1] - slope_dd_table[i]));
        i += MINBLEP_PHASES;
        index++;
    }
}
#endif // LEAF_INCLUDE_MINBLEP_TABLES


    /*! @} */

