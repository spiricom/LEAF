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

void t808Cowbell_init(t808Cowbell** const cowbellInst, int useStick, LEAF* const leaf)
{
    t808Cowbell_initToPool(cowbellInst, useStick, &leaf->mempool);
}

void        t808Cowbell_initToPool      (t808Cowbell** const cowbellInst, int useStick, tMempool** const mp)
{
    tMempool* m = *mp;
    t808Cowbell* cowbell = *cowbellInst = (t808Cowbell*) mpool_alloc(sizeof(t808Cowbell), m);
    cowbell->mempool = m;
    
    tSquare_initToPool(&cowbell->p[0], mp);
    tSquare_setFreq(cowbell->p[0], 540.0f);
    
    tSquare_initToPool(&cowbell->p[1], mp);
    tSquare_setFreq(cowbell->p[1], 1.48148f * 540.0f);
    
    cowbell->oscMix = 0.5f;
    
    tSVF_initToPool(&cowbell->bandpassOsc, SVFTypeBandpass, 2500, 1.0f, mp);
    
    tSVF_initToPool(&cowbell->bandpassStick, SVFTypeBandpass, 1800, 1.0f, mp);
    
    tEnvelope_initToPool(&cowbell->envGain, 5.0f, 100.0f, 0, mp);
    
    tEnvelope_initToPool(&cowbell->envFilter, 5.0, 100.0f, 0, mp);
    
    tHighpass_initToPool(&cowbell->highpass, 1000.0f, mp);
    
    tNoise_initToPool(&cowbell->stick, WhiteNoise, mp);
    
    tEnvelope_initToPool(&cowbell->envStick, 5.0f, 5.0f, 0, mp);
    
    cowbell->useStick = useStick;
}

void        t808Cowbell_free    (t808Cowbell** const cowbellInst)
{
    t808Cowbell* cowbell = *cowbellInst;
    
    tSquare_free(&cowbell->p[0]);
    tSquare_free(&cowbell->p[1]);
    tSVF_free(&cowbell->bandpassOsc);
    tSVF_free(&cowbell->bandpassStick);
    tEnvelope_free(&cowbell->envGain);
    tEnvelope_free(&cowbell->envFilter);
    tHighpass_free(&cowbell->highpass);
    tNoise_free(&cowbell->stick);
    tEnvelope_free(&cowbell->envStick);
    mpool_free((char*)cowbell, cowbell->mempool)t ;
}

void t808Cowbell_on(t808Cowbell* const cowbell, Lfloat vel)
{
    tEnvelope_on(cowbell->envGain, vel);
    
    if (cowbell->useStick)
        tEnvelope_on(cowbell->envStick,vel);
}

Lfloat t808Cowbell_tick(t808Cowbell* const cowbell)
{
    Lfloat sample = 0.0f;
    
    // Mix oscillators.
    sample = (cowbell->oscMix * tSquare_tick(cowbell->p[0])) + ((1.0f-cowbell->oscMix) * tSquare_tick(cowbell->p[1]));
    
    // Filter dive and filter.
    tSVF_setFreq(cowbell->bandpassOsc, cowbell->filterCutoff + 1000.0f * tEnvelope_tick(cowbell->envFilter));
    sample = tSVF_tick(cowbell->bandpassOsc,sample);
    
    sample *= (0.9f * tEnvelope_tick(cowbell->envGain));
    
    if (cowbell->useStick)
        sample += (0.1f * tEnvelope_tick(cowbell->envStick) * tSVF_tick(cowbell->bandpassStick, tNoise_tick(cowbell->stick)));

    
    sample = tHighpass_tick(cowbell->highpass, sample);
    
    return sample;
}

void t808Cowbell_setDecay(t808Cowbell* const cowbell, Lfloat decay)
{
    tEnvelope_setDecay(cowbell->envGain,decay);
}

void t808Cowbell_setHighpassFreq(t808Cowbell* cowbell, Lfloat freq)
{
    tHighpass_setFreq(cowbell->highpass,freq);
}

