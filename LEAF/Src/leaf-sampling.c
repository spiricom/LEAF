/*
 ==============================================================================
 
 leaf-sampling.c
 Created: 20 Jan 2017 12:02:17pm
 Author:  Michael R Mulshine
 
 ==============================================================================
 */


#if _WIN32 || _WIN64

#include "..\Inc\leaf-sampling.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-sampling.h"
#include "../leaf.h"


#endif

//==============================================================================

void  tBuffer_init (tBuffer* const sb, uint32_t length)
{
    tBuffer_initToPool(sb, length, &leaf.mempool);
}

void  tBuffer_free (tBuffer* const sb)
{
    tBuffer_freeFromPool(sb, &leaf.mempool);
}

void  tBuffer_initToPool (tBuffer* const sb, uint32_t length, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tBuffer* s = *sb = (_tBuffer*) mpool_alloc(sizeof(_tBuffer), m);
    
    s->buff = (float*) mpool_alloc( sizeof(float) * length, m);
    
    s->bufferLength = length;
    s->recordedLength = 0;
    s->active = 0;
    s->idx = 0;
    s->mode = RecordOneShot;
}

void  tBuffer_freeFromPool (tBuffer* const sb, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tBuffer* s = *sb;
    
    mpool_free(s->buff, m);
    mpool_free(s, m);
}

void tBuffer_tick (tBuffer* const sb, float sample)
{
    _tBuffer* s = *sb;
    
    if (s->active == 1)
    {
        s->buff[s->idx] = sample;
        
        s->idx += 1;
        
        if (s->idx >= s->bufferLength)
        {
            if (s->mode == RecordOneShot)
            {
                tBuffer_stop(sb);
            }
            else if (s->mode == RecordLoop)
            {
                s->idx = 0;
            }
        }
    }
}

void  tBuffer_read(tBuffer* const sb, float* buff, uint32_t len)
{
    _tBuffer* s = *sb;
    for (int i = 0; i < s->bufferLength; i++)
    {
        if (i < len)    s->buff[i] = buff[i];
        else            s->buff[i] = 0.f;
    }
    s->recordedLength = len;
}

float tBuffer_get (tBuffer* const sb, int idx)
{
    _tBuffer* s = *sb;
    if ((idx < 0) || (idx >= s->bufferLength)) return 0.f;
    return s->buff[idx];
}

void  tBuffer_record(tBuffer* const sb)
{
    _tBuffer* s = *sb;
    s->active = 1;
    s->idx = 0;
}

void  tBuffer_stop(tBuffer* const sb)
{
    _tBuffer* s = *sb;
    s->active = 0;
    s->recordedLength = s->idx;
}

int   tBuffer_getRecordPosition(tBuffer* const sb)
{
    _tBuffer* s = *sb;
    return s->idx;
}

void  tBuffer_setRecordMode (tBuffer* const sb, RecordMode mode)
{
    _tBuffer* s = *sb;
    s->mode = mode;
}

void  tBuffer_clear (tBuffer* const sb)
{
    _tBuffer* s = *sb;
    for (int i = 0; i < s->bufferLength; i++)
    {
        s->buff[i] = 0.f;
    }
}

uint32_t tBuffer_getBufferLength(tBuffer* const sb)
{
    _tBuffer* s = *sb;
    return s->bufferLength;
}

uint32_t tBuffer_getRecordedLength(tBuffer* const sb)
{
    _tBuffer* s = *sb;
    return s->recordedLength;
}

//================================tSampler=====================================

static void handleStartEndChange(tSampler* const sp);

static void attemptStartEndChange(tSampler* const sp);

void tSampler_init(tSampler* const sp, tBuffer* const b)
{
    tSampler_initToPool(sp, b, &leaf.mempool);
}

void tSampler_free         (tSampler* const sp)
{
    tSampler_freeFromPool(sp, &leaf.mempool);
}

void tSampler_initToPool(tSampler* const sp, tBuffer* const b, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSampler* p = *sp = (_tSampler*) mpool_alloc(sizeof(_tSampler), m);
    _tBuffer* s = *b;
    
    p->samp = s;
    
    p->active = 0;
    
    p->start = 0;
    p->end = 0;
    
    p->len = p->end - p->start;
    
    p->idx = 0.f;
    p->inc = 1.f;
    p->iinc = 1.f;
    
    p->dir = 1;
    p->flip = 1;
    p->bnf = 1;
    
    p->mode = PlayNormal;
    
    p->cfxlen = 500; // default 300 sample crossfade
    
    tRamp_initToPool(&p->gain, 7.0f, 1, mp);
    tRamp_setVal(&p->gain, 0.f);
    
    p->targetstart = -1;
    p->targetend = -1;
    
    p->inCrossfade = 0;
    p->flipStart = -1;
    p->flipIdx = -1;
}

