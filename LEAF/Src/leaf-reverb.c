/*==============================================================================

    leaf-reverb.c
    Created: 20 Jan 2017 12:02:04pm
    Author:  Michael R Mulshine

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-reverb.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-reverb.h"
#include "../leaf.h"

#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ PRCReverb ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tPRCReverb_init(tPRCReverb* const rev, float t60)
{
    _tPRCReverb* r = *rev = (_tPRCReverb*) leaf_alloc(sizeof(_tPRCReverb));
    
    if (t60 <= 0.0f) t60 = 0.001f;
    
    r->inv_441 = 1.0f/44100.0f;
    
    int lengths[4] = { 341, 613, 1557, 2137 }; // Delay lengths for 44100 Hz sample rate.
    double scaler = leaf.sampleRate * r->inv_441;
    
    int delay, i;
    if (scaler != 1.0f)
    {
        for (i=0; i<4; i++)
        {
            delay = (int) scaler * lengths[i];
            
            if ( (delay & 1) == 0)          delay++;
            
            while ( !LEAF_isPrime(delay) )  delay += 2;
            
            lengths[i] = delay;
        }
    }
    
    tDelay_init(&r->allpassDelays[0], lengths[0], lengths[0] * 2);
    tDelay_init(&r->allpassDelays[1], lengths[1], lengths[1] * 2);
    tDelay_init(&r->combDelay, lengths[2], lengths[2] * 2);
    
    tPRCReverb_setT60(rev, t60);
    
    r->allpassCoeff = 0.7f;
    r->mix = 0.5f;
}

void tPRCReverb_free(tPRCReverb* const rev)
{
    _tPRCReverb* r = *rev;
    
    tDelay_free(&r->allpassDelays[0]);
    tDelay_free(&r->allpassDelays[1]);
    tDelay_free(&r->combDelay);
    leaf_free((char*)r);
}

void    tPRCReverb_initToPool   (tPRCReverb* const rev, float t60, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPRCReverb* r = *rev = (_tPRCReverb*) mpool_alloc(sizeof(_tPRCReverb), m);
    
    if (t60 <= 0.0f) t60 = 0.001f;
    
    r->inv_441 = 1.0f/44100.0f;
    
    int lengths[4] = { 341, 613, 1557, 2137 }; // Delay lengths for 44100 Hz sample rate.
    double scaler = leaf.sampleRate * r->inv_441;
    
    int delay, i;
    if (scaler != 1.0f)
    {
        for (i=0; i<4; i++)
        {
            delay = (int) scaler * lengths[i];
            
            if ( (delay & 1) == 0)          delay++;
            
            while ( !LEAF_isPrime(delay) )  delay += 2;
            
            lengths[i] = delay;
        }
    }
    
    tDelay_initToPool(&r->allpassDelays[0], lengths[0], lengths[0] * 2, mp);
    tDelay_initToPool(&r->allpassDelays[1], lengths[1], lengths[1] * 2, mp);
    tDelay_initToPool(&r->combDelay, lengths[2], lengths[2] * 2, mp);
    
    tPRCReverb_setT60(rev, t60);
    
    r->allpassCoeff = 0.7f;
    r->mix = 0.5f;
}

void    tPRCReverb_freeFromPool (tPRCReverb* const rev, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPRCReverb* r = *rev;
    
    tDelay_freeFromPool(&r->allpassDelays[0], mp);
    tDelay_freeFromPool(&r->allpassDelays[1], mp);
    tDelay_freeFromPool(&r->combDelay, mp);
    mpool_free((char*)r, m);
}

void    tPRCRevert_clear(tPRCReverb* const rev)
{
    _tPRCReverb* r = *rev;
    
    tDelay_clear(&r->allpassDelays[0]);
    tDelay_clear(&r->allpassDelays[1]);
    tDelay_clear(&r->combDelay);
}

void    tPRCReverb_setT60(tPRCReverb* const rev, float t60)
{
    _tPRCReverb* r = *rev;
    
    if ( t60 <= 0.0f ) t60 = 0.001f;
    
    r->t60 = t60;
    
    r->combCoeff = pow(10.0f, (-3.0f * tDelay_getDelay(&r->combDelay) * leaf.invSampleRate / t60 ));
    
}

void    tPRCReverb_setMix(tPRCReverb* const rev, float mix)
{
    _tPRCReverb* r = *rev;
    r->mix = mix;
}

float   tPRCReverb_tick(tPRCReverb* const rev, float input)
{
    _tPRCReverb* r = *rev;
    
    float temp, temp0, temp1, temp2;
    float out;
    
    r->lastIn = input;
    
    temp = tDelay_getLastOut(&r->allpassDelays[0]);
    temp0 = r->allpassCoeff * temp;
    temp0 += input;
    tDelay_tick(&r->allpassDelays[0], temp0);
    temp0 = -( r->allpassCoeff * temp0) + temp;
    
    temp = tDelay_getLastOut(&r->allpassDelays[1]);
    temp1 = r->allpassCoeff * temp;
    temp1 += temp0;
    tDelay_tick(&r->allpassDelays[1], temp1);
    temp1 = -(r->allpassCoeff * temp1) + temp;
    
    temp2 = temp1 + ( r->combCoeff * tDelay_getLastOut(&r->combDelay));
    
    out = r->mix * tDelay_tick(&r->combDelay, temp2);
    
    temp = (1.0f - r->mix) * input;
    
    out += temp;
    
    r->lastOut = out;
    
    return out;
}

void     tPRCReverbSampleRateChanged (tPRCReverb* const rev)
{
    _tPRCReverb* r = *rev;
    r->combCoeff = pow(10.0f, (-3.0f * tDelay_getDelay(&r->combDelay) * leaf.invSampleRate / r->t60 ));
}

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NReverb ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void    tNReverb_init(tNReverb* const rev, float t60)
{
    _tNReverb* r = *rev = (_tNReverb*) leaf_alloc(sizeof(_tNReverb));
    
    if (t60 <= 0.0f) t60 = 0.001f;
    
    r->inv_441 = 1.0f/44100.0f;
    
    int lengths[15] = {1433, 1601, 1867, 2053, 2251, 2399, 347, 113, 37, 59, 53, 43, 37, 29, 19}; // Delay lengths for 44100 Hz sample rate.
    double scaler = leaf.sampleRate / 25641.0f;
    
    int delay, i;
    
    for (i=0; i < 15; i++)
    {
        delay = (int) scaler * lengths[i];
        if ( (delay & 1) == 0)
            delay++;
        while ( !LEAF_isPrime(delay) )
            delay += 2;
        lengths[i] = delay;
    }
    
    for ( i=0; i<6; i++ )
    {
    	tLinearDelay_init(&r->combDelays[i], lengths[i], lengths[i] * 2.0f);
    	tLinearDelay_clear(&r->combDelays[i]);
        r->combCoeffs[i] = pow(10.0, (-3 * lengths[i] * leaf.invSampleRate / t60));
    }
    
    for ( i=0; i<8; i++ )
    {
    	tLinearDelay_init(&r->allpassDelays[i], lengths[i+6], lengths[i+6] * 2.0f);
    	tLinearDelay_clear(&r->allpassDelays[i]);
    }

    
    tNReverb_setT60(rev, t60);
    r->allpassCoeff = 0.7f;
    r->mix = 0.3f;
}

void    tNReverb_free(tNReverb* const rev)
{
    _tNReverb* r = *rev;
    
    for (int i = 0; i < 6; i++)
    {
    	tLinearDelay_free(&r->combDelays[i]);
    }
    
    for (int i = 0; i < 8; i++)
    {
    	tLinearDelay_free(&r->allpassDelays[i]);
    }
    
    leaf_free((char*)r);
}

void    tNReverb_initToPool     (tNReverb* const rev, float t60, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNReverb* r = *rev = (_tNReverb*) mpool_alloc(sizeof(_tNReverb), m);
    
    if (t60 <= 0.0f) t60 = 0.001f;
    
    r->inv_441 = 1.0f/44100.0f;
    
    int lengths[15] = {1433, 1601, 1867, 2053, 2251, 2399, 347, 113, 37, 59, 53, 43, 37, 29, 19}; // Delay lengths for 44100 Hz sample rate.
    double scaler = leaf.sampleRate / 25641.0f;
    
    int delay, i;
    
    for (i=0; i < 15; i++)
    {
        delay = (int) scaler * lengths[i];
        if ( (delay & 1) == 0)
            delay++;
        while ( !LEAF_isPrime(delay) )
            delay += 2;
        lengths[i] = delay;
    }
    
    for ( i=0; i<6; i++ )
    {
        tLinearDelay_initToPool(&r->combDelays[i], lengths[i], lengths[i] * 2.0f, mp);
        r->combCoeffs[i] = pow(10.0, (-3 * lengths[i] * leaf.invSampleRate / t60));
    }
    
    for ( i=0; i<8; i++ )
    {
        tLinearDelay_initToPool(&r->allpassDelays[i], lengths[i+6], lengths[i+6] * 2.0f, mp);
    }
    
    
    tNReverb_setT60(rev, t60);
    r->allpassCoeff = 0.7f;
    r->mix = 0.3f;
}

void    tNReverb_freeFromPool   (tNReverb* const rev, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tNReverb* r = *rev;
    
    for (int i = 0; i < 6; i++)
    {
        tLinearDelay_freeFromPool(&r->combDelays[i], mp);
    }
    
    for (int i = 0; i < 8; i++)
    {
        tLinearDelay_freeFromPool(&r->allpassDelays[i], mp);
    }
    
    mpool_free((char*)r, m);
}

void    tNReverb_setT60(tNReverb* const rev, float t60)
{
    _tNReverb* r = *rev;
    
    if (t60 <= 0.0f)           t60 = 0.001f;
    
    r->t60 = t60;
    
    for (int i=0; i<6; i++)   r->combCoeffs[i] = pow(10.0, (-3.0 * tLinearDelay_getDelay(&r->combDelays[i]) * leaf.invSampleRate / t60 ));
    
}

void    tNReverb_setMix(tNReverb* const rev, float mix)
{
    _tNReverb* r = *rev;
    r->mix = mix;
}

void    tNReverb_clear             (tNReverb* const rev)
{
    _tNReverb* r = *rev;
    
    for (int i = 0; i < 6; i++)
    {
        tLinearDelay_clear(&r->combDelays[i]);
    }
    
    for (int i = 0; i < 8; i++)
    {
        tLinearDelay_clear(&r->allpassDelays[i]);
    }
}

float   tNReverb_tick(tNReverb* const rev, float input)
{
    _tNReverb* r = *rev;
    r->lastIn = input;
    
    float temp, temp0, temp1, temp2, out;
    int i;
    
    temp0 = 0.0;
    for ( i=0; i<6; i++ )
    {
        temp = input + (r->combCoeffs[i] * tLinearDelay_getLastOut(&r->combDelays[i]));
        temp0 += tLinearDelay_tick(&r->combDelays[i],temp);
    }
    
    for ( i=0; i<3; i++ )
    {
        temp = tLinearDelay_getLastOut(&r->allpassDelays[i]);
        temp1 = r->allpassCoeff * temp;
        temp1 += temp0;
        tLinearDelay_tick(&r->allpassDelays[i], temp1);
        temp0 = -(r->allpassCoeff * temp1) + temp;
    }
    
    // One-pole lowpass filter.
    r->lowpassState = 0.7f * r->lowpassState + 0.3f * temp0;

    temp = tLinearDelay_getLastOut(&r->allpassDelays[3]);
    temp1 = r->allpassCoeff * temp;
    temp1 += r->lowpassState;
    tLinearDelay_tick(&r->allpassDelays[3], temp1 );
    temp1 = -(r->allpassCoeff * temp1) + temp;
    
    temp = tLinearDelay_getLastOut(&r->allpassDelays[4]);
    temp2 = r->allpassCoeff * temp;
    temp2 += temp1;
    tLinearDelay_tick(&r->allpassDelays[4], temp2 );
    out = -( r->allpassCoeff * temp2 ) + temp ;
    
    //the other channel in stereo version below
/*
     temp = tLinearDelay_getLastOut(&r->allpassDelays[5]);
     temp3 = r->allpassCoeff * temp;
     temp3 += temp1;
     tLinearDelay_tick(&r->allpassDelays[5], temp3 );
     out = r->mix *( - ( r->allpassCoeff * temp3 ) + temp );
*/

    temp = ( 1.0f - r->mix ) * input;

    out += temp;

    r->lastOut = out;

    return out;
}

