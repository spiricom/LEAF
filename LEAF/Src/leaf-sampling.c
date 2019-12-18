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
    _tBuffer* s = *sb = (_tBuffer*) leaf_alloc(sizeof(_tBuffer));
    
    s->buff = (float*) leaf_alloc( sizeof(float) * length);
    
    s->length = length;
    s->active = 0;
    s->idx = 0;
    s->mode = RecordOneShot;
    
    tBuffer_clear(sb);
}

void  tBuffer_free (tBuffer* const sb)
{
    _tBuffer* s = *sb;
    
    leaf_free(s->buff);
    leaf_free(s);
}

void tBuffer_tick (tBuffer* const sb, float sample)
{
    _tBuffer* s = *sb;
    
    if (s->active == 1)
    {
        s->buff[s->idx] = sample;
        
        s->idx += 1;
        
        if (s->idx >= s->length)
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
    for (int i = 0; i < s->length; i++)
    {
        if (i < len)    s->buff[i] = buff[i];
        else            s->buff[i] = 0.f;
    }
}

float tBuffer_get (tBuffer* const sb, int idx)
{
    _tBuffer* s = *sb;
    if ((idx < 0) || (idx >= s->length)) return 0.f;
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
    for (int i = 0; i < s->length; i++)
    {
        s->buff[i] = 0.f;
    }
}

uint32_t tBuffer_getLength(tBuffer* const sb)
{
    _tBuffer* s = *sb;
    return s->length;
}

//================================tSampler=====================================

static void handleStartEndChange(tSampler* const sp);

static void attemptStartEndChange(tSampler* const sp);

static void updateStartEnd(tSampler* const sp);

void tSampler_init(tSampler* const sp, tBuffer* const b)
{
    _tSampler* p = *sp = (_tSampler*) leaf_alloc(sizeof(_tSampler));
    _tBuffer* s = *b;
    
    p->samp = s;
    
    p->active = 0;
    
    p->start = 0;
    p->end = p->samp->length - 1;
    
    p->len = p->end - p->start;
    
    p->idx = 0.f;
    p->inc = 1.f;
    p->iinc = 1.f;
    
    p->dir = 1;
    p->flip = 1;
    p->bnf = 1;
    
    p->mode = PlayNormal;
    
    p->cfxlen = 500; // default 300 sample crossfade
    
    tRamp_init(&p->gain, 7.0f, 1);
    tRamp_setVal(&p->gain, 0.f);
    
    p->targetstart = -1;
    p->targetend = -1;
}

void tSampler_free         (tSampler* const sp)
{
    _tSampler* p = *sp;
    tRamp_free(&p->gain);
    
    leaf_free(p);
}

void tSampler_setSample (tSampler* const sp, tBuffer* const b)
{
    _tSampler* p = *sp;
    _tBuffer* s = *b;
    
    p->samp = s;
}

float tSampler_tick        (tSampler* const sp)
{
    _tSampler* p = *sp;
    
    if (p->active == 0)         return 0.f;
    
    if ((p->inc == 0.0f) || (p->len < 4))
    {
        updateStartEnd(sp);
        return p->last;
    }
    
    //    attemptStartEndChange(sp);
    
    float sample = 0.f;
    float cfxsample = 0.f;
    float numticks;
    float g1 = 1.f, g2 = 0.f;
    
    float* buff = p->samp->buff;
    
    int dir = p->bnf * p->dir * p->flip;
    
    int idx, revidx;
    float alpha, revalpha;
    
    
    idx = (int) p->idx;
    alpha = p->idx - idx;
    
    revidx = idx + 1;// we think this is because flooring on int works different when reading backwards
    revalpha = 1.f - alpha;
    
    
    int32_t start = p->start, end = p->end;
    if (p->flip < 0)
    {
        start = p->end;
        end = p->start;
    }
    
    uint32_t cfxlen = p->cfxlen;
    if (p->len < cfxlen) cfxlen = 0;//p->len;
    
    int length = p->samp->length;
    
    // Check dir (direction) to interpolate properly
    if (dir > 0)
    {
        // FORWARD NORMAL SAMPLE
        int i1 = ((idx-1) < 0) ? 0 : idx-1;
        int i3 = ((idx+1) >= length) ? (idx) : (idx+1);
        int i4 = ((idx+2) >= length) ? (length-1) : (idx+2);
        
        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[idx],
                                               buff[i3],
                                               buff[i4],
                                               alpha);
        
        // num samples to end of loop
        numticks = (end-idx) * p->iinc;
        //        if (numticks > cfxlen)
        //        {
        //            updateStartEnd(sp);
        //            start = p->start;
        //            end = p->end;
        //            if (p->flip < 0)
        //            {
        //                start = p->end;
        //                end = p->start;
        //            }
        //            numticks = (end-idx) * p->iinc;
        //        }
        //numsamps = (dir > 0) ? (end - idx) : (idx - start);
        //numsamps *= p->iinc;
        
        if (p->mode == PlayLoop)
        {
            if (numticks <= (float) cfxlen)
            {
                // CROSSFADE SAMPLE
                int cdx = start - numticks * p->inc;
                if (cdx < 1)
                {
                    cdx = -cdx;
                    
                    i1 = ((cdx+1) >= length) ? (length-1) : cdx+1;
                    i3 = ((cdx-1) < 0) ? cdx : (cdx-1);
                    i4 = ((cdx-2) < 0) ? 0 : (cdx-2);
                    
                    cfxsample =     LEAF_interpolate_hermite (buff[i1],
                                                              buff[cdx],
                                                              buff[i3],
                                                              buff[i4],
                                                              revalpha);
                }
                else
                {
                    i1 = ((cdx-1) < 0) ? 0 : cdx-1;
                    i3 = ((cdx+1) >= length) ? (cdx) : (cdx+1);
                    i4 = ((cdx+2) >= length) ? (length-1) : (cdx+2);
                    
                    cfxsample =     LEAF_interpolate_hermite (buff[i1],
                                                              buff[cdx],
                                                              buff[i3],
                                                              buff[i4],
                                                              alpha);
                }
                g2 = (float) (cfxlen - numticks) / (float) cfxlen;
            }
        }
    }
    else
    {
        // REVERSE
        int i1 = ((revidx+1) >= length) ? (length-1) : revidx+1;
        int i3 = ((revidx-1) < 0) ? revidx : (revidx-1);
        int i4 = ((revidx-2) < 0) ? 0 : (revidx-2);
        
        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[revidx],
                                               buff[i3],
                                               buff[i4],
                                               revalpha);
        
        numticks = (revidx-start) * p->iinc;
        
        // still get clicks i think because updating start/end can put us in the middle of a crossfade
        //        if (numticks > cfxlen)
        //        {
        //            updateStartEnd(sp);
        //            start = p->start;
        //            end = p->end;
        //            if (p->flip < 0)
        //            {
        //                start = p->end;
        //                end = p->start;
        //            }
        //            numticks = (revidx-start) * p->iinc;
        //        }
        
        if (p->mode == PlayLoop)
        {
            if (numticks <= (float) cfxlen)
            {
                // CROSSFADE SAMPLE
                int cdx = end + numticks * p->inc;
                if (cdx > p->samp->length - 2)
                {
                    cdx = end - numticks * p->inc;
                    
                    i1 = ((cdx-1) < 0) ? 0 : cdx-1;
                    i3 = ((cdx+1) >= length) ? (cdx) : (cdx+1);
                    i4 = ((cdx+2) >= length) ? (length-1) : (cdx+2);
                    
                    cfxsample =     LEAF_interpolate_hermite (buff[i1],
                                                              buff[cdx],
                                                              buff[i3],
                                                              buff[i4],
                                                              alpha);
                }
                else
                {
                    i1 = ((cdx+1) >= length) ? (length-1) : cdx+1;
                    i3 = ((cdx-1) < 0) ? cdx : (cdx-1);
                    i4 = ((cdx-2) < 0) ? 0 : (cdx-2);
                    
                    cfxsample =     LEAF_interpolate_hermite (buff[i1],
                                                              buff[cdx],
                                                              buff[i3],
                                                              buff[i4],
                                                              revalpha);
                }
                g2 = (float) (cfxlen - numticks) / (float) cfxlen;
            }
        }
    }
    
    
    float inc = fmod(p->inc, p->len);
    p->idx += (dir * inc);
    
    //    attemptStartEndChange(sp);
    
    if (p->mode == PlayNormal)
    {
        if (numticks < (0.007f * leaf.sampleRate))
        {
            tRamp_setDest(&p->gain, 0.f);
            p->active = -1;
        }
    }
    else if (p->mode == PlayLoop)
    {
        // mike's suggestion: make sure idx is within bounds of start and end (instead of targetstart/targetend stuff)
        if (idx < start)
        {
            updateStartEnd(sp);
            p->idx += (float)(p->len);
        }
        else if (idx > end)
        {
            updateStartEnd(sp);
            p->idx -= (float)(p->len);
        }
    }
    else // == PlayBackAndForth
    {
        if (p->idx < start)
        {
            updateStartEnd(sp);
            p->bnf = -p->bnf;
            p->idx = start;
        }
        else if (p->idx > end)
        {
            updateStartEnd(sp);
            p->bnf = -p->bnf;
            p->idx = end;
        }
    }
    
    g1 = 1.f - g2;
    
    sample = sample * g1 + cfxsample * g2;
    
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
    
    uint32_t cfxlen = LEAF_clip(0, length, 1000);
    
    //if (cfxlen > p->len)  cfxlen = p->len * 0.25f;
    
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
    
    //if (p->len < p->cfxlen) p->cfxlen = p->len * 0.9f;
    
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