void tSampler_freeFromPool         (tSampler* const sp, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSampler* p = *sp;
    tRamp_freeFromPool(&p->gain, mp);
    
    mpool_free(p, m);
}

void tSampler_setSample (tSampler* const sp, tBuffer* const b)
{
    _tSampler* p = *sp;
    _tBuffer* s = *b;
    
    p->samp = s;
    
    p->start = 0;
    p->end = p->samp->bufferLength - 1;
    
    p->len = p->end - p->start;
    
    p->idx = 0.f;
}

volatile uint32_t errorState = 0;

float tSampler_tick        (tSampler* const sp)
{
    _tSampler* p = *sp;
    
    attemptStartEndChange(sp);
    
    if (p->active == 0)         return 0.f;
    
    if ((p->inc == 0.0f) || (p->len < 2))
    {
        //        p->inCrossfade = 1;
        return p->last;
    }
    
    float sample = 0.0f;
    float cfxsample = 0.0f;
    float crossfadeMix = 0.0f;
    float flipsample = 0.0f;
    float flipMix = 0.0f;
    
    float* buff = p->samp->buff;
    
    // Variables so start is also before end
    int32_t start = p->start;
    int32_t end = p->end;
    if (p->flip < 0)
    {
        start = p->end;
        end = p->start;
    }
    
    // Get the direction and a reverse flag for some calcs
    int dir = p->bnf * p->dir * p->flip;
    int rev = 0;
    if (dir < 0) rev = 1;
    
    // Get the current integer index and alpha for interpolation
    int idx = (int) p->idx;
    float alpha = rev + (p->idx - idx) * dir;
    idx += rev;
    
    // Get the indexes for interpolation
    int i1 = idx-(1*dir);
    int i2 = idx;
    int i3 = idx+(1*dir);
    int i4 = idx+(2*dir);
    
    int length = p->samp->recordedLength;
    
    // Wrap as needed
    i1 = (i1 < length*rev) ? i1 + (length * (1-rev)) : i1 - (length * rev);
    i2 = (i2 < length*rev) ? i2 + (length * (1-rev)) : i2 - (length * rev);
    i3 = (i3 < length*(1-rev)) ? i3 + (length * rev) : i3 - (length * (1-rev));
    i4 = (i4 < length*(1-rev)) ? i4 + (length * rev) : i4 - (length * (1-rev));
    
    sample = LEAF_interpolate_hermite_x (buff[i1],
                                         buff[i2],
                                         buff[i3],
                                         buff[i4],
                                         alpha);
    
    uint32_t cfxlen = p->cfxlen;
    if (p->len * 0.25f < cfxlen) cfxlen = p->len * 0.25f;
    
    // Determine crossfade points
    uint32_t fadeLeftStart = 0;
    if (start >= cfxlen) fadeLeftStart = start - cfxlen;
    uint32_t fadeLeftEnd = fadeLeftStart + cfxlen;
    
    uint32_t fadeRightEnd = end;// + (fadeLeftEnd - start);
    //    if (fadeRightEnd >= length) fadeRightEnd = length - 1;
    uint32_t fadeRightStart = fadeRightEnd - cfxlen;
    
    if (p->mode == PlayLoop)
    {
        
        int offset = 0;
        int cdx = 0;
        if ((fadeLeftStart <= idx) && (idx <= fadeLeftEnd))
        {
            offset = fadeLeftEnd - idx;
            cdx = fadeRightEnd - offset;
            p->inCrossfade = 1;
        }
        else if ((fadeRightStart <= idx) && (idx <= fadeRightEnd))
        {
            offset = idx - fadeRightStart;
            cdx = fadeLeftStart + offset;
            p->inCrossfade = 1;
        }
        else p->inCrossfade = 0;
        
        if (p->inCrossfade)
        {
            int c1 = cdx-(1*dir);
            int c2 = cdx;
            int c3 = cdx+(1*dir);
            int c4 = cdx+(2*dir);
            
            // Wrap as needed
            c1 = (c1 < length * rev) ? c1 + (length * (1-rev)) : c1 - (length * rev);
            c2 = (c2 < length * rev) ? c2 + (length * (1-rev)) : c2 - (length * rev);
            c3 = (c3 < length * (1-rev)) ? c3 + (length * rev) : c3 - (length * (1-rev));
            c4 = (c4 < length * (1-rev)) ? c4 + (length * rev) : c4 - (length * (1-rev));
            
            cfxsample = LEAF_interpolate_hermite_x (buff[c1],
                                                    buff[c2],
                                                    buff[c3],
                                                    buff[c4],
                                                    alpha);
            crossfadeMix = (float) offset / (float) cfxlen;
        }
        
        float flipLength = fabsf(p->flipIdx - p->flipStart);
        if (flipLength > cfxlen)
        {
            p->flipStart = -1;
            p->flipIdx = -1;
        }
        if (p->flipIdx >= 0)
        {
            if (p->flipStart == -1)
            {
                p->flipStart = p->idx;
                p->flipIdx = p->idx;
            }
            flipLength = fabsf(p->flipIdx - p->flipStart);
            
            int fdx = (int) p->flipIdx;
            float falpha = (1-rev) - (p->flipIdx - fdx) * dir;
            idx += (1-rev);
            
            // Get the indexes for interpolation
            int f1 = fdx+(1*dir);
            int f2 = fdx;
            int f3 = fdx-(1*dir);
            int f4 = fdx-(2*dir);
            
            // Wrap as needed
            f1 = (f1 < length*(1-rev)) ? f1 + (length * rev) : f1 - (length * (1-rev));
            f2 = (f2 < length*(1-rev)) ? f2 + (length * rev) : f2 - (length * (1-rev));
            f3 = (f3 < length*rev) ? f3 + (length * (1-rev)) : f3 - (length * rev);
            f4 = (f4 < length*rev) ? f4 + (length * (1-rev)) : f4 - (length * rev);
            
            flipsample = LEAF_interpolate_hermite_x (buff[f1],
                                                     buff[f2],
                                                     buff[f3],
                                                     buff[f4],
                                                     falpha);
            flipMix = (float) (cfxlen - flipLength) / (float) cfxlen;
        }
    }
    
    float inc = fmod(p->inc, p->len);
    p->idx += (dir * inc);
    if (p->flipStart >= 0)
    {
        p->flipIdx += (-dir * inc);
        if((int)p->flipIdx < 0)
        {
            p->idx += (float)length;
        }
        if((int)p->idx >= length)
        {
            
            p->idx -= (float)length;
        }
    }
    
    if (p->mode == PlayLoop)
    {
        if((int)p->idx < start)
        {
            p->idx += (float)(fadeRightEnd - fadeLeftEnd);
        }
        if((int)p->idx > end)
        {
            
            p->idx -= (float)(fadeRightEnd - fadeLeftEnd);
        }
    }
    else if (p->mode == PlayBackAndForth)
    {
        if (p->idx < start)
        {
            p->bnf = -p->bnf;
            p->idx = start + 1;
        }
        else if (p->idx > end)
        {
            p->bnf = -p->bnf;
            p->idx = end - 1;
        }
    }
    
    float ticksToEnd = rev ? ((idx - start) * p->iinc) : ((end - idx) * p->iinc);
    if (p->mode == PlayNormal)
    {
        if (ticksToEnd < (0.007f * leaf.sampleRate))
        {
            tRamp_setDest(&p->gain, 0.f);
            p->active = -1;
        }
    }
    
    sample = ((sample * (1.0f - crossfadeMix)) + (cfxsample * crossfadeMix)) * (1.0f - flipMix) + (flipsample * flipMix);
    
    sample = sample * tRamp_tick(&p->gain);
    
    if (p->active < 0)
    {
        if (tRamp_sample(&p->gain) <= 0.00001f)
        {
            if (p->retrigger == 1)
            {
                p->active = 1;
                p->retrigger = 0;
                tRamp_setDest(&p->gain, 1.f);
                
                if (p->dir > 0)
                {
                    if (p->flip > 0)    p->idx = p->start;
                    else                p->idx = p->end;
                }
                else
                {
                    if (p->flip > 0)    p->idx = p->end;
                    else                p->idx = p->start;
                }
            }
            else
            {
                p->active = 0;
            }
            
        }
    }
    
    if (fabsf(sample-p->last) > 0.1f)
    {
        errorState = 1;
    }
    
    p->last = sample;
    
    
    return p->last;
}

