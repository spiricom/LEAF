/*==============================================================================

    leaf-string.c
    Created: 30 Nov 2018 10:41:42am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-string.h"

#else

#include "../Inc/leaf-string.h"

#endif

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tPluck ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void    tPluck_init         (tPluck* const p, float lowestFrequency)
{
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 10.0f;
    
    tNoise_init(&p->noise, WhiteNoise);
    
    tOnePole_init(&p->pickFilter, 0.0f);
    
    tOneZero_init(&p->loopFilter, 0.0f);
    
    tDelayA_init(&p->delayLine, 0.0f, leaf.sampleRate * 2);
    
    tPluck_setFrequency(p, 220.0f);
}

void tPluck_free (tPluck* const p)
{
    tNoise_free(&p->noise);
    tOnePole_free(&p->pickFilter);
    tOneZero_free(&p->loopFilter);
    tDelayA_free(&p->delayLine);

    leaf_free(p);
}

float   tPluck_getLastOut    (tPluck *p)
{
    return p->lastOut;
}

float   tPluck_tick          (tPluck *p)
{
    return (p->lastOut = 3.0f * tDelayA_tick(&p->delayLine, tOneZero_tick(&p->loopFilter, tDelayA_getLastOut(&p->delayLine) * p->loopGain ) ));
}

void    tPluck_pluck         (tPluck* const p, float amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    tOnePole_setPole(&p->pickFilter, 0.999f - (amplitude * 0.15f));
    tOnePole_setGain(&p->pickFilter, amplitude * 0.5f );
    
    // Fill delay with noise additively with current contents.
    for ( uint32_t i = 0; i < (uint32_t)tDelayA_getDelay(&p->delayLine); i++ )
        tDelayA_tick(&p->delayLine, 0.6f * tDelayA_getLastOut(&p->delayLine) + tOnePole_tick(&p->pickFilter, tNoise_tick(&p->noise) ) );
}

// Start a note with the given frequency and amplitude.;
void    tPluck_noteOn        (tPluck* const p, float frequency, float amplitude )
{
    p->lastFreq = frequency;
    tPluck_setFrequency( p, frequency );
    tPluck_pluck( p, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tPluck_noteOff       (tPluck* const p, float amplitude )
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tPluck_setFrequency  (tPluck* const p, float frequency )
{
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    // Delay = length - filter delay.
    float delay = ( leaf.sampleRate / frequency ) - tOneZero_getPhaseDelay(&p->loopFilter, frequency );
    
    tDelayA_setDelay(&p->delayLine, delay );
    
    p->loopGain = 0.99f + (frequency * 0.000005f);
    
    if ( p->loopGain >= 0.999f ) p->loopGain = 0.999f;
    
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tPluck_controlChange (tPluck* const p, int number, float value)
{
    return;
}

void tPluckSampleRateChanged(tPluck* const p)
{
    //tPluckSetFrequency(p, p->lastFreq);
}

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tStifKarp ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void    tStifKarp_init          (tStifKarp* const p, float lowestFrequency)
{
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 8.0f;
    
    tDelayA_init(&p->delayLine, 0.0f, leaf.sampleRate * 2);

    tDelayL_init(&p->combDelay, 0.0f, leaf.sampleRate * 2);
    
    tOneZero_init(&p->filter, 0.0f);
    
    tNoise_init(&p->noise, WhiteNoise);
    
    for (int i = 0; i < 4; i++)
    {
        tBiQuad_init(&p->biquad[i]);
    }
    
    p->pluckAmplitude = 0.3f;
    p->pickupPosition = 0.4f;
    
    p->stretching = 0.9999f;
    p->baseLoopGain = 0.995f;
    p->loopGain = 0.999f;
    
    tStifKarp_setFrequency( p, 220.0f );
    
}

void tStifKarp_free (tStifKarp* const p)
{
    tDelayA_free(&p->delayLine);
    tDelayL_free(&p->combDelay);
    tOneZero_free(&p->filter);
    tNoise_free(&p->noise);
    
    for (int i = 0; i < 4; i++)
    {
        tBiQuad_free(&p->biquad[i]);
    }
    
    leaf_free(p);
}

float   tStifKarp_getLastOut    (tStifKarp* const p)
{
    return p->lastOut;
}

float   tStifKarp_tick          (tStifKarp* const p)
{
    float temp = tDelayA_getLastOut(&p->delayLine) * p->loopGain;
    
    // Calculate allpass stretching.
    for (int i=0; i<4; i++)     temp = tBiQuad_tick(&p->biquad[i],temp);
    
    // Moving average filter.
    temp = tOneZero_tick(&p->filter, temp);
    
    float out = tDelayA_tick(&p->delayLine, temp);
    out = out - tDelayL_tick(&p->combDelay, out);
    p->lastOut = out;
    
    return p->lastOut;
}

void    tStifKarp_pluck         (tStifKarp* const p, float amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->pluckAmplitude = amplitude;
    
    for ( uint32_t i=0; i < (uint32_t)tDelayA_getDelay(&p->delayLine); i++ )
    {
        // Fill delay with noise additively with current contents.
        tDelayA_tick(&p->delayLine, (tDelayA_getLastOut(&p->delayLine) * 0.6f) + 0.4f * tNoise_tick(&p->noise) * p->pluckAmplitude );
        //delayLine_.tick( combDelay_.tick((delayLine_.lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude_) );
    }
}

// Start a note with the given frequency and amplitude.;
void    tStifKarp_noteOn        (tStifKarp* const p, float frequency, float amplitude )
{
    tStifKarp_setFrequency( p, frequency );
    tStifKarp_pluck( p, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tStifKarp_noteOff       (tStifKarp* const p, float amplitude )
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tStifKarp_setFrequency  (tStifKarp* const p, float frequency )
{
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    p->lastFrequency = frequency;
    p->lastLength = leaf.sampleRate / p->lastFrequency;
    float delay = p->lastLength - 0.5f;
    tDelayA_setDelay(&p->delayLine, delay);
    
    // MAYBE MODIFY LOOP GAINS
    p->loopGain = p->baseLoopGain + (frequency * 0.000005f);
    if (p->loopGain >= 1.0f) p->loopGain = 0.99999f;
    
    tStifKarp_setStretch(p, p->stretching);
    
    tDelayL_setDelay(&p->combDelay, 0.5f * p->pickupPosition * p->lastLength );
    
}

// Set the stretch "factor" of the string (0.0 - 1.0).
void    tStifKarp_setStretch         (tStifKarp* const p, float stretch )
{
    p->stretching = stretch;
    float coefficient;
    float freq = p->lastFrequency * 2.0f;
    float dFreq = ( (0.5f * leaf.sampleRate) - freq ) * 0.25f;
    float temp = 0.5f + (stretch * 0.5f);
    if ( temp > 0.9999f ) temp = 0.9999f;
    
    for ( int i=0; i<4; i++ )
    {
        coefficient = temp * temp;
        tBiQuad_setA2(&p->biquad[i], coefficient);
        tBiQuad_setB0(&p->biquad[i], coefficient);
        tBiQuad_setB2(&p->biquad[i], 1.0f);
        
        coefficient = -2.0f * temp * cos(TWO_PI * freq / leaf.sampleRate);
        tBiQuad_setA1(&p->biquad[i], coefficient);
        tBiQuad_setB1(&p->biquad[i], coefficient);
        
        freq += dFreq;
    }
}

// Set the pluck or "excitation" position along the string (0.0 - 1.0).
void    tStifKarp_setPickupPosition  (tStifKarp* const p, float position )
{
    if (position < 0.0f)        p->pickupPosition = 0.0f;
    else if (position <= 1.0f)  p->pickupPosition = position;
    else                        p->pickupPosition = 1.0f;
    
    tDelayL_setDelay(&p->combDelay, 0.5f * p->pickupPosition * p->lastLength);
}

// Set the base loop gain.
void    tStifKarp_setBaseLoopGain    (tStifKarp* const p, float aGain )
{
    p->baseLoopGain = aGain;
    p->loopGain = p->baseLoopGain + (p->lastFrequency * 0.000005f);
    if ( p->loopGain > 0.99999f ) p->loopGain = 0.99999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tStifKarp_controlChange (tStifKarp* const p, SKControlType type, float value)
{
    if ( value < 0.0f )         value = 0.0f;
    else if (value > 128.0f)   value = 128.0f;
    
    float normalizedValue = value * INV_128;
    
    if (type == SKPickPosition) // 4
        tStifKarp_setPickupPosition( p, normalizedValue );
    else if (type == SKStringDamping) // 11
        tStifKarp_setBaseLoopGain( p, 0.97f + (normalizedValue * 0.03f) );
    else if (type == SKDetune) // 1
        tStifKarp_setStretch( p, 0.91f + (0.09f * (1.0f - normalizedValue)) );
    
}

void    tStifKarpSampleRateChanged (tStifKarp* const c)
{
    tStifKarp_setFrequency(c, c->lastFrequency);
    tStifKarp_setStretch(c, c->stretching);
}
