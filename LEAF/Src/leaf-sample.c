/*
  ==============================================================================

    LEAFUtilities.c
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

void  tSample_init (tSample* const s, uint32_t length)
{
    s->buff = (float*) leaf_alloc( sizeof(float) * length);
    
    
    s->length = length;
    s->active = 0;
    s->idx = 0;
    s->mode = RecordOneShot;
    
    tSample_clear(s);
}

void  tSample_free (tSample* const s)
{
    leaf_free(s->buff);
    leaf_free(s);
}

void tSample_tick (tSample* const s, float sample)
{
    if (s->active == 1)
    {
        s->buff[s->idx] = sample;
        
        s->idx += 1;
        
        if (s->idx >= s->length)
        {
            if (s->mode == RecordOneShot)
            {
                tSample_stop(s);
            }
            else if (s->mode == RecordLoop)
            {
                s->idx = 0;
            }
        }
    }
}

void  tSample_read(tSample* const s, float* buff, uint32_t len)
{
    for (int i = 0; i < s->length; i++)
    {
        if (i < len)    s->buff[i] = buff[i];
        else            s->buff[i] = 0.f;
    }
}

float tSample_get (tSample* const s, int idx)
{
    if ((idx < 0) || (idx >= s->length)) return 0.f;
    
    return s->buff[idx];
}

void  tSample_start(tSample* const s)
{
    s->active = 1;
    s->idx = 0;
}

void  tSample_stop(tSample* const s)
{
    s->active = 0;
}

void  tSample_setRecordMode (tSample* const s, RecordMode mode)
{
    s->mode = mode;
}

void  tSample_clear (tSample* const s)
{
    for (int i = 0; i < s->length; i++)
    {
        s->buff[i] = 0.f;
    }
}

//==============================================================================

void tSamplePlayer_init         (tSamplePlayer* const p, tSample* s)
{
    p->samp = s;
    
    p->active = 0;
    
    p->start = 0;
    p->end = p->samp->length - 1;
    
    p->len = p->end - p->start;
    
    p->idx = 0.f;
    p->inc = 1.f;
    p->dir = 1;
    p->flip = 0;
    
    p->mode = Normal;

    p->gain = 1.;
}

void tSamplePlayer_free         (tSamplePlayer* const p)
{
    leaf_free(p->samp);
    leaf_free(p);
}

float tSamplePlayer_tick        (tSamplePlayer* const p)
{
    if ((p->mode == Normal) && (p->cnt > 0))    p->active = 0;
    
    if (p->active == 0 || (p->len < 4))         return 0.f;
    
    float sample = 0.f;
    
    float* buff = p->samp->buff;
    
    int idx =  (int) p->idx;
    
    // Check dir (direction bit) to interpolate properly
    if (p->dir > 0)
    {
        // FORWARD
        float alpha = p->idx - idx;
        
        int i1 = idx-1;
        int i3 = idx+1;
        int i4 = idx+2;
        
        if (i1 < p->start)  i1 += p->len;
        if (i3 > p->end)    i3 -= p->len;
        if (i4 > p->end)    i4 -= p->len;
        
        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[idx],
                                               buff[i3],
                                               buff[i4],
                                               alpha);
    }
    else
    {
        // REVERSE
        float alpha = 1.0f - (p->idx - idx);
        
        int i1 = idx+1;
        int i3 = idx-1;
        int i4 = idx-2;
        
        if (i1 > p->start)  i1 -= p->len;
        if (i3 < p->end)    i3 += p->len;
        if (i4 < p->end)    i4 += p->len;
        
        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[idx],
                                               buff[i3],
                                               buff[i4],
                                               alpha);
    }
    
    
    p->idx += (p->dir * p->inc);
    
    if (p->mode != BackAndForth)
    {
        // Check flip bit to change loop test
        if (p->flip > 0)
        {
            if (idx <= p->end)
            {
                p->idx += (float)(p->len);
                p->cnt++;
            }
            else if (idx >= p->start)
            {
                p->idx -= (float)(p->len);
                p->cnt++;
            }
        }
        else
        {
            if (idx <= p->start)
            {
                p->idx += (float)(p->len);
                p->cnt++;
            }
            else if (idx >= p->end)
            {
                p->idx -= (float)(p->len);
                p->cnt++;
            }
        }
    }
    else // BackAndForth
    {
        if (p->flip > 0)
        {
            if ((idx < p->start) || (idx > p->end))
            {
                p->dir = -p->dir;
                p->idx += (2*p->inc);
                p->cnt++;
            }
        }
        else
        {
            if ((idx > p->start) || (idx < p->end))
            {
                p->dir = -p->dir;
                p->idx += (2*p->inc);
                p->cnt++;
            }
        }
        
    }
    
    
    return sample * p->gain;
}

void tSamplePlayer_setSample    (tSamplePlayer* const p, tSample* s)
{
    p->samp = s;
}

void tSamplePlayer_setMode      (tSamplePlayer* const p, Mode mode)
{
    p->mode = mode;
}

void tSamplePlayer_play         (tSamplePlayer* const p)
{
    p->active = 1;
    p->cnt = 0;
}

void tSamplePlayer_stop         (tSamplePlayer* const p)
{
    p->active = 0;
}

static void handleStartEndChange(tSamplePlayer* const p)
{
    if (p->start > p->end)
    {
        p->dir = -1;
        p->flip = 1;
    }
    else
    {
        p->dir = 1;
        p->flip = 0;
    }
}

void tSamplePlayer_setStart     (tSamplePlayer* const p, int32_t start)
{
    p->start = LEAF_clipInt(0, start, (p->samp->length - 1));
    
    p->len = abs(p->end - p->start);
    
    handleStartEndChange(p);
}

void tSamplePlayer_setEnd       (tSamplePlayer* const p, int32_t end)
{
    p->end = LEAF_clipInt(0, end, (p->samp->length - 1));

    p->len = abs(p->end - p->start);
    
    handleStartEndChange(p);
}

void tSamplePlayer_setRate      (tSamplePlayer* const p, float rate)
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
}


//==============================================================================
