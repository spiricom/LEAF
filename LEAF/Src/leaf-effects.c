/*==============================================================================

    leaf-vocoder.c
    Created: 20 Jan 2017 12:01:54pm
    Author:  Michael R Mulshine

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-effects.c"
#include "..\leaf.h"

#else

#include "../Inc/leaf-effects.h"
#include "../leaf.h"

#endif



//============================================================================================================
// TALKBOX
//============================================================================================================

void tTalkbox_init(tTalkbox* const v, int bufsize)
{
    v->param[0] = 0.5f;  //wet
    v->param[1] = 0.0f;  //dry
    v->param[2] = 0; // Swap
    v->param[3] = 1.0f;  //quality
    
    v->bufsize = bufsize;
    
    v->car0 =   (float*) leaf_alloc(sizeof(float) * v->bufsize);
    v->car1 =   (float*) leaf_alloc(sizeof(float) * v->bufsize);
    v->window = (float*) leaf_alloc(sizeof(float) * v->bufsize);
    v->buf0 =   (float*) leaf_alloc(sizeof(float) * v->bufsize);
    v->buf1 =   (float*) leaf_alloc(sizeof(float) * v->bufsize);
    
    tTalkbox_update(v);
}

void tTalkbox_free(tTalkbox* const v)
{
    leaf_free(v->buf1);
    leaf_free(v->buf0);
    leaf_free(v->window);
    leaf_free(v->car1);
    leaf_free(v->car0);
}

void tTalkbox_update(tTalkbox* const v) ///update internal parameters...
{
    float fs = leaf.sampleRate;
    if(fs <  8000.0f) fs =  8000.0f;
    if(fs > 96000.0f) fs = 96000.0f;
    
    int32_t n = (int32_t)(0.01633f * fs);
    if(n > v->bufsize) n = v->bufsize;
    
    //O = (VstInt32)(0.0005f * fs);
    v->O = (int32_t)((0.0001f + 0.0004f * v->param[3]) * fs);
    
    if(n != v->N) //recalc hanning window
    {
        v->N = n;
        float dp = TWO_PI / v->N;
        float p = 0.0f;
        for(n=0; n<v->N; n++)
        {
            v->window[n] = 0.5f - 0.5f * cosf(p);
            p += dp;
        }
    }
    v->wet = 0.5f * v->param[0] * v->param[0];
    v->dry = 2.0f * v->param[1] * v->param[1];
}

void tTalkbox_suspend(tTalkbox* const v) ///clear any buffers...
{
    v->pos = v->K = 0;
    v->emphasis = 0.0f;
    v->FX = 0;
    
    v->u0 = v->u1 = v->u2 = v->u3 = v->u4 = 0.0f;
    v->d0 = v->d1 = v->d2 = v->d3 = v->d4 = 0.0f;
    
    for (int32_t i = 0; i < v->bufsize; i++)
    {
        v->buf0[i] = 0;
        v->buf1[i] = 0;
        v->car0[i] = 0;
        v->car1[i] = 0;
    }
}


#define ORD_MAX           100 // Was 50. Increasing this gets rid of glitchiness, lowering it breaks it; not sure how it affects performance
void tTalkbox_lpc(float *buf, float *car, int32_t n, int32_t o)
{
    float z[ORD_MAX], r[ORD_MAX], k[ORD_MAX], G, x;
    int32_t i, j, nn=n;
    
    for(j=0; j<=o; j++, nn--)  //buf[] is already emphasized and windowed
    {
        z[j] = r[j] = 0.0f;
        for(i=0; i<nn; i++) r[j] += buf[i] * buf[i+j]; //autocorrelation
    }
    r[0] *= 1.001f;  //stability fix
    
    float min = 0.00001f;
    if(r[0] < min) { for(i=0; i<n; i++) buf[i] = 0.0f; return; }
    
    tTalkbox_lpcDurbin(r, o, k, &G);  //calc reflection coeffs
    
    for(i=0; i<=o; i++)
    {
        if(k[i] > 0.995f) k[i] = 0.995f; else if(k[i] < -0.995f) k[i] = -.995f;
    }
    
    for(i=0; i<n; i++)
    {
        x = G * car[i];
        for(j=o; j>0; j--)  //lattice filter
        {
            x -= k[j] * z[j-1];
            z[j] = z[j-1] + k[j] * x;
        }
        buf[i] = z[0] = x;  //output buf[] will be windowed elsewhere
    }
}


void tTalkbox_lpcDurbin(float *r, int p, float *k, float *g)
{
    int i, j;
    float a[ORD_MAX], at[ORD_MAX], e=r[0];
    
    for(i=0; i<=p; i++) a[i] = at[i] = 0.0f; //probably don't need to clear at[] or k[]
    
    for(i=1; i<=p; i++)
    {
        k[i] = -r[i];
        
        for(j=1; j<i; j++)
        {
            at[j] = a[j];
            k[i] -= a[j] * r[i-j];
        }
        if(fabs(e) < 1.0e-20f) { e = 0.0f;  break; }
        k[i] /= e; // This might be costing us
        
        a[i] = k[i];
        for(j=1; j<i; j++) a[j] = at[j] + k[i] * at[i-j];
        
        e *= 1.0f - k[i] * k[i];
    }
    
    if(e < 1.0e-20f) e = 0.0f;
    *g = sqrtf(e);
}

float tTalkbox_tick(tTalkbox* const v, float synth, float voice)
{

    int32_t  p0=v->pos, p1 = (v->pos + v->N/2) % v->N;
    float e=v->emphasis, w, o, x, dr, fx=v->FX;
    float p, q, h0=0.3f, h1=0.77f;
    
    o = voice;
    x = synth;
    
    dr = o;
    
    p = v->d0 + h0 *  x; v->d0 = v->d1;  v->d1 = x  - h0 * p;
    q = v->d2 + h1 * v->d4; v->d2 = v->d3;  v->d3 = v->d4 - h1 * q;
    v->d4 = x;
    x = p + q;
    
    if(v->K++)
    {
        v->K = 0;
        
        v->car0[p0] = v->car1[p1] = x; //carrier input
        
        x = o - e;  e = o;  //6dB/oct pre-emphasis
        
        w = v->window[p0]; fx = v->buf0[p0] * w;  v->buf0[p0] = x * w;  //50% overlapping hanning windows
        if(++p0 >= v->N) { tTalkbox_lpc(v->buf0, v->car0, v->N, v->O);  p0 = 0; }
        
        w = 1.0f - w;  fx += v->buf1[p1] * w;  v->buf1[p1] = x * w;
        if(++p1 >= v->N) { tTalkbox_lpc(v->buf1, v->car1, v->N, v->O);  p1 = 0; }
    }
    
    p = v->u0 + h0 * fx; v->u0 = v->u1;  v->u1 = fx - h0 * p;
    q = v->u2 + h1 * v->u4; v->u2 = v->u3;  v->u3 = v->u4 - h1 * q;
    v->u4 = fx;
    x = p + q;
    
    o = x;
    
    v->emphasis = e;
    v->pos = p0;
    v->FX = fx;

    float den = 1.0e-10f; //(float)pow(10.0f, -10.0f * param[4]);
    if(fabs(v->d0) < den) v->d0 = 0.0f; //anti-denormal (doesn't seem necessary but P4?)
    if(fabs(v->d1) < den) v->d1 = 0.0f;
    if(fabs(v->d2) < den) v->d2 = 0.0f;
    if(fabs(v->d3) < den) v->d3 = 0.0f;
    if(fabs(v->u0) < den) v->u0 = 0.0f;
    if(fabs(v->u1) < den) v->u1 = 0.0f;
    if(fabs(v->u2) < den) v->u2 = 0.0f;
    if(fabs(v->u3) < den) v->u3 = 0.0f;
    return o;
}

void tTalkbox_setQuality(tTalkbox* const v, float quality)
{
	v->param[3] = quality;
	v->O = (int32_t)((0.0001f + 0.0004f * v->param[3]) * leaf.sampleRate);
}


//============================================================================================================
// VOCODER
//============================================================================================================

void   tVocoder_init        (tVocoder* const v)
{
    v->param[0] = 0.33f;  //input select
    v->param[1] = 0.50f;  //output dB
    v->param[2] = 0.40f;  //hi thru
    v->param[3] = 0.40f;  //hi band
    v->param[4] = 0.16f;  //envelope
    v->param[5] = 0.55f;  //filter q
    v->param[6] = 0.6667f;//freq range
    v->param[7] = 0.33f;  //num bands
    
    tVocoder_update(v);
}

void tVocoder_free (tVocoder* const v)
{
    
}

void        tVocoder_update      (tVocoder* const v)
{
    float tpofs = 6.2831853f * leaf.invSampleRate;
    
    float rr, th, re;
    
    float sh;
    
    int32_t i;
    
    v->gain = (float)pow(10.0f, 2.0f * v->param[1] - 3.0f * v->param[5] - 2.0f);
    
    v->thru = (float)pow(10.0f, 0.5f + 2.0f * v->param[1]);
    v->high =  v->param[3] * v->param[3] * v->param[3] * v->thru;
    v->thru *= v->param[2] * v->param[2] * v->param[2];
    
    if(v->param[7]<0.5f)
    {
        v->nbnd=8;
        re=0.003f;
        v->f[1][2] = 3000.0f;
        v->f[2][2] = 2200.0f;
        v->f[3][2] = 1500.0f;
        v->f[4][2] = 1080.0f;
        v->f[5][2] = 700.0f;
        v->f[6][2] = 390.0f;
        v->f[7][2] = 190.0f;
    }
    else
    {
        v->nbnd=16;
        re=0.0015f;
        v->f[ 1][2] = 5000.0f; //+1000
        v->f[ 2][2] = 4000.0f; //+750
        v->f[ 3][2] = 3250.0f; //+500
        v->f[ 4][2] = 2750.0f; //+450
        v->f[ 5][2] = 2300.0f; //+300
        v->f[ 6][2] = 2000.0f; //+250
        v->f[ 7][2] = 1750.0f; //+250
        v->f[ 8][2] = 1500.0f; //+250
        v->f[ 9][2] = 1250.0f; //+250
        v->f[10][2] = 1000.0f; //+250
        v->f[11][2] =  750.0f; //+210
        v->f[12][2] =  540.0f; //+190
        v->f[13][2] =  350.0f; //+155
        v->f[14][2] =  195.0f; //+100
        v->f[15][2] =   95.0f;
    }
    
    if(v->param[4]<0.05f) //freeze
    {
        for(i=0;i<v->nbnd;i++) v->f[i][12]=0.0f;
    }
    else
    {
        v->f[0][12] = (float)pow(10.0, -1.7 - 2.7f * v->param[4]); //envelope speed
        
        rr = 0.022f / (float)v->nbnd; //minimum proportional to frequency to stop distortion
        for(i=1;i<v->nbnd;i++)
        {
            v->f[i][12] = (float)(0.025 - rr * (double)i);
            if(v->f[0][12] < v->f[i][12]) v->f[i][12] = v->f[0][12];
        }
        v->f[0][12] = 0.5f * v->f[0][12]; //only top band is at full rate
    }
    
    rr = 1.0 - pow(10.0f, -1.0f - 1.2f * v->param[5]);
    sh = (float)pow(2.0f, 3.0f * v->param[6] - 1.0f); //filter bank range shift
    
    for(i=1;i<v->nbnd;i++)
    {
        v->f[i][2] *= sh;
        th = acos((2.0 * rr * cos(tpofs * v->f[i][2])) / (1.0 + rr * rr));
        v->f[i][0] = (float)(2.0 * rr * cos(th)); //a0
        v->f[i][1] = (float)(-rr * rr);           //a1
        //was .98
        v->f[i][2] *= 0.96f; //shift 2nd stage slightly to stop high resonance peaks
        th = acos((2.0 * rr * cos(tpofs * v->f[i][2])) / (1.0 + rr * rr));
        v->f[i][2] = (float)(2.0 * rr * cos(th));
    }
}

float       tVocoder_tick        (tVocoder* const v, float synth, float voice)
{
    float a, b, o=0.0f, aa, bb, oo = v->kout, g = v->gain, ht = v->thru, hh = v->high, tmp;
    uint32_t i, k = v->kval, nb = v->nbnd;

    a = voice; //speech
    b = synth; //synth
    
    tmp = a - v->f[0][7]; //integrate modulator for HF band and filter bank pre-emphasis
    v->f[0][7] = a;
    a = tmp;
    
    if(tmp<0.0f) tmp = -tmp;
    v->f[0][11] -= v->f[0][12] * (v->f[0][11] - tmp);      //high band envelope
    o = v->f[0][11] * (ht * a + hh * (b - v->f[0][3])); //high band + high thru
    
    v->f[0][3] = b; //integrate carrier for HF band
    
    if(++k & 0x1) //this block runs at half sample rate
    {
        oo = 0.0f;
        aa = a + v->f[0][9] - v->f[0][8] - v->f[0][8];  //apply zeros here instead of in each reson
        v->f[0][9] = v->f[0][8];  v->f[0][8] = a;
        bb = b + v->f[0][5] - v->f[0][4] - v->f[0][4];
        v->f[0][5] = v->f[0][4];  v->f[0][4] = b;
        
        for(i=1; i<nb; i++) //filter bank: 4th-order band pass
        {
            tmp = v->f[i][0] * v->f[i][3] + v->f[i][1] * v->f[i][4] + bb;
            v->f[i][4] = v->f[i][3];
            v->f[i][3] = tmp;
            tmp += v->f[i][2] * v->f[i][5] + v->f[i][1] * v->f[i][6];
            v->f[i][6] = v->f[i][5];
            v->f[i][5] = tmp;
            
            tmp = v->f[i][0] * v->f[i][7] + v->f[i][1] * v->f[i][8] + aa;
            v->f[i][8] = v->f[i][7];
            v->f[i][7] = tmp;
            tmp += v->f[i][2] * v->f[i][9] + v->f[i][1] * v->f[i][10];
            v->f[i][10] = v->f[i][9];
            v->f[i][9] = tmp;
            
            if(tmp<0.0f) tmp = -tmp;
            v->f[i][11] -= v->f[i][12] * (v->f[i][11] - tmp);
            oo += v->f[i][5] * v->f[i][11];
        }
    }
    o += oo * g; //effect of interpolating back up to Fs would be minimal (aliasing >16kHz)

    v->kout = oo;
    v->kval = k & 0x1;
    if(fabs(v->f[0][11])<1.0e-10) v->f[0][11] = 0.0f; //catch HF envelope denormal
    
    for(i=1;i<nb;i++)
        if(fabs(v->f[i][3])<1.0e-10 || fabs(v->f[i][7])<1.0e-10)
            for(k=3; k<12; k++) v->f[i][k] = 0.0f; //catch reson & envelope denormals
    
    if(fabs(o)>10.0f) tVocoder_suspend(v); //catch instability
    
    return o;
    
}

void        tVocoder_suspend     (tVocoder* const v)
{
    int32_t i, j;
    
    for(i=0; i<v->nbnd; i++) for(j=3; j<12; j++) v->f[i][j] = 0.0f; //zero band filters and envelopes
    v->kout = 0.0f;
    v->kval = 0;
}

//============================================================================================================
// RETUNE
//============================================================================================================

void tRetune_init(tRetune* const r, int numVoices, int bufSize, int frameSize)
{
    r->bufSize = bufSize;
    r->frameSize = frameSize;
    r->numVoices = numVoices;
    
    r->inBuffer = (float*) leaf_alloc(sizeof(float) * r->bufSize);
    r->outBuffers = (float**) leaf_alloc(sizeof(float*) * r->numVoices);
    
    r->hopSize = DEFHOPSIZE;
    r->windowSize = DEFWINDOWSIZE;
    r->fba = FBA;
    tRetune_setTimeConstant(r, DEFTIMECONSTANT);
    
    r->inputPeriod = 0.0f;
    
    r->ps = (tPitchShift*) leaf_alloc(sizeof(tPitchShift) * r->numVoices);
    r->pitchFactor = (float*) leaf_alloc(sizeof(float) * r->numVoices);
    r->tickOutput = (float*) leaf_alloc(sizeof(float) * r->numVoices);
    for (int i = 0; i < r->numVoices; ++i)
    {
        r->outBuffers[i] = (float*) leaf_alloc(sizeof(float) * r->bufSize);
    }
    
    tPeriodDetection_init(&r->pd, r->inBuffer, r->outBuffers[0], r->bufSize, r->frameSize);
    
    for (int i = 0; i < r->numVoices; ++i)
    {
        tPitchShift_init(&r->ps[i], &r->pd, r->outBuffers[i], r->bufSize);
    }
}

void tRetune_free(tRetune* const r)
{
    tPeriodDetection_free(&r->pd);
    for (int i = 0; i < r->numVoices; ++i)
    {
        tPitchShift_free(&r->ps[i]);
        leaf_free(r->outBuffers[i]);
    }
    leaf_free(r->tickOutput);
    leaf_free(r->pitchFactor);
    leaf_free(r->ps);
    leaf_free(r->inBuffer);
    leaf_free(r->outBuffers);
}

float* tRetune_tick(tRetune* const r, float sample)
{
    r->inputPeriod = tPeriodDetection_findPeriod(&r->pd, sample);
    
    for (int v = 0; v < r->numVoices; ++v)
    {
        r->tickOutput[v] = tPitchShift_shift(&r->ps[v]);
    }

    return r->tickOutput;
}

void tRetune_setNumVoices(tRetune* const r, int numVoices)
{
    for (int i = 0; i < r->numVoices; ++i)
    {
        tPitchShift_free(&r->ps[i]);
        leaf_free(r->outBuffers[i]);
    }
    leaf_free(r->tickOutput);
    leaf_free(r->pitchFactor);
    leaf_free(r->ps);
    leaf_free(r->outBuffers);
    
    r->numVoices = numVoices;
    
    r->outBuffers = (float**) leaf_alloc(sizeof(float*) * r->numVoices);
    r->ps = (tPitchShift*) leaf_alloc(sizeof(tPitchShift) * r->numVoices);
    r->pitchFactor = (float*) leaf_alloc(sizeof(float) * r->numVoices);
    r->tickOutput = (float*) leaf_alloc(sizeof(float) * r->numVoices);
    for (int i = 0; i < r->numVoices; ++i)
    {
        r->outBuffers[i] = (float*) leaf_alloc(sizeof(float) * r->bufSize);
        tPitchShift_init(&r->ps[i], &r->pd, r->outBuffers[i], r->bufSize);
    }
    
    
}

void tRetune_setPitchFactors(tRetune* const r, float pf)
{
    for (int i = 0; i < r->numVoices; ++i)
    {
        r->pitchFactor[i] = pf;
        tPitchShift_setPitchFactor(&r->ps[i], r->pitchFactor[i]);
    }
}

void tRetune_setPitchFactor(tRetune* const r, float pf, int voice)
{
    r->pitchFactor[voice] = pf;
    tPitchShift_setPitchFactor(&r->ps[voice], r->pitchFactor[voice]);
}

void tRetune_setTimeConstant(tRetune* const r, float tc)
{
    r->timeConstant = tc;
    r->radius = expf(-1000.0f * r->hopSize * leaf.invSampleRate / r->timeConstant);
}

void tRetune_setHopSize(tRetune* const r, int hs)
{
    r->hopSize = hs;
    tPeriodDetection_setHopSize(&r->pd, r->hopSize);
}

void tRetune_setWindowSize(tRetune* const r, int ws)
{
    r->windowSize = ws;
    tPeriodDetection_setWindowSize(&r->pd, r->windowSize);
}

float tRetune_getInputPeriod(tRetune* const r)
{
    return r->inputPeriod;
}

float tRetune_getInputFreq(tRetune* const r)
{
    return 1.0f/r->inputPeriod;
}

//============================================================================================================
// AUTOTUNE
//============================================================================================================

void tAutotune_init(tAutotune* const r, int numVoices, int bufSize, int frameSize)
{
    r->bufSize = bufSize;
    r->frameSize = frameSize;
    r->numVoices = numVoices;
    
    r->inBuffer = (float*) leaf_alloc(sizeof(float) * r->bufSize);
    r->outBuffers = (float**) leaf_alloc(sizeof(float*) * r->numVoices);
    
    r->hopSize = DEFHOPSIZE;
    r->windowSize = DEFWINDOWSIZE;
    r->fba = FBA;
    tAutotune_setTimeConstant(r, DEFTIMECONSTANT);
    
    
    
    r->ps = (tPitchShift*) leaf_alloc(sizeof(tPitchShift) * r->numVoices);
    r->freq = (float*) leaf_alloc(sizeof(float) * r->numVoices);
    r->tickOutput = (float*) leaf_alloc(sizeof(float) * r->numVoices);
    for (int i = 0; i < r->numVoices; ++i)
    {
        r->outBuffers[i] = (float*) leaf_alloc(sizeof(float) * r->bufSize);
    }
    
    tPeriodDetection_init(&r->pd, r->inBuffer, r->outBuffers[0], r->bufSize, r->frameSize);
    
    for (int i = 0; i < r->numVoices; ++i)
    {
        tPitchShift_init(&r->ps[i], &r->pd, r->outBuffers[i], r->bufSize);
    }
    
    r->inputPeriod = 0.0f;
}

void tAutotune_free(tAutotune* const r)
{
    tPeriodDetection_free(&r->pd);
    for (int i = 0; i < r->numVoices; ++i)
    {
        tPitchShift_free(&r->ps[i]);
        leaf_free(r->outBuffers[i]);
    }
    leaf_free(r->tickOutput);
    leaf_free(r->freq);
    leaf_free(r->ps);
    leaf_free(r->inBuffer);
    leaf_free(r->outBuffers);
}

float* tAutotune_tick(tAutotune* const r, float sample)
{
    r->inputPeriod = tPeriodDetection_findPeriod(&r->pd, sample);
    
    for (int v = 0; v < r->numVoices; ++v)
    {
        r->tickOutput[v] = tPitchShift_shiftToFreq(&r->ps[v], r->freq[v]);
    }
    
    return r->tickOutput;
}

void tAutotune_setNumVoices(tAutotune* const r, int numVoices)
{
    for (int i = 0; i < r->numVoices; ++i)
    {
        tPitchShift_free(&r->ps[i]);
        leaf_free(r->outBuffers[i]);
    }
    leaf_free(r->tickOutput);
    leaf_free(r->freq);
    leaf_free(r->ps);
    leaf_free(r->outBuffers);
    
    r->numVoices = numVoices;
    
    r->outBuffers = (float**) leaf_alloc(sizeof(float*) * r->numVoices);
    r->ps = (tPitchShift*) leaf_alloc(sizeof(tPitchShift) * r->numVoices);
    r->freq = (float*) leaf_alloc(sizeof(float) * r->numVoices);
    r->tickOutput = (float*) leaf_alloc(sizeof(float) * r->numVoices);
    for (int i = 0; i < r->numVoices; ++i)
    {
        r->outBuffers[i] = (float*) leaf_alloc(sizeof(float) * r->bufSize);
        tPitchShift_init(&r->ps[i], &r->pd, r->outBuffers[i], r->bufSize);
    }
    
    
}

void tAutotune_setFreqs(tAutotune* const r, float f)
{
    for (int i = 0; i < r->numVoices; ++i)
    {
        r->freq[i] = f;
    }
}

void tAutotune_setFreq(tAutotune* const r, float f, int voice)
{
    r->freq[voice] = f;
}

void tAutotune_setTimeConstant(tAutotune* const r, float tc)
{
    r->timeConstant = tc;
    r->radius = expf(-1000.0f * r->hopSize * leaf.invSampleRate / r->timeConstant);
}

void tAutotune_setHopSize(tAutotune* const r, int hs)
{
    r->hopSize = hs;
    tPeriodDetection_setHopSize(&r->pd, r->hopSize);
}

void tAutotune_setWindowSize(tAutotune* const r, int ws)
{
    r->windowSize = ws;
    tPeriodDetection_setWindowSize(&r->pd, r->windowSize);
}

float tAutotune_getInputPeriod(tAutotune* const r)
{
    return r->inputPeriod;
}

float tAutotune_getInputFreq(tAutotune* const r)
{
    return 1.0f/r->inputPeriod;
}

//============================================================================================================
// PITCHSHIFT
//============================================================================================================

static int pitchshift_attackdetect(tPitchShift* ps)
{
    float envout;
    
    envout = tEnvPD_tick(&ps->p->env);
    
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

void tPitchShift_init (tPitchShift* const ps, tPeriodDetection* p, float* out, int bufSize)
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

void tPitchShift_setPitchFactor(tPitchShift* ps, float pf)
{
    ps->pitchFactor = pf;
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
        
        if (period != 0) ps->pitchFactor = period*freq*leaf.invSampleRate;
        else ps->pitchFactor = 1.0f;

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

//============================================================================================================
// SOLAD
//============================================================================================================
/******************************************************************************/
/***************** static function declarations *******************************/
/******************************************************************************/

