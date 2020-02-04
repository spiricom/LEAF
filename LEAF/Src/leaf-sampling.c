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
static void  buffer_init (tBuffer* const sb, uint32_t length)
{
    _tBuffer* s = *sb;
    s->bufferLength = length;
    s->recordedLength = 0;
    s->active = 0;
    s->idx = 0;
    s->mode = RecordOneShot;
}

void  tBuffer_init (tBuffer* const sb, uint32_t length)
{
    _tBuffer* s = *sb = (_tBuffer*) leaf_alloc(sizeof(_tBuffer));
    buffer_init(sb, length);
    s->buff = (float*) leaf_alloc( sizeof(float) * length);
}

void  tBuffer_free (tBuffer* const sb)
{
    _tBuffer* s = *sb;
    leaf_free(s->buff);
    leaf_free(s);
}

void  tBuffer_initToPool (tBuffer* const sb, uint32_t length, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tBuffer* s = *sb = (_tBuffer*) mpool_alloc(sizeof(_tBuffer), &m->pool);
    buffer_init(sb, length);
    s->buff = (float*) mpool_alloc( sizeof(float) * length, &m->pool);
}

void  tBuffer_freeFromPool (tBuffer* const sb, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tBuffer* s = *sb;
    mpool_free(s->buff, &m->pool);
    mpool_free(s, &m->pool);
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

static void sampler_init(tSampler* const sp, tBuffer* const b)
{
    _tSampler* p = *sp;
    _tBuffer* s = *b;
    
    p->samp = s;
    
    p->active = 0;
    
    p->start = 0;
    p->end = p->samp->bufferLength - 1;
    
    p->len = p->end - p->start;
    
    p->idx = 0.f;
    p->inc = 1.f;
    p->iinc = 1.f;
    
    p->dir = 1;
    p->flip = 1;
    p->bnf = 1;
    
    p->mode = PlayNormal;
    
    p->cfxlen = 500; // default 300 sample crossfade
    
    tRamp_setVal(&p->gain, 0.f);
    
    p->targetstart = -1;
    p->targetend = -1;
}

void tSampler_init(tSampler* const sp, tBuffer* const b)
{
    _tSampler* p = *sp = (_tSampler*) leaf_alloc(sizeof(_tSampler));
    tRamp_init(&p->gain, 7.0f, 1);
    sampler_init(sp, b);
}

void tSampler_free         (tSampler* const sp)
{
    _tSampler* p = *sp;
    tRamp_free(&p->gain);
    leaf_free(p);
}

void tSampler_initToPool    (tSampler* const sp, tBuffer* const b, tMempool* mp)
{
    _tMempool* m = *mp;
    _tSampler* p = *sp = (_tSampler*) mpool_alloc(sizeof(_tSampler), &m->pool);
    tRamp_initToPool(&p->gain, 7.0f, 1, mp);
    sampler_init(sp, b);
}

void tSampler_freeFromPool  (tSampler* const sp, tMempool* mp)
{
    _tMempool* m = *mp;
    _tSampler* p = *sp;
    tRamp_freeFromPool(&p->gain, mp);
    mpool_free(p, &m->pool);
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
    
    if (p->active == 0)         return 0.f;

    if ((p->inc == 0.0f) || (p->len == 0))
    {
        return p->last;
    }

    attemptStartEndChange(sp);

    float sample = 0.f;
    float cfxsample = 0.f;
    float numticks;
    float g1 = 1.f, g2 = 0.f;

    float* buff = p->samp->buff;

    

    int idx, revidx;
    float alpha, revalpha;

    int32_t start = p->start;
    int32_t end = p->end;
    if (p->flip < 0)
    {
        start = p->end;
        end = p->start;
    }



    int dir = p->bnf * p->dir * p->flip;
    idx = (int) p->idx;
    alpha = p->idx - idx;
    
    revidx = idx + 1;// we think this is because flooring on int works different when reading backwards
    revalpha = 1.f - alpha;
    
    


    
    uint32_t cfxlen = p->cfxlen;
    if (p->len < cfxlen) cfxlen = p->len * 0.25f;//p->len;
    
    int length = p->samp->recordedLength;
    
    if (dir > 0)
    {			// num samples (hopping the increment size) to end of loop
		numticks = (end-idx) * p->iinc;
    }
    else
    {
		numticks = (revidx-start) * p->iinc;
    }

	// Check dir (direction) to interpolate properly
	if (dir > 0)
	{
		// FORWARD NORMAL SAMPLE
		int i1 = ((idx-1) < 0) ? 0 : idx-1;
		int i3 = ((idx+1) >= length) ? (idx) : (idx+1);
		int i4 = ((idx+2) >= length) ? (length-1) : (idx+2);

		sample =     LEAF_interpolate_hermite_x (buff[i1],
											   buff[idx],
											   buff[i3],
											   buff[i4],
											   alpha);

		if (cfxlen > 0)// necessary to avoid divide by zero, also a waste of computation otherwise
		{

			if (p->mode == PlayLoop)
			{



				if (numticks <= (float) cfxlen)
				{
					// CROSSFADE SAMPLE
					int cdx = start - (numticks * p->inc);
					if (cdx < 1)
					{
						cdx = -cdx;

						i1 = ((cdx+1) >= length) ? (length-1) : cdx+1;
						i3 = ((cdx-1) < 0) ? cdx : (cdx-1);
						i4 = ((cdx-2) < 0) ? 0 : (cdx-2);

						cfxsample =     LEAF_interpolate_hermite_x (buff[i1],
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

						cfxsample =     LEAF_interpolate_hermite_x (buff[i1],
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
	    	g2 = 0.0f;
	    }

	}
	else
	{
		// REVERSE
		int i1 = ((revidx+1) >= length) ? (length-1) : revidx+1;
		int i3 = ((revidx-1) < 0) ? revidx : (revidx-1);
		int i4 = ((revidx-2) < 0) ? 0 : (revidx-2);

		sample =     LEAF_interpolate_hermite_x (buff[i1],
											   buff[revidx],
											   buff[i3],
											   buff[i4],
											   revalpha);



		if (cfxlen > 0)// necessary to avoid divide by zero, also a waste of computation otherwise
		{

			if (p->mode == PlayLoop)
			{
				if (numticks <= (float) cfxlen)
				{
					// CROSSFADE SAMPLE
					int cdx = end + (numticks * p->inc);
					if (cdx > length - 2)
					{

						//the problem with the click is here --- at some point it crosses this threshold and jumps from a point near the boundary to a point far away from the boundary - that's not correct
						///// ooooops

						cdx = end - (numticks * p->inc);

						i1 = ((cdx-1) < 0) ? 0 : cdx-1;
						i3 = ((cdx+1) >= length) ? (cdx) : (cdx+1);
						i4 = ((cdx+2) >= length) ? (length-1) : (cdx+2);

						cfxsample =     LEAF_interpolate_hermite_x (buff[i1],
																  buff[cdx],
																  buff[i3],
																  buff[i4],
																  revalpha);
					}
					else
					{
						i1 = ((cdx+1) >= length) ? (length-1) : cdx+1;
						i3 = ((cdx-1) < 0) ? cdx : (cdx-1);
						i4 = ((cdx-2) < 0) ? 0 : (cdx-2);

						cfxsample =     LEAF_interpolate_hermite_x (buff[i1],
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
	    	g2 = 0.0f;
	    }

	}


    
    float inc = fmod(p->inc, p->len);
    p->idx += (dir * inc);

    //handle start and end cases for looping and back and forth modes
    if (p->mode == PlayLoop)
    {

        while((int)p->idx < start)
        {
            p->idx += (float)(p->len);
        }
        while((int)p->idx > end)
        {

            p->idx -= (float)(p->len);
        }
    }
    else // == PlayBackAndForth
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


    //handle very short fade out for end of one-shot normal playback
    if (p->mode == PlayNormal)
    {
        if (numticks < (0.007f * leaf.sampleRate))
        {
            tRamp_setDest(&p->gain, 0.f);
            p->active = -1;
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
    
    uint32_t cfxlen = LEAF_clip(0, length, 1000);
    
    if (cfxlen > (p->len * 0.25f))  cfxlen = p->len * 0.25f;
    
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

    if (p->len < (p->cfxlen * 0.25f)) p->cfxlen = p->len * 0.25f;

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
   // if (p->active)
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
        uint32_t cfxlen = (p->len < p->cfxlen) ? 0 : p->cfxlen;
        if ((tempflip > 0) && (dir > 0)) // start is start and we're playing forward
        {
            if (((start > p->idx) || (p->end-p->idx <= cfxlen)) && (start > p->end))// start given is after current index or we're in a crossfade
            {
                p->targetstart = start;
                return;
            }
        }
        else if ((tempflip < 0) && (dir < 0)) // start is end and we're playing in reverse
        {
            if (((start < p->idx) || (p->idx-p->end <= cfxlen)) && (start < p->end))// start given is before current index or we're in a crossfade
            {
                p->targetstart = start;
                return;
            }
        }
    }

	p->start = LEAF_clipInt(0, start, p->samp->recordedLength - 1);
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
    //if (p->active)
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
        uint32_t cfxlen = (p->len < p->cfxlen) ? 0 : p->cfxlen;
        if (tempflip > 0 && dir < 0) // end is end and we're playing in reverse
        {
            if (((end < p->idx) || (p->idx-p->start <= cfxlen)) && (end < p->start)) // end given is before current index or we're in a crossfade
            {
                p->targetend = end;
                return;
            }
        }
        else if (tempflip < 0 && dir > 0) // end is start and we're playing forward
        {
            if (((end > p->idx) || (p->start-p->idx <= cfxlen)) && (end > p->start)) // end given is after current index or we're in a crossfade
            {
                p->targetend = end;
                return;
            }
        }
    }
    p->end = LEAF_clipInt(0, end, (p->samp->recordedLength - 1));
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

    p->inc = rate;
    p->iinc = 1.f / p->inc;
}

//==============================================================================
