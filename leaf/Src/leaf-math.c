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

void LEAF_generate_sine(Lfloat* buffer, int size)
{
    Lfloat phase;
    for (int i = 0; i < size; i++)
    {
        phase = (Lfloat) i / (Lfloat) size;
        buffer[i] = sinf(phase * TWO_PI);
    }
}

void LEAF_generate_sawtooth(Lfloat* buffer, Lfloat basefreq, int size, LEAF* const leaf)
{
    int harmonic = 1;
    Lfloat phase = 0.0f;
    Lfloat freq = harmonic * basefreq;
    Lfloat amp;
    
    while (freq < (leaf->sampleRate * 0.5))
    {
        amp = 1.0f / harmonic;
        for (int i = 0; i < size; i++)
        {
            phase = (Lfloat) i / (Lfloat) size;
            buffer[i] += (amp * sinf(harmonic * phase * TWO_PI));
        }
        
        harmonic++;
        freq = harmonic * basefreq;
    }
}


void LEAF_generate_triangle(Lfloat* buffer, Lfloat basefreq, int size, LEAF* const leaf)
{
    int harmonic = 1;
    Lfloat phase = 0.0f;
    Lfloat freq = harmonic * basefreq;
    Lfloat amp = 1.0f;
    
    int count = 0;
    Lfloat mult = 1.0f;
    
    while (freq < (leaf->sampleRate * 0.5))
    {
        amp = 1.0f / (Lfloat)(harmonic * harmonic);
        
        if (count % 2)  mult = -1.0f;
        else            mult =  1.0f;
        
        for (int i = 0; i < size; i++)
        {
            phase = (Lfloat) i / (Lfloat) size;
            buffer[i] += (mult * amp * sinf(harmonic * phase * TWO_PI));
        }
        
        count++;
        harmonic += 2;
        freq = harmonic * basefreq;
    }
}

void LEAF_generate_square(Lfloat* buffer, Lfloat basefreq, int size, LEAF* const leaf)
{
    int harmonic = 1;
    Lfloat phase = 0.0f;
    Lfloat freq = harmonic * basefreq;
    Lfloat amp = 1.0f;
    
    while (freq < (leaf->sampleRate * 0.5))
    {
        amp = 1.0f / (Lfloat)(harmonic);
        
        for (int i = 0; i < size; i++)
        {
            phase = (Lfloat) i / (Lfloat) size;
            buffer[i] += (amp * sinf(harmonic * phase * TWO_PI));
        }
        
        harmonic += 2;
        freq = harmonic * basefreq;
    }
}


//0.001 base gives a good curve that goes from 1 to near zero
void LEAF_generate_exp(Lfloat* buffer, Lfloat base, Lfloat start, Lfloat end, Lfloat offset, int size)
{
    Lfloat increment = (end - start) / (Lfloat)(size-1);
    Lfloat x = start;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = powf(base, x) + offset;
        x += increment;
    }
}


void LEAF_generate_table_skew_non_sym_double(Lfloat* buffer, Lfloat start, Lfloat end, Lfloat center, int size)
{
    double skew = log (0.5) / log ((center - start) / (end - start));
    double increment = 1.0 / (double)(size-1);
    double x = 0.0000000001;
    double proportion = 0.0;
    for (int i = 0; i < size; i++)
    {
        proportion = exp (log(x) / skew);
        buffer[i] = (Lfloat)(start + (end - start) * proportion);
        x += increment;
    }
}


void LEAF_generate_table_skew_non_sym(Lfloat* buffer, Lfloat start, Lfloat end, Lfloat center, int size)
{
    Lfloat skew = logf (0.5) / logf ((center - start) / (end - start));
    Lfloat increment = 1.0f / (Lfloat)(size-1);
    Lfloat x = 0.0000000001f;
    Lfloat proportion = 0.0;
    for (int i = 0; i < size; i++)
    {
        proportion = expf (logf(x) / skew);
        buffer[i] = (Lfloat)(start + (end - start) * proportion);
        x += increment;
    }
}


