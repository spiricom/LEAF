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

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tPluck ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void    tPluck_init         (tPluck* const pl, float lowestFrequency)
{
    _tPluck* p = *pl = (_tPluck*) leaf_alloc(sizeof(_tPluck));
    
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 10.0f;
    
    tNoise_init(&p->noise, WhiteNoise);
    
    tOnePole_init(&p->pickFilter, 0.0f);
    
    tOneZero_init(&p->loopFilter, 0.0f);
    
    tAllpassDelay_init(&p->delayLine, 0.0f, leaf.sampleRate * 2);
    
    tPluck_setFrequency(pl, 220.0f);
}

void tPluck_free (tPluck* const pl)
{
    _tPluck* p = *pl;
    
    tNoise_free(&p->noise);
    tOnePole_free(&p->pickFilter);
    tOneZero_free(&p->loopFilter);
    tAllpassDelay_free(&p->delayLine);
    
    leaf_free(p);
}

void    tPluck_initToPool    (tPluck* const pl, float lowestFrequency, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPluck* p = *pl = (_tPluck*) mpool_alloc(sizeof(_tPluck), &m->pool);
    
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 10.0f;
    
    tNoise_initToPool(&p->noise, WhiteNoise, mp);
    
    tOnePole_initToPool(&p->pickFilter, 0.0f, mp);
    
    tOneZero_initToPool(&p->loopFilter, 0.0f, mp);
    
    tAllpassDelay_initToPool(&p->delayLine, 0.0f, leaf.sampleRate * 2, mp);
    
    tPluck_setFrequency(pl, 220.0f);
}

void    tPluck_freeFromPool  (tPluck* const pl, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPluck* p = *pl;
    
    tNoise_freeFromPool(&p->noise, mp);
    tOnePole_freeFromPool(&p->pickFilter, mp);
    tOneZero_freeFromPool(&p->loopFilter, mp);
    tAllpassDelay_freeFromPool(&p->delayLine, mp);
    
    mpool_free(p, &m->pool);
}

float   tPluck_getLastOut    (tPluck* const pl)
{
    _tPluck* p = *pl;
    return p->lastOut;
}

float   tPluck_tick          (tPluck* const pl)
{
    _tPluck* p = *pl;
    return (p->lastOut = 3.0f * tAllpassDelay_tick(&p->delayLine, tOneZero_tick(&p->loopFilter, tAllpassDelay_getLastOut(&p->delayLine) * p->loopGain ) ));
}

void    tPluck_pluck         (tPluck* const pl, float amplitude)
{
    _tPluck* p = *pl;
    
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    tOnePole_setPole(&p->pickFilter, 0.999f - (amplitude * 0.15f));
    tOnePole_setGain(&p->pickFilter, amplitude * 0.5f );
    
    // Fill delay with noise additively with current contents.
    for ( uint32_t i = 0; i < (uint32_t)tAllpassDelay_getDelay(&p->delayLine); i++ )
        tAllpassDelay_tick(&p->delayLine, 0.6f * tAllpassDelay_getLastOut(&p->delayLine) + tOnePole_tick(&p->pickFilter, tNoise_tick(&p->noise) ) );
}