void   tNReverb_tickStereo(tNReverb* const rev, float input, float* output)
{
    _tNReverb* r = *rev;
    r->lastIn = input;

    float temp, temp0, temp1, temp2, temp3, out;
    int i;

    temp0 = 0.0;
    for ( i=0; i<6; i++ )
    {
        temp = input + (r->combCoeffs[i] * tLinearDelay_getLastOut(&r->combDelays[i]));
        temp0 += tLinearDelay_tick(&r->combDelays[i],temp);
    }

    for ( i=0; i<3; i++ )
    {
        temp = tLinearDelay_getLastOut(&r->allpassDelays[i]);
        temp1 = r->allpassCoeff * temp;
        temp1 += temp0;
        tLinearDelay_tick(&r->allpassDelays[i], temp1);
        temp0 = -(r->allpassCoeff * temp1) + temp;
    }

    // One-pole lowpass filter.
    r->lowpassState = 0.7f * r->lowpassState + 0.3f * temp0;

    temp = tLinearDelay_getLastOut(&r->allpassDelays[3]);
    temp1 = r->allpassCoeff * temp;
    temp1 += r->lowpassState;
    tLinearDelay_tick(&r->allpassDelays[3], temp1 );
    temp1 = -(r->allpassCoeff * temp1) + temp;

    float drymix = ( 1.0f - r->mix ) * input;

    temp = tLinearDelay_getLastOut(&r->allpassDelays[4]);
    temp2 = r->allpassCoeff * temp;
    temp2 += temp1;
    tLinearDelay_tick(&r->allpassDelays[4], temp2 );
    output[0] = -( r->allpassCoeff * temp2 ) + temp + drymix;
    out = output[0];


    temp = tLinearDelay_getLastOut(&r->allpassDelays[5]);
    temp3 = r->allpassCoeff * temp;
    temp3 += temp1;
    tLinearDelay_tick(&r->allpassDelays[5], temp3 );
    output[1] = r->mix *( - ( r->allpassCoeff * temp3 ) + temp + drymix);

    r->lastOut = out;
}