static void solad_init(tSOLAD *w);
static inline float read_sample(tSOLAD *w, float floatindex);
static void pitchdown(tSOLAD *w, float *out);
static void pitchup(tSOLAD *w, float *out);

/******************************************************************************/
/***************** public access functions ************************************/
/******************************************************************************/

// init
void     tSOLAD_init(tSOLAD* const w)
{
    w->pitchfactor = 1.;
    w->delaybuf = (float*) leaf_alloc(sizeof(float) * (LOOPSIZE+16));
    solad_init(w);
}

void tSOLAD_free(tSOLAD* const w)
{
    leaf_free(w->delaybuf);
}

// send one block of input samples, receive one block of output samples
void tSOLAD_ioSamples(tSOLAD* const w, float* in, float* out, int blocksize)
{
    int i = w->timeindex;
    int n = w->blocksize = blocksize;
    
    if(!i) w->delaybuf[LOOPSIZE] = in[0];   // copy one sample for interpolation
    while(n--) w->delaybuf[i++] = *in++;    // copy one input block to delay buffer
    
    if(w->pitchfactor > 1) pitchup(w, out);
    else pitchdown(w, out);
    
    w->timeindex += blocksize;
    w->timeindex &= LOOPMASK;
}

// set periodicity analysis data
void tSOLAD_setPeriod(tSOLAD* const w, float period)
{
    if(period > MAXPERIOD) period = MAXPERIOD;
    if(period > MINPERIOD) w->period = period;  // ignore period when too small
}

