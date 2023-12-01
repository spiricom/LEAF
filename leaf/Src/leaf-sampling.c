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

void  tBuffer_init (tBuffer* const sb, uint32_t length, LEAF* const leaf)
{
    tBuffer_initToPool(sb, length, &leaf->mempool);
}

void  tBuffer_initToPool (tBuffer* const sb, uint32_t length, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tBuffer* s = *sb = (_tBuffer*) mpool_alloc(sizeof(_tBuffer), m);
    s->mempool = m;
    LEAF* leaf = s->mempool->leaf;
    
    s->buff = (Lfloat*) mpool_alloc( sizeof(Lfloat) * length, m);
    s->sampleRate = leaf->sampleRate;
    s->channels = 1;
    s->bufferLength = length;
    s->recordedLength = 0;
    s->active = 0;
    s->idx = 0;
    s->mode = RecordOneShot;
}

void  tBuffer_free (tBuffer* const sb)
{
    _tBuffer* s = *sb;
    
    mpool_free((char*)s->buff, s->mempool);
    mpool_free((char*)s, s->mempool);
}

void tBuffer_tick (tBuffer* const sb, Lfloat sample)
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
        s->recordedLength = s->idx;
    }
}

void  tBuffer_read(tBuffer* const sb, Lfloat* buff, uint32_t len)
{
    _tBuffer* s = *sb;
    for (unsigned i = 0; i < s->bufferLength; i++)
    {
        if (i < len)    s->buff[i] = buff[i];
        else            s->buff[i] = 0.f;
    }
    s->recordedLength = len;
}

