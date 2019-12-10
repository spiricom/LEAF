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
    	return p->last;
    }

    float sample = 0.f;
    float cfxsample = 0.f;
    int numsamps;
    float g1 = 1.f, g2 = 0.f;
    
    float* buff = p->samp->buff;
    
    int dir = p->bnf * p->dir * p->flip;
    
    int idx;
    float alpha;

    if (dir > 0)
    {
    	idx = (int) p->idx;
    	alpha = p->idx - idx;
    }
    else
    {
    	idx = (int) (p->idx+1.f); // we think this is because flooring on int works different when reading backwards
    	alpha = (p->idx+1.f) - idx;
    }
    
    int32_t start = p->start, end = p->end;
    if (p->flip < 0)
    {
        start = p->end;
        end = p->start;
    }
    
    // Check dir (direction) to interpolate properly
    if (dir > 0)
    {
        // FORWARD NORMAL SAMPLE
        int i1 = ((idx-1) + p->len) % p->len;
        int i3 = (idx+1) % p->len;
        int i4 = (idx+2) % p->len;

        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[idx],
                                               buff[i3],
                                               buff[i4],
                                               alpha);
        
        // num samples to end of loop
        numsamps = (idx - start) / p->inc;
        //numsamps = (dir > 0) ? (end - idx) : (idx - start);
        //numsamps *= p->iinc;
        
        if (p->mode == PlayLoop)
        {
            if (numsamps <= p->cfxlen)
            {
                // CROSSFADE SAMPLE
                float idxx =  p->idx - p->len;
                int cdx = ((int)(idxx) + p->len) % p->len;
                
                i1 = ((cdx-1) + p->len) % p->len;
                i3 = (cdx+1) % p->len;
                i4 = (cdx+2) % p->len;
                
                cfxsample =     LEAF_interpolate_hermite (buff[i1],
                                                          buff[cdx],
                                                          buff[i3],
                                                          buff[i4],
                                                          alpha);
                
                g2 = (float) (p->cfxlen - numsamps) / (float) p->cfxlen;
            }
        }
    }
    else
    {
        // REVERSE
        int i1 = (idx+1) % p->len;
        int i3 = ((idx-1) + p->len) % p->len;
        int i4 = ((idx-2) + p->len) % p->len;
    
        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[idx],
                                               buff[i3],
                                               buff[i4],
                                               1.0f-alpha);
        
        numsamps = (idx - start) / p->inc;
        
        if (p->mode == PlayLoop)
        {
            if (numsamps <= p->cfxlen)
            {
                // CROSSFADE SAMPLE
                float idxx =  p->idx + p->len + 1.f;
                int cdx = ((int)(idxx)) % p->len;
                alpha = idxx - cdx;
                
                i1 = (cdx+1) % p->len;
                i3 = ((cdx-1) + p->len) % p->len;
                i4 = ((cdx-2) + p->len) % p->len;
                
                cfxsample =     LEAF_interpolate_hermite (buff[i1],
                                                          buff[cdx],
                                                          buff[i3],
                                                          buff[i4],
                                                          1.f-alpha);
                
                g2 = (float) (p->cfxlen - numsamps) / (float) p->cfxlen;
            }
        }
    }
    
    p->idx += (dir * p->inc);
    
    if (p->mode == PlayNormal)
    {
        if (numsamps < (0.007f * leaf.sampleRate))
        {
            tRamp_setDest(&p->gain, 0.f);
            p->active = -1;
        }
    }
    else if (p->mode == PlayLoop )
    {
        if (idx <= start)
        {
            p->idx += (float)(p->len);
        }
        else if (idx >= end)
        {
            p->idx -= (float)(p->len);
        }
    }
    else // == PlayBackAndForth
    {
        if (p->idx < start)
        {
            p->bnf = -p->bnf;
            p->idx = start;
        }
        else if (p->idx > end)
        {
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

void tSampler_setStart     (tSampler* const sp, int32_t start)
{
    _tSampler* p = *sp;
    
    p->start = LEAF_clipInt(0, start, (p->samp->length - 1));
    
    handleStartEndChange(sp);
}

void tSampler_setEnd       (tSampler* const sp, int32_t end)
{
    _tSampler* p = *sp;
    
    p->end = LEAF_clipInt(0, end, (p->samp->length - 1));
    
    handleStartEndChange(sp);
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