static void updateStartEnd(tSampler* const sp)
{
    _tSampler* p = *sp;
    if (p->targetstart >= 0)
    {
        p->start = p->targetstart;
        handleStartEndChange(sp);
        p->targetstart = -1;
    }
    if (p->targetend >= 0)
    {
        p->end = p->targetend;
        handleStartEndChange(sp);
        p->targetend = -1;
    }
}

void tSampler_setStart     (tSampler* const sp, int32_t start)
{
    _tSampler* p = *sp;
    
    if (p->active)
    {
        int dir = p->bnf * p->dir * p->flip;
        uint32_t cfxlen = (p->len < p->cfxlen) ? 0 : p->cfxlen;
        if (p->flip > 0 && dir > 0) // start is start and we're playing forward
        {
            if ((start > p->idx) || (p->end-p->idx <= cfxlen)) // start given is after current index or we're in a crossfade
            {
                p->targetstart = start;
                return;
            }
        }
        else if (p->flip < 0 && dir < 0) // start is end and we're playing in reverse
        {
            if ((start < p->idx) || (p->idx-p->end <= cfxlen)) // start given is before current index or we're in a crossfade
            {
                p->targetstart = start;
                return;
            }
        }
    }
    
    p->start = LEAF_clipInt(0, start, p->samp->length - 1);
    handleStartEndChange(sp);
    p->targetstart = -1;
}

void tSampler_setEnd       (tSampler* const sp, int32_t end)
{
    _tSampler* p = *sp;
    
    if (p->active)
    {
        int dir = p->bnf * p->dir * p->flip;
        uint32_t cfxlen = (p->len < p->cfxlen) ? 0 : p->cfxlen;
        if (p->flip > 0 && dir < 0) // end is end and we're playing in reverse
        {
            if ((end < p->idx) || (p->idx-p->start <= cfxlen)) // end given is before current index or we're in a crossfade
            {
                p->targetend = end;
                return;
            }
        }
        else if (p->flip < 0 && dir > 0) // end is start and we're playing forward
        {
            if ((end > p->idx) || (p->start-p->idx <= cfxlen)) // end given is after current index or we're in a crossfade
            {
                p->targetend = end;
                return;
            }
        }
    }
    
    p->end = LEAF_clipInt(0, end, (p->samp->length - 1));
    handleStartEndChange(sp);
    p->targetend = -1;
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
    
    p->inc = rate; //LEAF_clip(0.f, rate, 8.0f); any reason to clip this?
    p->iinc = 1.f / p->inc;
}

//==============================================================================