// set pitch factor between 0.25 and 4
void tSOLAD_setPitchFactor(tSOLAD* const w, float pitchfactor)
{
    if (pitchfactor <= 0.0f) return;
    w->pitchfactor = pitchfactor;
}

// force readpointer lag
void tSOLAD_setReadLag(tSOLAD* const w, float readlag)
{
    if(readlag < 0) readlag = 0;
    if(readlag < w->readlag)               // do not jump backward, only forward
    {
        w->jump = w->readlag - readlag;
        w->readlag = readlag;
        w->xfadelength = readlag;
        w->xfadevalue = 1;
    }
}

// reset state variables
void tSOLAD_resetState(tSOLAD* const w)
{
    int n = LOOPSIZE + 1;
    float *buf = w->delaybuf;
    
    while(n--) *buf++ = 0;
    solad_init(w);
}

/******************************************************************************/
/******************** private procedures **************************************/
/******************************************************************************/

/*
 Function pitchdown() is called to read samples from the delay buffer when pitch
 factor is between 0.25 and 1. The read pointer lags behind because of the slowed
 down speed, and it must jump forward towards the write pointer soon as there is
 sufficient space to jump. That is, if there is at least one period of the input
 signal between read pointer and write pointer.  When short periods follow up on
 long periods, the read pointer may have space to jump over more than one period
 lenghts. Jump length must be [periodlength ^ 2] in any case.
 
 A linear crossfade function joins the jump-from point with the jump-to point.
 The crossfade must be completed before another read pointer jump is allowed.
 Length of the crossfade function is stored as a number of samples in terms of
 the input sample rate. This length is dynamically translated
 to a crossfade length expressed in output reading rate, according to pitch
 factor which can change before the crossfade is completed. Crossfade length does
 not cover an invariable length in periods for all pitch transposition factors.
 For pitch factors from 0.5 till 1, crossfade length is stretched in the
 output just as much as the signal itself, as crossfade speed is set to equal
 pitch factor. For pitch factors below 0.5, the read pointer wants to jump
 forward before one period is read, therefore the crossfade length as expressed
 in output periods must be shorter. Crossfade speed is set to [1 - pitchfactor]
 for those cases. Pitch factor 0.5 is the natural switch point between crossfade
 speeds [pitchfactor] and [1 - pitchfactor] because 0.5 == 1 - 0.5. The crossfade
 speed modification for pitch factors below 0.5 also means that much of the
 original signal content will be skipped.
 */


