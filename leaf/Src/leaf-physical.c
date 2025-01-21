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

Lfloat   pickupNonLinearity          (Lfloat x)
{

	x = x * 2.0f;
	Lfloat out = (0.075f * x) + (0.00675f * x * x) +( 0.00211f * x * x * x) + (0.000475f * x * x * x * x) + (0.000831f * x * x * x * x *x);
	out *= 4.366812227074236f;
	return out;
}


void   tPickupNonLinearity_init          (tPickupNonLinearity* const pl, LEAF* const leaf)
{
	tPickupNonLinearity_initToPool(pl, &leaf->mempool);
}
void   tPickupNonLinearity_initToPool          (tPickupNonLinearity* const pl, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPickupNonLinearity* p = *pl = (_tPickupNonLinearity*) mpool_alloc(sizeof(_tPickupNonLinearity), m);
    p->mempool = m;
    p->prev = 0.0f;
}
void   tPickupNonLinearity_free          (tPickupNonLinearity* const pl)
{
	_tPickupNonLinearity* p = *pl;
	mpool_free((char*)p, p->mempool);
}
Lfloat   tPickupNonLinearity_tick          (tPickupNonLinearity const p, Lfloat x)
{
	x = x * 2.0f;
	Lfloat out = (0.075f * x) + (0.00675f * x * x) +( 0.00211f * x * x * x) + (0.000475f * x * x * x * x) + (0.000831f * x * x * x * x *x);
	out *= 4.366812227074236f;
	Lfloat y = out - p->prev;
	return y;
}

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
    tAllpassDelay_clear(p->delayLine);
    
    tPluck_setFrequency(p, 220.0f);
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

Lfloat   tPluck_getLastOut    (tPluck const p)
{
    return p->lastOut;
}

Lfloat   tPluck_tick          (tPluck const p)
{
    return (p->lastOut = 3.0f * tAllpassDelay_tick(p->delayLine, tOneZero_tick(p->loopFilter, tAllpassDelay_getLastOut(p->delayLine) * p->loopGain ) ));
}

void    tPluck_pluck         (tPluck const p, Lfloat amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    tOnePole_setPole(p->pickFilter, 0.999f - (amplitude * 0.15f));
    tOnePole_setGain(p->pickFilter, amplitude * 0.5f );
    
    // Fill delay with noise additively with current contents.
    for ( uint32_t i = 0; i < (uint32_t)tAllpassDelay_getDelay(p->delayLine); i++ )
        tAllpassDelay_tick(p->delayLine, 0.6f * tAllpassDelay_getLastOut(p->delayLine) + tOnePole_tick(p->pickFilter, tNoise_tick(p->noise) ) );
}

