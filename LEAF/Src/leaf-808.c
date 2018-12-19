/*
  ==============================================================================

    leaf_808.cpp
    Created: 30 Nov 2018 10:24:21am
    Author:  airship

  ==============================================================================
*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-808.h"

#else

#include "../Inc/leaf-808.h"

#endif

#define USE_STICK 0
void t808Cowbell_on(t808Cowbell* const cowbell, float vel)
{
    
    tEnvelope_on(&cowbell->envGain, vel);
    
#if USE_STICK
    tEnvelope_on(&cowbell->envStick,vel);
#endif
    
}

float t808Cowbell_tick(t808Cowbell* const cowbell) {
    
    float sample = 0.0f;
    
    // Mix oscillators.
    sample = (cowbell->oscMix * tSquare_tick(&cowbell->p[0])) + ((1.0f-cowbell->oscMix) * tSquare_tick(&cowbell->p[1]));
    
    // Filter dive and filter.
    tSVF_setFreq(&cowbell->bandpassOsc, cowbell->filterCutoff + 1000.0f * tEnvelope_tick(&cowbell->envFilter));
    sample = tSVF_tick(&cowbell->bandpassOsc,sample);
    
    sample *= (0.9f * tEnvelope_tick(&cowbell->envGain));
    
#if USE_STICK
    sample += (0.1f * tEnvelope_tick(&cowbell->envStick) * tSVF_tick(&cowbell->bandpassStick, tNoise_tick(&cowbell->stick)));
#endif
    
    sample = tHighpass_tick(&cowbell->highpass, sample);
    
    return sample;
}

void t808Cowbell_setDecay(t808Cowbell* const cowbell, float decay)
{
    tEnvelope_setDecay(&cowbell->envGain,decay);
}

void t808Cowbell_setHighpassFreq(t808Cowbell *cowbell, float freq)
{
    tHighpass_setFreq(&cowbell->highpass,freq);
}

void t808Cowbell_setBandpassFreq(t808Cowbell* const cowbell, float freq)
{
    cowbell->filterCutoff = freq;
}

void t808Cowbell_setFreq(t808Cowbell* const cowbell, float freq)
{
    
    tSquare_setFreq(&cowbell->p[0],freq);
    tSquare_setFreq(&cowbell->p[1],1.48148f*freq);
}

void t808Cowbell_setOscMix(t808Cowbell* const cowbell, float oscMix)
{
    cowbell->oscMix = oscMix;
}

void t808Cowbell_init(t808Cowbell* const cowbell) {
    
    tSquare_init(&cowbell->p[0]);
    tSquare_setFreq(&cowbell->p[0], 540.0f);
    
    tSquare_init(&cowbell->p[1]);
    tSquare_setFreq(&cowbell->p[1], 1.48148f * 540.0f);
    
    cowbell->oscMix = 0.5f;
    
    tSVF_init(&cowbell->bandpassOsc, SVFTypeBandpass, 2500, 1.0f);
    
    tSVF_init(&cowbell->bandpassStick, SVFTypeBandpass, 1800, 1.0f);
    
    tEnvelope_init(&cowbell->envGain, 5.0f, 100.0f, OFALSE);
    
    tEnvelope_init(&cowbell->envFilter, 5.0, 100.0f, OFALSE);
    
    tHighpass_init(&cowbell->highpass, 1000.0f);
    
#if USE_STICK
    tNoise_init(&cowbell->stick, NoiseTypeWhite);
    tEnvelope_init(&cowbell->envStick, 5.0f, 5.0f, 0);
#endif
}

void t808Hihat_on(t808Hihat* const hihat, float vel) {
    
    tEnvelope_on(&hihat->envGain, vel);
    tEnvelope_on(&hihat->envStick, vel);
    
}

void t808Hihat_setOscNoiseMix(t808Hihat* const hihat, float oscNoiseMix) {
    
    hihat->oscNoiseMix = oscNoiseMix;
    
}

float t808Hihat_tick(t808Hihat* const hihat) {
    
    float sample = 0.0f;
    float gainScale = 0.3f;
    
    for (int i = 0; i < 6; i++)
    {
        sample += tSquare_tick(&hihat->p[i]);
    }
    
    sample *= gainScale;
    
    sample = (hihat->oscNoiseMix * sample) + ((1.0f-hihat->oscNoiseMix) * (tNoise_tick(&hihat->n)));
    
    sample = tSVF_tick(&hihat->bandpassOsc, sample);
    
    float myGain = tEnvelope_tick(&hihat->envGain);
    sample *= myGain*myGain;//square the output
    sample = tHighpass_tick(&hihat->highpass, sample);
    sample += ((0.5f * tEnvelope_tick(&hihat->envStick)) * tSVF_tick(&hihat->bandpassStick, tNoise_tick(&hihat->stick)));
    return sample;
}

void t808Hihat_setDecay(t808Hihat* const hihat, float decay)
{
    tEnvelope_setDecay(&hihat->envGain,decay);
}

void t808Hihat_setHighpassFreq(t808Hihat* const hihat, float freq)
{
    tHighpass_setFreq(&hihat->highpass,freq);
}

void t808Hihat_setOscBandpassFreq(t808Hihat* const hihat, float freq)
{
    tSVF_setFreq(&hihat->bandpassOsc,freq);
}

void t808Hihat_setStickBandPassFreq(t808Hihat* const hihat, float freq)
{
    tSVF_setFreq(&hihat->bandpassStick,freq);
}


void t808Hihat_setOscFreq(t808Hihat* const hihat, float freq)
{
	//if (freq < 5600.0f) //to avoid aliasing (for some reason high frequency settings here cause hard faults)
	{
		tSquare_setFreq(&hihat->p[0], 2.0f * freq);
		tSquare_setFreq(&hihat->p[1], 3.00f * freq);
		tSquare_setFreq(&hihat->p[2], 4.16f * freq);
		tSquare_setFreq(&hihat->p[3], 5.43f * freq);
		tSquare_setFreq(&hihat->p[4], 6.79f * freq);
		tSquare_setFreq(&hihat->p[5], 8.21f * freq);
	}
}

void t808Hihat_init(t808Hihat* const hihat)
{
    for (int i = 0; i < 6; i++)
    {
        tSquare_init(&hihat->p[i]);
    }
    
    tNoise_init(&hihat->stick, WhiteNoise);
    tNoise_init(&hihat->n, WhiteNoise);
    
    // need to fix SVF to be generic
    tSVF_init(&hihat->bandpassStick, SVFTypeBandpass,2500.0,1.2f);
    tSVF_init(&hihat->bandpassOsc, SVFTypeBandpass,3500,0.3f);
    
    tEnvelope_init(&hihat->envGain, 0.0f, 50.0f, OFALSE);
    tEnvelope_init(&hihat->envStick, 0.0f, 4.0f, OFALSE);
    
    tHighpass_init(&hihat->highpass, 7000.0f);
    
    float freq = 40.0f;
    
    tSquare_setFreq(&hihat->p[0], 2.0f * freq);
    tSquare_setFreq(&hihat->p[1], 3.00f * freq);
    tSquare_setFreq(&hihat->p[2], 4.16f * freq);
    tSquare_setFreq(&hihat->p[3], 5.43f * freq);
    tSquare_setFreq(&hihat->p[4], 6.79f * freq);
    tSquare_setFreq(&hihat->p[5], 8.21f * freq);
}

void t808Snare_on(t808Snare* const snare, float vel)
{
    for (int i = 0; i < 2; i++)
    {
        tEnvelope_on(&snare->toneEnvOsc[i], vel);
        tEnvelope_on(&snare->toneEnvGain[i], vel);
        tEnvelope_on(&snare->toneEnvFilter[i], vel);
    }
    
    tEnvelope_on(&snare->noiseEnvGain, vel);
    tEnvelope_on(&snare->noiseEnvFilter, vel);
}

void t808Snare_setTone1Freq(t808Snare* const snare, float freq)
{
    snare->tone1Freq = freq;
    tTriangle_setFreq(&snare->tone[0], freq);
    
}

void t808Snare_setTone2Freq(t808Snare* const snare, float freq)
{
    snare->tone2Freq = freq;
    tTriangle_setFreq(&snare->tone[1],freq);
}

void t808Snare_setTone1Decay(t808Snare* const snare, float decay)
{
    tEnvelope_setDecay(&snare->toneEnvGain[0],decay);
}

void t808Snare_setTone2Decay(t808Snare* const snare, float decay)
{
    tEnvelope_setDecay(&snare->toneEnvGain[1],decay);
}

void t808Snare_setNoiseDecay(t808Snare* const snare, float decay)
{
    tEnvelope_setDecay(&snare->noiseEnvGain,decay);
}

void t808Snare_setToneNoiseMix(t808Snare* const snare, float toneNoiseMix)
{
    snare->toneNoiseMix = toneNoiseMix;
}

void t808Snare_setNoiseFilterFreq(t808Snare* const snare, float noiseFilterFreq)
{
    snare->noiseFilterFreq = noiseFilterFreq;
}

void t808Snare_setNoiseFilterQ(t808Snare* const snare, float noiseFilterQ)
{
    tSVF_setQ(&snare->noiseLowpass, noiseFilterQ);
}


static float tone[2];

float t808Snare_tick(t808Snare* const snare)
{
    for (int i = 0; i < 2; i++)
    {
        tTriangle_setFreq(&snare->tone[i], snare->tone1Freq + (50.0f * tEnvelope_tick(&snare->toneEnvOsc[i])));
        tone[i] = tTriangle_tick(&snare->tone[i]);
        
        tSVF_setFreq(&snare->toneLowpass[i], 2000 + (500 * tEnvelope_tick(&snare->toneEnvFilter[i])));
        tone[i] = tSVF_tick(&snare->toneLowpass[i], tone[i]) * tEnvelope_tick(&snare->toneEnvGain[i]);
    }
    
    float noise = tNoise_tick(&snare->noiseOsc);
    tSVF_setFreq(&snare->noiseLowpass, snare->noiseFilterFreq +(500 * tEnvelope_tick(&snare->noiseEnvFilter)));
    noise = tSVF_tick(&snare->noiseLowpass, noise) * tEnvelope_tick(&snare->noiseEnvGain);
    
    float sample = (snare->toneNoiseMix)*(tone[0] * snare->toneGain[0] + tone[1] * snare->toneGain[1]) + (1.0f-snare->toneNoiseMix) * (noise * snare->noiseGain);
    
    return sample;
}

void t808Snare_init(t808Snare* const snare)
{
    float ratio[2] = {1.0, 1.5};
    for (int i = 0; i < 2; i++)
    {
        tTriangle_init(&snare->tone[i]);
        tTriangle_setFreq(&snare->tone[i], ratio[i] * 400.0f);
        tSVF_init(&snare->toneLowpass[i], SVFTypeLowpass, 2000, 1.0f);
        tEnvelope_init(&snare->toneEnvOsc[i], 3.0f, 20.0f, OFALSE);
        tEnvelope_init(&snare->toneEnvGain[i], 10.0f, 200.0f, OFALSE);
        tEnvelope_init(&snare->toneEnvFilter[i], 3.0f, 200.0f, OFALSE);
        
        snare->toneGain[i] = 0.5f;
    }
    
    
    tNoise_init(&snare->noiseOsc, WhiteNoise);
    tSVF_init(&snare->noiseLowpass, SVFTypeLowpass, 2000, 3.0f);
    tEnvelope_init(&snare->noiseEnvGain, 10.0f, 125.0f, OFALSE);
    tEnvelope_init(&snare->noiseEnvFilter, 3.0f, 100.0f, OFALSE);
    snare->noiseGain = 0.3f;
}

void        t808Snare_free                  (t808Snare* const snare)
{
    for (int i = 0; i < 2; i++)
    {
        tTriangle_free(&snare->tone[i]);
        tSVF_free(&snare->toneLowpass[i]);
        tEnvelope_free(&snare->toneEnvOsc[i]);
        tEnvelope_free(&snare->toneEnvGain[i]);
        tEnvelope_free(&snare->toneEnvFilter[i]);
    }
    
    
    tNoise_free(&snare->noiseOsc);
    tSVF_free(&snare->noiseLowpass);
    tEnvelope_free(&snare->noiseEnvGain);
    tEnvelope_free(&snare->noiseEnvFilter);
}