static void pitchdown(tSOLAD* const w, float *out)
{
    int n = w->blocksize;
    float refindex = (float)(w->timeindex + LOOPSIZE); // no negative values!
    float pitchfactor = w->pitchfactor;
    float period = w->period;
    float readlag = w->readlag;
    float readlagstep = 1 - pitchfactor;
    float jump = w->jump;
    float xfadevalue = w->xfadevalue;
    float xfadelength = w->xfadelength;
    float xfadespeed, xfadestep, readindex, outputsample;
    
    if(pitchfactor > 0.5) xfadespeed = pitchfactor;
    else xfadespeed = 1 - pitchfactor;
    xfadestep = xfadespeed / xfadelength;
    
    while(n--)
    {
        if(readlag > period)        // check if read pointer may jump forward...
        {
            if(xfadevalue <= 0)      // ...but do not interrupt crossfade
            {
                jump = period;                           // jump forward
                while((jump * 2) < readlag) jump *= 2;   // use available space
                readlag -= jump;                         // reduce read pointer lag
                xfadevalue = 1;                          // start crossfade
                xfadelength = period - 1;
                xfadestep = xfadespeed / xfadelength;
            }
        }
        
        readindex = refindex - readlag;
        outputsample = read_sample(w, readindex);
        
        if(xfadevalue > 0)
        {
            outputsample *= (1 - xfadevalue);                               // fadein
            outputsample += read_sample(w, readindex - jump) * xfadevalue;  // fadeout
            xfadevalue -= xfadestep;
        }
        
        *out++ = outputsample;
        refindex += 1;
        readlag += readlagstep;
    }
    
    w->jump = jump;                 // state variables
    w->readlag = readlag;
    w->xfadevalue = xfadevalue;
    w->xfadelength = xfadelength;
}