void tSampler_setMode      (tSampler* const sp, PlayMode mode)
{
    _tSampler* p = *sp;
    p->mode = mode;
}

void tSampler_setCrossfadeLength  (tSampler* const sp, uint32_t length)
{
    _tSampler* p = *sp;
    
    uint32_t cfxlen = LEAF_clip(0, length, p->len * 0.25f);
    
    p->cfxlen = cfxlen;
}

void tSampler_play         (tSampler* const sp)
{
    _tSampler* p = *sp;
    
    if (p->active != 0)
    {
        p->active = -1;
        p->retrigger = 1;
        
        tRamp_setDest(&p->gain, 0.f);
    }
    else
    {
        p->active = 1;
        p->retrigger = 0;
        
        tRamp_setDest(&p->gain, 1.f);
        
        if (p->dir > 0)
        {
            if (p->flip > 0)    p->idx = p->start;
            else                p->idx = p->end;
        }
        else
        {
            if (p->flip > 0)    p->idx = p->end;
            else                p->idx = p->start;
        }
        handleStartEndChange(&p);
    }
}

void tSampler_stop         (tSampler* const sp)
{
    _tSampler* p = *sp;
    
    p->active = -1;
    
    tRamp_setDest(&p->gain, 0.f);
}

static void handleStartEndChange(tSampler* const sp)
{
    _tSampler* p = *sp;
    
    p->len = abs(p->end - p->start);
    
    if (p->cfxlen > (p->len * 0.25f)) p->cfxlen = p->len * 0.25f;
    
    if (p->start > p->end)
    {
        p->flip = -1;
    }
    else
    {
        p->flip = 1;
    }
}

