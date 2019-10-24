/*==============================================================================

    leaf-pitch.c
    Created: 30 Nov 2018 11:02:59am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-pitch.h"
#else

#include "../Inc/leaf-pitch.h"

#endif

static int pitchshifter_attackdetect(tPitchShifter* ps);

void tPitchShifter_init(tPitchShifter* const ps, float* in, float* out, int bufSize, int frameSize)
{
    ps->inBuffer = in;
    ps->outBuffer = out;
    ps->bufSize = bufSize;
    ps->frameSize = frameSize;
    ps->framesPerBuffer = ps->bufSize / ps->frameSize;
    ps->curBlock = 1;
    ps->lastBlock = 0;
    ps->index = 0;
    
    ps->hopSize = DEFHOPSIZE;
    ps->windowSize = DEFWINDOWSIZE;
    ps->fba = FBA;

    tEnv_init(&ps->env, ps->windowSize, ps->hopSize, ps->frameSize);
    
    tSNAC_init(&ps->snac, DEFOVERLAP);
    
    tSOLAD_init(&ps->sola);
    
    tHighpass_init(&ps->hp, HPFREQ);
    
    tSOLAD_setPitchFactor(&ps->sola, DEFPITCHRATIO);
    
    tPitchShifter_setTimeConstant(ps, DEFTIMECONSTANT);
}

void tPitchShifter_free(tPitchShifter* const ps)
{
    tEnv_free(&ps->env);
    tSNAC_free(&ps->snac);
    tSOLAD_free(&ps->sola);
    tHighpass_free(&ps->hp);
}

float tPitchShifter_tick(tPitchShifter* ps, float sample)
{
    float period, out;
    int i, iLast;
    
    i = (ps->curBlock*ps->frameSize);
    iLast = (ps->lastBlock*ps->frameSize)+ps->index;
    
    out = tHighpass_tick(&ps->hp, ps->outBuffer[iLast]);
    ps->inBuffer[i+ps->index] = sample;
    
    ps->index++;
    if (ps->index >= ps->frameSize)
    {
        ps->index = 0;
        
        tEnv_processBlock(&ps->env, &(ps->inBuffer[i]));
        
        if(pitchshifter_attackdetect(ps) == 1)
        {
            ps->fba = 5;
            tSOLAD_setReadLag(&ps->sola, ps->windowSize);
        }
        
        tSNAC_ioSamples(&ps->snac, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        period = tSNAC_getPeriod(&ps->snac);
        
        ps->curBlock++;
        if (ps->curBlock >= ps->framesPerBuffer) ps->curBlock = 0;
        ps->lastBlock++;
        if (ps->lastBlock >= ps->framesPerBuffer) ps->lastBlock = 0;
        
        //separate here
        
        tSOLAD_setPeriod(&ps->sola, period);
        
        tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
        tSOLAD_ioSamples(&ps->sola, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        
    }
    
    return out;
}

float tPitchShifterToFreq_tick(tPitchShifter* ps, float sample, float freq)
{
    float period, out;
    int i, iLast;
    
    i = (ps->curBlock*ps->frameSize);
    iLast = (ps->lastBlock*ps->frameSize)+ps->index;
    
    out = tHighpass_tick(&ps->hp, ps->outBuffer[iLast]);
    ps->inBuffer[i+ps->index] = sample;
    
    ps->index++;
    if (ps->index >= ps->frameSize)
    {
        ps->index = 0;
        
        tEnv_processBlock(&ps->env, &(ps->inBuffer[i]));
        
        if(pitchshifter_attackdetect(ps) == 1)
        {
            ps->fba = 5;
            tSOLAD_setReadLag(&ps->sola, ps->windowSize);
        }
        
        tSNAC_ioSamples(&ps->snac, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        period = tSNAC_getPeriod(&ps->snac);
        
        tSOLAD_setPeriod(&ps->sola, period);
        
        ps->pitchFactor = period*freq*leaf.invSampleRate;
        tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
        tSOLAD_ioSamples(&ps->sola, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        
        ps->curBlock++;
        if (ps->curBlock >= ps->framesPerBuffer) ps->curBlock = 0;
        ps->lastBlock++;
        if (ps->lastBlock >= ps->framesPerBuffer) ps->lastBlock = 0;
    }
    
    return out;
}

float tPitchShifterToFunc_tick(tPitchShifter* ps, float sample, float (*fun)(float))
{
    float period, out;
    int i, iLast;
    
    i = (ps->curBlock*ps->frameSize);
    iLast = (ps->lastBlock*ps->frameSize)+ps->index;
    
    out = tHighpass_tick(&ps->hp, ps->outBuffer[iLast]);
    ps->inBuffer[i+ps->index] = sample;
    
    ps->index++;
    if (ps->index >= ps->frameSize)
    {
        ps->index = 0;
        
        tEnv_processBlock(&ps->env, &(ps->inBuffer[i]));
        
        if(pitchshifter_attackdetect(ps) == 1)
        {
            ps->fba = 5;
            tSOLAD_setReadLag(&ps->sola, ps->windowSize);
        }
        
        tSNAC_ioSamples(&ps->snac, (&ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        period = tSNAC_getPeriod(&ps->snac);
        
        tSOLAD_setPeriod(&ps->sola, period);
        
        ps->pitchFactor = period/fun(period);
        tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
        tSOLAD_ioSamples(&ps->sola, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        
        ps->curBlock++;
        if (ps->curBlock >= ps->framesPerBuffer) ps->curBlock = 0;
        ps->lastBlock++;
        if (ps->lastBlock >= ps->framesPerBuffer) ps->lastBlock = 0;
    }
    
    return out;
}

void tPitchShifter_ioSamples(tPitchShifter* ps, float* in, float* out, int size)
{
    float period;
    
    tEnv_processBlock(&ps->env, in);
    
    if(pitchshifter_attackdetect(ps) == 1)
    {
        ps->fba = 5;
        tSOLAD_setReadLag(&ps->sola, ps->windowSize);
    }
    
    tSNAC_ioSamples(&ps->snac, in, out, size);
    period = tSNAC_getPeriod(&ps->snac);
    
    tSOLAD_setPeriod(&ps->sola, period);
    
    tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
    tSOLAD_ioSamples(&ps->sola, in, out, size);
    
    for (int cc = 0; cc < size; ++cc)
    {
        out[cc] = tHighpass_tick(&ps->hp, out[cc]);
    }
}

void tPitchShifter_ioSamples_toFreq(tPitchShifter* ps, float* in, float* out, int size, float toFreq)
{
    float period;
    
    tEnv_processBlock(&ps->env, in);
    
    if(pitchshifter_attackdetect(ps) == 1)
    {
        ps->fba = 5;
        tSOLAD_setReadLag(&ps->sola, ps->windowSize);
    }
    
    tSNAC_ioSamples(&ps->snac, in, out, size);
    period = tSNAC_getPeriod(&ps->snac);
    
    tSOLAD_setPeriod(&ps->sola, period);
    ps->pitchFactor = period*toFreq;
    tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
    tSOLAD_ioSamples(&ps->sola, in, out, size);
    
    for (int cc = 0; cc < size; ++cc)
    {
        out[cc] = tHighpass_tick(&ps->hp, out[cc]);
    }
}

void tPitchShifter_ioSamples_toPeriod(tPitchShifter* ps, float* in, float* out, int size, float toPeriod)
{
    float period;
    
    tEnv_processBlock(&ps->env, in);
    
    if(pitchshifter_attackdetect(ps) == 1)
    {
        ps->fba = 5;
        tSOLAD_setReadLag(&ps->sola, ps->windowSize);
    }
    
    tSNAC_ioSamples(&ps->snac, in, out, size);
    period = tSNAC_getPeriod(&ps->snac);
    
    tSOLAD_setPeriod(&ps->sola, period);
    ps->pitchFactor = period/toPeriod;
    tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
    tSOLAD_ioSamples(&ps->sola, in, out, size);
    
    for (int cc = 0; cc < size; ++cc)
    {
        out[cc] = tHighpass_tick(&ps->hp, out[cc]);
    }
}

void tPitchShifter_ioSamples_toFunc(tPitchShifter* ps, float* in, float* out, int size, float (*fun)(float))
{
    float period;
    
    tEnv_processBlock(&ps->env, in);
    
    if(pitchshifter_attackdetect(ps) == 1)
    {
        ps->fba = 5;
        tSOLAD_setReadLag(&ps->sola, ps->windowSize);
    }
    
    tSNAC_ioSamples(&ps->snac, in, out, size);
    period = tSNAC_getPeriod(&ps->snac);
    
    tSOLAD_setPeriod(&ps->sola, period);
    ps->pitchFactor = period/fun(period);
    tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
    tSOLAD_ioSamples(&ps->sola, in, out, size);
    
    for (int cc = 0; cc < size; ++cc)
    {
        out[cc] = tHighpass_tick(&ps->hp, out[cc]);
    }
}

void tPitchShifter_setPitchFactor(tPitchShifter* ps, float pf)
{
    ps->pitchFactor = pf;
}

void tPitchShifter_setTimeConstant(tPitchShifter* ps, float tc)
{
    ps->timeConstant = tc;
    ps->radius = expf(-1000.0f * ps->hopSize * leaf.invSampleRate / ps->timeConstant);
}

void tPitchShifter_setHopSize(tPitchShifter* ps, int hs)
{
    ps->hopSize = hs;
}

void tPitchShifter_setWindowSize(tPitchShifter* ps, int ws)
{
    ps->windowSize = ws;
}

float tPitchShifter_getPeriod(tPitchShifter* ps)
{
    return tSNAC_getPeriod(&ps->snac);
}

static int pitchshifter_attackdetect(tPitchShifter* ps)
{
    float envout;
    
    envout = tEnv_tick(&ps->env);
    
    if (envout >= 1.0f)
    {
        ps->lastmax = ps->max;
        if (envout > ps->max)
        {
            ps->max = envout;
        }
        else
        {
            ps->deltamax = envout - ps->max;
            ps->max = ps->max * ps->radius;
        }
        ps->deltamax = ps->max - ps->lastmax;
    }
    
    ps->fba = ps->fba ? (ps->fba - 1) : 0;
    
    return (ps->fba == 0 && (ps->max > 60 && ps->deltamax > 6)) ? 1 : 0;
}


void     tPeriod_init    (tPeriod* const p, float* in, float* out, int bufSize, int frameSize)
{
    p->inBuffer = in;
    p->outBuffer = out;
    p->bufSize = bufSize;
    p->frameSize = frameSize;
    p->framesPerBuffer = p->bufSize / p->frameSize;
    p->curBlock = 1;
    p->lastBlock = 0;
    p->index = 0;
    
    p->hopSize = DEFHOPSIZE;
    p->windowSize = DEFWINDOWSIZE;
    p->fba = FBA;
    
    tEnv_init(&p->env, p->windowSize, p->hopSize, p->frameSize);
    
    tSNAC_init(&p->snac, DEFOVERLAP);
    
    p->timeConstant = DEFTIMECONSTANT;
    p->radius = expf(-1000.0f * p->hopSize * leaf.invSampleRate / p->timeConstant);
}

void tPeriod_free (tPeriod* const p)
{
    tEnv_free(&p->env);
    tSNAC_free(&p->snac);
}

float tPeriod_findPeriod (tPeriod* p, float sample)
{
    int i, iLast;
    
    i = (p->curBlock*p->frameSize);
    iLast = (p->lastBlock*p->frameSize)+p->index;
    
    p->i = i;
    p->iLast = iLast;
    
    p->inBuffer[i+p->index] = sample;
    
    p->index++;
    p->indexstore = p->index;
    if (p->index >= p->frameSize)
    {
        p->index = 0;
        
        tEnv_processBlock(&p->env, &(p->inBuffer[i]));
        
        tSNAC_ioSamples(&p->snac, &(p->inBuffer[i]), &(p->outBuffer[i]), p->frameSize);
        p->period = tSNAC_getPeriod(&p->snac);
        
        p->curBlock++;
        if (p->curBlock >= p->framesPerBuffer) p->curBlock = 0;
        p->lastBlock++;
        if (p->lastBlock >= p->framesPerBuffer) p->lastBlock = 0;
    }
    
    // changed from period to p->period
    return p->period;
}

void tPeriod_setHopSize(tPeriod* p, int hs)
{
    p->hopSize = hs;
}

void tPeriod_setWindowSize(tPeriod* p, int ws)
{
    p->windowSize = ws;
}

void tPitchShift_setPitchFactor(tPitchShift* ps, float pf)
{
    ps->pitchFactor = pf;
}

static int pitchshift_attackdetect(tPitchShift* ps)
{
    float envout;
    
    envout = tEnv_tick(&ps->p->env);
    
    if (envout >= 1.0f)
    {
        ps->p->lastmax = ps->p->max;
        if (envout > ps->p->max)
        {
            ps->p->max = envout;
        }
        else
        {
            ps->p->deltamax = envout - ps->p->max;
            ps->p->max = ps->p->max * ps->radius;
        }
        ps->p->deltamax = ps->p->max - ps->p->lastmax;
    }
    
    ps->p->fba = ps->p->fba ? (ps->p->fba - 1) : 0;
    
    return (ps->p->fba == 0 && (ps->p->max > 60 && ps->p->deltamax > 6)) ? 1 : 0;
}

void tPitchShift_init (tPitchShift* const ps, tPeriod* p, float* out, int bufSize)
{
    ps->p = p;
    
    ps->outBuffer = out;
    ps->bufSize = bufSize;
    ps->frameSize = p->frameSize;
    ps->framesPerBuffer = ps->bufSize / ps->frameSize;
    ps->curBlock = 1;
    ps->lastBlock = 0;
    ps->index = 0;
    ps->pitchFactor = 1.0f;
    
    tSOLAD_init(&ps->sola);
    
    tHighpass_init(&ps->hp, HPFREQ);
    
    tSOLAD_setPitchFactor(&ps->sola, DEFPITCHRATIO);
}

void tPitchShift_free(tPitchShift* const ps)
{
    tSOLAD_free(&ps->sola);
    tHighpass_free(&ps->hp);
}

float tPitchShift_shift (tPitchShift* ps)
{
    float period, out;
    int i, iLast;
    
    i = ps->p->i;
    iLast = ps->p->iLast;
    
    out = tHighpass_tick(&ps->hp, ps->outBuffer[iLast]);
    
    if (ps->p->indexstore >= ps->frameSize)
    {
        period = ps->p->period;
        
        if(pitchshift_attackdetect(ps) == 1)
        {
            ps->p->fba = 5;
            tSOLAD_setReadLag(&ps->sola, ps->p->windowSize);
        }
        
        tSOLAD_setPeriod(&ps->sola, period);
        tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
        
        tSOLAD_ioSamples(&ps->sola, &(ps->p->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
    }
    
    return out;
}

float tPitchShift_shiftToFreq (tPitchShift* ps, float freq)
{
    float period, out;
    int i, iLast;
    
    i = ps->p->i;
    iLast = ps->p->iLast;
    
    out = tHighpass_tick(&ps->hp, ps->outBuffer[iLast]);
    
    if (ps->p->indexstore >= ps->frameSize)
    {
        period = ps->p->period;
        
        if(pitchshift_attackdetect(ps) == 1)
        {
            ps->p->fba = 5;
            tSOLAD_setReadLag(&ps->sola, ps->p->windowSize);
        }
        
        tSOLAD_setPeriod(&ps->sola, period);
        
        ps->pitchFactor = period*freq*leaf.invSampleRate;
        tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
        
        tSOLAD_ioSamples(&ps->sola, &(ps->p->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
    }
    return out;
}

float tPitchShift_shiftToFunc (tPitchShift* ps, float (*fun)(float))
{
    float period, out;
    int i, iLast;
    
    i = ps->p->i;
    iLast = ps->p->iLast;
    
    out = tHighpass_tick(&ps->hp, ps->outBuffer[iLast]);
    
    if (ps->p->indexstore >= ps->frameSize)
    {
        period = ps->p->period;
        
        if(pitchshift_attackdetect(ps) == 1)
        {
            ps->p->fba = 5;
            tSOLAD_setReadLag(&ps->sola, ps->p->windowSize);
        }
        
        tSOLAD_setPeriod(&ps->sola, period);
        
        ps->pitchFactor = period/fun(period);
        tSOLAD_setPitchFactor(&ps->sola, ps->pitchFactor);
        
        tSOLAD_ioSamples(&ps->sola, &(ps->p->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        
        ps->curBlock++;
        if (ps->curBlock >= ps->p->framesPerBuffer) ps->curBlock = 0;
        ps->lastBlock++;
        if (ps->lastBlock >= ps->framesPerBuffer) ps->lastBlock = 0;
    }
    
    return out;
}