Lfloat tBuffer_get (tBuffer* const sb, int idx)
{
    _tBuffer* s = *sb;
    if ((idx < 0) || (idx >= (int) s->bufferLength)) return 0.f;
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

void   tBuffer_setRecordPosition(tBuffer* const sb, int pos)
{
    _tBuffer* s = *sb;
    s->idx = pos;
}

void  tBuffer_setRecordMode (tBuffer* const sb, RecordMode mode)
{
    _tBuffer* s = *sb;
    s->mode = mode;
}

void  tBuffer_clear (tBuffer* const sb)
{
    _tBuffer* s = *sb;
    for (unsigned i = 0; i < s->bufferLength; i++)
    {
        s->buff[i] = 0.f;
    }

}

void tBuffer_setBuffer(tBuffer* const sb, Lfloat* externalBuffer, int length, int channels, int sampleRate)
{
    _tBuffer* s = *sb;

    s->buff = externalBuffer;
    s->channels = channels;
    s->sampleRate = sampleRate;
    s->recordedLength = length/channels;
    s->bufferLength = s->recordedLength;
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

void tBuffer_setRecordedLength(tBuffer* const sb, int length)
{
    _tBuffer* s = *sb;
    s->recordedLength = length;
}

int tBuffer_isActive(tBuffer* const sb)
{
    _tBuffer* s = *sb;
    return s->active;
}

//================================tSampler=====================================

static void handleStartEndChange(tSampler* const sp);

static void attemptStartEndChange(tSampler* const sp);

void tSampler_init(tSampler* const sp, tBuffer* const b, LEAF* const leaf)
{
    tSampler_initToPool(sp, b, &leaf->mempool, leaf);
}

void tSampler_initToPool(tSampler* const sp, tBuffer* const b, tMempool* const mp, LEAF* const leaf)
{
    _tMempool* m = *mp;
    _tSampler* p = *sp = (_tSampler*) mpool_alloc(sizeof(_tSampler), m);
    p->mempool = m;
    
    _tBuffer* s = *b;
    
    p->invSampleRate = leaf->invSampleRate;
    p->sampleRate = leaf->sampleRate;
    p->ticksPerSevenMs = 0.007f * p->sampleRate;
    p->rateFactor = s->sampleRate * p->invSampleRate;
    p->channels = s->channels;

    p->samp = s;
    
    p->active = 0;
    
    p->start = 0;
    p->end = p->samp->bufferLength - 1;
    
    p->len = p->end - p->start;
    
    p->idx = 0.f;
    Lfloat rate = p->rateFactor; //adjust for sampling rate of buffer (may be different from leaf.sampleRate if audio file was loaded form SD card)
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
    
    p->dir = 1;
    p->flip = 1;
    p->bnf = 1;
    
    p->mode = PlayNormal;
    
    p->cfxlen = 500; // default 300 sample crossfade
    
    tRamp_initToPool(&p->gain, 5.0f, 1, mp);
    tRamp_setVal(&p->gain, 0.f);
    
    p->targetstart = -1;
    p->targetend = -1;
    
    p->inCrossfade = 0;
    p->flipStart = -1;
    p->flipIdx = -1;
}

void tSampler_free (tSampler* const sp)
{
    _tSampler* p = *sp;
    tRamp_free(&p->gain);
    
    mpool_free((char*)p, p->mempool);
}

void tSampler_setSample (tSampler* const sp, tBuffer* const b)
{
    _tSampler* p = *sp;
    _tBuffer* s = *b;
    
    p->samp = s;

    p->rateFactor = s->sampleRate * p->invSampleRate;
    p->channels = s->channels;

    p->start = 0;
    p->end = p->samp->bufferLength - 1;
    
    p->len = p->end - p->start;
    
    p->idx = 0.f;
}

Lfloat tSampler_tick        (tSampler* const sp)
{
    _tSampler* p = *sp;
    
    attemptStartEndChange(sp);
    
    if (p->active == 0)         return 0.f;
    
    if ((p->inc == 0.0f) || (p->len < 2))
    {
        //        p->inCrossfade = 1;
        return p->last;
    }
    
    Lfloat sample = 0.0f;
    Lfloat cfxsample = 0.0f;
    Lfloat crossfadeMix = 0.0f;
    Lfloat flipsample = 0.0f;
    Lfloat flipMix = 0.0f;
    
    Lfloat* buff = p->samp->buff;
    
    // Variables so start is also before end
    int myStart = p->start;
    int myEnd = p->end;
    if (p->flip < 0)
    {
        myStart = p->end;
        myEnd = p->start;
    }
    
    // Get the direction and a reverse flag for some calcs
    int dir = p->bnf * p->dir * p->flip;
    int rev = 0;
    if (dir < 0) rev = 1;
    
    // Get the current integer index and alpha for interpolation
    int idx = (int) p->idx;
    Lfloat alpha = rev + (p->idx - idx) * dir;
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
    
    int32_t cfxlen = p->cfxlen;
    if (p->len * 0.25f < cfxlen) cfxlen = p->len * 0.25f;
    
    // Determine crossfade points
    int32_t fadeLeftStart = 0;
    if (myStart >= cfxlen) fadeLeftStart = myStart - cfxlen;
    int32_t fadeLeftEnd = fadeLeftStart + cfxlen;
    
    int32_t fadeRightEnd = myEnd;// + (fadeLeftEnd - start);
    //    if (fadeRightEnd >= length) fadeRightEnd = length - 1;
    int32_t fadeRightStart = fadeRightEnd - cfxlen;

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
            if (cfxlen > 0.0f) crossfadeMix = (Lfloat) offset / (Lfloat) cfxlen;
            else crossfadeMix = 0.0f;
        }
        
        Lfloat flipLength = fabsf(p->flipIdx - p->flipStart);
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
            Lfloat falpha = (1-rev) - (p->flipIdx - fdx) * dir;
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
            flipMix = (Lfloat) (cfxlen - flipLength) / (Lfloat) cfxlen;
        }
    }
    
    Lfloat inc = fmodf(p->inc, (Lfloat)p->len);
    p->idx += (dir * inc);
    if (p->flipStart >= 0)
    {
        p->flipIdx += (-dir * inc);
        if((int)p->flipIdx < 0)
        {
            p->idx += (Lfloat)length;
        }
        if((int)p->idx >= length)
        {
            
            p->idx -= (Lfloat)length;
        }
    }
    


    attemptStartEndChange(sp);


    if (p->mode == PlayLoop)
    {
        if((int)p->idx < myStart)
        {
            p->idx += (Lfloat)(fadeRightEnd - fadeLeftEnd);
        }
        if((int)p->idx > myEnd)
        {
            
            p->idx -= (Lfloat)(fadeRightEnd - fadeLeftEnd);
        }
    }
    else if (p->mode == PlayBackAndForth)
    {
        if (p->idx < myStart)
        {
            p->bnf = -p->bnf;
            p->idx = myStart + 1;
        }
        else if (p->idx > myEnd)
        {
            p->bnf = -p->bnf;
            p->idx = myEnd - 1;
        }
    }
    

    if (p->mode == PlayNormal)
    {
        if (p->idx < myStart)
        {
            p->idx = myStart;
        }
        else if (p->idx > myEnd)
        {
            p->idx = myEnd;
        }
        Lfloat ticksToEnd = rev ? ((idx - myStart) * p->iinc) : ((myEnd - idx) * p->iinc);
        if ((ticksToEnd < p->ticksPerSevenMs) && (p->active == 1))
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
    
    p->last = sample;
    
    
    return p->last;
}