/*
 Function pitchup() for pitch factors above 1 is more complicated than
 pitchdown(). The read pointer increments faster than the write pointer and a
 backward jump must happen in time, reckoning with the crossfade region. The read
 pointer backward jump length is always one period. In order to minimize the area
 of signal duplicates, crossfade length is aimed at [period / pitchfactor].
 This leads to a crossfade speed of [pitchfactor * pitchfactor].
 
 Some samples for the fade out (but not all of them) must already be in the
 buffer, otherwise we will run out of input samples before the crossfade is
 completed. The ratio of past samples and future samples for a crossfade of any
 length is as follows:
 
 past samples: xfadelength * (1 - 1 / pitchfactor)
 future samples: xfadelength * (1 / pitchfactor)
 
 For example in the case of pitch factor 1.5 this would be:
 
 past samples: xfadelength * (1 - 1 / 1.5) = xfadelength * 1 / 3
 future samples: xfadelength * (1 / 1.5) = xfadelength * 2 / 3
 
 In the case of pitch factor 4 this would be:
 
 past samples: xfadelength * (1 - 1 / 4) = xfadelength * 3 / 4
 future samples: xfadelength * (1 / 4) = xfadelength * 1 / 4
 
 The read pointer lag must therefore preserve a minimum dependent on pitch
 factor. The minimum is called 'limit' here:
 
 limit = period * (pitchfactor - 1) / pitchfactor * pitchfactor
 
 Components of this expression are combined to reuse them in operations, while
 (pitchfactor - 1) is changed to (pitchfactor - 0.99) to avoid numerical
 resolution issues for pitch factors slightly above 1:
 
 xfadespeed = pitchfactor * pitchfactor
 limitfactor = (pitchfactor - 0.99) / xfadespeed
 limit = period * limitfactor
 
 When read lag is smaller than this limit, the read pointer must preferably
 jump backward, unless a previous crossfade is not yet completed. Crossfades must
 preferably be completed, unless the read pointer lag becomes smaller than zero.
 With fluctuating period lengths and pitch factors, the readpointer lag limit may
 change from one input block to the next in such a way that the actual lag is
 suddenly much smaller than the limit, and the intended crossfade length can not
 be applied. Therefore the crossfade length is simply calculated from the
 available amount of samples for all cases, like so:
 
 xfadelength = readlag / limitfactor
 
 For most occurrences, this will amount to a crossfade length reduced to
 [period / pitchfactor] in the output for pitch factors above 1, while in some
 cases it will be considerably shorter. Fortunately, an incidental aberration of
 the intended crossfade length hardly ever creates an audible artifact. The
 reason to specify preferred crossfade length according to pitch factor is to
 minimize the impression of echoes without sacrificing too much of the signal
 content. The readpointer jump length remains one period in any case.
 
 Sometimes, the input signal periodicity may decrease substantially between one
 signal block and the next. In such cases it may be possible for the read pointer
 to jump forward and reduce latency. For every signal block, a check on this
 possibility is done. A previous crossfade must be completed before a forward
 jump is allowed.
 */