void t808Cowbell_setBandpassFreq(t808Cowbell* const cowbell, Lfloat freq)
{
    cowbell->filterCutoff = freq;
}

void t808Cowbell_setFreq(t808Cowbell* const cowbell, Lfloat freq)
{
    tSquare_setFreq(cowbell->p[0],freq);
    tSquare_setFreq(cowbell->p[1],1.48148f*freq);
}

void t808Cowbell_setOscMix(t808Cowbell* const cowbell, Lfloat oscMix)
{
    cowbell->oscMix = oscMix;
}

void t808Cowbell_setStick(t808Cowbell* const cowbell, int useStick)
{
    cowbell->useStick = useStick;
}

void t808Cowbell_setSampleRate(t808Cowbell* const cowbell, Lfloat sr)
{
    tSquare_setSampleRate(cowbell->p[0], sr);
    tSquare_setSampleRate(cowbell->p[1], sr);
    tSVF_setSampleRate(cowbell->bandpassOsc, sr);
    tSVF_setSampleRate(cowbell->bandpassStick, sr);
    tHighpass_setSampleRate(cowbell->highpass, sr);
}

// ----------------- HIHAT ----------------------------//

void t808Hihat_init(t808Hihat** const hihatInst, LEAF* const leaf)
{
    t808Hihat_initToPool(hihatInst, &leaf->mempool);
}

void    t808Hihat_initToPool  (t808Hihat** const hihatInst, tMempool** const mp)
{
    tMempool* m = *mp;
    t808Hihat* hihat = *hihatInst = (t808Hihat*) mpool_alloc(sizeof(t808Hihat), m);
    hihat->mempool = m;
    
    for (int i = 0; i < 6; i++)
    {
        tSquare_initToPool(&hihat->p[i], mp);
    }
    
    tNoise_initToPool(&hihat->stick, PinkNoise, mp);
    tNoise_initToPool(&hihat->n, WhiteNoise, mp);
    
    // need to fix SVF to be generic
    tSVF_initToPool(&hihat->bandpassStick, SVFTypeBandpass,2500.0f,1.2f, mp);
    tSVF_initToPool(&hihat->bandpassOsc, SVFTypeBandpass,3500.0f,0.3f, mp);
    
    tEnvelope_initToPool(&hihat->envGain, 0.0f, 50.0f, 0, mp);
    tEnvelope_initToPool(&hihat->envStick, 0.0f, 7.0f, 0, mp);
    
    
    tHighpass_initToPool(&hihat->highpass, 7000.0f, mp);
    
    hihat->freq = 40.0f;
    hihat->stretch = 0.0f;
    
    tSquare_setFreq(hihat->p[0], 2.0f * hihat->freq);
    tSquare_setFreq(hihat->p[1], 3.00f * hihat->freq);
    tSquare_setFreq(hihat->p[2], 4.16f * hihat->freq);
    tSquare_setFreq(hihat->p[3], 5.43f * hihat->freq);
    tSquare_setFreq(hihat->p[4], 6.79f * hihat->freq);
    tSquare_setFreq(hihat->p[5], 8.21f * hihat->freq);
}

void    t808Hihat_free  (t808Hihat** const hihatInst)
{
    t808Hihat* hihat = *hihatInst;
    
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
    
    mpool_free((char*)hihat, hihat->mempool);
}

void t808Hihat_on(t808Hihat* const hihat, Lfloat vel)
{
    tEnvelope_on(hihat->envGain, vel);
    tEnvelope_on(hihat->envStick, vel);
}

void t808Hihat_setOscNoiseMix(t808Hihat* const hihat, Lfloat oscNoiseMix)
{
    hihat->oscNoiseMix = oscNoiseMix;
}