Lfloat tSampler_tickStereo        (tSampler* const sp, Lfloat* outputArray)
{
    _tSampler* p = *sp;

    attemptStartEndChange(sp);

    if (p->active == 0)         return 0.f;

    if ((p->inc == 0.0f) || (p->len < 2))
    {
        //        p->inCrossfade = 1;
        return p->last;
    }

    Lfloat cfxsample[2] = {0.0f, 0.0f};
    Lfloat crossfadeMix = 0.0f;
    Lfloat flipsample[2] = {0.0f, 0.0f};
    Lfloat flipMix = 0.0f;

    Lfloat* buff = p->samp->buff;

    // Variables so start is also before end
    int myStart = p->start;
    int myEnd = p->end;
    if (p->flip < 0)
    {
        myStart = p->end;
        myEnd = p->start;
    }

    // Get the direction and a reverse flag for some calcs
    int dir = p->bnf * p->dir * p->flip;
    int rev = 0;
    if (dir < 0) rev = 1;

    // Get the current integer index and alpha for interpolation
    int idx = (int) p->idx;
    Lfloat alpha = rev + (p->idx - idx) * dir;
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

    outputArray[0] = LEAF_interpolate_hermite_x (buff[i1 * p->channels],
                                         buff[i2 * p->channels],
                                         buff[i3 * p->channels],
                                         buff[i4 * p->channels],
                                         alpha);

    outputArray[1] = LEAF_interpolate_hermite_x (buff[(i1 * p->channels) + 1],
                                         buff[(i2 * p->channels) + 1],
                                         buff[(i3 * p->channels) + 1],
                                         buff[(i4 * p->channels) + 1],
                                         alpha);

    int32_t cfxlen = p->cfxlen;
    if (p->len * 0.25f < cfxlen) cfxlen = p->len * 0.25f;

    // Determine crossfade points
    int32_t fadeLeftStart = 0;
    if (myStart >= cfxlen) fadeLeftStart = myStart - cfxlen;
    int32_t fadeLeftEnd = fadeLeftStart + cfxlen;

    int32_t fadeRightEnd = myEnd;// + (fadeLeftEnd - start);
    //    if (fadeRightEnd >= length) fadeRightEnd = length - 1;
    int32_t fadeRightStart = fadeRightEnd - cfxlen;

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

            cfxsample[0] = LEAF_interpolate_hermite_x (buff[c1 * p->channels],
                                                    buff[c2 * p->channels],
                                                    buff[c3 * p->channels],
                                                    buff[c4 * p->channels],
                                                    alpha);

            cfxsample[1] = LEAF_interpolate_hermite_x (buff[(c1 * p->channels) + 1],
                                                                buff[(c2 * p->channels) + 1],
                                                                buff[(c3 * p->channels) + 1],
                                                                buff[(c4 * p->channels) + 1],
                                                                alpha);

            crossfadeMix = (Lfloat) offset / (Lfloat) cfxlen;
        }

        Lfloat flipLength = fabsf(p->flipIdx - p->flipStart);
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
            Lfloat falpha = (1-rev) - (p->flipIdx - fdx) * dir;
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

            flipsample[0] = LEAF_interpolate_hermite_x (buff[f1 * p->channels],
                                                     buff[f2 * p->channels],
                                                     buff[f3 * p->channels],
                                                     buff[f4 * p->channels],
                                                     falpha);

            flipsample[1] = LEAF_interpolate_hermite_x (buff[(f1 * p->channels) + 1],
                                                     buff[(f2 * p->channels) + 1],
                                                     buff[(f3 * p->channels) + 1],
                                                     buff[(f4 * p->channels) + 1],
                                                     falpha);

            if (cfxlen > 0) flipMix = (Lfloat) (cfxlen - flipLength) / (Lfloat) cfxlen;
            else flipMix = 1.0f;
        }
    }

    Lfloat inc = fmodf(p->inc, (Lfloat)p->len);
    p->idx += (dir * inc);
    if (p->flipStart >= 0)
    {
        p->flipIdx += (-dir * inc);
        if((int)p->flipIdx < 0)
        {
            p->idx += (Lfloat)length;
        }
        if((int)p->idx >= length)
        {

            p->idx -= (Lfloat)length;
        }
    }

    attemptStartEndChange(sp);

    if (p->mode == PlayLoop)
    {
        if((int)p->idx < myStart)
        {
            p->idx += (Lfloat)(fadeRightEnd - fadeLeftEnd);
        }
        if((int)p->idx > myEnd)
        {

            p->idx -= (Lfloat)(fadeRightEnd - fadeLeftEnd);
        }
    }
    else if (p->mode == PlayBackAndForth)
    {
        if (p->idx < myStart)
        {
            p->bnf = -p->bnf;
            p->idx = myStart + 1;
        }
        else if (p->idx > myEnd)
        {
            p->bnf = -p->bnf;
            p->idx = myEnd - 1;
        }
    }


    if (p->mode == PlayNormal)
    {
        if (p->idx < myStart)
        {
            p->idx = myStart;
        }
        else if (p->idx > myEnd)
        {
            p->idx = myEnd;
        }
        Lfloat ticksToEnd = rev ? ((idx - myStart) * p->iinc) : ((myEnd - idx) * p->iinc);
        if ((ticksToEnd < p->ticksPerSevenMs) && (p->active == 1))
        {
            tRamp_setDest(&p->gain, 0.f);
            p->active = -1;
        }
    }

    Lfloat sampleGain = tRamp_tick(&p->gain);
    for (int i = 0; i < p->channels; i++)
    {
        outputArray[i] = ((outputArray[i] * (1.0f - crossfadeMix)) + (cfxsample[i] * crossfadeMix)) * (1.0f - flipMix) + (flipsample[i] * flipMix);
        outputArray[i]  = outputArray[i] * sampleGain;
    }

    if (p->active < 0)
    {
        //if was fading out and reached silence
    	if (tRamp_sample(&p->gain) <= 0.0001f)
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
                handleStartEndChange(&p);
            }
            else
            {
                p->active = 0;
            }

        }
    }

    p->last =  outputArray[0];

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

    if (p->active > 0)
    {
        p->active = -1;
        p->retrigger = 1;
        
        tRamp_setDest(&p->gain, 0.f);
    }

    else if (p->active < 0)
    {
        p->active = -1;
        p->retrigger = 1;

        //tRamp_setDest(&p->gain, 0.f);
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
    /*
    if (start == p->end)
    {
        return;
    }
    */
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
                Lfloat tempLen = abs(p->end - start) * 0.25f;
                if (cfxlen > tempLen)
                {
                    p->cfxlen = tempLen;
                }
                return;
            }
        }
        else if ((tempflip < 0) && (dir < 0)) // start is end and we're playing in reverse
        {
            if (start < p->idx)// start given is before current index or we're in a crossfade
            {
                p->targetstart = start;
                Lfloat tempLen = abs(p->end - start) * 0.25f;
                if (cfxlen > tempLen)
                {
                    p->cfxlen = tempLen;
                }
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
    
    /*
    if (end == p->start)
    {
        return;
    }
    */
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
                Lfloat tempLen = abs(end - p->start) * 0.25f;
                if (cfxlen > tempLen)
                {
                    p->cfxlen = tempLen;
                }
                return;
            }
        }
        else if (tempflip < 0 && dir > 0) // end is start and we're playing forward
        {
            if (end > p->idx) // end given is after current index or we're in a crossfade
            {
                p->targetend = end;
                Lfloat tempLen = abs(end - p->start) * 0.25f;
                if (cfxlen > tempLen)
                {
                    p->cfxlen = tempLen;
                }
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

void tSampler_setEndUnsafe     (tSampler* const sp, int32_t end)
{
    _tSampler* p = *sp;
    
    int tempflip;
    
    /*
    if (end == p->start)
    {
        return;
    }
    */
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
                Lfloat tempLen = abs(end - p->start) * 0.25f;
                if (cfxlen > tempLen)
                {
                    p->cfxlen = tempLen;
                }
                return;
            }
        }
        else if (tempflip < 0 && dir > 0) // end is start and we're playing forward
        {
            if (end > p->idx) // end given is after current index or we're in a crossfade
            {
                p->targetend = end;
                Lfloat tempLen = abs(end - p->start) * 0.25f;
                if (cfxlen > tempLen)
                {
                    p->cfxlen = tempLen;
                }
                return;
            }
        }
        if (tempflip != p->flip && p->flipStart < 0)
        {
            p->flipIdx = 0;
        }
    }
    
    p->end = LEAF_clipInt(0, end, end);
    handleStartEndChange(sp);
    p->targetend = -1;
}