static void pitchup(tSOLAD* const w, float *out)
{
    int n = w->blocksize;
    float refindex = (float)(w->timeindex + LOOPSIZE); // no negative values
    float pitchfactor = w->pitchfactor;
    float period = w->period;
    float readlag = w->readlag;
    float jump = w->jump;
    float xfadevalue = w->xfadevalue;
    float xfadelength = w->xfadelength;
    
    float readlagstep = pitchfactor - 1;
    float xfadespeed = pitchfactor * pitchfactor;
    float xfadestep = xfadespeed / xfadelength;
    float limitfactor = (pitchfactor - (float)0.99) / xfadespeed;
    float limit = period * limitfactor;
    float readindex, outputsample;
    
    if((readlag > (period + 2 * limit)) & (xfadevalue < 0))
    {
        jump = period;                                        // jump forward
        while((jump * 2) < (readlag - 2 * limit)) jump *= 2;  // use available space
        readlag -= jump;                                      // reduce read pointer lag
        xfadevalue = 1;                                       // start crossfade
        xfadelength = period - 1;
        xfadestep = xfadespeed / xfadelength;
    }
    
    while(n--)
    {
        if(readlag < limit)  // check if read pointer should jump backward...
        {
            if((xfadevalue < 0) | (readlag < 0)) // ...but try not to interrupt crossfade
            {
                xfadelength = readlag / limitfactor;
                if(xfadelength < 1) xfadelength = 1;
                xfadestep = xfadespeed / xfadelength;
                
                jump = -period;         // jump backward
                readlag += period;      // increase read pointer lag
                xfadevalue = 1;         // start crossfade
            }
        }
        
        readindex = refindex - readlag;
        outputsample = read_sample(w, readindex);
        
        if(xfadevalue > 0)
        {
            outputsample *= (1 - xfadevalue);
            outputsample += read_sample(w, readindex - jump) * xfadevalue;
            xfadevalue -= xfadestep;
        }
        
        *out++ = outputsample;
        refindex += 1;
        readlag -= readlagstep;
    }
    
    w->readlag = readlag;               // state variables
    w->jump = jump;
    w->xfadelength = xfadelength;
    w->xfadevalue = xfadevalue;
}

