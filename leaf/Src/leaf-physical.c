/*==============================================================================

    leaf-string.c
    Created: 30 Nov 2018 10:41:42am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-physical.h"

#else

#include "../Inc/leaf-physical.h"

#endif

#ifdef ARM_MATH_CM7
#include <arm_math.h>
#endif

float   pickupNonLinearity          (float x)
{

	x = x * 2.0f;
	float out = (0.075f * x) + (0.00675f * x * x) +( 0.00211f * x * x * x) + (0.000475f * x * x * x * x) + (0.000831f * x * x * x * x *x);
	out *= 4.366812227074236f;
	return out;
}


void tPickupNonLinearity_create(tMempool** const mp, tPickupNonLinearity** const pl)
{
    ALLOC_FROM_POOL(tPickupNonLinearity, pl, mp);
}
void tPickupNonLinearity_init(LEAF* const leaf, tPickupNonLinearity* const p)
{

p->prev = 0.0f;

}
void   tPickupNonLinearity_free          (tPickupNonLinearity** const pl)
{
	tPickupNonLinearity* p = *pl;
	mpool_free((char*)p, p->mempool);
}
float   tPickupNonLinearity_tick          (tPickupNonLinearity* const p, float x)
{
	x = x * 2.0f;
	float out = (0.075f * x) + (0.00675f * x * x) +( 0.00211f * x * x * x) + (0.000475f * x * x * x * x) + (0.000831f * x * x * x * x *x);
	out *= 4.366812227074236f;
	float y = out - p->prev;
	return y;
}

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tPluck ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void tPluck_create(tMempool** const mp, tPluck** const pl)
{
    ALLOC_FROM_POOL(tPluck, pl, mp);
}

void tPluck_init(LEAF* const leaf, tPluck* const p, float lowestFrequency)
{


    p->sampleRate = leaf->sampleRate;

    if ( lowestFrequency <= 0.0f )  lowestFrequency = 10.0f;


    tAllpassDelay_create(&p->mempool, &p->delayLine);

    tAllpassDelay_init(leaf,p->delayLine, 0.0f, p->sampleRate * 2);
    tAllpassDelay_clear(p->delayLine);

    tPluck_setFrequency(p, 220.0f);

}

void    tPluck_free (tPluck** const pl)
{
    tPluck* p = *pl;


    tAllpassDelay_free(&p->delayLine);

    mpool_free((char*)p, p->mempool);
}

float   tPluck_getLastOut    (tPluck* const p)
{
    return p->lastOut;
}

float   tPluck_tick          (tPluck* const p)
{
    return (p->lastOut = 3.0f * tAllpassDelay_tick(p->delayLine, tOneZero_tick(&p->loopFilter, tAllpassDelay_getLastOut(p->delayLine) * p->loopGain ) ));
}

void    tPluck_pluck         (tPluck* const p, float amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;

    tOnePole_setPole(&p->pickFilter, 0.999f - (amplitude * 0.15f));
    tOnePole_setGain(&p->pickFilter, amplitude * 0.5f );

    // Fill delay with noise additively with current contents.
    for ( uint32_t i = 0; i < (uint32_t)tAllpassDelay_getDelay(p->delayLine); i++ )
        tAllpassDelay_tick(p->delayLine, 0.6f * tAllpassDelay_getLastOut(p->delayLine) + tOnePole_tick(&p->pickFilter, tNoise_tick(&p->noise) ) );
}

// Start a note with the given frequency and amplitude.;
void    tPluck_noteOn        (tPluck* const p, float frequency, float amplitude )
{
    p->lastFreq = frequency;
    tPluck_setFrequency(p, frequency );
    tPluck_pluck(p, amplitude );
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
    float delay = ( p->sampleRate / frequency ) - tOneZero_getPhaseDelay(&p->loopFilter, frequency );

    tAllpassDelay_setDelay(p->delayLine, delay );

    p->loopGain = 0.99f + (frequency * 0.000005f);

    if (p->loopGain >= 0.999f ) p->loopGain = 0.999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tPluck_controlChange (tPluck* const p, int number, float value)
{
    return;
}

void tPluck_setSampleRate(LEAF* const leaf, tPluck* const p, float sr)
{
    p->sampleRate = sr;

    tAllpassDelay_free(&p->delayLine);
    tAllpassDelay_create(&p->mempool, &p->delayLine);
    tAllpassDelay_init(leaf, p->delayLine, 0.0f, p->sampleRate * 2);
    tAllpassDelay_clear(p->delayLine);

    tPluck_setFrequency(p, p->lastFreq);
    tOnePole_setSampleRate(&p->pickFilter, p->sampleRate);
    tOneZero_setSampleRate(&p->loopFilter, p->sampleRate);
}

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tKarplusStrong ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void tKarplusStrong_create(tMempool** const mp, tKarplusStrong** const pl)
{
    ALLOC_FROM_POOL(tKarplusStrong, pl, mp);
}

void tKarplusStrong_init(LEAF* const leaf, tKarplusStrong* const p, float lowestFrequency)
{


    p->sampleRate = leaf->sampleRate;

    if ( lowestFrequency <= 0.0f )  lowestFrequency = 8.0f;
    tAllpassDelay_create(&p->mempool, &p->delayLine);
    tAllpassDelay_init(leaf,p->delayLine, 0.0f, p->sampleRate * 2);
    tAllpassDelay_clear(p->delayLine);

    tLinearDelay_create(&p->mempool,&p->combDelay);
    tLinearDelay_init(leaf,p->combDelay, 0.0f, p->sampleRate * 2);
    tLinearDelay_clear(p->combDelay);

    tOneZero_init(leaf,&p->filter, 0.0f);

    tNoise_init(leaf,&p->noise, WhiteNoise);

    for (int i = 0; i < 4; i++)
    {
        tBiQuad_init(leaf, &p->biquad[i]);

    }

    p->pluckAmplitude = 0.3f;
    p->pickupPosition = 0.4f;

    p->stretching = 0.9999f;
    p->baseLoopGain = 0.995f;
    p->loopGain = 0.999f;

    tKarplusStrong_setFrequency( p, 220.0f );
}

void    tKarplusStrong_free (tKarplusStrong** const pl)
{
    tKarplusStrong* p = *pl;

    tAllpassDelay_free(&p->delayLine);
    tLinearDelay_free(&p->combDelay);

    mpool_free((char*)p, p->mempool);
}

float   tKarplusStrong_getLastOut    (tKarplusStrong* const p)
{
    return p->lastOut;
}

float   tKarplusStrong_tick          (tKarplusStrong* const p)
{
    float temp = tAllpassDelay_getLastOut(p->delayLine) * p->loopGain;

    // Calculate allpass stretching.
    for (int i=0; i<4; i++)     temp = tBiQuad_tick(&p->biquad[i],temp);

    // Moving average filter.
    temp = tOneZero_tick(&p->filter, temp);

    float out = tAllpassDelay_tick(p->delayLine, temp);
    out = out - tLinearDelay_tick(p->combDelay, out);
    p->lastOut = out;

    return p->lastOut;
}

void    tKarplusStrong_pluck         (tKarplusStrong* const p, float amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;

    p->pluckAmplitude = amplitude;

    for ( uint32_t i=0; i < (uint32_t)tAllpassDelay_getDelay(p->delayLine); i++ )
    {
        // Fill delay with noise additively with current contents.
        tAllpassDelay_tick(p->delayLine, (tAllpassDelay_getLastOut(p->delayLine) * 0.6f) + 0.4f * tNoise_tick(&p->noise) * p->pluckAmplitude );
        //delayLine_.tick( combDelay_.tick((delayLine_.lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude_) );
    }
}

// Start a note with the given frequency and amplitude.;
void    tKarplusStrong_noteOn        (tKarplusStrong* const p, float frequency, float amplitude )
{
    tKarplusStrong_setFrequency(p, frequency );
    tKarplusStrong_pluck(p, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tKarplusStrong_noteOff       (tKarplusStrong* const p, float amplitude )
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;

    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tKarplusStrong_setFrequency  (tKarplusStrong* const p, float frequency )
{
    if ( frequency <= 0.0f )   frequency = 0.001f;

    p->lastFrequency = frequency;
    p->lastLength = p->sampleRate / p->lastFrequency;
    float delay = p->lastLength - 0.5f;
    tAllpassDelay_setDelay(p->delayLine, delay);

    // MAYBE MODIFY LOOP GAINS
    p->loopGain = p->baseLoopGain + (frequency * 0.000005f);
    if (p->loopGain >= 1.0f) p->loopGain = 0.99999f;

    tKarplusStrong_setStretch(p, p->stretching);

    tLinearDelay_setDelay(p->combDelay, 0.5f * p->pickupPosition * p->lastLength);
}

// Set the stretch "factor" of the string (0.0 - 1.0).
void    tKarplusStrong_setStretch         (tKarplusStrong* const p, float stretch)
{
    p->stretching = stretch;
    float coefficient;
    float freq = p->lastFrequency * 2.0f;
    float dFreq = ( (0.5f * p->sampleRate) - freq ) * 0.25f;
    float temp = 0.5f + (stretch * 0.5f);
    if ( temp > 0.9999f ) temp = 0.9999f;

    for ( int i=0; i<4; i++ )
    {
        coefficient = temp * temp;
        tBiQuad_setA2(&p->biquad[i], coefficient);
        tBiQuad_setB0(&p->biquad[i], coefficient);
        tBiQuad_setB2(&p->biquad[i], 1.0f);

        coefficient = -2.0f * temp * cosf(TWO_PI * freq / p->sampleRate);
        tBiQuad_setA1(&p->biquad[i], coefficient);
        tBiQuad_setB1(&p->biquad[i], coefficient);

        freq += dFreq;
    }
}

// Set the pluck or "excitation" position along the string (0.0 - 1.0).
void    tKarplusStrong_setPickupPosition  (tKarplusStrong* const p, float position )
{
    if (position < 0.0f)        p->pickupPosition = 0.0f;
    else if (position <= 1.0f)  p->pickupPosition = position;
    else                        p->pickupPosition = 1.0f;

    tLinearDelay_setDelay(p->combDelay, 0.5f * p->pickupPosition * p->lastLength);
}

// Set the base loop gain.
void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong* const p, float aGain )
{
    p->baseLoopGain = aGain;
    p->loopGain = p->baseLoopGain + (p->lastFrequency * 0.000005f);
    if ( p->loopGain > 0.99999f ) p->loopGain = 0.99999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tKarplusStrong_controlChange (tKarplusStrong* const p, SKControlType type, float value)
{
    if ( value < 0.0f )         value = 0.0f;
    else if (value > 128.0f)   value = 128.0f;

    float normalizedValue = value * INV_128;

    if (type == SKPickPosition) // 4
        tKarplusStrong_setPickupPosition(p, normalizedValue );
    else if (type == SKStringDamping) // 11
        tKarplusStrong_setBaseLoopGain(p, 0.97f + (normalizedValue * 0.03f) );
    else if (type == SKDetune) // 1
        tKarplusStrong_setStretch(p, 0.91f + (0.09f * (1.0f - normalizedValue)) );
}

void    tKarplusStrong_setSampleRate (LEAF * const leaf, tKarplusStrong* const p, float sr)
{
    p->sampleRate = sr;

    tAllpassDelay_free(&p->delayLine);
    tAllpassDelay_create(&p->mempool,&p->delayLine);
    tAllpassDelay_init(leaf,p->delayLine, 0.0f, p->sampleRate * 2);
    tAllpassDelay_clear(p->delayLine);

    tLinearDelay_free(&p->combDelay);
    tLinearDelay_create(&p->mempool,&p->combDelay);
    tLinearDelay_init(leaf, p->combDelay, 0.0f, p->sampleRate * 2);
    tLinearDelay_clear(p->combDelay);

    tKarplusStrong_setFrequency(p, p->lastFrequency);
    tOneZero_setSampleRate(&p->filter, p->sampleRate);

    for (int i = 0; i < 4; i++)
    {
        tBiQuad_setSampleRate(&p->biquad[i], p->sampleRate);
    }
}

/* Simple Living String*/

void tSimpleLivingString_create(tMempool** const mp, tSimpleLivingString** const pl)
{
    ALLOC_FROM_POOL(tSimpleLivingString, pl, mp);
}

void tSimpleLivingString_init(LEAF* const leaf, tSimpleLivingString* const p, float freq, float dampFreq, float decay, float targetLev, float levSmoothFactor, float levStrength, int levMode)
{


    p->sampleRate = leaf->sampleRate;
    p->curr=0.0f;
    tExpSmooth_init(leaf, &p->wlSmooth, p->sampleRate/freq, 0.01f); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tSimpleLivingString_setFreq(p, freq);
    tLinearDelay_create(&p->mempool,&p->delayLine);
    tLinearDelay_init(leaf,p->delayLine,p->waveLengthInSamples, 2400);
    tLinearDelay_clear(p->delayLine);
    tOnePole_init(leaf,&p->bridgeFilter, dampFreq);
    tHighpass_init(leaf,&p->DCblocker,13);
    p->decay=decay;
    tFeedbackLeveler_init(leaf,&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode);
    p->levMode=levMode;

}

void    tSimpleLivingString_free (tSimpleLivingString** const pl)
{
    tSimpleLivingString* p = *pl;

    tLinearDelay_free(&p->delayLine);

    mpool_free((char*)p, p->mempool);
}

