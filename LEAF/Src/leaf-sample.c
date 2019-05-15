/*
  ==============================================================================

    leaf-sample.c
    Created: 20 Jan 2017 12:02:17pm
    Author:  Michael R Mulshine

  ==============================================================================
*/


#if _WIN32 || _WIN64

#include "..\Inc\leaf-sample.h"
#include "..\leaf.h"

#else

#include "../Inc/leaf-sample.h"
#include "../leaf.h"

#endif

//==============================================================================

void  tBuffer_init (tBuffer* const s, uint32_t length)
{
    s->buff = (float*) leaf_alloc( sizeof(float) * length);
    
    
    s->length = length;
    s->active = 0;
    s->idx = 0;
    s->mode = RecordOneShot;
    
    tBuffer_clear(s);
}

void  tBuffer_free (tBuffer* const s)
{
    leaf_free(s->buff);
    leaf_free(s);
}

void tBuffer_tick (tBuffer* const s, float sample)
{
    if (s->active == 1)
    {
        s->buff[s->idx] = sample;
        
        s->idx += 1;
        
        if (s->idx >= s->length)
        {
            if (s->mode == RecordOneShot)
            {
                tBuffer_stop(s);
            }
            else if (s->mode == RecordLoop)
            {
                s->idx = 0;
            }
        }
    }
}

void  tBuffer_read(tBuffer* const s, float* buff, uint32_t len)
{
    for (int i = 0; i < s->length; i++)
    {
        if (i < len)    s->buff[i] = buff[i];
        else            s->buff[i] = 0.f;
    }
}

float tBuffer_get (tBuffer* const s, int idx)
{
    if ((idx < 0) || (idx >= s->length)) return 0.f;
    
    return s->buff[idx];
}

void  tBuffer_record(tBuffer* const s)
{
    s->active = 1;
    s->idx = 0;
}

void  tBuffer_stop(tBuffer* const s)
{
    s->active = 0;
}

void  tBuffer_setRecordMode (tBuffer* const s, RecordMode mode)
{
    s->mode = mode;
}

void  tBuffer_clear (tBuffer* const s)
{
    for (int i = 0; i < s->length; i++)
    {
        s->buff[i] = 0.f;
    }
}

//================================tSampler=====================================

void tSampler_init         (tSampler* const p, tBuffer* s)
{
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
    
    p->cfxlen = 400; // default 300 sample crossfade
    
    p->last = 0.0f;
    
    tRamp_init(&p->gain, 7.0f, 1);
    tRamp_setVal(&p->gain, 0.f);
}

void tSampler_free         (tSampler* const p)
{
    leaf_free(p->samp);
    leaf_free(p);
}

float tSampler_tick        (tSampler* const p)
{
    if (p->active == 0)
    {
        return 0.0f;
    }
    
    if ((p->inc == 0.0f) || (p->len < 4))
    {
        return p->last;
    }
    
    float sample = 0.f;
    float cfxsample = 0.f;
    float numsamps;
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
        int i1 = idx-1;
        int i3 = idx+1;
        int i4 = idx+2;
    
        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[idx],
                                               buff[i3],
                                               buff[i4],
                                               alpha);
        
        // num samples to end of loop
        numsamps = ((float)(end - idx)) / p->inc;
        
        if (p->mode == PlayLoop)
        {
            if (numsamps < p->cfxlen)
            {
                // CROSSFADE SAMPLE
                float idxx =  fabsf(p->idx - p->len);
        
                int cdx = (int)(idxx);
            
                i1 = cdx-1;
                i3 = cdx+1;
                i4 = cdx+2;
                
                cfxsample =     LEAF_interpolate_hermite (buff[i1],
                                                          buff[cdx],
                                                          buff[i3],
                                                          buff[i4],
                                                          1.0f-alpha);
                
                g2 = (float) (p->cfxlen - numsamps) / (float) p->cfxlen;
            }
        }
    }
    else
    {
        // REVERSE
        int i1 = idx+1;
        int i3 = idx-1;
        int i4 = idx-2;
    
        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[idx],
                                               buff[i3],
                                               buff[i4],
                                               1.0f-alpha);
        
        numsamps = ((float)(idx - start)) / p->inc;
        
        if (p->mode == PlayLoop)
        {
            if (numsamps < p->cfxlen)
            {
                // CROSSFADE SAMPLE
                float idxx =  p->idx + p->len + 1.f;
            
                int cdx = (int)(idxx);
                
                alpha = idxx - cdx;
                
                i1 = cdx+1;
                i3 = cdx-1;
                i4 = cdx-2;

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

void tSampler_setSample    (tSampler* const p, tBuffer* s)
{
    p->samp = s;
}

void tSampler_setMode      (tSampler* const p, PlayMode mode)
{
    p->mode = mode;
}

void tSampler_setCrossfadeLength  (tSampler* const p, uint32_t length)
{
    uint32_t cfxlen = LEAF_clip(0, length, 10000);
    
    if (cfxlen > (p->len*0.5))  cfxlen = p->len * 0.5f;
    
    p->cfxlen = cfxlen;
}

void tSampler_play         (tSampler* const p)
{
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

void tSampler_stop         (tSampler* const p)
{
    p->active = -1;
    
    tRamp_setDest(&p->gain, 0.f);
}

static void handleStartEndChange(tSampler* const p)
{
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

void tSampler_setStart     (tSampler* const p, int32_t start)
{
    p->start = LEAF_clipInt(0, start, (p->samp->length - 1));
    
    handleStartEndChange(p);
}

void tSampler_setEnd       (tSampler* const p, int32_t end)
{
    p->end = LEAF_clipInt(0, end, (p->samp->length - 1));
    
    handleStartEndChange(p);
}

void tSampler_setRate      (tSampler* const p, float rate)
{
    if (rate < 0.f)
    {
        rate = -rate;
        p->dir = -1;
    }
    else
    {
        p->dir = 1;
    }
    
    p->inc = LEAF_clip(0.f, rate, 8.0f);
    p->iinc = 1.f / p->inc;
}


//==============================================================================