void    tSampler_setLength    (tSampler* const sp, int32_t length)
{
    _tSampler* p = *sp;
    if (length == 0) length = 1;
    tSampler_setEnd(sp, p->start + length);
}

void tSampler_setRate      (tSampler* const sp, Lfloat rate)
{
    _tSampler* p = *sp;
    
    rate = rate * p->rateFactor; //adjust for sampling rate of buffer (may be different from leaf.sampleRate if audio file was loaded form SD card)
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


void tSampler_setSampleRate(tSampler* const sp, Lfloat sr)
{
    _tSampler* p = *sp;
    _tBuffer* s = p->samp;
    p->sampleRate = sr;
    p->invSampleRate = 1.0f/p->sampleRate;
    p->ticksPerSevenMs = 0.007f * p->sampleRate;
    p->rateFactor = s->sampleRate * p->invSampleRate;
    tRamp_setSampleRate(&p->gain, p->sampleRate);
}

//==============================================================================

void    tAutoSampler_init   (tAutoSampler* const as, tBuffer* const b, LEAF* const leaf)
{
    tAutoSampler_initToPool(as, b, &leaf->mempool, leaf);
}

void    tAutoSampler_initToPool (tAutoSampler* const as, tBuffer* const b, tMempool* const mp, LEAF* const leaf)
{
    _tMempool* m = *mp;
    _tAutoSampler* a = *as = (_tAutoSampler*) mpool_alloc(sizeof(_tAutoSampler), m);
    a->mempool = m;
    
    tBuffer_setRecordMode(b, RecordOneShot);
    tSampler_initToPool(&a->sampler, b, mp, leaf);
    tSampler_setMode(&a->sampler, PlayLoop);
    tEnvelopeFollower_initToPool(&a->ef, 0.05f, 0.9999f, mp);
}

void    tAutoSampler_free (tAutoSampler* const as)
{
    _tAutoSampler* a = *as;
    
    tEnvelopeFollower_free(&a->ef);
    tSampler_free(&a->sampler);
    
    mpool_free((char*)a, a->mempool);
}

Lfloat   tAutoSampler_tick               (tAutoSampler* const as, Lfloat input)
{
    _tAutoSampler* a = *as;
    Lfloat currentPower = tEnvelopeFollower_tick(&a->ef, input);
    
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

void    tAutoSampler_setThreshold       (tAutoSampler* const as, Lfloat thresh)
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

void    tAutoSampler_setRate    (tAutoSampler* const as, Lfloat rate)
{
    ;
}

void    tAutoSampler_setSampleRate (tAutoSampler* const as, Lfloat sr)
{
    _tAutoSampler* a = *as;
    tSampler_setSampleRate(&a->sampler, sr);
}


void tMBSampler_init(tMBSampler* const sp, tBuffer* const b, LEAF* const leaf)
{
    tMBSampler_initToPool(sp, b, &leaf->mempool);
}

void tMBSampler_initToPool(tMBSampler* const sp, tBuffer* const b, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tMBSampler* c = *sp = (_tMBSampler*) mpool_alloc(sizeof(_tMBSampler), m);
    c->mempool = m;
    
    c->samp = *b;
    
    c->mode = PlayLoop;
    c->active = 0;
        
    tExpSmooth_initToPool(&c->gain, 0.0f, 0.01f, mp);
    
    c->last = 0.0f;
    c->amp = 1.0f;
    c->_p = 0.0f;
    c->_w = 1.0f;
    c->syncin = 0.0f;
    c->_z = 0.0f;
    c->_j = 0;
    memset (c->_f, 0, (FILLEN + STEP_DD_PULSE_LENGTH) * sizeof (Lfloat));

    c->start = 0;
    c->end = 1;
    c->currentLoopLength = 1;
    tMBSampler_setEnd(sp, c->samp->bufferLength);
}

void tMBSampler_free (tMBSampler* const sp)
{
    _tMBSampler* p = *sp;
    
    tExpSmooth_free(&p->gain);
    
    mpool_free((char*)p, p->mempool);
}

void tMBSampler_setSample (tMBSampler* const sp, tBuffer* const b)
{
    _tMBSampler* p = *sp;
    
    p->samp = *b;;
    
    p->start = 0;
    tMBSampler_setEnd(sp, p->samp->bufferLength);
    
    p->_p = 0.0f;
}

Lfloat tMBSampler_tick        (tMBSampler* const sp)
{
    _tMBSampler* c = *sp;
    
    if ((c->gain->curr == 0.0f) && (!c->active)) return 0.0f;
    if (c->_w == 0.0f)
    {
        c->_last_w = 0.0f;
        return c->out;
    }
    
    Lfloat last, beforeLast;
    int start, end, length;
    Lfloat* buff;
    int    j;
    //Lfloat  syncin;
    Lfloat  a, p, w, z;
   // syncin  = c->syncin;
        
    start = c->start;
    end = c->end;

    buff = c->samp->buff;
    
    last = c->last;
    beforeLast = c->beforeLast;
    p = c->_p;  /* position */
    w = fminf((Lfloat)c->currentLoopLength * 0.5f, c->_w);  /* rate */
    z = c->_z;  /* low pass filter state */
    j = c->_j;  /* index into buffer _f */
    
    length = end - start;

    //a = 0.2 + 0.8 * vco->_port [FILT];
    a = 0.5f; // when a = 1, LPfilter is disabled
    
    p += w;
    
    Lfloat next, afterNext;
//    if (syncin >= 1e-20f) {  /* sync to master */
//
//        Lfloat eof_offset = (syncin - 1e-20f) * w;
//        Lfloat p_at_reset = p - eof_offset;
//        p = eof_offset;
//
//        /* place any DD that may have occurred in subsample before reset */
//        if (p_at_reset >= end) {
//            while (p_at_reset >= (Lfloat) end) p_at_reset -= (Lfloat) length;
//
//            Lfloat f = p_at_reset + eof_offset;
//            int i = (int) f;
//            f -= i;
//            Lfloat n = buff[i] * (1.0f - f) + buff[i+1] * f;
//
//            place_step_dd(c->_f, j, p_at_reset + eof_offset, w,
//                          n - c->out);
//            place_slope_dd(c->_f, j, p_at_reset + eof_offset, w, (n - c->out) - c->last_delta);
//        }
//
//        Lfloat f = p_at_reset;
//        int i = (int) f;
//        f -= i;
//        next = buff[i] * (1.0f - f) + buff[i+1] * f;
//
//        /* now place reset DD */
//        place_step_dd(c->_f, j, p, w, next - c->out);
//        place_slope_dd(c->_f, j, p, w, (next - c->out) - c->last_delta);
//
//    } else
    
    if (w > 0.0f) {
    
        if (p >= (Lfloat) end) {  /* normal phase reset */
        
            // start and end are never negative and end must also be greater than start
            // so this loop is fine
            while (p >= (Lfloat) end)
            {
                p -= (Lfloat) length;
                c->currentLoopLength = length;
            }
            
            Lfloat f = p;
            int i = (int) f;
            f -= i;
            next = buff[i] * (1.0f - f) + buff[i+1] * f;
            
            f = p + w;
            i = (int) f;
            f -= i;
            afterNext = buff[i] * (1.0f - f) + buff[i+1] * f;
 
            place_step_dd(c->_f, j, p - start, w, next - last);
            Lfloat nextSlope = (afterNext - next) / w;
            Lfloat lastSlope = (last - beforeLast) / w;
            place_slope_dd(c->_f, j, p - start, w, nextSlope - lastSlope);
            if (c->mode == PlayNormal)
            {
            	c->active = 0;
            }
            else if (c->mode == PlayBackAndForth) w = -w;
        }
//        else if (p < (Lfloat) start) { /* start has been set ahead of the current phase */
//            
//            p = (Lfloat) start;
//            next = buff[start];
//           
//            Lfloat f = p + w;
//            int i = (int) f;
//            f -= i;
//            afterNext = buff[i] * (1.0f - f) + buff[i+1] * f;
//
//            place_step_dd(c->_f, j, 0, w, next - last);
//            Lfloat nextSlope = (afterNext - next) / w;
//            Lfloat lastSlope = (last - beforeLast) / w;
//            place_slope_dd(c->_f, j, 0, w, nextSlope - lastSlope);
//        }
        else {
            
            Lfloat f = p;
            int i = (int) f;
            f -= i;
            next = buff[i] * (1.0f - f) + buff[i+1] * f;


            if ((end - p) < 480) // 480 samples should be enough to let the tExpSmooth go from 1 to 0 (10ms at 48k, 5ms at 192k)
            if (c->mode == PlayNormal)
            {
            	tExpSmooth_setDest(&c->gain, 0.0f);
            }
        }

        if (c->_last_w < 0.0f)
        {
            Lfloat f = p + w;
            int i = (int) f;
            f -= i;
            afterNext = buff[i] * (1.0f - f) + buff[i+1] * f;
            
            Lfloat nextSlope = (afterNext - next) / w;
            Lfloat lastSlope = (last - beforeLast) / w;
            place_slope_dd(c->_f, j, p - start, w, nextSlope - lastSlope);
        }
        
    } else { // if (w < 0.0f) {
        
        if (p < (Lfloat) start) {
        
            while (p < (Lfloat) start)
            {
                p += (Lfloat) length;
                c->currentLoopLength = length;
            }
            
            Lfloat f = p;
            int i = (int) f;
            f -= i;
            next = buff[i] * (1.0f - f) + buff[i+1] * f;

            f = p + w;
            i = (int) f;
            f -= i;
            afterNext = buff[i] * (1.0f - f) + buff[i+1] * f;

            place_step_dd(c->_f, j, end - p, w, next - last);
            Lfloat nextSlope = (afterNext - next) / w;
            Lfloat lastSlope = (last - beforeLast) / w;
            place_slope_dd(c->_f, j, end - p, w, nextSlope - lastSlope);
            
            if (c->mode == PlayNormal)
            {
            	c->active = 0;
            }
            else if (c->mode == PlayBackAndForth) w = -w;
        }
//        else if (p > (Lfloat) end) {
//
//            p = (Lfloat) end;
//            next = buff[end];
//
//            Lfloat f = p + w;
//            int i = (int) f;
//            f -= i;
//            afterNext = buff[i] * (1.0f - f) + buff[i+1] * f;
//
//            place_step_dd(c->_f, j, 0, w, next - last);
//            Lfloat nextSlope = (afterNext - next) / w;
//            Lfloat lastSlope = (last - beforeLast) / w;
//            place_slope_dd(c->_f, j, 0, w, nextSlope - lastSlope);
//        }
        else {
            
            if ((p - start) < 480) // 480 samples should be enough to let the tExpSmooth go from 1 to 0 (10ms at 48k, 5ms at 192k)
            if (c->mode == PlayNormal)
            {
            	tExpSmooth_setDest(&c->gain, 0.0f);
            }
            Lfloat f = p;
            int i = (int) f;
            f -= i;
            next = buff[i] * (1.0f - f) + buff[i+1] * f;
        }
        
        if (c->_last_w > 0.0f)
        {
            Lfloat f = p + w;
            int i = (int) f;
            f -= i;
            afterNext = buff[i] * (1.0f - f) + buff[i+1] * f;
            
            Lfloat nextSlope = (afterNext - next) / w;
            Lfloat lastSlope = (last - beforeLast) / w;
            place_slope_dd(c->_f, j, end - p, w, nextSlope - lastSlope);
        }
    }
    
    c->beforeLast = last;
    c->last = next;
    
    c->_f[j + DD_SAMPLE_DELAY] += next;
    
    z += a * (c->_f[j] - z); // LP filtering
    next = c->amp * z;
    
    c->out = next;
    
    if (++j == FILLEN)
    {
        j = 0;
        memcpy (c->_f, c->_f + FILLEN, STEP_DD_PULSE_LENGTH * sizeof (Lfloat));
        memset (c->_f + STEP_DD_PULSE_LENGTH, 0,  FILLEN * sizeof (Lfloat));
    }
    
    c->_p = p;
    c->_w = c->_last_w = w;
    c->_z = z;
    c->_j = j;
    
    return c->out * tExpSmooth_tick(&c->gain);
}

void tMBSampler_setMode      (tMBSampler* const sp, PlayMode mode)
{
    _tMBSampler* p = *sp;
    p->mode = mode;
}

void tMBSampler_play         (tMBSampler* const sp)
{
    _tMBSampler* p = *sp;
    
    if (p->active > 0)
    {
        p->syncin = 1e-20f;
    }
    tExpSmooth_setDest(&p->gain, 1.0f);
    p->active = 1;
    p->_p = p->start;
    p->_z = 0.0f;
    p->_j = 0;
}

void tMBSampler_stop         (tMBSampler* const sp)
{
    _tMBSampler* p = *sp;
    
    tExpSmooth_setDest(&p->gain, 0.0f);
    p->active = 0;
}

void tMBSampler_setStart     (tMBSampler* const sp, int32_t start)
{
    _tMBSampler* p = *sp;
    
    if (start > p->end - 1) p->start = p->end - 1;
    else if (start < 0) p->start = 0;
    else p->start = start;
}

void tMBSampler_setEnd       (tMBSampler* const sp, int32_t end)
{
    _tMBSampler* p = *sp;
    
    if (end < p->start + 1) p->end = p->start + 1;
    // always leave a trailing sample after the end
    else if (end >= p->samp->bufferLength) p->end = p->samp->bufferLength - 1;
    else p->end = end;
}

void    tMBSampler_setLength    (tMBSampler* const sp, int32_t length)
{
    _tMBSampler* p = *sp;
    tMBSampler_setEnd(sp, p->start + length);
}

void tMBSampler_setRate      (tMBSampler* const sp, Lfloat rate)
{
    _tMBSampler* p = *sp;
    
    p->_w = rate;
}

