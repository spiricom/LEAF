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

#define CFX 300

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
    p->flip = 1;
    
    p->mode = Normal;

    p->g1 = 1.f;
    p->g2 = 0.f;
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
    float cfxsample = 0.f;
    int cfx = CFX; // make this part of class
    
    float* buff = p->samp->buff;
    
    int dir = p->dir * p->flip;
    
    int idx;
    float alpha;

    if (dir > 0)
    {
    	idx = (int) p->idx;
    	alpha = p->idx - idx;
    }
    else
    {
    	idx = (int) (p->idx + 1.f); // we think this is because flooring on int works different when reading backwards
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
        
        if (i1 < start)  i1 += p->len;
        if (i3 > end)    i3 -= p->len;
        if (i4 > end)    i4 -= p->len;

        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[idx],
                                               buff[i3],
                                               buff[i4],
                                               alpha);
        
        cfx = (end - idx) / p->inc; // num samples to end of loop,  use inverse increment

        if (cfx <= CFX)
        {
            // CROSSFADE SAMPLE
            float idxx =  p->idx - p->len;
            int cdx = (int)(idxx);
    
            i1 = cdx-1;
            i3 = cdx+1;
            i4 = cdx+2;
            
            cfxsample =     LEAF_interpolate_hermite (buff[i1],
                                                      buff[cdx],
                                                      buff[i3],
                                                      buff[i4],
                                                      alpha);
            
            //rprintf("cidx: %d\n", cdx);
        }
        else   cfx = CFX;
    }
    else
    {
        // REVERSE
        int i1 = idx+1;
        int i3 = idx-1;
        int i4 = idx-2;
        
        if (i1 > end)    i1 -= p->len;
        if (i3 < start)  i3 += p->len;
        if (i4 < start)  i4 += p->len;

        sample =     LEAF_interpolate_hermite (buff[i1],
                                               buff[idx],
                                               buff[i3],
                                               buff[i4],
                                               1.0f-alpha);
        
        cfx = (idx - start) / p->inc;
        
        if (cfx <= CFX)
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
        }
        else   cfx = CFX;
    }
    
    p->idx += (dir * p->inc);
    
    if (p->mode != BackAndForth ) // == Normal or Loop
    {
        if (idx <= start)
        {
            p->idx += (float)(p->len);
            p->cnt++;
        }
        else if (idx >= end)
        {
            p->idx -= (float)(p->len);
            p->cnt++;
        }
    }
    else // == BackAndForth
    {
        if ((idx <= start) || (idx >= end))
        {
            p->dir = -p->dir;
            p->cnt++;
        }
    }
    
    p->g2 = (float) (CFX - cfx) / (float) CFX;
    p->g1 = 1.f - p->g2;
    
    sample = sample * p->g1 + cfxsample * p->g2;
    
    return sample;
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

void tSamplePlayer_stop         (tSamplePlayer* const p)
{
    p->active = 0;
}

static void handleStartEndChange(tSamplePlayer* const p)
{
    if (p->start > p->end)
    {
        p->flip = -1;
    }
    else
    {
        p->flip = 1;
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
