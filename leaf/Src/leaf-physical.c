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
void    tPluck_init         (tPluck* const pl, float lowestFrequency, LEAF* const leaf)
{
    tPluck_initToPool(pl, lowestFrequency, &leaf->mempool);
}

void    tPluck_initToPool    (tPluck* const pl, float lowestFrequency, tMempool* const mp)
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
    float delay = ( p->sampleRate / frequency ) - tOneZero_getPhaseDelay(&p->loopFilter, frequency );
    
    tAllpassDelay_setDelay(&p->delayLine, delay );
    
    p->loopGain = 0.99f + (frequency * 0.000005f);
    
    if ( p->loopGain >= 0.999f ) p->loopGain = 0.999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tPluck_controlChange (tPluck* const pl, int number, float value)
{
    return;
}

void tPluck_setSampleRate(tPluck* const pl, float sr)
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
void    tKarplusStrong_init (tKarplusStrong* const pl, float lowestFrequency, LEAF* const leaf)
{
    tKarplusStrong_initToPool(pl, lowestFrequency, &leaf->mempool);
}

void    tKarplusStrong_initToPool   (tKarplusStrong* const pl, float lowestFrequency, tMempool* const mp)
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
    p->lastLength = p->sampleRate / p->lastFrequency;
    float delay = p->lastLength - 0.5f;
    tAllpassDelay_setDelay(&p->delayLine, delay);
    
    // MAYBE MODIFY LOOP GAINS
    p->loopGain = p->baseLoopGain + (frequency * 0.000005f);
    if (p->loopGain >= 1.0f) p->loopGain = 0.99999f;
    
    tKarplusStrong_setStretch(pl, p->stretching);
    
    tLinearDelay_setDelay(&p->combDelay, 0.5f * p->pickupPosition * p->lastLength );
}