// Start a note with the given frequency and amplitude.;
void    tPluck_noteOn        (tPluck* const pl, float frequency, float amplitude )
{
    _tPluck* p = *pl;
    p->lastFreq = frequency;
    tPluck_setFrequency( pl, frequency );
    tPluck_pluck( pl, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tPluck_noteOff       (tPluck* const pl, float amplitude )
{
    _tPluck* p = *pl;
    
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tPluck_setFrequency  (tPluck* const pl, float frequency )
{
    _tPluck* p = *pl;
    
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    // Delay = length - filter delay.
    float delay = ( leaf.sampleRate / frequency ) - tOneZero_getPhaseDelay(&p->loopFilter, frequency );
    
    tAllpassDelay_setDelay(&p->delayLine, delay );
    
    p->loopGain = 0.99f + (frequency * 0.000005f);
    
    if ( p->loopGain >= 0.999f ) p->loopGain = 0.999f;
    
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tPluck_controlChange (tPluck* const pl, int number, float value)
{
    return;
}

void tPluckSampleRateChanged(tPluck* const pl)
{
    _tPluck* p = *pl;
    tPluck_setFrequency(pl, p->lastFreq);
}

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tKarplusStrong ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void    tKarplusStrong_init (tKarplusStrong* const pl, float lowestFrequency)
{
    _tKarplusStrong* p = *pl = (_tKarplusStrong*) leaf_alloc(sizeof(_tKarplusStrong));
    
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 8.0f;
    
    tAllpassDelay_init(&p->delayLine, 0.0f, leaf.sampleRate * 2);

    tLinearDelay_init(&p->combDelay, 0.0f, leaf.sampleRate * 2);
    
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
    
    tKarplusStrong_setFrequency( pl, 220.0f );
}

void tKarplusStrong_free (tKarplusStrong* const pl)
{
    _tKarplusStrong* p = *pl;
    
    tAllpassDelay_free(&p->delayLine);
    tLinearDelay_free(&p->combDelay);
    tOneZero_free(&p->filter);
    tNoise_free(&p->noise);
    
    for (int i = 0; i < 4; i++)
    {
        tBiQuad_free(&p->biquad[i]);
    }
    
    leaf_free(p);
}

void    tKarplusStrong_initToPool   (tKarplusStrong* const pl, float lowestFrequency, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tKarplusStrong* p = *pl = (_tKarplusStrong*) mpool_alloc(sizeof(_tKarplusStrong), &m->pool);
    
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 8.0f;
    
    tAllpassDelay_initToPool(&p->delayLine, 0.0f, leaf.sampleRate * 2, mp);
    
    tLinearDelay_initToPool(&p->combDelay, 0.0f, leaf.sampleRate * 2, mp);
    
    tOneZero_initToPool(&p->filter, 0.0f, mp);
    
    tNoise_initToPool(&p->noise, WhiteNoise, mp);
    
    for (int i = 0; i < 4; i++)
    {
        tBiQuad_initToPool(&p->biquad[i], mp);
    }
    
    p->pluckAmplitude = 0.3f;
    p->pickupPosition = 0.4f;
    
    p->stretching = 0.9999f;
    p->baseLoopGain = 0.995f;
    p->loopGain = 0.999f;
    
    tKarplusStrong_setFrequency( pl, 220.0f );
}

void    tKarplusStrong_freeFromPool (tKarplusStrong* const pl, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tKarplusStrong* p = *pl;
    
    tAllpassDelay_freeFromPool(&p->delayLine, mp);
    tLinearDelay_freeFromPool(&p->combDelay, mp);
    tOneZero_freeFromPool(&p->filter, mp);
    tNoise_freeFromPool(&p->noise, mp);
    
    for (int i = 0; i < 4; i++)
    {
        tBiQuad_freeFromPool(&p->biquad[i], mp);
    }
    
    mpool_free(p, &m->pool);
}

float   tKarplusStrong_getLastOut    (tKarplusStrong* const pl)
{
    _tKarplusStrong* p = *pl;
    return p->lastOut;
}

float   tKarplusStrong_tick          (tKarplusStrong* const pl)
{
    _tKarplusStrong* p = *pl;
    
    float temp = tAllpassDelay_getLastOut(&p->delayLine) * p->loopGain;
    
    // Calculate allpass stretching.
    for (int i=0; i<4; i++)     temp = tBiQuad_tick(&p->biquad[i],temp);
    
    // Moving average filter.
    temp = tOneZero_tick(&p->filter, temp);
    
    float out = tAllpassDelay_tick(&p->delayLine, temp);
    out = out - tLinearDelay_tick(&p->combDelay, out);
    p->lastOut = out;
    
    return p->lastOut;
}

void    tKarplusStrong_pluck         (tKarplusStrong* const pl, float amplitude)
{
    _tKarplusStrong* p = *pl;
    
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->pluckAmplitude = amplitude;
    
    for ( uint32_t i=0; i < (uint32_t)tAllpassDelay_getDelay(&p->delayLine); i++ )
    {
        // Fill delay with noise additively with current contents.
        tAllpassDelay_tick(&p->delayLine, (tAllpassDelay_getLastOut(&p->delayLine) * 0.6f) + 0.4f * tNoise_tick(&p->noise) * p->pluckAmplitude );
        //delayLine_.tick( combDelay_.tick((delayLine_.lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude_) );
    }
}

// Start a note with the given frequency and amplitude.;
void    tKarplusStrong_noteOn        (tKarplusStrong* const pl, float frequency, float amplitude )
{
    tKarplusStrong_setFrequency( pl, frequency );
    tKarplusStrong_pluck( pl, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tKarplusStrong_noteOff       (tKarplusStrong* const pl, float amplitude )
{
    _tKarplusStrong* p = *pl;
    
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tKarplusStrong_setFrequency  (tKarplusStrong* const pl, float frequency )
{
    _tKarplusStrong* p = *pl;
    
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    p->lastFrequency = frequency;
    p->lastLength = leaf.sampleRate / p->lastFrequency;
    float delay = p->lastLength - 0.5f;
    tAllpassDelay_setDelay(&p->delayLine, delay);
    
    // MAYBE MODIFY LOOP GAINS
    p->loopGain = p->baseLoopGain + (frequency * 0.000005f);
    if (p->loopGain >= 1.0f) p->loopGain = 0.99999f;
    
    tKarplusStrong_setStretch(pl, p->stretching);
    
    tLinearDelay_setDelay(&p->combDelay, 0.5f * p->pickupPosition * p->lastLength );
    
}

// Set the stretch "factor" of the string (0.0 - 1.0).
void    tKarplusStrong_setStretch         (tKarplusStrong* const pl, float stretch )
{
    _tKarplusStrong* p = *pl;
    
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
void    tKarplusStrong_setPickupPosition  (tKarplusStrong* const pl, float position )
{
    _tKarplusStrong* p = *pl;
    
    if (position < 0.0f)        p->pickupPosition = 0.0f;
    else if (position <= 1.0f)  p->pickupPosition = position;
    else                        p->pickupPosition = 1.0f;
    
    tLinearDelay_setDelay(&p->combDelay, 0.5f * p->pickupPosition * p->lastLength);
}

// Set the base loop gain.
void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong* const pl, float aGain )
{
    _tKarplusStrong* p = *pl;
    
    p->baseLoopGain = aGain;
    p->loopGain = p->baseLoopGain + (p->lastFrequency * 0.000005f);
    if ( p->loopGain > 0.99999f ) p->loopGain = 0.99999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tKarplusStrong_controlChange (tKarplusStrong* const pl, SKControlType type, float value)
{
    if ( value < 0.0f )         value = 0.0f;
    else if (value > 128.0f)   value = 128.0f;
    
    float normalizedValue = value * INV_128;
    
    if (type == SKPickPosition) // 4
        tKarplusStrong_setPickupPosition( pl, normalizedValue );
    else if (type == SKStringDamping) // 11
        tKarplusStrong_setBaseLoopGain( pl, 0.97f + (normalizedValue * 0.03f) );
    else if (type == SKDetune) // 1
        tKarplusStrong_setStretch( pl, 0.91f + (0.09f * (1.0f - normalizedValue)) );
}

void    tKarplusStrongSampleRateChanged (tKarplusStrong* const pl)
{
    _tKarplusStrong* p = *pl;
    
    tKarplusStrong_setFrequency(pl, p->lastFrequency);
    tKarplusStrong_setStretch(pl, p->stretching);
}

/* Simple Living String*/

void    tSimpleLivingString_init(tSimpleLivingString* const pl, float freq, float dampFreq,
                                 float decay, float targetLev, float levSmoothFactor,
                                 float levStrength, int levMode)
{
    _tSimpleLivingString* p = *pl = (_tSimpleLivingString*) leaf_alloc(sizeof(_tSimpleLivingString));
    
    p->curr=0.0f;
    tExpSmooth_init(&p->wlSmooth, leaf.sampleRate/freq, 0.01); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tSimpleLivingString_setFreq(pl, freq);
    tLinearDelay_init(&p->delayLine,p->waveLengthInSamples, 2400);
    tOnePole_init(&p->bridgeFilter, dampFreq);
    tHighpass_init(&p->DCblocker,13);
    p->decay=decay;
    tFeedbackLeveler_init(&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode);
    p->levMode=levMode;
}

void tSimpleLivingString_free(tSimpleLivingString* const pl)
{
    _tSimpleLivingString* p = *pl;
    
    tExpSmooth_free(&p->wlSmooth);
    tLinearDelay_free(&p->delayLine);
    tOnePole_free(&p->bridgeFilter);
    tHighpass_free(&p->DCblocker);
    tFeedbackLeveler_free(&p->fbLev);
    
    leaf_free(p);
}

void    tSimpleLivingString_initToPool  (tSimpleLivingString* const pl, float freq, float dampFreq,
                                         float decay, float targetLev, float levSmoothFactor,
                                         float levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimpleLivingString* p = *pl = (_tSimpleLivingString*) mpool_alloc(sizeof(_tSimpleLivingString), &m->pool);
    
    p->curr=0.0f;
    tExpSmooth_initToPool(&p->wlSmooth, leaf.sampleRate/freq, 0.01, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tSimpleLivingString_setFreq(pl, freq);
    tLinearDelay_initToPool(&p->delayLine,p->waveLengthInSamples, 2400, mp);
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tHighpass_initToPool(&p->DCblocker,13, mp);
    p->decay=decay;
    tFeedbackLeveler_initToPool(&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
}

void    tSimpleLivingString_freeFromPool    (tSimpleLivingString* const pl, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimpleLivingString* p = *pl;
    
    tExpSmooth_freeFromPool(&p->wlSmooth, mp);
    tLinearDelay_freeFromPool(&p->delayLine, mp);
    tOnePole_freeFromPool(&p->bridgeFilter, mp);
    tHighpass_freeFromPool(&p->DCblocker, mp);
    tFeedbackLeveler_freeFromPool(&p->fbLev, mp);
    
    mpool_free(p, &m->pool);
}

void     tSimpleLivingString_setFreq(tSimpleLivingString* const pl, float freq)
{
    _tSimpleLivingString* p = *pl;
    
    if (freq<20) freq=20;
    else if (freq>10000) freq=10000;
    p->waveLengthInSamples = leaf.sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString_setWaveLength(tSimpleLivingString* const pl, float waveLength)
{
    _tSimpleLivingString* p = *pl;
    
    if (waveLength<4.8) waveLength=4.8;
    else if (waveLength>2400) waveLength=2400;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString_setDampFreq(tSimpleLivingString* const pl, float dampFreq)
{
    _tSimpleLivingString* p = *pl;
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString_setDecay(tSimpleLivingString* const pl, float decay)
{
    _tSimpleLivingString* p = *pl;
    p->decay=decay;
}

void     tSimpleLivingString_setTargetLev(tSimpleLivingString* const pl, float targetLev)
{
    _tSimpleLivingString* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
}

void     tSimpleLivingString_setLevSmoothFactor(tSimpleLivingString* const pl, float levSmoothFactor)
{
    _tSimpleLivingString* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString_setLevStrength(tSimpleLivingString* const pl, float levStrength)
{
    _tSimpleLivingString* p = *pl;
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
}

void     tSimpleLivingString_setLevMode(tSimpleLivingString* const pl, int levMode)
{
    _tSimpleLivingString* p = *pl;
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    p->levMode=levMode;
}

float   tSimpleLivingString_tick(tSimpleLivingString* const pl, float input)
{
    _tSimpleLivingString* p = *pl;
    
    float stringOut=tOnePole_tick(&p->bridgeFilter,tLinearDelay_tickOut(&p->delayLine));
    float stringInput=tHighpass_tick(&p->DCblocker, tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    tLinearDelay_tickIn(&p->delayLine, stringInput);
    tLinearDelay_setDelay(&p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    p->curr = stringOut;
    return p->curr;
}

float   tSimpleLivingString_sample(tSimpleLivingString* const pl)
{
    _tSimpleLivingString* p = *pl;
    return p->curr;
}

/* Living String*/

void    tLivingString_init(tLivingString* const pl, float freq, float pickPos, float prepIndex,
                           float dampFreq, float decay, float targetLev, float levSmoothFactor,
                           float levStrength, int levMode)
{
    _tLivingString* p = *pl = (_tLivingString*) leaf_alloc(sizeof(_tLivingString));
    
    p->curr=0.0f;
    tExpSmooth_init(&p->wlSmooth, leaf.sampleRate/freq, 0.01); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tLivingString_setFreq(pl, freq);
    p->freq = freq;
    tExpSmooth_init(&p->ppSmooth, pickPos, 0.01); // smoother for pick position
    tLivingString_setPickPos(pl, pickPos);
    p->prepIndex=prepIndex;
    tLinearDelay_init(&p->delLF,p->waveLengthInSamples, 2400);
    tLinearDelay_init(&p->delUF,p->waveLengthInSamples, 2400);
    tLinearDelay_init(&p->delUB,p->waveLengthInSamples, 2400);
    tLinearDelay_init(&p->delLB,p->waveLengthInSamples, 2400);
    p->dampFreq = dampFreq;
    tOnePole_init(&p->bridgeFilter, dampFreq);
    tOnePole_init(&p->nutFilter, dampFreq);
    tOnePole_init(&p->prepFilterU, dampFreq);
    tOnePole_init(&p->prepFilterL, dampFreq);
    tHighpass_init(&p->DCblockerU,13);
    tHighpass_init(&p->DCblockerL,13);
    p->decay=decay;
    p->prepIndex = prepIndex;
    tFeedbackLeveler_init(&p->fbLevU, targetLev, levSmoothFactor, levStrength, levMode);
    tFeedbackLeveler_init(&p->fbLevL, targetLev, levSmoothFactor, levStrength, levMode);
    p->levMode=levMode;
}

void tLivingString_free(tLivingString* const pl)
{
    _tLivingString* p = *pl;
    
    tExpSmooth_free(&p->wlSmooth);
    tExpSmooth_free(&p->ppSmooth);
    tLinearDelay_free(&p->delLF);
    tLinearDelay_free(&p->delUF);
    tLinearDelay_free(&p->delUB);
    tLinearDelay_free(&p->delLB);
    tOnePole_free(&p->bridgeFilter);
    tOnePole_free(&p->nutFilter);
    tOnePole_free(&p->prepFilterU);
    tOnePole_free(&p->prepFilterL);
    tHighpass_free(&p->DCblockerU);
    tHighpass_free(&p->DCblockerL);
    tFeedbackLeveler_free(&p->fbLevU);
    tFeedbackLeveler_free(&p->fbLevL);
    
    leaf_free(p);
}

void    tLivingString_initToPool    (tLivingString* const pl, float freq, float pickPos, float prepIndex,
                                     float dampFreq, float decay, float targetLev, float levSmoothFactor,
                                     float levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tLivingString* p = *pl = (_tLivingString*) mpool_alloc(sizeof(_tLivingString), &m->pool);
    
    p->curr=0.0f;
    tExpSmooth_initToPool(&p->wlSmooth, leaf.sampleRate/freq, 0.01, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tLivingString_setFreq(pl, freq);
    p->freq = freq;
    tExpSmooth_initToPool(&p->ppSmooth, pickPos, 0.01, mp); // smoother for pick position
    tLivingString_setPickPos(pl, pickPos);
    p->prepIndex=prepIndex;
    tLinearDelay_initToPool(&p->delLF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delLB,p->waveLengthInSamples, 2400, mp);
    p->dampFreq = dampFreq;
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tOnePole_initToPool(&p->nutFilter, dampFreq, mp);
    tOnePole_initToPool(&p->prepFilterU, dampFreq, mp);
    tOnePole_initToPool(&p->prepFilterL, dampFreq, mp);
    tHighpass_initToPool(&p->DCblockerU,13, mp);
    tHighpass_initToPool(&p->DCblockerL,13, mp);
    p->decay=decay;
    p->prepIndex = prepIndex;
    tFeedbackLeveler_initToPool(&p->fbLevU, targetLev, levSmoothFactor, levStrength, levMode, mp);
    tFeedbackLeveler_initToPool(&p->fbLevL, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
}

void    tLivingString_freeFromPool  (tLivingString* const pl, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tLivingString* p = *pl;
    
    tExpSmooth_freeFromPool(&p->wlSmooth, mp);
    tExpSmooth_freeFromPool(&p->ppSmooth, mp);
    tLinearDelay_freeFromPool(&p->delLF, mp);
    tLinearDelay_freeFromPool(&p->delUF, mp);
    tLinearDelay_freeFromPool(&p->delUB, mp);
    tLinearDelay_freeFromPool(&p->delLB, mp);
    tOnePole_freeFromPool(&p->bridgeFilter, mp);
    tOnePole_freeFromPool(&p->nutFilter, mp);
    tOnePole_freeFromPool(&p->prepFilterU, mp);
    tOnePole_freeFromPool(&p->prepFilterL, mp);
    tHighpass_freeFromPool(&p->DCblockerU, mp);
    tHighpass_freeFromPool(&p->DCblockerL, mp);
    tFeedbackLeveler_freeFromPool(&p->fbLevU, mp);
    tFeedbackLeveler_freeFromPool(&p->fbLevL, mp);
    
    mpool_free(p, &m->pool);
}

void     tLivingString_setFreq(tLivingString* const pl, float freq)
{    // NOTE: It is faster to set wavelength in samples directly
    _tLivingString* p = *pl;
    if (freq<20) freq=20;
    else if (freq>10000) freq=10000;
    p->waveLengthInSamples = leaf.sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString_setWaveLength(tLivingString* const pl, float waveLength)
{
    _tLivingString* p = *pl;
    if (waveLength<4.8) waveLength=4.8;
    else if (waveLength>2400) waveLength=2400;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString_setPickPos(tLivingString* const pl, float pickPos)
{    // between 0 and 1
    _tLivingString* p = *pl;
    if (pickPos<0.f) pickPos=0.f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(&p->ppSmooth, p->pickPos);
}

void     tLivingString_setPrepIndex(tLivingString* const pl, float prepIndex)
{    // between 0 and 1
    _tLivingString* p = *pl;
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tLivingString_setDampFreq(tLivingString* const pl, float dampFreq)
{
    _tLivingString* p = *pl;
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    tOnePole_setFreq(&p->nutFilter, dampFreq);
    tOnePole_setFreq(&p->prepFilterU, dampFreq);
    tOnePole_setFreq(&p->prepFilterL, dampFreq);
}

void     tLivingString_setDecay(tLivingString* const pl, float decay)
{
    _tLivingString* p = *pl;
    p->decay=decay;
}

void     tLivingString_setTargetLev(tLivingString* const pl, float targetLev)
{
    _tLivingString* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLevL, targetLev);
}

void     tLivingString_setLevSmoothFactor(tLivingString* const pl, float levSmoothFactor)
{
    _tLivingString* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLevL, levSmoothFactor);
}

void     tLivingString_setLevStrength(tLivingString* const pl, float levStrength)
{
    _tLivingString* p = *pl;
    tFeedbackLeveler_setStrength(&p->fbLevU, levStrength);
    tFeedbackLeveler_setStrength(&p->fbLevL, levStrength);
}

void     tLivingString_setLevMode(tLivingString* const pl, int levMode)
{
    _tLivingString* p = *pl;
    tFeedbackLeveler_setMode(&p->fbLevU, levMode);
    tFeedbackLeveler_setMode(&p->fbLevL, levMode);
    p->levMode=levMode;
}

float   tLivingString_tick(tLivingString* const pl, float input)
{
    _tLivingString* p = *pl;
    
    // from pickPos upwards=forwards
    float fromLF=tLinearDelay_tickOut(&p->delLF);
    float fromUF=tLinearDelay_tickOut(&p->delUF);
    float fromUB=tLinearDelay_tickOut(&p->delUB);
    float fromLB=tLinearDelay_tickOut(&p->delLB);
    // into upper half of string, from nut, going backwards
    float fromNut=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(&p->DCblockerU, tOnePole_tick(&p->nutFilter, fromUF)));
    tLinearDelay_tickIn(&p->delUB, fromNut);
    // into lower half of string, from pickpoint, going backwards
    float fromLowerPrep=-tOnePole_tick(&p->prepFilterL, fromLF);
    float intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB+input;
    tLinearDelay_tickIn(&p->delLB, intoLower);
    // into lower half of string, from bridge
    float fromBridge=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tOnePole_tick(&p->bridgeFilter, fromLB)));
    tLinearDelay_tickIn(&p->delLF, fromBridge);
    // into upper half of string, from pickpoint, going forwards/upwards
    float fromUpperPrep=-tOnePole_tick(&p->prepFilterU, fromUB);
    float intoUpper=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF+input;
    tLinearDelay_tickIn(&p->delUF, intoUpper);
    // update all delay lengths
    float pickP=tExpSmooth_tick(&p->ppSmooth);
    float wLen=tExpSmooth_tick(&p->wlSmooth);
    float lowLen=pickP*wLen;
    float upLen=(1.0f-pickP)*wLen;
    tLinearDelay_setDelay(&p->delLF, lowLen);
    tLinearDelay_setDelay(&p->delLB, lowLen);
    tLinearDelay_setDelay(&p->delUF, upLen);
    tLinearDelay_setDelay(&p->delUB, upLen);
    p->curr = fromBridge;
    return p->curr;
}

float   tLivingString_sample(tLivingString* const pl)
{
    _tLivingString* p = *pl;
    return p->curr;
}

///Reed Table model
//default values from STK are 0.6 offset and -0.8 slope

void    tReedTable_init      (tReedTable* const pm, float offset, float slope)
{
    _tReedTable* p = *pm = (_tReedTable*) leaf_alloc(sizeof(_tReedTable));
    
    p->offset = offset;
    p->slope = slope;
}

void    tReedTable_free      (tReedTable* const pm)
{
    _tReedTable* p = *pm;
    
    leaf_free(p);
}

void    tReedTable_initToPool   (tReedTable* const pm, float offset, float slope, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tReedTable* p = *pm = (_tReedTable*) mpool_alloc(sizeof(_tReedTable), &m->pool);
    
    p->offset = offset;
    p->slope = slope;
}

void    tReedTable_freeFromPool (tReedTable* const pm, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tReedTable* p = *pm;
    
    mpool_free(p, &m->pool);
}

float   tReedTable_tick      (tReedTable* const pm, float input)
{
    _tReedTable* p = *pm;
    
    // The input is differential pressure across the reed.
    float output = p->offset + (p->slope * input);
    
    // If output is > 1, the reed has slammed shut and the
    // reflection function value saturates at 1.0.
    if ( output > 1.0f) output = 1.0f;
    
    // This is nearly impossible in a physical system, but
    // a reflection function value of -1.0 corresponds to
    // an open end (and no discontinuity in bore profile).
    if ( output < -1.0f) output = -1.0f;
    
    return output;
}

float   tReedTable_tanh_tick     (tReedTable* const pm, float input)
{
    _tReedTable* p = *pm;
    
    // The input is differential pressure across the reed.
    float output = p->offset + (p->slope * input);
    
    // If output is > 1, the reed has slammed shut and the
    // reflection function value saturates at 1.0.
    return tanhf(output);
}

void     tReedTable_setOffset   (tReedTable* const pm, float offset)
{
    _tReedTable* p = *pm;
    p->offset = offset;
}

void     tReedTable_setSlope   (tReedTable* const pm, float slope)
{
    _tReedTable* p = *pm;
    p->slope = slope;
}