// read one sample from delay buffer, with linear interpolation
static inline float read_sample(tSOLAD* const w, float floatindex)
{
    int index = (int)floatindex;
    float fraction = floatindex - (float)index;
    float *buf = w->delaybuf;
    index &= LOOPMASK;
    
    return (buf[index] + (fraction * (buf[index+1] - buf[index])));
}

static void solad_init(tSOLAD* const w)
{
    w->timeindex = 0;
    w->xfadevalue = -1;
    w->period = INITPERIOD;
    w->readlag = INITPERIOD;
    w->blocksize = INITPERIOD;
}
//============================================================================================================
// FORMANTSHIFTER
//============================================================================================================

void tFormantShifter_init(tFormantShifter* const fs, int bufsize, int order)
{
    fs->ford = order;
    fs->bufsize = bufsize;
    fs->fk = (float*) leaf_alloc(sizeof(float) * fs->ford);
    fs->fb = (float*) leaf_alloc(sizeof(float) * fs->ford);
    fs->fc = (float*) leaf_alloc(sizeof(float) * fs->ford);
    fs->frb = (float*) leaf_alloc(sizeof(float) * fs->ford);
    fs->frc = (float*) leaf_alloc(sizeof(float) * fs->ford);
    fs->fsig = (float*) leaf_alloc(sizeof(float) * fs->ford);
    fs->fsmooth = (float*) leaf_alloc(sizeof(float) * fs->ford);
    fs->ftvec = (float*) leaf_alloc(sizeof(float) * fs->ford);
    
    fs->fbuff = (float**) leaf_alloc(sizeof(float*) * fs->ford);
    for (int i = 0; i < fs->ford; i++)
    {
        fs->fbuff[i] = (float*) leaf_alloc(sizeof(float) * fs->bufsize);
    }
    
    
    fs->falph = powf(0.001f, 80.0f / (leaf.sampleRate));
    fs->flamb = -(0.8517f*sqrt(atanf(0.06583f*leaf.sampleRate))-0.1916f);
    fs->fhp = 0.0f;
    fs->flp = 0.0f;
    fs->flpa = powf(0.001f, 10.0f / (leaf.sampleRate));
    fs->fmute = 1.0f;
    fs->fmutealph = powf(0.001f, 1.0f / (leaf.sampleRate));
    fs->cbi = 0;
}

