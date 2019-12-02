/*==============================================================================

    leaf-instruments.c
    Created: 30 Nov 2018 10:24:21am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-instruments.h"

#else

#include "../Inc/leaf-instruments.h"

#endif

// ----------------- COWBELL ----------------------------//

void t808Cowbell_init(t808Cowbell* const cowbellInst, int useStick)
{
    _t808Cowbell* cowbell = *cowbellInst = (_t808Cowbell*) leaf_alloc(sizeof(_t808Cowbell));
    
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
    
    tNoise_init(&cowbell->stick, WhiteNoise);
    
    tEnvelope_init(&cowbell->envStick, 5.0f, 5.0f, 0);
    
    cowbell->useStick = useStick;
}

void t808Cowebell_free(t808Cowbell* const cowbellInst)
{
    _t808Cowbell* cowbell = *cowbellInst;
    
    tSquare_free(&cowbell->p[0]);
    tSquare_free(&cowbell->p[1]);
    tSVF_free(&cowbell->bandpassOsc);
    tSVF_free(&cowbell->bandpassStick);
    tEnvelope_free(&cowbell->envGain);
    tEnvelope_free(&cowbell->envFilter);
    tHighpass_free(&cowbell->highpass);
    tNoise_free(&cowbell->stick);
    tEnvelope_free(&cowbell->envStick);
    leaf_free(cowbell);
}

void t808Cowbell_on(t808Cowbell* const cowbellInst, float vel)
{
    _t808Cowbell* cowbell = *cowbellInst;
    
    tEnvelope_on(&cowbell->envGain, vel);
    
    if (cowbell->useStick)
        tEnvelope_on(&cowbell->envStick,vel);
}

float t808Cowbell_tick(t808Cowbell* const cowbellInst)
{
    _t808Cowbell* cowbell = *cowbellInst;
    
    float sample = 0.0f;
    
    // Mix oscillators.
    sample = (cowbell->oscMix * tSquare_tick(&cowbell->p[0])) + ((1.0f-cowbell->oscMix) * tSquare_tick(&cowbell->p[1]));
    
    // Filter dive and filter.
    tSVF_setFreq(&cowbell->bandpassOsc, cowbell->filterCutoff + 1000.0f * tEnvelope_tick(&cowbell->envFilter));
    sample = tSVF_tick(&cowbell->bandpassOsc,sample);
    
    sample *= (0.9f * tEnvelope_tick(&cowbell->envGain));
    
    if (cowbell->useStick)
        sample += (0.1f * tEnvelope_tick(&cowbell->envStick) * tSVF_tick(&cowbell->bandpassStick, tNoise_tick(&cowbell->stick)));

    
    sample = tHighpass_tick(&cowbell->highpass, sample);
    
    return sample;
}

void t808Cowbell_setDecay(t808Cowbell* const cowbellInst, float decay)
{
    _t808Cowbell* cowbell = *cowbellInst;
    tEnvelope_setDecay(&cowbell->envGain,decay);
}

void t808Cowbell_setHighpassFreq(t808Cowbell *cowbellInst, float freq)
{
    _t808Cowbell* cowbell = *cowbellInst;
    tHighpass_setFreq(&cowbell->highpass,freq);
}

void t808Cowbell_setBandpassFreq(t808Cowbell* const cowbellInst, float freq)
{
    _t808Cowbell* cowbell = *cowbellInst;
    cowbell->filterCutoff = freq;
}

void t808Cowbell_setFreq(t808Cowbell* const cowbellInst, float freq)
{
    _t808Cowbell* cowbell = *cowbellInst;
    tSquare_setFreq(&cowbell->p[0],freq);
    tSquare_setFreq(&cowbell->p[1],1.48148f*freq);
}

void t808Cowbell_setOscMix(t808Cowbell* const cowbellInst, float oscMix)
{
    _t808Cowbell* cowbell = *cowbellInst;
    cowbell->oscMix = oscMix;
}

void t808Cowbell_setStick(t808Cowbell* const cowbellInst, int useStick)
{
    _t808Cowbell* cowbell = *cowbellInst;
    cowbell->useStick = useStick;
}

// ----------------- HIHAT ----------------------------//

void t808Hihat_init(t808Hihat* const hihatInst)
{
    _t808Hihat* hihat = *hihatInst = (_t808Hihat*) leaf_alloc(sizeof(_t808Hihat));
    
    for (int i = 0; i < 6; i++)
    {
        tSquare_init(&hihat->p[i]);
    }
    
    tNoise_init(&hihat->stick, PinkNoise);
    tNoise_init(&hihat->n, WhiteNoise);
    
    // need to fix SVF to be generic
    tSVF_init(&hihat->bandpassStick, SVFTypeBandpass,2500.0f,1.2f);
    tSVF_init(&hihat->bandpassOsc, SVFTypeBandpass,3500.0f,0.3f);
    
    tEnvelope_init(&hihat->envGain, 0.0f, 50.0f, OFALSE);
    tEnvelope_init(&hihat->envStick, 0.0f, 7.0f, OFALSE);
    
    
    tHighpass_init(&hihat->highpass, 7000.0f);
    
    hihat->freq = 40.0f;
    hihat->stretch = 0.0f;
    
    tSquare_setFreq(&hihat->p[0], 2.0f * hihat->freq);
    tSquare_setFreq(&hihat->p[1], 3.00f * hihat->freq);
    tSquare_setFreq(&hihat->p[2], 4.16f * hihat->freq);
    tSquare_setFreq(&hihat->p[3], 5.43f * hihat->freq);
    tSquare_setFreq(&hihat->p[4], 6.79f * hihat->freq);
    tSquare_setFreq(&hihat->p[5], 8.21f * hihat->freq);
}

void t808Hihat_free(t808Hihat* const hihatInst)
{
    _t808Hihat* hihat = *hihatInst;
    
    for (int i = 0; i < 6; i++)
    {
        tSquare_free(&hihat->p[i]);
    }
    
    tNoise_free(&hihat->stick);
    tNoise_free(&hihat->n);
    
    // need to fix SVF to be generic
    tSVF_free(&hihat->bandpassStick);
    tSVF_free(&hihat->bandpassOsc);
    tEnvelope_free(&hihat->envGain);
    tEnvelope_free(&hihat->envStick);
    
    tHighpass_free(&hihat->highpass);
    
    leaf_free(hihat);
}

void t808Hihat_on(t808Hihat* const hihatInst, float vel)
{
    _t808Hihat* hihat = *hihatInst;
    tEnvelope_on(&hihat->envGain, vel);
    tEnvelope_on(&hihat->envStick, vel);
}

void t808Hihat_setOscNoiseMix(t808Hihat* const hihatInst, float oscNoiseMix)
{
    _t808Hihat* hihat = *hihatInst;
    hihat->oscNoiseMix = oscNoiseMix;
}

float t808Hihat_tick(t808Hihat* const hihatInst)
{
    _t808Hihat* hihat = *hihatInst;
    
    float sample = 0.0f;
    float gainScale = 0.1666f;

    float myNoise = tNoise_tick(&hihat->n);

	tSquare_setFreq(&hihat->p[0], ((2.0f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(&hihat->p[1], ((3.00f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(&hihat->p[2], ((4.16f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(&hihat->p[3], ((5.43f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(&hihat->p[4], ((6.79f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(&hihat->p[5], ((8.21f + hihat->stretch) * hihat->freq));

    for (int i = 0; i < 6; i++)
    {
        sample += tSquare_tick(&hihat->p[i]);
    }
    
    sample *= gainScale;
    
    sample = (hihat->oscNoiseMix * sample) + ((1.0f-hihat->oscNoiseMix) * myNoise);
    
    sample = tSVF_tick(&hihat->bandpassOsc, sample);
    
    float myGain = tEnvelope_tick(&hihat->envGain);
    sample *= (myGain*myGain);//square the output gain envelope
    sample = tHighpass_tick(&hihat->highpass, sample);
    sample += ((0.5f * tEnvelope_tick(&hihat->envStick)) * tSVF_tick(&hihat->bandpassStick, tNoise_tick(&hihat->stick)));
    sample = tanhf(sample * 2.0f);

    return sample;
}

void t808Hihat_setDecay(t808Hihat* const hihatInst, float decay)
{
    _t808Hihat* hihat = *hihatInst;
    tEnvelope_setDecay(&hihat->envGain,decay);
}

void t808Hihat_setHighpassFreq(t808Hihat* const hihatInst, float freq)
{
    _t808Hihat* hihat = *hihatInst;
    tHighpass_setFreq(&hihat->highpass,freq);
}

void t808Hihat_setStretch(t808Hihat* const hihatInst, float stretch)
{
    _t808Hihat* hihat = *hihatInst;
    hihat->stretch = stretch;
}

void t808Hihat_setFM(t808Hihat* const hihatInst, float FM_amount)
{
    _t808Hihat* hihat = *hihatInst;
    hihat->FM_amount = FM_amount;
}

void t808Hihat_setOscBandpassFreq(t808Hihat* const hihatInst, float freq)
{
    _t808Hihat* hihat = *hihatInst;
    tSVF_setFreq(&hihat->bandpassOsc,freq);
}

void t808Hihat_setOscBandpassQ(t808Hihat* const hihatInst, float Q)
{
    _t808Hihat* hihat = *hihatInst;
    tSVF_setQ(&hihat->bandpassOsc,Q);
}

void t808Hihat_setStickBandPassFreq(t808Hihat* const hihatInst, float freq)
{
    _t808Hihat* hihat = *hihatInst;
    tSVF_setFreq(&hihat->bandpassStick,freq);
}

void t808Hihat_setStickBandPassQ(t808Hihat* const hihatInst, float Q)
{
    _t808Hihat* hihat = *hihatInst;
    tSVF_setQ(&hihat->bandpassStick,Q);
}



void t808Hihat_setOscFreq(t808Hihat* const hihatInst, float freq)
{
    _t808Hihat* hihat = *hihatInst;
    hihat->freq = freq;
}

// ----------------- SNARE ----------------------------//

void t808Snare_init(t808Snare* const snareInst)
{
    _t808Snare* snare = *snareInst = (_t808Snare*) leaf_alloc(sizeof(_t808Snare));
    
    float ratio[2] = {1.0, 1.5};
    for (int i = 0; i < 2; i++)
    {
        tTriangle_init(&snare->tone[i]);
        
        tTriangle_setFreq(&snare->tone[i], ratio[i] * 400.0f);
        tSVF_init(&snare->toneLowpass[i], SVFTypeLowpass, 4000, 1.0f);
        tEnvelope_init(&snare->toneEnvOsc[i], 0.0f, 50.0f, OFALSE);
        tEnvelope_init(&snare->toneEnvGain[i], 1.0f, 150.0f, OFALSE);
        tEnvelope_init(&snare->toneEnvFilter[i], 1.0f, 2000.0f, OFALSE);
        
        snare->toneGain[i] = 0.5f;
    }
    
    snare->tone1Freq = ratio[0] * 100.0f;
    snare->tone2Freq = ratio[1] * 100.0f;
    snare->noiseFilterFreq = 3000.0f;
    tNoise_init(&snare->noiseOsc, WhiteNoise);
    tSVF_init(&snare->noiseLowpass, SVFTypeLowpass, 12000.0f, 0.8f);
    tEnvelope_init(&snare->noiseEnvGain, 0.0f, 100.0f, OFALSE);
    tEnvelope_init(&snare->noiseEnvFilter, 0.0f, 1000.0f, OFALSE);
    snare->noiseGain = 1.0f;
}

void        t808Snare_free                  (t808Snare* const snareInst)
{
    _t808Snare* snare = *snareInst;
    
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
    
    leaf_free(snare);
}

void t808Snare_on(t808Snare* const snareInst, float vel)
{
    _t808Snare* snare = *snareInst;
    
    for (int i = 0; i < 2; i++)
    {
        tEnvelope_on(&snare->toneEnvOsc[i], vel);
        tEnvelope_on(&snare->toneEnvGain[i], vel);
        tEnvelope_on(&snare->toneEnvFilter[i], vel);
    }
    
    tEnvelope_on(&snare->noiseEnvGain, vel);
    tEnvelope_on(&snare->noiseEnvFilter, vel);
}

void t808Snare_setTone1Freq(t808Snare* const snareInst, float freq)
{
    _t808Snare* snare = *snareInst;
    snare->tone1Freq = freq;
    tTriangle_setFreq(&snare->tone[0], freq);
}

void t808Snare_setTone2Freq(t808Snare* const snareInst, float freq)
{
    _t808Snare* snare = *snareInst;
    snare->tone2Freq = freq;
    tTriangle_setFreq(&snare->tone[1],freq);
}

void t808Snare_setTone1Decay(t808Snare* const snareInst, float decay)
{
    _t808Snare* snare = *snareInst;
    tEnvelope_setDecay(&snare->toneEnvGain[0],decay);
}

void t808Snare_setTone2Decay(t808Snare* const snareInst, float decay)
{
    _t808Snare* snare = *snareInst;
    tEnvelope_setDecay(&snare->toneEnvGain[1],decay);
}

void t808Snare_setNoiseDecay(t808Snare* const snareInst, float decay)
{
    _t808Snare* snare = *snareInst;
    tEnvelope_setDecay(&snare->noiseEnvGain,decay);
}

void t808Snare_setToneNoiseMix(t808Snare* const snareInst, float toneNoiseMix)
{
    _t808Snare* snare = *snareInst;
    snare->toneNoiseMix = toneNoiseMix;
}

void t808Snare_setNoiseFilterFreq(t808Snare* const snareInst, float noiseFilterFreq)
{
    _t808Snare* snare = *snareInst;
    snare->noiseFilterFreq = noiseFilterFreq;
}

void t808Snare_setNoiseFilterQ(t808Snare* const snareInst, float noiseFilterQ)
{
    _t808Snare* snare = *snareInst;
    tSVF_setQ(&snare->noiseLowpass, noiseFilterQ);
}

static float tone[2];

float t808Snare_tick(t808Snare* const snareInst)
{
    _t808Snare* snare = *snareInst;
    
    for (int i = 0; i < 2; i++)
    {
        tTriangle_setFreq(&snare->tone[i], snare->tone1Freq + (20.0f * tEnvelope_tick(&snare->toneEnvOsc[i])));
        tone[i] = tTriangle_tick(&snare->tone[i]);
        
        tSVF_setFreq(&snare->toneLowpass[i], 2000.0f + (500.0f * tEnvelope_tick(&snare->toneEnvFilter[i])));
        tone[i] = tSVF_tick(&snare->toneLowpass[i], tone[i]) * tEnvelope_tick(&snare->toneEnvGain[i]);
    }
    
    float noise = tNoise_tick(&snare->noiseOsc);
    tSVF_setFreq(&snare->noiseLowpass, snare->noiseFilterFreq + (1000.0f * tEnvelope_tick(&snare->noiseEnvFilter)));
    noise = tSVF_tick(&snare->noiseLowpass, noise) * tEnvelope_tick(&snare->noiseEnvGain);
    
    float sample = (snare->toneNoiseMix)*(tone[0] * snare->toneGain[0] + tone[1] * snare->toneGain[1]) + (1.0f-snare->toneNoiseMix) * (noise * snare->noiseGain);
    sample = tanhf(sample * 2.0f);
    return sample;
}

// ----------------- KICK ----------------------------//

void        t808Kick_init        			(t808Kick* const kickInst)
{
    _t808Kick* kick = *kickInst = (_t808Kick*) leaf_alloc(sizeof(_t808Kick));
    
	tCycle_init(&kick->tone);
	kick->toneInitialFreq = 40.0f;
	kick->sighAmountInHz = 7.0f;
	kick->chirpRatioMinusOne = 3.3f;
	tCycle_setFreq(&kick->tone, 50.0f);
	tSVF_init(&kick->toneLowpass, SVFTypeLowpass, 2000.0f, 0.5f);
	tEnvelope_init(&kick->toneEnvOscChirp, 0.0f, 20.0f, OFALSE);
	tEnvelope_init(&kick->toneEnvOscSigh, 0.0f, 2500.0f, OFALSE);
	tEnvelope_init(&kick->toneEnvGain, 0.0f, 800.0f, OFALSE);
	tNoise_init(&kick->noiseOsc, PinkNoise);
	tEnvelope_init(&kick->noiseEnvGain, 0.0f, 1.0f, OFALSE);
	kick->noiseGain = 0.3f;
}

void        t808Kick_free                  (t808Kick* const kickInst)
{
    _t808Kick* kick = *kickInst;
    
	tCycle_free(&kick->tone);
	tSVF_free(&kick->toneLowpass);
	tEnvelope_free(&kick->toneEnvOscChirp);
	tEnvelope_free(&kick->toneEnvOscSigh);
	tEnvelope_free(&kick->toneEnvGain);
	tNoise_free(&kick->noiseOsc);
	tEnvelope_free(&kick->noiseEnvGain);
    
    leaf_free(kick);
}

float       t808Kick_tick                  (t808Kick* const kickInst)
{
    _t808Kick* kick = *kickInst;
    
	tCycle_setFreq(&kick->tone, (kick->toneInitialFreq * (1.0f + (kick->chirpRatioMinusOne * tEnvelope_tick(&kick->toneEnvOscChirp)))) + (kick->sighAmountInHz * tEnvelope_tick(&kick->toneEnvOscSigh)));
	float sample = tCycle_tick(&kick->tone) * tEnvelope_tick(&kick->toneEnvGain);
	sample+= tNoise_tick(&kick->noiseOsc) * tEnvelope_tick(&kick->noiseEnvGain);
	//add distortion here
	sample = tSVF_tick(&kick->toneLowpass, sample);
	return sample;
}

void        t808Kick_on                    (t808Kick* const kickInst, float vel)
{
    _t808Kick* kick = *kickInst;
	tEnvelope_on(&kick->toneEnvOscChirp, vel);
	tEnvelope_on(&kick->toneEnvOscSigh, vel);
	tEnvelope_on(&kick->toneEnvGain, vel);
	tEnvelope_on(&kick->noiseEnvGain, vel);

}
void        t808Kick_setToneFreq          (t808Kick* const kickInst, float freq)
{
    _t808Kick* kick = *kickInst;
	kick->toneInitialFreq = freq;

}

void        t808Kick_setToneDecay         (t808Kick* const kickInst, float decay)
{
    _t808Kick* kick = *kickInst;
	tEnvelope_setDecay(&kick->toneEnvGain,decay);
	tEnvelope_setDecay(&kick->toneEnvGain,decay * 3.0f);
}

void        t808Kick_setNoiseDecay         (t808Kick* const kickInst, float decay);
void        t808Kick_setSighAmount         (t808Kick* const kickInst, float sigh);
void        t808Kick_setChirpAmount         (t808Kick* const kickInst, float chirp);
void        t808Kick_setToneNoiseMix       (t808Kick* const kickInst, float toneNoiseMix);
void        t808Kick_setNoiseFilterFreq    (t808Kick* const kickInst, float noiseFilterFreq);
void        t808Kick_setNoiseFilterQ       (t808Kick* const kickInst, float noiseFilterQ);