// Set the stretch "factor" of the string (0.0 - 1.0).
void    tKarplusStrong_setStretch         (tKarplusStrong* const pl, float stretch)
{
    _tKarplusStrong* p = *pl;
    
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

void    tKarplusStrong_setSampleRate (tKarplusStrong* const pl, float sr)
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

void    tSimpleLivingString_init(tSimpleLivingString* const pl, float freq, float dampFreq,
                                 float decay, float targetLev, float levSmoothFactor,
                                 float levStrength, int levMode, LEAF* const leaf)
{
    tSimpleLivingString_initToPool(pl, freq, dampFreq, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tSimpleLivingString_initToPool  (tSimpleLivingString* const pl, float freq, float dampFreq,
                                         float decay, float targetLev, float levSmoothFactor,
                                         float levStrength, int levMode, tMempool* const mp)
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

void     tSimpleLivingString_setFreq(tSimpleLivingString* const pl, float freq)
{
    _tSimpleLivingString* p = *pl;
    
    if (freq<20) freq=20;
    else if (freq>10000) freq=10000;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString_setWaveLength(tSimpleLivingString* const pl, float waveLength)
{
    _tSimpleLivingString* p = *pl;
    
    if (waveLength<4.8) waveLength=4.8f;
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

void   tSimpleLivingString_setSampleRate(tSimpleLivingString* const pl, float sr)
{
    _tSimpleLivingString* p = *pl;
    float freq = p->sampleRate/p->waveLengthInSamples;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(&p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(&p->DCblocker, p->sampleRate);
}

/* Living String*/

void    tLivingString_init(tLivingString* const pl, float freq, float pickPos, float prepIndex,
                           float dampFreq, float decay, float targetLev, float levSmoothFactor,
                           float levStrength, int levMode, LEAF* const leaf)
{
    tLivingString_initToPool(pl, freq, pickPos, prepIndex, dampFreq, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tLivingString_initToPool    (tLivingString* const pl, float freq, float pickPos, float prepIndex,
                                     float dampFreq, float decay, float targetLev, float levSmoothFactor,
                                     float levStrength, int levMode, tMempool* const mp)
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

void     tLivingString_setFreq(tLivingString* const pl, float freq)
{    // NOTE: It is faster to set wavelength in samples directly
    _tLivingString* p = *pl;
    
    if (freq<20.f) freq=20.f;
    else if (freq>10000.f) freq=10000.f;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString_setWaveLength(tLivingString* const pl, float waveLength)
{
    _tLivingString* p = *pl;
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.f) waveLength=2400.f;
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

void   tLivingString_setSampleRate(tLivingString* const pl, float sr)
{
    _tLivingString* p = *pl;
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


//////////---------------------------
/* Version of Living String with Hermite Interpolation */
/*Living String experiment 2 */

/* Living String*/

void    tLivingString2_init(tLivingString2* const pl, float freq, float pickPos, float prepPos, float pickupPos, float prepIndex,
                           float brightness, float decay, float targetLev, float levSmoothFactor,
                           float levStrength, int levMode, LEAF* const leaf)
{
    tLivingString2_initToPool(pl, freq, pickPos, prepPos, pickupPos, prepIndex, brightness, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tLivingString2_initToPool    (tLivingString2* const pl, float freq, float pickPos, float prepPos, float pickupPos, float prepIndex,
                                     float brightness, float decay, float targetLev, float levSmoothFactor,
                                     float levStrength, int levMode, tMempool* const mp)
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
    tHighpass_initToPool(&p->DCblockerU,13, mp);
    tHighpass_initToPool(&p->DCblockerL,13, mp);
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

void     tLivingString2_setFreq(tLivingString2* const pl, float freq)
{    // NOTE: It is faster to set wavelength in samples directly
    _tLivingString2* p = *pl;
    
    if (freq<20.f) freq=20.f;
    else if (freq>10000.f) freq=10000.f;
    freq = freq*2;
    p->freq = freq;
    p->waveLengthInSamples = (p->sampleRate/p->freq) - 1;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString2_setWaveLength(tLivingString2* const pl, float waveLength)
{
    _tLivingString2* p = *pl;
    
    waveLength = waveLength * 0.5f;
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.f) waveLength=2400.f;
    p->waveLengthInSamples = waveLength - 1;
    p->freq = p->sampleRate / waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString2_setPickPos(tLivingString2* const pl, float pickPos)
{    // between 0 and 1
    _tLivingString2* p = *pl;
    if (pickPos<0.f) pickPos=0.f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(&p->ppSmooth, p->pickPos);
}

void     tLivingString2_setPrepPos(tLivingString2* const pl, float prepPos)
{    // between 0 and 1
    _tLivingString2* p = *pl;
    if (prepPos<0.f) prepPos=0.f;
    else if (prepPos>1.f) prepPos=1.f;
    p->prepPos = prepPos;
    tExpSmooth_setDest(&p->prpSmooth, p->prepPos);
}

void     tLivingString2_setPickupPos(tLivingString2* const pl, float pickupPos)
{    // between 0 and 1
    _tLivingString2* p = *pl;
    if (pickupPos<0.f) pickupPos=0.f;
    else if (pickupPos>1.f) pickupPos=1.f;
    p->pickupPos = pickupPos;
    tExpSmooth_setDest(&p->puSmooth, p->pickupPos);
}

void     tLivingString2_setPrepIndex(tLivingString2* const pl, float prepIndex)
{    // between 0 and 1
    _tLivingString2* p = *pl;
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tLivingString2_setBrightness(tLivingString2* const pl, float brightness)
{
    _tLivingString2* p = *pl;
    float h0=(1.0 + brightness) * 0.5f;
    float h1=(1.0 - brightness) * 0.25f;

    tTwoZero_setCoefficients(&p->bridgeFilter, h1, h0, h1);
    tTwoZero_setCoefficients(&p->nutFilter, h1, h0, h1);
    tTwoZero_setCoefficients(&p->prepFilterU, h1, h0, h1);
    tTwoZero_setCoefficients(&p->prepFilterL, h1, h0, h1);
}

void     tLivingString2_setDecay(tLivingString2* const pl, float decay)
{
    _tLivingString2* p = *pl;
    p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tLivingString2_setTargetLev(tLivingString2* const pl, float targetLev)
{
    _tLivingString2* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLevL, targetLev);
}

void     tLivingString2_setLevSmoothFactor(tLivingString2* const pl, float levSmoothFactor)
{
    _tLivingString2* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLevL, levSmoothFactor);
}

void     tLivingString2_setLevStrength(tLivingString2* const pl, float levStrength)
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

float   tLivingString2_tick(tLivingString2* const pl, float input)
{
    _tLivingString2* p = *pl;

    input = input * 0.5f; // drop gain by half since we'll be equally adding it at half amplitude to forward and backward waveguides
    // from prepPos upwards=forwards
    float wLen=tExpSmooth_tick(&p->wlSmooth);

    float pickP=tExpSmooth_tick(&p->ppSmooth);

    //float pickupPos=tExpSmooth_tick(&p->puSmooth);

    //need to determine which delay line to put it into (should be half amplitude into forward and backward lines for the correct portion of string)
    float prepP=tExpSmooth_tick(&p->prpSmooth);
    float lowLen=prepP*wLen;
    float upLen=(1.0f-prepP)*wLen;
    uint32_t pickPInt;
/*
    if (pickP > prepP)
    {
        float fullPickPoint =  ((pickP*wLen) - lowLen);
        pickPInt = (uint) fullPickPoint; // where does the input go? that's the pick point
        float pickPFloat = fullPickPoint - pickPInt;

        tHermiteDelay_addTo(&p->delUF, input * (1.0f - pickPFloat), pickPInt);
        tHermiteDelay_addTo(&p->delUF, input * pickPFloat, pickPInt + 1);
        tHermiteDelay_addTo(&p->delUB, input * (1.0f - pickPFloat), (uint) (upLen - pickPInt));
        tHermiteDelay_addTo(&p->delUB, input * pickPFloat, (uint) (upLen - pickPInt - 1));
    }
    else
    {
         float fullPickPoint =  pickP * wLen;
        pickPInt = (uint) fullPickPoint; // where does the input go? that's the pick point
        float pickPFloat = fullPickPoint - pickPInt;

        tHermiteDelay_addTo(&p->delLF, input * (1.0f - pickPFloat), pickPInt);
        tHermiteDelay_addTo(&p->delLF, input * pickPFloat, pickPInt + 1);
        tHermiteDelay_addTo(&p->delLB, input * (1.0f - pickPFloat), (uint) (lowLen - pickPInt));
        tHermiteDelay_addTo(&p->delLB, input * pickPFloat, (uint) (lowLen - pickPInt - 1));
    }
*/
    if (pickP > prepP)
    {
        float fullPickPoint =  ((pickP*wLen) - lowLen);
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

        tHermiteDelay_addTo(&p->delUF, input, pickPInt);
        tHermiteDelay_addTo(&p->delUB, input, (uint32_t) (upLen - pickPInt));
    }
    else
    {
        float fullPickPoint =  pickP * wLen;
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

        tHermiteDelay_addTo(&p->delLF, input, pickPInt);
        tHermiteDelay_addTo(&p->delLB, input, (uint32_t) (lowLen - pickPInt));
    }

    float fromLF=tHermiteDelay_tickOut(&p->delLF);
    float fromUF=tHermiteDelay_tickOut(&p->delUF);
    float fromUB=tHermiteDelay_tickOut(&p->delUB);
    float fromLB=tHermiteDelay_tickOut(&p->delLB);
    // into upper half of string, from bridge, going backwards
    float fromBridge=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(&p->DCblockerU, tTwoZero_tick(&p->bridgeFilter, fromUF)));
    tHermiteDelay_tickIn(&p->delUB, fromBridge);
    // into lower half of string, from prepPoint, going backwards
    float fromLowerPrep=-tTwoZero_tick(&p->prepFilterL, fromLF);
    float intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB; //used to add input here
    tHermiteDelay_tickIn(&p->delLB, intoLower);
    // into lower half of string, from nut
    float fromNut=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tTwoZero_tick(&p->nutFilter, fromLB)));
    tHermiteDelay_tickIn(&p->delLF, fromNut);
    // into upper half of string, from prepPoint, going forwards/upwards
    float fromUpperPrep=-tTwoZero_tick(&p->prepFilterU, fromUB);
    float intoUpper=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF;
    tHermiteDelay_tickIn(&p->delUF, intoUpper);
    // update all delay lengths

    tHermiteDelay_setDelay(&p->delLF, lowLen);
    tHermiteDelay_setDelay(&p->delLB, lowLen);
    tHermiteDelay_setDelay(&p->delUF, upLen);
    tHermiteDelay_setDelay(&p->delUB, upLen);
    
    uint32_t pickupPosInt;
    float pickupOut = 0.0f;
    if (p->pickupPos < 0.98f)
    {
        float fullPickupPos = (p->pickupPos*upLen);
        pickupPosInt = (uint32_t) fullPickupPos;
        float pickupPosFloat = fullPickupPos - pickupPosInt;
        if (pickupPosInt == 0)
        {
            pickupPosInt = 1;
        }
        pickupOut = tHermiteDelay_tapOutInterpolated(&p->delUF, pickupPosInt, pickupPosFloat);
        pickupOut += tHermiteDelay_tapOutInterpolated(&p->delUB, (uint32_t) (upLen - pickupPosInt), pickupPosFloat);
        p->curr = pickupOut;
    }
    else
    
    {
        p->curr = fromBridge;
    }

    //p->curr = fromBridge;
    //p->curr += fromNut;

    return p->curr;
}

float   tLivingString2_tickEfficient(tLivingString2* const pl, float input)
{
    _tLivingString2* p = *pl;

    input = input * 0.5f; // drop gain by half since we'll be equally adding it at half amplitude to forward and backward waveguides
    // from prepPos upwards=forwards
    //float pickupPos=tExpSmooth_tick(&p->puSmooth);
    float wLen = p->wlSmooth->dest;

    float pickP = p->ppSmooth->dest;

    //need to determine which delay line to put it into (should be half amplitude into forward and backward lines for the correct portion of string)
    float prepP = p->prpSmooth->dest;
    float lowLen=p->prpSmooth->dest*p->wlSmooth->dest;
    float upLen=(1.0f-p->prpSmooth->dest)*p->wlSmooth->dest;
    uint32_t pickPInt;
    if (pickP > prepP)
    {
        float fullPickPoint =  ((pickP*wLen) - lowLen);
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

        tHermiteDelay_addTo(&p->delUF, input, pickPInt);
        tHermiteDelay_addTo(&p->delUB, input, (uint32_t) (upLen - pickPInt));
    }
    else
    {
        float fullPickPoint =  pickP * wLen;
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

        tHermiteDelay_addTo(&p->delLF, input, pickPInt);
        tHermiteDelay_addTo(&p->delLB, input, (uint32_t) (lowLen - pickPInt));
    }
    float fromLF=tHermiteDelay_tickOut(&p->delLF);
    float fromUF=tHermiteDelay_tickOut(&p->delUF);
    float fromUB=tHermiteDelay_tickOut(&p->delUB);
    float fromLB=tHermiteDelay_tickOut(&p->delLB);
    // into upper half of string, from bridge, going backwards
    //float fromBridge=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerU, tTwoZero_tick(&p->bridgeFilter, fromUF)));
    float fromBridge=-tFeedbackLeveler_tick(&p->fbLevU,tHighpass_tick(&p->DCblockerU, tTwoZero_tick(&p->bridgeFilter, fromUF)));
    tHermiteDelay_tickIn(&p->delUB, fromBridge);
    // into lower half of string, from prepPoint, going backwards
    float fromLowerPrep=-tTwoZero_tick(&p->prepFilterL, fromLF);
    float intoLower=(p->prepIndex*fromLowerPrep)+((1.0f - p->prepIndex)*fromUB); //used to add input here
    tHermiteDelay_tickIn(&p->delLB, intoLower);
    // into lower half of string, from nut
    //float fromNut=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tTwoZero_tick(&p->nutFilter, fromLB)));
    float fromNut=-tFeedbackLeveler_tick(&p->fbLevL,tHighpass_tick(&p->DCblockerL, tTwoZero_tick(&p->nutFilter, fromLB)));
    tHermiteDelay_tickIn(&p->delLF, fromNut);
    // into upper half of string, from prepPoint, going forwards/upwards
    float fromUpperPrep=-tTwoZero_tick(&p->prepFilterU, fromUB);
    float intoUpper=(p->prepIndex*fromUpperPrep)+((1.0f - p->prepIndex)*fromLF);
    tHermiteDelay_tickIn(&p->delUF, intoUpper);
    // update all delay lengths

    p->curr = fromBridge;

    //p->curr = fromBridge;
    //p->curr += fromNut;

    return p->curr;
}

float   tLivingString2_udpateDelays(tLivingString2* const pl)
{
    _tLivingString2* p = *pl;



    //need to determine which delay line to put it into (should be half amplitude into forward and backward lines for the correct portion of string)

    float lowLen=p->prpSmooth->dest*p->wlSmooth->dest;
    float upLen=(1.0f-p->prpSmooth->dest)*p->wlSmooth->dest;



	tHermiteDelay_setDelay(&p->delLF, lowLen);
    tHermiteDelay_setDelay(&p->delLB, lowLen);
    tHermiteDelay_setDelay(&p->delUF, upLen);
    tHermiteDelay_setDelay(&p->delUB, upLen);
}

float   tLivingString2_sample(tLivingString2* const pl)
{
    _tLivingString2* p = *pl;
    return p->curr;
}

void    tLivingString2_setSampleRate(tLivingString2* const pl, float sr)
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

void    tComplexLivingString_init(tComplexLivingString* const pl, float freq, float pickPos, float prepPos, float prepIndex,
                           float dampFreq, float decay, float targetLev, float levSmoothFactor,
                           float levStrength, int levMode, LEAF* const leaf)
{
    tComplexLivingString_initToPool(pl, freq, pickPos, prepPos, prepIndex, dampFreq, decay, targetLev, levSmoothFactor, levStrength, levMode, &leaf->mempool);
}

void    tComplexLivingString_initToPool    (tComplexLivingString* const pl, float freq, float pickPos, float prepPos, float prepIndex,
                                     float dampFreq, float decay, float targetLev, float levSmoothFactor,
                                     float levStrength, int levMode, tMempool* const mp)
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

void     tComplexLivingString_setFreq(tComplexLivingString* const pl, float freq)
{    // NOTE: It is faster to set wavelength in samples directly
    _tComplexLivingString* p = *pl;
    
    if (freq<20.0f) freq=20.0f;
    else if (freq>10000.0f) freq=10000.0f;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tComplexLivingString_setWaveLength(tComplexLivingString* const pl, float waveLength)
{
    _tComplexLivingString* p = *pl;
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.0f) waveLength=2400.0f;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(&p->wlSmooth, p->waveLengthInSamples);
}

void     tComplexLivingString_setPickPos(tComplexLivingString* const pl, float pickPos)
{    // between 0 and 1
    _tComplexLivingString* p = *pl;
    if (pickPos<0.5f) pickPos=0.5f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(&p->pickPosSmooth, p->pickPos);
}

void     tComplexLivingString_setPrepPos(tComplexLivingString* const pl, float prepPos)
{    // between 0 and 1
    _tComplexLivingString* p = *pl;
    if (prepPos<0.f) prepPos=0.f;
    else if (prepPos>0.5f) prepPos=0.5f;
    p->prepPos = prepPos;
    tExpSmooth_setDest(&p->prepPosSmooth, p->prepPos);
}

void     tComplexLivingString_setPrepIndex(tComplexLivingString* const pl, float prepIndex)
{    // between 0 and 1
    _tComplexLivingString* p = *pl;
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tComplexLivingString_setDampFreq(tComplexLivingString* const pl, float dampFreq)
{
    _tComplexLivingString* p = *pl;
    tOnePole_setFreq(&p->bridgeFilter, dampFreq);
    tOnePole_setFreq(&p->nutFilter, dampFreq);
    tOnePole_setFreq(&p->prepFilterU, dampFreq);
    tOnePole_setFreq(&p->prepFilterL, dampFreq);
}

void     tComplexLivingString_setDecay(tComplexLivingString* const pl, float decay)
{
    _tComplexLivingString* p = *pl;
    p->decay=decay;
}

void     tComplexLivingString_setTargetLev(tComplexLivingString* const pl, float targetLev)
{
    _tComplexLivingString* p = *pl;
    tFeedbackLeveler_setTargetLevel(&p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(&p->fbLevL, targetLev);
}

void     tComplexLivingString_setLevSmoothFactor(tComplexLivingString* const pl, float levSmoothFactor)
{
    _tComplexLivingString* p = *pl;
    tFeedbackLeveler_setFactor(&p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(&p->fbLevL, levSmoothFactor);
}

void     tComplexLivingString_setLevStrength(tComplexLivingString* const pl, float levStrength)
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

float   tComplexLivingString_tick(tComplexLivingString* const pl, float input)
{
    _tComplexLivingString* p = *pl;

    // from pickPos upwards=forwards
    float fromLF=tLinearDelay_tickOut(&p->delLF);
    float fromMF=tLinearDelay_tickOut(&p->delMF);
    float fromUF=tLinearDelay_tickOut(&p->delUF);
    float fromUB=tLinearDelay_tickOut(&p->delUB);
    float fromMB=tLinearDelay_tickOut(&p->delMB);
    float fromLB=tLinearDelay_tickOut(&p->delLB);

    // into upper part of string, from bridge, going backwards
    float fromBridge=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(&p->DCblockerU, tOnePole_tick(&p->bridgeFilter, fromUF)));
    tLinearDelay_tickIn(&p->delUB, fromBridge);

    // into pick position, take input and add it into the waveguide, going to come out of middle segment
    tLinearDelay_tickIn(&p->delMB, fromUB+input);

    // into lower part of string, from prepPos, going backwards
    float fromLowerPrep=-tOnePole_tick(&p->prepFilterL, fromLF);
    float intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromMB;
    tLinearDelay_tickIn(&p->delLB, intoLower);

    // into lower part of string, from nut, going forwards toward prep position
    float fromNut=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tOnePole_tick(&p->nutFilter, fromLB)));
    tLinearDelay_tickIn(&p->delLF, fromNut);

    // into middle part of string, from prep going toward pick position
    float fromUpperPrep=-tOnePole_tick(&p->prepFilterU, fromUB);
    float intoMiddle=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF;

    //pick position, take input and add it into the waveguide, going to come out of middle segment
    tLinearDelay_tickIn(&p->delMF, intoMiddle + input);

    //take output of middle segment and put it into upper segment connecting to the bridge
    tLinearDelay_tickIn(&p->delUF, fromMF);

    // update all delay lengths
    float pickP=tExpSmooth_tick(&p->pickPosSmooth);
    float prepP=tExpSmooth_tick(&p->prepPosSmooth);
    float wLen=tExpSmooth_tick(&p->wlSmooth);

    float midLen = (pickP-prepP) * wLen; // the length between the pick and the prep;
    float lowLen = prepP*wLen; // the length from prep to nut
    float upLen = (1.0f-pickP)*wLen; // the length from pick to bridge


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

float   tComplexLivingString_sample(tComplexLivingString* const pl)
{
    _tComplexLivingString* p = *pl;
    return p->curr;
}

void    tComplexLivingString_setSampleRate(tComplexLivingString* const pl, float sr)
{
    _tComplexLivingString* p = *pl;
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

///Reed Table model
//default values from STK are 0.6 offset and -0.8 slope

void    tReedTable_init      (tReedTable* const pm, float offset, float slope, LEAF* const leaf)
{
    tReedTable_initToPool(pm, offset, slope, &leaf->mempool);
}

void    tReedTable_initToPool   (tReedTable* const pm, float offset, float slope, tMempool* const mp)
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