void     tNReverbSampleRateChanged (tNReverb* const rev)
{
    _tNReverb* r = *rev;
    for (int i=0; i<6; i++)   r->combCoeffs[i] = pow(10.0, (-3.0 * tLinearDelay_getDelay(&r->combDelays[i]) * leaf.invSampleRate / r->t60 ));
}

// ======================================DATTORRO=========================================

#define SAMP(in) (in*r->t)

float       in_allpass_delays[4] = { 4.771f, 3.595f, 12.73f, 9.307f };
float       in_allpass_gains[4] = { 0.75f, 0.75f, 0.625f, 0.625f };


void    tDattorroReverb_init              (tDattorroReverb* const rev)
{
    tDattorroReverb_initToPool(rev, &leaf.mempool);
}

void    tDattorroReverb_free              (tDattorroReverb* const rev)
{
    tDattorroReverb_freeFromPool(rev, &leaf.mempool);
}

void    tDattorroReverb_initToPool        (tDattorroReverb* const rev, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tDattorroReverb* r = *rev = (_tDattorroReverb*) mpool_alloc(sizeof(_tDattorroReverb), m);
    
    r->size_max = 2.0f;
    r->size = 1.f;
    r->t = r->size * leaf.sampleRate * 0.001f;
    r->frozen = 0;
    // INPUT
    tTapeDelay_initToPool(&r->in_delay, 0.f, SAMP(200.f), mp);
    tOnePole_initToPool(&r->in_filter, 1.f, mp);
    
    for (int i = 0; i < 4; i++)
    {
        tAllpass_initToPool(&r->in_allpass[i], SAMP(in_allpass_delays[i]), SAMP(20.f), mp); // * r->size_max
        tAllpass_setGain(&r->in_allpass[i], in_allpass_gains[i]);
    }
    
    // FEEDBACK 1
    tAllpass_initToPool(&r->f1_allpass, SAMP(30.51f), SAMP(100.f), mp); // * r->size_max
    tAllpass_setGain(&r->f1_allpass, 0.7f);
    
    tTapeDelay_initToPool(&r->f1_delay_1, SAMP(141.69f), SAMP(200.0f) * r->size_max + 1, mp);
    tTapeDelay_initToPool(&r->f1_delay_2, SAMP(89.24f), SAMP(100.0f) * r->size_max + 1, mp);
    tTapeDelay_initToPool(&r->f1_delay_3, SAMP(125.f), SAMP(200.0f) * r->size_max + 1, mp);
    
    tOnePole_initToPool(&r->f1_filter, 1.f, mp);
    
    tHighpass_initToPool(&r->f1_hp, 20.f, mp);
    
    tCycle_initToPool(&r->f1_lfo, mp);
    tCycle_setFreq(&r->f1_lfo, 0.1f);
    
    // FEEDBACK 2
    tAllpass_initToPool(&r->f2_allpass, SAMP(22.58f), SAMP(100.f), mp); // * r->size_max
    tAllpass_setGain(&r->f2_allpass, 0.7f);
    
    tTapeDelay_initToPool(&r->f2_delay_1, SAMP(149.62f), SAMP(200.f) * r->size_max + 1, mp);
    tTapeDelay_initToPool(&r->f2_delay_2, SAMP(60.48f), SAMP(100.f) * r->size_max + 1, mp);
    tTapeDelay_initToPool(&r->f2_delay_3, SAMP(106.28f), SAMP(200.f) * r->size_max + 1, mp);
    
    tOnePole_initToPool(&r->f2_filter, 1.f, mp);
    
    tHighpass_initToPool(&r->f2_hp, 20.f, mp);
    
    tCycle_initToPool(&r->f2_lfo, mp);
    tCycle_setFreq(&r->f2_lfo, 0.07f);
    
    
    // PARAMETERS
    tDattorroReverb_setMix(rev, 0.5f);
    
    tDattorroReverb_setInputDelay(rev,  0.f);
    
    tDattorroReverb_setInputFilter(rev, 10000.f);
    
    tDattorroReverb_setFeedbackFilter(rev, 5000.f);
    
    tDattorroReverb_setFeedbackGain(rev, 0.4f);
}