static void attemptStartEndChange(tSampler* const sp)
{
    _tSampler* p = *sp;
    
    // Try to update start/end if needed
    if (p->targetstart >= 0)
    {
        tSampler_setStart(sp, p->targetstart);
    }
    if (p->targetend >= 0)
    {
        tSampler_setEnd(sp, p->targetend);
    }
}

void tSampler_setStart     (tSampler* const sp, int32_t start)
{
    _tSampler* p = *sp;
    
    int tempflip;
    if (start == p->end)
    {
        return;
    }
    if (p->active) // only bother with these checks if we're actually playing
    {
        if (start > p->end)
        {
            tempflip = -1;
        }
        else
        {
            tempflip = 1;
        }
        
        int dir = p->bnf * p->dir * tempflip;
        
        uint32_t cfxlen = p->cfxlen;
        if (p->len * 0.25f < cfxlen) cfxlen = p->len * 0.25f;
        
        if (p->inCrossfade || p->flipStart >= 0)
        {
            p->targetstart = start;
            return;
        }
        if ((tempflip > 0) && (dir > 0)) // start is start and we're playing forward
        {
            if (start > p->idx)// start given is after current index or we're in a crossfade
            {
                p->targetstart = start;
                return;
            }
        }
        else if ((tempflip < 0) && (dir < 0)) // start is end and we're playing in reverse
        {
            if (start < p->idx)// start given is before current index or we're in a crossfade
            {
                p->targetstart = start;
                return;
            }
        }
        if (tempflip != p->flip && p->flipStart < 0)
        {
            p->flipIdx = 0;
        }
    }
    
    p->start = LEAF_clipInt(0, start, p->samp->recordedLength-1);
    handleStartEndChange(sp);
    p->targetstart = -1;
    
}

void tSampler_setEnd       (tSampler* const sp, int32_t end)
{
    _tSampler* p = *sp;
    
    int tempflip;
    
    if (end == p->start)
    {
        return;
    }
    if (p->active) // only bother with these checks if we're actually playing
    {
        if (p->start > end)
        {
            tempflip = -1;
        }
        else
        {
            tempflip = 1;
        }
        
        int dir = p->bnf * p->dir * tempflip;
        
        uint32_t cfxlen = p->cfxlen;
        if (p->len * 0.25f < cfxlen) cfxlen = p->len * 0.25f;
        
        if (p->inCrossfade || p->flipStart >= 0)
        {
            p->targetend = end;
            return;
        }
        if (tempflip > 0 && dir < 0) // end is end and we're playing in reverse
        {
            if (end < p->idx) // end given is before current index or we're in a crossfade
            {
                p->targetend = end;
                return;
            }
        }
        else if (tempflip < 0 && dir > 0) // end is start and we're playing forward
        {
            if (end > p->idx) // end given is after current index or we're in a crossfade
            {
                p->targetend = end;
                return;
            }
        }
        if (tempflip != p->flip && p->flipStart < 0)
        {
            p->flipIdx = 0;
        }
    }
    
    p->end = LEAF_clipInt(0, end, p->samp->recordedLength-1);
    handleStartEndChange(sp);
    p->targetend = -1;
}

void    tSampler_setLength    (tSampler* const sp, int32_t length)
{
    _tSampler* p = *sp;
    if (length == 0) length = 1;
    tSampler_setEnd(sp, p->start + length);
}