// Start a note with the given frequency and amplitude.;
void    tPluck_noteOn        (tPluck const p, Lfloat frequency, Lfloat amplitude )
{
    p->lastFreq = frequency;
    tPluck_setFrequency(p, frequency );
    tPluck_pluck(p, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tPluck_noteOff       (tPluck const p, Lfloat amplitude )
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tPluck_setFrequency  (tPluck const p, Lfloat frequency )
{
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    // Delay = length - filter delay.
    Lfloat delay = ( p->sampleRate / frequency ) - tOneZero_getPhaseDelay(p->loopFilter, frequency );
    
    tAllpassDelay_setDelay(p->delayLine, delay );
    
    p->loopGain = 0.99f + (frequency * 0.000005f);
    
    if (p->loopGain >= 0.999f ) p->loopGain = 0.999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tPluck_controlChange (tPluck const p, int number, Lfloat value)
{
    return;
}

void tPluck_setSampleRate(tPluck const p, Lfloat sr)
{
    p->sampleRate = sr;
    
    tAllpassDelay_free(&p->delayLine);
    tAllpassDelay_initToPool(&p->delayLine, 0.0f, p->sampleRate * 2, &p->mempool);
    tAllpassDelay_clear(p->delayLine);
    
    tPluck_setFrequency(p, p->lastFreq);
    tOnePole_setSampleRate(p->pickFilter, p->sampleRate);
    tOneZero_setSampleRate(p->loopFilter, p->sampleRate);
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
    tAllpassDelay_clear(p->delayLine);
    
    tLinearDelay_initToPool(&p->combDelay, 0.0f, p->sampleRate * 2, mp);
    tLinearDelay_clear(p->combDelay);
    
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
    
    tKarplusStrong_setFrequency( *pl, 220.0f );
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

Lfloat   tKarplusStrong_getLastOut    (tKarplusStrong const p)
{
    return p->lastOut;
}

Lfloat   tKarplusStrong_tick          (tKarplusStrong const p)
{
    Lfloat temp = tAllpassDelay_getLastOut(p->delayLine) * p->loopGain;
    
    // Calculate allpass stretching.
    for (int i=0; i<4; i++)     temp = tBiQuad_tick(p->biquad[i],temp);
    
    // Moving average filter.
    temp = tOneZero_tick(p->filter, temp);
    
    Lfloat out = tAllpassDelay_tick(p->delayLine, temp);
    out = out - tLinearDelay_tick(p->combDelay, out);
    p->lastOut = out;
    
    return p->lastOut;
}

void    tKarplusStrong_pluck         (tKarplusStrong const p, Lfloat amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->pluckAmplitude = amplitude;
    
    for ( uint32_t i=0; i < (uint32_t)tAllpassDelay_getDelay(p->delayLine); i++ )
    {
        // Fill delay with noise additively with current contents.
        tAllpassDelay_tick(p->delayLine, (tAllpassDelay_getLastOut(p->delayLine) * 0.6f) + 0.4f * tNoise_tick(p->noise) * p->pluckAmplitude );
        //delayLine_.tick( combDelay_.tick((delayLine_.lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude_) );
    }
}

// Start a note with the given frequency and amplitude.;
void    tKarplusStrong_noteOn        (tKarplusStrong const p, Lfloat frequency, Lfloat amplitude )
{
    tKarplusStrong_setFrequency(p, frequency );
    tKarplusStrong_pluck(p, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tKarplusStrong_noteOff       (tKarplusStrong const p, Lfloat amplitude )
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tKarplusStrong_setFrequency  (tKarplusStrong const p, Lfloat frequency )
{
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    p->lastFrequency = frequency;
    p->lastLength = p->sampleRate / p->lastFrequency;
    Lfloat delay = p->lastLength - 0.5f;
    tAllpassDelay_setDelay(p->delayLine, delay);
    
    // MAYBE MODIFY LOOP GAINS
    p->loopGain = p->baseLoopGain + (frequency * 0.000005f);
    if (p->loopGain >= 1.0f) p->loopGain = 0.99999f;
    
    tKarplusStrong_setStretch(p, p->stretching);
    
    tLinearDelay_setDelay(p->combDelay, 0.5f * p->pickupPosition * p->lastLength);
}

// Set the stretch "factor" of the string (0.0 - 1.0).
void    tKarplusStrong_setStretch         (tKarplusStrong const p, Lfloat stretch)
{
    p->stretching = stretch;
    Lfloat coefficient;
    Lfloat freq = p->lastFrequency * 2.0f;
    Lfloat dFreq = ( (0.5f * p->sampleRate) - freq ) * 0.25f;
    Lfloat temp = 0.5f + (stretch * 0.5f);
    if ( temp > 0.9999f ) temp = 0.9999f;
    
    for ( int i=0; i<4; i++ )
    {
        coefficient = temp * temp;
        tBiQuad_setA2(p->biquad[i], coefficient);
        tBiQuad_setB0(p->biquad[i], coefficient);
        tBiQuad_setB2(p->biquad[i], 1.0f);
        
        coefficient = -2.0f * temp * cosf(TWO_PI * freq / p->sampleRate);
        tBiQuad_setA1(p->biquad[i], coefficient);
        tBiQuad_setB1(p->biquad[i], coefficient);
        
        freq += dFreq;
    }
}

// Set the pluck or "excitation" position along the string (0.0 - 1.0).
void    tKarplusStrong_setPickupPosition  (tKarplusStrong const p, Lfloat position )
{
    if (position < 0.0f)        p->pickupPosition = 0.0f;
    else if (position <= 1.0f)  p->pickupPosition = position;
    else                        p->pickupPosition = 1.0f;
    
    tLinearDelay_setDelay(p->combDelay, 0.5f * p->pickupPosition * p->lastLength);
}

// Set the base loop gain.
void    tKarplusStrong_setBaseLoopGain    (tKarplusStrong const p, Lfloat aGain )
{
    p->baseLoopGain = aGain;
    p->loopGain = p->baseLoopGain + (p->lastFrequency * 0.000005f);
    if ( p->loopGain > 0.99999f ) p->loopGain = 0.99999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tKarplusStrong_controlChange (tKarplusStrong const p, SKControlType type, Lfloat value)
{
    if ( value < 0.0f )         value = 0.0f;
    else if (value > 128.0f)   value = 128.0f;
    
    Lfloat normalizedValue = value * INV_128;
    
    if (type == SKPickPosition) // 4
        tKarplusStrong_setPickupPosition(p, normalizedValue );
    else if (type == SKStringDamping) // 11
        tKarplusStrong_setBaseLoopGain(p, 0.97f + (normalizedValue * 0.03f) );
    else if (type == SKDetune) // 1
        tKarplusStrong_setStretch(p, 0.91f + (0.09f * (1.0f - normalizedValue)) );
}

void    tKarplusStrong_setSampleRate (tKarplusStrong const p, Lfloat sr)
{
    p->sampleRate = sr;
    
    tAllpassDelay_free(&p->delayLine);
    tAllpassDelay_initToPool(&p->delayLine, 0.0f, p->sampleRate * 2, &p->mempool);
    tAllpassDelay_clear(p->delayLine);
    
    tLinearDelay_free(&p->combDelay);
    tLinearDelay_initToPool(&p->combDelay, 0.0f, p->sampleRate * 2, &p->mempool);
    tLinearDelay_clear(p->combDelay);
    
    tKarplusStrong_setFrequency(p, p->lastFrequency);
    tOneZero_setSampleRate(p->filter, p->sampleRate);
    
    for (int i = 0; i < 4; i++)
    {
        tBiQuad_setSampleRate(p->biquad[i], p->sampleRate);
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
    tSimpleLivingString_setFreq(*pl, freq);
    tLinearDelay_initToPool(&p->delayLine,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_clear(p->delayLine);
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

void     tSimpleLivingString_setFreq(tSimpleLivingString const p, Lfloat freq)
{
    if (freq<20) freq=20;
    else if (freq>10000) freq=10000;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString_setWaveLength(tSimpleLivingString const p, Lfloat waveLength)
{
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>2400) waveLength=2400;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString_setDampFreq(tSimpleLivingString const p, Lfloat dampFreq)
{
    tOnePole_setFreq(p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString_setDecay(tSimpleLivingString const p, Lfloat decay)
{
    p->decay=decay;
}

void     tSimpleLivingString_setTargetLev(tSimpleLivingString const p, Lfloat targetLev)
{
    tFeedbackLeveler_setTargetLevel(p->fbLev, targetLev);
}

void     tSimpleLivingString_setLevSmoothFactor(tSimpleLivingString const p, Lfloat levSmoothFactor)
{
    tFeedbackLeveler_setFactor(p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString_setLevStrength(tSimpleLivingString const p, Lfloat levStrength)
{
    tFeedbackLeveler_setStrength(p->fbLev, levStrength);
}

void     tSimpleLivingString_setLevMode(tSimpleLivingString const p, int levMode)
{
    tFeedbackLeveler_setMode(p->fbLev, levMode);
    p->levMode=levMode;
}

Lfloat   tSimpleLivingString_tick(tSimpleLivingString const p, Lfloat input)
{
    Lfloat stringOut=tOnePole_tick(p->bridgeFilter,tLinearDelay_tickOut(p->delayLine));
    Lfloat stringInput=tHighpass_tick(p->DCblocker, tFeedbackLeveler_tick(p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    tLinearDelay_tickIn(p->delayLine, stringInput);
    tLinearDelay_setDelay(p->delayLine, tExpSmooth_tick(p->wlSmooth));
    p->curr = stringOut;
    return p->curr;
}

Lfloat   tSimpleLivingString_sample(tSimpleLivingString const p)
{
    return p->curr;
}

void   tSimpleLivingString_setSampleRate(tSimpleLivingString const p, Lfloat sr)
{
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(p->DCblocker, p->sampleRate);
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
    tSimpleLivingString2_setFreq(*pl, freq);
    tHermiteDelay_initToPool(&p->delayLine,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_clear(p->delayLine);
    tTwoZero_initToPool(&p->bridgeFilter, mp);
    tSimpleLivingString2_setBrightness(*pl, brightness);
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

void     tSimpleLivingString2_setFreq(tSimpleLivingString2 const p, Lfloat freq)
{
    if (freq<20) freq=20;
    else if (freq>10000) freq=10000;
    p->waveLengthInSamples = p->sampleRate/freq -1;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString2_setWaveLength(tSimpleLivingString2 const p, Lfloat waveLength)
{
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>2400) waveLength=2400;
    p->waveLengthInSamples = waveLength-1;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString2_setBrightness(tSimpleLivingString2 const p, Lfloat brightness)
{
    Lfloat h0 = (1.0f + brightness) * 0.5f;
    Lfloat h1 = (1.0f - brightness) * 0.25f;
    tTwoZero_setCoefficients(p->bridgeFilter, h1, h0, h1);
}

void     tSimpleLivingString2_setDecay(tSimpleLivingString2 const p, Lfloat decay)
{
    p->decay=decay;
}

void     tSimpleLivingString2_setTargetLev(tSimpleLivingString2 const p, Lfloat targetLev)
{
    tFeedbackLeveler_setTargetLevel(p->fbLev, targetLev);
}

void     tSimpleLivingString2_setLevSmoothFactor(tSimpleLivingString2 const p, Lfloat levSmoothFactor)
{
    tFeedbackLeveler_setFactor(p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString2_setLevStrength(tSimpleLivingString2 const p, Lfloat levStrength)
{
    tFeedbackLeveler_setStrength(p->fbLev, levStrength);
}

void     tSimpleLivingString2_setLevMode(tSimpleLivingString2 const p, int levMode)
{
    tFeedbackLeveler_setMode(p->fbLev, levMode);
    p->levMode=levMode;
}

Lfloat   tSimpleLivingString2_tick(tSimpleLivingString2 const p, Lfloat input)
{
    Lfloat stringOut=tTwoZero_tick(p->bridgeFilter,tHermiteDelay_tickOut(p->delayLine));
    Lfloat stringInput=tHighpass_tick(p->DCblocker,(tFeedbackLeveler_tick(p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input)));
    tHermiteDelay_tickIn(p->delayLine, stringInput);
    tHermiteDelay_setDelay(p->delayLine, tExpSmooth_tick(p->wlSmooth));
    p->curr = stringOut;
    return p->curr;
}


Lfloat   tSimpleLivingString2_sample(tSimpleLivingString2 const p)
{
    return p->curr;
}

void   tSimpleLivingString2_setSampleRate(tSimpleLivingString2 const p, Lfloat sr)
{
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    tTwoZero_setSampleRate(p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(p->DCblocker, p->sampleRate);
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
    tLivingString_setFreq(*pl, freq);
    p->freq = freq;
    tExpSmooth_initToPool(&p->ppSmooth, pickPos, 0.01f, mp); // smoother for pick position
    tLivingString_setPickPos(p, pickPos);
    p->prepIndex=prepIndex;
    tLinearDelay_initToPool(&p->delLF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delLB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_clear(p->delLF);
    tLinearDelay_clear(p->delUF);
    tLinearDelay_clear(p->delUB);
    tLinearDelay_clear(p->delLB);
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
    tSimpleLivingString3_setFreq(*pl, freq);
    tLinearDelay_setDelay(p->delayLineU, p->waveLengthInSamples);
    tLinearDelay_setDelay(p->delayLineL, p->waveLengthInSamples);
    //tSimpleLivingString3_setWaveLength(pl, 4800);
    tLinearDelay_clear(p->delayLineU);
    tLinearDelay_clear(p->delayLineL);
    p->dampFreq = dampFreq;
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tOnePole_setSampleRate(p->bridgeFilter, p->sampleRate);
    tOnePole_setFreq(p->bridgeFilter, dampFreq);
    p->rippleGain = 0.0f;
    p->rippleDelay = 0.5f;
    p->invOnePlusr = 1.0f;
    tHighpass_initToPool(&p->DCblocker,13, mp);
    tHighpass_setSampleRate(p->DCblocker,p->sampleRate);
    tHighpass_setFreq(p->DCblocker,13);
    p->userDecay = decay;

    tFeedbackLeveler_initToPool(&p->fbLev, targetLev, levSmoothFactor, levStrength, levMode, mp);
    p->levMode=levMode;
    p->changeGainCompensator = 1.0f;

    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);

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
    tOnePole_free(&p->bridgeFilter);
    tLinearDelay_free(&p->delayLineL);
    tLinearDelay_free(&p->delayLineU);

    tExpSmooth_free(&p->wlSmooth);

    
    mpool_free((char*)p, p->mempool);
}

void     tSimpleLivingString3_setFreq(tSimpleLivingString3 const p, Lfloat freq)
{
    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    //Lfloat decay = p->userDecay;

    Lfloat temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);

    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
}

void     tSimpleLivingString3_setWaveLength(tSimpleLivingString3 const p, Lfloat waveLength)
{
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString3_setDampFreq(tSimpleLivingString3 const p, Lfloat dampFreq)
{
    //p->temp1 = (1.0f - brightness) * 0.25f * p->decay;
    //p->temp2 = (1.0f + brightness) * 0.5f * p->decay;
    //tBiQuad_setCoefficients(&p->bridgeFilter2, p->temp1 , p->temp2, p->temp1 , 0.0f, 0.0f);

    tOnePole_setFreq(p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString3_setDecay(tSimpleLivingString3 const p, Lfloat decay)
{
    p->userDecay = decay;

    Lfloat temp = ((decay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.9078 * ((1.0 / p->freq)  / temp));


    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);
    //p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tSimpleLivingString3_setTargetLev(tSimpleLivingString3 const p, Lfloat targetLev)
{
    tFeedbackLeveler_setTargetLevel(p->fbLev, targetLev);
}

void     tSimpleLivingString3_setLevSmoothFactor(tSimpleLivingString3 const p, Lfloat levSmoothFactor)
{
    tFeedbackLeveler_setFactor(p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString3_setLevStrength(tSimpleLivingString3 const p, Lfloat levStrength)
{
    tFeedbackLeveler_setStrength(p->fbLev, levStrength);
}

void     tSimpleLivingString3_setLevMode(tSimpleLivingString3 const p, int levMode)
{
    tFeedbackLeveler_setMode(p->fbLev, levMode);
    p->levMode=levMode;
}

void   tSimpleLivingString3_pluck(tSimpleLivingString3 const p, Lfloat input, Lfloat position)
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

Lfloat   tSimpleLivingString3_tick(tSimpleLivingString3 const p, Lfloat input)
{
    //p->changeGainCompensator = 1.0f;
    Lfloat wl = tExpSmooth_tick(p->wlSmooth);
    //Lfloat changeInDelayTime = wl - p->prevDelayLength;
    //if (changeInDelayTime < 0.0f)
    {
    //	p->changeGainCompensator = sqrtf(1.0f - changeInDelayTime);
    }
    tLinearDelay_setDelay(p->delayLineU, wl);
    tLinearDelay_setDelay(p->delayLineL, wl);
    
    for (int i = 0; i < p->oversampling; i++)
    {
		p->Uout = tHighpass_tick(p->DCblocker,tOnePole_tick(p->bridgeFilter,tLinearDelay_tickOut(p->delayLineU))* (p->decay * p->invOnePlusr));
		p->Uout = LEAF_clip(-1.0f, tFeedbackLeveler_tick(p->fbLev, p->Uout), 1.0f);
    	//p->Uout = tLinearDelay_tickOut(&p->delayLineU) * p->decay;
		p->Lout = LEAF_clip(-1.0f, tLinearDelay_tickOut(p->delayLineL), 1.0f);

		tLinearDelay_tickIn(p->delayLineU, (-1.0f * p->Lout) + input);
		tLinearDelay_tickIn(p->delayLineL, -1.0f * p->Uout);
		tLinearDelay_addTo (p->delayLineU, p->Lout * p->rippleGain, p->rippleDelay*wl);
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


void   tSimpleLivingString3_setPickupPoint(tSimpleLivingString3 const p, Lfloat pickupPoint)
{
    p->pickupPoint = pickupPoint;
}


Lfloat   tSimpleLivingString3_sample(tSimpleLivingString3 const p)
{
    return p->curr;
}

void   tSimpleLivingString3_setSampleRate(tSimpleLivingString3 const p, Lfloat sr)
{
    p->sampleRate = sr*p->oversampling;
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;

    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(p->DCblocker, p->sampleRate);
    tBiQuad_setSampleRate(p->bridgeFilter2, p->sampleRate);
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
    tSimpleLivingString4_setFreq(p, freq);
    tLinearDelay_setDelay(p->delayLineU, p->waveLengthInSamples);
    tLinearDelay_setDelay(p->delayLineL, p->waveLengthInSamples);
    //tSimpleLivingString4_setWaveLength(pl, 4800);
    tLinearDelay_clear(p->delayLineU);
    tLinearDelay_clear(p->delayLineL);
    p->dampFreq = dampFreq;
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tOnePole_setSampleRate(p->bridgeFilter, p->sampleRate);
    tOnePole_setFreq(p->bridgeFilter, dampFreq);
    p->rippleGain = 0.0f;
    p->rippleDelay = 0.5f;

    tBiQuad_initToPool(&p->bridgeFilter2, mp);
    tBiQuad_setSampleRate(p->bridgeFilter2, p->sampleRate);

    tHighpass_initToPool(&p->DCblocker,0.001f, mp);
    tHighpass_setSampleRate(p->DCblocker,p->sampleRate);
    tHighpass_setFreq(p->DCblocker,0.001f);
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
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);

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

void     tSimpleLivingString4_setFreq(tSimpleLivingString4 const p, Lfloat freq)
{
    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    //Lfloat decay = p->userDecay;

    Lfloat temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);

    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
}

void     tSimpleLivingString4_setWaveLength(tSimpleLivingString4 const p, Lfloat waveLength)
{
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength = p->maxLength*2;
    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString4_setDampFreq(tSimpleLivingString4 const p, Lfloat dampFreq)
{
    //p->temp1 = (1.0f - brightness) * 0.25f * p->decay;
    //p->temp2 = (1.0f + brightness) * 0.5f * p->decay;
    //tBiQuad_setCoefficients(&p->bridgeFilter2, p->temp1 , p->temp2, p->temp1 , 0.0f, 0.0f);

    tOnePole_setFreq(p->bridgeFilter, dampFreq);
}

void     tSimpleLivingString4_setDecay(tSimpleLivingString4 const p, Lfloat decay)
{
    p->userDecay = decay;

    Lfloat temp = ((decay * 0.01f) + 0.01f) * 6.9078f;
    p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);
    //p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tSimpleLivingString4_setTargetLev(tSimpleLivingString4 const p, Lfloat targetLev)
{
    tFeedbackLeveler_setTargetLevel(p->fbLev, targetLev);
}

void     tSimpleLivingString4_setLevSmoothFactor(tSimpleLivingString4 const p, Lfloat levSmoothFactor)
{
    tFeedbackLeveler_setFactor(p->fbLev, levSmoothFactor);
}

void     tSimpleLivingString4_setLevStrength(tSimpleLivingString4 const p, Lfloat levStrength)
{
    tFeedbackLeveler_setStrength(p->fbLev, levStrength);
}

void     tSimpleLivingString4_setLevMode(tSimpleLivingString4 const p, int levMode)
{
    tFeedbackLeveler_setMode(p->fbLev, levMode);
    p->levMode=levMode;
}

void   tSimpleLivingString4_setPluckPosition(tSimpleLivingString4 const p, Lfloat position)
{
    p->pluckPosition = position;
}


void   tSimpleLivingString4_pluck(tSimpleLivingString4 const p, Lfloat input, Lfloat position)
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

void   tSimpleLivingString4_pluckNoPosition(tSimpleLivingString4 const p, Lfloat input)
{
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



Lfloat   tSimpleLivingString4_tick(tSimpleLivingString4 const p, Lfloat input)
{
    p->changeGainCompensator = 1.0f;
    Lfloat wl = tExpSmooth_tick(p->wlSmooth);
    volatile Lfloat changeInDelayTime = -0.01875f*(wl*0.5f - p->prevDelayLength*0.5f);
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
		p->Uout = tHighpass_tick(p->DCblocker,tOnePole_tick(p->bridgeFilter,tLinearDelay_tickOut(p->delayLineU))* (p->decay - fabsf(p->rippleGain)));
		p->Uout = LEAF_clip(-1.0f, tFeedbackLeveler_tick(p->fbLev, p->Uout), 1.0f);
    	//p->Uout = tLinearDelay_tickOut(&p->delayLineU) * p->decay;
		p->Lout = LEAF_clip(-1.0f, tLinearDelay_tickOut(p->delayLineL), 1.0f);

		tLinearDelay_tickIn(p->delayLineU, (-1.0f * p->Lout)* p->changeGainCompensator);
		tLinearDelay_tickIn(p->delayLineL, (-1.0f * p->Uout));
        tLinearDelay_addTo (p->delayLineU, input, p->pluckPosition*wl);
        tLinearDelay_addTo (p->delayLineL, input, (1.0f - p->pluckPosition)*wl);
		tLinearDelay_addTo (p->delayLineU, p->Lout * p->rippleGain, p->rippleDelay*wl);
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


void   tSimpleLivingString4_setPickupPoint(tSimpleLivingString4 const p, Lfloat pickupPoint)
{
    p->pickupPoint = pickupPoint;
}


Lfloat   tSimpleLivingString4_sample(tSimpleLivingString4 const p)
{
    return p->curr;
}

void   tSimpleLivingString4_setSampleRate(tSimpleLivingString4 const p, Lfloat sr)
{
    p->sampleRate = sr*p->oversampling;
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;

    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(p->DCblocker, p->sampleRate);
    tBiQuad_setSampleRate(p->bridgeFilter2, p->sampleRate);
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
    tSimpleLivingString5_setFreq(p, freq);
    tLagrangeDelay_setDelay(p->delUF, p->waveLengthInSamples-(p->prepPos*p->waveLengthInSamples));
    tLagrangeDelay_setDelay(p->delUB, p->waveLengthInSamples-((1.0f-p->prepPos)*p->waveLengthInSamples));
    tLagrangeDelay_setDelay(p->delLF, p->waveLengthInSamples-(p->prepPos*p->waveLengthInSamples));
    tLagrangeDelay_setDelay(p->delLB, p->waveLengthInSamples-((1.0f-p->prepPos)*p->waveLengthInSamples));
    //tSimpleLivingString5_setWaveLength(pl, 4800);
    tLagrangeDelay_clear(p->delUF);
    tLagrangeDelay_clear(p->delUB);
    tLagrangeDelay_clear(p->delLF);
    tLagrangeDelay_clear(p->delLB);
    p->dampFreq = dampFreq;
    tOnePole_initToPool(&p->bridgeFilter, dampFreq, mp);
    tOnePole_setSampleRate(p->bridgeFilter, p->sampleRate);
    tOnePole_setFreq(p->bridgeFilter, dampFreq);

    tOnePole_initToPool(&p->nutFilter, dampFreq, mp);
    tOnePole_setSampleRate(p->nutFilter, p->sampleRate);
    tOnePole_setFreq(p->nutFilter, dampFreq);




    p->rippleGain = 0.0f;
    p->rippleDelay = 0.5f;


    tHighpass_initToPool(&p->DCblocker,0.001f, mp);
    tHighpass_setSampleRate(p->DCblocker,p->sampleRate);
    tHighpass_setFreq(p->DCblocker,0.001f);
    
    tHighpass_initToPool(&p->DCblocker2,0.001f, mp);
    tHighpass_setSampleRate(p->DCblocker2,p->sampleRate);
    tHighpass_setFreq(p->DCblocker2,0.001f);
    
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
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);

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

void     tSimpleLivingString5_setFreq(tSimpleLivingString5 const p, Lfloat freq)
{
    p->freq = freq;
    Lfloat waveLength = (p->sampleRate/freq);
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples =  waveLength * 0.5f;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    //Lfloat decay = p->userDecay;

    //Lfloat temp = ((p->userDecay * 0.01f) + 0.01f) * 6.9078f;
    //p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);

    //p->decay=powf(0.001f,1.0f/(p->freq*p->userDecay));
}

void     tSimpleLivingString5_setWaveLength(tSimpleLivingString5 const p, Lfloat waveLength)
{
    if (waveLength<4.8) waveLength=4.8f;
    else if (waveLength>p->maxLength*2) waveLength=p->maxLength*2;
    p->waveLengthInSamples = waveLength * 0.5f;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tSimpleLivingString5_setDampFreq(tSimpleLivingString5 const p, Lfloat dampFreq)
{
    //p->temp1 = (1.0f - brightness) * 0.25f * p->decay;
    //p->temp2 = (1.0f + brightness) * 0.5f * p->decay;
    //tBiQuad_setCoefficients(&p->bridgeFilter2, p->temp1 , p->temp2, p->temp1 , 0.0f, 0.0f);

    tOnePole_setFreq(p->bridgeFilter, dampFreq);
    tOnePole_setFreq(p->nutFilter, dampFreq);
}

void     tSimpleLivingString5_setDecay(tSimpleLivingString5 const p, Lfloat decay)
{
    p->userDecay = decay;

    
    //Lfloat temp = ((decay * 0.01f) + 0.01f) * 6.9078f;
    //p->decay = exp(-6.91 * ((1.0 / p->freq)  / temp));
    p->decay = decay;//;
    //tBiQuad_setCoefficients(&p->bridgeFilter2,p->temp1,p->temp2, p->temp1, 0.0f, 0.0f);
    //p->decay=powf(0.001f,1.0f/(p->freq*decay));
}
void   tSimpleLivingString5_setPrepPosition(tSimpleLivingString5 const p, Lfloat prepPosition)
{
    p->prepPos = prepPosition;
    tExpSmooth_setDest(p->prepPosSmooth, prepPosition);
}
void   tSimpleLivingString5_setPrepIndex(tSimpleLivingString5 const p, Lfloat prepIndex)
{
     p->prepIndex = prepIndex;
    tExpSmooth_setDest(p->prepIndexSmooth, prepIndex);
}

void     tSimpleLivingString5_setTargetLev(tSimpleLivingString5 const p, Lfloat targetLev)
{
    tFeedbackLeveler_setTargetLevel(p->fbLev, targetLev);
    tFeedbackLeveler_setTargetLevel(p->fbLev2, targetLev);
}

void     tSimpleLivingString5_setLevSmoothFactor(tSimpleLivingString5 const p, Lfloat levSmoothFactor)
{
    tFeedbackLeveler_setFactor(p->fbLev, levSmoothFactor);
    tFeedbackLeveler_setFactor(p->fbLev2, levSmoothFactor);
}

void     tSimpleLivingString5_setLevStrength(tSimpleLivingString5 const p, Lfloat levStrength)
{
    tFeedbackLeveler_setStrength(p->fbLev, levStrength);
    tFeedbackLeveler_setStrength(p->fbLev2, levStrength);
}

void     tSimpleLivingString5_setLevMode(tSimpleLivingString5 const p, int levMode)
{
    tFeedbackLeveler_setMode(p->fbLev, levMode);
    tFeedbackLeveler_setMode(p->fbLev2, levMode);
    p->levMode=levMode;
}

void   tSimpleLivingString5_setPluckPosition(tSimpleLivingString5 const p, Lfloat position)
{
    p->pluckPosition = position;
    tExpSmooth_setDest(p->pluckPosSmooth, position);
    
}


void   tSimpleLivingString5_pluck(tSimpleLivingString5 const p, Lfloat input, Lfloat position)
{
    //p->pluckPosition = position;
    p->pluckPosition = 0.5f;
    tExpSmooth_setDest(p->pluckPosSmooth, position);
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





Lfloat   tSimpleLivingString5_tick(tSimpleLivingString5 const p, Lfloat input)
{
    //p->changeGainCompensator = 1.0f;
    Lfloat wl = tExpSmooth_tick(p->wlSmooth);

    //volatile Lfloat changeInDelayTime = -0.01875f*(wl*0.5f - p->prevDelayLength*0.5f);
    //if (changeInDelayTime < -0.1f)
    {
    //	p->changeGainCompensator = sqrtf(1.0f - changeInDelayTime);
    }
    //else
    {
    //    p->changeGainCompensator = 1.0f;
    }
    Lfloat prepPosSmoothed = tExpSmooth_tick(p->prepPosSmooth);
    Lfloat FLen = wl*(1.0f-prepPosSmoothed);
    uint32_t FLenInt = (uint32_t)FLen;
    Lfloat BLen = wl*prepPosSmoothed;
    uint32_t BLenInt = (uint32_t)BLen;
    tLagrangeDelay_setDelay(p->delUF, FLen);
    tLagrangeDelay_setDelay(p->delUB, BLen);
    tLagrangeDelay_setDelay(p->delLF, FLen);
    tLagrangeDelay_setDelay(p->delLB, BLen);
    Lfloat pluckPosSmoothed = tExpSmooth_tick(p->pluckPosSmooth);
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
        
        
        Lfloat fromUF=tLagrangeDelay_tickOut(p->delUF);
        Lfloat fromLF=tLagrangeDelay_tickOut(p->delLF);
        Lfloat fromLB=tLagrangeDelay_tickOut(p->delLB);
        Lfloat fromUB=tLagrangeDelay_tickOut(p->delUB);
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
        tLagrangeDelay_tickIn(p->delLF, fromBridge);
        // into back half of string, from prepPoint, going backwards (lower section)
        //Lfloat fromUpperPrep=-tOnePole_tick(&p->prepFilterL, fromUB);
        Lfloat fromUpperPrep=-fromUB;
        //fromUpperPrep = LEAF_clip(-1.0f, fromUpperPrep, 1.0f);
        Lfloat intoLower=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF; //used to add input here
        //intoLower = LEAF_clip(-1.0f, intoLower, 1.0f);
        tLagrangeDelay_tickIn(p->delLB, intoLower);
        // into back of string, from nut going forward (upper section)
        //Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLev2, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblocker2,fromLB));
        //Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLev2, (p->levMode==0?p->decay:1.0f)*fromLB);
        Lfloat fromNut=-p->decay*fromLB;//tOnePole_tick(&p->nutFilter, fromLB);
        //Lfloat fromNut=-p->decay*tHighpass_tick(&p->DCblocker2,fromLB)); //version without feedback leveler
        //fromNut = LEAF_clip(-1.0f, fromNut, 1.0f);
        tLagrangeDelay_tickIn(p->delUB, fromNut);
        // into front half of string, from prepPoint, going forward (upper section)
        //Lfloat fromLowerPrep=-tOnePole_tick(&p->prepFilterU, fromLF);
        Lfloat fromLowerPrep=-fromLF;
        //fromLowerPrep = LEAF_clip(-1.0f, fromLowerPrep, 1.0f);
        Lfloat intoUpperFront=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB;
        //intoUpper = tanhf(intoUpper);
        //intoUpperFront = LEAF_clip(-1.0f, intoUpperFront, 1.0f);
        tLagrangeDelay_tickIn(p->delUF, intoUpperFront);

        p->curr = fromBridge;
    }

    //pick up the signal
    Lfloat pickupPointSmoothed = tExpSmooth_tick(p->pickupPointSmooth);
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

    //Lfloat stringInput=tHighpass_tick(&p->DCblocker, tFeedbackLeveler_tick(&p->fbLev, (p->levMode==0?p->decay*stringOut:stringOut)+input));
    //tLinearDelay_tickIn(&p->delayLine, stringInput);
    //tLinearDelay_setDelay(&p->delayLine, tExpSmooth_tick(&p->wlSmooth));
    //p->curr = d1 + d2;
    return p->curr;
}


void   tSimpleLivingString5_setPickupPoint(tSimpleLivingString5 const p, Lfloat pickupPoint)
{
    p->pickupPoint = pickupPoint;
    tExpSmooth_setDest(p->pickupPointSmooth, pickupPoint);
}


Lfloat   tSimpleLivingString5_sample(tSimpleLivingString5 const p)
{
    return p->curr;
}

void   tSimpleLivingString5_setSampleRate(tSimpleLivingString5 const p, Lfloat sr)
{
    p->sampleRate = sr*p->oversampling;
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;

    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(p->bridgeFilter, p->sampleRate);
    tHighpass_setSampleRate(p->DCblocker, p->sampleRate);
}


void   tSimpleLivingString5_setFFAmount(tSimpleLivingString5 const p, Lfloat ff)
{
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

void     tLivingString_setFreq(tLivingString const p, Lfloat freq)
{    // NOTE: It is faster to set wavelength in samples directly
    if (freq<20.f) freq=20.f;
    else if (freq>10000.f) freq=10000.f;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString_setWaveLength(tLivingString const p, Lfloat waveLength)
{
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.f) waveLength=2400.f;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString_setPickPos(tLivingString const p, Lfloat pickPos)
{    // between 0 and 1
    if (pickPos<0.f) pickPos=0.f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(p->ppSmooth, p->pickPos);
}

void     tLivingString_setPrepIndex(tLivingString const p, Lfloat prepIndex)
{    // between 0 and 1
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tLivingString_setDampFreq(tLivingString const p, Lfloat dampFreq)
{
    tOnePole_setFreq(p->bridgeFilter, dampFreq);
    tOnePole_setFreq(p->nutFilter, dampFreq);
    tOnePole_setFreq(p->prepFilterU, dampFreq);
    tOnePole_setFreq(p->prepFilterL, dampFreq);
}

void     tLivingString_setDecay(tLivingString const p, Lfloat decay)
{
    p->decay=decay;
}

void     tLivingString_setTargetLev(tLivingString const p, Lfloat targetLev)
{
    tFeedbackLeveler_setTargetLevel(p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(p->fbLevL, targetLev);
}

void     tLivingString_setLevSmoothFactor(tLivingString const p, Lfloat levSmoothFactor)
{
    tFeedbackLeveler_setFactor(p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(p->fbLevL, levSmoothFactor);
}

void     tLivingString_setLevStrength(tLivingString const p, Lfloat levStrength)
{
    tFeedbackLeveler_setStrength(p->fbLevU, levStrength);
    tFeedbackLeveler_setStrength(p->fbLevL, levStrength);
}

void     tLivingString_setLevMode(tLivingString const p, int levMode)
{
    tFeedbackLeveler_setMode(p->fbLevU, levMode);
    tFeedbackLeveler_setMode(p->fbLevL, levMode);
    p->levMode=levMode;
}

Lfloat   tLivingString_tick(tLivingString const p, Lfloat input)
{
    // from pickPos upwards=forwards
    Lfloat fromLF=tLinearDelay_tickOut(p->delLF);
    Lfloat fromUF=tLinearDelay_tickOut(p->delUF);
    Lfloat fromUB=tLinearDelay_tickOut(p->delUB);
    Lfloat fromLB=tLinearDelay_tickOut(p->delLB);
    // into upper half of string, from nut, going backwards
    Lfloat fromNut=-tFeedbackLeveler_tick(p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(p->DCblockerU, tOnePole_tick(p->nutFilter, fromUF)));
    tLinearDelay_tickIn(p->delUB, fromNut);
    // into lower half of string, from pickpoint, going backwards
    Lfloat fromLowerPrep=-tOnePole_tick(p->prepFilterL, fromLF);
    Lfloat intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB+input;
    tLinearDelay_tickIn(p->delLB, intoLower);
    // into lower half of string, from bridge
    Lfloat fromBridge=-tFeedbackLeveler_tick(p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(p->DCblockerL, tOnePole_tick(p->bridgeFilter, fromLB)));
    tLinearDelay_tickIn(p->delLF, fromBridge);
    // into upper half of string, from pickpoint, going forwards/upwards
    Lfloat fromUpperPrep=-tOnePole_tick(p->prepFilterU, fromUB);
    Lfloat intoUpper=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF+input;
    tLinearDelay_tickIn(p->delUF, intoUpper);
    // update all delay lengths
    Lfloat pickP=tExpSmooth_tick(p->ppSmooth);
    Lfloat wLen=tExpSmooth_tick(p->wlSmooth);
    Lfloat lowLen=pickP*wLen;
    Lfloat upLen=(1.0f-pickP)*wLen;
    tLinearDelay_setDelay(p->delLF, lowLen);
    tLinearDelay_setDelay(p->delLB, lowLen);
    tLinearDelay_setDelay(p->delUF, upLen);
    tLinearDelay_setDelay(p->delUB, upLen);
    p->curr = fromBridge;
    return p->curr;
}

Lfloat   tLivingString_sample(tLivingString const p)
{
    return p->curr;
}

void   tLivingString_setSampleRate(tLivingString const p, Lfloat sr)
{
    Lfloat freq = p->sampleRate/p->waveLengthInSamples;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(p->bridgeFilter, p->sampleRate);
    tOnePole_setSampleRate(p->nutFilter, p->sampleRate);
    tOnePole_setSampleRate(p->prepFilterU, p->sampleRate);
    tOnePole_setSampleRate(p->prepFilterL, p->sampleRate);
    tHighpass_setSampleRate(p->DCblockerU, p->sampleRate);
    tHighpass_setSampleRate(p->DCblockerL, p->sampleRate);
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
    tLivingString2_setFreq(*pl, freq);
    p->freq = freq;
    p->prepPos = prepPos;
    tExpSmooth_initToPool(&p->ppSmooth, pickPos, 0.01f, mp); // smoother for pick position
    tExpSmooth_initToPool(&p->prpSmooth, prepPos, 0.01f, mp); // smoother for prep position
    tExpSmooth_initToPool(&p->puSmooth, pickupPos, 0.01f, mp); // smoother for pickup position
    tLivingString2_setPickPos(*pl, pickPos);
    tLivingString2_setPrepPos(*pl, prepPos);
    p->prepIndex = prepIndex;
    p->pickupPos = pickupPos;
    tHermiteDelay_initToPool(&p->delLF,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_initToPool(&p->delUF,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_initToPool(&p->delUB,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_initToPool(&p->delLB,p->waveLengthInSamples, 2400, mp);
    tHermiteDelay_clear(p->delLF);
    tHermiteDelay_clear(p->delUF);
    tHermiteDelay_clear(p->delUB);
    tHermiteDelay_clear(p->delLB);
    p->brightness = brightness;
    tTwoZero_initToPool(&p->bridgeFilter, mp);
    tTwoZero_initToPool(&p->nutFilter, mp);
    tTwoZero_initToPool(&p->prepFilterU, mp);
    tTwoZero_initToPool(&p->prepFilterL, mp);
    tLivingString2_setBrightness(p, brightness);
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

void     tLivingString2_setFreq(tLivingString2 const p, Lfloat freq)
{    // NOTE: It is faster to set wavelength in samples directly
    if (freq<20.f) freq=20.f;
    else if (freq>10000.f) freq=10000.f;
    freq = freq*2;
    p->freq = freq;
    p->waveLengthInSamples = (p->sampleRate/p->freq) - 1;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString2_setWaveLength(tLivingString2 const p, Lfloat waveLength)
{
    waveLength = waveLength * 0.5f;
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.f) waveLength=2400.f;
    p->waveLengthInSamples = waveLength - 1;
    p->freq = p->sampleRate / waveLength;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tLivingString2_setPickPos(tLivingString2 const p, Lfloat pickPos)
{    // between 0 and 1
    if (pickPos<0.f) pickPos=0.f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(p->ppSmooth, p->pickPos);
}

void     tLivingString2_setPrepPos(tLivingString2 const p, Lfloat prepPos)
{    // between 0 and 1
    if (prepPos<0.f) prepPos=0.f;
    else if (prepPos>1.f) prepPos=1.f;
    p->prepPos = prepPos;
    tExpSmooth_setDest(p->prpSmooth, p->prepPos);
}

void     tLivingString2_setPickupPos(tLivingString2 const p, Lfloat pickupPos)
{    // between 0 and 1
    if (pickupPos<0.f) pickupPos=0.f;
    else if (pickupPos>1.f) pickupPos=1.f;
    p->pickupPos = pickupPos;
    tExpSmooth_setDest(p->puSmooth, p->pickupPos);
}

void     tLivingString2_setPrepIndex(tLivingString2 const p, Lfloat prepIndex)
{    // between 0 and 1
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tLivingString2_setBrightness(tLivingString2 const p, Lfloat brightness)
{
    Lfloat h0=(1.0 + brightness) * 0.5f;
    Lfloat h1=(1.0 - brightness) * 0.25f;

    tTwoZero_setCoefficients(p->bridgeFilter, h1, h0, h1);
    tTwoZero_setCoefficients(p->nutFilter, h1, h0, h1);
    tTwoZero_setCoefficients(p->prepFilterU, h1, h0, h1);
    tTwoZero_setCoefficients(p->prepFilterL, h1, h0, h1);
}

void     tLivingString2_setDecay(tLivingString2 const p, Lfloat decay)
{
    p->decay=powf(0.001f,1.0f/(p->freq*decay));
}

void     tLivingString2_setTargetLev(tLivingString2 const p, Lfloat targetLev)
{
    tFeedbackLeveler_setTargetLevel(p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(p->fbLevL, targetLev);
}

void     tLivingString2_setLevSmoothFactor(tLivingString2 const p, Lfloat levSmoothFactor)
{
    tFeedbackLeveler_setFactor(p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(p->fbLevL, levSmoothFactor);
}

void     tLivingString2_setLevStrength(tLivingString2 const p, Lfloat levStrength)
{
    tFeedbackLeveler_setStrength(p->fbLevU, levStrength);
    tFeedbackLeveler_setStrength(p->fbLevL, levStrength);
}

void     tLivingString2_setLevMode(tLivingString2 const p, int levMode)
{
    tFeedbackLeveler_setMode(p->fbLevU, levMode);
    tFeedbackLeveler_setMode(p->fbLevL, levMode);
    p->levMode=levMode;
}

Lfloat   tLivingString2_tick(tLivingString2 const p, Lfloat input)
{
    input = input * 0.5f; // drop gain by half since we'll be equally adding it at half amplitude to forward and backward waveguides
    // from prepPos upwards=forwards
    Lfloat wLen=tExpSmooth_tick(p->wlSmooth);

    Lfloat pickP=tExpSmooth_tick(p->ppSmooth);

    //Lfloat pickupPos=tExpSmooth_tick(&p->puSmooth);

    //need to determine which delay line to put it into (should be half amplitude into forward and backward lines for the correct portion of string)
    Lfloat prepP=tExpSmooth_tick(p->prpSmooth);
    Lfloat lowLen=prepP*wLen;
    Lfloat upLen=(1.0f-prepP)*wLen;
    uint32_t pickPInt;

    if (pickP > prepP)
    {
        Lfloat fullPickPoint =  ((pickP*wLen) - lowLen);
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point
        Lfloat pickPLfloat = fullPickPoint - pickPInt;

        tHermiteDelay_addTo(p->delUF, input * (1.0f - pickPLfloat), pickPInt);
        tHermiteDelay_addTo(p->delUF, input * pickPLfloat, pickPInt + 1);
        tHermiteDelay_addTo(p->delUB, input * (1.0f - pickPLfloat), (uint32_t) (upLen - pickPInt));
        tHermiteDelay_addTo(p->delUB, input * pickPLfloat, (uint32_t) (upLen - pickPInt - 1));
    }
    else
    {
         Lfloat fullPickPoint =  pickP * wLen;
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point
        Lfloat pickPLfloat = fullPickPoint - pickPInt;

        tHermiteDelay_addTo(p->delLF, input * (1.0f - pickPLfloat), pickPInt);
        tHermiteDelay_addTo(p->delLF, input * pickPLfloat, pickPInt + 1);
        tHermiteDelay_addTo(p->delLB, input * (1.0f - pickPLfloat), (uint32_t) (lowLen - pickPInt));
        tHermiteDelay_addTo(p->delLB, input * pickPLfloat, (uint32_t) (lowLen - pickPInt - 1));
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
    Lfloat fromLF=tHermiteDelay_tickOut(p->delLF);
    Lfloat fromUF=tHermiteDelay_tickOut(p->delUF);
    Lfloat fromUB=tHermiteDelay_tickOut(p->delUB);
    Lfloat fromLB=tHermiteDelay_tickOut(p->delLB);
    fromLB = LEAF_clip(-1.0f, fromLB, 1.0f);
    fromUB = LEAF_clip(-1.0f, fromUB, 1.0f);
    fromUF = LEAF_clip(-1.0f, fromUF, 1.0f);
    fromLF = LEAF_clip(-1.0f, fromLF, 1.0f);

    // into upper half of string, from bridge, going backwards
    Lfloat fromBridge=-tFeedbackLeveler_tick(p->fbLevU, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(p->DCblockerU, tTwoZero_tick(p->bridgeFilter, fromUF)));
    fromBridge = LEAF_clip(-1.0f, fromBridge, 1.0f);
    tHermiteDelay_tickIn(p->delUB, fromBridge);
    // into lower half of string, from prepPoint, going backwards
    Lfloat fromLowerPrep=-tTwoZero_tick(p->prepFilterL, fromLF);
    fromLowerPrep = LEAF_clip(-1.0f, fromLowerPrep, 1.0f);
    Lfloat intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromUB; //used to add input here
    intoLower = LEAF_clip(-1.0f, intoLower, 1.0f);
    tHermiteDelay_tickIn(p->delLB, intoLower);
    // into lower half of string, from nut
    Lfloat fromNut=-tFeedbackLeveler_tick(p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(p->DCblockerL, tTwoZero_tick(p->nutFilter, fromLB)));
    fromNut = LEAF_clip(-1.0f, fromNut, 1.0f);
    tHermiteDelay_tickIn(p->delLF, fromNut);
    // into upper half of string, from prepPoint, going forwards/upwards
    Lfloat fromUpperPrep=-tTwoZero_tick(p->prepFilterU, fromUB);
    fromUpperPrep = LEAF_clip(-1.0f, fromUpperPrep, 1.0f);
    Lfloat intoUpper=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF;
    //intoUpper = tanhf(intoUpper);
    intoUpper = LEAF_clip(-1.0f, intoUpper, 1.0f);
    tHermiteDelay_tickIn(p->delUF, intoUpper);
    // update all delay lengths

    tHermiteDelay_setDelay(p->delLF, lowLen);
    tHermiteDelay_setDelay(p->delLB, lowLen);
    tHermiteDelay_setDelay(p->delUF, upLen);
    tHermiteDelay_setDelay(p->delUB, upLen);
    
    uint32_t PUPInt;
    Lfloat pickupOut = 0.0f;
    Lfloat pupos = tExpSmooth_tick(p->puSmooth);
    if (pupos < 0.9999f)
    {
        if (pupos > prepP)
        {
            Lfloat fullPUPoint =  ((pupos*wLen) - lowLen);
            PUPInt = (uint32_t) fullPUPoint; // where does the input go? that's the pick point
            Lfloat PUPLfloat = fullPUPoint - PUPInt;

            pickupOut = tHermiteDelay_tapOut(p->delUF, PUPInt) * (1.0f - PUPLfloat);
            pickupOut += tHermiteDelay_tapOut(p->delUF, PUPInt + 1) * PUPLfloat;
            pickupOut += tHermiteDelay_tapOut(p->delUB, (uint32_t) (upLen - PUPInt)) * (1.0f - PUPLfloat);
            pickupOut += tHermiteDelay_tapOut(p->delUB, (uint32_t) (upLen - PUPInt - 1))  * PUPLfloat;
        }
        else
        {
             Lfloat fullPUPoint =  pupos * wLen;
            PUPInt = (uint32_t) fullPUPoint; // where does the input go? that's the pick point
            Lfloat PUPLfloat = fullPUPoint - PUPInt;

            pickupOut = tHermiteDelay_tapOut(p->delLF, PUPInt) * (1.0f - PUPLfloat);
            pickupOut += tHermiteDelay_tapOut(p->delLF,  PUPInt + 1) * PUPLfloat;
            pickupOut += tHermiteDelay_tapOut(p->delLB, (uint32_t) (lowLen - PUPInt)) * (1.0f - PUPLfloat);
            pickupOut += tHermiteDelay_tapOut(p->delLB, (uint32_t) (lowLen - PUPInt - 1)) * PUPLfloat;
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

Lfloat   tLivingString2_tickEfficient(tLivingString2 const p, Lfloat input)
{
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

        tHermiteDelay_addTo(p->delUF, input, pickPInt);
        tHermiteDelay_addTo(p->delUB, input, (uint32_t) (upLen - pickPInt));
    }
    else
    {
        Lfloat fullPickPoint =  pickP * wLen;
        pickPInt = (uint32_t) fullPickPoint; // where does the input go? that's the pick point

        tHermiteDelay_addTo(p->delLF, input, pickPInt);
        tHermiteDelay_addTo(p->delLB, input, (uint32_t) (lowLen - pickPInt));
    }
    Lfloat fromLF=tHermiteDelay_tickOut(p->delLF);
    Lfloat fromUF=tHermiteDelay_tickOut(p->delUF);
    Lfloat fromUB=tHermiteDelay_tickOut(p->delUB);
    Lfloat fromLB=tHermiteDelay_tickOut(p->delLB);
    // into upper half of string, from bridge, going backwards
    //Lfloat fromBridge=-tFeedbackLeveler_tick(&p->fbLevU, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerU, tTwoZero_tick(&p->bridgeFilter, fromUF)));
    Lfloat fromBridge=-tFeedbackLeveler_tick(p->fbLevU,tHighpass_tick(p->DCblockerU, tTwoZero_tick(p->bridgeFilter, fromUF)));
    tHermiteDelay_tickIn(p->delUB, fromBridge);
    // into lower half of string, from prepPoint, going backwards
    Lfloat fromLowerPrep=-tTwoZero_tick(p->prepFilterL, fromLF);
    Lfloat intoLower=(p->prepIndex*fromLowerPrep)+((1.0f - p->prepIndex)*fromUB); //used to add input here
    tHermiteDelay_tickIn(p->delLB, intoLower);
    // into lower half of string, from nut
    //Lfloat fromNut=-tFeedbackLeveler_tick(&p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(&p->DCblockerL, tTwoZero_tick(&p->nutFilter, fromLB)));
    Lfloat fromNut=-tFeedbackLeveler_tick(p->fbLevL,tHighpass_tick(p->DCblockerL, tTwoZero_tick(p->nutFilter, fromLB)));
    tHermiteDelay_tickIn(p->delLF, fromNut);
    // into upper half of string, from prepPoint, going forwards/upwards
    Lfloat fromUpperPrep=-tTwoZero_tick(p->prepFilterU, fromUB);
    Lfloat intoUpper=(p->prepIndex*fromUpperPrep)+((1.0f - p->prepIndex)*fromLF);
    tHermiteDelay_tickIn(p->delUF, intoUpper);
    // update all delay lengths

    p->curr = fromBridge;

    //p->curr = fromBridge;
    //p->curr += fromNut;

    return p->curr;
}


void tLivingString2_updateDelays(tLivingString2 const p)
{
    //need to determine which delay line to put it into (should be half amplitude into forward and backward lines for the correct portion of string)

    Lfloat lowLen=p->prpSmooth->dest*p->wlSmooth->dest;
    Lfloat upLen=(1.0f-p->prpSmooth->dest)*p->wlSmooth->dest;

	tHermiteDelay_setDelay(p->delLF, lowLen);
    tHermiteDelay_setDelay(p->delLB, lowLen);
    tHermiteDelay_setDelay(p->delUF, upLen);
    tHermiteDelay_setDelay(p->delUB, upLen);
}

Lfloat   tLivingString2_sample(tLivingString2 const p)
{
    return p->curr;
}

void    tLivingString2_setSampleRate(tLivingString2 const p, Lfloat sr)
{
    p->sampleRate = sr;
    p->waveLengthInSamples = (p->sampleRate/p->freq) - 1;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    tTwoZero_setSampleRate(p->bridgeFilter, p->sampleRate);
    tTwoZero_setSampleRate(p->nutFilter, p->sampleRate);
    tTwoZero_setSampleRate(p->prepFilterU, p->sampleRate);
    tTwoZero_setSampleRate(p->prepFilterL, p->sampleRate);
    tHighpass_setSampleRate(p->DCblockerU, p->sampleRate);
    tHighpass_setSampleRate(p->DCblockerL, p->sampleRate);
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
    tComplexLivingString_setFreq(*pl, freq);
    p->freq = freq;
    tExpSmooth_initToPool(&p->pickPosSmooth, pickPos, 0.01f, mp); // smoother for pick position
    tExpSmooth_initToPool(&p->prepPosSmooth, prepPos, 0.01f, mp); // smoother for pick position

    tComplexLivingString_setPickPos(*pl, pickPos);
    tComplexLivingString_setPrepPos(*pl, prepPos);

    p->prepPos=prepPos;
    p->pickPos=pickPos;
    tLinearDelay_initToPool(&p->delLF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delMF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUF,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delUB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delMB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_initToPool(&p->delLB,p->waveLengthInSamples, 2400, mp);
    tLinearDelay_clear(p->delLF);
    tLinearDelay_clear(p->delMF);
    tLinearDelay_clear(p->delUF);
    tLinearDelay_clear(p->delUB);
    tLinearDelay_clear(p->delMB);
    tLinearDelay_clear(p->delLB);
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

void     tComplexLivingString_setFreq(tComplexLivingString const p, Lfloat freq)
{    // NOTE: It is faster to set wavelength in samples directly
    if (freq<20.0f) freq=20.0f;
    else if (freq>10000.0f) freq=10000.0f;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tComplexLivingString_setWaveLength(tComplexLivingString const p, Lfloat waveLength)
{
    if (waveLength<4.8f) waveLength=4.8f;
    else if (waveLength>2400.0f) waveLength=2400.0f;
    p->waveLengthInSamples = waveLength;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
}

void     tComplexLivingString_setPickPos(tComplexLivingString const p, Lfloat pickPos)
{    // between 0 and 1
    if (pickPos<0.5f) pickPos=0.5f;
    else if (pickPos>1.f) pickPos=1.f;
    p->pickPos = pickPos;
    tExpSmooth_setDest(p->pickPosSmooth, p->pickPos);
}

void     tComplexLivingString_setPrepPos(tComplexLivingString const p, Lfloat prepPos)
{    // between 0 and 1
    if (prepPos<0.f) prepPos=0.f;
    else if (prepPos>0.5f) prepPos=0.5f;
    p->prepPos = prepPos;
    tExpSmooth_setDest(p->prepPosSmooth, p->prepPos);
}

void     tComplexLivingString_setPrepIndex(tComplexLivingString const p, Lfloat prepIndex)
{    // between 0 and 1
    if (prepIndex<0.f) prepIndex=0.f;
    else if (prepIndex>1.f) prepIndex=1.f;
    p->prepIndex = prepIndex;
}

void     tComplexLivingString_setDampFreq(tComplexLivingString const p, Lfloat dampFreq)
{
    tOnePole_setFreq(p->bridgeFilter, dampFreq);
    tOnePole_setFreq(p->nutFilter, dampFreq);
    tOnePole_setFreq(p->prepFilterU, dampFreq);
    tOnePole_setFreq(p->prepFilterL, dampFreq);
}

void     tComplexLivingString_setDecay(tComplexLivingString const p, Lfloat decay)
{
    p->decay=decay;
}

void     tComplexLivingString_setTargetLev(tComplexLivingString const p, Lfloat targetLev)
{
    tFeedbackLeveler_setTargetLevel(p->fbLevU, targetLev);
    tFeedbackLeveler_setTargetLevel(p->fbLevL, targetLev);
}

void     tComplexLivingString_setLevSmoothFactor(tComplexLivingString const p, Lfloat levSmoothFactor)
{
    tFeedbackLeveler_setFactor(p->fbLevU, levSmoothFactor);
    tFeedbackLeveler_setFactor(p->fbLevL, levSmoothFactor);
}

void     tComplexLivingString_setLevStrength(tComplexLivingString const p, Lfloat levStrength)
{
    tFeedbackLeveler_setStrength(p->fbLevU, levStrength);
    tFeedbackLeveler_setStrength(p->fbLevL, levStrength);
}

void     tComplexLivingString_setLevMode(tComplexLivingString const p, int levMode)
{
    tFeedbackLeveler_setMode(p->fbLevU, levMode);
    tFeedbackLeveler_setMode(p->fbLevL, levMode);
    p->levMode=levMode;
}

Lfloat   tComplexLivingString_tick(tComplexLivingString const p, Lfloat input)
{
    // from pickPos upwards=forwards
    Lfloat fromLF=tLinearDelay_tickOut(p->delLF);
    Lfloat fromMF=tLinearDelay_tickOut(p->delMF);
    Lfloat fromUF=tLinearDelay_tickOut(p->delUF);
    Lfloat fromUB=tLinearDelay_tickOut(p->delUB);
    Lfloat fromMB=tLinearDelay_tickOut(p->delMB);
    Lfloat fromLB=tLinearDelay_tickOut(p->delLB);

    // into upper part of string, from bridge, going backwards
    Lfloat fromBridge=-tFeedbackLeveler_tick(p->fbLevU, (p->levMode==0?p->decay:1)*tHighpass_tick(p->DCblockerU, tOnePole_tick(p->bridgeFilter, fromUF)));
    tLinearDelay_tickIn(p->delUB, fromBridge);

    // into pick position, take input and add it into the waveguide, going to come out of middle segment
    tLinearDelay_tickIn(p->delMB, fromUB+input);

    // into lower part of string, from prepPos, going backwards
    Lfloat fromLowerPrep=-tOnePole_tick(p->prepFilterL, fromLF);
    Lfloat intoLower=p->prepIndex*fromLowerPrep+(1.0f - p->prepIndex)*fromMB;
    tLinearDelay_tickIn(p->delLB, intoLower);

    // into lower part of string, from nut, going forwards toward prep position
    Lfloat fromNut=-tFeedbackLeveler_tick(p->fbLevL, (p->levMode==0?p->decay:1.0f)*tHighpass_tick(p->DCblockerL, tOnePole_tick(p->nutFilter, fromLB)));
    tLinearDelay_tickIn(p->delLF, fromNut);

    // into middle part of string, from prep going toward pick position
    Lfloat fromUpperPrep=-tOnePole_tick(p->prepFilterU, fromMB);
    Lfloat intoMiddle=p->prepIndex*fromUpperPrep+(1.0f - p->prepIndex)*fromLF;

    //pick position, going to come out of middle segment
    tLinearDelay_tickIn(p->delMF, intoMiddle);

    //take output of middle segment and put it into upper segment connecting to the bridge, take input and add it into the waveguide, 
    tLinearDelay_tickIn(p->delUF, fromMF + input);

    // update all delay lengths
    Lfloat pickP=tExpSmooth_tick(p->pickPosSmooth);
    Lfloat prepP=tExpSmooth_tick(p->prepPosSmooth);
    Lfloat wLen=tExpSmooth_tick(p->wlSmooth);

    Lfloat midLen = (pickP-prepP) * wLen; // the length between the pick and the prep;
    Lfloat lowLen = prepP*wLen; // the length from prep to nut
    Lfloat upLen = (1.0f-pickP)*wLen; // the length from pick to bridge


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

Lfloat   tComplexLivingString_sample(tComplexLivingString const p)
{
    return p->curr;
}

void    tComplexLivingString_setSampleRate(tComplexLivingString const p, Lfloat sr)
{
    Lfloat freq = p->waveLengthInSamples/p->sampleRate;
    p->sampleRate = sr;
    p->waveLengthInSamples = p->sampleRate/freq;
    tExpSmooth_setDest(p->wlSmooth, p->waveLengthInSamples);
    tOnePole_setSampleRate(p->bridgeFilter, p->sampleRate);
    tOnePole_setSampleRate(p->nutFilter, p->sampleRate);
    tOnePole_setSampleRate(p->prepFilterU, p->sampleRate);
    tOnePole_setSampleRate(p->prepFilterL, p->sampleRate);
    tHighpass_setSampleRate(p->DCblockerU, p->sampleRate);
    tHighpass_setSampleRate(p->DCblockerL, p->sampleRate);
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
    tLinearDelay_clear(x->neckDelay);
    tLinearDelay_clear(x->bridgeDelay);
    tCookOnePole_initToPool(&x->reflFilt, mp);
    tCookOnePole_setSampleRate(x->reflFilt, x->sampleRate);
    
    tCookOnePole_setPole(x->reflFilt, 0.6f - (0.1f * 22050.f / x->sampleRate));
    tCookOnePole_setGain(x->reflFilt, .95f);
    tBowTable_initToPool(&x->bowTabl, mp);
    x->bowTabl->slope = 3.0f;
    tBowed_setFreq(x, x->x_fr);
    
    tSVF_initToPool(&x->lowpass, SVFTypeLowpass, 18000.0f, 0.6f, mp);
    tSVF_setSampleRate(x->lowpass, x->sampleRate);
    tSVF_setFreq(x->lowpass,16000.0f);
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

Lfloat   tBowed_tick  (tBowed const x)
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

    x->bowTabl->slope = bp;

    if (bpos != x->betaRatio) {
        x->betaRatio = bpos;
        tLinearDelay_setDelay(x->bridgeDelay, x->baseDelay * x->betaRatio);      /* bow to bridge length   */
        tLinearDelay_setDelay(x->neckDelay, x->baseDelay * (1. - x->betaRatio)); /* bow to nut (finger) length   */


        for (int i = 0; i < x->oversampling; i++) {
            bridgeRefl = -tCookOnePole_tick(x->reflFilt, x->bridgeDelay->lastOut); /* Bridge Reflection      */
            nutRefl = x->neckDelay->lastOut;                                /* Nut Reflection         */
            stringVel = bridgeRefl + nutRefl;                                   /* Sum is String Velocity */
            velDiff = bv - stringVel;                                         /* Differential Velocity  */
            newVel = velDiff * tBowTable_lookup(x->bowTabl, velDiff);         /* Non-Lin Bow Function   */
            tLinearDelay_tick(x->neckDelay, bridgeRefl + newVel);                     /* Do string              */
            tLinearDelay_tick(x->bridgeDelay, nutRefl + newVel);                      /*   propagations         */
            x->output = tSVF_tick(x->lowpass, x->bridgeDelay->lastOut);
        }

        return x->output;
    }
}

void    tBowed_setFreq               (tBowed const x, Lfloat freq)
{
    if (freq < 20.f)
        freq = 20.f;
    x->baseDelay = (x->sampleRate / freq) - 4.0;                               /* delay - approx. filter delay */
    tLinearDelay_setDelay(x->bridgeDelay, x->baseDelay * x->betaRatio);      /* bow to bridge length */
    tLinearDelay_setDelay(x->neckDelay, x->baseDelay * (1. - x->betaRatio)); /* bow to nut (finger) length */
}



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
    Lfloat maxDelayTemp = x->sampleRate / lowestFreq;
    tLagrangeDelay_initToPool(&x->delay, 100.0f, maxDelayTemp, mp);
    x->actualLowestFreq = x->sampleRate / tLagrangeDelay_getMaxDelay (x->delay); //adjusted to create a power-of-two size buffer

    tLagrangeDelay_clear(x->delay);
    tLagrangeDelay_initToPool(&x->delayP, 100.0f, maxDelayTemp, mp);
    tLagrangeDelay_clear(x->delayP);

    x->maxDelay = tLagrangeDelay_getMaxDelay (x->delay);

    tCookOnePole_initToPool(&x->reflFilt, mp);
    tCookOnePole_setSampleRate(x->reflFilt, x->sampleRate);
    tCookOnePole_initToPool(&x->reflFiltP, mp);
    tCookOnePole_setSampleRate(x->reflFiltP, x->sampleRate);

    tCookOnePole_setGainAndPole(x->reflFilt,0.999f, -0.0014f);
    tCookOnePole_setGainAndPole(x->reflFiltP,0.999f, -0.0014f);
   
    tTString_setFreq(x, 440.0f);

    tExpSmooth_initToPool(&x->tensionSmoother, 0.0f, 0.004f * x->invOversampling, mp);
    tExpSmooth_setDest(x->tensionSmoother, 0.0f);
    tExpSmooth_initToPool(&x->pitchSmoother, 100.0f, 0.04f * x->invOversampling, mp);
    tExpSmooth_setDest(x->pitchSmoother, 100.0f);
    tExpSmooth_initToPool(&x->pickNoise, 0.0f, 0.09f * x->invOversampling, mp);
    tExpSmooth_setDest(x->pickNoise, 0.0f);
    tThiranAllpassSOCascade_initToPool(&x->allpass, 4, mp);
    tThiranAllpassSOCascade_initToPool(&x->allpassP, 4, mp);
    x->allpassDelay = tThiranAllpassSOCascade_setCoeff(x->allpass, 0.0001f, 100.0f, x->invOversampling);
    x->allpassDelayP = tThiranAllpassSOCascade_setCoeff(x->allpassP, 0.000025f, 100.0f, x->invOversampling);

    tSVF_initToPool(&x->lowpassP, SVFTypeLowpass, 5000.0f, 0.8f, mp);
    tSVF_setSampleRate(x->lowpassP, x->sampleRate);
    tSVF_setFreq(x->lowpassP, 6000.0f);
    tSVF_initToPool(&x->highpassP, SVFTypeHighpass, 1800.0f, 0.8f, mp);
    tSVF_setSampleRate(x->highpassP, x->sampleRate);
    tSVF_setFreq(x->highpassP, 1800.0f);
    x->twoPiTimesInvSampleRate = TWO_PI * x->invSampleRate;
    tCycle_initToPool(&x->tensionModOsc, mp);
    tCycle_setSampleRate(x->tensionModOsc, x->sampleRate);
    tCycle_initToPool(&x->pickupModOsc, mp);
    tCycle_setSampleRate(x->pickupModOsc, x->sampleRate);
    x->pickupModOscFreq = 440.0f;
    x->pickupModOscAmp = 1.0f;
    tSVF_initToPool(&x->pickupFilter, SVFTypeLowpass, 2900.0f, 1.0f, mp);
    tSVF_setSampleRate(x->pickupFilter, x->sampleRate);

    tSVF_setFreq(x->pickupFilter, 3900.0f);

    tSVF_initToPool(&x->pickupFilter2, SVFTypeLowpass, 3800.0f, 1.1f, mp);
    tSVF_setSampleRate(x->pickupFilter2, x->sampleRate);

    tSVF_setFreq(x->pickupFilter2, 4100.0f);


    tSVF_initToPool(&x->peakFilt, SVFTypePeak, 1000.0f, .9f, mp);
    tSVF_setSampleRate(x->peakFilt, x->sampleRate);

    tSVF_setFreq(x->peakFilt, 1000.0f);

    tNoise_initToPool(&x->noise, PinkNoise, mp);
    tHighpass_initToPool(&x->dcBlock, 1.0f, mp);
    tHighpass_setSampleRate(x->dcBlock,x->sampleRate);
    tHighpass_initToPool(&x->dcBlockP, 1.0f, mp);
    tHighpass_setSampleRate(x->dcBlockP,x->sampleRate);
    tSlide_initToPool(&x->slide, 0, 3000, mp);//100 1400
    if (x->wound)
    {

    	tExpSmooth_initToPool(&x->barSmooth2, 0.0f,0.0005f, mp); //was 0.0005
    	    tExpSmooth_setDest(x->barSmooth2, 0.0f);
    	    tExpSmooth_initToPool(&x->barSmoothVol, 0.0f,0.0008f, mp); //was 0.008
    	    tExpSmooth_setDest(x->barSmoothVol, 0.0f);
    }
    else
    {

    	tExpSmooth_initToPool(&x->barSmooth2, 0.0f,0.005f, mp); //was 0.0005
    	tExpSmooth_setDest(x->barSmooth2, 0.0f);
    	tExpSmooth_initToPool(&x->barSmoothVol, 0.0f,0.004f, mp); //was 0.008
    	tExpSmooth_setDest(x->barSmoothVol, 0.0f);
    }
    tSlide_initToPool(&x->barSmooth, 1000, 1000, mp);//600 600
    tSlide_initToPool(&x->barPulseSlide, 2, 30, mp);//100 1400 // 10 3000
    tExpSmooth_initToPool(&x->barPulse, 0.0f,0.05f, mp); //was 0.05
    tExpSmooth_setDest(x->barPulse, 0.0f);


    tSVF_initToPool(&x->barResonator, SVFTypeBandpass, 5.0f, 15.0f, mp);

    x->timeSinceLastBump = 1;
    tHighpass_initToPool(&x->barHP, 30.0f, mp);
    tSVF_initToPool(&x->barLP, SVFTypeLowpass, 7000.0f, 0.9f, mp);

    x->inharmonic = 1;
    x->inharmonicMult = 1.0f;
    tFeedbackLeveler_initToPool(&x->feedback, 0.25f, 0.04f * x->invOversampling, 0.1f, 1, mp);
    tFeedbackLeveler_initToPool(&x->feedbackP, 0.25f, 0.04f * x->invOversampling, 0.1f, 1, mp);
    x->quarterSampleRate = x->sampleRate * 0.245f; // a little less than a quarter because we want to compute filters with it ( normalized filter cutoff needs to be less than half pi to work with freq approximation)
    x->windingsPerInch = 70.0f;

    tNoise_initToPool(&x->pickNoiseSource, PinkNoise, mp);
    x->pickupAmount = 0.0f;
    tPickupNonLinearity_initToPool(&x->p, mp);
}

void    tTString_free (tTString* const bw)
{
    _tTString* x = *bw;
    tPickupNonLinearity_free(&x->p);
    tNoise_free(&x->pickNoiseSource);
    tFeedbackLeveler_free(&x->feedbackP);
    tFeedbackLeveler_free(&x->feedback);
    tSVF_free(&x->barLP);
    tHighpass_free(&x->barHP);
    tSVF_free(&x->barResonator);
    tExpSmooth_free(&x->barPulse);
    tSlide_free(&x->barPulseSlide);
    tSlide_free(&x->barSmooth);
	tExpSmooth_free(&x->barSmoothVol);
	tExpSmooth_free(&x->barSmooth2);
	tSlide_free(&x->slide);
    tHighpass_free(&x->dcBlockP);
    tHighpass_free(&x->dcBlock);
    tNoise_free(&x->noise);
    tSVF_free(&x->peakFilt);
    tSVF_free(&x->pickupFilter2);
    tSVF_free(&x->pickupFilter);
    tCycle_free(&x->pickupModOsc);
    tCycle_free(&x->tensionModOsc);
    tSVF_free(&x->highpassP);
    tSVF_free(&x->lowpassP),
    tThiranAllpassSOCascade_free(&x->allpassP);
    tThiranAllpassSOCascade_free(&x->allpass);
    tExpSmooth_free(&x->pickNoise);
    tExpSmooth_free(&x->pitchSmoother);
    tExpSmooth_free(&x->tensionSmoother);
    tCookOnePole_free(&x->reflFiltP);
    tCookOnePole_free(&x->reflFilt);
    tLagrangeDelay_free(&x->delayP);
    tLagrangeDelay_free(&x->delay);
    mpool_free((char*)x, x->mempool);
}

Lfloat   tTString_tick                  (tTString const x)
{
    Lfloat theOutput = 0.0f;
    x->feedbackNoise = tNoise_tick(x->noise);

    Lfloat barPos = x->barPosition;
    if ((barPos > (x->prevBarPosSmoothed + 3.0f)) || (barPos < (x->prevBarPosSmoothed - 3.0f)))
	{
    	tExpSmooth_setValAndDest(x->barSmooth2,barPos);
	}
    else
    {
    	tExpSmooth_setDest(x->barSmooth2,barPos);
    }

    //Lfloat currentBumpSmoothed = currentBump;
    Lfloat barPosSmoothed = tExpSmooth_tick(x->barSmooth2);

    Lfloat barDifference = fastabsf(barPosSmoothed - x->prevBarPosSmoothed);


    tExpSmooth_setDest(x->barSmoothVol,barPos);
        Lfloat barPosSmoothVol = tExpSmooth_tick(x->barSmoothVol);
        Lfloat differenceFastSmooth = fastabsf(barPosSmoothVol - x->prevBarPosSmoothVol);
        x->prevBarPosSmoothVol = barPosSmoothVol;
        Lfloat volCut = 1.0f;
        if (differenceFastSmooth < 0.0001f)
        {
        	//volCut = LEAF_map(differenceFastSmooth, 0.00001f, 0.0001f, 0.0f, 1.0f);
        	volCut = ((differenceFastSmooth - 0.00001f) * 1111.111111111111111f);
        	if (volCut < 0.0f)
        	{
        		volCut = 0.0f;
        	}
        }
    Lfloat currentBump = barPosSmoothed * x->windingsPerInch;
    Lfloat bumpDifference = fastabsf(currentBump - x->lastBump);

    x->prevBarPosSmoothed = barPosSmoothed;
    barDifference = LEAF_clip(0.0f, barDifference*2.0f, 1.0f);
    x->slideAmount = tSlide_tick(x->slide, barDifference);
    if (x->wound)
    {
		if (bumpDifference > 1.0f)
		{
			tExpSmooth_setVal(x->barPulse, 1.0f);

			tSlide_setDest(x->barPulseSlide, 1.0f);

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

			tSlide_setDest(x->barSmooth, (x->sampleRate * x->invOversampling / x->timeSinceLastBump));
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
				tExpSmooth_setVal(x->barPulse, 1.0f);
				tSlide_setDest(x->barPulseSlide, 1.0f);
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
			tSlide_setDest(x->barSmooth, 5.0f);
		}
		Lfloat barFreq = tSlide_tickNoInput(x->barSmooth);
		tSVF_setFreq(x->barResonator, barFreq);
		Lfloat filterAmount = 0.5f;
		Lfloat dryAmount = 1.0f;
		x->slideNoise = x->feedbackNoise;

		Lfloat pulseVal = tSlide_tickNoInput(x->barPulseSlide);

		if (pulseVal > .99f)
		{
			tSlide_setDest(x->barPulseSlide, 0.0f);

		}

		x->slideNoise *= pulseVal;
		//x->slideNoise = tExpSmooth_tick(&x->barPulse);
		x->slideNoise *= tExpSmooth_tick(x->barPulse);
		//x->slideNoise *= x->slideGain;
		Lfloat tempSlideNoise1 = x->slideNoise;
		Lfloat tempSlideNoise2 = fast_tanh5(tSVF_tick(x->barResonator, tempSlideNoise1)*x->barDrive);

		Lfloat filterFade = 1.0f;
		if (barFreq < 300.0f)
		{
			//filterFade = LEAF_map(barFreq, 100.0f, 300.0f, 0.0f, 1.0f);
			filterFade = (barFreq - 100.0f) * 0.005f;
			filterFade = LEAF_clip(0.0f, filterFade, 1.0f);
		}

		x->slideNoise = ((tempSlideNoise1 * dryAmount) + (tempSlideNoise2 * filterAmount * filterFade));// * x->slideAmount;
		x->slideNoise *= x->muted;
		x->slideNoise = tHighpass_tick(x->barHP, x->slideNoise);
		x->slideNoise = tSVF_tick(x->barLP, x->slideNoise * x->slideGain * volCut);
		x->slideNoise = x->slideNoise * x->slideGain;
    }
    else
    {
		//x->slideNoise = tNoise_tick(&x->pickNoiseSource)*0.3f;
    	x->slideNoise = x->feedbackNoise;
		x->slideNoise *= x->muted;
		x->slideNoise = tHighpass_tick(x->barHP, x->slideNoise);
		x->slideNoise = tSVF_tick(x->barLP, x->slideNoise * x->slideGain * 0.5f * volCut);
		x->slideNoise = x->slideNoise * x->slideGain;
    }
    for (int i = 0; i < x->oversampling; i++)
    {
    	x->baseDelay = tExpSmooth_tick(x->pitchSmoother);

    	Lfloat tension = tExpSmooth_tick(x->tensionSmoother) * x->tensionGain * x->baseDelay;


    	//x->tensionAmount = (tension + (tCycle_tick(&x->tensionModOsc) * tension * 0.0f)) * 0.01f;
    	x->tensionAmount = tension * 0.01f;
    	Lfloat currentDelay = x->baseDelay;

    	//Lfloat currentDelay = x->baseDelay;
        if (currentDelay < 5.0f)
        {
        	currentDelay = 5.0f;
        }

        if (currentDelay > x->maxDelay)
        {
        	currentDelay = x->maxDelay;
        }
        //tCycle_setFreq(&x->tensionModOsc, (x->sampleRate / (currentDelay - x->tensionAmount)) * 2.0f);
        Lfloat halfCurrentDelay = currentDelay * 0.5f;

        //tLagrangeDelay_setDelay(&x->delay, x->wavelength-2);//currentDelay - x->tensionAmount - (x->allpassDelay * x->inharmonicMult));// - ((x->phaseComp * INV_TWO_PI ) * currentDelay));
        //tLagrangeDelay_setDelay(&x->delayP,x->wavelength-2);// currentDelay - x->tensionAmount - (x->allpassDelayP * x->inharmonicMult));// - ((x->phaseComp * INV_TWO_PI ) * currentDelay));
        tLagrangeDelay_setDelay(x->delay, currentDelay - x->tensionAmount - (x->allpassDelay * x->inharmonicMult) - ((x->phaseComp * INV_TWO_PI ) * currentDelay));
        tLagrangeDelay_setDelay(x->delayP,currentDelay - x->tensionAmount - (x->allpassDelayP * x->inharmonicMult) - ((x->phaseComp * INV_TWO_PI ) * currentDelay));


        Lfloat pickNoiseSound = tNoise_tick(x->pickNoiseSource) * tExpSmooth_tick(x->pickNoise);
        tLagrangeDelay_addTo(x->delay,pickNoiseSound * 2.0f, (uint32_t)x->pluckPoint_forInput);
        tLagrangeDelay_addTo(x->delayP,pickNoiseSound * 2.0f, (uint32_t)x->pluckPoint_forInput);

        Lfloat filterOut = tCookOnePole_tick(x->reflFilt, x->output);
        Lfloat filterOutP = tCookOnePole_tick(x->reflFiltP, x->outputP);
        //Lfloat filterOut = x->output;
        //Lfloat filterOutP = x->outputP;

        Lfloat rippleLength = x->rippleRate *  currentDelay;
        //Lfloat rippleLength = 0.0f;
        uint32_t rippleLengthInt = (uint32_t) rippleLength;
        Lfloat rippleLengthAlpha = rippleLength - rippleLengthInt;

       // Lfloat rippleLength2 = currentDelay - rippleLength;
        //uint32_t rippleLengthInt2 = (uint32_t) rippleLength2;
        //Lfloat rippleLengthAlpha2 = rippleLength2 - rippleLengthInt2;


        //Lfloat rippleOut = (tLagrangeDelay_tapOutInterpolated(&x->delay,rippleLengthInt,rippleLengthAlpha) * x->r);
        //delayOut -= (tLagrangeDelay_tapOutInterpolated(&x->delay,rippleLengthInt2,rippleLengthAlpha2) * x->r);
        Lfloat delayOut = tLagrangeDelay_tickOut(x->delay);
        Lfloat delayOutP = tLagrangeDelay_tickOut(x->delayP);
        Lfloat rippleOut1 = (tLagrangeDelay_tapOutInterpolated(x->delay,rippleLengthInt,rippleLengthAlpha) * x->r);
        Lfloat rippleOut1P = (tLagrangeDelay_tapOutInterpolated(x->delayP,rippleLengthInt,rippleLengthAlpha) * x->r);


        //Lfloat rippleSampleP = (tLagrangeDelay_tapOutInterpolated(&x->delayP,rippleLengthInt,rippleLengthAlpha) * x->r);
        //delayOutP += rippleSampleP;
        //Lfloat outputP = tLagrangeDelay_tickOut(&x->delayP);

        //filterOut = rippleOut1 + rippleOut2;
        Lfloat rippleOut = delayOut + rippleOut1;
        Lfloat rippleOutP = delayOutP + rippleOut1P;
       // outputP += (tLagrangeDelay_tapOutInterpolated(&x->delayP,rippleLengthInt,rippleLengthAlpha) * x->r);


        //output += (tLagrangeDelay_tapOutInterpolated(&x->delay,rippleLengthInt2,rippleLengthAlpha2) * x->r);
        //output += (tLagrangeDelay_tapOutInterpolated(&x->delayP,rippleLengthInt2,rippleLengthAlpha2) * x->r);
        Lfloat allpassOut = rippleOut;
        Lfloat allpassOutP = rippleOutP;
        if (x->inharmonic)
        {
        	allpassOut = tThiranAllpassSOCascade_tick(x->allpass, rippleOut);
        	allpassOutP = tThiranAllpassSOCascade_tick(x->allpassP, rippleOutP);
        }


        x->output = LEAF_clip(-1.0f, (allpassOut * (x->decayCoeff * x->invOnePlusr) * x->muteCoeff), 1.0f);
        x->outputP = LEAF_clip(-1.0f, (allpassOutP * (x->decayCoeff * x->invOnePlusr) * x->muteCoeff), 1.0f);


        //if past the string ends, need to read backwards and invert
        uint32_t backwards = 0;

        Lfloat positionMin = (x->openStringLength * 0.104166666666667f);
        Lfloat positionMax = (x->openStringLength * 0.021666666666667f);
        Lfloat pickupInput = x->pickupPos + (tCycle_tick(x->pickupModOsc) * x->pickupModOscAmp);
        //Lfloat pickupInput = x->pickupPos;
        Lfloat pickupPos = LEAF_mapFromZeroToOneInput(pickupInput, positionMin, positionMax);

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

        x->pickup_Ratio = pickupPosFinal / halfCurrentDelay;
        uint32_t pickupPosInt = (uint32_t) pickupPosFinal;
        uint32_t pickupPosIntPlusOne = pickupPosInt + 1;
        Lfloat pickupPosAlpha = pickupPosFinal - pickupPosInt;
        Lfloat oneMinusPickupPosAlpha = 1.0f - pickupPosAlpha;
        Lfloat inversePickupPos = currentDelay - pickupPos;
        if (backwards)
        {
        	inversePickupPos = halfCurrentDelay + pickupPos;
        }
        uint32_t inversePickupPosInt = (uint32_t) inversePickupPos;
        uint32_t inversePickupPosIntPlusOne = inversePickupPosInt + 1;
        Lfloat inversePickupPosAlpha = inversePickupPos - inversePickupPosInt;
        Lfloat oneMinusinversePickupPosAlpha = 1.0f - inversePickupPosAlpha;

        Lfloat rightgoing = tLagrangeDelay_tapOut(x->delay, pickupPosInt) * oneMinusPickupPosAlpha;
        rightgoing += tLagrangeDelay_tapOut(x->delay, pickupPosIntPlusOne) * pickupPosAlpha;
        Lfloat leftgoing = tLagrangeDelay_tapOut(x->delay, inversePickupPosInt) * oneMinusinversePickupPosAlpha;
        leftgoing += tLagrangeDelay_tapOut(x->delay, inversePickupPosIntPlusOne) * inversePickupPosAlpha;

        //x->pickupOut =  (((volumes[1] * pickupNonLinearity(rightgoing)) + (volumes[0] * rightgoing)) - ((volumes[1] * pickupNonLinearity(leftgoing)) + (volumes[0] * leftgoing)));
        x->pickupOut =  (pickupNonLinearity(rightgoing) - pickupNonLinearity(leftgoing));
        rightgoing = tLagrangeDelay_tapOut(x->delayP, pickupPosInt) * oneMinusPickupPosAlpha;
		rightgoing += tLagrangeDelay_tapOut(x->delayP, pickupPosIntPlusOne) * pickupPosAlpha;
		leftgoing = tLagrangeDelay_tapOut(x->delayP, inversePickupPosInt) * oneMinusinversePickupPosAlpha;
		leftgoing += tLagrangeDelay_tapOut(x->delayP, inversePickupPosIntPlusOne) * inversePickupPosAlpha;

		x->pickupOutP =  (pickupNonLinearity(rightgoing) - pickupNonLinearity(leftgoing));

		Lfloat inputSample = tFeedbackLeveler_tick(x->feedback, tHighpass_tick(x->dcBlock, filterOut + x->slideNoise + x->feedbackNoise * x->feedbackNoiseLevel));
		Lfloat inputSampleP = tFeedbackLeveler_tick(x->feedbackP, tHighpass_tick(x->dcBlockP, filterOutP + x->slideNoise + x->feedbackNoise * x->feedbackNoiseLevel));
        tLagrangeDelay_tickIn(x->delay, inputSample);
        tLagrangeDelay_tickIn(x->delayP, inputSampleP);
        //tLagrangeDelay_tickIn(&x->delayP, delayOutP + x->slideNoise);
        //Lfloat outputPfilt = 0.0f;
        Lfloat outputPfilt = tSVF_tick(x->lowpassP, x->pickupOutP);
        outputPfilt = tSVF_tick(x->highpassP, outputPfilt);
        Lfloat volumes[2];
        LEAF_crossfade(x->pickupAmount * 2.0f - 1.0f,volumes);
        Lfloat prefilter = (x->pickupOut + (outputPfilt * x->phantomGain)) * 2.0f;
        Lfloat prefilter2 = tSVF_tick(x->pickupFilter2, prefilter);// + x->slideNoise;
        Lfloat prefilter3 = tSVF_tick(x->pickupFilter, prefilter2);// + x->slideNoise;
        Lfloat prefilter4 = tSVF_tick(x->peakFilt, prefilter3);// + x->slideNoise;

        theOutput = (prefilter4 * 1.3f* volumes[1]) + (prefilter * 1.3f * volumes[0]);
    }
    return theOutput;
}

void    tTString_setFreq               (tTString const x, Lfloat freq)
{
    if (freq < x->actualLowestFreq)
        freq = x->actualLowestFreq;
    //freq = freq * x->invOversampling;
    x->freq = freq;

    Lfloat Fc = (freq*x->invSampleRateTimesTwoPi);

    //tExpSmooth_setDest(&x->pitchSmoother, (x->sampleRate / freq) - 1.0f);
    tExpSmooth_setDest(x->pitchSmoother, (x->sampleRate / freq) - 2.0f); //why 2? is it the combo of the lowpass one pole and highpass one pole?

    //tCycle_setFreq(&x->tensionModOsc, freq * x->invOversamplingTimesTwo);
#ifdef ARM_MATH_CM7
    Lfloat result = 0.0f;
    arm_atan2_f32(x->poleCoeff *  arm_sin_f32(Fc) , 1.0f - (x->poleCoeff *  arm_cos_f32(Fc)), &result);
     x->phaseComp = result;
    //if I can figure out how to use a newer CMSIS DSP pack, the new version 1.15 has an atan2f approximation that's likely faster
#else
    x->phaseComp = atan2f((x->poleCoeff * sinf(Fc)) , (1.0f - (x->poleCoeff * cosf(Fc))));
#endif
}

void    tTString_setDecay               (tTString const x, Lfloat decay)
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

void    tTString_setDecayInSeconds               (tTString const x, Lfloat decay)
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

void    tTString_setPickupPos               (tTString const x, Lfloat pos)
{
	x->pickupPos = pos;
}

void    tTString_setPickupAmount               (tTString const x, Lfloat amount)
{
	x->pickupAmount = amount;
}

void    tTString_setPickupModAmp               (tTString const x, Lfloat amp)
{
	x->pickupModOscAmp = amp;
}


void    tTString_setPickupModFreq               (tTString const x, Lfloat freq)
{
	x->pickupModOscFreq = freq;
	tCycle_setFreq(x->pickupModOsc, freq);
}



void    tTString_mute              (tTString const x)
{
	 x->muteCoeff = fastExp3(-1.0f / (x->freq * 0.007f));;
	 if (x->muteCoeff < 0.5f)
	 {
		 x->muteCoeff = 0.5f;
	 }
	 x->muted = 0.0f;
}

//0-1
void    tTString_setFilter              (tTString const x, Lfloat filter)
{
	//filter = ((filter * 18000.0f) + 20.0f)* x->twoPiTimesInvSampleRate;
	 //Lfloat lowestNumOvertones = LEAF_map(x->freq, 20.0f, 3000.0f, 15.0f, 12.0f);
	// LEAF_clip(7.0f, lowestNumOvertones, 10.0f);
	 Lfloat overtone = LEAF_mapFromZeroToOneInput(filter, 14.0f, 100.0f);

	 Lfloat freqToUse = (overtone * x->freq);

	freqToUse = LEAF_clip(10.0f, freqToUse, x->quarterSampleRate);
	x->filterFreq = freqToUse;
#ifdef ARM_MATH_CM7
	filter = 1.0f - arm_sin_f32(freqToUse * x->twoPiTimesInvSampleRate);
#else
	filter = 1.0f - sinf(freqToUse * x->twoPiTimesInvSampleRate);
#endif
	//volatile Lfloat normalizedFreq = (overtone * x->freq) * x->twoPiTimesInvSampleRate;
	//filter = 1.0f - normalizedFreq;

	tCookOnePole_setPole(x->reflFilt,filter);
	tCookOnePole_setPole(x->reflFiltP,filter);
	x->poleCoeff = filter;
}

void    tTString_setFilterFreqDirectly              (tTString const x, Lfloat freq)
{
	Lfloat freqToUse = LEAF_clip(10.0f, freq, x->quarterSampleRate);
	x->filterFreq = freqToUse;
#ifdef ARM_MATH_CM7
	freq = 1.0f - arm_sin_f32(freqToUse * x->twoPiTimesInvSampleRate);
#else
	freq = 1.0f - sinf(freqToUse * x->twoPiTimesInvSampleRate);
#endif
	//volatile Lfloat normalizedFreq = (overtone * x->freq) * x->twoPiTimesInvSampleRate;
	//filter = 1.0f - normalizedFreq;

	tCookOnePole_setPole(x->reflFilt,freq);
	tCookOnePole_setPole(x->reflFiltP,freq);
	x->poleCoeff = freq;
}

void   tTString_setTensionGain                  (tTString const x, Lfloat tensionGain)
{
	x->tensionGain = tensionGain;
}

//0-1
void   tTString_setBarDrive                  (tTString const x, Lfloat drive)
{
	x->barDrive = drive + 1.0f;
}

void   tTString_setBarPosition                  (tTString const x, Lfloat barPosition)
{
	x->prevBarPosition = x->barPosition;
	x->barPosition = barPosition;
}

void   tTString_setOpenStringFrequency                  (tTString const x, Lfloat openStringFrequency)
{
	x->openStringLength = ((x->sampleRate / openStringFrequency) - 2.0f);
}

void   tTString_setPickupRatio                  (tTString const x, Lfloat ratio)
{
	x->pickupRatio = ratio;
}

void   tTString_setPhantomHarmonicsGain                  (tTString const x, Lfloat gain)
{
	x->phantomGain = gain;
}

void   tTString_setSlideGain                  (tTString const x, Lfloat gain)
{
	x->slideGain = gain;
}

void   tTString_setTensionSpeed                  (tTString const x, Lfloat tensionSpeed)
{
	tExpSmooth_setFactor(x->tensionSmoother, 0.00064f * x->invOversampling * (1.0f - tensionSpeed) + 0.00001f); //0.5 is good
}


void   tTString_setRippleDepth                  (tTString const x, Lfloat depth)
{
	x->r = fastExp3(-1.0f / (x->freq * (x->decayInSeconds * 0.005f + 0.0001f))) * depth * 0.5f;
	x->invOnePlusr = 1.0f / (1.0f + x->r);
	Lfloat highpassFreq = depth * 30.0f + 1.0f;
	tHighpass_setFreq(x->dcBlock,highpassFreq);
	tHighpass_setFreq(x->dcBlockP,highpassFreq);
}

void   tTString_setHarmonic                  (tTString const x, Lfloat harmonic)
{
	x->harmonic = harmonic;
	x->rippleRate = 1.0f / harmonic;
}
void   tTString_setFeedbackStrength                  (tTString const x, Lfloat strength)
{
	Lfloat strengthVal = 0.5f * strength;
	tFeedbackLeveler_setStrength(x->feedback, strengthVal);
	tFeedbackLeveler_setStrength(x->feedbackP, strengthVal);
	x->feedbackNoiseLevel = strength * 0.005f;

}
void   tTString_setFeedbackReactionSpeed                  (tTString const x, Lfloat speed)
{
	Lfloat speedVal = speed * 0.1f + 0.00001f;
	tFeedbackLeveler_setFactor(x->feedback, speedVal);
	tFeedbackLeveler_setFactor(x->feedbackP, speedVal);
}

void    tTString_pluck               (tTString const x, Lfloat position, Lfloat amplitude)
{
    x->muted = 1.0f;
    tExpSmooth_setVal(x->tensionSmoother, amplitude);
    tFeedbackLeveler_setTargetLevel(x->feedback, amplitude * 0.25f);
    tFeedbackLeveler_setTargetLevel(x->feedbackP, amplitude * 0.25f);
    tExpSmooth_setValAndDest(x->pitchSmoother, x->pitchSmoother->dest);
    x->baseDelay = x->pitchSmoother->dest;
    Lfloat currentDelay = x->baseDelay;// - x->allpassDelay;
    x->muteCoeff = 1.0f;
    uint32_t halfCurrentDelay = currentDelay * 0.5f;
    uint32_t currentDelayInt = halfCurrentDelay * 2.0f;
    Lfloat positionMin = (x->openStringLength * 0.204166666666667f);
    Lfloat positionMax = (x->openStringLength * 0.001f);

    volatile Lfloat pluckPoint = LEAF_mapFromZeroToOneInput(position, positionMin, positionMax);
    //0.083335f; * position;//position * halfCurrentDelay;
    pluckPoint = LEAF_clip(0.0f, pluckPoint, halfCurrentDelay);
    uint32_t pluckPointInt = (uint32_t) pluckPoint;
    Lfloat invPluckPointInt = (1.0f / ((Lfloat)pluckPointInt+.001f));
    volatile Lfloat invRemainder = 1.0f / ((halfCurrentDelay-pluckPoint)+.001f);
    tLagrangeDelay_clear(x->delay);
    x->pluckRatio = pluckPoint / halfCurrentDelay;
    x->pluckPoint_forInput = pluckPoint;
    for (uint32_t i = 0; i < halfCurrentDelay; i++)
    {
        Lfloat val = 0.0f;
        //displacement waves
        if (i == pluckPointInt)
        {
            val = 1.0f;
        }
        else if (i < pluckPointInt)
        {
            val = ((Lfloat)i * invPluckPointInt);
        }
        else
        {
            val = (1.0f - (((Lfloat)i-(Lfloat)pluckPointInt)*invRemainder));
        }
        val = LEAF_tanh(val* 1.2f) * amplitude;
        tLagrangeDelay_tapIn(x->delay, val, i);
        tLagrangeDelay_tapIn(x->delay, -val, currentDelayInt-i);
        tLagrangeDelay_tapIn(x->delayP, val, i);
        tLagrangeDelay_tapIn(x->delayP, -val, currentDelayInt-i);

    }
    tThiranAllpassSOCascade_clear(x->allpass);
    tThiranAllpassSOCascade_clear(x->allpassP);

    tExpSmooth_setVal(x->pickNoise, amplitude);
}



void    tTString_setWavelength         (tTString const x, uint32_t waveLength)
{
	//tExpSmooth_setDest(&x->pitchSmoother,waveLength);; // in samples
	x->wavelength = waveLength;
}


void    tTString_setHarmonicity         (tTString const x, Lfloat B, Lfloat freq)
{
    x->allpassDelay = tThiranAllpassSOCascade_setCoeff(x->allpass, B, freq * x->invOversampling, x->oversampling);
    x->allpassDelayP = tThiranAllpassSOCascade_setCoeff(x->allpassP, B * 0.25f, freq * x->invOversampling, x->oversampling);
}

void    tTString_setInharmonic         (tTString const x, uint32_t onOrOff)
{
    x->inharmonic = onOrOff;
    x->inharmonicMult = (Lfloat)onOrOff;
}
void    tTString_setWoundOrUnwound         (tTString const x, uint32_t wound)
{
    x->wound = wound;
    if (x->wound)
	{

    	tExpSmooth_setFactor(x->barSmooth2, 0.0005f);
    	tExpSmooth_setFactor(x->barSmoothVol, 0.0008f);

	}
	else
	{

		tExpSmooth_setFactor(x->barSmooth2, 0.005f);
		tExpSmooth_setFactor(x->barSmoothVol, 0.004f);

	}
}
void    tTString_setWindingsPerInch         (tTString const x, uint32_t windings)
{
    x->windingsPerInch = windings;
}

void    tTString_setPickupFilterFreq         (tTString const x, Lfloat cutoff)
{
    tSVF_setFreq(x->pickupFilter,cutoff);

}
void    tTString_setPickupFilterQ        (tTString const x, Lfloat Q)
{
    tSVF_setQ(x->pickupFilter,Q+0.5f);

}

void    tTString_setPeakFilterFreq         (tTString const x, Lfloat cutoff)
{
    tSVF_setFreq(x->peakFilt,cutoff);

}
void    tTString_setPeakFilterQ        (tTString const x, Lfloat Q)
{
    tSVF_setQ(x->peakFilt,Q+0.5f);

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
Lfloat    tBowTable_lookup               (tBowTable const x, Lfloat sample)
{
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

Lfloat   tReedTable_tick      (tReedTable const p, Lfloat input)
{
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

Lfloat   tReedTable_tanh_tick     (tReedTable const p, Lfloat input)
{
    // The input is differential pressure across the reed.
    Lfloat output = p->offset + (p->slope * input);
    
    // If output is > 1, the reed has slammed shut and the
    // reflection function value saturates at 1.0.
    return tanhf(output);
}

void     tReedTable_setOffset   (tReedTable const p, Lfloat offset)
{
    p->offset = offset;
}

void     tReedTable_setSlope   (tReedTable const p, Lfloat slope)
{
    p->slope = slope;
}

/* ============================ */

void    tStiffString_init      (tStiffString* const pm, int numModes, LEAF* const leaf)
{
    tStiffString_initToPool(pm, numModes, &leaf->mempool);
}

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
    p->muteDecay = 0.4f;
    p->sampleRate = m->leaf->sampleRate;
    p->twoPiTimesInvSampleRate = m->leaf->twoPiTimesInvSampleRate;
    p->nyquist = p->sampleRate * 0.5f;
    Lfloat lessThanNyquist = p->sampleRate * 0.4f;
    p->nyquistScalingFactor = 1.0f / (lessThanNyquist - p->nyquist);
    p->amp = 0.0f;
    p->gainComp = 0.0f;

    // allocate memory
    /*
    p->osc = (tDampedOscillator *) mpool_alloc(numModes * sizeof(tDampedOscillator), m);
    for (int i = 0; i < numModes; ++i) {
    	tDampedOscillator_initToPool(&p->osc[i], &m);
    }
    */
    p->osc = (tCycle *) mpool_alloc(numModes * sizeof(tCycle), m);
    for (int i = 0; i < numModes; ++i) {
    	tCycle_initToPool(&p->osc[i], &m);
    }
    //
    p->amplitudes = (Lfloat *) mpool_alloc(numModes * sizeof(Lfloat), m);
    p->outputWeights = (Lfloat *) mpool_alloc(numModes * sizeof(Lfloat), m);
    p->decayScalar = (Lfloat *) mpool_alloc(numModes * sizeof(Lfloat), m);
    p->decayVal = (Lfloat *) mpool_alloc(numModes * sizeof(Lfloat), m);
    p->nyquistCoeff = (Lfloat *) mpool_alloc(numModes * sizeof(Lfloat), m);
    tStiffString_updateOutputWeights(p);
}



void    tStiffString_free (tStiffString* const pm)
{
    _tStiffString* p = *pm;

    for (int i = 0; i < p->numModes; ++i) {
        //tDampedOscillator_free(&p->osc[i]);
    	tCycle_free(&p->osc[i]);
    	//
    }
    mpool_free((char *) p->nyquistCoeff, p->mempool);
    mpool_free((char *) p->decayScalar, p->mempool);
    mpool_free((char *) p->decayVal, p->mempool);
    mpool_free((char *) p->amplitudes, p->mempool);
    mpool_free((char *) p->outputWeights, p->mempool);
    mpool_free((char *) p, p->mempool);
}

void tStiffString_updateOscillators(tStiffString const p)
{
	Lfloat kappa_sq = p->stiffness * p->stiffness;
	Lfloat compensation = 0.0f;
    for (int i = 0; i < p->numModes; ++i) {
      int n = i + 1;
      int n_sq = n * n;
      Lfloat sig = p->decay + p->decayHighFreq * n_sq;
      //Lfloat w0 = n * sqrtf(1.0f + kappa_sq * n_sq);
       Lfloat w0 = n * (1.0f + 0.5f * kappa_sq * n_sq);
      Lfloat zeta = sig / w0;
      //Lfloat w = w0 * sqrtf(1.0f - zeta * zeta);
       Lfloat w = w0 * (1.0f - 0.5f * zeta * zeta);
      if (i == 0)
      {
    	  compensation = 1.0f / w;
      }
      /*
      if ((p->freqHz * w) < (p->sampleRate * 0.4f))
      {
    	  tDampedOscillator_setFreq(&p->osc[i], p->freqHz * w * compensation);
    	  tDampedOscillator_setDecay(&p->osc[i],p->freqHz * sig);
      }
      else
      {
    	  tDampedOscillator_setDecay(&p->osc[i],0.5f);
      }
      */
      Lfloat	testFreq = (p->freqHz * w);
      Lfloat nyquistTest = (testFreq - p->nyquist) * p->nyquistScalingFactor;
      p->nyquistCoeff[i] = LEAF_clip(0.0f, nyquistTest, 1.0f);
	  tCycle_setFreq(p->osc[i], testFreq * compensation);
	  //tDampedOscillator_setDecay(&p->osc[i],p->freqHz * sig);
	  Lfloat val = p->freqHz * sig;
	  Lfloat r = fastExp4(-val * p->twoPiTimesInvSampleRate);
	  p->decayScalar[i] = r * r;
    }
}
void tStiffString_updateOutputWeights(tStiffString const p)
{
		Lfloat x0 = p->pickupPos * PI;
		Lfloat totalGain = 0.0f;
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

Lfloat   tStiffString_tick                  (tStiffString const p)
{
    Lfloat sample = 0.0f;
    for (int i = 0; i < p->numModes; ++i) {
      //sample += tDampedOscillator_tick(&p->osc[i]) * p->amplitudes[i] * p->outputWeights[i];
      sample += tCycle_tick(p->osc[i]) * p->amplitudes[i] * p->outputWeights[i] * p->decayVal[i] * p->nyquistCoeff[i];
      p->decayVal[i] *= p->decayScalar[i] * p->muteDecay;
    }
    return sample * p->amp * p->gainComp;
}

void tStiffString_setStiffness(tStiffString const p, Lfloat newValue)
{
    p->stiffness = LEAF_mapFromZeroToOneInput(newValue,0.00f, 0.2f);
}

void tStiffString_setPickupPos(tStiffString const p, Lfloat newValue)
{
    p->pickupPos = LEAF_clip(0.01f, newValue, 0.99f);
    tStiffString_updateOutputWeights(p);
}

void tStiffString_setPluckPos(tStiffString const p, Lfloat newValue)
{
    p->pluckPos = LEAF_clip(0.01f, newValue, 0.99f);
    tStiffString_updateOutputWeights(p);
}


void tStiffString_setFreq(tStiffString const p, Lfloat newFreq)
{
    p->freqHz = newFreq;
    tStiffString_updateOscillators(p);
}

void tStiffString_setDecay(tStiffString const p, Lfloat decay)
{
    p->decay = decay;
    tStiffString_updateOscillators(p);
}

void tStiffString_setDecayHighFreq(tStiffString const p, Lfloat decayHF)
{
    p->decayHighFreq = decayHF;
    tStiffString_updateOscillators(p);
}

void tStiffString_mute(tStiffString const p)
{
    p->muteDecay = 0.99f;
}

void tStiffString_pluck(tStiffString const p, Lfloat amp)
{
    Lfloat x0 = p->pluckPos * PI;
    p->muteDecay = 1.0f;
    for (int i = 0; i < p->numModes; ++i) {
        int n = i + 1;
        float denom = n * n * x0 * (PI - x0);
        if (denom < 0.001f)
        {
        	denom = 0.001f; // to avoid divide by zero
        }
#ifdef ARM_MATH_CM7
		  p->amplitudes[i] = 2.0f * arm_sin_f32(x0 * n) / denom;
#else
	      p->amplitudes[i] = 2.0f * sinf(x0 * n) / denom;
#endif
        //tDampedOscillator_reset(&p->osc[i]);
        p->decayVal[i] = 1.0f;
    }
    p->amp = amp;
    tStiffString_updateOutputWeights(p);
}

void tStiffString_setSampleRate(tStiffString* const pm, Lfloat sr)
{
    tStiffString p = *pm;

    p->sampleRate = sr;
    p->twoPiTimesInvSampleRate = TWO_PI / sr;
}

void tStiffString_setStiffnessNoUpdate(tStiffString const p, Lfloat newValue)
{

    p->stiffness = LEAF_mapFromZeroToOneInput(newValue,0.00f, 0.2f);
}

void tStiffString_setPickupPosNoUpdate(tStiffString const p, Lfloat newValue)
{
    p->pickupPos = LEAF_clip(0.01f, newValue, 0.99f);
}

void tStiffString_setPluckPosNoUpdate(tStiffString const p, Lfloat newValue)
{
    p->pluckPos = LEAF_clip(0.01f, newValue, 0.99f);
}


void tStiffString_setFreqNoUpdate(tStiffString const p, Lfloat newFreq)
{
    p->freqHz = newFreq;
}

void tStiffString_setDecayNoUpdate(tStiffString const p, Lfloat decay)
{
    p->decay = decay;
}

void tStiffString_setDecayHighFreqNoUpdate(tStiffString const p, Lfloat decayHF)
{
    p->decayHighFreq = decayHF;
}


void tStiffString_pluckNoUpdate(tStiffString const p, Lfloat amp)
{
    Lfloat x0 = p->pluckPos * 0.5f * PI;
    p->muteDecay = 1.0f;
    for (int i = 0; i < p->numModes; ++i) {
        int n = i + 1;
        float denom = n * n * x0 * (PI - x0);
        if (denom < 0.001f)
        {
        	denom = 0.001f; // to avoid divide by zero
        }
#ifdef ARM_MATH_CM7
		  p->amplitudes[i] = 2.0f * arm_sin_f32(x0 * n) / denom;
#else
	      p->amplitudes[i] = 2.0f * sinf(x0 * n) / denom;
#endif
        //tDampedOscillator_reset(&p->osc[i]);
        p->decayVal[i] = 1.0f;
    }
    p->amp = amp;
}