void    tDattorroReverb_freeFromPool      (tDattorroReverb* const rev, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tDattorroReverb* r = *rev;
    
    // INPUT
    tTapeDelay_freeFromPool(&r->in_delay, mp);
    tOnePole_freeFromPool(&r->in_filter, mp);
    
    for (int i = 0; i < 4; i++)
    {
        tAllpass_freeFromPool(&r->in_allpass[i], mp);
    }
    
    // FEEDBACK 1
    tAllpass_freeFromPool(&r->f1_allpass, mp);
    
    tTapeDelay_freeFromPool(&r->f1_delay_1, mp);
    tTapeDelay_freeFromPool(&r->f1_delay_2, mp);
    tTapeDelay_freeFromPool(&r->f1_delay_3, mp);
    
    tOnePole_freeFromPool(&r->f1_filter, mp);
    
    tHighpass_freeFromPool(&r->f1_hp, mp);
    
    tCycle_freeFromPool(&r->f1_lfo, mp);
    
    // FEEDBACK 2
    tAllpass_freeFromPool(&r->f2_allpass, mp);
    
    tTapeDelay_freeFromPool(&r->f2_delay_1, mp);
    tTapeDelay_freeFromPool(&r->f2_delay_2, mp);
    tTapeDelay_freeFromPool(&r->f2_delay_3, mp);
    
    tOnePole_freeFromPool(&r->f2_filter, mp);
    
    tHighpass_freeFromPool(&r->f2_hp, mp);
    
    tCycle_freeFromPool(&r->f2_lfo, mp);
    
    mpool_free((char*)r, m);
}

