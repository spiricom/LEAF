/*
  ==============================================================================

    LEAFReverb.c
    Created: 20 Jan 2017 12:02:04pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-reverb.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-reverb.h"
#include "../leaf.h"

#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ PRCRev ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tPRCRev_init(tPRCRev* const r, float t60)
{
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
    
    r->allpassDelays[0] = (tDelay*) leaf_alloc(sizeof(tDelay));
    tDelay_init(r->allpassDelays[0], lengths[0], lengths[0] * 2);
    
    r->allpassDelays[1] = (tDelay*) leaf_alloc(sizeof(tDelay));
    tDelay_init(r->allpassDelays[1], lengths[1], lengths[1] * 2);
    
    r->combDelay = (tDelay*) leaf_alloc(sizeof(tDelay));
    tDelay_init(r->combDelay, lengths[2], lengths[2] * 2);
    
    tPRCRev_setT60(r, t60);
    
    r->allpassCoeff = 0.7f;
    r->mix = 0.5f;
}

void tPRCRev_free(tPRCRev* const r)
{
    tDelay_free(r->allpassDelays[0]);
    tDelay_free(r->allpassDelays[1]);
    tDelay_free(r->combDelay);
    leaf_free(r);
}

void    tPRCRev_setT60(tPRCRev* const r, float t60)
{
    if ( t60 <= 0.0f ) t60 = 0.001f;
    
    r->t60 = t60;
    
    r->combCoeff = pow(10.0f, (-3.0f * tDelay_getDelay(r->combDelay) * leaf.invSampleRate / t60 ));
    
}

void    tPRCRev_setMix(tPRCRev* const r, float mix)
{
    r->mix = mix;
}

float   tPRCRev_tick(tPRCRev* const r, float input)
{
    float temp, temp0, temp1, temp2;
    float out;
    
    r->lastIn = input;
    
    temp = tDelay_getLastOut(r->allpassDelays[0]);
    temp0 = r->allpassCoeff * temp;
    temp0 += input;
    tDelay_tick(r->allpassDelays[0], temp0);
    temp0 = -( r->allpassCoeff * temp0) + temp;
    
    temp = tDelay_getLastOut(r->allpassDelays[1]);
    temp1 = r->allpassCoeff * temp;
    temp1 += temp0;
    tDelay_tick(r->allpassDelays[1], temp1);
    temp1 = -(r->allpassCoeff * temp1) + temp;
    
    temp2 = temp1 + ( r->combCoeff * tDelay_getLastOut(r->combDelay));
    
    out = r->mix * tDelay_tick(r->combDelay, temp2);
    
    temp = (1.0f - r->mix) * input;
    
    out += temp;
    
    r->lastOut = out;
    
    return out;
}

void     tPRCRevSampleRateChanged (tPRCRev* const r)
{
    r->combCoeff = pow(10.0f, (-3.0f * tDelay_getDelay(r->combDelay) * leaf.invSampleRate / r->t60 ));
}

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NRev ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void    tNRev_init(tNRev* const r, float t60)
{
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
        r->combDelays[i] = (tDelay*) leaf_alloc(sizeof(tDelay));
        tDelay_init(r->combDelays[i], lengths[i], lengths[i] * 2.0f);
        r->combCoeffs[i] = pow(10.0, (-3 * lengths[i] * leaf.invSampleRate / t60));
    }
    
    for ( i=0; i<8; i++ )
    {
        r->allpassDelays[i] = (tDelay*) leaf_alloc(sizeof(tDelay));
        tDelay_init(r->allpassDelays[i], lengths[i+6], lengths[i+6] * 2.0f);
    }
    
    for ( i=0; i<2; i++ )
    {
        tDelay_setDelay(r->allpassDelays[i], lengths[i]);
        tDelay_setDelay(r->combDelays[i], lengths[i+2]);
    }
    
    tNRev_setT60(r, t60);
    r->allpassCoeff = 0.7f;
    r->mix = 0.3f;
}

void    tNRev_free(tNRev* const r)
{
    for (int i = 0; i < 6; i++)
    {
        tDelay_free(r->combDelays[i]);
    }
    
    for (int i = 0; i < 8; i++)
    {
        tDelay_free(r->allpassDelays[i]);
    }
    
    leaf_free(r);
}

void    tNRev_setT60(tNRev* const r, float t60)
{
    if (t60 <= 0.0f)           t60 = 0.001f;
    
    r->t60 = t60;
    
    for (int i=0; i<6; i++)   r->combCoeffs[i] = pow(10.0, (-3.0 * tDelay_getDelay(r->combDelays[i]) * leaf.invSampleRate / t60 ));
    
}

void    tNRev_setMix(tNRev* const r, float mix)
{
    r->mix = mix;
}

float   tNRev_tick(tNRev* const r, float input)
{
    r->lastIn = input;
    
    float temp, temp0, temp1, temp2, out;
    int i;
    
    temp0 = 0.0;
    for ( i=0; i<6; i++ )
    {
        temp = input + (r->combCoeffs[i] * tDelay_getLastOut(r->combDelays[i]));
        temp0 += tDelay_tick(r->combDelays[i],temp);
    }
    
    for ( i=0; i<3; i++ )
    {
        temp = tDelay_getLastOut(r->allpassDelays[i]);
        temp1 = r->allpassCoeff * temp;
        temp1 += temp0;
        tDelay_tick(r->allpassDelays[i], temp1);
        temp0 = -(r->allpassCoeff * temp1) + temp;
    }
    
    // One-pole lowpass filter.
    r->lowpassState = 0.7f * r->lowpassState + 0.3f * temp0;
    temp = tDelay_getLastOut(r->allpassDelays[3]);
    temp1 = r->allpassCoeff * temp;
    temp1 += r->lowpassState;
    tDelay_tick(r->allpassDelays[3], temp1 );
    temp1 = -(r->allpassCoeff * temp1) + temp;
    
    temp = tDelay_getLastOut(r->allpassDelays[4]);
    temp2 = r->allpassCoeff * temp;
    temp2 += temp1;
    tDelay_tick(r->allpassDelays[4], temp2 );
    out = r->mix * ( -( r->allpassCoeff * temp2 ) + temp );
    
    /*
     temp = tDelayLGetLastOut(&r->allpassDelays[5]);
     temp3 = r->allpassCoeff * temp;
     temp3 += temp1;
     tDelayLTick(&r->allpassDelays[5], temp3 );
     lastFrame_[1] = effectMix_*( - ( r->allpassCoeff * temp3 ) + temp );
     */
    
    temp = ( 1.0f - r->mix ) * input;
    
    out += temp;
    
    r->lastOut = out;
    
    return out;
}



void     tNRevSampleRateChanged (tNRev* const r)
{
    for (int i=0; i<6; i++)   r->combCoeffs[i] = pow(10.0, (-3.0 * tDelay_getDelay(r->combDelays[i]) * leaf.invSampleRate / r->t60 ));
}