void tFormantShifter_free(tFormantShifter* const fs)
{
    leaf_free(fs->fk);
    leaf_free(fs->fb);
    leaf_free(fs->fc);
    leaf_free(fs->frb);
    leaf_free(fs->frc);
    leaf_free(fs->fsig);
    leaf_free(fs->fsmooth);
    leaf_free(fs->ftvec);
    for (int i = 0; i < fs->ford; i++)
    {
        leaf_free(fs->fbuff[i]);
    }
    leaf_free(fs->fbuff);
}


float tFormantShifter_tick(tFormantShifter* fs, float in, float fwarp)
{
    return tFormantShifter_add(fs, tFormantShifter_remove(fs, in), fwarp);
}

float tFormantShifter_remove(tFormantShifter* fs, float in)
{
    float fa, fb, fc, foma, falph, ford, flpa, flamb, tf, fk;
    int ti4;
    ford = fs->ford;
    falph = fs->falph;
    foma = (1.0f - falph);
    flpa = fs->flpa;
    flamb = fs->flamb;
    
    tf = in;
    ti4 = fs->cbi;
    
    fa = tf - fs->fhp;
    fs->fhp = tf;
    fb = fa;
    for(int i = 0; i < ford; i++)
    {
        fs->fsig[i] = fa*fa*foma + fs->fsig[i]*falph;
        fc = (fb - fs->fc[i])*flamb + fs->fb[i];
        fs->fc[i] = fc;
        fs->fb[i] = fb;
        fk = fa*fc*foma + fs->fk[i]*falph;
        fs->fk[i] = fk;
        tf = fk/(fs->fsig[i] + 0.000001f);
        tf = tf*foma + fs->fsmooth[i]*falph;
        fs->fsmooth[i] = tf;
        fs->fbuff[i][ti4] = tf;
        fb = fc - tf*fa;
        fa = fa - tf*fc;
    }
    fs->cbi++;
    if(fs->cbi >= fs->bufsize)
    {
        fs->cbi = 0;
    }
    
    return fa;
}

float tFormantShifter_add(tFormantShifter* fs, float in, float fwarp)
{
    float fa, fb, fc, foma, falph, ford, flpa, flamb, tf, tf2, f0resp, f1resp, frlamb;
    int ti4;
    ford = fs->ford;
    falph = fs->falph;
    foma = (1.0f - falph);
    flpa = fs->flpa;
    flamb = fs->flamb;
    tf = exp2(fwarp/2.0f) * (1+flamb)/(1-flamb);
    frlamb = (tf-1)/(tf+1);
    ti4 = fs->cbi;
    
    tf2 = in;
    fa = 0;
    fb = fa;
    for (int i=0; i<ford; i++)
    {
        fc = (fb-fs->frc[i])*frlamb + fs->frb[i];
        tf = fs->fbuff[i][ti4];
        fb = fc - tf*fa;
        fs->ftvec[i] = tf*fc;
        fa = fa - fs->ftvec[i];
    }
    tf = -fa;
    for (int i=ford-1; i>=0; i--)
    {
        tf = tf + fs->ftvec[i];
    }
    f0resp = tf;
    
    //  second time: compute 1-response
    fa = 1;
    fb = fa;
    for (int i=0; i<ford; i++)
    {
        fc = (fb-fs->frc[i])*frlamb + fs->frb[i];
        tf = fs->fbuff[i][ti4];
        fb = fc - tf*fa;
        fs->ftvec[i] = tf*fc;
        fa = fa - fs->ftvec[i];
    }
    tf = -fa;
    for (int i=ford-1; i>=0; i--)
    {
        tf = tf + fs->ftvec[i];
    }
    f1resp = tf;
    
    //  now solve equations for output, based on 0-response and 1-response
    tf = (float)2*tf2;
    tf2 = tf;
    tf = ((float)1 - f1resp + f0resp);
    if (tf!=0)
    {
        tf2 = (tf2 + f0resp) / tf;
    }
    else
    {
        tf2 = 0;
    }
    
    //  third time: update delay registers
    fa = tf2;
    fb = fa;
    for (int i=0; i<ford; i++)
    {
        fc = (fb-fs->frc[i])*frlamb + fs->frb[i];
        fs->frc[i] = fc;
        fs->frb[i] = fb;
        tf = fs->fbuff[i][ti4];
        fb = fc - tf*fa;
        fa = fa - tf*fc;
    }
    tf = tf2;
    tf = tf + flpa * fs->flp;  // lowpass post-emphasis filter
    fs->flp = tf;
    
    // Bring up the gain slowly when formant correction goes from disabled
    // to enabled, while things stabilize.
    if (fs->fmute>0.5)
    {
        tf = tf*(fs->fmute - 0.5)*2;
    }
    else
    {
        tf = 0;
    }
    tf2 = fs->fmutealph;
    fs->fmute = (1-tf2) + tf2*fs->fmute;
    // now tf is signal output
    // ...and we're done messing with formants
    
    return tf;
}