void    tDattorroReverb_clear             (tDattorroReverb* const rev)
{
    _tDattorroReverb* r = *rev;
    
    tTapeDelay_clear(&r->in_delay);
    tTapeDelay_clear(&r->f1_delay_1);
    tTapeDelay_clear(&r->f1_delay_2);
    tTapeDelay_clear(&r->f1_delay_3);
    tTapeDelay_clear(&r->f2_delay_1);
    tTapeDelay_clear(&r->f2_delay_2);
    tTapeDelay_clear(&r->f2_delay_3);
}

float   tDattorroReverb_tick              (tDattorroReverb* const rev, float input)
{
    _tDattorroReverb* r = *rev;
    

    float in_sample, f1_sample,f1_delay_2_sample,  f2_sample, f2_delay_2_sample;

    if (r->frozen)
    {
    	input = 0.0f;
    	//r->f1_last = 0.0f;
    	//r->f2_last = 0.0f;
	}
	// INPUT
	in_sample = tTapeDelay_tick(&r->in_delay, input);

	in_sample = tOnePole_tick(&r->in_filter, in_sample);

	for (int i = 0; i < 4; i++)
	{
		in_sample = tAllpass_tick(&r->in_allpass[i], in_sample);
	}

	// FEEDBACK 1
	f1_sample = in_sample + r->f2_last; // + f2_last_out;

	tAllpass_setDelay(&r->f1_allpass, SAMP(30.51f) + tCycle_tick(&r->f1_lfo) * SAMP(4.0f));

	f1_sample = tAllpass_tick(&r->f1_allpass, f1_sample);

	f1_sample = tTapeDelay_tick(&r->f1_delay_1, f1_sample);

	f1_sample = tOnePole_tick(&r->f1_filter, f1_sample);

	f1_sample = f1_sample + r->f1_delay_2_last * 0.5f;

	f1_delay_2_sample = tTapeDelay_tick(&r->f1_delay_2, f1_sample * 0.5f);

	r->f1_delay_2_last = f1_delay_2_sample;

	f1_sample = r->f1_delay_2_last + f1_sample;

	f1_sample = tHighpass_tick(&r->f1_hp, f1_sample);

	f1_sample *= r->feedback_gain;

	r->f1_last = tTapeDelay_tick(&r->f1_delay_3, f1_sample);

	// FEEDBACK 2
	f2_sample = in_sample + r->f1_last;

	tAllpass_setDelay(&r->f2_allpass, SAMP(22.58f) + tCycle_tick(&r->f2_lfo) * SAMP(4.0f));

	f2_sample = tAllpass_tick(&r->f2_allpass, f2_sample);

	f2_sample = tTapeDelay_tick(&r->f2_delay_1, f2_sample);

	f2_sample = tOnePole_tick(&r->f2_filter, f2_sample);

	f2_sample = f2_sample + r->f2_delay_2_last * 0.5f;

	f2_delay_2_sample = tTapeDelay_tick(&r->f2_delay_2, f2_sample * 0.5f);

	r->f2_delay_2_last = f2_delay_2_sample;

	f2_sample = r->f2_delay_2_last + f2_sample;

	f2_sample = tHighpass_tick(&r->f2_hp, f2_sample);

	f2_sample *= r->feedback_gain;

	r->f2_last = tTapeDelay_tick(&r->f2_delay_3, f2_sample);
    
    // TAP OUT 1
    f1_sample =     tTapeDelay_tapOut(&r->f1_delay_1, SAMP(8.9f)) +
    tTapeDelay_tapOut(&r->f1_delay_1, SAMP(99.8f));
    
    f1_sample -=    tTapeDelay_tapOut(&r->f1_delay_2, SAMP(64.2f));
    
    f1_sample +=    tTapeDelay_tapOut(&r->f1_delay_3, SAMP(67.f));
    
    f1_sample -=    tTapeDelay_tapOut(&r->f2_delay_1, SAMP(66.8f));
    
    f1_sample -=    tTapeDelay_tapOut(&r->f2_delay_2, SAMP(6.3f));
    
    f1_sample -=    tTapeDelay_tapOut(&r->f2_delay_3, SAMP(35.8f));
    
    f1_sample *=    0.14f;
    
    // TAP OUT 2
    f2_sample =     tTapeDelay_tapOut(&r->f2_delay_1, SAMP(11.8f)) +
    tTapeDelay_tapOut(&r->f2_delay_1, SAMP(121.7f));
    
    f2_sample -=    tTapeDelay_tapOut(&r->f2_delay_2, SAMP(6.3f));
    
    f2_sample +=    tTapeDelay_tapOut(&r->f2_delay_3, SAMP(89.7f));
    
    f2_sample -=    tTapeDelay_tapOut(&r->f1_delay_1, SAMP(70.8f));
    
    f2_sample -=    tTapeDelay_tapOut(&r->f1_delay_2, SAMP(11.2f));
    
    f2_sample -=    tTapeDelay_tapOut(&r->f1_delay_3, SAMP(4.1f));
    
    f2_sample *=    0.14f;
    
    float sample = (f1_sample + f2_sample) * 0.5f;
    
    return (input * (1.0f - r->mix) + sample * r->mix);
}

