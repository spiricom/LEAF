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
    tFeedbackLeveler_free(&p->fbLev);
    tHighpass_free(&p->DCblocker);
    tLinearDelay_free(&p->delayLineL);
    tLinearDelay_free(&p->delayLineU);
    tOnePole_free(&p->bridgeFilter);
    tExpSmooth_free(&p->wlSmooth);

    
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
    tExpSmooth_initToPool(&p->prepPosSmooth, prepPos, 0.001f , mp);
    tExpSmooth_initToPool(&p->prepIndexSmooth, prepIndex, 0.001f , mp);

    tExpSmooth_initToPool(&p->wlSmooth, p->sampleRate/freq/2.0f, 0.05f , mp); // smoother for string wavelength (not freq, to avoid expensive divisions)
    
    tExpSmooth_initToPool(&p->pluckPosSmooth, pluckPos, 0.001f , mp);
    tExpSmooth_initToPool(&p->pickupPointSmooth, pluckPos, 0.001f , mp);
    
    tLagrangeDelay_initToPool(&p->delUF,p->waveLengthInSamples, p->maxLength, mp);
    tLagrangeDelay_initToPool(&p->delUB,p->waveLengthInSamples, p->maxLength, mp);
    tLagrangeDelay_initToPool(&p->delLF,p->waveLengthInSamples, p->maxLength, mp);
    tLagrangeDelay_initToPool(&p->delLB,p->waveLengthInSamples, p->maxLength, mp);
    tSimpleLivingString5_setFreq(pl, freq);
    tLagrangeDelay_setDelay(&p->delUF, p->waveLengthInSamples-(p->prepPos*p->waveLengthInSamples));
    tLagrangeDelay_setDelay(&p->delUB, p->waveLengthInSamples-((1.0f-p->prepPos)*p->waveLengthInSamples));
    tLagrangeDelay_setDelay(&p->delLF, p->waveLengthInSamples-(p->prepPos*p->waveLengthInSamples));
    tLagrangeDelay_setDelay(&p->delLB, p->waveLengthInSamples-((1.0f-p->prepPos)*p->waveLengthInSamples));
    //tSimpleLivingString5_setWaveLength(pl, 4800);
    tLagrangeDelay_clear(&p->delUF);
    tLagrangeDelay_clear(&p->delUB);
    tLagrangeDelay_clear(&p->delLF);
    tLagrangeDelay_clear(&p->delLB);
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

    p->ff = 0.3f;
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
    tExpSmooth_free(&p->pluckPosSmooth);
    tExpSmooth_free(&p->pickupPointSmooth);
    tLagrangeDelay_free(&p->delUF);
    tLagrangeDelay_free(&p->delUB);
    tLagrangeDelay_free(&p->delLF);
    tLagrangeDelay_free(&p->delLB);
    tOnePole_free(&p->bridgeFilter);
    tOnePole_free(&p->nutFilter);


    tHighpass_free(&p->DCblocker);
    tHighpass_free(&p->DCblocker2);
    tFeedbackLeveler_free(&p->fbLev);
    tFeedbackLeveler_free(&p->fbLev2);
    


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
    tExpSmooth_setDest(&p->prepPosSmooth, prepPosition);
}
void   tSimpleLivingString5_setPrepIndex(tSimpleLivingString5* const pl, Lfloat prepIndex)
{
     _tSimpleLivingString5* p = *pl;
     p->prepIndex = prepIndex;
    tExpSmooth_setDest(&p->prepIndexSmooth, prepIndex);
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
    tExpSmooth_setDest(&p->pluckPosSmooth, position);
    
}