Lfloat t808Hihat_tick(t808Hihat* const hihat)
{
    Lfloat sample = 0.0f;
    Lfloat gainScale = 0.1666f;

    Lfloat myNoise = tNoise_tick(hihat->n);

	tSquare_setFreq(hihat->p[0], ((2.0f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(hihat->p[1], ((3.00f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(hihat->p[2], ((4.16f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(hihat->p[3], ((5.43f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(hihat->p[4], ((6.79f + hihat->stretch) * hihat->freq));
	tSquare_setFreq(hihat->p[5], ((8.21f + hihat->stretch) * hihat->freq));

    for (int i = 0; i < 6; i++)
    {
        sample += tSquare_tick(hihat->p[i]);
    }
    
    sample *= gainScale;
    
    sample = (hihat->oscNoiseMix * sample) + ((1.0f-hihat->oscNoiseMix) * myNoise);
    
    sample = tSVF_tick(hihat->bandpassOsc, sample);
    
    Lfloat myGain = tEnvelope_tick(hihat->envGain);
    sample *= (myGain*myGain);//square the output gain envelope
    sample = tHighpass_tick(hihat->highpass, sample);
    sample += ((0.5f * tEnvelope_tick(hihat->envStick)) * tSVF_tick(hihat->bandpassStick, tNoise_tick(hihat->stick)));
    sample = tanhf(sample * 2.0f);

    return sample;
}

void t808Hihat_setDecay(t808Hihat* const hihat, Lfloat decay)
{
    tEnvelope_setDecay(hihat->envGain,decay);
}

void t808Hihat_setHighpassFreq(t808Hihat* const hihat, Lfloat freq)
{
    tHighpass_setFreq(hihat->highpass,freq);
}

void t808Hihat_setStretch(t808Hihat* const hihat, Lfloat stretch)
{
    hihat->stretch = stretch;
}

void t808Hihat_setFM(t808Hihat* const hihat, Lfloat FM_amount)
{
    hihat->FM_amount = FM_amount;
}

void t808Hihat_setOscBandpassFreq(t808Hihat* const hihat, Lfloat freq)
{
    tSVF_setFreq(hihat->bandpassOsc,freq);
}

void t808Hihat_setOscBandpassQ(t808Hihat* const hihat, Lfloat Q)
{
    tSVF_setQ(hihat->bandpassOsc,Q);
}

void t808Hihat_setStickBandPassFreq(t808Hihat* const hihat, Lfloat freq)
{
    tSVF_setFreq(hihat->bandpassStick,freq);
}

void t808Hihat_setStickBandPassQ(t808Hihat* const hihat, Lfloat Q)
{
    tSVF_setQ(hihat->bandpassStick,Q);
}

void t808Hihat_setOscFreq(t808Hihat* const hihat, Lfloat freq)
{
    hihat->freq = freq;
}

void t808Hihat_setSampleRate(t808Hihat* const hihat, Lfloat sr)
{
    for (int i = 0; i < 6; i++)
    {
        tSquare_setSampleRate(hihat->p[i], sr);
    }
    
    // need to fix SVF to be generic
    tSVF_setSampleRate(hihat->bandpassStick, sr);
    tSVF_setSampleRate(hihat->bandpassOsc, sr);
    
    tHighpass_setSampleRate(hihat->highpass, sr);
}

// ----------------- SNARE ----------------------------//

void t808Snare_init(t808Snare** const snareInst, LEAF* const leaf)
{
    t808Snare_initToPool(snareInst, &leaf->mempool);
}

void    t808Snare_initToPool    (t808Snare** const snareInst, tMempool** const mp)
{
    tMempool* m = *mp;
    t808Snare* snare = *snareInst = (t808Snare*) mpool_alloc(sizeof(t808Snare), m);
    snare->mempool = m;
    
    Lfloat ratio[2] = {1.0, 1.5};
    for (int i = 0; i < 2; i++)
    {
        tTriangle_initToPool(&snare->tone[i], mp);
        
        tTriangle_setFreq(snare->tone[i], ratio[i] * 400.0f);
        tSVF_initToPool(&snare->toneLowpass[i], SVFTypeLowpass, 4000, 1.0f, mp);
        tEnvelope_initToPool(&snare->toneEnvOsc[i], 0.0f, 50.0f, 0,  mp);
        tEnvelope_initToPool(&snare->toneEnvGain[i], 1.0f, 150.0f, 0, mp);
        tEnvelope_initToPool(&snare->toneEnvFilter[i], 1.0f, 2000.0f, 0, mp);
        
        snare->toneGain[i] = 0.5f;
    }
    
    snare->tone1Freq = ratio[0] * 100.0f;
    snare->tone2Freq = ratio[1] * 100.0f;
    snare->noiseFilterFreq = 3000.0f;
    tNoise_initToPool(&snare->noiseOsc, WhiteNoise, mp);
    tSVF_initToPool(&snare->noiseLowpass, SVFTypeLowpass, 12000.0f, 0.8f, mp);
    tEnvelope_initToPool(&snare->noiseEnvGain, 0.0f, 100.0f, 0, mp);
    tEnvelope_initToPool(&snare->noiseEnvFilter, 0.0f, 1000.0f, 0, mp);
    snare->noiseGain = 1.0f;
}

void    t808Snare_free  (t808Snare** const snareInst)
{
    t808Snare* snare = *snareInst;
    
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
    
    mpool_free((char*)snare, snare->mempool);
}

void t808Snare_on(t808Snare* const snare, Lfloat vel)
{
    for (int i = 0; i < 2; i++)
    {
        tEnvelope_on(snare->toneEnvOsc[i], vel);
        tEnvelope_on(snare->toneEnvGain[i], vel);
        tEnvelope_on(snare->toneEnvFilter[i], vel);
    }
    
    tEnvelope_on(snare->noiseEnvGain, vel);
    tEnvelope_on(snare->noiseEnvFilter, vel);
}

void t808Snare_setTone1Freq(t808Snare* const snare, Lfloat freq)
{
    snare->tone1Freq = freq;
    tTriangle_setFreq(snare->tone[0], freq);
}

void t808Snare_setTone2Freq(t808Snare* const snare, Lfloat freq)
{
    snare->tone2Freq = freq;
    tTriangle_setFreq(snare->tone[1],freq);
}

void t808Snare_setTone1Decay(t808Snare* const snare, Lfloat decay)
{
    tEnvelope_setDecay(snare->toneEnvGain[0],decay);
}

void t808Snare_setTone2Decay(t808Snare* const snare, Lfloat decay)
{
    tEnvelope_setDecay(snare->toneEnvGain[1],decay);
}

void t808Snare_setNoiseDecay(t808Snare* const snare, Lfloat decay)
{
    tEnvelope_setDecay(snare->noiseEnvGain,decay);
}

void t808Snare_setToneNoiseMix(t808Snare* const snare, Lfloat toneNoiseMix)
{
    snare->toneNoiseMix = toneNoiseMix;
}

void t808Snare_setNoiseFilterFreq(t808Snare* const snare, Lfloat noiseFilterFreq)
{
    snare->noiseFilterFreq = noiseFilterFreq;
}

void t808Snare_setNoiseFilterQ(t808Snare* const snare, Lfloat noiseFilterQ)
{
    tSVF_setQ(snare->noiseLowpass, noiseFilterQ);
}

static Lfloat tone[2];

Lfloat t808Snare_tick(t808Snare* const snare)
{
    Lfloat tone[2];
    for (int i = 0; i < 2; i++)
    {
        tTriangle_setFreq(snare->tone[i], snare->tone1Freq + (20.0f * tEnvelope_tick(snare->toneEnvOsc[i])));
        tone[i] = tTriangle_tick(snare->tone[i]);
        
        tSVF_setFreq(snare->toneLowpass[i], 2000.0f + (500.0f * tEnvelope_tick(snare->toneEnvFilter[i])));
        tone[i] = tSVF_tick(snare->toneLowpass[i], tone[i]) * tEnvelope_tick(snare->toneEnvGain[i]);
    }
    
    Lfloat noise = tNoise_tick(snare->noiseOsc);
    tSVF_setFreq(snare->noiseLowpass, snare->noiseFilterFreq + (1000.0f * tEnvelope_tick(snare->noiseEnvFilter)));
    noise = tSVF_tick(snare->noiseLowpass, noise) * tEnvelope_tick(snare->noiseEnvGain);
    
    Lfloat sample = (snare->toneNoiseMix)*(tone[0] * snare->toneGain[0] + tone[1] * snare->toneGain[1]) + (1.0f-snare->toneNoiseMix) * (noise * snare->noiseGain);
    sample = tanhf(sample * 2.0f);
    return sample;
}

void t808Snare_setSampleRate(t808Snare* const snare, Lfloat sr)
{
    for (int i = 0; i < 2; i++)
    {
        tTriangle_setSampleRate(snare->tone[i], sr);
        tSVF_setSampleRate(snare->toneLowpass[i], sr);
    }
    tSVF_setSampleRate(snare->noiseLowpass, sr);
}


// ----------------- SNARE WITHOUT ENVELOPE TABLE (SMALLER) ----------------------------//

void t808SnareSmall_init(t808SnareSmall** const snareInst, LEAF* const leaf)
{
    t808SnareSmall_initToPool(snareInst, &leaf->mempool);
}

void    t808SnareSmall_initToPool    (t808SnareSmall** const snareInst, tMempool** const mp)
{
    tMempool* m = *mp;
    t808SnareSmall* snare = *snareInst = (t808SnareSmall*) mpool_alloc(sizeof(t808SnareSmall), m);
    snare->mempool = m;
    
    Lfloat ratio[2] = {1.0, 1.5};
    for (int i = 0; i < 2; i++)
    {
        tPBTriangle_initToPool(&snare->tone[i], mp);
        tPBTriangle_setFreq(snare->tone[i], ratio[i] * 400.0f);
        tSVF_initToPool(&snare->toneLowpass[i], SVFTypeLowpass, 4000, 1.0f, mp);
        tADSRS_initToPool(&snare->toneEnvOsc[i], 0.0f, 50.0f, 0.0f, 50.0f, mp);
        tADSRS_initToPool(&snare->toneEnvGain[i], 1.0f, 150.0f, 0.0f, 50.0f, mp);
        tADSRS_initToPool(&snare->toneEnvFilter[i], 1.0f, 2000.0f, 0.0f, 50.0f, mp);
        
        snare->toneGain[i] = 0.5f;
    }
    
    snare->tone1Freq = ratio[0] * 100.0f;
    snare->tone2Freq = ratio[1] * 100.0f;
    snare->noiseFilterFreq = 3000.0f;
    tNoise_initToPool(&snare->noiseOsc, WhiteNoise, mp);
    tSVF_initToPool(&snare->noiseLowpass, SVFTypeLowpass, 12000.0f, 0.8f, mp);
    tADSRS_initToPool(&snare->noiseEnvGain, 0.0f, 100.0f, 0.0f, 50.0f, mp);
    tADSRS_initToPool(&snare->noiseEnvFilter, 0.0f, 1000.0f, 0.0f, 50.0f, mp);
    snare->noiseGain = 1.0f;
}

void    t808SnareSmall_free  (t808SnareSmall** const snareInst)
{
    t808SnareSmall* snare = *snareInst;
    
    for (int i = 0; i < 2; i++)
    {
        tPBTriangle_free(&snare->tone[i]);
        tSVF_free(&snare->toneLowpass[i]);
        tADSRS_free(&snare->toneEnvOsc[i]);
        tADSRS_free(&snare->toneEnvGain[i]);
        tADSRS_free(&snare->toneEnvFilter[i]);
    }
    
    tNoise_free(&snare->noiseOsc);
    tSVF_free(&snare->noiseLowpass);
    tADSRS_free(&snare->noiseEnvGain);
    tADSRS_free(&snare->noiseEnvFilter);
    
    mpool_free((char*)snare, snare->mempool);
}

void t808SnareSmall_on(t808SnareSmall* const snare, Lfloat vel)
{
    for (int i = 0; i < 2; i++)
    {
        tADSRS_on(snare->toneEnvOsc[i], vel);
        tADSRS_on(snare->toneEnvGain[i], vel);
        tADSRS_on(snare->toneEnvFilter[i], vel);
    }
    
    tADSRS_on(snare->noiseEnvGain, vel);
    tADSRS_on(snare->noiseEnvFilter, vel);
}

void t808SnareSmall_setTone1Freq(t808SnareSmall* const snare, Lfloat freq)
{
    snare->tone1Freq = freq;
    tPBTriangle_setFreq(snare->tone[0], freq);
}

void t808SnareSmall_setTone2Freq(t808SnareSmall* const snare, Lfloat freq)
{
    snare->tone2Freq = freq;
    tPBTriangle_setFreq(snare->tone[1],freq);
}

void t808SnareSmall_setTone1Decay(t808SnareSmall* const snare, Lfloat decay)
{
    tADSRS_setDecay(snare->toneEnvGain[0],decay);
}

void t808SnareSmall_setTone2Decay(t808SnareSmall* const snare, Lfloat decay)
{
    tADSRS_setDecay(snare->toneEnvGain[1],decay);
}

void t808SnareSmall_setNoiseDecay(t808SnareSmall* const snare, Lfloat decay)
{
    tADSRS_setDecay(snare->noiseEnvGain,decay);
}

void t808SnareSmall_setToneNoiseMix(t808SnareSmall* const snare, Lfloat toneNoiseMix)
{
    snare->toneNoiseMix = toneNoiseMix;
}

void t808SnareSmall_setNoiseFilterFreq(t808SnareSmall* const snare, Lfloat noiseFilterFreq)
{
    snare->noiseFilterFreq = noiseFilterFreq;
}

void t808SnareSmall_setNoiseFilterQ(t808SnareSmall* const snare, Lfloat noiseFilterQ)
{
    tSVF_setQ(snare->noiseLowpass, noiseFilterQ);
}


Lfloat t808SnareSmall_tick(t808SnareSmall* const snare)
{
    Lfloat tone[2];
    for (int i = 0; i < 2; i++)
    {
        tPBTriangle_setFreq(snare->tone[i], snare->tone1Freq + (20.0f * tADSRS_tick(snare->toneEnvOsc[i])));
        tone[i] = tPBTriangle_tick(snare->tone[i]);
        
        tSVF_setFreq(snare->toneLowpass[i], 2000.0f + (500.0f * tADSRS_tick(snare->toneEnvFilter[i])));
        tone[i] = tSVF_tick(snare->toneLowpass[i], tone[i]) * tADSRS_tick(snare->toneEnvGain[i]);
    }
    
    Lfloat noise = tNoise_tick(snare->noiseOsc);
    tSVF_setFreq(snare->noiseLowpass, snare->noiseFilterFreq + (1000.0f * tADSRS_tick(snare->noiseEnvFilter)));
    noise = tSVF_tick(snare->noiseLowpass, noise) * tADSRS_tick(snare->noiseEnvGain);
    
    Lfloat sample = (snare->toneNoiseMix)*(tone[0] * snare->toneGain[0] + tone[1] * snare->toneGain[1]) + (1.0f-snare->toneNoiseMix) * (noise * snare->noiseGain);
    //sample = tanhf(sample * 2.0f);
    return sample;
}

void t808SnareSmall_setSampleRate(t808SnareSmall* const snare, Lfloat sr)
{
    for (int i = 0; i < 2; i++)
    {
        tPBTriangle_setSampleRate(snare->tone[i], sr);
        tSVF_setSampleRate(snare->toneLowpass[i], sr);
    }
    tSVF_setSampleRate(snare->noiseLowpass, sr);
}

// ----------------- KICK ----------------------------//

void t808Kick_init(t808Kick** const kickInst, LEAF* const leaf)
{
    t808Kick_initToPool(kickInst, &leaf->mempool);
}

void t808Kick_initToPool (t808Kick** const kickInst, tMempool** const mp)
{
    tMempool* m = *mp;
    t808Kick* kick = *kickInst = (t808Kick*) mpool_alloc(sizeof(t808Kick), m);
    kick->mempool = m;
    
    tCycle_initToPool(&kick->tone, mp);
    kick->toneInitialFreq = 40.0f;
    kick->sighAmountInHz = 7.0f;
    kick->chirpRatioMinusOne = 3.3f;
    tCycle_setFreq(kick->tone, 50.0f);
    tSVF_initToPool(&kick->toneLowpass, SVFTypeLowpass, 2000.0f, 0.5f, mp);
    tEnvelope_initToPool(&kick->toneEnvOscChirp, 0.0f, 20.0f, 0, mp);
    tEnvelope_initToPool(&kick->toneEnvOscSigh, 0.0f, 2500.0f, 0, mp);
    tEnvelope_initToPool(&kick->toneEnvGain, 0.0f, 800.0f, 0, mp);
    tNoise_initToPool(&kick->noiseOsc, PinkNoise, mp);
    tEnvelope_initToPool(&kick->noiseEnvGain, 0.0f, 1.0f, 0, mp);
    kick->noiseGain = 0.3f;
}

void    t808Kick_free   (t808Kick** const kickInst)
{
    t808Kick* kick = *kickInst;
    
    tCycle_free(&kick->tone);
    tSVF_free(&kick->toneLowpass);
    tEnvelope_free(&kick->toneEnvOscChirp);
    tEnvelope_free(&kick->toneEnvOscSigh);
    tEnvelope_free(&kick->toneEnvGain);
    tNoise_free(&kick->noiseOsc);
    tEnvelope_free(&kick->noiseEnvGain);
    
    mpool_free((char*)kick, kick->mempool);
}

Lfloat       t808Kick_tick                  (t808Kick* const kick)
{
	tCycle_setFreq(kick->tone, (kick->toneInitialFreq * (1.0f + (kick->chirpRatioMinusOne * tEnvelope_tick(kick->toneEnvOscChirp)))) + (kick->sighAmountInHz * tEnvelope_tick(kick->toneEnvOscSigh)));
	Lfloat sample = tCycle_tick(kick->tone) * tEnvelope_tick(kick->toneEnvGain);
	sample+= tNoise_tick(kick->noiseOsc) * tEnvelope_tick(kick->noiseEnvGain);
	//add distortion here
	sample = tSVF_tick(kick->toneLowpass, sample);
	return sample;
}

void        t808Kick_on                    (t808Kick* const kick, Lfloat vel)
{
	tEnvelope_on(kick->toneEnvOscChirp, vel);
	tEnvelope_on(kick->toneEnvOscSigh, vel);
	tEnvelope_on(kick->toneEnvGain, vel);
	tEnvelope_on(kick->noiseEnvGain, vel);

}
void        t808Kick_setToneFreq          (t808Kick* const kick, Lfloat freq)
{
	kick->toneInitialFreq = freq;

}

void        t808Kick_setToneDecay         (t808Kick* const kick, Lfloat decay)
{
	tEnvelope_setDecay(kick->toneEnvGain,decay);
	tEnvelope_setDecay(kick->toneEnvGain,decay * 3.0f);
}


void        t808Kick_setSampleRate  (t808Kick** const kickInst, Lfloat sr)
{
    t808Kick* kick = *kickInst;
    
    tCycle_setSampleRate(kick->tone, sr);
    tSVF_setSampleRate(kick->toneLowpass, sr);
}


// ----------------- KICK ----------------------------//

void t808KickSmall_init(t808KickSmall** const kickInst, LEAF* const leaf)
{
    t808KickSmall_initToPool(kickInst, &leaf->mempool);
}

void t808KickSmall_initToPool (t808KickSmall** const kickInst, tMempool** const mp)
{
    tMempool* m = *mp;
    t808KickSmall* kick = *kickInst = (t808KickSmall*) mpool_alloc(sizeof(t808KickSmall), m);
    kick->mempool = m;
    
    tCycle_initToPool(&kick->tone, mp);
    kick->toneInitialFreq = 40.0f;
    kick->sighAmountInHz = 7.0f;
    kick->chirpRatioMinusOne = 3.3f;
    tCycle_setFreq(kick->tone, 50.0f);
    tSVF_initToPool(&kick->toneLowpass, SVFTypeLowpass, 2000.0f, 0.5f, mp);
    tADSRS_initToPool(&kick->toneEnvOscChirp, 0.0f, 20.0f, 0.0f, 0.0f, mp);
    tADSRS_initToPool(&kick->toneEnvOscSigh, 0.0f, 2500.0f, 0.0f, 0.0f, mp);
    tADSRS_initToPool(&kick->toneEnvGain, 0.0f, 800.0f, 0.0f, 0.0f, mp);
    tNoise_initToPool(&kick->noiseOsc, PinkNoise, mp);
    tADSRS_initToPool(&kick->noiseEnvGain, 0.0f, 1.0f, 0.0f, 0.0f, mp);
    kick->noiseGain = 0.3f;
}

void    t808KickSmall_free   (t808KickSmall** const kickInst)
{
    t808KickSmall* kick = *kickInst;
    
    tCycle_free(&kick->tone);
    tSVF_free(&kick->toneLowpass);
    tADSRS_free(&kick->toneEnvOscChirp);
    tADSRS_free(&kick->toneEnvOscSigh);
    tADSRS_free(&kick->toneEnvGain);
    tNoise_free(&kick->noiseOsc);
    tADSRS_free(&kick->noiseEnvGain);
    
    mpool_free((char*)kick, kick->mempool);
}

Lfloat       t808KickSmall_tick                  (t808KickSmall* const kick)
{
	tCycle_setFreq(kick->tone, (kick->toneInitialFreq * (1.0f + (kick->chirpRatioMinusOne * tADSRS_tick(kick->toneEnvOscChirp)))) + (kick->sighAmountInHz * tADSRS_tick(kick->toneEnvOscSigh)));
	Lfloat sample = tCycle_tick(kick->tone) * tADSRS_tick(kick->toneEnvGain);
	sample+= tNoise_tick(kick->noiseOsc) * tADSRS_tick(kick->noiseEnvGain);
	//add distortion here
	sample = tSVF_tick(kick->toneLowpass, sample);
	return sample;
}

void        t808KickSmall_on                    (t808KickSmall* const kick, Lfloat vel)
{
	tADSRS_on(kick->toneEnvOscChirp, vel);
	tADSRS_on(kick->toneEnvOscSigh, vel);
	tADSRS_on(kick->toneEnvGain, vel);
	tADSRS_on(kick->noiseEnvGain, vel);

}
void        t808KickSmall_setToneFreq          (t808KickSmall* const kick, Lfloat freq)
{
	kick->toneInitialFreq = freq;

}

void        t808KickSmall_setToneDecay         (t808KickSmall* const kick, Lfloat decay)
{
	tADSRS_setDecay(kick->toneEnvGain,decay);
	tADSRS_setDecay(kick->toneEnvGain,decay * 3.0f);
}


void        t808KickSmall_setSampleRate  (t808KickSmall* const kick, Lfloat sr)
{
    tCycle_setSampleRate(kick->tone, sr);
    tSVF_setSampleRate(kick->toneLowpass, sr);
}