void   tDattorroReverb_tickStereo              (tDattorroReverb* const rev, float input, float* output)
{
    _tDattorroReverb* r = *rev;
    float in_sample, f1_sample,f1_delay_2_sample,  f2_sample, f2_delay_2_sample;

    if (r->frozen)
    {
    	input = 0.0f;
    	//r->f1_last = 0.0f;
    	//r->f2_last = 0.0f;
    }
	// INPUT
	in_sample = tTapeDelay_tick(&r->in_delay, input);

	in_sample = tOnePole_tick(&r->in_filter, in_sample);

	for (int i = 0; i < 4; i++)
	{
		in_sample = tAllpass_tick(&r->in_allpass[i], in_sample);
	}


		// FEEDBACK 1
		f1_sample = in_sample + r->f2_last; // + f2_last_out;

		tAllpass_setDelay(&r->f1_allpass, SAMP(30.51f) + tCycle_tick(&r->f1_lfo) * SAMP(4.0f));

		f1_sample = tAllpass_tick(&r->f1_allpass, f1_sample);

		f1_sample = tTapeDelay_tick(&r->f1_delay_1, f1_sample);

		f1_sample = tOnePole_tick(&r->f1_filter, f1_sample);

		f1_sample = f1_sample + r->f1_delay_2_last * 0.5f;

		f1_delay_2_sample = tTapeDelay_tick(&r->f1_delay_2, f1_sample * 0.5f);

		r->f1_delay_2_last = f1_delay_2_sample;

		f1_sample = r->f1_delay_2_last + f1_sample;

		f1_sample = tHighpass_tick(&r->f1_hp, f1_sample);

		f1_sample *= r->feedback_gain;

		if (r->frozen)
		{
			f1_sample = 0.0f;
		}

		r->f1_last = tTapeDelay_tick(&r->f1_delay_3, f1_sample);

		// FEEDBACK 2
		f2_sample = in_sample + r->f1_last;

		tAllpass_setDelay(&r->f2_allpass, SAMP(22.58f) + tCycle_tick(&r->f2_lfo) * SAMP(4.0f));

		f2_sample = tAllpass_tick(&r->f2_allpass, f2_sample);

		f2_sample = tTapeDelay_tick(&r->f2_delay_1, f2_sample);

		f2_sample = tOnePole_tick(&r->f2_filter, f2_sample);

		f2_sample = f2_sample + r->f2_delay_2_last * 0.5f;

		f2_delay_2_sample = tTapeDelay_tick(&r->f2_delay_2, f2_sample * 0.5f);

		r->f2_delay_2_last = f2_delay_2_sample;

		f2_sample = r->f2_delay_2_last + f2_sample;

		f2_sample = tHighpass_tick(&r->f2_hp, f2_sample);

		f2_sample *= r->feedback_gain;

		if (r->frozen)
		{
			f2_sample = 0.0f;
		}
		r->f2_last = tTapeDelay_tick(&r->f2_delay_3, f2_sample);



    // TAP OUT 1
    f1_sample =     tTapeDelay_tapOut(&r->f1_delay_1, SAMP(8.9f)) +
    tTapeDelay_tapOut(&r->f1_delay_1, SAMP(99.8f));

    f1_sample -=    tTapeDelay_tapOut(&r->f1_delay_2, SAMP(64.2f));

    f1_sample +=    tTapeDelay_tapOut(&r->f1_delay_3, SAMP(67.f));

    f1_sample -=    tTapeDelay_tapOut(&r->f2_delay_1, SAMP(66.8f));

    f1_sample -=    tTapeDelay_tapOut(&r->f2_delay_2, SAMP(6.3f));

    f1_sample -=    tTapeDelay_tapOut(&r->f2_delay_3, SAMP(35.8f));

    f1_sample *=    0.14f;

    // TAP OUT 2
    f2_sample =     tTapeDelay_tapOut(&r->f2_delay_1, SAMP(11.8f)) +
    tTapeDelay_tapOut(&r->f2_delay_1, SAMP(121.7f));

    f2_sample -=    tTapeDelay_tapOut(&r->f2_delay_2, SAMP(6.3f));

    f2_sample +=    tTapeDelay_tapOut(&r->f2_delay_3, SAMP(89.7f));

    f2_sample -=    tTapeDelay_tapOut(&r->f1_delay_1, SAMP(70.8f));

    f2_sample -=    tTapeDelay_tapOut(&r->f1_delay_2, SAMP(11.2f));

    f2_sample -=    tTapeDelay_tapOut(&r->f1_delay_3, SAMP(4.1f));

    f2_sample *=    0.14f;

    output[0] = input * (1.0f - r->mix) + f1_sample  * r->mix;
    output[1] = input * (1.0f - r->mix) + f2_sample * r->mix;

}