void LEAF_generate_atodb(Lfloat* buffer, int size, Lfloat min, Lfloat max)
{
    Lfloat increment = (max-min) / (Lfloat)(size-1);
    Lfloat x = min;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = atodb(x);
        x += increment;
    }
}

void LEAF_generate_dbtoa(Lfloat* buffer, int size, Lfloat minDb, Lfloat maxDb)
{
    Lfloat increment = (maxDb-minDb) / (Lfloat)(size-1);
    Lfloat x = minDb;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = dbtoa(x);
        x += increment;
    }
}


void LEAF_generate_atodbPositiveClipped(Lfloat* buffer, Lfloat lowerThreshold, Lfloat range, int size)
{
    Lfloat increment = 1.0f / (Lfloat)(size-1);
    Lfloat x = 0.000000001f;
    Lfloat scalar = range / fastabsf(lowerThreshold);
    for (int i = 0; i < size; i++)
    {
        Lfloat temp = atodb(x);
        temp = LEAF_clip(lowerThreshold, temp, 0.0f);
        buffer[i] = (temp-lowerThreshold) * scalar;
        x += increment;
    }
}


void LEAF_generate_mtof(Lfloat* buffer, Lfloat startMIDI, Lfloat endMIDI, int size)
{
    Lfloat increment = 1.0f / (Lfloat)(size-1);
    Lfloat x = 0.0f;
    Lfloat scalar = (endMIDI-startMIDI);
    for (int i = 0; i < size; i++)
    {
        Lfloat midiVal = (x * scalar) + startMIDI;
        buffer[i] = mtof(midiVal);
        x += increment;
    }
}

void LEAF_generate_ftom(Lfloat* buffer, Lfloat startFreq, Lfloat endFreq, int size)
{
    Lfloat increment = 1.0f / (Lfloat)(size-1);
    Lfloat x = 0.0f;
    Lfloat scalar = (endFreq-startFreq);
    for (int i = 0; i < size; i++)
    {
        Lfloat midiVal = (x * scalar) + startFreq;
        buffer[i] = ftom(midiVal);
        x += increment;
    }
}

//not sure that this works
float fast_sinf2(Lfloat x)
{
    Lfloat invert = 1.0f;
    Lfloat out;

    x = x * INV_TWO_PI_TIMES_SINE_TABLE_SIZE;

    if (x < 0.0f)
    {
        x *= -1.0f;
        invert = -1.0f;
    }
    int intX = ((int)x) & 2047;
    int intXNext = (intX + 1) & 2047;
    Lfloat LfloatX = x-intX;

    out = __leaf_table_sinewave[intX] * (1.0f - LfloatX);
    out += __leaf_table_sinewave[intXNext] * LfloatX;
    out *= invert;
    return out;
}
#if LEAF_INCLUDE_MINBLEP_TABLES
/// MINBLEPS
// https://github.com/MrBlueXav/Dekrispator_v2 blepvco.c
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) place_step_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat scale)
#else
void place_step_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat scale)
#endif
{
    Lfloat r;
    long i;

    r = MINBLEP_PHASES * phase / w;
    i = lrintf(r - 0.5f);
    r -= (Lfloat)i;
    i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */

    while (i < MINBLEP_PHASES * STEP_DD_PULSE_LENGTH) {
        buffer[index] += scale * (step_dd_table[i].value + r * step_dd_table[i].delta);
        i += MINBLEP_PHASES;
        index++;
    }
}




#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) place_slope_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat slope_delta)
#else
void place_slope_dd(Lfloat *buffer, int index, float phase, float w, Lfloat slope_delta)
#endif
{
    Lfloat r;
    long i;

    r = MINBLEP_PHASES * phase / w;
    i = lrintf(r - 0.5f);
    r -= (Lfloat)i;
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