void   tSimpleLivingString5_pluck(tSimpleLivingString5* const pl, Lfloat input, Lfloat position)
{
    _tSimpleLivingString5* p = *pl;

    //p->pluckPosition = position;
    p->pluckPosition = 0.5f;
    tExpSmooth_setDest(&p->pluckPosSmooth, position);
    volatile Lfloat pluckPoint = position*p->waveLengthInSamples;
    pluckPoint = LEAF_clip(1.0f, pluckPoint, p->waveLengthInSamples-1.0f);
    uint32_t pluckPointInt = (uint32_t) pluckPoint;
    volatile Lfloat remainder = p->waveLengthInSamples-pluckPoint;
    Lfloat BLen = p->delUB->delay;
    Lfloat FLen = p->delUF->delay;
    uint32_t FLenInt = (uint32_t)FLen;
    //float FLenAlpha = FLen -FLenInt;
    uint32_t BLenInt = (uint32_t)BLen;
    Lfloat BLenAlpha = BLen -BLenInt;
    
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
            tLagrangeDelay_addTo(&p->delUB, val, i);
            tLagrangeDelay_addTo(&p->delLB, val, BLenInt-i);
        }
        else if (i == BLenInt)
        {
            tLagrangeDelay_addTo(&p->delUB, val * (1.0f-BLenAlpha), i);
            tLagrangeDelay_addTo(&p->delLB, val * (1.0f-BLenAlpha), BLenInt-i);
            tLagrangeDelay_addTo(&p->delUF, val * BLenAlpha, i-BLenInt);
            tLagrangeDelay_addTo(&p->delLF, val * BLenAlpha, (FLenInt-1)-(i-BLenInt));
        }
        else
        {
            tLagrangeDelay_addTo(&p->delUF, val, i-BLenInt);
            tLagrangeDelay_addTo(&p->delLF, val, (FLenInt-1)-(i-BLenInt));
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
    Lfloat prepPosSmoothed = tExpSmooth_tick(&p->prepPosSmooth);
    Lfloat FLen = wl*(1.0f-prepPosSmoothed);
    uint32_t FLenInt = (uint32_t)FLen;
    Lfloat BLen = wl*prepPosSmoothed;
    uint32_t BLenInt = (uint32_t)BLen;
    tLagrangeDelay_setDelay(&p->delUF, FLen);
    tLagrangeDelay_setDelay(&p->delUB, BLen);
    tLagrangeDelay_setDelay(&p->delLF, FLen);
    tLagrangeDelay_setDelay(&p->delLB, BLen);
    Lfloat pluckPosSmoothed = tExpSmooth_tick(&p->pluckPosSmooth);
    Lfloat pluckPosInSamples = pluckPosSmoothed * wl;
    uint32_t pluckPosInSamplesInt = (uint32_t) pluckPosInSamples;
    Lfloat alpha = pluckPosInSamples - pluckPosInSamplesInt;

    uint32_t sample1 = pluckPosInSamplesInt;
    uint32_t sample2 = pluckPosInSamplesInt + 1;

    uint32_t sample1Front = (uint32_t) (sample1 - BLen);
    uint32_t sample2Front = (uint32_t) (sample2 - BLen);

        

         
    //now tick out the output data and filter (oversampled)
    for (int i = 0; i < p->oversampling; i++)
    {
        if (sample1 < BLenInt)
        {
            tLagrangeDelay_addTo(&p->delUB, input * (1.0f - alpha), sample1);
            tLagrangeDelay_addTo(&p->delLB, input * (1.0f - alpha), BLenInt-sample1);
        }
        else
        {
            tLagrangeDelay_addTo(&p->delUF, input * (1.0f - alpha), sample1Front);
            tLagrangeDelay_addTo(&p->delLF, input * (1.0f - alpha), FLenInt-sample1Front);
        }
        if (sample2 < BLenInt)
        {
            tLagrangeDelay_addTo(&p->delUB, input * alpha, sample2);
            tLagrangeDelay_addTo(&p->delLB, input * alpha, BLenInt-sample2);
        }
        else
        {
            tLagrangeDelay_addTo(&p->delUF, input * alpha, sample2Front);
            tLagrangeDelay_addTo(&p->delLF, input * alpha, FLenInt-sample2Front);
        }
        
        
        Lfloat fromUF=tLagrangeDelay_tickOut(&p->delUF);
        Lfloat fromLF=tLagrangeDelay_tickOut(&p->delLF);
        Lfloat fromLB=tLagrangeDelay_tickOut(&p->delLB);
        Lfloat fromUB=tLagrangeDelay_tickOut(&p->delUB);
        //fromUF = tanhf(p->fbSample1) * p->fb + fromUF*0.1f;
        //fromLB = tanhf(p->fbSample2) * p->fb + fromLB*0.1f;
        //p->fbSample1 = fromUF;
        //p->fbSample2 = fromLB;
        //fromUF = fromUF * 1.0f / (1.0f+p->fb);
        //fromLB = fromLB * 1.0f / (1.0f+p->fb);
        //fromUF = tWavefolder_tick(&p->wf1, fromUF);
        //fromLB = tWavefolder_tick(&p->wf2, fromLB);
        //softclip approx for tanh saturation in original code
        //float fromUFSat = tanhf(fromUF + input);
        float fromUFSat = tanhf(fromUF);
        float fromLBSat = tanhf(fromLB);
        
        fromUF = (p->ff * fromUFSat) + ((1.0f - p->ff) * fromUF);
        fromLB = (p->ff * fromLBSat) + ((1.0f - p->ff) * fromLB);
        
#if 0
        Lfloat fromBridge = -tOnePole_tick(&p->bridgeFilter, fromUF);
        tLagrangeDelay_tickIn(&p->delLF, fromBridge + input);
        tLagrangeDelay_tickIn(&p->delLB, fromLF);
        Lfloat fromNut=-fromLB;
        tLagrangeDelay_tickIn(&p->delUB, fromNut);
        tLagrangeDelay_tickIn(&p->delUF, fromUB);
#endif

        // into front half of string, from bridge, going backwards (lower section)
        //Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblocker, tOnePole_tick(&p->bridgeFilter, fromUF)));
        //Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay:1.0f)*fromUF);
        Lfloat fromBridge=-p->decay*fromUF;//tOnePole_tick(&p->bridgeFilter, fromUF);
        //Lfloat fromBridge=-tHighpass_tick(&p->DCblocker, tOnePole_tick(&p->bridgeFilter, fromUF)) * p->decay; //version with feedbackleveler
        //fromBridge = LEAF_clip(-1.0f, fromBridge, 1.0f);
        tLagrangeDelay_tickIn(&p->delLF, fromBridge);
        // into back half of string, from prepPoint, going backwards (lower section)
        //Lfloat fromUpperPrep=-tOnePole_tick(&p->prepFilterL, fromUB);
        Lfloat fromUpperPrep=-fromUB;
        //fromUpperPrep = LEAF_clip(-1.0f, fromUpperPrep, 1.0f);
        Lfloat intoLower=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF; //used to add input here
        //intoLower = LEAF_clip(-1.0f, intoLower, 1.0f);
        tLagrangeDelay_tickIn(&p->delLB, intoLower);
        // into back of string, from nut going forward (upper section)
        //Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLev2, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblocker2,fromLB));
        //Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLev2, (p->levMode==0?p->decay:1.0f)*fromLB);
        Lfloat fromNut=-p->decay*fromLB;//tOnePole_tick(&p->nutFilter, fromLB);
        //Lfloat fromNut=-p->decay*tHighpass_tick(&p->DCblocker2,fromLB)); //version without feedback leveler
        //fromNut = LEAF_clip(-1.0f, fromNut, 1.0f);
        tLagrangeDelay_tickIn(&p->delUB, fromNut);
        // into front half of string, from prepPoint, going forward (upper section)
        //Lfloat fromLowerPrep=-tOnePole_tick(&p->prepFilterU, fromLF);
        Lfloat fromLowerPrep=-fromLF;
        //fromLowerPrep = LEAF_clip(-1.0f, fromLowerPrep, 1.0f);
        Lfloat intoUpperFront=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB;
        //intoUpper = tanhf(intoUpper);
        //intoUpperFront = LEAF_clip(-1.0f, intoUpperFront, 1.0f);
        tLagrangeDelay_tickIn(&p->delUF, intoUpperFront);

        p->curr = fromBridge;
    }

    //pick up the signal
    Lfloat pickupPointSmoothed = tExpSmooth_tick(&p->pickupPointSmooth);
    Lfloat pickupPosInSamples = pickupPointSmoothed * wl;

    
    Lfloat sample1Front2 = (pickupPosInSamples - BLen);

    uint32_t a1 = (uint32_t) pickupPosInSamples;
    Lfloat a1F =pickupPosInSamples - (Lfloat) a1;
    
    uint32_t a2 = (uint32_t) (BLen-pickupPosInSamples);
    Lfloat a2F =(BLen-pickupPosInSamples) - (Lfloat) a2;
    
    uint32_t a3 = (uint32_t) sample1Front2;
    Lfloat a3F =sample1Front2 - (Lfloat) a3;
    
    uint32_t a4 = (uint32_t) (FLen-sample1Front2);
    Lfloat a4F = (FLen-sample1Front2) - (Lfloat) a4;
    
    Lfloat outputSample1 = 0.0f;

    if (pickupPosInSamples < BLen)
    {
        outputSample1 = tLagrangeDelay_tapOutInterpolated(&p->delUB, a1, a1F);
        outputSample1 += tLagrangeDelay_tapOutInterpolated(&p->delLB, a2, a2F);
    }
    else
    {
        outputSample1 = tLagrangeDelay_tapOutInterpolated(&p->delUF, a3, a3F);
        outputSample1 += tLagrangeDelay_tapOutInterpolated(&p->delLF, a4, a4F);
    }

    p->curr = 0.5f * outputSample1;
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
    tExpSmooth_setDest(&p->pickupPointSmooth, pickupPoint);
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


void   tSimpleLivingString5_setFFAmount(tSimpleLivingString5* const pl, Lfloat ff)
{
    _tSimpleLivingString5* p = *pl;
    p->ff = ff;
    
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


void    tBowed_init                  (tBowed* const b, int oversampling, LEAF* const leaf)
{
    tBowed_initToPool(b, oversampling, &leaf->mempool);
}
void    tBowed_initToPool            (tBowed* const bw, int oversampling, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tBowed* x = *bw = (_tBowed*) mpool_alloc(sizeof(_tBowed), m);
    x->mempool = m;
    LEAF* leaf = x->mempool->leaf;
    
    x->x_bp   = 01.6f;
    x->x_bpos = 0.2f;
    x->x_bv   = .9f;
    x->x_fr   = 440.f;
    x->oversampling = oversampling;
    x->sampleRate          = leaf->sampleRate * oversampling;
    x->invSampleRate = 1.f / (x->sampleRate * oversampling);
    
    tLinearDelay_initToPool(&x->neckDelay, 100.0f, 2400.0f, mp);
    tLinearDelay_initToPool(&x->bridgeDelay, 29.0f, 2400.0f, mp);
    tLinearDelay_clear(&x->neckDelay);
    tLinearDelay_clear(&x->bridgeDelay);
    tCookOnePole_initToPool(&x->reflFilt, mp);
    tCookOnePole_setSampleRate(&x->reflFilt, x->sampleRate);
    
    tCookOnePole_setPole(&x->reflFilt, 0.6f - (0.1f * 22050.f / x->sampleRate));
    tCookOnePole_setGain(&x->reflFilt, .95f);
    tBowTable_initToPool(&x->bowTabl, mp);
    x->bowTabl->slope = 3.0f;
    tBowed_setFreq(&x, x->x_fr);
    
    tSVF_initToPool(&x->lowpass, SVFTypeLowpass, 18000.0f, 0.6f, mp);
    tSVF_setSampleRate(&x->lowpass, x->sampleRate);
    tSVF_setFreq(&x->lowpass,16000.0f);
    x->betaRatio = 0.127236;
    x->fr_save = x->x_fr;
    
}
void    tBowed_free                  (tBowed* const bw)
{
    _tBowed* x = *bw;
    tLinearDelay_free(&x->neckDelay);
    tLinearDelay_free(&x->bridgeDelay);
    tCookOnePole_free(&x->reflFilt);
    mpool_free((char*)x, x->mempool);
}

Lfloat   tBowed_tick                  (tBowed* const bw)
{
    _tBowed* x = *bw;
    float bp   =  x->x_bp;
    float bpos =  x->x_bpos;
    float bv   =  x->x_bv;
    float fr   =  x->x_fr;
    float nutRefl, newVel, velDiff, stringVel, bridgeRefl;
    if (fr != x->fr_save)
    {
        tBowed_setFreq(&x, fr);
        x->fr_save = fr;
    }
    
    x->bowTabl->slope = bp;
    
    if (bpos != x->betaRatio) {
        x->betaRatio = bpos;
        tLinearDelay_setDelay(&x->bridgeDelay, x->baseDelay * x->betaRatio);      /* bow to bridge length   */
        tLinearDelay_setDelay(&x->neckDelay, x->baseDelay * (1. - x->betaRatio)); /* bow to nut (finger) length   */
    }

    for (int i = 0; i < x->oversampling; i++)
    {
        bridgeRefl = -tCookOnePole_tick(&x->reflFilt, x->bridgeDelay->lastOut); /* Bridge Reflection      */
        nutRefl    = x->neckDelay->lastOut;                                /* Nut Reflection         */
        stringVel  = bridgeRefl + nutRefl;                                   /* Sum is String Velocity */
        velDiff    = bv - stringVel;                                         /* Differential Velocity  */
        newVel     = velDiff * tBowTable_lookup(&x->bowTabl, velDiff);         /* Non-Lin Bow Function   */
        tLinearDelay_tick(&x->neckDelay, bridgeRefl + newVel);                     /* Do string              */
        tLinearDelay_tick(&x->bridgeDelay, nutRefl + newVel);                      /*   propagations         */
        x->output = tSVF_tick(&x->lowpass, x->bridgeDelay->lastOut);
    }
    return x->output;
}

void    tBowed_setFreq               (tBowed* const bw, Lfloat freq)
{
    _tBowed* x = *bw;
    if (freq < 20.f)
        freq = 20.f;
    x->baseDelay = (x->sampleRate / freq) - 4.0;                               /* delay - approx. filter delay */
    tLinearDelay_setDelay(&x->bridgeDelay, x->baseDelay * x->betaRatio);      /* bow to bridge length */
    tLinearDelay_setDelay(&x->neckDelay, x->baseDelay * (1. - x->betaRatio)); /* bow to nut (finger) length */
}

void    tBowed_setWaveLength         (tBowed* const, Lfloat waveLength); // in samples
void    tBowed_setSampleRate         (tBowed* const, Lfloat sr);



//
void    tTString_init                  (tTString* const b, int oversampling, Lfloat lowestFreq, LEAF* const leaf)
{
    tTString_initToPool(b, oversampling, lowestFreq, &leaf->mempool);
}
void    tTString_initToPool            (tTString* const bw, int oversampling, Lfloat lowestFreq, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tTString* x = *bw = (_tTString*) mpool_alloc(sizeof(_tTString), m);
    x->mempool = m;
    LEAF* leaf = x->mempool->leaf;
    
    x->oversampling = oversampling;
    x->invOversampling = 1.0f / oversampling;
    x->invOversamplingTimesTwo = x->invOversampling * 2.0f;
    x->sampleRate          = leaf->sampleRate * oversampling;
    x->invSampleRate = 1.f / x->sampleRate;
    x->prevTension = 0.0f;
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
    x->prevDelay = 100.0f;
    x->baseDelay = 100.0f;
    x->ldev = 0.0f;
    x->tensionSum = 0.0f;
    x->tensionAmount = 0.0f;
    x->tempSum = 0.0f;
    x->prevSum = 0.0f;
    x->pickupPos = 0.0f;
    x->slideGain = 0.0f;
    x->wound = 1;
    x->barPulsePhasor = 0;
    Lfloat maxDelayTemp = x->sampleRate / lowestFreq;
    tLagrangeDelay_initToPool(&x->delay, 100.0f, maxDelayTemp, mp);
    x->actualLowestFreq = x->sampleRate / tLagrangeDelay_getMaxDelay (&x->delay); //adjusted to create a power-of-two size buffer
    tLagrangeDelay_clear(&x->delay);
    tLagrangeDelay_initToPool(&x->delayP, 100.0f, maxDelayTemp, mp);
    tLagrangeDelay_clear(&x->delayP);


    tCookOnePole_initToPool(&x->reflFilt, mp);
    tCookOnePole_setSampleRate(&x->reflFilt, x->sampleRate);
    tCookOnePole_initToPool(&x->reflFiltP, mp);
    tCookOnePole_setSampleRate(&x->reflFiltP, x->sampleRate);
    
    tCookOnePole_initToPool(&x->tensionFilt, mp);
    tCookOnePole_setSampleRate(&x->tensionFilt, x->sampleRate);


    //tCookOnePole_setPole(&x->reflFilt, 0.6f - (0.1f * 22050.f / x->sampleRate));
    //tCookOnePole_setGain(&x->reflFilt, .999999f);
    tCookOnePole_setGainAndPole(&x->reflFilt,0.999f, -0.0014f);
    tCookOnePole_setGainAndPole(&x->reflFiltP,0.999f, -0.0014f);
   
    tTString_setFreq(&x, 440.0f);
    //Lfloat slideTime = x->sampleRate * 0.000555555555556f;

    //tHighpass_initToPool(&x->dcBlock, .01f,  mp);
    tExpSmooth_initToPool(&x->tensionSmoother, 0.0f, 0.001f, mp);

    tExpSmooth_setDest(&x->tensionSmoother, 0.0f);

    tExpSmooth_initToPool(&x->pitchSmoother, 100.0f, 0.01f, mp);

    tExpSmooth_setDest(&x->pitchSmoother, 100.0f);

    tThiranAllpassSOCascade_initToPool(&x->allpass, 4, mp);
    tThiranAllpassSOCascade_initToPool(&x->allpassP, 4, mp);
    x->allpassDelay = tThiranAllpassSOCascade_setCoeff(&x->allpass, 0.0001f, 100.0f, x->invOversampling);
    x->allpassDelayP = tThiranAllpassSOCascade_setCoeff(&x->allpassP, 0.000025f, 100.0f, x->invOversampling);

    tSVF_initToPool(&x->lowpassP, SVFTypeLowpass, 6000.0f, 0.8f, mp);
    tSVF_setSampleRate(&x->lowpassP, x->sampleRate);
    tSVF_setFreq(&x->lowpassP, 6000.0f);
    tSVF_initToPool(&x->highpassP, SVFTypeHighpass, 1800.0f, 0.8f, mp);
    tSVF_setSampleRate(&x->highpassP, x->sampleRate);
    tSVF_setFreq(&x->highpassP, 1800.0f);
    x->twoPiTimesInvSampleRate = TWO_PI * x->invSampleRate;
    x->doTension = 0;
    tCycle_initToPool(&x->tensionModOsc, mp);
    tCycle_setSampleRate(&x->tensionModOsc, x->sampleRate);
    tCycle_initToPool(&x->pickupModOsc, mp);
    tCycle_setSampleRate(&x->pickupModOsc, x->sampleRate);
    x->pickupModOscFreq = 440.0f;
    x->pickupModOscAmp = 1.0f;
    tSVF_initToPool(&x->pickupFilter, SVFTypeLowpass, 3500.0f, 0.8f, mp);
    tSVF_setSampleRate(&x->pickupFilter, x->sampleRate);

    tSVF_setFreq(&x->pickupFilter, 3500.0f);
    tNoise_initToPool(&x->noise, PinkNoise, mp);
    tHighpass_initToPool(&x->dcBlock, 1.0f, mp);
    tHighpass_initToPool(&x->dcBlockP, 1.0f, mp);
    tSlide_initToPool(&x->slide, 0, 3000, mp);//100 1400
    tSlide_initToPool(&x->barSmooth, 1000, 1000, mp);//600 600
    tSlide_initToPool(&x->barPulseSlide, 2, 30, mp);//100 1400 // 10 3000
    tBiQuad_initToPool(&x->barFilter1, mp);

    //tBiQuad_setCoefficients(&x->barFilter1, 0.294f, -0.54f, 0.249f, -1.98f, 0.984f);
    tBiQuad_setCoefficients(&x->barFilter1, 0.221f, -0.393f, 0.188f, -1.973f, 0.984f);

    tBiQuad_initToPool(&x->barFilter2, mp);

    //tBiQuad_setCoefficients(&x->barFilter2, 0.031f, -0.051f, 0.020f, -1.985f, 0.986f);
    tBiQuad_setCoefficients(&x->barFilter2, 0.25f, -0.436f, 0.188f, -1.973f, 0.986f);

    tExpSmooth_initToPool(&x->barPulse, 0.0f,0.05f, mp); //was 0.02
    tExpSmooth_setDest(&x->barPulse, 0.0f);
    tExpSmooth_initToPool(&x->barSmooth2, 0.0f,0.0005f, mp); //was 0.001
    tExpSmooth_setDest(&x->barSmooth2, 0.0f);
    tExpSmooth_initToPool(&x->barSmoothVol, 0.0f,0.0008f, mp); //was 0.02
    tExpSmooth_setDest(&x->barSmoothVol, 0.0f);
    tSVF_initToPool(&x->barResonator, SVFTypeBandpass, 5.0f, 15.0f, mp);
    tSVF_initToPool(&x->barLowpass, SVFTypeLowpass, 300.0f, 0.707f, mp);

    tCycle_initToPool(&x->barOsc, mp);
    tCycle_setSampleRate(&x->barOsc, x->sampleRate*x->invOversampling);
    x->timeSinceLastBump = 1;
    tHighpass_initToPool(&x->barHP, 30.0f, mp);
    tSVF_initToPool(&x->barLP, SVFTypeLowpass, 8000.0f, 0.7f, mp);

}
void    tTString_free                  (tTString* const bw)
{
    _tTString* x = *bw;
    tHighpass_free(&x->dcBlockP);
    tHighpass_free(&x->dcBlock);
    tNoise_free(&x->noise);
    tSVF_free(&x->pickupFilter);
    tCycle_free(&x->tensionModOsc);
    tSVF_free(&x->highpassP);
    tSVF_free(&x->lowpassP),
    tThiranAllpassSOCascade_free(&x->allpassP);
    tThiranAllpassSOCascade_free(&x->allpass);
    tExpSmooth_free(&x->pitchSmoother);
    tExpSmooth_free(&x->tensionSmoother);
    tSlide_free(&x->slide);
    tCookOnePole_free(&x->tensionFilt);
    tCookOnePole_free(&x->reflFiltP);
    tCookOnePole_free(&x->reflFilt);
    tLagrangeDelay_free(&x->delayP);
    tLagrangeDelay_free(&x->delay);
    mpool_free((char*)x, x->mempool);
}

Lfloat   tTString_tick                  (tTString* const bw)
{
    _tTString* x = *bw;
    Lfloat theOutput = 0.0f;
    //return x->mempool->leaf->random() * 2.0f - 1.0f;
/*
    //bar noise not oversampled
    Lfloat tempSlide = fabsf(x->barPosition - x->prevBarPosition) * 100.0f;// * 500.0f;
	//tempSlide *= 0.25f;
    if (tempSlide > 1.0f)
    {
    	tempSlide = 1.0f;
    }
	tempSlide = tSVF_tick(&x->barLowpass, tempSlide) ;

	x->slideAmount = tSlide_tick(&x->slide, tempSlide);



	Lfloat thresholdedGain = 1.0f;
	if (x->slideAmount < 0.007f)
	{
		thresholdedGain = LEAF_map(x->slideAmount, 0.005f, 0.007f, 0.0, 1.0f);
		{
			if (thresholdedGain < 0.0f)
			{
				thresholdedGain = 0.0f;
			}
		}
	}
	x->slideAmount *= 10.0f * thresholdedGain;


	Lfloat barFreq = LEAF_map(x->slideAmount, 0.0000f, 0.07f, 5.0f, 1000.0f) ; //was 500 to 2000
	tSVF_setFreq(&x->barResonator, barFreq);
	x->barPulseInc = 1.0f / barFreq * x->sampleRate * x->invOversampling;
	if (x->barPulsePhasor > x->barPulseInc)
	{
		x->barPulsePhasor = 0;
		tExpSmooth_setVal(&x->barPulse, 1.0f);
	}
	x->barPulsePhasor++;

	x->slideNoise = tNoise_tick(&x->noise) * (Lfloat)(x->muteCoeff > 0.9f);
	if (x->wound)
	{
		Lfloat val = tExpSmooth_tick(&x->barPulse);

		//x->slideNoise *=  val;
		x->slideNoise =  val;
		//x->slideNoise =  val;
	   Lfloat tempSlideNoise1 = x->slideNoise * x->slideGain * x->slideAmount * thresholdedGain;

	   Lfloat tempSlideNoise2 = tempSlideNoise1;
	   Lfloat tempSlideNoise3 = tempSlideNoise1;

	   //tempSlideNoise1 = tBiQuad_tick(&x->barFilter1, tempSlideNoise1);
	   //tempSlideNoise1 = tBiQuad_tick(&x->barFilter2, tempSlideNoise1);

	   //tempSlideNoise2 = tSVF_tick(&x->barResonator, tempSlideNoise2) * thresholdedGain;
	   //tempSlideNoise2 = LEAF_tanh(tempSlideNoise2 * 1.4f);

	   x->slideNoise = tempSlideNoise1 * 0.3f + tempSlideNoise2 * 0.4f + tempSlideNoise3 * 0.3f;
	}
	else
	{
		x->slideNoise = x->slideNoise * x->slideGain * x->slideAmount * thresholdedGain;
	}


	*/
    Lfloat barPos = x->barPosition;
    //currentBump = tSVF_tick(&x->barLowpass, currentBump) ;
    if ((barPos > (x->prevBarPosSmoothed + 3.0f)) || (barPos < (x->prevBarPosSmoothed - 3.0f)))
	{
    	tExpSmooth_setValAndDest(&x->barSmooth2,barPos);
	}
    else
    {
    	tExpSmooth_setDest(&x->barSmooth2,barPos);
    }

    //Lfloat currentBumpSmoothed = currentBump;
    Lfloat barPosSmoothed = tExpSmooth_tick(&x->barSmooth2);

    Lfloat barDifference = fabsf(barPosSmoothed - x->prevBarPosSmoothed);


    tExpSmooth_setDest(&x->barSmoothVol,barPos);
        Lfloat barPosSmoothVol = tExpSmooth_tick(&x->barSmoothVol);
        Lfloat differenceFastSmooth = fabsf(barPosSmoothVol - x->prevBarPosSmoothVol);
        x->prevBarPosSmoothVol = barPosSmoothVol;
        Lfloat volCut = 1.0f;
        if (differenceFastSmooth < 0.0001f)
        {
        	volCut = LEAF_map(differenceFastSmooth, 0.00001f, 0.0001f, 0.0f, 1.0f);
        	if (volCut < 0.0f)
        	{
        		volCut = 0.0f;
        	}
        }
    Lfloat currentBump = barPosSmoothed * 70.0f;
    Lfloat bumpDifference = fabsf(currentBump - x->lastBump);

    x->prevBarPosSmoothed = barPosSmoothed;
    barDifference = LEAF_clip(0.0f, barDifference*2.0f, 1.0f);
    x->slideAmount = tSlide_tick(&x->slide, barDifference);
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

    	tSlide_setDest(&x->barSmooth, (x->sampleRate * x->invOversampling / x->timeSinceLastBump)*1.0f);
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
    Lfloat barFreq = tSlide_tickNoInput(&x->barSmooth);
    tSVF_setFreq(&x->barResonator, barFreq);
    tCycle_setFreq(&x->barOsc, barFreq);
    Lfloat filterAmount = 0.5f;
    Lfloat dryAmount = 1.0f;
    x->slideNoise = tNoise_tick(&x->noise);

    Lfloat pulseVal = tSlide_tickNoInput(&x->barPulseSlide);

    if (pulseVal > .99f)
    {
    	tSlide_setDest(&x->barPulseSlide, 0.0f);

    }

    x->slideNoise *= pulseVal;
    //x->slideNoise = tExpSmooth_tick(&x->barPulse);
    x->slideNoise *= tExpSmooth_tick(&x->barPulse);
    //x->slideNoise *= x->slideGain;
    Lfloat tempSlideNoise1 = x->slideNoise;
    Lfloat tempSlideNoise2 = fast_tanh(tSVF_tick(&x->barResonator, tempSlideNoise1)*(x->barDrive + 1.0f));

    Lfloat filterFade = 1.0f;
    if (barFreq < 300.0f)
    {
    	filterFade = LEAF_map(barFreq, 100.0f, 300.0f, 0.0f, 1.0f);
    	filterFade = LEAF_clip(0.0f, filterFade, 1.0f);
    }
    //tempSlideNoise2 += tCycle_tick(&x->barOsc);
    //tempSlideNoise1 = tBiQuad_tick(&x->barFilter1, tempSlideNoise1);
   	//tempSlideNoise1 = tBiQuad_tick(&x->barFilter2, tempSlideNoise1);
   	x->slideNoise = ((tempSlideNoise1 * dryAmount) + (tempSlideNoise2 * filterAmount * filterFade));// * x->slideAmount;
   	//x->slideNoise =tempSlideNoise2 * filterAmount* filterFade * x->slideAmount;
   	x->slideNoise *= (Lfloat)(x->muteCoeff > 0.99f);
   	x->slideNoise = tHighpass_tick(&x->barHP, x->slideNoise);
   	x->slideNoise = tSVF_tick(&x->barLP, x->slideNoise * x->slideGain * 2.0f * volCut);
   	x->slideNoise = x->slideNoise * x->slideGain;
	x->prevDelay = x->baseDelay;

    for (int i = 0; i < x->oversampling; i++)
    {
    	x->baseDelay = tExpSmooth_tick(&x->pitchSmoother);

        Lfloat currentDelay = x->baseDelay - x->allpassDelay - x->tensionAmount;

    	//Lfloat currentDelay = x->baseDelay;
        if (currentDelay < 10.0f)
        {
        	currentDelay = 10.0f;
        }
        Lfloat maxDelay = tLagrangeDelay_getMaxDelay (&x->delay);
        if (currentDelay > maxDelay)
        {
        	currentDelay = maxDelay;
        }
        Lfloat halfCurrentDelay = currentDelay * 0.5f;


    	volatile Lfloat tension = tExpSmooth_tick(&x->tensionSmoother) * x->tensionGain * x->baseDelay;

    	x->tensionAmount = (tension + (tCycle_tick(&x->tensionModOsc) * tension * 0.1f)) * 0.01f;

        if (currentDelay > 10.0f)
        {
        	tLagrangeDelay_setDelay(&x->delay, currentDelay);
        }


        Lfloat delayOut = tCookOnePole_tick(&x->reflFilt, x->output);
        //x->tempSum -= delayOut;
        //Lfloat delayOut = x->output;
        Lfloat output = tLagrangeDelay_tickOut(&x->delay);

        Lfloat rippleLength = x->rippleRate * x->baseDelay;
        uint32_t rippleLengthInt = (uint32_t) rippleLength;
        Lfloat rippleLengthAlpha = rippleLength - rippleLengthInt;
        output += (tLagrangeDelay_tapOutInterpolated(&x->delay,rippleLengthInt,rippleLengthAlpha) * x->r);


        output = tThiranAllpassSOCascade_tick(&x->allpass, output);
        x->output = LEAF_clip(-1.0f, (output * (x->decayCoeff * x->invOnePlusr) * x->muteCoeff), 1.0f);


        //if past the string ends, need to read backwards and invert
        uint32_t backwards = 0;
        Lfloat invert = 1.0f;

        currentDelay = x->baseDelay;
        halfCurrentDelay = currentDelay * 0.5f;
        //Lfloat pickupPos = x->openStringLength * x->pickupPos + (tCycle_tick(&x->pickupModOsc) * x->pickupModOscAmp * x->openStringLength);//(x->pickupPos) * halfCurrentDelay;
        Lfloat positionMin = (x->openStringLength * 0.104166666666667f);
        Lfloat positionMax = (x->openStringLength * 0.021666666666667f);
        Lfloat pickupInput = x->pickupPos + (tCycle_tick(&x->pickupModOsc) * x->pickupModOscAmp);
        Lfloat pickupPos = LEAF_map( pickupInput, 0.0f, 1.0f, positionMin, positionMax);

        //Lfloat pickupPos = x->openStringLength * 0.041666666666667f;


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
        Lfloat pickupPosFinal = pickupPos;
        if (backwards)
        {
        	pickupPosFinal = halfCurrentDelay - pickupPos;
        }


        uint32_t pickupPosInt = (uint32_t) pickupPosFinal;
        Lfloat pickupPosAlpha = pickupPosFinal - pickupPosInt;
        Lfloat inversePickupPos = currentDelay - pickupPos;
        if (backwards)
        {
        	inversePickupPos = halfCurrentDelay + pickupPos;
        }
        uint32_t inversePickupPosInt = (uint32_t) inversePickupPos;
        Lfloat inversePickupPosAlpha = inversePickupPos - inversePickupPosInt;
       	Lfloat rightgoing = 0.0f;
       	Lfloat leftgoing = 0.0f;


        rightgoing = tLagrangeDelay_tapOut(&x->delay, pickupPosInt) * (1.0f - pickupPosAlpha) * invert;
        rightgoing += tLagrangeDelay_tapOut(&x->delay, pickupPosInt + 1) * pickupPosAlpha * invert;
        leftgoing = tLagrangeDelay_tapOut(&x->delay, inversePickupPosInt) * (1.0f - inversePickupPosAlpha) * invert;
        leftgoing += tLagrangeDelay_tapOut(&x->delay, inversePickupPosInt + 1) * inversePickupPosAlpha * invert;

        x->pickupOut =  (rightgoing - leftgoing);


        //tLagrangeDelay_addTo(&x->delay, x->slideNoise, 0);

        //tLagrangeDelay_tickIn(&x->delay, tHighpass_tick(&x->dcBlock, delayOut + x->slideNoise));// + x->slideNoise);
        tLagrangeDelay_tickIn(&x->delay, delayOut + x->slideNoise);
        x->pickupPosL = pickupPosFinal;
        x->pickupPosR = inversePickupPos;
        x->pickupPosLAlpha = pickupPosAlpha;
        x->pickupPosRAlpha = inversePickupPosAlpha;
        x->backwards = backwards;

        Lfloat currentDelayP = x->baseDelay - x->allpassDelayP - x->tensionAmount;
        if (currentDelayP < 10.0f)
        {
        	currentDelayP = 10.0f;
        }
        Lfloat maxDelayP = tLagrangeDelay_getMaxDelay (&x->delayP);
        if (currentDelayP > maxDelayP)
        {
        	currentDelayP = maxDelayP;
        }

        if (currentDelayP > 10.0f)
        {
        	tLagrangeDelay_setDelay(&x->delayP, currentDelayP);
        }


        Lfloat delayOutP = tCookOnePole_tick(&x->reflFilt, x->outputP);

        Lfloat outputP = tLagrangeDelay_tickOut(&x->delayP);
/*
        Lfloat rippleLengthP = x->rippleRate * currentDelayP;
        if (rippleLengthP > currentDelayP)
        {
        	rippleLengthP = currentDelayP;
        }
        if (rippleLengthP < 10.0f)
        {
        	rippleLengthP = 10.0f;
        }
        uint32_t rippleLengthIntP = (uint32_t) rippleLengthP;
        Lfloat rippleLengthAlphaP = rippleLengthP - rippleLengthIntP;
        */
        outputP += (tLagrangeDelay_tapOutInterpolated(&x->delayP,rippleLengthInt,rippleLengthAlpha) * x->r);


        outputP = tThiranAllpassSOCascade_tick(&x->allpassP, outputP);
        x->outputP = LEAF_clip(-1.0f, (outputP * (x->decayCoeff * x->invOnePlusr) * x->muteCoeff), 1.0f);


        rightgoing = tLagrangeDelay_tapOut(&x->delayP, pickupPosInt) * (1.0f - pickupPosAlpha);
        rightgoing += tLagrangeDelay_tapOut(&x->delayP, pickupPosInt + 1) * pickupPosAlpha;
        leftgoing = tLagrangeDelay_tapOut(&x->delayP, inversePickupPosInt) * (1.0f - inversePickupPosAlpha);
        leftgoing += tLagrangeDelay_tapOut(&x->delayP, inversePickupPosInt + 1) * inversePickupPosAlpha;


        x->pickupOutP =  (rightgoing - leftgoing);

        //tLagrangeDelay_tickIn(&x->delayP, tHighpass_tick(&x->dcBlockP, delayOutP + x->slideNoise));
        tLagrangeDelay_tickIn(&x->delayP, delayOutP + x->slideNoise);

        Lfloat outputPfilt = tSVF_tick(&x->lowpassP, x->pickupOutP );
        outputPfilt = tSVF_tick(&x->highpassP, outputPfilt);

        Lfloat prefilter = (x->pickupOut + (outputPfilt * x->phantomGain)) * 2.0f;
        theOutput = tSVF_tick(&x->pickupFilter, prefilter );// + x->slideNoise;
    }
    return theOutput;

    //return x->output;
}

void    tTString_setFreq               (tTString* const bw, Lfloat freq)
{
    _tTString* x = *bw;
    if (freq < x->actualLowestFreq)
        freq = x->actualLowestFreq;
    //freq = freq * x->invOversampling;
    x->freq = freq;
    tExpSmooth_setDest(&x->pitchSmoother, (x->sampleRate / freq) - 2.0f);
    tCycle_setFreq(&x->tensionModOsc, freq * x->invOversamplingTimesTwo);
}

void    tTString_setDecay               (tTString* const bw, Lfloat decay)
{
	 _tTString* x = *bw;
	 x->decayInSeconds = (decay * 20.0f);
	 if (x->decayInSeconds > 19.0f)
	 {
		 x->decayCoeff = 1.0f;
	 }
	 else
	 {
		 decay = expf(-1.0f / (x->freq * x->decayInSeconds));
		 //decay = LEAF_map(decay, 0.0f, 1.0f, 0.99f, 1.01f);
		 decay = LEAF_clip(0.9f, decay, 1.0f);
		 x->decayCoeff = decay;
	 }
}

void    tTString_setPickupPos               (tTString* const bw, Lfloat pos)
{
	_tTString* x = *bw;
	x->pickupPos = pos;
}

void    tTString_setPickupModAmp               (tTString* const bw, Lfloat amp)
{
	_tTString* x = *bw;
	x->pickupModOscAmp = amp * 1.0f;
}


void    tTString_setPickupModFreq               (tTString* const bw, Lfloat freq)
{
	_tTString* x = *bw;
	x->pickupModOscFreq = freq;
	tCycle_setFreq(&x->pickupModOsc, freq);
}



void    tTString_mute              (tTString* const bw)
{
	 _tTString* x = *bw;
	 x->muteCoeff = 0.8f;
}

void    tTString_setFilter              (tTString* const bw, Lfloat filter)
{
	 _tTString* x = *bw;

	 filter = ((filter * 18000.0f) + 20.0f)* x->twoPiTimesInvSampleRate;
	tCookOnePole_setPole(&x->reflFilt,filter);
	tCookOnePole_setPole(&x->reflFiltP,filter);

}

void   tTString_setTensionGain                  (tTString* const bw, Lfloat tensionGain)
{
	_tTString* x = *bw;
	x->tensionGain = tensionGain;
}

void   tTString_setBarDrive                  (tTString* const bw, Lfloat drive)
{
	_tTString* x = *bw;
	x->barDrive = drive;
}

void   tTString_setBarPosition                  (tTString* const bw, Lfloat barPosition)
{
	_tTString* x = *bw;
	x->prevBarPosition = x->barPosition;
	x->barPosition = barPosition;
}

void   tTString_setOpenStringFrequency                  (tTString* const bw, Lfloat openStringFrequency)
{
	_tTString* x = *bw;
	x->openStringLength = ((x->sampleRate / openStringFrequency) - 2.0f);
}

void   tTString_setPickupRatio                  (tTString* const bw, Lfloat ratio)
{
	_tTString* x = *bw;
	x->pickupRatio = ratio;
}

void   tTString_setPhantomHarmonicsGain                  (tTString* const bw, Lfloat gain)
{
	_tTString* x = *bw;
	x->phantomGain = gain;
}

void   tTString_setSlideGain                  (tTString* const bw, Lfloat gain)
{
	_tTString* x = *bw;
	x->slideGain = gain;
}

void   tTString_setTensionSpeed                  (tTString* const bw, Lfloat tensionSpeed)
{
	_tTString* x = *bw;
	tExpSmooth_setFactor(&x->tensionSmoother, 0.00016f * (1.0f - tensionSpeed) + 0.00001f); //0.5 is good
}


void   tTString_setRippleDepth                  (tTString* const bw, Lfloat depth)
{
	_tTString* x = *bw;
	x->r = expf(-1.0f / (x->freq * (x->decayInSeconds * 0.005f))) * depth;

	x->invOnePlusr = 1.0f / (1.0f + x->r);
}

void   tTString_setHarmonic                  (tTString* const bw, Lfloat harmonic)
{
	_tTString* x = *bw;
	x->harmonic = harmonic;
	x->rippleRate = 1.0f / harmonic;
}


void    tTString_pluck               (tTString* const bw, Lfloat position, Lfloat amplitude)
{
    _tTString* x = *bw;
    tExpSmooth_setVal(&x->tensionSmoother, amplitude);
    x->baseDelay = tExpSmooth_tick(&x->pitchSmoother);
    Lfloat currentDelay = x->baseDelay;// - x->allpassDelay;
    x->muteCoeff = 1.0f;
    uint32_t halfCurrentDelay = currentDelay * 0.5f;
    Lfloat positionMin = (x->openStringLength * 0.104166666666667f);
    Lfloat positionMax = (x->openStringLength * 0.0625f);

    volatile Lfloat pluckPoint = LEAF_map(position, 0.0f, 1.0f, positionMin, positionMax);
    //0.083335f; * position;//position * halfCurrentDelay;
    pluckPoint = LEAF_clip(0.0f, pluckPoint, halfCurrentDelay);
    uint32_t pluckPointInt = (uint32_t) pluckPoint;
    volatile Lfloat remainder = halfCurrentDelay-pluckPoint;
    tLagrangeDelay_clear(&x->delay);
    for (uint32_t i = 0; i < halfCurrentDelay; i++)
    {
        Lfloat val = 0.0f;



        //displacement waves

        if (i <= pluckPointInt)
        {
            val = amplitude * ((Lfloat)i/(Lfloat)pluckPointInt);
        }
        else
        {
            val = amplitude * (1.0f - (((Lfloat)i-(Lfloat)pluckPointInt)/(Lfloat)remainder));
        }
        
        tLagrangeDelay_tapIn(&x->delay, val, i);
        tLagrangeDelay_tapIn(&x->delay, -val, halfCurrentDelay*2-i);


        //slope waves version
        /*
        if (i <= pluckPointInt)
		{
			val = amplitude * position;
		}
		else
		{
			val = -amplitude * (1.0f - position);
		}

		tLagrangeDelay_tapIn(&x->delay, val, i);
		tLagrangeDelay_tapIn(&x->delay, val, halfCurrentDelay*2-i);
		*/
    }
    tThiranAllpassSOCascade_clear(&x->allpass);



    Lfloat currentDelayP = x->baseDelay - x->allpassDelayP;

    uint32_t halfCurrentDelayP = currentDelayP * 0.5f;
    volatile Lfloat pluckPointP = position * halfCurrentDelayP;
    pluckPointP = LEAF_clip(1.0f, pluckPoint, halfCurrentDelayP-1.0f);
    uint32_t pluckPointIntP = (uint32_t) pluckPointP;
    volatile Lfloat remainderP = halfCurrentDelayP-pluckPointP;
    tLagrangeDelay_clear(&x->delayP);
    for (uint32_t i = 0; i < halfCurrentDelayP; i++)
    {
        Lfloat val = 0.0f;

        if (i <= pluckPointIntP)
        {
            val = amplitude * ((Lfloat)i/(Lfloat)pluckPointIntP);
        }
        else
        {
            val = amplitude * (1.0f - (((Lfloat)i-(Lfloat)pluckPointIntP)/(Lfloat)remainderP));
        }

        //slope waves version
        /*
        if (i <= pluckPointIntP)
		{
			val = amplitude * position;
		}
		else
		{
			val = -amplitude * (1.0f - position);
		}*/
        tLagrangeDelay_tapIn(&x->delayP, val, i);
        tLagrangeDelay_tapIn(&x->delayP, -val, halfCurrentDelayP*2-i);
    }
    tThiranAllpassSOCascade_clear(&x->allpassP);
}



void    tTString_setWaveLength         (tTString* const, Lfloat waveLength); // in samples
void    tTString_setSampleRate         (tTString* const, Lfloat sr);

void    tTString_setHarmonicity         (tTString* const bw, Lfloat B, Lfloat freq)
{
    _tTString* x = *bw;
    x->allpassDelay = tThiranAllpassSOCascade_setCoeff(&x->allpass, B, freq * x->invOversampling, x->oversampling);
    x->allpassDelayP = tThiranAllpassSOCascade_setCoeff(&x->allpassP, B * 0.25f, freq * x->invOversampling, x->oversampling);
}



////BOW TABLE OBJECT

void    tBowTable_init                  (tBowTable* const bt, LEAF* const leaf)
{
    tBowTable_initToPool(bt, &leaf->mempool);
}
void    tBowTable_initToPool            (tBowTable* const bt, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tBowTable* x = *bt = (_tBowTable*) mpool_alloc(sizeof(_tBowTable), m);
    x->mempool = m;
    
    x->offSet = 0.0f;
    x->slope = 0.1f;
    x->lastOutput = 0.0f;
}
void    tBowTable_free                 (tBowTable* const bt)
{
    _tBowTable* x = *bt;
    mpool_free((char*)x, x->mempool);
    
}
Lfloat    tBowTable_lookup               (tBowTable* const bt, Lfloat sample)
{
    _tBowTable* x = *bt;
    Lfloat input;
    input = sample + x->offSet;                                /*  add bias to sample      */
    input *= x->slope;                                         /*  scale it                */
    x->lastOutput = (float)fabs((double) input) + 0.75f;       /*  below min delta, friction = 1 */
    x->lastOutput = pow(x->lastOutput, -4.0f);
    if (x->lastOutput > 1.0f ) x->lastOutput = 1.0f;     /*  maximum friction is 1.0 */
    return x->lastOutput;
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