void    tDattorroReverb_setMix            (tDattorroReverb* const rev, float mix)
{
    _tDattorroReverb* r = *rev;
    r->mix = LEAF_clip(0.0f, mix, 1.0f);
}

void    tDattorroReverb_setFreeze            (tDattorroReverb* const rev, uint32_t freeze)
{
    _tDattorroReverb* r = *rev;
    r->frozen = freeze;
    if (freeze)
    {
    	tAllpass_setGain(&r->f2_allpass, 1.0f);
    	tAllpass_setGain(&r->f1_allpass, 1.0f);
		for (int i = 0; i < 4; i++)
		{

			//tAllpass_setGain(&r->in_allpass[i], 1.0f);
		}
    	tCycle_setFreq(&r->f1_lfo, 0.0f);
    	tCycle_setFreq(&r->f2_lfo, 0.0f);
    }
    else
    {
    	tAllpass_setGain(&r->f2_allpass, 0.7f);
    	tAllpass_setGain(&r->f1_allpass, 0.7f);
    	for (int i = 0; i < 4; i++)
		{
    		 //tAllpass_setGain(&r->in_allpass[i], in_allpass_gains[i]);
		}
    	tCycle_setFreq(&r->f1_lfo, 0.1f);
    	tCycle_setFreq(&r->f2_lfo, 0.07f);
    }
}