void     tSimpleLivingString_setFreq(tSimpleLivingString* const p, float freq)
{
    if (freq<20) freq=20;
    else if (freq>10000) freq=10000;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString_setWaveLength(tSimpleLivingString* const p, float waveLength)
{
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>2400) waveLength=2400;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString_setDampFreq(tSimpleLivingString* const p, float dampFreq)
{
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString_setDecay(tSimpleLivingString* const p, float decay)
{
    p->decay=decay;
}

void     tSimpleLivingString_setTargetLev(tSimpleLivingString* const p, float targetLev)
{
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
}

void     tSimpleLivingString_setLevSmoothFactor(tSimpleLivingString* const p, float levSmoothFactor)
{
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString_setLevStrength(tSimpleLivingString* const p, float levStrength)
{
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
}

void     tSimpleLivingString_setLevMode(tSimpleLivingString* const p, int levMode)
{
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    p->levMode=levMode;
}

float   tSimpleLivingString_tick(tSimpleLivingString* const p, float input)
{
    float stringOut=tOnePole_tick(&p->bridgeFilter,tLinearDelay_tickOut(p->delayLine));
    float stringInput=tHighpass_tick(&p->DCblocker, tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    tLinearDelay_tickIn(p->delayLine, stringInput);
    tLinearDelay_setDelay(p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    p->curr = stringOut;
    return p->curr;
}

float   tSimpleLivingString_sample(tSimpleLivingString* const p)
{
    return p->curr;
}

void   tSimpleLivingString_setSampleRate(tSimpleLivingString* const p, float sr)
{
    float freq = p->sampleRate/p->waveLengthInSamples;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
}


/* Simple Living String 2*/

void tSimpleLivingString2_create(tMempool** const mp, tSimpleLivingString2** const pl)
{
    ALLOC_FROM_POOL(tSimpleLivingString2, pl, mp);
}

void tSimpleLivingString2_init(LEAF* const leaf, tSimpleLivingString2* const p,
                              float freq, float brightness, float decay,
                              float targetLev, float levSmoothFactor, float levStrength, int levMode)
{
    p->sampleRate = leaf->sampleRate;
    p->curr = 0.0f;

    tExpSmooth_init(leaf, &p->wlSmooth, p->sampleRate / freq, 0.01f); // smoother for wavelength
    tSimpleLivingString2_setFreq(p, freq);

    tHermiteDelay_create(&p->mempool, &p->delayLine);
    tHermiteDelay_init(leaf, p->delayLine, p->waveLengthInSamples, 2400);
    tHermiteDelay_clear(p->delayLine);

    tTwoZero_init(leaf, &p->bridgeFilter);
    tSimpleLivingString2_setBrightness(p, brightness);

    tHighpass_init(leaf, &p->DCblocker, 13);

    p->decay = decay;

    tFeedbackLeveler_init(leaf, &p->fbLev, targetLev, levSmoothFactor, levStrength, levMode);
    p->levMode = levMode;
}

void tSimpleLivingString2_free(tSimpleLivingString2** const pl)
{
    tSimpleLivingString2* p = *pl;

    tHermiteDelay_free(&p->delayLine);

    mpool_free((char*)p, p->mempool);
}

void tSimpleLivingString2_setFreq(tSimpleLivingString2* const p, float freq)
{
    if (freq < 20) freq = 20;
    else if (freq > 10000) freq = 10000;

    p->freq = freq;
    p->waveLengthInSamples = (p->sampleRate / freq) - 1.0f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void tSimpleLivingString2_setWaveLength(tSimpleLivingString2* const p, float waveLength)
{
    if (waveLength < 4.8f) waveLength = 4.8f;
    else if (waveLength > 2400) waveLength = 2400;

    p->waveLengthInSamples = waveLength - 1.0f;
    p->freq = p->sampleRate / (p->waveLengthInSamples + 1.0f);
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void tSimpleLivingString2_setBrightness(tSimpleLivingString2* const p, float brightness)
{
    p->brightness = brightness;

    float h0 = (1.0f + brightness) * 0.5f;
    float h1 = (1.0f - brightness) * 0.25f;
    tTwoZero_setCoefficients(&p->bridgeFilter, h1, h0, h1);
}

void tSimpleLivingString2_setDecay(tSimpleLivingString2* const p, float decay)
{
    p->decay = decay;
}

void tSimpleLivingString2_setTargetLev(tSimpleLivingString2* const p, float targetLev)
{
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
}

void tSimpleLivingString2_setLevSmoothFactor(tSimpleLivingString2* const p, float levSmoothFactor)
{
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
}

void tSimpleLivingString2_setLevStrength(tSimpleLivingString2* const p, float levStrength)
{
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
}

void tSimpleLivingString2_setLevMode(tSimpleLivingString2* const p, int levMode)
{
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    p->levMode = levMode;
}

float tSimpleLivingString2_tick(tSimpleLivingString2* const p, float input)
{
    float stringOut = tTwoZero_tick(&p->bridgeFilter, tHermiteDelay_tickOut(p->delayLine));
    float stringInput = tHighpass_tick(&p->DCblocker,
                        tFeedbackLeveler_tick(&p->fbLev, (p->levMode == 0 ? p->decay * stringOut : stringOut) + input));
    tHermiteDelay_tickIn(p->delayLine, stringInput);
    tHermiteDelay_setDelay(p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    p->curr = stringOut;
    return p->curr;
}

float tSimpleLivingString2_sample(tSimpleLivingString2* const p)
{
    return p->curr;
}

void tSimpleLivingString2_setSampleRate(tSimpleLivingString2* const p, float sr)
{
    float freq = p->sampleRate / (p->waveLengthInSamples + 1.0f);

    p->sampleRate = sr;
    p->waveLengthInSamples = (p->sampleRate / freq) - 1.0f;

    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tTwoZero_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
}


/* Living String*/

void tLivingString_create(tMempool** const mp, tLivingString** const pl)
{
    ALLOC_FROM_POOL(tLivingString, pl, mp);
}

void tLivingString_init(LEAF* const leaf, tLivingString* const p,
                        float freq, float pickPos, float prepIndex,
                        float dampFreq, float decay,
                        float targetLev, float levSmoothFactor, float levStrength, int levMode)
{
    p->sampleRate = leaf->sampleRate;
    p->curr = 0.0f;

    tExpSmooth_init(leaf, &p->wlSmooth, p->sampleRate / freq, 0.01f); // smoother for wavelength
    tLivingString_setFreq(p, freq);
    p->freq = freq;

    tExpSmooth_init(leaf, &p->ppSmooth, pickPos, 0.01f); // smoother for pick position
    tLivingString_setPickPos(p, pickPos);

    p->prepIndex = prepIndex;

    tLinearDelay_create(&p->mempool, &p->delLF);
    tLinearDelay_create(&p->mempool, &p->delUF);
    tLinearDelay_create(&p->mempool, &p->delUB);
    tLinearDelay_create(&p->mempool, &p->delLB);

    tLinearDelay_init(leaf, p->delLF, p->waveLengthInSamples, 2400);
    tLinearDelay_init(leaf, p->delUF, p->waveLengthInSamples, 2400);
    tLinearDelay_init(leaf, p->delUB, p->waveLengthInSamples, 2400);
    tLinearDelay_init(leaf, p->delLB, p->waveLengthInSamples, 2400);

    tLinearDelay_clear(p->delLF);
    tLinearDelay_clear(p->delUF);
    tLinearDelay_clear(p->delUB);
    tLinearDelay_clear(p->delLB);

    p->dampFreq = dampFreq;

    tOnePole_init(leaf, &p->bridgeFilter, dampFreq);
    tOnePole_init(leaf, &p->nutFilter, dampFreq);
    tOnePole_init(leaf, &p->prepFilterU, dampFreq);
    tOnePole_init(leaf, &p->prepFilterL, dampFreq);

    tHighpass_init(leaf, &p->DCblockerU, 13);
    tHighpass_init(leaf, &p->DCblockerL, 13);

    p->decay = decay;

    tFeedbackLeveler_init(leaf, &p->fbLevU, targetLev, levSmoothFactor, levStrength, levMode);
    tFeedbackLeveler_init(leaf, &p->fbLevL, targetLev, levSmoothFactor, levStrength, levMode);

    p->levMode = levMode;
}

void tLivingString_free(tLivingString** const pl)
{
    tLivingString* p = *pl;

    tLinearDelay_free(&p->delLF);
    tLinearDelay_free(&p->delUF);
    tLinearDelay_free(&p->delUB);
    tLinearDelay_free(&p->delLB);

    mpool_free((char*)p, p->mempool);
}

void     tLivingString_setFreq(tLivingString* const p, float freq)
{    // NOTE: It is faster to set wavelength in samples directly
    if (freq<20.f) freq=20.f;
    else if (freq>10000.f) freq=10000.f;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString_setWaveLength(tLivingString* const p, float waveLength)
{
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.f) waveLength=2400.f;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString_setPickPos(tLivingString* const p, float pickPos)
{    // between 0 and 1
    if (pickPos<0.f) pickPos=0.f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(&p->ppSmooth, p->pickPos);
}

void     tLivingString_setPrepIndex(tLivingString* const p, float prepIndex)
{    // between 0 and 1
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tLivingString_setDampFreq(tLivingString* const p, float dampFreq)
{
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    tOnePole_setFreq(&p->nutFilter, dampFreq);
    tOnePole_setFreq(&p->prepFilterU, dampFreq);
    tOnePole_setFreq(&p->prepFilterL, dampFreq);
}

void     tLivingString_setDecay(tLivingString* const p, float decay)
{
    p->decay=decay;
}
void     tLivingString_setTargetLev(tLivingString* const p, float targetLev)
{
    tFeedbackLeveler_setTargetLevel(&p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLevL, targetLev);
}

void     tLivingString_setLevSmoothFactor(tLivingString* const p, float levSmoothFactor)
{
    tFeedbackLeveler_setFactor(&p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLevL, levSmoothFactor);
}

void     tLivingString_setLevStrength(tLivingString* const p, float levStrength)
{
    tFeedbackLeveler_setStrength(&p->fbLevU, levStrength);
    tFeedbackLeveler_setStrength(&p->fbLevL, levStrength);
}

void     tLivingString_setLevMode(tLivingString* const p, int levMode)
{
    tFeedbackLeveler_setMode(&p->fbLevU, levMode);
    tFeedbackLeveler_setMode(&p->fbLevL, levMode);
    p->levMode=levMode;
}
float   tLivingString_tick(tLivingString *const p, float input)
{
    // from pickPos upwards=forwards
    float fromLF=tLinearDelay_tickOut(p->delLF);
    float fromUF=tLinearDelay_tickOut(p->delUF);
    float fromUB=tLinearDelay_tickOut(p->delUB);
    float fromLB=tLinearDelay_tickOut(p->delLB);
    // into upper half of string, from nut, going backwards
    float fromNut=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(&p->DCblockerU, tOnePole_tick(&p->nutFilter, fromUF)));
    tLinearDelay_tickIn(p->delUB, fromNut);
    // into lower half of string, from pickpoint, going backwards
    float fromLowerPrep=-tOnePole_tick(&p->prepFilterL, fromLF);
    float intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB+input;
    tLinearDelay_tickIn(p->delLB, intoLower);
    // into lower half of string, from bridge
    float fromBridge=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tOnePole_tick(&p->bridgeFilter, fromLB)));
    tLinearDelay_tickIn(p->delLF, fromBridge);
    // into upper half of string, from pickpoint, going forwards/upwards
    float fromUpperPrep=-tOnePole_tick(&p->prepFilterU, fromUB);
    float intoUpper=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF+input;
    tLinearDelay_tickIn(p->delUF, intoUpper);
    // update all delay lengths
    float pickP=tExpSmooth_tick(&p->ppSmooth);
    float wLen=tExpSmooth_tick(&p->wlSmooth);
    float lowLen=pickP*wLen;
    float upLen=(1.0f-pickP)*wLen;
    tLinearDelay_setDelay(p->delLF, lowLen);
    tLinearDelay_setDelay(p->delLB, lowLen);
    tLinearDelay_setDelay(p->delUF, upLen);
    tLinearDelay_setDelay(p->delUB, upLen);
    p->curr = fromBridge;
    return p->curr;
}
float   tLivingString_sample(tLivingString* const p)
{
    return p->curr;
}

void   tLivingString_setSampleRate(tLivingString* const p, float sr)
{
    float freq = p->sampleRate/p->waveLengthInSamples;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tOnePole_setSampleRate(&p->nutFilter, p->sampleRate);
    tOnePole_setSampleRate(&p->prepFilterU, p->sampleRate);
    tOnePole_setSampleRate(&p->prepFilterL, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblockerU, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblockerL, p->sampleRate);
}

/* Simple Living String 3*/

void tSimpleLivingString3_create(tMempool** const mp, tSimpleLivingString3** const pl)
{
    ALLOC_FROM_POOL(tSimpleLivingString3, pl, mp);
}

void tSimpleLivingString3_init(LEAF* const leaf, tSimpleLivingString3* const p,
                               int oversampling, float freq, float dampFreq, float decay,
                               float targetLev, float levSmoothFactor, float levStrength, int levMode)
{
    p->oversampling = oversampling;
    p->sampleRate = leaf->sampleRate * oversampling;
    p->curr = 0.0f;
    p->maxLength = 2400 * oversampling;
    p->freq = freq;

    tExpSmooth_init(leaf, &p->wlSmooth, p->sampleRate / freq / 2.0f, 0.01f);

    tLinearDelay_create(&p->mempool, &p->delayLineU);
    tLinearDelay_create(&p->mempool, &p->delayLineL);
    p->userDecay = decay;
    tSimpleLivingString3_setDecay(p, decay);
    tSimpleLivingString3_setFreq(p, freq);

    tLinearDelay_init(leaf, p->delayLineU, p->waveLengthInSamples, p->maxLength);
    tLinearDelay_init(leaf, p->delayLineL, p->waveLengthInSamples, p->maxLength);

    tLinearDelay_setDelay(p->delayLineU, p->waveLengthInSamples);
    tLinearDelay_setDelay(p->delayLineL, p->waveLengthInSamples);

    tLinearDelay_clear(p->delayLineU);
    tLinearDelay_clear(p->delayLineL);

    p->dampFreq = dampFreq;

    tOnePole_init(leaf, &p->bridgeFilter, dampFreq);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);

    tBiQuad_init(leaf, &p->bridgeFilter2);
    tBiQuad_setSampleRate(&p->bridgeFilter2, p->sampleRate);

    p->rippleGain = 0.0f;
    p->rippleDelay = 0.5f;
    p->invOnePlusr = 1.0f;

    tHighpass_init(leaf, &p->DCblocker, 13);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
    tHighpass_setFreq(&p->DCblocker, 13);



    tFeedbackLeveler_init(leaf, &p->fbLev, targetLev, levSmoothFactor, levStrength, levMode);
    p->levMode = levMode;

    p->changeGainCompensator = 1.0f;

    p->freq = freq;

    float waveLength = (p->sampleRate / freq);
    if (waveLength < 4.8f) waveLength = 4.8f;
    else if (waveLength > p->maxLength * 2) waveLength = p->maxLength * 2;

    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);

    p->pickupPoint = 0.9f;
    p->prevDelayLength = p->waveLengthInSamples;
}

void tSimpleLivingString3_free(tSimpleLivingString3** const pl)
{
    tSimpleLivingString3* p = *pl;

    tLinearDelay_free(&p->delayLineL);
    tLinearDelay_free(&p->delayLineU);

    mpool_free((char*)p, p->mempool);
}

void     tSimpleLivingString3_setFreq(tSimpleLivingString3* const p, float freq)
{
    p->freq = freq;
    float waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    //float decay = p->userDecay;

    float temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);

    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
}
void     tSimpleLivingString3_setWaveLength(tSimpleLivingString3* const p, float waveLength)
{
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString3_setDampFreq(tSimpleLivingString3* const p, float dampFreq)
{
    //p->temp1 = (1.0f - brightness) * 0.25f * p->decay;
    //p->temp2 = (1.0f + brightness) * 0.5f * p->decay;
    //tBiQuad_setCoefficients(p->bridgeFilter2, p->temp1 , p->temp2, p->temp1 , 0.0f, 0.0f);

    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString3_setDecay(tSimpleLivingString3* const p, float decay)
{
    p->userDecay = decay;

    float temp = ((decay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.9078 * ((1.0 / p->freq)  / temp));


    //tBiQuad_setCoefficients(p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);
    //p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tSimpleLivingString3_setTargetLev(tSimpleLivingString3* const p, float targetLev)
{
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
}

void     tSimpleLivingString3_setLevSmoothFactor(tSimpleLivingString3* const p, float levSmoothFactor)
{
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString3_setLevStrength(tSimpleLivingString3* const p, float levStrength)
{
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
}

void     tSimpleLivingString3_setLevMode(tSimpleLivingString3* const p, int levMode)
{
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    p->levMode=levMode;
}
void   tSimpleLivingString3_pluck(tSimpleLivingString3* const p, float input, float position)
{
    int length = p->waveLengthInSamples;
    int pluckPoint = (int)(length * position);
    if (pluckPoint < 1)
    {
        pluckPoint = 1;
    }
    else if (pluckPoint > (length-1))
    {
        pluckPoint = length-1;
    }
    int remainder = length-pluckPoint;
    for (int i = 0; i < length; i++)
    {
        float val = 0.0f;
        if (i <= pluckPoint)
        {
            val = input * ((float)i/(float)pluckPoint);
        }
        else
        {
            val = input * (1.0f - (((float)i-(float)pluckPoint)/(float)remainder));

        }
        int bufWritePoint = (i+p->delayLineU->outPoint)%p->delayLineU->maxDelay;
        p->delayLineU->buff[bufWritePoint] = val;
    }
    for (int i = 0; i < length; i++)
    {
        int currentBufWritePoint = (i+p->delayLineL->outPoint) % p->delayLineL->maxDelay;
        int currentBufReadPoint = ((length-1-i)+p->delayLineU->outPoint);
        int currentBufReadPointMod = currentBufReadPoint % p->delayLineU->maxDelay;
        p->delayLineL->buff[currentBufWritePoint] = p->delayLineU->buff[currentBufReadPointMod];
    }
}
float   tSimpleLivingString3_tick(tSimpleLivingString3* const p, float input)
{
    //p->changeGainCompensator = 1.0f;
    float wl = tExpSmooth_tick(&p->wlSmooth);
    //float changeInDelayTime = wl - p->prevDelayLength;
    //if (changeInDelayTime < 0.0f)
    {
    //	p->changeGainCompensator = sqrtf(1.0f - changeInDelayTime);
    }
    tLinearDelay_setDelay(p->delayLineU, wl);
    tLinearDelay_setDelay(p->delayLineL, wl);

    for (int i = 0; i < p->oversampling; i++)
    {
		p->Uout = tHighpass_tick(&p->DCblocker,tOnePole_tick(&p->bridgeFilter,tLinearDelay_tickOut(p->delayLineU))* (p->decay * p->invOnePlusr));
		p->Uout = LEAF_clip(-1.0f, tFeedbackLeveler_tick(&p->fbLev, p->Uout), 1.0f);
    	//p->Uout = tLinearDelay_tickOut(p->delayLineU) * p->decay;
		p->Lout = LEAF_clip(-1.0f, tLinearDelay_tickOut(p->delayLineL), 1.0f);

		tLinearDelay_tickIn(p->delayLineU, (-1.0f * p->Lout) + input);
		tLinearDelay_tickIn(p->delayLineL, -1.0f * p->Uout);
		tLinearDelay_addTo (p->delayLineU, p->Lout * p->rippleGain, p->rippleDelay*wl);
    }

    //calculate pickup point
    float point = wl * p->pickupPoint;
    float reversePoint = wl * (1.0f - p->pickupPoint);
    int32_t whichPoint = ((int32_t)roundf(point));
    float floatPart = point - whichPoint;

    int32_t outpointplus =((int32_t)p->delayLineU->outPoint + whichPoint);
    int32_t outpointmod = outpointplus % (int32_t)p->delayLineU->maxDelay;
    int32_t outpointmod2 = (outpointmod + 1) % (int32_t)p->delayLineU->maxDelay;
    float sampleBitOne = (p->delayLineU->buff[outpointmod] * (1.0f - floatPart)) + (p->delayLineU->buff[outpointmod2] * floatPart);


    whichPoint = (int32_t)roundf(reversePoint);
    floatPart = reversePoint - whichPoint;
    outpointplus =(((int32_t)p->delayLineL->outPoint) + whichPoint);
    outpointmod = outpointplus % (int32_t)p->delayLineL->maxDelay;
    outpointmod2 = (outpointmod + 1) % (int32_t)p->delayLineL->maxDelay;
    float sampleBitTwo = (p->delayLineL->buff[outpointmod] * (1.0f - floatPart)) + (p->delayLineL->buff[outpointmod2] * floatPart);

    p->curr = 0.5f * (sampleBitOne + sampleBitTwo);
    //p->curr = p->Uout;

    p->prevDelayLength = p->waveLengthInSamples;
    //float stringInput=tHighpass_tick(p->DCblocker, tFeedbackLeveler_tick(p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    //tLinearDelay_tickIn(p->delayLine, stringInput);
    //tLinearDelay_setDelay(p->delayLine, tExpSmooth_tick(p->wlSmooth));
    //p->curr = d1 + d2;
    return p->curr;
}


void   tSimpleLivingString3_setPickupPoint(tSimpleLivingString3* const p, float pickupPoint)
{
    p->pickupPoint = pickupPoint;
}


float   tSimpleLivingString3_sample(tSimpleLivingString3* const p)
{
    return p->curr;
}

void   tSimpleLivingString3_setSampleRate(tSimpleLivingString3* const p, float sr)
{
    p->sampleRate = sr*p->oversampling;
    float freq = p->sampleRate/p->waveLengthInSamples;

    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
    tBiQuad_setSampleRate(&p->bridgeFilter2, p->sampleRate);
}


/* Simple Living String 4*/

void tSimpleLivingString4_create(tMempool** const mp, tSimpleLivingString4** const pl)
{
    ALLOC_FROM_POOL(tSimpleLivingString4, pl, mp);
}

void tSimpleLivingString4_init(LEAF* const leaf, tSimpleLivingString4* const p,
                               int oversampling, float freq, float dampFreq, float decay,
                               float targetLev, float levSmoothFactor, float levStrength, int levMode)
{
    p->oversampling = oversampling;
    p->sampleRate = leaf->sampleRate * oversampling;
    p->curr = 0.0f;
    p->maxLength = 2400 * oversampling;

    tExpSmooth_init(leaf, &p->wlSmooth, p->sampleRate / freq / 2.0f, 0.01f);

    tLinearDelay_create(&p->mempool, &p->delayLineU);
    tLinearDelay_create(&p->mempool, &p->delayLineL);

    tSimpleLivingString4_setFreq(p, freq);

    tLinearDelay_init(leaf, p->delayLineU, p->waveLengthInSamples, p->maxLength);
    tLinearDelay_init(leaf, p->delayLineL, p->waveLengthInSamples, p->maxLength);

    tLinearDelay_setDelay(p->delayLineU, p->waveLengthInSamples);
    tLinearDelay_setDelay(p->delayLineL, p->waveLengthInSamples);

    tLinearDelay_clear(p->delayLineU);
    tLinearDelay_clear(p->delayLineL);

    p->dampFreq = dampFreq;

    tOnePole_init(leaf, &p->bridgeFilter, dampFreq);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);

    tBiQuad_init(leaf, &p->bridgeFilter2);
    tBiQuad_setSampleRate(&p->bridgeFilter2, p->sampleRate);

    p->rippleGain = 0.0f;
    p->rippleDelay = 0.5f;

    tHighpass_init(leaf, &p->DCblocker, 0.001f);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
    tHighpass_setFreq(&p->DCblocker, 0.001f);

    p->userDecay = decay;
    p->pluckPosition = 0.8f;

    tFeedbackLeveler_init(leaf, &p->fbLev, targetLev, levSmoothFactor, levStrength, levMode);
    p->levMode = levMode;

    p->changeGainCompensator = 1.0f;

    p->freq = freq;

    float waveLength = (p->sampleRate / freq);
    if (waveLength < 4.8f) waveLength = 4.8f;
    else if (waveLength > p->maxLength * 2) waveLength = p->maxLength * 2;

    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);

    float temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = expf(-6.91f * ((1.0f / p->freq) / temp));

    p->pickupPoint = 0.9f;
    p->prevDelayLength = p->waveLengthInSamples;
}

void tSimpleLivingString4_free(tSimpleLivingString4** const pl)
{
    tSimpleLivingString4* p = *pl;

    tLinearDelay_free(&p->delayLineU);
    tLinearDelay_free(&p->delayLineL);

    mpool_free((char*)p, p->mempool);
}

void     tSimpleLivingString4_setFreq(tSimpleLivingString4* const p, float freq)
{
    p->freq = freq;
    float waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    //float decay = p->userDecay;

    float temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);

    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
}

void     tSimpleLivingString4_setWaveLength(tSimpleLivingString4* const p, float waveLength)
{
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength = p->maxLength*2;
    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString4_setDampFreq(tSimpleLivingString4* const p, float dampFreq)
{
    //p->temp1 = (1.0f - brightness) * 0.25f * p->decay;
    //p->temp2 = (1.0f + brightness) * 0.5f * p->decay;
    //tBiQuad_setCoefficients(p->bridgeFilter2, p->temp1 , p->temp2, p->temp1 , 0.0f, 0.0f);

    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString4_setDecay(tSimpleLivingString4* const p, float decay)
{
    p->userDecay = decay;

    float temp = ((decay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);
    //p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tSimpleLivingString4_setTargetLev(tSimpleLivingString4* const p, float targetLev)
{
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
}

void     tSimpleLivingString4_setLevSmoothFactor(tSimpleLivingString4* const p, float levSmoothFactor)
{
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString4_setLevStrength(tSimpleLivingString4* const p, float levStrength)
{
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
}

void     tSimpleLivingString4_setLevMode(tSimpleLivingString4* const p, int levMode)
{
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    p->levMode=levMode;
}

void   tSimpleLivingString4_setPluckPosition(tSimpleLivingString4* const p, float position)
{
    p->pluckPosition = position;
}


void   tSimpleLivingString4_pluck(tSimpleLivingString4* const p, float input, float position)
{
    int length = p->waveLengthInSamples;
    p->pluckPosition = position;
    int pluckPoint = (int)(length * position);
    if (pluckPoint < 1)
    {
    	pluckPoint = 1;
    }
    else if (pluckPoint > (length-1))
    {
    	pluckPoint = length-1;
    }
    int remainder = length-pluckPoint;
    for (int i = 0; i < length; i++)
    {
        float val = 0.0f;
        if (i <= pluckPoint)
        {
            val = input * LEAF_tanh(((float)i/(float)pluckPoint)*1.2f);
        }
        else
        {
            val = input * LEAF_tanh((1.0f - (((float)i-(float)pluckPoint)/(float)remainder))*1.2f);

        }
        int bufWritePoint = (i+p->delayLineU->outPoint)%p->delayLineU->maxDelay;
        p->delayLineU->buff[bufWritePoint] = val;
    }
    for (int i = 0; i < length; i++)
    {
        int otherBufPosition = ((length-1-i) + p->delayLineU->outPoint)%p->delayLineU->maxDelay;
        int bufWritePoint = (i+p->delayLineL->outPoint)%p->delayLineL->maxDelay;
        p->delayLineL->buff[bufWritePoint] = p->delayLineU->buff[otherBufPosition];
    }
}

void   tSimpleLivingString4_pluckNoPosition(tSimpleLivingString4* const p, float input)
{
    int length = p->waveLengthInSamples;
    float position = p->pluckPosition;
    int pluckPoint = (int)(length * position);
    if (pluckPoint < 1)
    {
    	pluckPoint = 1;
    }
    else if (pluckPoint > (length-1))
    {
    	pluckPoint = length-1;
    }
    int remainder = length-pluckPoint;
    for (int i = 0; i < length; i++)
    {
        float val = 0.0f;
        if (i <= pluckPoint)
        {
            val = input * LEAF_tanh(((float)i/(float)pluckPoint)*1.2f);
        }
        else
        {
            val = input * LEAF_tanh((1.0f - (((float)i-(float)pluckPoint)/(float)remainder))*1.2f);

        }
        int bufWritePoint = (i+p->delayLineU->outPoint)%p->delayLineU->maxDelay;
        p->delayLineU->buff[bufWritePoint] = val;
    }
    for (int i = 0; i < length; i++)
    {
        int otherBufPosition = ((length-1-i) + p->delayLineU->outPoint)%p->delayLineU->maxDelay;
        int bufWritePoint = (i+p->delayLineL->outPoint)%p->delayLineL->maxDelay;
        p->delayLineL->buff[bufWritePoint] = p->delayLineU->buff[otherBufPosition];
    }
}



float   tSimpleLivingString4_tick(tSimpleLivingString4* const p, float input)
{
    p->changeGainCompensator = 1.0f;
    float wl = tExpSmooth_tick(&p->wlSmooth);
    volatile float changeInDelayTime = -0.01875f*(wl*0.5f - p->prevDelayLength*0.5f);
    if (changeInDelayTime < -0.1f)
    {
    	p->changeGainCompensator = sqrtf(1.0f - changeInDelayTime);
    }
    else{
        p->changeGainCompensator = 1.0f;
    }
    tLinearDelay_setDelay(p->delayLineU, wl);
    tLinearDelay_setDelay(p->delayLineL, wl);

    for (int i = 0; i < p->oversampling; i++)
    {
		p->Uout = tHighpass_tick(&p->DCblocker,tOnePole_tick(&p->bridgeFilter,tLinearDelay_tickOut(p->delayLineU))* (p->decay - fabsf(p->rippleGain)));
		p->Uout = LEAF_clip(-1.0f, tFeedbackLeveler_tick(&p->fbLev, p->Uout), 1.0f);
    	//p->Uout = tLinearDelay_tickOut(p->delayLineU) * p->decay;
		p->Lout = LEAF_clip(-1.0f, tLinearDelay_tickOut(p->delayLineL), 1.0f);

		tLinearDelay_tickIn(p->delayLineU, (-1.0f * p->Lout)* p->changeGainCompensator);
		tLinearDelay_tickIn(p->delayLineL, (-1.0f * p->Uout));
        tLinearDelay_addTo (p->delayLineU, input, p->pluckPosition*wl);
        tLinearDelay_addTo (p->delayLineL, input, (1.0f - p->pluckPosition)*wl);
		tLinearDelay_addTo (p->delayLineU, p->Lout * p->rippleGain, p->rippleDelay*wl);
    }

    //calculate pickup point
    volatile float point = LEAF_clip(2.0f, wl * p->pickupPoint, wl-2.0f);
    volatile float reversePoint = LEAF_clip(2.0f, wl * (1.0f - p->pickupPoint),wl-2.0f);
    volatile int32_t whichPoint = ((int32_t)(point+0.5f));
    volatile float floatPart = point - whichPoint;

    volatile int32_t outpointplus =((int32_t)p->delayLineU->outPoint + whichPoint);
    volatile int32_t outpointmod = outpointplus % (int32_t)p->delayLineU->maxDelay;
    volatile int32_t outpointmod2 = (outpointmod + 1) % (int32_t)p->delayLineU->maxDelay;
    volatile float sampleBitOne = (p->delayLineU->buff[outpointmod] * (1.0f - floatPart)) + (p->delayLineU->buff[outpointmod2] * floatPart);


    whichPoint = (int32_t)(reversePoint+0.5f);
    floatPart = reversePoint - whichPoint;
    outpointplus =(((int32_t)p->delayLineL->outPoint) + whichPoint);
    outpointmod = outpointplus % (int32_t)p->delayLineL->maxDelay;
    outpointmod2 = (outpointmod + 1) % (int32_t)p->delayLineL->maxDelay;
    float sampleBitTwo = (p->delayLineL->buff[outpointmod] * (1.0f - floatPart)) + (p->delayLineL->buff[outpointmod2] * floatPart);

    p->curr = 0.5f * (sampleBitOne + sampleBitTwo) * p->changeGainCompensator;
    //p->curr = p->Uout;

    p->prevDelayLength = p->waveLengthInSamples;
    //float stringInput=tHighpass_tick(p->DCblocker, tFeedbackLeveler_tick(p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    //tLinearDelay_tickIn(p->delayLine, stringInput);
    //tLinearDelay_setDelay(p->delayLine, tExpSmooth_tick(p->wlSmooth));
    //p->curr = d1 + d2;
    return p->curr;
}


void   tSimpleLivingString4_setPickupPoint(tSimpleLivingString4* const p, float pickupPoint)
{
    p->pickupPoint = pickupPoint;
}


float   tSimpleLivingString4_sample(tSimpleLivingString4* const p)
{
    return p->curr;
}

void   tSimpleLivingString4_setSampleRate(tSimpleLivingString4* const p, float sr)
{
    p->sampleRate = sr*p->oversampling;
    float freq = p->sampleRate/p->waveLengthInSamples;

    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
    tBiQuad_setSampleRate(&p->bridgeFilter2, p->sampleRate);
}


/* Simple Living String 5*/

void tSimpleLivingString5_create(tMempool** const mp, tSimpleLivingString5** const pl)
{
    ALLOC_FROM_POOL(tSimpleLivingString5, pl, mp);
}

void tSimpleLivingString5_init(LEAF* const leaf, tSimpleLivingString5* const p,
                               int oversampling, float freq, float dampFreq, float decay,
                               float prepPos, float prepIndex, float pluckPos,
                               float targetLev, float levSmoothFactor, float levStrength, int levMode)
{
    p->oversampling = oversampling;
    p->sampleRate = leaf->sampleRate * oversampling;
    p->curr = 0.0f;
    p->maxLength = 2400 * oversampling;

    p->prepPos = prepPos;
    p->prepIndex = prepIndex;
    p->pluckPosition = pluckPos;

    tExpSmooth_init(leaf, &p->prepPosSmooth, prepPos, 0.001f);
    tExpSmooth_init(leaf, &p->prepIndexSmooth, prepIndex, 0.001f);
    tExpSmooth_init(leaf, &p->wlSmooth, p->sampleRate / freq / 2.0f, 0.05f);
    tExpSmooth_init(leaf, &p->pluckPosSmooth, pluckPos, 0.001f);
    tExpSmooth_init(leaf, &p->pickupPointSmooth, pluckPos, 0.001f);

    tLagrangeDelay_create(&p->mempool, &p->delUF);
    tLagrangeDelay_create(&p->mempool, &p->delUB);
    tLagrangeDelay_create(&p->mempool, &p->delLF);
    tLagrangeDelay_create(&p->mempool, &p->delLB);

    tSimpleLivingString5_setFreq(p, freq);

    tLagrangeDelay_init(leaf, p->delUF, p->waveLengthInSamples, p->maxLength);
    tLagrangeDelay_init(leaf, p->delUB, p->waveLengthInSamples, p->maxLength);
    tLagrangeDelay_init(leaf, p->delLF, p->waveLengthInSamples, p->maxLength);
    tLagrangeDelay_init(leaf, p->delLB, p->waveLengthInSamples, p->maxLength);

    tLagrangeDelay_setDelay(p->delUF, p->waveLengthInSamples - (p->prepPos * p->waveLengthInSamples));
    tLagrangeDelay_setDelay(p->delUB, p->waveLengthInSamples - ((1.0f - p->prepPos) * p->waveLengthInSamples));
    tLagrangeDelay_setDelay(p->delLF, p->waveLengthInSamples - (p->prepPos * p->waveLengthInSamples));
    tLagrangeDelay_setDelay(p->delLB, p->waveLengthInSamples - ((1.0f - p->prepPos) * p->waveLengthInSamples));

    tLagrangeDelay_clear(p->delUF);
    tLagrangeDelay_clear(p->delUB);
    tLagrangeDelay_clear(p->delLF);
    tLagrangeDelay_clear(p->delLB);

    p->dampFreq = dampFreq;

    tOnePole_init(leaf, &p->bridgeFilter, dampFreq);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);

    tOnePole_init(leaf, &p->nutFilter, dampFreq);
    tOnePole_setSampleRate(&p->nutFilter, p->sampleRate);
    tOnePole_setFreq(&p->nutFilter, dampFreq);

    p->rippleGain = 0.0f;
    p->rippleDelay = 0.5f;

    tHighpass_init(leaf, &p->DCblocker, 0.001f);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
    tHighpass_setFreq(&p->DCblocker, 0.001f);

    tHighpass_init(leaf, &p->DCblocker2, 0.001f);
    tHighpass_setSampleRate(&p->DCblocker2, p->sampleRate);
    tHighpass_setFreq(&p->DCblocker2, 0.001f);

    p->userDecay = decay;
    p->pluckPosition = 0.8f;

    tFeedbackLeveler_init(leaf, &p->fbLev, targetLev, levSmoothFactor, levStrength, levMode);
    tFeedbackLeveler_init(leaf, &p->fbLev2, targetLev, levSmoothFactor, levStrength, levMode);

    p->levMode = levMode;
    p->changeGainCompensator = 1.0f;

    p->ff = 0.3f;
    p->freq = freq;

    float waveLength = (p->sampleRate / freq);
    if (waveLength < 4.8f) waveLength = 4.8f;
    else if (waveLength > p->maxLength * 2) waveLength = p->maxLength * 2;

    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);

    p->decay = decay;
    p->pickupPoint = 0.9f;
    p->prevDelayLength = p->waveLengthInSamples;
}

void tSimpleLivingString5_free(tSimpleLivingString5** const pl)
{
    tSimpleLivingString5* p = *pl;

    tLagrangeDelay_free(&p->delUF);
    tLagrangeDelay_free(&p->delUB);
    tLagrangeDelay_free(&p->delLF);
    tLagrangeDelay_free(&p->delLB);

    mpool_free((char*)p, p->mempool);
}
void     tSimpleLivingString5_setFreq(tSimpleLivingString5* const p, float freq)
{
    p->freq = freq;
    float waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    //float decay = p->userDecay;

    //float temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    //p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);

    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
}

void     tSimpleLivingString5_setWaveLength(tSimpleLivingString5* const p, float waveLength)
{
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString5_setDampFreq(tSimpleLivingString5* const p, float dampFreq)
{
    //p->temp1 = (1.0f - brightness) * 0.25f * p->decay;
    //p->temp2 = (1.0f + brightness) * 0.5f * p->decay;
    //tBiQuad_setCoefficients(p->bridgeFilter2, p->temp1 , p->temp2, p->temp1 , 0.0f, 0.0f);

    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    tOnePole_setFreq(&p->nutFilter, dampFreq);
}

void     tSimpleLivingString5_setDecay(tSimpleLivingString5* const p, float decay)
{
    p->userDecay = decay;


    //float temp = ((decay * 0.01f) + 0.01f) * 6.9078f;
    //p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    p->decay = decay;//;
    //tBiQuad_setCoefficients(p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);
    //p->decay=powf(0.001f,1.0f/(p->freq*decay));
}
void   tSimpleLivingString5_setPrepPosition(tSimpleLivingString5* const p, float prepPosition)
{
    p->prepPos = prepPosition;
    tExpSmooth_setDest(&p->prepPosSmooth, prepPosition);
}
void   tSimpleLivingString5_setPrepIndex(tSimpleLivingString5* const p, float prepIndex)
{
     p->prepIndex = prepIndex;
    tExpSmooth_setDest(&p->prepIndexSmooth, prepIndex);
}

void     tSimpleLivingString5_setTargetLev(tSimpleLivingString5* const p, float targetLev)
{
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLev2, targetLev);
}

void     tSimpleLivingString5_setLevSmoothFactor(tSimpleLivingString5* const p, float levSmoothFactor)
{
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLev2, levSmoothFactor);
}

void     tSimpleLivingString5_setLevStrength(tSimpleLivingString5* const p, float levStrength)
{
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
    tFeedbackLeveler_setStrength(&p->fbLev2, levStrength);
}

void     tSimpleLivingString5_setLevMode(tSimpleLivingString5* const p, int levMode)
{
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    tFeedbackLeveler_setMode(&p->fbLev2, levMode);
    p->levMode=levMode;
}

void   tSimpleLivingString5_setPluckPosition(tSimpleLivingString5* const p, float position)
{
    p->pluckPosition = position;
    tExpSmooth_setDest(&p->pluckPosSmooth, position);

}


void   tSimpleLivingString5_pluck(tSimpleLivingString5* const p, float input, float position)
{
    //p->pluckPosition = position;
    p->pluckPosition = 0.5f;
    tExpSmooth_setDest(&p->pluckPosSmooth, position);
    volatile float pluckPoint = position*p->waveLengthInSamples;
    pluckPoint = LEAF_clip(1.0f, pluckPoint, p->waveLengthInSamples-1.0f);
    uint32_t pluckPointInt = (uint32_t) pluckPoint;
    volatile float remainder = p->waveLengthInSamples-pluckPoint;
    float BLen = p->delUB->delay;
    float FLen = p->delUF->delay;
    uint32_t FLenInt = (uint32_t)FLen;
    //float FLenAlpha = FLen -FLenInt;
    uint32_t BLenInt = (uint32_t)BLen;
    float BLenAlpha = BLen -BLenInt;

    for (uint32_t i = 0; i < p->waveLengthInSamples; i++)
    {
        float val = 0.0f;
        if (i <= pluckPointInt)
        {
            val = input * ((float)i/(float)pluckPointInt);
        }
        else
        {
            val = input * (1.0f - (((float)i-(float)pluckPointInt)/(float)remainder));
        }
        if (i  < BLenInt)
        {
            tLagrangeDelay_addTo(p->delUB, val, i);
            tLagrangeDelay_addTo(p->delLB, val, BLenInt-i);
        }
        else if (i == BLenInt)
        {
            tLagrangeDelay_addTo(p->delUB, val * (1.0f-BLenAlpha), i);
            tLagrangeDelay_addTo(p->delLB, val * (1.0f-BLenAlpha), BLenInt-i);
            tLagrangeDelay_addTo(p->delUF, val * BLenAlpha, i-BLenInt);
            tLagrangeDelay_addTo(p->delLF, val * BLenAlpha, (FLenInt-1)-(i-BLenInt));
        }
        else
        {
            tLagrangeDelay_addTo(p->delUF, val, i-BLenInt);
            tLagrangeDelay_addTo(p->delLF, val, (FLenInt-1)-(i-BLenInt));
        }
    }
}





float   tSimpleLivingString5_tick(tSimpleLivingString5* const p, float input)
{
    //p->changeGainCompensator = 1.0f;
    float wl = tExpSmooth_tick(&p->wlSmooth);

    //volatile float changeInDelayTime = -0.01875f*(wl*0.5f - p->prevDelayLength*0.5f);
    //if (changeInDelayTime < -0.1f)
    {
    //	p->changeGainCompensator = sqrtf(1.0f - changeInDelayTime);
    }
    //else
    {
    //    p->changeGainCompensator = 1.0f;
    }
    float prepPosSmoothed = tExpSmooth_tick(&p->prepPosSmooth);
    float FLen = wl*(1.0f-prepPosSmoothed);
    uint32_t FLenInt = (uint32_t)FLen;
    float BLen = wl*prepPosSmoothed;
    uint32_t BLenInt = (uint32_t)BLen;
    tLagrangeDelay_setDelay(p->delUF, FLen);
    tLagrangeDelay_setDelay(p->delUB, BLen);
    tLagrangeDelay_setDelay(p->delLF, FLen);
    tLagrangeDelay_setDelay(p->delLB, BLen);
    float pluckPosSmoothed = tExpSmooth_tick(&p->pluckPosSmooth);
    float pluckPosInSamples = pluckPosSmoothed * wl;
    uint32_t pluckPosInSamplesInt = (uint32_t) pluckPosInSamples;
    float alpha = pluckPosInSamples - pluckPosInSamplesInt;

    uint32_t sample1 = pluckPosInSamplesInt;
    uint32_t sample2 = pluckPosInSamplesInt + 1;

    uint32_t sample1Front = (uint32_t) (sample1 - BLen);
    uint32_t sample2Front = (uint32_t) (sample2 - BLen);




    //now tick out the output data and filter (oversampled)
    for (int i = 0; i < p->oversampling; i++)
    {
        if (sample1 < BLenInt)
        {
            tLagrangeDelay_addTo(p->delUB, input * (1.0f - alpha), sample1);
            tLagrangeDelay_addTo(p->delLB, input * (1.0f - alpha), BLenInt-sample1);
        }
        else
        {
            tLagrangeDelay_addTo(p->delUF, input * (1.0f - alpha), sample1Front);
            tLagrangeDelay_addTo(p->delLF, input * (1.0f - alpha), FLenInt-sample1Front);
        }
        if (sample2 < BLenInt)
        {
            tLagrangeDelay_addTo(p->delUB, input * alpha, sample2);
            tLagrangeDelay_addTo(p->delLB, input * alpha, BLenInt-sample2);
        }
        else
        {
            tLagrangeDelay_addTo(p->delUF, input * alpha, sample2Front);
            tLagrangeDelay_addTo(p->delLF, input * alpha, FLenInt-sample2Front);
        }


        float fromUF=tLagrangeDelay_tickOut(p->delUF);
        float fromLF=tLagrangeDelay_tickOut(p->delLF);
        float fromLB=tLagrangeDelay_tickOut(p->delLB);
        float fromUB=tLagrangeDelay_tickOut(p->delUB);
        //fromUF = tanhf(p->fbSample1) * p->fb + fromUF*0.1f;
        //fromLB = tanhf(p->fbSample2) * p->fb + fromLB*0.1f;
        //p->fbSample1 = fromUF;
        //p->fbSample2 = fromLB;
        //fromUF = fromUF * 1.0f / (1.0f+p->fb);
        //fromLB = fromLB * 1.0f / (1.0f+p->fb);
        //fromUF = tWavefolder_tick(p->wf1, fromUF);
        //fromLB = tWavefolder_tick(p->wf2, fromLB);
        //softclip approx for tanh saturation in original code
        //float fromUFSat = tanhf(fromUF + input);
        float fromUFSat = tanhf(fromUF);
        float fromLBSat = tanhf(fromLB);

        fromUF = (p->ff * fromUFSat) + ((1.0f - p->ff) * fromUF);
        fromLB = (p->ff * fromLBSat) + ((1.0f - p->ff) * fromLB);

#if 0
        float fromBridge = -tOnePole_tick(p->bridgeFilter, fromUF);
        tLagrangeDelay_tickIn(p->delLF, fromBridge + input);
        tLagrangeDelay_tickIn(p->delLB, fromLF);
        float fromNut=-fromLB;
        tLagrangeDelay_tickIn(p->delUB, fromNut);
        tLagrangeDelay_tickIn(p->delUF, fromUB);
#endif

        // into front half of string, from bridge, going backwards (lower section)
        //float fromBridge=-tFeedbackLeveler_tick(p->fbLev, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(p->DCblocker, tOnePole_tick(p->bridgeFilter, fromUF)));
        //float fromBridge=-tFeedbackLeveler_tick(p->fbLev, (p->levMode==0?p->decay:1.0f)*fromUF);
        float fromBridge=-p->decay*fromUF;//tOnePole_tick(p->bridgeFilter, fromUF);
        //float fromBridge=-tHighpass_tick(p->DCblocker, tOnePole_tick(p->bridgeFilter, fromUF)) * p->decay; //version with feedbackleveler
        //fromBridge = LEAF_clip(-1.0f, fromBridge, 1.0f);
        tLagrangeDelay_tickIn(p->delLF, fromBridge);
        // into back half of string, from prepPoint, going backwards (lower section)
        //float fromUpperPrep=-tOnePole_tick(p->prepFilterL, fromUB);
        float fromUpperPrep=-fromUB;
        //fromUpperPrep = LEAF_clip(-1.0f, fromUpperPrep, 1.0f);
        float intoLower=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF; //used to add input here
        //intoLower = LEAF_clip(-1.0f, intoLower, 1.0f);
        tLagrangeDelay_tickIn(p->delLB, intoLower);
        // into back of string, from nut going forward (upper section)
        //float fromNut=-tFeedbackLeveler_tick(p->fbLev2, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(p->DCblocker2,fromLB));
        //float fromNut=-tFeedbackLeveler_tick(p->fbLev2, (p->levMode==0?p->decay:1.0f)*fromLB);
        float fromNut=-p->decay*fromLB;//tOnePole_tick(p->nutFilter, fromLB);
        //float fromNut=-p->decay*tHighpass_tick(p->DCblocker2,fromLB)); //version without feedback leveler
        //fromNut = LEAF_clip(-1.0f, fromNut, 1.0f);
        tLagrangeDelay_tickIn(p->delUB, fromNut);
        // into front half of string, from prepPoint, going forward (upper section)
        //float fromLowerPrep=-tOnePole_tick(p->prepFilterU, fromLF);
        float fromLowerPrep=-fromLF;
        //fromLowerPrep = LEAF_clip(-1.0f, fromLowerPrep, 1.0f);
        float intoUpperFront=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB;
        //intoUpper = tanhf(intoUpper);
        //intoUpperFront = LEAF_clip(-1.0f, intoUpperFront, 1.0f);
        tLagrangeDelay_tickIn(p->delUF, intoUpperFront);

        p->curr = fromBridge;
    }

    //pick up the signal
    float pickupPointSmoothed = tExpSmooth_tick(&p->pickupPointSmooth);
    float pickupPosInSamples = pickupPointSmoothed * wl;


    float sample1Front2 = (pickupPosInSamples - BLen);

    uint32_t a1 = (uint32_t) pickupPosInSamples;
    float a1F =pickupPosInSamples - (float) a1;

    uint32_t a2 = (uint32_t) (BLen-pickupPosInSamples);
    float a2F =(BLen-pickupPosInSamples) - (float) a2;

    uint32_t a3 = (uint32_t) sample1Front2;
    float a3F =sample1Front2 - (float) a3;

    uint32_t a4 = (uint32_t) (FLen-sample1Front2);
    float a4F = (FLen-sample1Front2) - (float) a4;

    float outputSample1 = 0.0f;

    if (pickupPosInSamples < BLen)
    {
        outputSample1 = tLagrangeDelay_tapOutInterpolated(p->delUB, a1, a1F);
        outputSample1 += tLagrangeDelay_tapOutInterpolated(p->delLB, a2, a2F);
    }
    else
    {
        outputSample1 = tLagrangeDelay_tapOutInterpolated(p->delUF, a3, a3F);
        outputSample1 += tLagrangeDelay_tapOutInterpolated(p->delLF, a4, a4F);
    }

    p->curr = 0.5f * outputSample1;
    //p->curr = p->Uout;
    p->prevDelayLength = p->waveLengthInSamples;

    //float stringInput=tHighpass_tick(p->DCblocker, tFeedbackLeveler_tick(p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    //tLinearDelay_tickIn(p->delayLine, stringInput);
    //tLinearDelay_setDelay(p->delayLine, tExpSmooth_tick(p->wlSmooth));
    //p->curr = d1 + d2;
    return p->curr;
}


void   tSimpleLivingString5_setPickupPoint(tSimpleLivingString5* const p, float pickupPoint)
{
    p->pickupPoint = pickupPoint;
    tExpSmooth_setDest(&p->pickupPointSmooth, pickupPoint);
}


float   tSimpleLivingString5_sample(tSimpleLivingString5* const p)
{
    return p->curr;
}

void   tSimpleLivingString5_setSampleRate(tSimpleLivingString5* const p, float sr)
{
    p->sampleRate = sr*p->oversampling;
    float freq = p->sampleRate/p->waveLengthInSamples;

    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
}


void   tSimpleLivingString5_setFFAmount(tSimpleLivingString5* const p, float ff)
{
    p->ff = ff;
}


/* Living String 2 : fix access patterns (tExpSmooth is a value, not a pointer) */
void tLivingString2_create(tMempool** const mp, tLivingString2** const pl)
{
    ALLOC_FROM_POOL(tLivingString2, pl, mp);
}
void tLivingString2_init(LEAF* const leaf, tLivingString2* const p,
                         float freq, float pickPos, float prepPos, float pickupPos,
                         float prepIndex, float brightness, float decay,
                         float targetLev, float levSmoothFactor, float levStrength, int levMode)
{
    p->sampleRate = leaf->sampleRate;
    p->curr = 0.0f;

    tExpSmooth_init(leaf, &p->wlSmooth, p->sampleRate / freq, 0.1f);
    tLivingString2_setFreq(p, freq);

    p->freq = freq;
    p->prepPos = prepPos;

    tExpSmooth_init(leaf, &p->ppSmooth, pickPos, 0.01f);
    tExpSmooth_init(leaf, &p->prpSmooth, prepPos, 0.01f);
    tExpSmooth_init(leaf, &p->puSmooth, pickupPos, 0.01f);

    tLivingString2_setPickPos(p, pickPos);
    tLivingString2_setPrepPos(p, prepPos);

    p->prepIndex = prepIndex;
    p->pickupPos = pickupPos;

    tHermiteDelay_create(&p->mempool, &p->delLF);
    tHermiteDelay_create(&p->mempool, &p->delUF);
    tHermiteDelay_create(&p->mempool, &p->delUB);
    tHermiteDelay_create(&p->mempool, &p->delLB);

    tHermiteDelay_init(leaf, p->delLF, p->waveLengthInSamples, 2400);
    tHermiteDelay_init(leaf, p->delUF, p->waveLengthInSamples, 2400);
    tHermiteDelay_init(leaf, p->delUB, p->waveLengthInSamples, 2400);
    tHermiteDelay_init(leaf, p->delLB, p->waveLengthInSamples, 2400);

    tHermiteDelay_clear(p->delLF);
    tHermiteDelay_clear(p->delUF);
    tHermiteDelay_clear(p->delUB);
    tHermiteDelay_clear(p->delLB);

    p->brightness = brightness;

    tTwoZero_init(leaf, &p->bridgeFilter);
    tTwoZero_init(leaf, &p->nutFilter);
    tTwoZero_init(leaf, &p->prepFilterU);
    tTwoZero_init(leaf, &p->prepFilterL);

    tLivingString2_setBrightness(p, brightness);

    tHighpass_init(leaf, &p->DCblockerU, 8);
    tHighpass_init(leaf, &p->DCblockerL, 8);

    p->decay = decay;

    tFeedbackLeveler_init(leaf, &p->fbLevU, targetLev, levSmoothFactor, levStrength, levMode);
    tFeedbackLeveler_init(leaf, &p->fbLevL, targetLev, levSmoothFactor, levStrength, levMode);

    p->levMode = levMode;
}

void tLivingString2_free(tLivingString2** const pl)
{
    tLivingString2* p = *pl;

    tHermiteDelay_free(&p->delLF);
    tHermiteDelay_free(&p->delUF);
    tHermiteDelay_free(&p->delUB);
    tHermiteDelay_free(&p->delLB);

    mpool_free((char*)p, p->mempool);
}
void     tLivingString2_setFreq(tLivingString2* const p, float freq)
{    // NOTE: It is faster to set wavelength in samples directly
    if (freq<20.f) freq=20.f;
    else if (freq>10000.f) freq=10000.f;
    freq = freq*2;
    p->freq = freq;
    p->waveLengthInSamples = (p->sampleRate/p->freq) - 1;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString2_setWaveLength(tLivingString2* const p, float waveLength)
{
    waveLength = waveLength * 0.5f;
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.f) waveLength=2400.f;
    p->waveLengthInSamples = waveLength - 1;
    p->freq = p->sampleRate / waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString2_setPickPos(tLivingString2* const p, float pickPos)
{    // between 0 and 1
    if (pickPos<0.f) pickPos=0.f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(&p->ppSmooth, p->pickPos);
}
void     tLivingString2_setPrepPos(tLivingString2* const p, float prepPos)
{    // between 0 and 1
    if (prepPos<0.f) prepPos=0.f;
    else if (prepPos>1.f) prepPos=1.f;
    p->prepPos = prepPos;
    tExpSmooth_setDest(&p->prpSmooth, p->prepPos);
}

void     tLivingString2_setPickupPos(tLivingString2* const p, float pickupPos)
{    // between 0 and 1
    if (pickupPos<0.f) pickupPos=0.f;
    else if (pickupPos>1.f) pickupPos=1.f;
    p->pickupPos = pickupPos;
    tExpSmooth_setDest(&p->puSmooth, p->pickupPos);
}

void     tLivingString2_setPrepIndex(tLivingString2* const p, float prepIndex)
{    // between 0 and 1
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tLivingString2_setBrightness(tLivingString2* const p, float brightness)
{
    float h0=(1.0 + brightness) * 0.5f;
    float h1=(1.0 - brightness) * 0.25f;

    tTwoZero_setCoefficients(&p->bridgeFilter, h1, h0, h1);
    tTwoZero_setCoefficients(&p->nutFilter, h1, h0, h1);
    tTwoZero_setCoefficients(&p->prepFilterU, h1, h0, h1);
    tTwoZero_setCoefficients(&p->prepFilterL, h1, h0, h1);
}
void     tLivingString2_setDecay(tLivingString2* const p, float decay)
{
    p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tLivingString2_setTargetLev(tLivingString2* const p, float targetLev)
{
    tFeedbackLeveler_setTargetLevel(&p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLevL, targetLev);
}

void     tLivingString2_setLevSmoothFactor(tLivingString2* const p, float levSmoothFactor)
{
    tFeedbackLeveler_setFactor(&p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLevL, levSmoothFactor);
}

void     tLivingString2_setLevStrength(tLivingString2* const p, float levStrength)
{
    tFeedbackLeveler_setStrength(&p->fbLevU, levStrength);
    tFeedbackLeveler_setStrength(&p->fbLevL, levStrength);
}

void     tLivingString2_setLevMode(tLivingString2* const p, int levMode)
{
    tFeedbackLeveler_setMode(&p->fbLevU, levMode);
    tFeedbackLeveler_setMode(&p->fbLevL, levMode);
    p->levMode=levMode;
}
float   tLivingString2_tick(tLivingString2* const p, float input)
{
    input = input * 0.5f; // drop gain by half since we'll be equally adding it at half amplitude to forward and backward waveguides
    // from prepPos upwards=forwards
    float wLen=tExpSmooth_tick(&p->wlSmooth);

    float pickP=tExpSmooth_tick(&p->ppSmooth);

    //float pickupPos=tExpSmooth_tick(p->puSmooth);

    //need to determine which delay line to put it into (should be half amplitude into forward and backward lines for the correct portion of string)
    float prepP=tExpSmooth_tick(&p->prpSmooth);
    float lowLen=prepP*wLen;
    float upLen=(1.0f-prepP)*wLen;
    uint32_t pickPInt;

    if (pickP > prepP)
    {
        float fullPickPoint =  ((pickP*wLen) - lowLen);
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point
        float pickPfloat = fullPickPoint - pickPInt;

        tHermiteDelay_addTo(p->delUF, input * (1.0f - pickPfloat), pickPInt);
        tHermiteDelay_addTo(p->delUF, input * pickPfloat, pickPInt + 1);
        tHermiteDelay_addTo(p->delUB, input * (1.0f - pickPfloat), (uint32_t) (upLen - pickPInt));
        tHermiteDelay_addTo(p->delUB, input * pickPfloat, (uint32_t) (upLen - pickPInt - 1));
    }
    else
    {
        float fullPickPoint =  pickP * wLen;
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point
        float pickPfloat = fullPickPoint - pickPInt;

        tHermiteDelay_addTo(p->delLF, input * (1.0f - pickPfloat), pickPInt);
        tHermiteDelay_addTo(p->delLF, input * pickPfloat, pickPInt + 1);
        tHermiteDelay_addTo(p->delLB, input * (1.0f - pickPfloat), (uint32_t) (lowLen - pickPInt));
        tHermiteDelay_addTo(p->delLB, input * pickPfloat, (uint32_t) (lowLen - pickPInt - 1));
    }
    /*
        if (pickP > prepP)
        {
            float fullPickPoint =  ((pickP*wLen) - lowLen);
            pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

            tHermiteDelay_addTo(p->delUF, input, pickPInt);
            tHermiteDelay_addTo(p->delUB, input, (uint32_t) (upLen - pickPInt));
        }
        else
        {
            float fullPickPoint =  pickP * wLen;
            pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

            tHermiteDelay_addTo(p->delLF, input, pickPInt);
            tHermiteDelay_addTo(p->delLB, input, (uint32_t) (lowLen - pickPInt));
        }
    */
    float fromLF=tHermiteDelay_tickOut(p->delLF);
    float fromUF=tHermiteDelay_tickOut(p->delUF);
    float fromUB=tHermiteDelay_tickOut(p->delUB);
    float fromLB=tHermiteDelay_tickOut(p->delLB);
    fromLB = LEAF_clip(-1.0f, fromLB, 1.0f);
    fromUB = LEAF_clip(-1.0f, fromUB, 1.0f);
    fromUF = LEAF_clip(-1.0f, fromUF, 1.0f);
    fromLF = LEAF_clip(-1.0f, fromLF, 1.0f);

    // into upper half of string, from bridge, going backwards
    float fromBridge=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerU, tTwoZero_tick(&p->bridgeFilter, fromUF)));
    fromBridge = LEAF_clip(-1.0f, fromBridge, 1.0f);
    tHermiteDelay_tickIn(p->delUB, fromBridge);
    // into lower half of string, from prepPoint, going backwards
    float fromLowerPrep=-tTwoZero_tick(&p->prepFilterL, fromLF);
    fromLowerPrep = LEAF_clip(-1.0f, fromLowerPrep, 1.0f);
    float intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB; //used to add input here
    intoLower = LEAF_clip(-1.0f, intoLower, 1.0f);
    tHermiteDelay_tickIn(p->delLB, intoLower);
    // into lower half of string, from nut
    float fromNut=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tTwoZero_tick(&p->nutFilter, fromLB)));
    fromNut = LEAF_clip(-1.0f, fromNut, 1.0f);
    tHermiteDelay_tickIn(p->delLF, fromNut);
    // into upper half of string, from prepPoint, going forwards/upwards
    float fromUpperPrep=-tTwoZero_tick(&p->prepFilterU, fromUB);
    fromUpperPrep = LEAF_clip(-1.0f, fromUpperPrep, 1.0f);
    float intoUpper=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF;
    //intoUpper = tanhf(intoUpper);
    intoUpper = LEAF_clip(-1.0f, intoUpper, 1.0f);
    tHermiteDelay_tickIn(p->delUF, intoUpper);
    // update all delay lengths
    tHermiteDelay_setDelay(p->delLF, lowLen);
    tHermiteDelay_setDelay(p->delLB, lowLen);
    tHermiteDelay_setDelay(p->delUF, upLen);
    tHermiteDelay_setDelay(p->delUB, upLen);

    uint32_t PUPInt;
    float pickupOut = 0.0f;
    float pupos = tExpSmooth_tick(&p->puSmooth);
    if (pupos < 0.9999f)
    {
        if (pupos > prepP)
        {
            float fullPUPoint =  ((pupos*wLen) - lowLen);
            PUPInt = (uint32_t) fullPUPoint; // where does the input go? that's the pick point
            float PUPfloat = fullPUPoint - PUPInt;

            pickupOut = tHermiteDelay_tapOut(p->delUF, PUPInt) * (1.0f - PUPfloat);
            pickupOut += tHermiteDelay_tapOut(p->delUF, PUPInt + 1) * PUPfloat;
            pickupOut += tHermiteDelay_tapOut(p->delUB, (uint32_t) (upLen - PUPInt)) * (1.0f - PUPfloat);
            pickupOut += tHermiteDelay_tapOut(p->delUB, (uint32_t) (upLen - PUPInt - 1))  * PUPfloat;
        }
        else
        {
            float fullPUPoint =  pupos * wLen;
            PUPInt = (uint32_t) fullPUPoint; // where does the input go? that's the pick point
            float PUPfloat = fullPUPoint - PUPInt;

            pickupOut = tHermiteDelay_tapOut(p->delLF, PUPInt) * (1.0f - PUPfloat);
            pickupOut += tHermiteDelay_tapOut(p->delLF,  PUPInt + 1) * PUPfloat;
            pickupOut += tHermiteDelay_tapOut(p->delLB, (uint32_t) (lowLen - PUPInt)) * (1.0f - PUPfloat);
            pickupOut += tHermiteDelay_tapOut(p->delLB, (uint32_t) (lowLen - PUPInt - 1)) * PUPfloat;
        }

        p->curr = pickupOut;


        /*
                float fullPickupPos = (pupos*upLen);
                pickupPosInt = (uint32_t) fullPickupPos;
                float pickupPosfloat = fullPickupPos - pickupPosInt;
                if (pickupPosInt == 0)
                {
                    pickupPosInt = 1;
                }
                pickupOut = tHermiteDelay_tapOutInterpolated(p->delUF, pickupPosInt, pickupPosfloat);
                pickupOut += tHermiteDelay_tapOutInterpolated(p->delUB, (uint32_t) (upLen - pickupPosInt), pickupPosfloat);
                p->curr = pickupOut;
                */
    }
    else

    {
        p->curr = fromBridge;
    }

    //p->curr = fromBridge;
    //p->curr += fromNut;

    return p->curr;
}
float tLivingString2_tickEfficient(tLivingString2* const p, float input)
{
    input = input * 0.5f;

    float wLen = p->wlSmooth.dest;
    float pickP = p->ppSmooth.dest;

    float prepP = p->prpSmooth.dest;
    float lowLen = p->prpSmooth.dest * p->wlSmooth.dest;
    float upLen  = (1.0f - p->prpSmooth.dest) * p->wlSmooth.dest;

    uint32_t pickPInt;
    if (pickP > prepP)
    {
        float fullPickPoint = ((pickP * wLen) - lowLen);
        pickPInt = (uint32_t) fullPickPoint;

        tHermiteDelay_addTo(p->delUF, input, pickPInt);
        tHermiteDelay_addTo(p->delUB, input, (uint32_t) (upLen - pickPInt));
    }
    else
    {
        float fullPickPoint = pickP * wLen;
        pickPInt = (uint32_t) fullPickPoint;

        tHermiteDelay_addTo(p->delLF, input, pickPInt);
        tHermiteDelay_addTo(p->delLB, input, (uint32_t) (lowLen - pickPInt));
    }

    float fromLF = tHermiteDelay_tickOut(p->delLF);
    float fromUF = tHermiteDelay_tickOut(p->delUF);
    float fromUB = tHermiteDelay_tickOut(p->delUB);
    float fromLB = tHermiteDelay_tickOut(p->delLB);

    float fromBridge = -tFeedbackLeveler_tick(&p->fbLevU, tHighpass_tick(&p->DCblockerU, tTwoZero_tick(&p->bridgeFilter, fromUF)));
    tHermiteDelay_tickIn(p->delUB, fromBridge);

    float fromLowerPrep = -tTwoZero_tick(&p->prepFilterL, fromLF);
    float intoLower = (p->prepIndex * fromLowerPrep) + ((1.0f - p->prepIndex) * fromUB);
    tHermiteDelay_tickIn(p->delLB, intoLower);

    float fromNut = -tFeedbackLeveler_tick(&p->fbLevL, tHighpass_tick(&p->DCblockerL, tTwoZero_tick(&p->nutFilter, fromLB)));
    tHermiteDelay_tickIn(p->delLF, fromNut);

    float fromUpperPrep = -tTwoZero_tick(&p->prepFilterU, fromUB);
    float intoUpper = (p->prepIndex * fromUpperPrep) + ((1.0f - p->prepIndex) * fromLF);
    tHermiteDelay_tickIn(p->delUF, intoUpper);

    p->curr = fromBridge;
    return p->curr;
}

void tLivingString2_updateDelays(tLivingString2* const p)
{
    float lowLen = p->prpSmooth.dest * p->wlSmooth.dest;
    float upLen  = (1.0f - p->prpSmooth.dest) * p->wlSmooth.dest;

    tHermiteDelay_setDelay(p->delLF, lowLen);
    tHermiteDelay_setDelay(p->delLB, lowLen);
    tHermiteDelay_setDelay(p->delUF, upLen);
    tHermiteDelay_setDelay(p->delUB, upLen);
}
float   tLivingString2_sample(tLivingString2* const p)
{
    return p->curr;
}

void    tLivingString2_setSampleRate(tLivingString2* const p, float sr)
{
    p->sampleRate = sr;
    p->waveLengthInSamples = (p->sampleRate/p->freq) - 1;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tTwoZero_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tTwoZero_setSampleRate(&p->nutFilter, p->sampleRate);
    tTwoZero_setSampleRate(&p->prepFilterU, p->sampleRate);
    tTwoZero_setSampleRate(&p->prepFilterL, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblockerU, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblockerL, p->sampleRate);
}


/* Complex Living String*/
void tComplexLivingString_create(tMempool** const mp, tComplexLivingString** const bt)
{
    ALLOC_FROM_POOL(tComplexLivingString, bt, mp);
}
void tComplexLivingString_init(LEAF* const leaf, tComplexLivingString* const p,
                               float freq, float pickPos, float prepPos, float prepIndex,
                               float dampFreq, float decay,
                               float targetLev, float levSmoothFactor, float levStrength, int levMode)
{
    p->sampleRate = leaf->sampleRate;
    p->curr = 0.0f;

    tExpSmooth_init(leaf, &p->wlSmooth, p->sampleRate / freq, 0.01f);
    tComplexLivingString_setFreq(p, freq);
    p->freq = freq;

    tExpSmooth_init(leaf, &p->pickPosSmooth, pickPos, 0.01f);
    tExpSmooth_init(leaf, &p->prepPosSmooth, prepPos, 0.01f);

    tComplexLivingString_setPickPos(p, pickPos);
    tComplexLivingString_setPrepPos(p, prepPos);

    p->prepPos = prepPos;
    p->pickPos = pickPos;

    tLinearDelay_create(&p->mempool, &p->delLF);
    tLinearDelay_create(&p->mempool, &p->delMF);
    tLinearDelay_create(&p->mempool, &p->delUF);
    tLinearDelay_create(&p->mempool, &p->delUB);
    tLinearDelay_create(&p->mempool, &p->delMB);
    tLinearDelay_create(&p->mempool, &p->delLB);

    tLinearDelay_init(leaf, p->delLF, p->waveLengthInSamples, 2400);
    tLinearDelay_init(leaf, p->delMF, p->waveLengthInSamples, 2400);
    tLinearDelay_init(leaf, p->delUF, p->waveLengthInSamples, 2400);
    tLinearDelay_init(leaf, p->delUB, p->waveLengthInSamples, 2400);
    tLinearDelay_init(leaf, p->delMB, p->waveLengthInSamples, 2400);
    tLinearDelay_init(leaf, p->delLB, p->waveLengthInSamples, 2400);

    tLinearDelay_clear(p->delLF);
    tLinearDelay_clear(p->delMF);
    tLinearDelay_clear(p->delUF);
    tLinearDelay_clear(p->delUB);
    tLinearDelay_clear(p->delMB);
    tLinearDelay_clear(p->delLB);

    p->dampFreq = dampFreq;

    tOnePole_init(leaf, &p->bridgeFilter, dampFreq);
    tOnePole_init(leaf, &p->nutFilter, dampFreq);
    tOnePole_init(leaf, &p->prepFilterU, dampFreq);
    tOnePole_init(leaf, &p->prepFilterL, dampFreq);

    tHighpass_init(leaf, &p->DCblockerU, 13);
    tHighpass_init(leaf, &p->DCblockerL, 13);

    p->decay = decay;
    p->prepIndex = prepIndex;

    tFeedbackLeveler_init(leaf, &p->fbLevU, targetLev, levSmoothFactor, levStrength, levMode);
    tFeedbackLeveler_init(leaf, &p->fbLevL, targetLev, levSmoothFactor, levStrength, levMode);

    p->levMode = levMode;
}

void tComplexLivingString_free(tComplexLivingString** const pl)
{
    tComplexLivingString* p = *pl;

    tLinearDelay_free(&p->delLF);
    tLinearDelay_free(&p->delMF);
    tLinearDelay_free(&p->delUF);
    tLinearDelay_free(&p->delUB);
    tLinearDelay_free(&p->delMB);
    tLinearDelay_free(&p->delLB);

    mpool_free((char*)p, p->mempool);
}

void     tComplexLivingString_setFreq(tComplexLivingString* const p, float freq)
{    // NOTE: It is faster to set wavelength in samples directly
    if (freq<20.0f) freq=20.0f;
    else if (freq>10000.0f) freq=10000.0f;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}
void     tComplexLivingString_setWaveLength(tComplexLivingString* const p, float waveLength)
{
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.0f) waveLength=2400.0f;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tComplexLivingString_setPickPos(tComplexLivingString* const p, float pickPos)
{    // between 0 and 1
    if (pickPos<0.5f) pickPos=0.5f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(&p->pickPosSmooth, p->pickPos);
}

void     tComplexLivingString_setPrepPos(tComplexLivingString* const p, float prepPos)
{    // between 0 and 1
    if (prepPos<0.f) prepPos=0.f;
    else if (prepPos>0.5f) prepPos=0.5f;
    p->prepPos = prepPos;
    tExpSmooth_setDest(&p->prepPosSmooth, p->prepPos);
}

void     tComplexLivingString_setPrepIndex(tComplexLivingString* const p, float prepIndex)
{    // between 0 and 1
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tComplexLivingString_setDampFreq(tComplexLivingString* const p, float dampFreq)
{
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    tOnePole_setFreq(&p->nutFilter, dampFreq);
    tOnePole_setFreq(&p->prepFilterU, dampFreq);
    tOnePole_setFreq(&p->prepFilterL, dampFreq);
}

void     tComplexLivingString_setDecay(tComplexLivingString* const p, float decay)
{
    p->decay=decay;
}

void     tComplexLivingString_setTargetLev(tComplexLivingString* const p, float targetLev)
{
    tFeedbackLeveler_setTargetLevel(&p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLevL, targetLev);
}

void     tComplexLivingString_setLevSmoothFactor(tComplexLivingString* const p, float levSmoothFactor)
{
    tFeedbackLeveler_setFactor(&p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLevL, levSmoothFactor);
}

void     tComplexLivingString_setLevStrength(tComplexLivingString* const p, float levStrength)
{
    tFeedbackLeveler_setStrength(&p->fbLevU, levStrength);
    tFeedbackLeveler_setStrength(&p->fbLevL, levStrength);
}

void     tComplexLivingString_setLevMode(tComplexLivingString* const p, int levMode)
{
    tFeedbackLeveler_setMode(&p->fbLevU, levMode);
    tFeedbackLeveler_setMode(&p->fbLevL, levMode);
    p->levMode=levMode;
}

float   tComplexLivingString_tick(tComplexLivingString* const p, float input)
{
    // from pickPos upwards=forwards
    float fromLF=tLinearDelay_tickOut(p->delLF);
    float fromMF=tLinearDelay_tickOut(p->delMF);
    float fromUF=tLinearDelay_tickOut(p->delUF);
    float fromUB=tLinearDelay_tickOut(p->delUB);
    float fromMB=tLinearDelay_tickOut(p->delMB);
    float fromLB=tLinearDelay_tickOut(p->delLB);

    // into upper part of string, from bridge, going backwards
    float fromBridge=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(&p->DCblockerU, tOnePole_tick(&p->bridgeFilter, fromUF)));
    tLinearDelay_tickIn(p->delUB, fromBridge);

    // into pick position, take input and add it into the waveguide, going to come out of middle segment
    tLinearDelay_tickIn(p->delMB, fromUB+input);

    // into lower part of string, from prepPos, going backwards
    float fromLowerPrep=-tOnePole_tick(&p->prepFilterL, fromLF);
    float intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromMB;
    tLinearDelay_tickIn(p->delLB, intoLower);

    // into lower part of string, from nut, going forwards toward prep position
    float fromNut=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tOnePole_tick(&p->nutFilter, fromLB)));
    tLinearDelay_tickIn(p->delLF, fromNut);

    // into middle part of string, from prep going toward pick position
    float fromUpperPrep=-tOnePole_tick(&p->prepFilterU, fromMB);
    float intoMiddle=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF;

    //pick position, going to come out of middle segment
    tLinearDelay_tickIn(p->delMF, intoMiddle);

    //take output of middle segment and put it into upper segment connecting to the bridge, take input and add it into the waveguide,
    tLinearDelay_tickIn(p->delUF, fromMF + input);

    // update all delay lengths
    float pickP=tExpSmooth_tick(&p->pickPosSmooth);
    float prepP=tExpSmooth_tick(&p->prepPosSmooth);
    float wLen=tExpSmooth_tick(&p->wlSmooth);

    float midLen = (pickP-prepP) * wLen; // the length between the pick and the prep;
    float lowLen = prepP*wLen; // the length from prep to nut
    float upLen = (1.0f-pickP)*wLen; // the length from pick to bridge


    tLinearDelay_setDelay(p->delLF, lowLen);
    tLinearDelay_setDelay(p->delLB, lowLen);

    tLinearDelay_setDelay(p->delMF, midLen);
    tLinearDelay_setDelay(p->delMB, midLen);

    tLinearDelay_setDelay(p->delUF, upLen);
    tLinearDelay_setDelay(p->delUB, upLen);

    //update this to allow pickup position variation
    p->curr = fromBridge;
    return p->curr;
}

float   tComplexLivingString_sample(tComplexLivingString* const p)
{
    return p->curr;
}

void    tComplexLivingString_setSampleRate(tComplexLivingString* const p, float sr)
{
    float freq = p->waveLengthInSamples/p->sampleRate;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tOnePole_setSampleRate(&p->nutFilter, p->sampleRate);
    tOnePole_setSampleRate(&p->prepFilterU, p->sampleRate);
    tOnePole_setSampleRate(&p->prepFilterL, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblockerU, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblockerL, p->sampleRate);
}


void tBowed_create(tMempool** const mp, tBowed** const b)
{
    ALLOC_FROM_POOL(tBowed, b, mp);
}
void tBowed_init(LEAF* const leaf, tBowed* const x, int oversampling)
{
    tMempool* mp = x->mempool;

    x->x_bp   = 01.6f;
    x->x_bpos = 0.2f;
    x->x_bv   = .9f;
    x->x_fr   = 440.f;
    x->oversampling = oversampling;
    x->sampleRate = leaf->sampleRate * (float)oversampling;
    x->invSampleRate = 1.f / x->sampleRate;

    tLinearDelay_create(&mp, &x->neckDelay);
    tLinearDelay_init(leaf, x->neckDelay, 100.0f, 2400.0f);
    tLinearDelay_create(&mp, &x->bridgeDelay);
    tLinearDelay_init(leaf, x->bridgeDelay, 29.0f, 2400.0f);

    tLinearDelay_clear(x->neckDelay);
    tLinearDelay_clear(x->bridgeDelay);

    tCookOnePole_init(leaf, &x->reflFilt);
    tCookOnePole_setSampleRate(&x->reflFilt, x->sampleRate);

    tCookOnePole_setPole(&x->reflFilt, 0.6f - (0.1f * 22050.f / x->sampleRate));
    tCookOnePole_setGain(&x->reflFilt, .95f);

    tBowTable_init(leaf, &x->bowTabl);
    x->bowTabl.slope = 3.0f;

    tBowed_setFreq(x, x->x_fr);

    tSVF_init(leaf, &x->lowpass, SVFTypeLowpass, 18000.0f, 0.6f);
    tSVF_setSampleRate(&x->lowpass, x->sampleRate);
    tSVF_setFreq(&x->lowpass, 16000.0f);

    x->betaRatio = 0.127236f;
    x->fr_save = x->x_fr;
}

void    tBowed_free                  (tBowed** const bw)
{
    tBowed* x = *bw;
    tLinearDelay_free(&x->neckDelay);
    tLinearDelay_free(&x->bridgeDelay);
    mpool_free((char*)x, x->mempool);
}

float   tBowed_tick  (tBowed* const x)
{
    float bp = x->x_bp;
    float bpos = x->x_bpos;
    float bv = x->x_bv;
    float fr = x->x_fr;
    float nutRefl, newVel, velDiff, stringVel, bridgeRefl;

    if (fr != x->fr_save) {
        tBowed_setFreq(x, fr);
        x->fr_save = fr;
    }

    x->bowTabl.slope = bp;

    if (bpos != x->betaRatio) {
        x->betaRatio = bpos;
        tLinearDelay_setDelay(x->bridgeDelay, x->baseDelay * x->betaRatio);
        tLinearDelay_setDelay(x->neckDelay, x->baseDelay * (1.f - x->betaRatio));
    }

    for (int i = 0; i < x->oversampling; i++) {
        bridgeRefl = -tCookOnePole_tick(&x->reflFilt, x->bridgeDelay->lastOut);
        nutRefl = x->neckDelay->lastOut;
        stringVel = bridgeRefl + nutRefl;
        velDiff = bv - stringVel;
        newVel = velDiff * tBowTable_lookup(&x->bowTabl, velDiff);
        tLinearDelay_tick(x->neckDelay, bridgeRefl + newVel);
        tLinearDelay_tick(x->bridgeDelay, nutRefl + newVel);
        x->output = tSVF_tick(&x->lowpass, x->bridgeDelay->lastOut);
    }

    return x->output;
}

void    tBowed_setFreq               (tBowed* const x, float freq)
{
    if (freq < 20.f)
        freq = 20.f;
    x->baseDelay = (x->sampleRate / freq) - 4.0f;
    tLinearDelay_setDelay(x->bridgeDelay, x->baseDelay * x->betaRatio);
    tLinearDelay_setDelay(x->neckDelay, x->baseDelay * (1.f - x->betaRatio));
}



void tTString_create(tMempool** const mp, tTString** const b)
{
    ALLOC_FROM_POOL(tTString, b, mp);
}
void tTString_init(LEAF* const leaf, tTString* const x, int oversampling, float lowestFreq)
{
    tMempool* mp = x->mempool;

    x->oversampling = oversampling;
    x->invOversampling = 1.0f / (float)oversampling;
    x->invOversamplingTimesTwo = x->invOversampling * 2.0f;
    x->sampleRate = leaf->sampleRate * (float)oversampling;
    x->invSampleRate = 1.f / x->sampleRate;
    x->invSampleRateTimesTwoPi = TWO_PI * x->invSampleRate;
    x->tensionGain = 0.0f;
    x->allpassDelay = 0.f;
    x->decayCoeff = 1.0f;
    x->muteCoeff = 1.0f;
    x->output = 0.0f;
    x->outputP = 0.0f;
    x->rippleRate = 1.0f;
    x->r = 0.0f;
    x->invOnePlusr = 1.0f;
    x->decayInSeconds = 10.0f;
    x->harmonic = 1.0f;
    x->baseDelay = 100.0f;

    x->tensionAmount = 0.0f;

    x->pickupPos = 0.0f;
    x->slideGain = 0.4f;
    x->barDrive = 1.5f;
    x->wound = 1;
    x->barPulsePhasor = 0;

    float maxDelayTemp = x->sampleRate / lowestFreq;

    tLagrangeDelay_create(&mp, &x->delay);
    tLagrangeDelay_init(leaf, x->delay, 100.0f, maxDelayTemp);
    x->actualLowestFreq = x->sampleRate / tLagrangeDelay_getMaxDelay(x->delay);
    tLagrangeDelay_clear(x->delay);

    tLagrangeDelay_create(&mp, &x->delayP);
    tLagrangeDelay_init(leaf, x->delayP, 100.0f, maxDelayTemp);
    tLagrangeDelay_clear(x->delayP);

    x->maxDelay = tLagrangeDelay_getMaxDelay(x->delay);

    tCookOnePole_init(leaf, &x->reflFilt);
    tCookOnePole_setSampleRate(&x->reflFilt, x->sampleRate);
    tCookOnePole_init(leaf, &x->reflFiltP);
    tCookOnePole_setSampleRate(&x->reflFiltP, x->sampleRate);

    tCookOnePole_setGainAndPole(&x->reflFilt, 0.999f, -0.0014f);
    tCookOnePole_setGainAndPole(&x->reflFiltP, 0.999f, -0.0014f);

    tTString_setFreq(x, 440.0f);

    tExpSmooth_init(leaf, &x->tensionSmoother, 0.0f, 0.004f * x->invOversampling);
    tExpSmooth_setDest(&x->tensionSmoother, 0.0f);
    tExpSmooth_init(leaf, &x->pitchSmoother, 100.0f, 0.04f * x->invOversampling);
    tExpSmooth_setDest(&x->pitchSmoother, 100.0f);
    tExpSmooth_init(leaf, &x->pickNoise, 0.0f, 0.09f * x->invOversampling);
    tExpSmooth_setDest(&x->pickNoise, 0.0f);

    tThiranAllpassSOCascade_create(&mp, &x->allpass);
    tThiranAllpassSOCascade_init(leaf, x->allpass, 4);
    tThiranAllpassSOCascade_create(&mp, &x->allpassP);
    tThiranAllpassSOCascade_init(leaf, x->allpassP, 4);

    x->allpassDelay = tThiranAllpassSOCascade_setCoeff(x->allpass, 0.0001f, 100.0f, x->invOversampling);
    x->allpassDelayP = tThiranAllpassSOCascade_setCoeff(x->allpassP, 0.000025f, 100.0f, x->invOversampling);

    tSVF_init(leaf, &x->lowpassP, SVFTypeLowpass, 5000.0f, 0.8f);
    tSVF_setSampleRate(&x->lowpassP, x->sampleRate);
    tSVF_setFreq(&x->lowpassP, 6000.0f);
    tSVF_init(leaf, &x->highpassP, SVFTypeHighpass, 1800.0f, 0.8f);
    tSVF_setSampleRate(&x->highpassP, x->sampleRate);
    tSVF_setFreq(&x->highpassP, 1800.0f);

    x->twoPiTimesInvSampleRate = TWO_PI * x->invSampleRate;

    tCycle_init(leaf, &x->tensionModOsc);
    tCycle_setSampleRate(&x->tensionModOsc, x->sampleRate);
    tCycle_init(leaf, &x->pickupModOsc);
    tCycle_setSampleRate(&x->pickupModOsc, x->sampleRate);

    x->pickupModOscFreq = 440.0f;
    x->pickupModOscAmp = 1.0f;

    tSVF_init(leaf, &x->pickupFilter, SVFTypeLowpass, 2900.0f, 1.0f);
    tSVF_setSampleRate(&x->pickupFilter, x->sampleRate);
    tSVF_setFreq(&x->pickupFilter, 3900.0f);

    tSVF_init(leaf, &x->pickupFilter2, SVFTypeLowpass, 3800.0f, 1.1f);
    tSVF_setSampleRate(&x->pickupFilter2, x->sampleRate);
    tSVF_setFreq(&x->pickupFilter2, 4100.0f);

    tSVF_init(leaf, &x->peakFilt, SVFTypePeak, 1000.0f, .9f);
    tSVF_setSampleRate(&x->peakFilt, x->sampleRate);
    tSVF_setFreq(&x->peakFilt, 1000.0f);

    tNoise_init(leaf, &x->noise, PinkNoise);

    tHighpass_init(leaf, &x->dcBlock, 1.0f);
    tHighpass_setSampleRate(&x->dcBlock, x->sampleRate);
    tHighpass_init(leaf, &x->dcBlockP, 1.0f);
    tHighpass_setSampleRate(&x->dcBlockP, x->sampleRate);

    tSlide_init(leaf, &x->slide, 0, 3000);

    if (x->wound)
    {
        tExpSmooth_init(leaf, &x->barSmooth2, 0.0f, 0.0005f);
        tExpSmooth_setDest(&x->barSmooth2, 0.0f);
        tExpSmooth_init(leaf, &x->barSmoothVol, 0.0f, 0.0008f);
        tExpSmooth_setDest(&x->barSmoothVol, 0.0f);
    }
    else
    {
        tExpSmooth_init(leaf, &x->barSmooth2, 0.0f, 0.005f);
        tExpSmooth_setDest(&x->barSmooth2, 0.0f);
        tExpSmooth_init(leaf, &x->barSmoothVol, 0.0f, 0.004f);
        tExpSmooth_setDest(&x->barSmoothVol, 0.0f);
    }

    tSlide_init(leaf, &x->barSmooth, 1000, 1000);
    tSlide_init(leaf, &x->barPulseSlide, 2, 30);
    tExpSmooth_init(leaf, &x->barPulse, 0.0f, 0.05f);
    tExpSmooth_setDest(&x->barPulse, 0.0f);

    tSVF_init(leaf, &x->barResonator, SVFTypeBandpass, 5.0f, 15.0f);

    x->timeSinceLastBump = 1;

    tHighpass_init(leaf, &x->barHP, 30.0f);
    tSVF_init(leaf, &x->barLP, SVFTypeLowpass, 7000.0f, 0.9f);

    x->inharmonic = 1;
    x->inharmonicMult = 1.0f;

    tFeedbackLeveler_init(leaf, &x->feedback, 0.25f, 0.04f * x->invOversampling, 0.1f, 1);
    tFeedbackLeveler_init(leaf, &x->feedbackP, 0.25f, 0.04f * x->invOversampling, 0.1f, 1);

    x->quarterSampleRate = x->sampleRate * 0.245f;
    x->windingsPerInch = 70.0f;

    tNoise_init(leaf, &x->pickNoiseSource, PinkNoise);
    x->pickupAmount = 0.0f;

    tPickupNonLinearity_init(leaf, &x->p);
}

void    tTString_free (tTString** const bw)
{
    tTString* x = *bw;
    tThiranAllpassSOCascade_free(&x->allpassP);
    tThiranAllpassSOCascade_free(&x->allpass);
    tLagrangeDelay_free(&x->delayP);
    tLagrangeDelay_free(&x->delay);
    mpool_free((char*)x, x->mempool);
}

float   tTString_tick                  (tTString* const x)
{
    float theOutput = 0.0f;
    x->feedbackNoise = tNoise_tick(&x->noise);

    float barPos = x->barPosition;
    if ((barPos > (x->prevBarPosSmoothed + 3.0f)) || (barPos < (x->prevBarPosSmoothed - 3.0f)))
	{
    	tExpSmooth_setValAndDest(&x->barSmooth2,barPos);
	}
    else
    {
    	tExpSmooth_setDest(&x->barSmooth2,barPos);
    }

    //float currentBumpSmoothed = currentBump;
    float barPosSmoothed = tExpSmooth_tick(&x->barSmooth2);

    float barDifference = fastabsf(barPosSmoothed - x->prevBarPosSmoothed);


    tExpSmooth_setDest(&x->barSmoothVol,barPos);
        float barPosSmoothVol = tExpSmooth_tick(&x->barSmoothVol);
        float differenceFastSmooth = fastabsf(barPosSmoothVol - x->prevBarPosSmoothVol);
        x->prevBarPosSmoothVol = barPosSmoothVol;
        float volCut = 1.0f;
        if (differenceFastSmooth < 0.0001f)
        {
        	//volCut = LEAF_map(differenceFastSmooth, 0.00001f, 0.0001f, 0.0f, 1.0f);
        	volCut = ((differenceFastSmooth - 0.00001f) * 1111.111111111111111f);
        	if (volCut < 0.0f)
        	{
        		volCut = 0.0f;
        	}
        }
    float currentBump = barPosSmoothed * x->windingsPerInch;
    float bumpDifference = fastabsf(currentBump - x->lastBump);

    x->prevBarPosSmoothed = barPosSmoothed;
    barDifference = LEAF_clip(0.0f, barDifference*2.0f, 1.0f);
    x->slideAmount = tSlide_tick(&x->slide, barDifference);
    if (x->wound)
    {
		if (bumpDifference > 1.0f)
		{
			tExpSmooth_setVal(&x->barPulse, 1.0f);

			tSlide_setDest(&x->barPulseSlide, 1.0f);

			x->lastBump = currentBump;
			x->timeSinceLastBump = x->sampleCount+1 / bumpDifference;
			if (x->timeSinceLastBump < 100)
			{
				x->bumpOsc = 1;
			}
			else
			{
				x->bumpOsc = 0;
			}

			tSlide_setDest(&x->barSmooth, (x->sampleRate * x->invOversampling / x->timeSinceLastBump));
			x->sampleCount = 0;
		}
		else
		{
			x->bumpOsc = 0;
		}

		if (x->bumpOsc)
		{
			if (x->bumpCount > x->timeSinceLastBump)
			{
				tExpSmooth_setVal(&x->barPulse, 1.0f);
				tSlide_setDest(&x->barPulseSlide, 1.0f);
				x->bumpCount = 0;
			}
			x->bumpCount++;
		}

		if (x->sampleCount < 3000)
		{
			x->sampleCount++;

		}
		else
		{
			tSlide_setDest(&x->barSmooth, 5.0f);
		}
		float barFreq = tSlide_tickNoInput(&x->barSmooth);
		tSVF_setFreq(&x->barResonator, barFreq);
		float filterAmount = 0.5f;
		float dryAmount = 1.0f;
		x->slideNoise = x->feedbackNoise;

		float pulseVal = tSlide_tickNoInput(&x->barPulseSlide);

		if (pulseVal > .99f)
		{
			tSlide_setDest(&x->barPulseSlide, 0.0f);

		}

		x->slideNoise *= pulseVal;
		//x->slideNoise = tExpSmooth_tick(x->barPulse);
		x->slideNoise *= tExpSmooth_tick(&x->barPulse);
		//x->slideNoise *= x->slideGain;
		float tempSlideNoise1 = x->slideNoise;
		float tempSlideNoise2 = fast_tanh5(tSVF_tick(&x->barResonator, tempSlideNoise1)*x->barDrive);

		float filterFade = 1.0f;
		if (barFreq < 300.0f)
		{
			//filterFade = LEAF_map(barFreq, 100.0f, 300.0f, 0.0f, 1.0f);
			filterFade = (barFreq - 100.0f) * 0.005f;
			filterFade = LEAF_clip(0.0f, filterFade, 1.0f);
		}

		x->slideNoise = ((tempSlideNoise1 * dryAmount) + (tempSlideNoise2 * filterAmount * filterFade));// * x->slideAmount;
		x->slideNoise *= x->muted;
		x->slideNoise = tHighpass_tick(&x->barHP, x->slideNoise);
		x->slideNoise = tSVF_tick(&x->barLP, x->slideNoise * x->slideGain * volCut);
		x->slideNoise = x->slideNoise * x->slideGain;
    }
    else
    {
		//x->slideNoise = tNoise_tick(x->pickNoiseSource)*0.3f;
    	x->slideNoise = x->feedbackNoise;
		x->slideNoise *= x->muted;
		x->slideNoise = tHighpass_tick(&x->barHP, x->slideNoise);
		x->slideNoise = tSVF_tick(&x->barLP, x->slideNoise * x->slideGain * 0.5f * volCut);
		x->slideNoise = x->slideNoise * x->slideGain;
    }
    for (int i = 0; i < x->oversampling; i++)
    {
    	x->baseDelay = tExpSmooth_tick(&x->pitchSmoother);

    	float tension = tExpSmooth_tick(&x->tensionSmoother) * x->tensionGain * x->baseDelay;


    	//x->tensionAmount = (tension + (tCycle_tick(x->tensionModOsc) * tension * 0.0f)) * 0.01f;
    	x->tensionAmount = tension * 0.01f;
    	float currentDelay = x->baseDelay;

    	//float currentDelay = x->baseDelay;
        if (currentDelay < 5.0f)
        {
        	currentDelay = 5.0f;
        }

        if (currentDelay > x->maxDelay)
        {
        	currentDelay = x->maxDelay;
        }
        //tCycle_setFreq(x->tensionModOsc, (x->sampleRate / (currentDelay - x->tensionAmount)) * 2.0f);
        float halfCurrentDelay = currentDelay * 0.5f;

        //tLagrangeDelay_setDelay(x->delay, x->wavelength-2);//currentDelay - x->tensionAmount - (x->allpassDelay * x->inharmonicMult));// - ((x->phaseComp * INV_TWO_PI ) * currentDelay));
        //tLagrangeDelay_setDelay(x->delayP,x->wavelength-2);// currentDelay - x->tensionAmount - (x->allpassDelayP * x->inharmonicMult));// - ((x->phaseComp * INV_TWO_PI ) * currentDelay));
        tLagrangeDelay_setDelay(x->delay, currentDelay - x->tensionAmount - (x->allpassDelay * x->inharmonicMult) - ((x->phaseComp * INV_TWO_PI ) * currentDelay));
        tLagrangeDelay_setDelay(x->delayP,currentDelay - x->tensionAmount - (x->allpassDelayP * x->inharmonicMult) - ((x->phaseComp * INV_TWO_PI ) * currentDelay));


        float pickNoiseSound = tNoise_tick(&x->pickNoiseSource) * tExpSmooth_tick(&x->pickNoise);
        tLagrangeDelay_addTo(x->delay,pickNoiseSound * 2.0f, (uint32_t)x->pluckPoint_forInput);
        tLagrangeDelay_addTo(x->delayP,pickNoiseSound * 2.0f, (uint32_t)x->pluckPoint_forInput);

        float filterOut = tCookOnePole_tick(&x->reflFilt, x->output);
        float filterOutP = tCookOnePole_tick(&x->reflFiltP, x->outputP);
        //float filterOut = x->output;
        //float filterOutP = x->outputP;

        float rippleLength = x->rippleRate *  currentDelay;
        //float rippleLength = 0.0f;
        uint32_t rippleLengthInt = (uint32_t) rippleLength;
        float rippleLengthAlpha = rippleLength - rippleLengthInt;

       // float rippleLength2 = currentDelay - rippleLength;
        //uint32_t rippleLengthInt2 = (uint32_t) rippleLength2;
        //float rippleLengthAlpha2 = rippleLength2 - rippleLengthInt2;


        //float rippleOut = (tLagrangeDelay_tapOutInterpolated(x->delay,rippleLengthInt,rippleLengthAlpha) * x->r);
        //delayOut -= (tLagrangeDelay_tapOutInterpolated(x->delay,rippleLengthInt2,rippleLengthAlpha2) * x->r);
        float delayOut = tLagrangeDelay_tickOut(x->delay);
        float delayOutP = tLagrangeDelay_tickOut(x->delayP);
        float rippleOut1 = (tLagrangeDelay_tapOutInterpolated(x->delay,rippleLengthInt,rippleLengthAlpha) * x->r);
        float rippleOut1P = (tLagrangeDelay_tapOutInterpolated(x->delayP,rippleLengthInt,rippleLengthAlpha) * x->r);


        //float rippleSampleP = (tLagrangeDelay_tapOutInterpolated(x->delayP,rippleLengthInt,rippleLengthAlpha) * x->r);
        //delayOutP += rippleSampleP;
        //float outputP = tLagrangeDelay_tickOut(x->delayP);

        //filterOut = rippleOut1 + rippleOut2;
        float rippleOut = delayOut + rippleOut1;
        float rippleOutP = delayOutP + rippleOut1P;
       // outputP += (tLagrangeDelay_tapOutInterpolated(x->delayP,rippleLengthInt,rippleLengthAlpha) * x->r);


        //output += (tLagrangeDelay_tapOutInterpolated(x->delay,rippleLengthInt2,rippleLengthAlpha2) * x->r);
        //output += (tLagrangeDelay_tapOutInterpolated(x->delayP,rippleLengthInt2,rippleLengthAlpha2) * x->r);
        float allpassOut = rippleOut;
        float allpassOutP = rippleOutP;
        if (x->inharmonic)
        {
        	allpassOut = tThiranAllpassSOCascade_tick(x->allpass, rippleOut);
        	allpassOutP = tThiranAllpassSOCascade_tick(x->allpassP, rippleOutP);
        }


        x->output = LEAF_clip(-1.0f, (allpassOut * (x->decayCoeff * x->invOnePlusr) * x->muteCoeff), 1.0f);
        x->outputP = LEAF_clip(-1.0f, (allpassOutP * (x->decayCoeff * x->invOnePlusr) * x->muteCoeff), 1.0f);


        //if past the string ends, need to read backwards and invert
        uint32_t backwards = 0;

        float positionMin = (x->openStringLength * 0.104166666666667f);
        float positionMax = (x->openStringLength * 0.021666666666667f);
        float pickupInput = x->pickupPos + (tCycle_tick(&x->pickupModOsc) * x->pickupModOscAmp);
        //float pickupInput = x->pickupPos;
        float pickupPos = LEAF_mapFromZeroToOneInput(pickupInput, positionMin, positionMax);

        while (pickupPos > halfCurrentDelay)
        {
        	pickupPos = pickupPos - halfCurrentDelay;
        	backwards = !backwards;
        }
        while (pickupPos < 0.0f)
        {
        	pickupPos = pickupPos + halfCurrentDelay;
        	backwards = !backwards;
        }
        float pickupPosFinal = pickupPos;
        if (backwards)
        {
        	pickupPosFinal = halfCurrentDelay - pickupPos;
        }

        x->pickup_Ratio = pickupPosFinal / halfCurrentDelay;
        uint32_t pickupPosInt = (uint32_t) pickupPosFinal;
        uint32_t pickupPosIntPlusOne = pickupPosInt + 1;
        float pickupPosAlpha = pickupPosFinal - pickupPosInt;
        float oneMinusPickupPosAlpha = 1.0f - pickupPosAlpha;
        float inversePickupPos = currentDelay - pickupPos;
        if (backwards)
        {
        	inversePickupPos = halfCurrentDelay + pickupPos;
        }
        uint32_t inversePickupPosInt = (uint32_t) inversePickupPos;
        uint32_t inversePickupPosIntPlusOne = inversePickupPosInt + 1;
        float inversePickupPosAlpha = inversePickupPos - inversePickupPosInt;
        float oneMinusinversePickupPosAlpha = 1.0f - inversePickupPosAlpha;

        float rightgoing = tLagrangeDelay_tapOut(x->delay, pickupPosInt) * oneMinusPickupPosAlpha;
        rightgoing += tLagrangeDelay_tapOut(x->delay, pickupPosIntPlusOne) * pickupPosAlpha;
        float leftgoing = tLagrangeDelay_tapOut(x->delay, inversePickupPosInt) * oneMinusinversePickupPosAlpha;
        leftgoing += tLagrangeDelay_tapOut(x->delay, inversePickupPosIntPlusOne) * inversePickupPosAlpha;

        //x->pickupOut =  (((volumes[1] * pickupNonLinearity(rightgoing)) + (volumes[0] * rightgoing)) - ((volumes[1] * pickupNonLinearity(leftgoing)) + (volumes[0] * leftgoing)));
        x->pickupOut =  (pickupNonLinearity(rightgoing) - pickupNonLinearity(leftgoing));
        rightgoing = tLagrangeDelay_tapOut(x->delayP, pickupPosInt) * oneMinusPickupPosAlpha;
		rightgoing += tLagrangeDelay_tapOut(x->delayP, pickupPosIntPlusOne) * pickupPosAlpha;
		leftgoing = tLagrangeDelay_tapOut(x->delayP, inversePickupPosInt) * oneMinusinversePickupPosAlpha;
		leftgoing += tLagrangeDelay_tapOut(x->delayP, inversePickupPosIntPlusOne) * inversePickupPosAlpha;

		x->pickupOutP =  (pickupNonLinearity(rightgoing) - pickupNonLinearity(leftgoing));

		float inputSample = tFeedbackLeveler_tick(&x->feedback, tHighpass_tick(&x->dcBlock, filterOut + x->slideNoise + x->feedbackNoise * x->feedbackNoiseLevel));
		float inputSampleP = tFeedbackLeveler_tick(&x->feedbackP, tHighpass_tick(&x->dcBlockP, filterOutP + x->slideNoise + x->feedbackNoise * x->feedbackNoiseLevel));
        tLagrangeDelay_tickIn(x->delay, inputSample);
        tLagrangeDelay_tickIn(x->delayP, inputSampleP);
        //tLagrangeDelay_tickIn(x->delayP, delayOutP + x->slideNoise);
        //float outputPfilt = 0.0f;
        float outputPfilt = tSVF_tick(&x->lowpassP, x->pickupOutP);
        outputPfilt = tSVF_tick(&x->highpassP, outputPfilt);
        float volumes[2];
        LEAF_crossfade(x->pickupAmount * 2.0f - 1.0f,volumes);
        float prefilter = (x->pickupOut + (outputPfilt * x->phantomGain)) * 2.0f;
        float prefilter2 = tSVF_tick(&x->pickupFilter2, prefilter);// + x->slideNoise;
        float prefilter3 = tSVF_tick(&x->pickupFilter, prefilter2);// + x->slideNoise;
        float prefilter4 = tSVF_tick(&x->peakFilt, prefilter3);// + x->slideNoise;

        theOutput = (prefilter4 * 1.3f* volumes[1]) + (prefilter * 1.3f * volumes[0]);
    }
    return theOutput;
}

void    tTString_setFreq               (tTString* const x, float freq)
{
    if (freq < x->actualLowestFreq)
        freq = x->actualLowestFreq;
    //freq = freq * x->invOversampling;
    x->freq = freq;

    float Fc = (freq*x->invSampleRateTimesTwoPi);

    //tExpSmooth_setDest(x->pitchSmoother, (x->sampleRate / freq) - 1.0f);
    tExpSmooth_setDest(&x->pitchSmoother, (x->sampleRate / freq) - 2.0f); //why 2? is it the combo of the lowpass one pole and highpass one pole?

    //tCycle_setFreq(x->tensionModOsc, freq * x->invOversamplingTimesTwo);
#ifdef ARM_MATH_CM7
    float result = 0.0f;
    arm_atan2_f32(x->poleCoeff *  arm_sin_f32(Fc) , 1.0f - (x->poleCoeff *  arm_cos_f32(Fc)), &result);
     x->phaseComp = result;
    //if I can figure out how to use a newer CMSIS DSP pack, the new version 1.15 has an atan2f approximation that's likely faster
#else
    x->phaseComp = atan2f((x->poleCoeff * sinf(Fc)) , (1.0f - (x->poleCoeff * cosf(Fc))));
#endif
}

void    tTString_setDecay               (tTString* const x, float decay)
{
	 x->decayInSeconds = (decay * 20.0f) + 0.1f;
	 if (x->decayInSeconds > 20.0f)
	 {
		 x->decayCoeff = 1.0f;
	 }
	 else
	 {
		 decay = fastExp3(-1.0f / (x->freq * x->decayInSeconds));
		 //decay = LEAF_map(decay, 0.0f, 1.0f, 0.99f, 1.01f);
		 decay = LEAF_clip(0.1f, decay, 1.0f);
		 x->decayCoeff = decay;
	 }
}

void    tTString_setDecayInSeconds               (tTString* const x, float decay)
{
	 x->decayInSeconds = decay;
	 if (x->decayInSeconds > 20.0f)
	 {
		 x->decayCoeff = 1.0f;
	 }
	 else
	 {
		 decay = fastExp3(-1.0f / (x->freq * x->decayInSeconds));
		 //decay = LEAF_map(decay, 0.0f, 1.0f, 0.99f, 1.01f);
		 decay = LEAF_clip(0.1f, decay, 1.0f);
		 x->decayCoeff = decay;
	 }
}

void    tTString_setPickupPos               (tTString* const x, float pos)
{
	x->pickupPos = pos;
}

void    tTString_setPickupAmount               (tTString* const x, float amount)
{
	x->pickupAmount = amount;
}

void    tTString_setPickupModAmp               (tTString* const x, float amp)
{
	x->pickupModOscAmp = amp;
}


void    tTString_setPickupModFreq               (tTString* const x, float freq)
{
	x->pickupModOscFreq = freq;
	tCycle_setFreq(&x->pickupModOsc, freq);
}



void    tTString_mute              (tTString* const x)
{
	 x->muteCoeff = fastExp3(-1.0f / (x->freq * 0.007f));;
	 if (x->muteCoeff < 0.5f)
	 {
		 x->muteCoeff = 0.5f;
	 }
	 x->muted = 0.0f;
}

//0-1
void    tTString_setFilter              (tTString* const x, float filter)
{
	//filter = ((filter * 18000.0f) + 20.0f)* x->twoPiTimesInvSampleRate;
	 //float lowestNumOvertones = LEAF_map(x->freq, 20.0f, 3000.0f, 15.0f, 12.0f);
	// LEAF_clip(7.0f, lowestNumOvertones, 10.0f);
	 float overtone = LEAF_mapFromZeroToOneInput(filter, 14.0f, 100.0f);

	 float freqToUse = (overtone * x->freq);

	freqToUse = LEAF_clip(10.0f, freqToUse, x->quarterSampleRate);
	x->filterFreq = freqToUse;
#ifdef ARM_MATH_CM7
	filter = 1.0f - arm_sin_f32(freqToUse * x->twoPiTimesInvSampleRate);
#else
	filter = 1.0f - sinf(freqToUse * x->twoPiTimesInvSampleRate);
#endif
	//volatile float normalizedFreq = (overtone * x->freq) * x->twoPiTimesInvSampleRate;
	//filter = 1.0f - normalizedFreq;

	tCookOnePole_setPole(&x->reflFilt,filter);
	tCookOnePole_setPole(&x->reflFiltP,filter);
	x->poleCoeff = filter;
}

void    tTString_setFilterFreqDirectly              (tTString* const x, float freq)
{
	float freqToUse = LEAF_clip(10.0f, freq, x->quarterSampleRate);
	x->filterFreq = freqToUse;
#ifdef ARM_MATH_CM7
	freq = 1.0f - arm_sin_f32(freqToUse * x->twoPiTimesInvSampleRate);
#else
	freq = 1.0f - sinf(freqToUse * x->twoPiTimesInvSampleRate);
#endif
	//volatile float normalizedFreq = (overtone * x->freq) * x->twoPiTimesInvSampleRate;
	//filter = 1.0f - normalizedFreq;

	tCookOnePole_setPole(&x->reflFilt,freq);
	tCookOnePole_setPole(&x->reflFiltP,freq);
	x->poleCoeff = freq;
}

void   tTString_setTensionGain                  (tTString* const x, float tensionGain)
{
	x->tensionGain = tensionGain;
}

//0-1
void   tTString_setBarDrive                  (tTString* const x, float drive)
{
	x->barDrive = drive + 1.0f;
}

void   tTString_setBarPosition                  (tTString* const x, float barPosition)
{
	x->prevBarPosition = x->barPosition;
	x->barPosition = barPosition;
}

void   tTString_setOpenStringFrequency                  (tTString* const x, float openStringFrequency)
{
	x->openStringLength = ((x->sampleRate / openStringFrequency) - 2.0f);
}

void   tTString_setPickupRatio                  (tTString* const x, float ratio)
{
	x->pickupRatio = ratio;
}

void   tTString_setPhantomHarmonicsGain                  (tTString* const x, float gain)
{
	x->phantomGain = gain;
}

void   tTString_setSlideGain                  (tTString* const x, float gain)
{
	x->slideGain = gain;
}

void   tTString_setTensionSpeed                  (tTString* const x, float tensionSpeed)
{
	tExpSmooth_setFactor(&x->tensionSmoother, 0.00064f * x->invOversampling * (1.0f - tensionSpeed) + 0.00001f); //0.5 is good
}


void   tTString_setRippleDepth                  (tTString* const x, float depth)
{
	x->r = fastExp3(-1.0f / (x->freq * (x->decayInSeconds * 0.005f + 0.0001f))) * depth * 0.5f;
	x->invOnePlusr = 1.0f / (1.0f + x->r);
	float highpassFreq = depth * 30.0f + 1.0f;
	tHighpass_setFreq(&x->dcBlock,highpassFreq);
	tHighpass_setFreq(&x->dcBlockP,highpassFreq);
}

void   tTString_setHarmonic                  (tTString* const x, float harmonic)
{
	x->harmonic = harmonic;
	x->rippleRate = 1.0f / harmonic;
}
void   tTString_setFeedbackStrength                  (tTString* const x, float strength)
{
	float strengthVal = 0.5f * strength;
	tFeedbackLeveler_setStrength(&x->feedback, strengthVal);
	tFeedbackLeveler_setStrength(&x->feedbackP, strengthVal);
	x->feedbackNoiseLevel = strength * 0.005f;

}
void   tTString_setFeedbackReactionSpeed                  (tTString* const x, float speed)
{
	float speedVal = speed * 0.1f + 0.00001f;
	tFeedbackLeveler_setFactor(&x->feedback, speedVal);
	tFeedbackLeveler_setFactor(&x->feedbackP, speedVal);
}

void    tTString_pluck               (tTString* const x, float position, float amplitude)
{
    x->muted = 1.0f;
    tExpSmooth_setVal(&x->tensionSmoother, amplitude);
    tFeedbackLeveler_setTargetLevel(&x->feedback, amplitude * 0.25f);
    tFeedbackLeveler_setTargetLevel(&x->feedbackP, amplitude * 0.25f);
    tExpSmooth_setValAndDest(&x->pitchSmoother, x->pitchSmoother.dest);
    x->baseDelay = x->pitchSmoother.dest;
    float currentDelay = x->baseDelay;// - x->allpassDelay;
    x->muteCoeff = 1.0f;
    uint32_t halfCurrentDelay = currentDelay * 0.5f;
    uint32_t currentDelayInt = halfCurrentDelay * 2.0f;
    float positionMin = (x->openStringLength * 0.204166666666667f);
    float positionMax = (x->openStringLength * 0.001f);

    volatile float pluckPoint = LEAF_mapFromZeroToOneInput(position, positionMin, positionMax);
    //0.083335f; * position;//position * halfCurrentDelay;
    pluckPoint = LEAF_clip(0.0f, pluckPoint, halfCurrentDelay);
    uint32_t pluckPointInt = (uint32_t) pluckPoint;
    float invPluckPointInt = (1.0f / ((float)pluckPointInt+.001f));
    volatile float invRemainder = 1.0f / ((halfCurrentDelay-pluckPoint)+.001f);
    tLagrangeDelay_clear(x->delay);
    x->pluckRatio = pluckPoint / halfCurrentDelay;
    x->pluckPoint_forInput = pluckPoint;
    for (uint32_t i = 0; i < halfCurrentDelay; i++)
    {
        float val = 0.0f;
        //displacement waves
        if (i == pluckPointInt)
        {
            val = 1.0f;
        }
        else if (i < pluckPointInt)
        {
            val = ((float)i * invPluckPointInt);
        }
        else
        {
            val = (1.0f - (((float)i-(float)pluckPointInt)*invRemainder));
        }
        val = LEAF_tanh(val* 1.2f) * amplitude;
        tLagrangeDelay_tapIn(x->delay, val, i);
        tLagrangeDelay_tapIn(x->delay, -val, currentDelayInt-i);
        tLagrangeDelay_tapIn(x->delayP, val, i);
        tLagrangeDelay_tapIn(x->delayP, -val, currentDelayInt-i);

    }
    tThiranAllpassSOCascade_clear(x->allpass);
    tThiranAllpassSOCascade_clear(x->allpassP);

    tExpSmooth_setVal(&x->pickNoise, amplitude);
}



void    tTString_setWavelength         (tTString* const x, uint32_t waveLength)
{
	//tExpSmooth_setDest(x->pitchSmoother,waveLength);; // in samples
	x->wavelength = waveLength;
}


void    tTString_setHarmonicity         (tTString* const x, float B, float freq)
{
    x->allpassDelay = tThiranAllpassSOCascade_setCoeff(x->allpass, B, freq * x->invOversampling, x->oversampling);
    x->allpassDelayP = tThiranAllpassSOCascade_setCoeff(x->allpassP, B * 0.25f, freq * x->invOversampling, x->oversampling);
}

void    tTString_setInharmonic         (tTString* const x, uint32_t onOrOff)
{
    x->inharmonic = onOrOff;
    x->inharmonicMult = (float)onOrOff;
}
void    tTString_setWoundOrUnwound         (tTString* const x, uint32_t wound)
{
    x->wound = wound;
    if (x->wound)
	{

    	tExpSmooth_setFactor(&x->barSmooth2, 0.0005f);
    	tExpSmooth_setFactor(&x->barSmoothVol, 0.0008f);

	}
	else
	{

		tExpSmooth_setFactor(&x->barSmooth2, 0.005f);
		tExpSmooth_setFactor(&x->barSmoothVol, 0.004f);

	}
}
void    tTString_setWindingsPerInch         (tTString* const x, uint32_t windings)
{
    x->windingsPerInch = windings;
}

void    tTString_setPickupFilterFreq         (tTString* const x, float cutoff)
{
    tSVF_setFreq(&x->pickupFilter,cutoff);

}
void    tTString_setPickupFilterQ        (tTString* const x, float Q)
{
    tSVF_setQ(&x->pickupFilter,Q+0.5f);

}

void    tTString_setPeakFilterFreq         (tTString* const x, float cutoff)
{
    tSVF_setFreq(&x->peakFilt,cutoff);

}
void    tTString_setPeakFilterQ        (tTString* const x, float Q)
{
    tSVF_setQ(&x->peakFilt,Q+0.5f);

}


void tBowTable_create(tMempool** const mp, tBowTable** const bt)
{
    ALLOC_FROM_POOL(tBowTable, bt, mp);
}
void tBowTable_init(LEAF* const leaf, tBowTable* const bt)
{
    (void) leaf;
    bt->offSet = 0.0f;
    bt->slope = 0.1f;
    bt->lastOutput = 0.0f;
}
void    tBowTable_free                 (tBowTable** const bt)
{
    tBowTable* x = *bt;
    mpool_free((char*)x, x->mempool);

}
float    tBowTable_lookup               (tBowTable* const x, float sample)
{
    float input;
    input = sample + x->offSet;
    input *= x->slope;
    x->lastOutput = (float)fabs((double) input) + 0.75f;
    x->lastOutput = pow(x->lastOutput, -4.0f);
    if (x->lastOutput > 1.0f ) x->lastOutput = 1.0f;
    return x->lastOutput;
}



void tReedTable_create(tMempool** const mp, tReedTable** const pm)
{
    ALLOC_FROM_POOL(tReedTable, pm, mp);
}

void tReedTable_init(LEAF* const leaf, tReedTable* const pm, float offset, float slope)
{
    (void) leaf;
    pm->offset = offset;
    pm->slope = slope;
}

void    tReedTable_free (tReedTable** const pm)
{
    tReedTable* p = *pm;
    mpool_free((char*)p, p->mempool);
}
/* ============================ */

void tStiffString_create(tMempool** const mp, tStiffString** const pm)
{
    ALLOC_FROM_POOL(tStiffString, pm, mp);
}

void tStiffString_init(LEAF* const leaf, tStiffString* const p, int numModes)
{
    tMempool* m = p->mempool;

    p->numModes = numModes;
    p->freqHz = 440.0f;
    p->stiffness = 0.001f;
    p->pluckPos = 0.2f;
    p->pickupPos = 0.3f;
    p->decay = 0.0001f;
    p->decayHighFreq = 0.0003f;
    p->muteDecay = 0.4f;
    p->sampleRate = leaf->sampleRate;
    p->twoPiTimesInvSampleRate = leaf->twoPiTimesInvSampleRate;
    p->nyquist = p->sampleRate * 0.5f;
    float lessThanNyquist = p->sampleRate * 0.4f;
    p->nyquistScalingFactor = 1.0f / (lessThanNyquist - p->nyquist);
    p->amp = 0.0f;
    p->gainComp = 0.0f;

    p->oscs = (tCycle*) mpool_alloc(numModes * sizeof(tCycle), m);
    for (int i = 0; i < numModes; ++i) {
        tCycle_init(leaf, &p->oscs[i]);
        tCycle_setSampleRate(&p->oscs[i], p->sampleRate);
    }

    p->amplitudes = (float*) mpool_alloc(numModes * sizeof(float), m);
    p->outputWeights = (float*) mpool_alloc(numModes * sizeof(float), m);
    p->decayScalar = (float*) mpool_alloc(numModes * sizeof(float), m);
    p->decayVal = (float*) mpool_alloc(numModes * sizeof(float), m);
    p->nyquistCoeff = (float*) mpool_alloc(numModes * sizeof(float), m);

    for (int i = 0; i < numModes; ++i) {
        p->amplitudes[i] = 0.0f;
        p->outputWeights[i] = 0.0f;
        p->decayScalar[i] = 1.0f;
        p->decayVal[i] = 1.0f;
        p->nyquistCoeff[i] = 1.0f;
    }

    tStiffString_updateOutputWeights(p);
}



void    tStiffString_free (tStiffString** const pm)
{
    tStiffString* p = *pm;


    mpool_free((char *) p->nyquistCoeff, p->mempool);
    mpool_free((char *) p->decayScalar, p->mempool);
    mpool_free((char *) p->decayVal, p->mempool);
    mpool_free((char *) p->amplitudes, p->mempool);
    mpool_free((char *) p->outputWeights, p->mempool);
    mpool_free((char *) p->oscs, p->mempool);
    mpool_free((char *) p, p->mempool);
}

void tStiffString_updateOscillators(tStiffString* const p)
{
	float kappa_sq = p->stiffness * p->stiffness;
	float compensation = 0.0f;
    for (int i = 0; i < p->numModes; ++i) {
      int n = i + 1;
      int n_sq = n * n;
      float sig = p->decay + p->decayHighFreq * n_sq;
       float w0 = n * (1.0f + 0.5f * kappa_sq * n_sq);
      float zeta = sig / w0;
       float w = w0 * (1.0f - 0.5f * zeta * zeta);
      if (i == 0)
      {
    	  compensation = 1.0f / w;
      }
      float	testFreq = (p->freqHz * w);
      float nyquistTest = (testFreq - p->nyquist) * p->nyquistScalingFactor;
      p->nyquistCoeff[i] = LEAF_clip(0.0f, nyquistTest, 1.0f);
	  tCycle_setFreq(&p->oscs[i], testFreq * compensation);
	  float val = p->freqHz * sig;
	  float r = fastExp4(-val * p->twoPiTimesInvSampleRate);
	  p->decayScalar[i] = r * r;
    }
}
void tStiffString_updateOutputWeights(tStiffString* const p)
{
		float x0 = p->pickupPos * PI;
		float totalGain = 0.0f;
	  for (int i = 0; i < p->numModes; ++i)
	  {
#ifdef ARM_MATH_CM7
		  p->outputWeights[i] = arm_sin_f32((i + 1) * x0);
		  totalGain += fabsf(p->outputWeights[i]) * p->amplitudes[i];;
#else
		  p->outputWeights[i] = sinf((i + 1) * x0);
		  totalGain += p->outputWeights[i] * p->amplitudes[i];
#endif
	  }
	  if (totalGain < 0.01f)
	  {
		  totalGain = 0.01f;
	  }
	  totalGain = LEAF_clip(0.01f, totalGain, 1.0f);
	  p->gainComp = 1.0f / totalGain;
}

float   tStiffString_tick                  (tStiffString* const p)
{
    float sample = 0.0f;
    for (int i = 0; i < p->numModes; ++i) {
      sample += tCycle_tick(&p->oscs[i]) * p->amplitudes[i] * p->outputWeights[i] * p->decayVal[i] * p->nyquistCoeff[i];
      p->decayVal[i] *= p->decayScalar[i] * p->muteDecay;
    }
    return sample * p->amp * p->gainComp;
}

void tStiffString_setStiffness(tStiffString* const p, float newValue)
{
    p->stiffness = LEAF_mapFromZeroToOneInput(newValue,0.00f, 0.2f);
}

void tStiffString_setPickupPos(tStiffString* const p, float newValue)
{
    p->pickupPos = LEAF_clip(0.01f, newValue, 0.99f);
    tStiffString_updateOutputWeights(p);
}

void tStiffString_setPluckPos(tStiffString* const p, float newValue)
{
    p->pluckPos = LEAF_clip(0.01f, newValue, 0.99f);
    tStiffString_updateOutputWeights(p);
}


void tStiffString_setFreq(tStiffString* const p, float newFreq)
{
    p->freqHz = newFreq;
    tStiffString_updateOscillators(p);
}

void tStiffString_setDecay(tStiffString* const p, float decay)
{
    p->decay = decay;
    tStiffString_updateOscillators(p);
}

void tStiffString_setDecayHighFreq(tStiffString* const p, float decayHF)
{
    p->decayHighFreq = decayHF;
    tStiffString_updateOscillators(p);
}

void tStiffString_mute(tStiffString* const p)
{
    p->muteDecay = 0.99f;
}

void tStiffString_pluck(tStiffString* const p, float amp)
{
    float x0 = p->pluckPos * PI;
    p->muteDecay = 1.0f;
    for (int i = 0; i < p->numModes; ++i) {
        int n = i + 1;
        float denom = n * n * x0 * (PI - x0);
        if (denom < 0.001f)
        {
        	denom = 0.001f;
        }
#ifdef ARM_MATH_CM7
		  p->amplitudes[i] = 2.0f * arm_sin_f32(x0 * n) / denom;
#else
	      p->amplitudes[i] = 2.0f * sinf(x0 * n) / denom;
#endif
        p->decayVal[i] = 1.0f;
    }
    p->amp = amp;
    tStiffString_updateOutputWeights(p);
}

void tStiffString_setSampleRate(tStiffString* const pm, float sr)
{
    pm->sampleRate = sr;
    pm->twoPiTimesInvSampleRate = TWO_PI / sr;
}

void tStiffString_setStiffnessNoUpdate(tStiffString* const p, float newValue)
{
    p->stiffness = LEAF_mapFromZeroToOneInput(newValue,0.00f, 0.2f);
}

void tStiffString_setPickupPosNoUpdate(tStiffString* const p, float newValue)
{
    p->pickupPos = LEAF_clip(0.01f, newValue, 0.99f);
}

void tStiffString_setPluckPosNoUpdate(tStiffString* const p, float newValue)
{
    p->pluckPos = LEAF_clip(0.01f, newValue, 0.99f);
}


void tStiffString_setFreqNoUpdate(tStiffString* const p, float newFreq)
{
    p->freqHz = newFreq;
}

void tStiffString_setDecayNoUpdate(tStiffString* const p, float decay)
{
    p->decay = decay;
}

void tStiffString_setDecayHighFreqNoUpdate(tStiffString* const p, float decayHF)
{
    p->decayHighFreq = decayHF;
}


void tStiffString_pluckNoUpdate(tStiffString* const p, float amp)
{
    float x0 = p->pluckPos * 0.5f * PI;
    p->muteDecay = 1.0f;
    for (int i = 0; i < p->numModes; ++i) {
        int n = i + 1;
        float denom = n * n * x0 * (PI - x0);
        if (denom < 0.001f)
        {
        	denom = 0.001f;
        }
#ifdef ARM_MATH_CM7
		  p->amplitudes[i] = 2.0f * arm_sin_f32(x0 * n) / denom;
#else
	      p->amplitudes[i] = 2.0f * sinf(x0 * n) / denom;
#endif
        p->decayVal[i] = 1.0f;
    }
    p->amp = amp;
}



void tStereoRotation_create(tMempool** const mp, tStereoRotation** const r)
{
    ALLOC_FROM_POOL(tStereoRotation, r, mp);
}

void tStereoRotation_init(LEAF* const leaf, tStereoRotation* const r)
{
    tMempool* mp = r->mempool;

    r->angle = 0.0f;
    r->vcaoutx = 0.0f;
    r->vcaouty = 0.0f;

    tHighpass_init(leaf, &r->hip1, 10.0f);
    tHighpass_init(leaf, &r->hip2, 10.0f);
    tOnePole_init(leaf, &r->filtx, 8000.0f);
    tOnePole_init(leaf, &r->filty, 8000.0f);

    r->rotGain = 0.9999f;

    tLagrangeDelay_create(&mp, &r->rotDelayx);
    tLagrangeDelay_init(leaf, r->rotDelayx, 100.0f, 2000.0f);

    tLagrangeDelay_create(&mp, &r->rotDelayy);
    tLagrangeDelay_init(leaf, r->rotDelayy, 100.0f, 2000.0f);

    r->feedbackFactorx  = 0.9995f;
    r->feedbackFactory  = 0.9995f;
}

void    tStereoRotation_tick                    (tStereoRotation* const r, float* samples)
{
    float samplex = (samples[0] + r->vcaoutx);
    float sampley = (samples[1] + r->vcaouty);

    samplex = tHighpass_tick(&r->hip1, samplex * r->rotGain);
    sampley = tHighpass_tick(&r->hip2, sampley * r->rotGain);

    float rotOutx = (samplex * cosf(r->angle)) - (sampley * sinf(r->angle));
    float rotOuty = (samplex * sinf(r->angle)) + (sampley * cosf(r->angle));

    float delayoutx = tLagrangeDelay_tick(r->rotDelayx, tanhf(rotOutx));
    float delayouty = tLagrangeDelay_tick(r->rotDelayy, tanhf(rotOuty));

    float filteroutx = tOnePole_tick(&r->filtx, delayoutx);
    float filterouty = tOnePole_tick(&r->filty, delayouty);
    r->vcaoutx = filteroutx * r->feedbackFactorx;
    r->vcaouty = filterouty * r->feedbackFactory;

    samples[0] = filteroutx;
    samples[1] = filterouty;
    return;
}

void    tStereoRotation_tickIn                   (tStereoRotation* const r, float* samples)
{
    float samplex = (samples[0] + r->vcaoutx);
    float sampley = (samples[1] + r->vcaouty);

    samplex = tHighpass_tick(&r->hip1, samplex * r->rotGain);
    sampley = tHighpass_tick(&r->hip2, sampley * r->rotGain);

    float rotOutx = (samplex * cosf(r->angle)) - (sampley * sinf(r->angle));
    float rotOuty = (samplex * sinf(r->angle)) + (sampley * cosf(r->angle));

    tLagrangeDelay_tickIn(r->rotDelayx, tanhf(rotOutx));
    tLagrangeDelay_tickIn(r->rotDelayy, tanhf(rotOuty));
    return;
}
void    tStereoRotation_tickOut                    (tStereoRotation* const r, float* samples)
{
    float delayoutx = tLagrangeDelay_tickOut(r->rotDelayx);
    float delayouty = tLagrangeDelay_tickOut(r->rotDelayy);

    float filteroutx = tOnePole_tick(&r->filtx, delayoutx);
    float filterouty = tOnePole_tick(&r->filty, delayouty);
    r->vcaoutx = filteroutx * r->feedbackFactorx;
    r->vcaouty = filterouty * r->feedbackFactory;

    samples[0] = filteroutx;
    samples[1] = filterouty;
    return;
}

void    tStereoRotation_setAngle                    (tStereoRotation* const r, float input)
{
    r->angle = input * TWO_PI;
}

void    tStereoRotation_setDelayX                    (tStereoRotation* const r, float time)
{
    tLagrangeDelay_setDelay(r->rotDelayx, time);
}

void    tStereoRotation_setDelayY                    (tStereoRotation* const r, float time)
{
    tLagrangeDelay_setDelay(r->rotDelayy, time);
}

void    tStereoRotation_setFeedbackX                (tStereoRotation* const r, float feedbackx)
{
    r->feedbackFactorx = feedbackx;
}
void    tStereoRotation_setFeedbackY                    (tStereoRotation* const r, float feedbacky)
{
    r->feedbackFactory = feedbacky;
}

void    tStereoRotation_setFilterX                    (tStereoRotation* const r, float freq)
{
    tOnePole_setFreq(&r->filtx, freq);
}

void    tStereoRotation_setFilterY                    (tStereoRotation* const r, float freq)
{
    tOnePole_setFreq(&r->filty, freq);
}

void    tStereoRotation_setGain                   (tStereoRotation* const r, float gain)
{
    r->rotGain = gain;
}
