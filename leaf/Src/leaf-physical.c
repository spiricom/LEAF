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
void    tPluck_init         (tPluck* const pl, Lfloat lowestFrequency, LEAF* const leaf)
{
    tPluck_initToPool(pl, lowestFrequency, &leaf->mempool);
}

void    tPluck_initToPool    (tPluck* const pl, Lfloat lowestFrequency, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPluck* p = *pl = (_tPluck*) mpool_alloc(sizeof(_tPluck), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;
    
    p->sampleRate = leaf->sampleRate;
    
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 10.0f;
    
    tNoise_initToPool(&p->noise, WhiteNoise, mp);
    
    tOnePole_initToPool(&p->pickFilter, 0.0f, mp);
    
    tOneZero_initToPool(&p->loopFilter, 0.0f, mp);
    
    tAllpassDelay_initToPool(&p->delayLine, 0.0f, p->sampleRate * 2, mp);
    tAllpassDelay_clear(&p->delayLine);
    
    tPluck_setFrequency(pl, 220.0f);
}

void    tPluck_free (tPluck* const pl)
{
    _tPluck* p = *pl;
    
    tNoise_free(&p->noise);
    tOnePole_free(&p->pickFilter);
    tOneZero_free(&p->loopFilter);
    tAllpassDelay_free(&p->delayLine);
    
    mpool_free((char*)p, p->mempool);
}

Lfloat   tPluck_getLastOut    (tPluck* const pl)
{
    _tPluck* p = *pl;
    return p->lastOut;
}

Lfloat   tPluck_tick          (tPluck* const pl)
{
    _tPluck* p = *pl;
    return (p->lastOut = 3.0f * tAllpassDelay_tick(&p->delayLine, tOneZero_tick(&p->loopFilter, tAllpassDelay_getLastOut(&p->delayLine) * p->loopGain ) ));
}

void    tPluck_pluck         (tPluck* const pl, Lfloat amplitude)
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
void    tPluck_noteOn        (tPluck* const pl, Lfloat frequency, Lfloat amplitude )
{
    _tPluck* p = *pl;
    p->lastFreq = frequency;
    tPluck_setFrequency( pl, frequency );
    tPluck_pluck( pl, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tPluck_noteOff       (tPluck* const pl, Lfloat amplitude )
{
    _tPluck* p = *pl;
    
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tPluck_setFrequency  (tPluck* const pl, Lfloat frequency )
{
    _tPluck* p = *pl;
    
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    // Delay = length - filter delay.
    Lfloat delay = ( p->sampleRate / frequency ) - tOneZero_getPhaseDelay(&p->loopFilter, frequency );
    
    tAllpassDelay_setDelay(&p->delayLine, delay );
    
    p->loopGain = 0.99f + (frequency * 0.000005f);
    
    if ( p->loopGain >= 0.999f ) p->loopGain = 0.999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tPluck_controlChange (tPluck* const pl, int number, Lfloat value)
{
    return;
}

void tPluck_setSampleRate(tPluck* const pl, Lfloat sr)
{
    _tPluck* p = *pl;
    p->sampleRate = sr;
    
    tAllpassDelay_free(&p->delayLine);
    tAllpassDelay_initToPool(&p->delayLine, 0.0f, p->sampleRate * 2, &p->mempool);
    tAllpassDelay_clear(&p->delayLine);
    
    tPluck_setFrequency(pl, p->lastFreq);
    tOnePole_setSampleRate(&p->pickFilter, p->sampleRate);
    tOneZero_setSampleRate(&p->loopFilter, p->sampleRate);
}

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tKarplusStrong ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void    tKarplusStrong_init (tKarplusStrong* const pl, Lfloat lowestFrequency, LEAF* const leaf)
{
    tKarplusStrong_initToPool(pl, lowestFrequency, &leaf->mempool);
}

void    tKarplusStrong_initToPool   (tKarplusStrong* const pl, Lfloat lowestFrequency, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tKarplusStrong* p = *pl = (_tKarplusStrong*) mpool_alloc(sizeof(_tKarplusStrong), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;
    
    p->sampleRate = leaf->sampleRate;
    
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 8.0f;
    
    tAllpassDelay_initToPool(&p->delayLine, 0.0f, p->sampleRate * 2, mp);
    tAllpassDelay_clear(&p->delayLine);
    
    tLinearDelay_initToPool(&p->combDelay, 0.0f, p->sampleRate * 2, mp);
    tLinearDelay_clear(&p->combDelay);
    
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

void    tKarplusStrong_free (tKarplusStrong* const pl)
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
    
    mpool_free((char*)p, p->mempool);
}

Lfloat   tKarplusStrong_getLastOut    (tKarplusStrong* const pl)
{
    _tKarplusStrong* p = *pl;
    return p->lastOut;
}

Lfloat   tKarplusStrong_tick          (tKarplusStrong* const pl)
{
    _tKarplusStrong* p = *pl;
    
    Lfloat temp = tAllpassDelay_getLastOut(&p->delayLine) * p->loopGain;
    
    // Calculate allpass stretching.
    for (int i=0; i<4; i++)     temp = tBiQuad_tick(&p->biquad[i],temp);
    
    // Moving average filter.
    temp = tOneZero_tick(&p->filter, temp);
    
    Lfloat out = tAllpassDelay_tick(&p->delayLine, temp);
    out = out - tLinearDelay_tick(&p->combDelay, out);
    p->lastOut = out;
    
    return p->lastOut;
}

void    tKarplusStrong_pluck         (tKarplusStrong* const pl, Lfloat amplitude)
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
void    tKarplusStrong_noteOn        (tKarplusStrong* const pl, Lfloat frequency, Lfloat amplitude )
{
    tKarplusStrong_setFrequency( pl, frequency );
    tKarplusStrong_pluck( pl, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tKarplusStrong_noteOff       (tKarplusStrong* const pl, Lfloat amplitude )
{
    _tKarplusStrong* p = *pl;
    
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tKarplusStrong_setFrequency  (tKarplusStrong* const pl, Lfloat frequency )
{
    _tKarplusStrong* p = *pl;
    
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    p->lastFrequency = frequency;
    p->lastLength = p->sampleRate / p->lastFrequency;
    Lfloat delay = p->lastLength - 0.5f;
    tAllpassDelay_setDelay(&p->delayLine, delay);
    
    // MAYBE MODIFY LOOP GAINS
    p->loopGain = p->baseLoopGain + (frequency * 0.000005f);
    if (p->loopGain >= 1.0f) p->loopGain = 0.99999f;
    
    tKarplusStrong_setStretch(pl, p->stretching);
    
    tLinearDelay_setDelay(&p->combDelay, 0.5f * p->pickupPosition * p->lastLength );
}

// Set the stretch "factor" of the string (0.0 - 1.0).
void    tKarplusStrong_setStretch         (tKarplusStrong* const pl, Lfloat stretch)
{
    _tKarplusStrong* p = *pl;
    
    p->stretching = stretch;
    Lfloat coefficient;
    Lfloat freq = p->lastFrequency * 2.0f;
    Lfloat dFreq = ( (0.5f * p->sampleRate) - freq ) * 0.25f;
    Lfloat temp = 0.5f + (stretch * 0.5f);
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
void    tKarplusStrong_setPickupPosition  (tKarplusStrong* const pl, Lfloat position )
{
    _tKarplusStrong* p = *pl;
    
    if (position < 0.0f)        p->pickupPosition = 0.0f;
    else if (position <= 1.0f)  p->pickupPosition = position;
    else                        p->pickupPosition = 1.0f;
    
    tLinearDelay_setDelay(&p->combDelay, 0.5f * p->pickupPosition * p->lastLength);
}

// Set the base loop gain.
void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong* const pl, Lfloat aGain )
{
    _tKarplusStrong* p = *pl;
    
    p->baseLoopGain = aGain;
    p->loopGain = p->baseLoopGain + (p->lastFrequency * 0.000005f);
    if ( p->loopGain > 0.99999f ) p->loopGain = 0.99999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tKarplusStrong_controlChange (tKarplusStrong* const pl, SKControlType type, Lfloat value)
{
    if ( value < 0.0f )         value = 0.0f;
    else if (value > 128.0f)   value = 128.0f;
    
    Lfloat normalizedValue = value * INV_128;
    
    if (type == SKPickPosition) // 4
        tKarplusStrong_setPickupPosition( pl, normalizedValue );
    else if (type == SKStringDamping) // 11
        tKarplusStrong_setBaseLoopGain( pl, 0.97f + (normalizedValue * 0.03f) );
    else if (type == SKDetune) // 1
        tKarplusStrong_setStretch( pl, 0.91f + (0.09f * (1.0f - normalizedValue)) );
}

void    tKarplusStrong_setSampleRate (tKarplusStrong* const pl, Lfloat sr)
{
    _tKarplusStrong* p = *pl;
    p->sampleRate = sr;
    
    tAllpassDelay_free(&p->delayLine);
    tAllpassDelay_initToPool(&p->delayLine, 0.0f, p->sampleRate * 2, &p->mempool);
    tAllpassDelay_clear(&p->delayLine);
    
    tLinearDelay_free(&p->combDelay);
    tLinearDelay_initToPool(&p->combDelay, 0.0f, p->sampleRate * 2, &p->mempool);
    tLinearDelay_clear(&p->combDelay);
    
    tKarplusStrong_setFrequency(pl, p->lastFrequency);
    tOneZero_setSampleRate(&p->filter, p->sampleRate);
    
    for (int i = 0; i < 4; i++)
    {
        tBiQuad_setSampleRate(&p->biquad[i], p->sampleRate);
    }
}

/* Simple Living String*/

void    tSimpleLivingString_init(tSimpleLivingString* const pl, Lfloat freq, Lfloat dampFreq,
                                 Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                 Lfloat levStrength, int levMode, LEAF* const leaf)
{
    tSimpleLivingString_initToPool(pl, freq, dampFreq, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tSimpleLivingString_initToPool  (tSimpleLivingString* const pl, Lfloat freq, Lfloat dampFreq,
                                         Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                         Lfloat levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimpleLivingString* p = *pl = (_tSimpleLivingString*) mpool_alloc(sizeof(_tSimpleLivingString), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;
    
    p->sampleRate = leaf->sampleRate;
    p->curr=0.0f;
    tExpSmooth_initToPool(&p->wlSmooth, p->sampleRate/freq, 0.01f, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tSimpleLivingString_setFreq(pl, freq);
    tLinearDelay_initToPool(&p->delayLine,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_clear(&p->delayLine);
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tHighpass_initToPool(&p->DCblocker,13, mp);
    p->decay=decay;
    tFeedbackLeveler_initToPool(&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
}

void    tSimpleLivingString_free (tSimpleLivingString* const pl)
{
    _tSimpleLivingString* p = *pl;
    
    tExpSmooth_free(&p->wlSmooth);
    tLinearDelay_free(&p->delayLine);
    tOnePole_free(&p->bridgeFilter);
    tHighpass_free(&p->DCblocker);
    tFeedbackLeveler_free(&p->fbLev);
    
    mpool_free((char*)p, p->mempool);
}

void     tSimpleLivingString_setFreq(tSimpleLivingString* const pl, Lfloat freq)
{
    _tSimpleLivingString* p = *pl;
    
    if (freq<20) freq=20;
    else if (freq>10000) freq=10000;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString_setWaveLength(tSimpleLivingString* const pl, Lfloat waveLength)
{
    _tSimpleLivingString* p = *pl;
    
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>2400) waveLength=2400;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString_setDampFreq(tSimpleLivingString* const pl, Lfloat dampFreq)
{
    _tSimpleLivingString* p = *pl;
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString_setDecay(tSimpleLivingString* const pl, Lfloat decay)
{
    _tSimpleLivingString* p = *pl;
    p->decay=decay;
}

void     tSimpleLivingString_setTargetLev(tSimpleLivingString* const pl, Lfloat targetLev)
{
    _tSimpleLivingString* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
}

void     tSimpleLivingString_setLevSmoothFactor(tSimpleLivingString* const pl, Lfloat levSmoothFactor)
{
    _tSimpleLivingString* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString_setLevStrength(tSimpleLivingString* const pl, Lfloat levStrength)
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

Lfloat   tSimpleLivingString_tick(tSimpleLivingString* const pl, Lfloat input)
{
    _tSimpleLivingString* p = *pl;
    
    Lfloat stringOut=tOnePole_tick(&p->bridgeFilter,tLinearDelay_tickOut(&p->delayLine));
    Lfloat stringInput=tHighpass_tick(&p->DCblocker, tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    tLinearDelay_tickIn(&p->delayLine, stringInput);
    tLinearDelay_setDelay(&p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    p->curr = stringOut;
    return p->curr;
}

Lfloat   tSimpleLivingString_sample(tSimpleLivingString* const pl)
{
    _tSimpleLivingString* p = *pl;
    return p->curr;
}

void   tSimpleLivingString_setSampleRate(tSimpleLivingString* const pl, Lfloat sr)
{
    _tSimpleLivingString* p = *pl;
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
}


/* Simple Living String 2*/

void    tSimpleLivingString2_init(tSimpleLivingString2* const pl, Lfloat freq, Lfloat brightness,
                                 Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                 Lfloat levStrength, int levMode, LEAF* const leaf)
{
    tSimpleLivingString2_initToPool(pl, freq, brightness, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tSimpleLivingString2_initToPool  (tSimpleLivingString2* const pl, Lfloat freq, Lfloat brightness,
                                         Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                         Lfloat levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimpleLivingString2* p = *pl = (_tSimpleLivingString2*) mpool_alloc(sizeof(_tSimpleLivingString2), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;

    p->sampleRate = leaf->sampleRate;
    p->curr=0.0f;
    tExpSmooth_initToPool(&p->wlSmooth, p->sampleRate/freq, 0.01f, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tSimpleLivingString2_setFreq(pl, freq);
    tHermiteDelay_initToPool(&p->delayLine,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_clear(&p->delayLine);
    tTwoZero_initToPool(&p->bridgeFilter, mp);
    tSimpleLivingString2_setBrightness(pl, brightness);
    tHighpass_initToPool(&p->DCblocker,13, mp);
    p->decay=decay;
    tFeedbackLeveler_initToPool(&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
}

void    tSimpleLivingString2_free (tSimpleLivingString2* const pl)
{
    _tSimpleLivingString2* p = *pl;

    tExpSmooth_free(&p->wlSmooth);
    tHermiteDelay_free(&p->delayLine);
    tTwoZero_free(&p->bridgeFilter);
    tHighpass_free(&p->DCblocker);
    tFeedbackLeveler_free(&p->fbLev);

    mpool_free((char*)p, p->mempool);
}

void     tSimpleLivingString2_setFreq(tSimpleLivingString2* const pl, Lfloat freq)
{
    _tSimpleLivingString2* p = *pl;

    if (freq<20) freq=20;
    else if (freq>10000) freq=10000;
    p->waveLengthInSamples = p->sampleRate/freq -1;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString2_setWaveLength(tSimpleLivingString2* const pl, Lfloat waveLength)
{
    _tSimpleLivingString2* p = *pl;

    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>2400) waveLength=2400;
    p->waveLengthInSamples = waveLength-1;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString2_setBrightness(tSimpleLivingString2* const pl, Lfloat brightness)
{
    _tSimpleLivingString2* p = *pl;
    Lfloat h0 = (1.0f + brightness) * 0.5f;
    Lfloat h1 = (1.0f - brightness) * 0.25f;
    tTwoZero_setCoefficients(&p->bridgeFilter, h1, h0, h1);
}

void     tSimpleLivingString2_setDecay(tSimpleLivingString2* const pl, Lfloat decay)
{
    _tSimpleLivingString2* p = *pl;
    p->decay=decay;
}

void     tSimpleLivingString2_setTargetLev(tSimpleLivingString2* const pl, Lfloat targetLev)
{
    _tSimpleLivingString2* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
}

void     tSimpleLivingString2_setLevSmoothFactor(tSimpleLivingString2* const pl, Lfloat levSmoothFactor)
{
    _tSimpleLivingString2* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString2_setLevStrength(tSimpleLivingString2* const pl, Lfloat levStrength)
{
    _tSimpleLivingString2* p = *pl;
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
}

void     tSimpleLivingString2_setLevMode(tSimpleLivingString2* const pl, int levMode)
{
    _tSimpleLivingString2* p = *pl;
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    p->levMode=levMode;
}

Lfloat   tSimpleLivingString2_tick(tSimpleLivingString2* const pl, Lfloat input)
{
    _tSimpleLivingString2* p = *pl;

    Lfloat stringOut=tTwoZero_tick(&p->bridgeFilter,tHermiteDelay_tickOut(&p->delayLine));
    Lfloat stringInput=tHighpass_tick(&p->DCblocker,(tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input)));
    tHermiteDelay_tickIn(&p->delayLine, stringInput);
    tHermiteDelay_setDelay(&p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    p->curr = stringOut;
    return p->curr;
}


Lfloat   tSimpleLivingString2_sample(tSimpleLivingString2* const pl)
{
    _tSimpleLivingString2* p = *pl;
    return p->curr;
}

void   tSimpleLivingString2_setSampleRate(tSimpleLivingString2* const pl, Lfloat sr)
{
    _tSimpleLivingString2* p = *pl;
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tTwoZero_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
}
/* Living String*/

void    tLivingString_init(tLivingString* const pl, Lfloat freq, Lfloat pickPos, Lfloat prepIndex,
                           Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                           Lfloat levStrength, int levMode, LEAF* const leaf)
{
    tLivingString_initToPool(pl, freq, pickPos, prepIndex, dampFreq, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tLivingString_initToPool    (tLivingString* const pl, Lfloat freq, Lfloat pickPos, Lfloat prepIndex,
                                     Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                     Lfloat levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tLivingString* p = *pl = (_tLivingString*) mpool_alloc(sizeof(_tLivingString), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;
    
    p->sampleRate = leaf->sampleRate;
    p->curr=0.0f;
    tExpSmooth_initToPool(&p->wlSmooth, p->sampleRate/freq, 0.01f, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tLivingString_setFreq(pl, freq);
    p->freq = freq;
    tExpSmooth_initToPool(&p->ppSmooth, pickPos, 0.01f, mp); // smoother for pick position
    tLivingString_setPickPos(pl, pickPos);
    p->prepIndex=prepIndex;
    tLinearDelay_initToPool(&p->delLF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delLB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_clear(&p->delLF);
    tLinearDelay_clear(&p->delUF);
    tLinearDelay_clear(&p->delUB);
    tLinearDelay_clear(&p->delLB);
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


/* Simple Living String 3*/

void    tSimpleLivingString3_init(tSimpleLivingString3* const pl, int oversampling, Lfloat freq, Lfloat dampFreq,
                                 Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                 Lfloat levStrength, int levMode, LEAF* const leaf)
{
    tSimpleLivingString3_initToPool(pl, oversampling, freq, dampFreq, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tSimpleLivingString3_initToPool  (tSimpleLivingString3* const pl, int oversampling, Lfloat freq, Lfloat dampFreq,
                                         Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                         Lfloat levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimpleLivingString3* p = *pl = (_tSimpleLivingString3*) mpool_alloc(sizeof(_tSimpleLivingString3), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;
    p->oversampling = oversampling;
    p->sampleRate = leaf->sampleRate * oversampling;
    p->curr=0.0f;
    p->maxLength = 2400 * oversampling;
    tExpSmooth_initToPool(&p->wlSmooth, p->sampleRate/freq/2.0f, 0.01f, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tLinearDelay_initToPool(&p->delayLineU,p->waveLengthInSamples, p->maxLength, mp);
    tLinearDelay_initToPool(&p->delayLineL,p->waveLengthInSamples, p->maxLength, mp);
    tSimpleLivingString3_setFreq(pl, freq);
    tLinearDelay_setDelay(&p->delayLineU, p->waveLengthInSamples);
    tLinearDelay_setDelay(&p->delayLineL, p->waveLengthInSamples);
    //tSimpleLivingString3_setWaveLength(pl, 4800);
    tLinearDelay_clear(&p->delayLineU);
    tLinearDelay_clear(&p->delayLineL);
    p->dampFreq = dampFreq;
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    p->rippleGain = 0.0f;
    p->rippleDelay = 0.5f;

    tHighpass_initToPool(&p->DCblocker,13, mp);
    tHighpass_setSampleRate(&p->DCblocker,p->sampleRate);
    tHighpass_setFreq(&p->DCblocker,13);
    p->userDecay = decay;

    tFeedbackLeveler_initToPool(&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
    p->changeGainCompensator = 1.0f;

    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);

    Lfloat temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    p->pickupPoint = 0.9f;
    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
    p->prevDelayLength = p->waveLengthInSamples;


}

void    tSimpleLivingString3_free (tSimpleLivingString3* const pl)
{
    _tSimpleLivingString3* p = *pl;
    
    tExpSmooth_free(&p->wlSmooth);
    tLinearDelay_free(&p->delayLineU);
    tLinearDelay_free(&p->delayLineL);
    tOnePole_free(&p->bridgeFilter);
    tHighpass_free(&p->DCblocker);
    tFeedbackLeveler_free(&p->fbLev);
    
    mpool_free((char*)p, p->mempool);
}

void     tSimpleLivingString3_setFreq(tSimpleLivingString3* const pl, Lfloat freq)
{
    _tSimpleLivingString3* p = *pl;
    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    //Lfloat decay = p->userDecay;

    Lfloat temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);

    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
}

void     tSimpleLivingString3_setWaveLength(tSimpleLivingString3* const pl, Lfloat waveLength)
{
    _tSimpleLivingString3* p = *pl;
    
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString3_setDampFreq(tSimpleLivingString3* const pl, Lfloat dampFreq)
{
    _tSimpleLivingString3* p = *pl;
    //p->temp1 = (1.0f - brightness) * 0.25f * p->decay;
    //p->temp2 = (1.0f + brightness) * 0.5f * p->decay;
    //tBiQuad_setCoefficients(&p->bridgeFilter2, p->temp1 , p->temp2, p->temp1 , 0.0f, 0.0f);

    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString3_setDecay(tSimpleLivingString3* const pl, Lfloat decay)
{
    _tSimpleLivingString3* p = *pl;
    p->userDecay = decay;

    Lfloat temp = ((decay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.9078 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);
    //p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tSimpleLivingString3_setTargetLev(tSimpleLivingString3* const pl, Lfloat targetLev)
{
    _tSimpleLivingString3* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
}

void     tSimpleLivingString3_setLevSmoothFactor(tSimpleLivingString3* const pl, Lfloat levSmoothFactor)
{
    _tSimpleLivingString3* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString3_setLevStrength(tSimpleLivingString3* const pl, Lfloat levStrength)
{
    _tSimpleLivingString3* p = *pl;
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
}

void     tSimpleLivingString3_setLevMode(tSimpleLivingString3* const pl, int levMode)
{
    _tSimpleLivingString3* p = *pl;
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    p->levMode=levMode;
}

void   tSimpleLivingString3_pluck(tSimpleLivingString3* const pl, Lfloat input, Lfloat position)
{
    _tSimpleLivingString3* p = *pl;
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
        Lfloat val = 0.0f;
        if (i <= pluckPoint)
        {
            val = input * ((Lfloat)i/(Lfloat)pluckPoint);
        }
        else
        {
            val = input * (1.0f - (((Lfloat)i-(Lfloat)pluckPoint)/(Lfloat)remainder));
                                 
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

Lfloat   tSimpleLivingString3_tick(tSimpleLivingString3* const pl, Lfloat input)
{
    _tSimpleLivingString3* p = *pl;

    //p->changeGainCompensator = 1.0f;
    Lfloat wl = tExpSmooth_tick(&p->wlSmooth);
    //Lfloat changeInDelayTime = wl - p->prevDelayLength;
    //if (changeInDelayTime < 0.0f)
    {
    //	p->changeGainCompensator = sqrtf(1.0f - changeInDelayTime);
    }
    tLinearDelay_setDelay(&p->delayLineU, wl);
    tLinearDelay_setDelay(&p->delayLineL, wl);
    
    for (int i = 0; i < p->oversampling; i++)
    {
		p->Uout = tHighpass_tick(&p->DCblocker,tOnePole_tick(&p->bridgeFilter,tLinearDelay_tickOut(&p->delayLineU))* (p->decay - p->rippleGain));
		p->Uout = LEAF_clip(-1.0f, tFeedbackLeveler_tick(&p->fbLev, p->Uout), 1.0f);
    	//p->Uout = tLinearDelay_tickOut(&p->delayLineU) * p->decay;
		p->Lout = LEAF_clip(-1.0f, tLinearDelay_tickOut(&p->delayLineL), 1.0f);

		tLinearDelay_tickIn(&p->delayLineU, (-1.0f * p->Lout) + input);
		tLinearDelay_tickIn(&p->delayLineL, -1.0f * p->Uout);
		tLinearDelay_addTo (&p->delayLineU, p->Lout * p->rippleGain, p->rippleDelay*wl);
    }
    
    //calculate pickup point
    Lfloat point = wl * p->pickupPoint;
    Lfloat reversePoint = wl * (1.0f - p->pickupPoint);
    int32_t whichPoint = ((int32_t)roundf(point));
    Lfloat LfloatPart = point - whichPoint;

    int32_t outpointplus =((int32_t)p->delayLineU->outPoint + whichPoint);
    int32_t outpointmod = outpointplus % (int32_t)p->delayLineU->maxDelay;
    int32_t outpointmod2 = (outpointmod + 1) % (int32_t)p->delayLineU->maxDelay;
    Lfloat sampleBitOne = (p->delayLineU->buff[outpointmod] * (1.0f - LfloatPart)) + (p->delayLineU->buff[outpointmod2] * LfloatPart);


    whichPoint = (int32_t)roundf(reversePoint);
    LfloatPart = reversePoint - whichPoint;
    outpointplus =(((int32_t)p->delayLineL->outPoint) + whichPoint);
    outpointmod = outpointplus % (int32_t)p->delayLineL->maxDelay;
    outpointmod2 = (outpointmod + 1) % (int32_t)p->delayLineL->maxDelay;
    Lfloat sampleBitTwo = (p->delayLineL->buff[outpointmod] * (1.0f - LfloatPart)) + (p->delayLineL->buff[outpointmod2] * LfloatPart);

    p->curr = 0.5f * (sampleBitOne + sampleBitTwo);
    //p->curr = p->Uout;

    p->prevDelayLength = p->waveLengthInSamples;
    //Lfloat stringInput=tHighpass_tick(&p->DCblocker, tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    //tLinearDelay_tickIn(&p->delayLine, stringInput);
    //tLinearDelay_setDelay(&p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    //p->curr = d1 + d2;
    return p->curr;
}


void   tSimpleLivingString3_setPickupPoint(tSimpleLivingString3* const pl, Lfloat pickupPoint)
{
    _tSimpleLivingString3* p = *pl;
    p->pickupPoint = pickupPoint;
}


Lfloat   tSimpleLivingString3_sample(tSimpleLivingString3* const pl)
{
    _tSimpleLivingString3* p = *pl;
    return p->curr;
}

void   tSimpleLivingString3_setSampleRate(tSimpleLivingString3* const pl, Lfloat sr)
{
    _tSimpleLivingString3* p = *pl;
    p->sampleRate = sr*p->oversampling;
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;

    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
    tBiQuad_setSampleRate(&p->bridgeFilter2, p->sampleRate);
}


/* Simple Living String 4*/

void    tSimpleLivingString4_init(tSimpleLivingString4* const pl, int oversampling, Lfloat freq, Lfloat dampFreq,
                                 Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                 Lfloat levStrength, int levMode, LEAF* const leaf)
{
    tSimpleLivingString4_initToPool(pl, oversampling, freq, dampFreq, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tSimpleLivingString4_initToPool  (tSimpleLivingString4* const pl, int oversampling, Lfloat freq, Lfloat dampFreq,
                                         Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                         Lfloat levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimpleLivingString4* p = *pl = (_tSimpleLivingString4*) mpool_alloc(sizeof(_tSimpleLivingString4), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;
    p->oversampling = oversampling;
    p->sampleRate = leaf->sampleRate * oversampling;
    p->curr=0.0f;
    p->maxLength = 2400 * oversampling;
    tExpSmooth_initToPool(&p->wlSmooth, p->sampleRate/freq/2.0f, 0.01f, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tLinearDelay_initToPool(&p->delayLineU,p->waveLengthInSamples, p->maxLength, mp);
    tLinearDelay_initToPool(&p->delayLineL,p->waveLengthInSamples, p->maxLength, mp);
    tSimpleLivingString4_setFreq(pl, freq);
    tLinearDelay_setDelay(&p->delayLineU, p->waveLengthInSamples);
    tLinearDelay_setDelay(&p->delayLineL, p->waveLengthInSamples);
    //tSimpleLivingString4_setWaveLength(pl, 4800);
    tLinearDelay_clear(&p->delayLineU);
    tLinearDelay_clear(&p->delayLineL);
    p->dampFreq = dampFreq;
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    p->rippleGain = 0.0f;
    p->rippleDelay = 0.5f;

    tBiQuad_initToPool(&p->bridgeFilter2, mp);
    tBiQuad_setSampleRate(&p->bridgeFilter2, p->sampleRate);

    tHighpass_initToPool(&p->DCblocker,0.001f, mp);
    tHighpass_setSampleRate(&p->DCblocker,p->sampleRate);
    tHighpass_setFreq(&p->DCblocker,0.001f);
    p->userDecay = decay;
    p->pluckPosition = 0.8f;
    tFeedbackLeveler_initToPool(&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
    p->changeGainCompensator = 1.0f;

    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);

    Lfloat temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    p->pickupPoint = 0.9f;
    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
    p->prevDelayLength = p->waveLengthInSamples;


}

void    tSimpleLivingString4_free (tSimpleLivingString4* const pl)
{
    _tSimpleLivingString4* p = *pl;
    
    tExpSmooth_free(&p->wlSmooth);
    tLinearDelay_free(&p->delayLineU);
    tLinearDelay_free(&p->delayLineL);
    tOnePole_free(&p->bridgeFilter);
    tBiQuad_free(&p->bridgeFilter2);
    tHighpass_free(&p->DCblocker);
    tFeedbackLeveler_free(&p->fbLev);
    
    mpool_free((char*)p, p->mempool);
}

void     tSimpleLivingString4_setFreq(tSimpleLivingString4* const pl, Lfloat freq)
{
    _tSimpleLivingString4* p = *pl;
    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    //Lfloat decay = p->userDecay;

    Lfloat temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);

    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
}

void     tSimpleLivingString4_setWaveLength(tSimpleLivingString4* const pl, Lfloat waveLength)
{
    _tSimpleLivingString4* p = *pl;
    
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString4_setDampFreq(tSimpleLivingString4* const pl, Lfloat dampFreq)
{
    _tSimpleLivingString4* p = *pl;
    //p->temp1 = (1.0f - brightness) * 0.25f * p->decay;
    //p->temp2 = (1.0f + brightness) * 0.5f * p->decay;
    //tBiQuad_setCoefficients(&p->bridgeFilter2, p->temp1 , p->temp2, p->temp1 , 0.0f, 0.0f);

    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString4_setDecay(tSimpleLivingString4* const pl, Lfloat decay)
{
    _tSimpleLivingString4* p = *pl;
    p->userDecay = decay;

    Lfloat temp = ((decay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);
    //p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tSimpleLivingString4_setTargetLev(tSimpleLivingString4* const pl, Lfloat targetLev)
{
    _tSimpleLivingString4* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
}

void     tSimpleLivingString4_setLevSmoothFactor(tSimpleLivingString4* const pl, Lfloat levSmoothFactor)
{
    _tSimpleLivingString4* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString4_setLevStrength(tSimpleLivingString4* const pl, Lfloat levStrength)
{
    _tSimpleLivingString4* p = *pl;
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
}

void     tSimpleLivingString4_setLevMode(tSimpleLivingString4* const pl, int levMode)
{
    _tSimpleLivingString4* p = *pl;
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    p->levMode=levMode;
}

void   tSimpleLivingString4_setPluckPosition(tSimpleLivingString4* const pl, Lfloat position)
{
    _tSimpleLivingString4* p = *pl;
    p->pluckPosition = position;
}


void   tSimpleLivingString4_pluck(tSimpleLivingString4* const pl, Lfloat input, Lfloat position)
{
    _tSimpleLivingString4* p = *pl;
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
        Lfloat val = 0.0f;
        if (i <= pluckPoint)
        {
            val = input * LEAF_tanh(((Lfloat)i/(Lfloat)pluckPoint)*1.2f);
        }
        else
        {
            val = input * LEAF_tanh((1.0f - (((Lfloat)i-(Lfloat)pluckPoint)/(Lfloat)remainder))*1.2f);
                                 
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

void   tSimpleLivingString4_pluckNoPosition(tSimpleLivingString4* const pl, Lfloat input)
{
    _tSimpleLivingString4* p = *pl;
    int length = p->waveLengthInSamples;
    Lfloat position = p->pluckPosition;
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
        Lfloat val = 0.0f;
        if (i <= pluckPoint)
        {
            val = input * LEAF_tanh(((Lfloat)i/(Lfloat)pluckPoint)*1.2f);
        }
        else
        {
            val = input * LEAF_tanh((1.0f - (((Lfloat)i-(Lfloat)pluckPoint)/(Lfloat)remainder))*1.2f);
                                 
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



Lfloat   tSimpleLivingString4_tick(tSimpleLivingString4* const pl, Lfloat input)
{
    _tSimpleLivingString4* p = *pl;

    p->changeGainCompensator = 1.0f;
    Lfloat wl = tExpSmooth_tick(&p->wlSmooth);
    volatile Lfloat changeInDelayTime = -0.01875f*(wl*0.5f - p->prevDelayLength*0.5f);
    if (changeInDelayTime < -0.1f)
    {
    	p->changeGainCompensator = sqrtf(1.0f - changeInDelayTime);
    }
    else{
        p->changeGainCompensator = 1.0f;
    }
    tLinearDelay_setDelay(&p->delayLineU, wl);
    tLinearDelay_setDelay(&p->delayLineL, wl);
    
    for (int i = 0; i < p->oversampling; i++)
    {
		p->Uout = tHighpass_tick(&p->DCblocker,tOnePole_tick(&p->bridgeFilter,tLinearDelay_tickOut(&p->delayLineU))* (p->decay - fabsf(p->rippleGain)));
		p->Uout = LEAF_clip(-1.0f, tFeedbackLeveler_tick(&p->fbLev, p->Uout), 1.0f);
    	//p->Uout = tLinearDelay_tickOut(&p->delayLineU) * p->decay;
		p->Lout = LEAF_clip(-1.0f, tLinearDelay_tickOut(&p->delayLineL), 1.0f);

		tLinearDelay_tickIn(&p->delayLineU, (-1.0f * p->Lout)* p->changeGainCompensator);
		tLinearDelay_tickIn(&p->delayLineL, (-1.0f * p->Uout));
        tLinearDelay_addTo (&p->delayLineU, input, p->pluckPosition*wl);
        tLinearDelay_addTo (&p->delayLineL, input, (1.0f - p->pluckPosition)*wl);
		tLinearDelay_addTo (&p->delayLineU, p->Lout * p->rippleGain, p->rippleDelay*wl);
    }
    
    //calculate pickup point
    volatile Lfloat point = LEAF_clip(2.0f, wl * p->pickupPoint, wl-2.0f);
    volatile Lfloat reversePoint = LEAF_clip(2.0f, wl * (1.0f - p->pickupPoint),wl-2.0f);
    volatile int32_t whichPoint = ((int32_t)(point+0.5f));
    volatile Lfloat LfloatPart = point - whichPoint;

    volatile int32_t outpointplus =((int32_t)p->delayLineU->outPoint + whichPoint);
    volatile int32_t outpointmod = outpointplus % (int32_t)p->delayLineU->maxDelay;
    volatile int32_t outpointmod2 = (outpointmod + 1) % (int32_t)p->delayLineU->maxDelay;
    volatile Lfloat sampleBitOne = (p->delayLineU->buff[outpointmod] * (1.0f - LfloatPart)) + (p->delayLineU->buff[outpointmod2] * LfloatPart);


    whichPoint = (int32_t)(reversePoint+0.5f);
    LfloatPart = reversePoint - whichPoint;
    outpointplus =(((int32_t)p->delayLineL->outPoint) + whichPoint);
    outpointmod = outpointplus % (int32_t)p->delayLineL->maxDelay;
    outpointmod2 = (outpointmod + 1) % (int32_t)p->delayLineL->maxDelay;
    Lfloat sampleBitTwo = (p->delayLineL->buff[outpointmod] * (1.0f - LfloatPart)) + (p->delayLineL->buff[outpointmod2] * LfloatPart);

    p->curr = 0.5f * (sampleBitOne + sampleBitTwo) * p->changeGainCompensator;
    //p->curr = p->Uout;

    p->prevDelayLength = p->waveLengthInSamples;
    //Lfloat stringInput=tHighpass_tick(&p->DCblocker, tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    //tLinearDelay_tickIn(&p->delayLine, stringInput);
    //tLinearDelay_setDelay(&p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    //p->curr = d1 + d2;
    return p->curr;
}


void   tSimpleLivingString4_setPickupPoint(tSimpleLivingString4* const pl, Lfloat pickupPoint)
{
    _tSimpleLivingString4* p = *pl;
    p->pickupPoint = pickupPoint;
}


Lfloat   tSimpleLivingString4_sample(tSimpleLivingString4* const pl)
{
    _tSimpleLivingString4* p = *pl;
    return p->curr;
}

void   tSimpleLivingString4_setSampleRate(tSimpleLivingString4* const pl, Lfloat sr)
{
    _tSimpleLivingString4* p = *pl;
    p->sampleRate = sr*p->oversampling;
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;

    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
    tBiQuad_setSampleRate(&p->bridgeFilter2, p->sampleRate);
}



/* Simple Living String 5*/

void    tSimpleLivingString5_init(tSimpleLivingString5* const pl, int oversampling, Lfloat freq, Lfloat dampFreq,
                                         Lfloat decay, Lfloat prepPos, Lfloat prepIndex, Lfloat pluckPos, Lfloat targetLev, Lfloat levSmoothFactor,
                                         Lfloat levStrength, int levMode, LEAF* const leaf)
{
    tSimpleLivingString5_initToPool(pl, oversampling, freq, dampFreq, decay, prepPos, prepIndex, pluckPos, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tSimpleLivingString5_initToPool  (tSimpleLivingString5* const pl, int oversampling, Lfloat freq, Lfloat dampFreq,
                                         Lfloat decay, Lfloat prepPos, Lfloat prepIndex, Lfloat pluckPos, Lfloat targetLev, Lfloat levSmoothFactor,
                                         Lfloat levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimpleLivingString5* p = *pl = (_tSimpleLivingString5*) mpool_alloc(sizeof(_tSimpleLivingString5), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;
    p->oversampling = oversampling;
    p->sampleRate = leaf->sampleRate * oversampling;
    p->curr=0.0f;
    p->maxLength = 2400 * oversampling;
    p->prepPos = prepPos;
    p->prepIndex = prepIndex;
    p->pluckPosition = pluckPos;
    tExpSmooth_initToPool(&p->prepPosSmooth, prepPos, 0.01f, mp);
    tExpSmooth_initToPool(&p->prepIndexSmooth, prepIndex, 0.01f, mp);

    tExpSmooth_initToPool(&p->wlSmooth, p->sampleRate/freq/2.0f, 0.01f, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tLinearDelay_initToPool(&p->delUF,p->waveLengthInSamples, p->maxLength, mp);
    tLinearDelay_initToPool(&p->delUB,p->waveLengthInSamples, p->maxLength, mp);
    tLinearDelay_initToPool(&p->delLF,p->waveLengthInSamples, p->maxLength, mp);
    tLinearDelay_initToPool(&p->delLB,p->waveLengthInSamples, p->maxLength, mp);
    tSimpleLivingString5_setFreq(pl, freq);
    tLinearDelay_setDelay(&p->delUF, p->waveLengthInSamples-(p->prepPos*p->waveLengthInSamples));
    tLinearDelay_setDelay(&p->delUB, p->waveLengthInSamples-((1.0f-p->prepPos)*p->waveLengthInSamples));
    tLinearDelay_setDelay(&p->delLF, p->waveLengthInSamples-(p->prepPos*p->waveLengthInSamples));
    tLinearDelay_setDelay(&p->delLB, p->waveLengthInSamples-((1.0f-p->prepPos)*p->waveLengthInSamples));
    //tSimpleLivingString5_setWaveLength(pl, 4800);
    tLinearDelay_clear(&p->delUF);
    tLinearDelay_clear(&p->delUB);
    tLinearDelay_clear(&p->delLF);
    tLinearDelay_clear(&p->delLB);
    p->dampFreq = dampFreq;
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);

    tOnePole_initToPool(&p->nutFilter, dampFreq, mp);
    tOnePole_setSampleRate(&p->nutFilter, p->sampleRate);
    tOnePole_setFreq(&p->nutFilter, dampFreq);




    p->rippleGain = 0.0f;
    p->rippleDelay = 0.5f;


    tHighpass_initToPool(&p->DCblocker,0.001f, mp);
    tHighpass_setSampleRate(&p->DCblocker,p->sampleRate);
    tHighpass_setFreq(&p->DCblocker,0.001f);
    
    tHighpass_initToPool(&p->DCblocker2,0.001f, mp);
    tHighpass_setSampleRate(&p->DCblocker2,p->sampleRate);
    tHighpass_setFreq(&p->DCblocker2,0.001f);
    
    p->userDecay = decay;
    p->pluckPosition = 0.8f;
    tFeedbackLeveler_initToPool(&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
    p->changeGainCompensator = 1.0f;
    
    tFeedbackLeveler_initToPool(&p->fbLev2, targetLev, levSmoothFactor, levStrength, levMode, mp);

    tWavefolder_initToPool(&p->wf1, 0.0f, 0.4f, 0.5f, mp);
    tWavefolder_initToPool(&p->wf2, 0.0f, 0.4f, 0.5f, mp);
    tWavefolder_initToPool(&p->wf3, 0.0f, 0.4f, 0.5f, mp);
    tWavefolder_initToPool(&p->wf4, 0.0f, 0.4f, 0.5f, mp);


    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);

    //Lfloat temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    //p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    p->decay = decay;
    p->pickupPoint = 0.9f;
    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
    p->prevDelayLength = p->waveLengthInSamples;


}

void    tSimpleLivingString5_free (tSimpleLivingString5* const pl)
{
    _tSimpleLivingString5* p = *pl;
    
    tExpSmooth_free(&p->wlSmooth);
    tExpSmooth_free(&p->prepIndexSmooth);
    tExpSmooth_free(&p->prepPosSmooth);
    tLinearDelay_free(&p->delUF);
    tLinearDelay_free(&p->delUB);
    tLinearDelay_free(&p->delLF);
    tLinearDelay_free(&p->delLB);
    tOnePole_free(&p->bridgeFilter);
    tOnePole_free(&p->nutFilter);


    tHighpass_free(&p->DCblocker);
    tHighpass_free(&p->DCblocker2);
    tFeedbackLeveler_free(&p->fbLev);
    tFeedbackLeveler_free(&p->fbLev2);
    
    tWavefolder_free(&p->wf1);
    tWavefolder_free(&p->wf2);
    tWavefolder_free(&p->wf3);
    tWavefolder_free(&p->wf4);

    mpool_free((char*)p, p->mempool);
}

void     tSimpleLivingString5_setFreq(tSimpleLivingString5* const pl, Lfloat freq)
{
    _tSimpleLivingString5* p = *pl;
    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    //Lfloat decay = p->userDecay;

    //Lfloat temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    //p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);

    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
}

void     tSimpleLivingString5_setWaveLength(tSimpleLivingString5* const pl, Lfloat waveLength)
{
    _tSimpleLivingString5* p = *pl;
    
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString5_setDampFreq(tSimpleLivingString5* const pl, Lfloat dampFreq)
{
    _tSimpleLivingString5* p = *pl;
    //p->temp1 = (1.0f - brightness) * 0.25f * p->decay;
    //p->temp2 = (1.0f + brightness) * 0.5f * p->decay;
    //tBiQuad_setCoefficients(&p->bridgeFilter2, p->temp1 , p->temp2, p->temp1 , 0.0f, 0.0f);

    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    tOnePole_setFreq(&p->nutFilter, dampFreq);
}

void     tSimpleLivingString5_setDecay(tSimpleLivingString5* const pl, Lfloat decay)
{
    _tSimpleLivingString5* p = *pl;
    p->userDecay = decay;

    
    //Lfloat temp = ((decay * 0.01f) + 0.01f) * 6.9078f;
    //p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    p->decay = decay;//;
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);
    //p->decay=powf(0.001f,1.0f/(p->freq*decay));
}
void   tSimpleLivingString5_setPrepPosition(tSimpleLivingString5* const pl, Lfloat prepPosition)
{
     _tSimpleLivingString5* p = *pl;
    p->prepPos = prepPosition;
}
void   tSimpleLivingString5_setPrepIndex(tSimpleLivingString5* const pl, Lfloat prepIndex)
{
     _tSimpleLivingString5* p = *pl;
     p->prepIndex = prepIndex;
}

void     tSimpleLivingString5_setTargetLev(tSimpleLivingString5* const pl, Lfloat targetLev)
{
    _tSimpleLivingString5* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLev, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLev2, targetLev);
}

void     tSimpleLivingString5_setLevSmoothFactor(tSimpleLivingString5* const pl, Lfloat levSmoothFactor)
{
    _tSimpleLivingString5* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLev, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLev2, levSmoothFactor);
}

void     tSimpleLivingString5_setLevStrength(tSimpleLivingString5* const pl, Lfloat levStrength)
{
    _tSimpleLivingString5* p = *pl;
    tFeedbackLeveler_setStrength(&p->fbLev, levStrength);
    tFeedbackLeveler_setStrength(&p->fbLev2, levStrength);
}

void     tSimpleLivingString5_setLevMode(tSimpleLivingString5* const pl, int levMode)
{
    _tSimpleLivingString5* p = *pl;
    tFeedbackLeveler_setMode(&p->fbLev, levMode);
    tFeedbackLeveler_setMode(&p->fbLev2, levMode);
    p->levMode=levMode;
}

void   tSimpleLivingString5_setPluckPosition(tSimpleLivingString5* const pl, Lfloat position)
{
    _tSimpleLivingString5* p = *pl;
    p->pluckPosition = position;
}


void   tSimpleLivingString5_pluck(tSimpleLivingString5* const pl, Lfloat input, Lfloat position)
{
    _tSimpleLivingString5* p = *pl;

    p->pluckPosition = position;
    volatile Lfloat pluckPoint = position*p->waveLengthInSamples;
    pluckPoint = LEAF_clip(1.0f, pluckPoint, p->waveLengthInSamples-1.0f);
    uint32_t pluckPointInt = (uint32_t) pluckPoint;
    volatile Lfloat remainder = p->waveLengthInSamples-pluckPoint;
    Lfloat BLen = p->delUB->delay;
    Lfloat FLen = p->delUF->delay;
    uint32_t FLenInt = (uint32_t)FLen;
    float FLenAlpha = FLen -FLenInt;
    uint32_t BLenInt = (uint32_t)BLen;
    float BLenAlpha = BLen -BLenInt;
    
    for (uint32_t i = 0; i < p->waveLengthInSamples; i++)
    {
        Lfloat val = 0.0f;
        if (i <= pluckPointInt)
        {
            val = input * ((Lfloat)i/(Lfloat)pluckPointInt);
        }
        else
        {
            val = input * (1.0f - (((Lfloat)i-(Lfloat)pluckPointInt)/(Lfloat)remainder));
        }
        if (i  < BLenInt)
        {
            tLinearDelay_addTo(&p->delUB, val, i);
            tLinearDelay_addTo(&p->delLB, val, BLenInt-i);
        }
        else if (i == BLenInt)
        {
            tLinearDelay_addTo(&p->delUB, val * (1.0f-BLenAlpha), i);
            tLinearDelay_addTo(&p->delLB, val * (1.0f-BLenAlpha), BLenInt-i);
            tLinearDelay_addTo(&p->delUF, val * BLenAlpha, i-BLenInt);
            tLinearDelay_addTo(&p->delLF, val * BLenAlpha, (FLenInt-1)-(i-BLenInt));
        }
        else
        {
            tLinearDelay_addTo(&p->delUF, val, i-BLenInt);
            tLinearDelay_addTo(&p->delLF, val, (FLenInt-1)-(i-BLenInt));
        }
    }
}

void   tSimpleLivingString5_pluckNoPosition(tSimpleLivingString5* const pl, Lfloat input)
{
    _tSimpleLivingString5* p = *pl;
     //pluck is to the right of the prep
    if (p->pluckPosition >= p->prepPos)
    {
        int length = p->waveLengthInSamples * (1.0f - p->prepPos); //what's the length of the right side of the prep
        int pluckPoint = (int)(length * (p->pluckPosition - (1.0f - p->prepPos)));
        if (pluckPoint < 2)
        {
            pluckPoint = 2;
        }
        else if (pluckPoint > (length-2))
        {
            pluckPoint = length-2;
        }
        int remainder = length-pluckPoint;
        
        for (int i = 0; i < length; i++)
        {
            Lfloat val = 0.0f;
            if (i <= pluckPoint)
            {
                val = input * LEAF_tanh(((Lfloat)i/(Lfloat)pluckPoint)*1.2f);
            }
            else
            {
                val = input * LEAF_tanh((1.0f - (((Lfloat)i-(Lfloat)pluckPoint)/(Lfloat)remainder))*1.2f);
                                    
            }
            int bufWritePoint = (i+p->delUF->outPoint)%p->delUF->maxDelay;
            p->delUF->buff[bufWritePoint] = val;
        }
        for (int i = 0; i < length; i++)
        {
            int otherBufPosition = ((length-1-i) + p->delUF->outPoint)%p->delUF->maxDelay;
            int bufWritePoint = (i+p->delLF->outPoint)%p->delLF->maxDelay;
            p->delLF->buff[bufWritePoint] = p->delUF->buff[otherBufPosition];
        }
    }
    else
    {
        int length = p->waveLengthInSamples * p->prepPos; //what's the length of the left side of the prep
        int pluckPoint = (int)(length * (p->pluckPosition - p->prepPos));
        if (pluckPoint < 2)
        {
            pluckPoint = 2;
        }
        else if (pluckPoint > (length-2))
        {
            pluckPoint = length-2;
        }
        int remainder = length-pluckPoint;
        
        for (int i = 0; i < length; i++)
        {
            Lfloat val = 0.0f;
            if (i <= pluckPoint)
            {
                val = input * LEAF_tanh(((Lfloat)i/(Lfloat)pluckPoint)*1.2f);
            }
            else
            {
                val = input * LEAF_tanh((1.0f - (((Lfloat)i-(Lfloat)pluckPoint)/(Lfloat)remainder))*1.2f);
                                    
            }
            int bufWritePoint = (i+p->delUB->outPoint)%p->delUB->maxDelay;
            p->delUB->buff[bufWritePoint] = val;
        }
        for (int i = 0; i < length; i++)
        {
            int otherBufPosition = ((length-1-i) + p->delUB->outPoint)%p->delUB->maxDelay;
            int bufWritePoint = (i+p->delLB->outPoint)%p->delLB->maxDelay;
            p->delLB->buff[bufWritePoint] = p->delUB->buff[otherBufPosition];
        }
    }
}





Lfloat   tSimpleLivingString5_tick(tSimpleLivingString5* const pl, Lfloat input)
{
    _tSimpleLivingString5* p = *pl;

    //p->changeGainCompensator = 1.0f;
    Lfloat wl = tExpSmooth_tick(&p->wlSmooth);
    //volatile Lfloat changeInDelayTime = -0.01875f*(wl*0.5f - p->prevDelayLength*0.5f);
    //if (changeInDelayTime < -0.1f)
    {
    //	p->changeGainCompensator = sqrtf(1.0f - changeInDelayTime);
    }
    //else
    {
    //    p->changeGainCompensator = 1.0f;
    }
    Lfloat FLen = wl*(1.0f-p->prepPos);
    uint32_t FLenInt = (uint32_t)FLen;
    Lfloat BLen = wl*p->prepPos;
    uint32_t BLenInt = (uint32_t)BLen;
    tLinearDelay_setDelay(&p->delUF, FLen);
    tLinearDelay_setDelay(&p->delUB, BLen);
    tLinearDelay_setDelay(&p->delLF, FLen);
    tLinearDelay_setDelay(&p->delLB, BLen);

    Lfloat pluckPosInSamples = p->pluckPosition * wl;
    uint32_t pluckPosInSamplesInt = (uint32_t) pluckPosInSamples;
    Lfloat alpha = pluckPosInSamples - pluckPosInSamplesInt;

    uint32_t sample1 = pluckPosInSamplesInt;
    uint32_t sample2 = pluckPosInSamplesInt + 1;

    uint32_t sample1Front = (uint32_t) (sample1 - BLen);
    uint32_t sample2Front = (uint32_t) (sample2 - BLen);

    if (sample1 < BLenInt)
    {
        tLinearDelay_addTo(&p->delUB, input * (1.0f - alpha), sample1);
        tLinearDelay_addTo(&p->delLB, input * (1.0f - alpha), BLenInt-sample1);
    }
    else
    {
        tLinearDelay_addTo(&p->delUF, input * (1.0f - alpha), sample1Front);
        tLinearDelay_addTo(&p->delLF, input * (1.0f - alpha), FLenInt-sample1Front);
    }
    if (sample2 < BLenInt)
    {
        tLinearDelay_addTo(&p->delUB, input * alpha, sample2);
        tLinearDelay_addTo(&p->delLB, input * alpha, BLenInt-sample2);
    }
    else
    {
        tLinearDelay_addTo(&p->delUF, input * alpha, sample2Front);
        tLinearDelay_addTo(&p->delLF, input * alpha, FLenInt-sample2Front);
    }

    //now tick out the output data and filter (oversampled)
    for (int i = 0; i < p->oversampling; i++)
    {
        Lfloat fromUF=tLinearDelay_tickOut(&p->delUF);
        Lfloat fromLF=tLinearDelay_tickOut(&p->delLF);
        Lfloat fromLB=tLinearDelay_tickOut(&p->delLB);
        Lfloat fromUB=tLinearDelay_tickOut(&p->delUB);
        
        fromUF = tWavefolder_tick(&p->wf1, fromUF);
        fromLB = tWavefolder_tick(&p->wf2, fromLB);


#if 0
        Lfloat fromBridge = -tOnePole_tick(&p->bridgeFilter, fromUF);
        tLinearDelay_tickIn(&p->delLF, fromBridge + input);
        tLinearDelay_tickIn(&p->delLB, fromLF);
        Lfloat fromNut=-fromLB;
        tLinearDelay_tickIn(&p->delUB, fromNut);
        tLinearDelay_tickIn(&p->delUF, fromUB);
#endif

        // into front half of string, from bridge, going backwards (lower section)
        //Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblocker, tOnePole_tick(&p->bridgeFilter, fromUF)));
        //Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay:1.0f)*fromUF);
        Lfloat fromBridge=-p->decay*tOnePole_tick(&p->bridgeFilter, fromUF);
        //Lfloat fromBridge=-tHighpass_tick(&p->DCblocker, tOnePole_tick(&p->bridgeFilter, fromUF)) * p->decay; //version with feedbackleveler
        //fromBridge = LEAF_clip(-1.0f, fromBridge, 1.0f);
        tLinearDelay_tickIn(&p->delLF, fromBridge);
        // into back half of string, from prepPoint, going backwards (lower section)
        //Lfloat fromUpperPrep=-tOnePole_tick(&p->prepFilterL, fromUB);
        Lfloat fromUpperPrep=-fromUB;
        //fromUpperPrep = LEAF_clip(-1.0f, fromUpperPrep, 1.0f);
        Lfloat intoLower=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF; //used to add input here
        //intoLower = LEAF_clip(-1.0f, intoLower, 1.0f);
        tLinearDelay_tickIn(&p->delLB, intoLower);
        // into back of string, from nut going forward (upper section)
        //Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLev2, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblocker2,fromLB));
        //Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLev2, (p->levMode==0?p->decay:1.0f)*fromLB);
        Lfloat fromNut=-p->decay*tOnePole_tick(&p->nutFilter, fromLB);
        //Lfloat fromNut=-p->decay*tHighpass_tick(&p->DCblocker2,fromLB)); //version without feedback leveler
        //fromNut = LEAF_clip(-1.0f, fromNut, 1.0f);
        tLinearDelay_tickIn(&p->delUB, fromNut);
        // into front half of string, from prepPoint, going forward (upper section)
        //Lfloat fromLowerPrep=-tOnePole_tick(&p->prepFilterU, fromLF);
        Lfloat fromLowerPrep=-fromLF;
        //fromLowerPrep = LEAF_clip(-1.0f, fromLowerPrep, 1.0f);
        Lfloat intoUpperFront=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB;
        //intoUpper = tanhf(intoUpper);
        //intoUpperFront = LEAF_clip(-1.0f, intoUpperFront, 1.0f);
        tLinearDelay_tickIn(&p->delUF, intoUpperFront);

        p->curr = fromBridge;
    }

    //pick up the signal
    Lfloat pickupPosInSamples = p->pickupPoint * wl;
    uint32_t pickupPosInSamplesInt = (uint32_t) pickupPosInSamples;
    alpha = pickupPosInSamples - pickupPosInSamplesInt;

    sample1 = pickupPosInSamplesInt;
    sample2 = pickupPosInSamplesInt + 1;

    uint32_t sample1Front2 = (uint32_t) (sample1 - BLen);
    uint32_t sample2Front2 = (uint32_t) (sample2 - BLen);

    float outputSample1 = 0.0f;
    float outputSample2 = 0.0f;

    if (sample1 < BLenInt)
    {
        outputSample1 = tLinearDelay_tapOut(&p->delUB, sample1);
        outputSample1 += tLinearDelay_tapOut(&p->delLB, BLenInt-sample1);
    }
    else
    {
        outputSample1 = tLinearDelay_tapOut(&p->delUF, sample1Front2);
        outputSample1 += tLinearDelay_tapOut(&p->delLF, FLenInt-sample1Front2);
    }
    if (sample2 < BLenInt)
    {
        outputSample2 = tLinearDelay_tapOut(&p->delUB, sample2);
        outputSample2 += tLinearDelay_tapOut(&p->delLB, BLenInt-sample2);
    }
    else
    {
        outputSample2 = tLinearDelay_tapOut(&p->delUF, sample2Front2);
        outputSample2 += tLinearDelay_tapOut(&p->delLF, FLenInt-sample2Front2);
    }
    p->curr = 0.5f * ((outputSample1 * (1.0f-alpha))+ (outputSample2 * alpha));

#if 0
    if (p->pickupPoint > p->prepPos)
    {
        Lfloat remain = (1.0f-p->prepPos);

        Lfloat pickupSection = (p->pickupPoint-remain) * 1.0f / remain;
        //calculate pickup point
        volatile Lfloat point = LEAF_clip(2.0f, FLen * pickupSection, FLen-2.0f);
        volatile Lfloat reversePoint = LEAF_clip(2.0f, FLen * (1.0f - pickupSection),FLen-2.0f);
        volatile int32_t whichPoint = ((int32_t)(point+0.5f));
        volatile Lfloat LfloatPart = point - whichPoint;

        volatile int32_t outpointplus =((int32_t)p->delUF->outPoint + whichPoint);
        volatile int32_t outpointmod = outpointplus % (int32_t)p->delUF->maxDelay;
        volatile int32_t outpointmod2 = (outpointmod + 1) % (int32_t)p->delUF->maxDelay;
        volatile Lfloat sampleBitOne = (p->delUF->buff[outpointmod] * (1.0f - LfloatPart)) + (p->delUF->buff[outpointmod2] * LfloatPart);


        whichPoint = (int32_t)(reversePoint+0.5f);
        LfloatPart = reversePoint - whichPoint;
        outpointplus =(((int32_t)p->delLF->outPoint) + whichPoint);
        outpointmod = outpointplus % (int32_t)p->delLF->maxDelay;
        outpointmod2 = (outpointmod + 1) % (int32_t)p->delLF->maxDelay;
        Lfloat sampleBitTwo = (p->delLF->buff[outpointmod] * (1.0f - LfloatPart)) + (p->delLF->buff[outpointmod2] * LfloatPart);

        p->curr = 0.5f * (sampleBitOne + sampleBitTwo) * p->changeGainCompensator;
    }
    else
    {
        //
        Lfloat pickupSection = 1.0f - ((p->prepPos-p->pickupPoint) * (1.0f/p->prepPos));
        volatile Lfloat point = LEAF_clip(2.0f, BLen * pickupSection, BLen-2.0f);
        volatile Lfloat reversePoint = LEAF_clip(2.0f, BLen * (1.0f - pickupSection),BLen-2.0f);
        volatile int32_t whichPoint = ((int32_t)(point+0.5f));
        volatile Lfloat LfloatPart = point - whichPoint;

        volatile int32_t outpointplus =((int32_t)p->delUB->outPoint + whichPoint);
        volatile int32_t outpointmod = outpointplus % (int32_t)p->delUB->maxDelay;
        volatile int32_t outpointmod2 = (outpointmod + 1) % (int32_t)p->delUB->maxDelay;
        volatile Lfloat sampleBitOne = (p->delUB->buff[outpointmod] * (1.0f - LfloatPart)) + (p->delUB->buff[outpointmod2] * LfloatPart);


        whichPoint = (int32_t)(reversePoint+0.5f);
        LfloatPart = reversePoint - whichPoint;
        outpointplus =(((int32_t)p->delLB->outPoint) + whichPoint);
        outpointmod = outpointplus % (int32_t)p->delLB->maxDelay;
        outpointmod2 = (outpointmod + 1) % (int32_t)p->delLB->maxDelay;
        Lfloat sampleBitTwo = (p->delLB->buff[outpointmod] * (1.0f - LfloatPart)) + (p->delLB->buff[outpointmod2] * LfloatPart);

        p->curr = 0.5f * (sampleBitOne + sampleBitTwo) * p->changeGainCompensator;
    }
#endif
    //p->curr = p->Uout;
    p->prevDelayLength = p->waveLengthInSamples;
    //Lfloat stringInput=tHighpass_tick(&p->DCblocker, tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    //tLinearDelay_tickIn(&p->delayLine, stringInput);
    //tLinearDelay_setDelay(&p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    //p->curr = d1 + d2;
    return p->curr;
}


void   tSimpleLivingString5_setPickupPoint(tSimpleLivingString5* const pl, Lfloat pickupPoint)
{
    _tSimpleLivingString5* p = *pl;
    p->pickupPoint = pickupPoint;
}


Lfloat   tSimpleLivingString5_sample(tSimpleLivingString5* const pl)
{
    _tSimpleLivingString5* p = *pl;
    return p->curr;
}

void   tSimpleLivingString5_setSampleRate(tSimpleLivingString5* const pl, Lfloat sr)
{
    _tSimpleLivingString5* p = *pl;
    p->sampleRate = sr*p->oversampling;
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;

    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
}

void   tSimpleLivingString5_setFBAmount(tSimpleLivingString5* const pl, Lfloat fb)
{
    _tSimpleLivingString5* p = *pl;
    tWavefolder_setFBAmount(&p->wf1, fb);
    tWavefolder_setFBAmount(&p->wf2, fb);
}
void   tSimpleLivingString5_setFFAmount(tSimpleLivingString5* const pl, Lfloat ff)
{
    _tSimpleLivingString5* p = *pl;
    tWavefolder_setFFAmount(&p->wf1, ff);
    tWavefolder_setFFAmount(&p->wf2, ff);
}
void   tSimpleLivingString5_setFoldDepth(tSimpleLivingString5* const pl, Lfloat depth)
{
    _tSimpleLivingString5* p = *pl;
    tWavefolder_setFoldDepth(&p->wf1, depth);
    tWavefolder_setFoldDepth(&p->wf2, depth);
}





void    tLivingString_free (tLivingString* const pl)
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
    
    mpool_free((char*)p, p->mempool);
}

void     tLivingString_setFreq(tLivingString* const pl, Lfloat freq)
{    // NOTE: It is faster to set wavelength in samples directly
    _tLivingString* p = *pl;
    
    if (freq<20.f) freq=20.f;
    else if (freq>10000.f) freq=10000.f;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString_setWaveLength(tLivingString* const pl, Lfloat waveLength)
{
    _tLivingString* p = *pl;
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.f) waveLength=2400.f;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString_setPickPos(tLivingString* const pl, Lfloat pickPos)
{    // between 0 and 1
    _tLivingString* p = *pl;
    if (pickPos<0.f) pickPos=0.f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(&p->ppSmooth, p->pickPos);
}

void     tLivingString_setPrepIndex(tLivingString* const pl, Lfloat prepIndex)
{    // between 0 and 1
    _tLivingString* p = *pl;
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tLivingString_setDampFreq(tLivingString* const pl, Lfloat dampFreq)
{
    _tLivingString* p = *pl;
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    tOnePole_setFreq(&p->nutFilter, dampFreq);
    tOnePole_setFreq(&p->prepFilterU, dampFreq);
    tOnePole_setFreq(&p->prepFilterL, dampFreq);
}

void     tLivingString_setDecay(tLivingString* const pl, Lfloat decay)
{
    _tLivingString* p = *pl;
    p->decay=decay;
}

void     tLivingString_setTargetLev(tLivingString* const pl, Lfloat targetLev)
{
    _tLivingString* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLevL, targetLev);
}

void     tLivingString_setLevSmoothFactor(tLivingString* const pl, Lfloat levSmoothFactor)
{
    _tLivingString* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLevL, levSmoothFactor);
}

void     tLivingString_setLevStrength(tLivingString* const pl, Lfloat levStrength)
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

Lfloat   tLivingString_tick(tLivingString* const pl, Lfloat input)
{
    _tLivingString* p = *pl;
    
    // from pickPos upwards=forwards
    Lfloat fromLF=tLinearDelay_tickOut(&p->delLF);
    Lfloat fromUF=tLinearDelay_tickOut(&p->delUF);
    Lfloat fromUB=tLinearDelay_tickOut(&p->delUB);
    Lfloat fromLB=tLinearDelay_tickOut(&p->delLB);
    // into upper half of string, from nut, going backwards
    Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(&p->DCblockerU, tOnePole_tick(&p->nutFilter, fromUF)));
    tLinearDelay_tickIn(&p->delUB, fromNut);
    // into lower half of string, from pickpoint, going backwards
    Lfloat fromLowerPrep=-tOnePole_tick(&p->prepFilterL, fromLF);
    Lfloat intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB+input;
    tLinearDelay_tickIn(&p->delLB, intoLower);
    // into lower half of string, from bridge
    Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tOnePole_tick(&p->bridgeFilter, fromLB)));
    tLinearDelay_tickIn(&p->delLF, fromBridge);
    // into upper half of string, from pickpoint, going forwards/upwards
    Lfloat fromUpperPrep=-tOnePole_tick(&p->prepFilterU, fromUB);
    Lfloat intoUpper=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF+input;
    tLinearDelay_tickIn(&p->delUF, intoUpper);
    // update all delay lengths
    Lfloat pickP=tExpSmooth_tick(&p->ppSmooth);
    Lfloat wLen=tExpSmooth_tick(&p->wlSmooth);
    Lfloat lowLen=pickP*wLen;
    Lfloat upLen=(1.0f-pickP)*wLen;
    tLinearDelay_setDelay(&p->delLF, lowLen);
    tLinearDelay_setDelay(&p->delLB, lowLen);
    tLinearDelay_setDelay(&p->delUF, upLen);
    tLinearDelay_setDelay(&p->delUB, upLen);
    p->curr = fromBridge;
    return p->curr;
}

Lfloat   tLivingString_sample(tLivingString* const pl)
{
    _tLivingString* p = *pl;
    return p->curr;
}

void   tLivingString_setSampleRate(tLivingString* const pl, Lfloat sr)
{
    _tLivingString* p = *pl;
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;
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


//////////---------------------------
/* Version of Living String with Hermite Interpolation */
/*Living String experiment 2 */

/* Living String*/

void    tLivingString2_init(tLivingString2* const pl, Lfloat freq, Lfloat pickPos, Lfloat prepPos, Lfloat pickupPos, Lfloat prepIndex,
                           Lfloat brightness, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                           Lfloat levStrength, int levMode, LEAF* const leaf)
{
    tLivingString2_initToPool(pl, freq, pickPos, prepPos, pickupPos, prepIndex, brightness, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tLivingString2_initToPool    (tLivingString2* const pl, Lfloat freq, Lfloat pickPos, Lfloat prepPos, Lfloat pickupPos, Lfloat prepIndex,
                                     Lfloat brightness, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                     Lfloat levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tLivingString2* p = *pl = (_tLivingString2*) mpool_alloc(sizeof(_tLivingString2), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;

    p->sampleRate = leaf->sampleRate;
    p->curr=0.0f;
    tExpSmooth_initToPool(&p->wlSmooth, p->sampleRate/freq, 0.1f, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tLivingString2_setFreq(pl, freq);
    p->freq = freq;
    p->prepPos = prepPos;
    tExpSmooth_initToPool(&p->ppSmooth, pickPos, 0.01f, mp); // smoother for pick position
    tExpSmooth_initToPool(&p->prpSmooth, prepPos, 0.01f, mp); // smoother for prep position
    tExpSmooth_initToPool(&p->puSmooth, pickupPos, 0.01f, mp); // smoother for pickup position
    tLivingString2_setPickPos(pl, pickPos);
    tLivingString2_setPrepPos(pl, prepPos);
    p->prepIndex = prepIndex;
    p->pickupPos = pickupPos;
    tHermiteDelay_initToPool(&p->delLF,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_initToPool(&p->delUF,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_initToPool(&p->delUB,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_initToPool(&p->delLB,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_clear(&p->delLF);
    tHermiteDelay_clear(&p->delUF);
    tHermiteDelay_clear(&p->delUB);
    tHermiteDelay_clear(&p->delLB);
    p->brightness = brightness;
    tTwoZero_initToPool(&p->bridgeFilter, mp);
    tTwoZero_initToPool(&p->nutFilter, mp);
    tTwoZero_initToPool(&p->prepFilterU, mp);
    tTwoZero_initToPool(&p->prepFilterL, mp);
    tLivingString2_setBrightness(pl, brightness);
    tHighpass_initToPool(&p->DCblockerU,8, mp);
    tHighpass_initToPool(&p->DCblockerL,8, mp);
    p->decay=decay;
    p->prepIndex = prepIndex;
    tFeedbackLeveler_initToPool(&p->fbLevU, targetLev, levSmoothFactor, levStrength, levMode, mp);
    tFeedbackLeveler_initToPool(&p->fbLevL, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
}

void    tLivingString2_free (tLivingString2* const pl)
{
    _tLivingString2* p = *pl;

    tExpSmooth_free(&p->wlSmooth);
    tExpSmooth_free(&p->ppSmooth);
    tExpSmooth_free(&p->prpSmooth);
    tExpSmooth_free(&p->puSmooth);
    tHermiteDelay_free(&p->delLF);
    tHermiteDelay_free(&p->delUF);
    tHermiteDelay_free(&p->delUB);
    tHermiteDelay_free(&p->delLB);
    tTwoZero_free(&p->bridgeFilter);
    tTwoZero_free(&p->nutFilter);
    tTwoZero_free(&p->prepFilterU);
    tTwoZero_free(&p->prepFilterL);
    tHighpass_free(&p->DCblockerU);
    tHighpass_free(&p->DCblockerL);
    tFeedbackLeveler_free(&p->fbLevU);
    tFeedbackLeveler_free(&p->fbLevL);

    mpool_free((char*)p, p->mempool);
}

void     tLivingString2_setFreq(tLivingString2* const pl, Lfloat freq)
{    // NOTE: It is faster to set wavelength in samples directly
    _tLivingString2* p = *pl;
    
    if (freq<20.f) freq=20.f;
    else if (freq>10000.f) freq=10000.f;
    freq = freq*2;
    p->freq = freq;
    p->waveLengthInSamples = (p->sampleRate/p->freq) - 1;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString2_setWaveLength(tLivingString2* const pl, Lfloat waveLength)
{
    _tLivingString2* p = *pl;
    
    waveLength = waveLength * 0.5f;
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.f) waveLength=2400.f;
    p->waveLengthInSamples = waveLength - 1;
    p->freq = p->sampleRate / waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString2_setPickPos(tLivingString2* const pl, Lfloat pickPos)
{    // between 0 and 1
    _tLivingString2* p = *pl;
    if (pickPos<0.f) pickPos=0.f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(&p->ppSmooth, p->pickPos);
}

void     tLivingString2_setPrepPos(tLivingString2* const pl, Lfloat prepPos)
{    // between 0 and 1
    _tLivingString2* p = *pl;
    if (prepPos<0.f) prepPos=0.f;
    else if (prepPos>1.f) prepPos=1.f;
    p->prepPos = prepPos;
    tExpSmooth_setDest(&p->prpSmooth, p->prepPos);
}

void     tLivingString2_setPickupPos(tLivingString2* const pl, Lfloat pickupPos)
{    // between 0 and 1
    _tLivingString2* p = *pl;
    if (pickupPos<0.f) pickupPos=0.f;
    else if (pickupPos>1.f) pickupPos=1.f;
    p->pickupPos = pickupPos;
    tExpSmooth_setDest(&p->puSmooth, p->pickupPos);
}

void     tLivingString2_setPrepIndex(tLivingString2* const pl, Lfloat prepIndex)
{    // between 0 and 1
    _tLivingString2* p = *pl;
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tLivingString2_setBrightness(tLivingString2* const pl, Lfloat brightness)
{
    _tLivingString2* p = *pl;
    Lfloat h0=(1.0 + brightness) * 0.5f;
    Lfloat h1=(1.0 - brightness) * 0.25f;

    tTwoZero_setCoefficients(&p->bridgeFilter, h1, h0, h1);
    tTwoZero_setCoefficients(&p->nutFilter, h1, h0, h1);
    tTwoZero_setCoefficients(&p->prepFilterU, h1, h0, h1);
    tTwoZero_setCoefficients(&p->prepFilterL, h1, h0, h1);
}

void     tLivingString2_setDecay(tLivingString2* const pl, Lfloat decay)
{
    _tLivingString2* p = *pl;
    p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tLivingString2_setTargetLev(tLivingString2* const pl, Lfloat targetLev)
{
    _tLivingString2* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLevL, targetLev);
}

void     tLivingString2_setLevSmoothFactor(tLivingString2* const pl, Lfloat levSmoothFactor)
{
    _tLivingString2* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLevL, levSmoothFactor);
}

void     tLivingString2_setLevStrength(tLivingString2* const pl, Lfloat levStrength)
{
    _tLivingString2* p = *pl;
    tFeedbackLeveler_setStrength(&p->fbLevU, levStrength);
    tFeedbackLeveler_setStrength(&p->fbLevL, levStrength);
}

void     tLivingString2_setLevMode(tLivingString2* const pl, int levMode)
{
    _tLivingString2* p = *pl;
    tFeedbackLeveler_setMode(&p->fbLevU, levMode);
    tFeedbackLeveler_setMode(&p->fbLevL, levMode);
    p->levMode=levMode;
}

Lfloat   tLivingString2_tick(tLivingString2* const pl, Lfloat input)
{
    _tLivingString2* p = *pl;

    input = input * 0.5f; // drop gain by half since we'll be equally adding it at half amplitude to forward and backward waveguides
    // from prepPos upwards=forwards
    Lfloat wLen=tExpSmooth_tick(&p->wlSmooth);

    Lfloat pickP=tExpSmooth_tick(&p->ppSmooth);

    //Lfloat pickupPos=tExpSmooth_tick(&p->puSmooth);

    //need to determine which delay line to put it into (should be half amplitude into forward and backward lines for the correct portion of string)
    Lfloat prepP=tExpSmooth_tick(&p->prpSmooth);
    Lfloat lowLen=prepP*wLen;
    Lfloat upLen=(1.0f-prepP)*wLen;
    uint32_t pickPInt;

    if (pickP > prepP)
    {
        Lfloat fullPickPoint =  ((pickP*wLen) - lowLen);
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point
        Lfloat pickPLfloat = fullPickPoint - pickPInt;

        tHermiteDelay_addTo(&p->delUF, input * (1.0f - pickPLfloat), pickPInt);
        tHermiteDelay_addTo(&p->delUF, input * pickPLfloat, pickPInt + 1);
        tHermiteDelay_addTo(&p->delUB, input * (1.0f - pickPLfloat), (uint32_t) (upLen - pickPInt));
        tHermiteDelay_addTo(&p->delUB, input * pickPLfloat, (uint32_t) (upLen - pickPInt - 1));
    }
    else
    {
         Lfloat fullPickPoint =  pickP * wLen;
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point
        Lfloat pickPLfloat = fullPickPoint - pickPInt;

        tHermiteDelay_addTo(&p->delLF, input * (1.0f - pickPLfloat), pickPInt);
        tHermiteDelay_addTo(&p->delLF, input * pickPLfloat, pickPInt + 1);
        tHermiteDelay_addTo(&p->delLB, input * (1.0f - pickPLfloat), (uint32_t) (lowLen - pickPInt));
        tHermiteDelay_addTo(&p->delLB, input * pickPLfloat, (uint32_t) (lowLen - pickPInt - 1));
    }
/*
    if (pickP > prepP)
    {
        Lfloat fullPickPoint =  ((pickP*wLen) - lowLen);
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

        tHermiteDelay_addTo(&p->delUF, input, pickPInt);
        tHermiteDelay_addTo(&p->delUB, input, (uint32_t) (upLen - pickPInt));
    }
    else
    {
        Lfloat fullPickPoint =  pickP * wLen;
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

        tHermiteDelay_addTo(&p->delLF, input, pickPInt);
        tHermiteDelay_addTo(&p->delLB, input, (uint32_t) (lowLen - pickPInt));
    }
*/
    Lfloat fromLF=tHermiteDelay_tickOut(&p->delLF);
    Lfloat fromUF=tHermiteDelay_tickOut(&p->delUF);
    Lfloat fromUB=tHermiteDelay_tickOut(&p->delUB);
    Lfloat fromLB=tHermiteDelay_tickOut(&p->delLB);
    fromLB = LEAF_clip(-1.0f, fromLB, 1.0f);
    fromUB = LEAF_clip(-1.0f, fromUB, 1.0f);
    fromUF = LEAF_clip(-1.0f, fromUF, 1.0f);
    fromLF = LEAF_clip(-1.0f, fromLF, 1.0f);

    // into upper half of string, from bridge, going backwards
    Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerU, tTwoZero_tick(&p->bridgeFilter, fromUF)));
    fromBridge = LEAF_clip(-1.0f, fromBridge, 1.0f);
    tHermiteDelay_tickIn(&p->delUB, fromBridge);
    // into lower half of string, from prepPoint, going backwards
    Lfloat fromLowerPrep=-tTwoZero_tick(&p->prepFilterL, fromLF);
    fromLowerPrep = LEAF_clip(-1.0f, fromLowerPrep, 1.0f);
    Lfloat intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB; //used to add input here
    intoLower = LEAF_clip(-1.0f, intoLower, 1.0f);
    tHermiteDelay_tickIn(&p->delLB, intoLower);
    // into lower half of string, from nut
    Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tTwoZero_tick(&p->nutFilter, fromLB)));
    fromNut = LEAF_clip(-1.0f, fromNut, 1.0f);
    tHermiteDelay_tickIn(&p->delLF, fromNut);
    // into upper half of string, from prepPoint, going forwards/upwards
    Lfloat fromUpperPrep=-tTwoZero_tick(&p->prepFilterU, fromUB);
    fromUpperPrep = LEAF_clip(-1.0f, fromUpperPrep, 1.0f);
    Lfloat intoUpper=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF;
    //intoUpper = tanhf(intoUpper);
    intoUpper = LEAF_clip(-1.0f, intoUpper, 1.0f);
    tHermiteDelay_tickIn(&p->delUF, intoUpper);
    // update all delay lengths

    tHermiteDelay_setDelay(&p->delLF, lowLen);
    tHermiteDelay_setDelay(&p->delLB, lowLen);
    tHermiteDelay_setDelay(&p->delUF, upLen);
    tHermiteDelay_setDelay(&p->delUB, upLen);
    
    uint32_t PUPInt;
    Lfloat pickupOut = 0.0f;
    Lfloat pupos = tExpSmooth_tick(&p->puSmooth);
    if (pupos < 0.9999f)
    {
        if (pupos > prepP)
        {
            Lfloat fullPUPoint =  ((pupos*wLen) - lowLen);
            PUPInt = (uint32_t) fullPUPoint; // where does the input go? that's the pick point
            Lfloat PUPLfloat = fullPUPoint - PUPInt;

            pickupOut = tHermiteDelay_tapOut(&p->delUF, PUPInt) * (1.0f - PUPLfloat);
            pickupOut += tHermiteDelay_tapOut(&p->delUF, PUPInt + 1) * PUPLfloat;
            pickupOut += tHermiteDelay_tapOut(&p->delUB, (uint32_t) (upLen - PUPInt)) * (1.0f - PUPLfloat);
            pickupOut += tHermiteDelay_tapOut(&p->delUB, (uint32_t) (upLen - PUPInt - 1))  * PUPLfloat;
        }
        else
        {
             Lfloat fullPUPoint =  pupos * wLen;
            PUPInt = (uint32_t) fullPUPoint; // where does the input go? that's the pick point
            Lfloat PUPLfloat = fullPUPoint - PUPInt;

            pickupOut = tHermiteDelay_tapOut(&p->delLF, PUPInt) * (1.0f - PUPLfloat);
            pickupOut += tHermiteDelay_tapOut(&p->delLF,  PUPInt + 1) * PUPLfloat;
            pickupOut += tHermiteDelay_tapOut(&p->delLB, (uint32_t) (lowLen - PUPInt)) * (1.0f - PUPLfloat);
            pickupOut += tHermiteDelay_tapOut(&p->delLB, (uint32_t) (lowLen - PUPInt - 1)) * PUPLfloat;
        }

        p->curr = pickupOut;




/*
    	Lfloat fullPickupPos = (pupos*upLen);
        pickupPosInt = (uint32_t) fullPickupPos;
        Lfloat pickupPosLfloat = fullPickupPos - pickupPosInt;
        if (pickupPosInt == 0)
        {
            pickupPosInt = 1;
        }
        pickupOut = tHermiteDelay_tapOutInterpolated(&p->delUF, pickupPosInt, pickupPosLfloat);
        pickupOut += tHermiteDelay_tapOutInterpolated(&p->delUB, (uint32_t) (upLen - pickupPosInt), pickupPosLfloat);
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

Lfloat   tLivingString2_tickEfficient(tLivingString2* const pl, Lfloat input)
{
    _tLivingString2* p = *pl;

    input = input * 0.5f; // drop gain by half since we'll be equally adding it at half amplitude to forward and backward waveguides
    // from prepPos upwards=forwards
    //Lfloat pickupPos=tExpSmooth_tick(&p->puSmooth);
    Lfloat wLen = p->wlSmooth->dest;

    Lfloat pickP = p->ppSmooth->dest;

    //need to determine which delay line to put it into (should be half amplitude into forward and backward lines for the correct portion of string)
    Lfloat prepP = p->prpSmooth->dest;
    Lfloat lowLen=p->prpSmooth->dest*p->wlSmooth->dest;
    Lfloat upLen=(1.0f-p->prpSmooth->dest)*p->wlSmooth->dest;
    uint32_t pickPInt;
    if (pickP > prepP)
    {
        Lfloat fullPickPoint =  ((pickP*wLen) - lowLen);
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

        tHermiteDelay_addTo(&p->delUF, input, pickPInt);
        tHermiteDelay_addTo(&p->delUB, input, (uint32_t) (upLen - pickPInt));
    }
    else
    {
        Lfloat fullPickPoint =  pickP * wLen;
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

        tHermiteDelay_addTo(&p->delLF, input, pickPInt);
        tHermiteDelay_addTo(&p->delLB, input, (uint32_t) (lowLen - pickPInt));
    }
    Lfloat fromLF=tHermiteDelay_tickOut(&p->delLF);
    Lfloat fromUF=tHermiteDelay_tickOut(&p->delUF);
    Lfloat fromUB=tHermiteDelay_tickOut(&p->delUB);
    Lfloat fromLB=tHermiteDelay_tickOut(&p->delLB);
    // into upper half of string, from bridge, going backwards
    //Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerU, tTwoZero_tick(&p->bridgeFilter, fromUF)));
    Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLevU,tHighpass_tick(&p->DCblockerU, tTwoZero_tick(&p->bridgeFilter, fromUF)));
    tHermiteDelay_tickIn(&p->delUB, fromBridge);
    // into lower half of string, from prepPoint, going backwards
    Lfloat fromLowerPrep=-tTwoZero_tick(&p->prepFilterL, fromLF);
    Lfloat intoLower=(p->prepIndex*fromLowerPrep)+((1.0f - p->prepIndex)*fromUB); //used to add input here
    tHermiteDelay_tickIn(&p->delLB, intoLower);
    // into lower half of string, from nut
    //Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tTwoZero_tick(&p->nutFilter, fromLB)));
    Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLevL,tHighpass_tick(&p->DCblockerL, tTwoZero_tick(&p->nutFilter, fromLB)));
    tHermiteDelay_tickIn(&p->delLF, fromNut);
    // into upper half of string, from prepPoint, going forwards/upwards
    Lfloat fromUpperPrep=-tTwoZero_tick(&p->prepFilterU, fromUB);
    Lfloat intoUpper=(p->prepIndex*fromUpperPrep)+((1.0f - p->prepIndex)*fromLF);
    tHermiteDelay_tickIn(&p->delUF, intoUpper);
    // update all delay lengths

    p->curr = fromBridge;

    //p->curr = fromBridge;
    //p->curr += fromNut;

    return p->curr;
}


void tLivingString2_updateDelays(tLivingString2* const pl)
{
    _tLivingString2* p = *pl;

    //need to determine which delay line to put it into (should be half amplitude into forward and backward lines for the correct portion of string)

    Lfloat lowLen=p->prpSmooth->dest*p->wlSmooth->dest;
    Lfloat upLen=(1.0f-p->prpSmooth->dest)*p->wlSmooth->dest;

	tHermiteDelay_setDelay(&p->delLF, lowLen);
    tHermiteDelay_setDelay(&p->delLB, lowLen);
    tHermiteDelay_setDelay(&p->delUF, upLen);
    tHermiteDelay_setDelay(&p->delUB, upLen);
}

Lfloat   tLivingString2_sample(tLivingString2* const pl)
{
    _tLivingString2* p = *pl;
    return p->curr;
}

void    tLivingString2_setSampleRate(tLivingString2* const pl, Lfloat sr)
{
    _tLivingString2* p = *pl;
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


//////////---------------------------

/* Complex Living String (has pick position and preparation position separated) */

void    tComplexLivingString_init(tComplexLivingString* const pl, Lfloat freq, Lfloat pickPos, Lfloat prepPos, Lfloat prepIndex,
                           Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                           Lfloat levStrength, int levMode, LEAF* const leaf)
{
    tComplexLivingString_initToPool(pl, freq, pickPos, prepPos, prepIndex, dampFreq, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tComplexLivingString_initToPool    (tComplexLivingString* const pl, Lfloat freq, Lfloat pickPos, Lfloat prepPos, Lfloat prepIndex,
                                     Lfloat dampFreq, Lfloat decay, Lfloat targetLev, Lfloat levSmoothFactor,
                                     Lfloat levStrength, int levMode, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tComplexLivingString* p = *pl = (_tComplexLivingString*) mpool_alloc(sizeof(_tComplexLivingString), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;

    p->sampleRate = leaf->sampleRate;
    p->curr=0.0f;
    tExpSmooth_initToPool(&p->wlSmooth, p->sampleRate/freq, 0.01f, mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    tComplexLivingString_setFreq(pl, freq);
    p->freq = freq;
    tExpSmooth_initToPool(&p->pickPosSmooth, pickPos, 0.01f, mp); // smoother for pick position
    tExpSmooth_initToPool(&p->prepPosSmooth, prepPos, 0.01f, mp); // smoother for pick position

    tComplexLivingString_setPickPos(pl, pickPos);
    tComplexLivingString_setPrepPos(pl, prepPos);

    p->prepPos=prepPos;
    p->pickPos=pickPos;
    tLinearDelay_initToPool(&p->delLF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delMF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delMB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delLB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_clear(&p->delLF);
    tLinearDelay_clear(&p->delMF);
    tLinearDelay_clear(&p->delUF);
    tLinearDelay_clear(&p->delUB);
    tLinearDelay_clear(&p->delMB);
    tLinearDelay_clear(&p->delLB);
    p->dampFreq = dampFreq;
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tOnePole_initToPool(&p->nutFilter, dampFreq, mp);
    tOnePole_initToPool(&p->prepFilterU, dampFreq, mp);
    tOnePole_initToPool(&p->prepFilterL, dampFreq, mp);
    tHighpass_initToPool(&p->DCblockerU, 13, mp);
    tHighpass_initToPool(&p->DCblockerL, 13, mp);
    p->decay=decay;
    p->prepIndex = prepIndex;
    tFeedbackLeveler_initToPool(&p->fbLevU, targetLev, levSmoothFactor, levStrength, levMode, mp);
    tFeedbackLeveler_initToPool(&p->fbLevL, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
}

void    tComplexLivingString_free (tComplexLivingString* const pl)
{
    _tComplexLivingString* p = *pl;

    tExpSmooth_free(&p->wlSmooth);
    tExpSmooth_free(&p->pickPosSmooth);
    tExpSmooth_free(&p->prepPosSmooth);
    tLinearDelay_free(&p->delLF);
    tLinearDelay_free(&p->delMF);
    tLinearDelay_free(&p->delUF);
    tLinearDelay_free(&p->delUB);
    tLinearDelay_free(&p->delMB);
    tLinearDelay_free(&p->delLB);
    tOnePole_free(&p->bridgeFilter);
    tOnePole_free(&p->nutFilter);
    tOnePole_free(&p->prepFilterU);
    tOnePole_free(&p->prepFilterL);
    tHighpass_free(&p->DCblockerU);
    tHighpass_free(&p->DCblockerL);
    tFeedbackLeveler_free(&p->fbLevU);
    tFeedbackLeveler_free(&p->fbLevL);

    mpool_free((char*)p, p->mempool);
}

void     tComplexLivingString_setFreq(tComplexLivingString* const pl, Lfloat freq)
{    // NOTE: It is faster to set wavelength in samples directly
    _tComplexLivingString* p = *pl;
    
    if (freq<20.0f) freq=20.0f;
    else if (freq>10000.0f) freq=10000.0f;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tComplexLivingString_setWaveLength(tComplexLivingString* const pl, Lfloat waveLength)
{
    _tComplexLivingString* p = *pl;
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.0f) waveLength=2400.0f;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tComplexLivingString_setPickPos(tComplexLivingString* const pl, Lfloat pickPos)
{    // between 0 and 1
    _tComplexLivingString* p = *pl;
    if (pickPos<0.5f) pickPos=0.5f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(&p->pickPosSmooth, p->pickPos);
}

void     tComplexLivingString_setPrepPos(tComplexLivingString* const pl, Lfloat prepPos)
{    // between 0 and 1
    _tComplexLivingString* p = *pl;
    if (prepPos<0.f) prepPos=0.f;
    else if (prepPos>0.5f) prepPos=0.5f;
    p->prepPos = prepPos;
    tExpSmooth_setDest(&p->prepPosSmooth, p->prepPos);
}

void     tComplexLivingString_setPrepIndex(tComplexLivingString* const pl, Lfloat prepIndex)
{    // between 0 and 1
    _tComplexLivingString* p = *pl;
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tComplexLivingString_setDampFreq(tComplexLivingString* const pl, Lfloat dampFreq)
{
    _tComplexLivingString* p = *pl;
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    tOnePole_setFreq(&p->nutFilter, dampFreq);
    tOnePole_setFreq(&p->prepFilterU, dampFreq);
    tOnePole_setFreq(&p->prepFilterL, dampFreq);
}

void     tComplexLivingString_setDecay(tComplexLivingString* const pl, Lfloat decay)
{
    _tComplexLivingString* p = *pl;
    p->decay=decay;
}

void     tComplexLivingString_setTargetLev(tComplexLivingString* const pl, Lfloat targetLev)
{
    _tComplexLivingString* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLevL, targetLev);
}

void     tComplexLivingString_setLevSmoothFactor(tComplexLivingString* const pl, Lfloat levSmoothFactor)
{
    _tComplexLivingString* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLevL, levSmoothFactor);
}

void     tComplexLivingString_setLevStrength(tComplexLivingString* const pl, Lfloat levStrength)
{
    _tComplexLivingString* p = *pl;
    tFeedbackLeveler_setStrength(&p->fbLevU, levStrength);
    tFeedbackLeveler_setStrength(&p->fbLevL, levStrength);
}

void     tComplexLivingString_setLevMode(tComplexLivingString* const pl, int levMode)
{
    _tComplexLivingString* p = *pl;
    tFeedbackLeveler_setMode(&p->fbLevU, levMode);
    tFeedbackLeveler_setMode(&p->fbLevL, levMode);
    p->levMode=levMode;
}

Lfloat   tComplexLivingString_tick(tComplexLivingString* const pl, Lfloat input)
{
    _tComplexLivingString* p = *pl;

    // from pickPos upwards=forwards
    Lfloat fromLF=tLinearDelay_tickOut(&p->delLF);
    Lfloat fromMF=tLinearDelay_tickOut(&p->delMF);
    Lfloat fromUF=tLinearDelay_tickOut(&p->delUF);
    Lfloat fromUB=tLinearDelay_tickOut(&p->delUB);
    Lfloat fromMB=tLinearDelay_tickOut(&p->delMB);
    Lfloat fromLB=tLinearDelay_tickOut(&p->delLB);

    // into upper part of string, from bridge, going backwards
    Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(&p->DCblockerU, tOnePole_tick(&p->bridgeFilter, fromUF)));
    tLinearDelay_tickIn(&p->delUB, fromBridge);

    // into pick position, take input and add it into the waveguide, going to come out of middle segment
    tLinearDelay_tickIn(&p->delMB, fromUB+input);

    // into lower part of string, from prepPos, going backwards
    Lfloat fromLowerPrep=-tOnePole_tick(&p->prepFilterL, fromLF);
    Lfloat intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromMB;
    tLinearDelay_tickIn(&p->delLB, intoLower);

    // into lower part of string, from nut, going forwards toward prep position
    Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tOnePole_tick(&p->nutFilter, fromLB)));
    tLinearDelay_tickIn(&p->delLF, fromNut);

    // into middle part of string, from prep going toward pick position
    Lfloat fromUpperPrep=-tOnePole_tick(&p->prepFilterU, fromMB);
    Lfloat intoMiddle=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF;

    //pick position, going to come out of middle segment
    tLinearDelay_tickIn(&p->delMF, intoMiddle);

    //take output of middle segment and put it into upper segment connecting to the bridge, take input and add it into the waveguide, 
    tLinearDelay_tickIn(&p->delUF, fromMF + input);

    // update all delay lengths
    Lfloat pickP=tExpSmooth_tick(&p->pickPosSmooth);
    Lfloat prepP=tExpSmooth_tick(&p->prepPosSmooth);
    Lfloat wLen=tExpSmooth_tick(&p->wlSmooth);

    Lfloat midLen = (pickP-prepP) * wLen; // the length between the pick and the prep;
    Lfloat lowLen = prepP*wLen; // the length from prep to nut
    Lfloat upLen = (1.0f-pickP)*wLen; // the length from pick to bridge


    tLinearDelay_setDelay(&p->delLF, lowLen);
    tLinearDelay_setDelay(&p->delLB, lowLen);

    tLinearDelay_setDelay(&p->delMF, midLen);
    tLinearDelay_setDelay(&p->delMB, midLen);

    tLinearDelay_setDelay(&p->delUF, upLen);
    tLinearDelay_setDelay(&p->delUB, upLen);

    //update this to allow pickup position variation
    p->curr = fromBridge;
    return p->curr;
}

Lfloat   tComplexLivingString_sample(tComplexLivingString* const pl)
{
    _tComplexLivingString* p = *pl;
    return p->curr;
}

void    tComplexLivingString_setSampleRate(tComplexLivingString* const pl, Lfloat sr)
{
    _tComplexLivingString* p = *pl;
    Lfloat freq = p->waveLengthInSamples/p->sampleRate;
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

///Reed Table model
//default values from STK are 0.6 offset and -0.8 slope

void    tReedTable_init      (tReedTable* const pm, Lfloat offset, Lfloat slope, LEAF* const leaf)
{
    tReedTable_initToPool(pm, offset, slope, &leaf->mempool);
}

void    tReedTable_initToPool   (tReedTable* const pm, Lfloat offset, Lfloat slope, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tReedTable* p = *pm = (_tReedTable*) mpool_alloc(sizeof(_tReedTable), m);
    p->mempool = m;
    
    p->offset = offset;
    p->slope = slope;
}

void    tReedTable_free (tReedTable* const pm)
{
    _tReedTable* p = *pm;
    
    mpool_free((char*)p, p->mempool);
}

Lfloat   tReedTable_tick      (tReedTable* const pm, Lfloat input)
{
    _tReedTable* p = *pm;
    
    // The input is differential pressure across the reed.
    Lfloat output = p->offset + (p->slope * input);
    
    // If output is > 1, the reed has slammed shut and the
    // reflection function value saturates at 1.0.
    if ( output > 1.0f) output = 1.0f;
    
    // This is nearly impossible in a physical system, but
    // a reflection function value of -1.0 corresponds to
    // an open end (and no discontinuity in bore profile).
    if ( output < -1.0f) output = -1.0f;
    
    return output;
}

Lfloat   tReedTable_tanh_tick     (tReedTable* const pm, Lfloat input)
{
    _tReedTable* p = *pm;
    
    // The input is differential pressure across the reed.
    Lfloat output = p->offset + (p->slope * input);
    
    // If output is > 1, the reed has slammed shut and the
    // reflection function value saturates at 1.0.
    return tanhf(output);
}

void     tReedTable_setOffset   (tReedTable* const pm, Lfloat offset)
{
    _tReedTable* p = *pm;
    p->offset = offset;
}

void     tReedTable_setSlope   (tReedTable* const pm, Lfloat slope)
{
    _tReedTable* p = *pm;
    p->slope = slope;
}

/* ============================ */

void    tStiffString_init      (tStiffString* const pm, int numModes, LEAF* const leaf)
{
    tStiffString_initToPool(pm, numModes, &leaf->mempool);
}

void tStiffString_updateOutputWeights(tStiffString const p);

void    tStiffString_initToPool   (tStiffString* const pm, int numModes, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tStiffString* p = *pm = (_tStiffString*) mpool_alloc(sizeof(_tStiffString), m);
    p->mempool = m;

    // initialize variables
    p->numModes = numModes;
    p->freqHz = 440.0f;
    p->stiffness = 0.001f;
    p->pluckPos = 0.2f;
    p->pickupPos = 0.3f;
    p->decay = 0.0001f;
    p->decayHighFreq = 0.0003f;
    p->sampleRate = m->leaf->sampleRate;
    p->twoPiTimesInvSampleRate = m->leaf->twoPiTimesInvSampleRate;

    // allocate memory
    p->osc = (tCycle *) mpool_alloc(numModes * sizeof(tCycle), m);
    for (int i; i < numModes; ++i) {
        tCycle_initToPool(&p->osc[i], &m);
    }
    p->amplitudes = (Lfloat *) mpool_alloc(numModes * sizeof(Lfloat), m);
    p->outputWeights = (Lfloat *) mpool_alloc(numModes * sizeof(Lfloat), m);
    tStiffString_updateOutputWeights(p);
}

void tStiffString_updateOutputWeights(tStiffString const p)
{
    Lfloat x0 = p->pickupPos * 0.5f * PI;
    for (int i = 0; i < p->numModes; ++i) {
        p->outputWeights[i] = sinf((i + 1) * x0);
    }
}

void    tStiffString_free (tStiffString* const pm)
{
    _tStiffString* p = *pm;

    for (int i; i < p->numModes; ++i) {
        tCycle_free(&p->osc[i]);
    }
    mpool_free((char *) p->osc, p->mempool);
    mpool_free((char *) p->amplitudes, p->mempool);
    mpool_free((char *) p->outputWeights, p->mempool);
    mpool_free((char *) p, p->mempool);
}

Lfloat   tStiffString_tick                  (tStiffString* const pm)
{
    _tStiffString *p = *pm;
    Lfloat sample = 0.0f;
    for (int i = 0; i < p->numModes; ++i) {
        sample += tCycle_tick(&p->osc[i]) * p->amplitudes[i] * p->outputWeights[i];
        int n = i + 1;
        Lfloat sig = p->decay + p->decayHighFreq * (n * n);
        //amplitudes[i] *= expf(-sig * freqHz * leaf->twoPiTimesInvSampleRate);
        sig = LEAF_clip(0.f, sig, 1.f);
        p->amplitudes[i] *= 1.0f -sig * p->freqHz * p->twoPiTimesInvSampleRate;
    }
    return sample;
}

void tStiffString_setStiffness(tStiffString* const pm, Lfloat newValue)
{
    tStiffString p = *pm;
    p->stiffness = newValue;
}

void tStiffString_setFreq(tStiffString* const pm, Lfloat newFreq)
{
    _tStiffString *p = *pm;
    p->freqHz = newFreq;
    Lfloat kappa_sq = p->stiffness * p->stiffness;
    for (int i = 0; i < p->numModes; ++i) {
        int n = i + 1;
        int n_sq = n * n;
        Lfloat sig = p->decay + p->decayHighFreq * n_sq;
        Lfloat w0 = n * sqrtf(1.0f + kappa_sq * n_sq);
        Lfloat zeta = sig / w0;
        Lfloat w = w0 * sqrtf(1.0f - zeta * zeta);
        tCycle_setFreq(&p->osc[i], p->freqHz * w);
    }
}

void tStiffString_setFreqFast(tStiffString* const pm, Lfloat newFreq)
{
    _tStiffString *p = *pm;
    p->freqHz = newFreq;
    Lfloat kappa_sq = p->stiffness * p->stiffness;
    for (int i = 0; i < p->numModes; ++i) {
        int n = i + 1;
        int n_sq = n * n;
        Lfloat sig = p->decay + p->decayHighFreq * n_sq;
        Lfloat w0 = n * (1.0f + 0.5f * kappa_sq * n_sq);
        Lfloat zeta = sig / w0;
        Lfloat w = w0 * (1.0f - 0.5f * zeta * zeta);
        tCycle_setFreq(&p->osc[i], p->freqHz * w);
    }
}

void tStiffString_setInitialAmplitudes(tStiffString* const mp)
{
    _tStiffString* p = *mp;
    Lfloat x0 = p->pluckPos * 0.5f * PI;
    for (int i = 0; i < p->numModes; ++i) {
        int n = i + 1;
        float denom = n * n * x0 * (PI - x0);
        p->amplitudes[i] = 2.0f * sinf(x0 * n) / denom;
    }
}