void    tDattorroReverb_setHP           (tDattorroReverb* const rev, float freq)
{
    _tDattorroReverb* r = *rev;
    float newFreq = LEAF_clip(20.0f, freq, 20000.0f);
    tHighpass_setFreq(&r->f1_hp, newFreq);
    tHighpass_setFreq(&r->f2_hp, newFreq);
}


void    tDattorroReverb_setSize           (tDattorroReverb* const rev, float size)
{
    _tDattorroReverb* r = *rev;
    
    r->size = LEAF_clip(0.01f, size*r->size_max, r->size_max);
    r->t = r->size * leaf.sampleRate * 0.001f;
    
    /*
     for (int i = 0; i < 4; i++)
     {
     tAllpass_setDelay(&r->in_allpass[i], SAMP(in_allpass_delays[i]));
     }
     */
    
    // FEEDBACK 1
    //tAllpass_setDelay(&r->f1_allpass, SAMP(30.51f));
    
    tTapeDelay_setDelay(&r->f1_delay_1, SAMP(141.69f));
    tTapeDelay_setDelay(&r->f1_delay_2, SAMP(89.24f));
    tTapeDelay_setDelay(&r->f1_delay_3, SAMP(125.f));
    
    // maybe change rate of SINE LFO's when size changes?
    //tCycle_setFreq(&r->f2_lfo, 0.07f * size * r->size_max);
    
    // FEEDBACK 2
    //tAllpass_setDelay(&r->f2_allpass, SAMP(22.58f));
    
    tTapeDelay_setDelay(&r->f2_delay_1, SAMP(149.62f));
    tTapeDelay_setDelay(&r->f2_delay_2, SAMP(60.48f));
    tTapeDelay_setDelay(&r->f2_delay_3, SAMP(106.28f));
}

void    tDattorroReverb_setInputDelay     (tDattorroReverb* const rev, float preDelay)
{
    _tDattorroReverb* r = *rev;
    
    r->predelay = LEAF_clip(0.0f, preDelay, 200.0f);
    
    tTapeDelay_setDelay(&r->in_delay, SAMP(r->predelay));
}

void    tDattorroReverb_setInputFilter    (tDattorroReverb* const rev, float freq)
{
    _tDattorroReverb* r = *rev;
    
    r->input_filter = LEAF_clip(0.0f, freq, 20000.0f);
    
    tOnePole_setFreq(&r->in_filter, r->input_filter);
}

void    tDattorroReverb_setFeedbackFilter (tDattorroReverb* const rev, float freq)
{
    _tDattorroReverb* r = *rev;
    
    r->feedback_filter = LEAF_clip(0.0f, freq, 20000.0f);
    
    tOnePole_setFreq(&r->f1_filter, r->feedback_filter);
    tOnePole_setFreq(&r->f2_filter, r->feedback_filter);
}

void    tDattorroReverb_setFeedbackGain   (tDattorroReverb* const rev, float gain)
{
    _tDattorroReverb* r = *rev;
    r->feedback_gain = gain;
}