void tSampler_setRate      (tSampler* const sp, float rate)
{
    _tSampler* p = *sp;
    
    if (rate < 0.f)
    {
        rate = -rate;
        p->dir = -1;
    }
    else
    {
        p->dir = 1;
    }
    
    p->inc = rate;
    p->iinc = 1.f / p->inc;
}

//==============================================================================

void    tAutoSampler_init   (tAutoSampler* const as, tBuffer* const b)
{
    tAutoSampler_initToPool(as, b, &leaf.mempool);
}

void    tAutoSampler_free   (tAutoSampler* const as)
{
    tAutoSampler_freeFromPool(as, &leaf.mempool);
}

void    tAutoSampler_initToPool (tAutoSampler* const as, tBuffer* const b, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tAutoSampler* a = *as = (_tAutoSampler*) mpool_alloc(sizeof(_tAutoSampler), m);
    
    tBuffer_setRecordMode(b, RecordOneShot);
    tSampler_initToPool(&a->sampler, b, mp);
    tSampler_setMode(&a->sampler, PlayLoop);
    tEnvelopeFollower_initToPool(&a->ef, 0.05f, 0.9999f, mp);
}

void    tAutoSampler_freeFromPool       (tAutoSampler* const as, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tAutoSampler* a = *as;
    
    tEnvelopeFollower_freeFromPool(&a->ef, mp);
    tSampler_freeFromPool(&a->sampler, mp);
    
    mpool_free(a, m);
}

float   tAutoSampler_tick               (tAutoSampler* const as, float input)
{
    _tAutoSampler* a = *as;
    float currentPower = tEnvelopeFollower_tick(&a->ef, input);
    
    if ((currentPower > (a->threshold)) &&
        (currentPower > a->previousPower + 0.001f) &&
        (a->sampleTriggered == 0) &&
        (a->sampleCounter == 0))
    {
        a->sampleTriggered = 1;
        tBuffer_record(&a->sampler->samp);
        a->sampler->samp->recordedLength = a->sampler->samp->bufferLength;
        a->sampleCounter = a->windowSize + 24;//arbitrary extra time to avoid resampling while playing previous sample - better solution would be alternating buffers and crossfading
        a->powerCounter = 1000;
    }
    
    if (a->sampleCounter > 0)
    {
        a->sampleCounter--;
    }
    
    
    tSampler_setEnd(&a->sampler, a->windowSize);
    tBuffer_tick(&a->sampler->samp, input);
    //on it's way down
    if (currentPower <= a->previousPower)
    {
        if (a->powerCounter > 0)
        {
            a->powerCounter--;
        }
        else if (a->sampleTriggered == 1)
        {
            a->sampleTriggered = 0;
        }
    }
    
    a->previousPower = currentPower;
    
    return tSampler_tick(&a->sampler);
}

void    tAutoSampler_setBuffer         (tAutoSampler* const as, tBuffer* const b)
{
    _tAutoSampler* a = *as;
    tBuffer_setRecordMode(b, RecordOneShot);
    if (a->windowSize > tBuffer_getBufferLength(b))
        a->windowSize = tBuffer_getBufferLength(b);
    tSampler_setSample(&a->sampler, b);
}

void    tAutoSampler_setMode            (tAutoSampler* const as, PlayMode mode)
{
    _tAutoSampler* a = *as;
    tSampler_setMode(&a->sampler, mode);
}

void    tAutoSampler_play               (tAutoSampler* const as)
{
    _tAutoSampler* a = *as;
    tSampler_play(&a->sampler);
}
void    tAutoSampler_stop               (tAutoSampler* const as)
{
    _tAutoSampler* a = *as;
    tSampler_stop(&a->sampler);
}

void    tAutoSampler_setThreshold       (tAutoSampler* const as, float thresh)
{
    _tAutoSampler* a = *as;
    a->threshold = thresh;
}

void    tAutoSampler_setWindowSize      (tAutoSampler* const as, uint32_t size)
{
    _tAutoSampler* a = *as;
    if (size > tBuffer_getBufferLength(&a->sampler->samp))
        a->windowSize = tBuffer_getBufferLength(&a->sampler->samp);
    else a->windowSize = size;
}

void    tAutoSampler_setCrossfadeLength (tAutoSampler* const as, uint32_t length)
{
    _tAutoSampler* a = *as;
    tSampler_setCrossfadeLength(&a->sampler, length);
}

void    tAutoSampler_setRate    (tAutoSampler* const as, float rate)
{
    ;
}
