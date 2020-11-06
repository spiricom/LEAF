/*==============================================================================

    leaf-analysis.c
    Created: 30 Nov 2018 11:56:49am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-analysis.h"
#include "..\Externals\d_fft_mayer.h"
#include <intrin.h>
#else

#include "../Inc/leaf-analysis.h"
#include "../Externals/d_fft_mayer.h"

#endif

//===========================================================================
/* Envelope Follower */
//===========================================================================

void    tEnvelopeFollower_init(tEnvelopeFollower* const ef, float attackThreshold, float decayCoeff, LEAF* const leaf)
{
    tEnvelopeFollower_initToPool(ef, attackThreshold, decayCoeff, &leaf->mempool);
}

void    tEnvelopeFollower_initToPool    (tEnvelopeFollower* const ef, float attackThreshold, float decayCoeff, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tEnvelopeFollower* e = *ef = (_tEnvelopeFollower*) mpool_alloc(sizeof(_tEnvelopeFollower), m);
    e->mempool = m;
    
    e->y = 0.0f;
    e->a_thresh = attackThreshold;
    e->d_coeff = decayCoeff;
}

void    tEnvelopeFollower_free  (tEnvelopeFollower* const ef)
{
    _tEnvelopeFollower* e = *ef;
    
    mpool_free((char*)e, e->mempool);
}

float   tEnvelopeFollower_tick(tEnvelopeFollower* const ef, float x)
{
    _tEnvelopeFollower* e = *ef;
    
    if (x < 0.0f ) x = -x;  /* Absolute value. */
    
    if ((x >= e->y) && (x > e->a_thresh)) e->y = x;                      /* If we hit a peak, ride the peak to the top. */
    else                                    e->y = e->y * e->d_coeff;    /* Else, exponential decay of output. */
    
    //ef->y = envelope_pow[(uint16_t)(ef->y * (float)UINT16_MAX)] * ef->d_coeff; //not quite the right behavior - too much loss of precision?
    //ef->y = powf(ef->y, 1.000009f) * ef->d_coeff;  // too expensive
    
#ifdef NO_DENORMAL_CHECK
#else
    if( e->y < VSF)   e->y = 0.0f;
#endif
    return e->y;
}

void    tEnvelopeFollower_setDecayCoefficient(tEnvelopeFollower* const ef, float decayCoeff)
{
    _tEnvelopeFollower* e = *ef;
    e->d_coeff = decayCoeff;
}

void    tEnvelopeFollower_setAttackThreshold(tEnvelopeFollower* const ef, float attackThresh)
{
    _tEnvelopeFollower* e = *ef;
    e->a_thresh = attackThresh;
}





// zero crossing detector

void    tZeroCrossingCounter_init         (tZeroCrossingCounter* const zc, int maxWindowSize, LEAF* const leaf)
{
    tZeroCrossingCounter_initToPool   (zc, maxWindowSize, &leaf->mempool);
}

void    tZeroCrossingCounter_initToPool   (tZeroCrossingCounter* const zc, int maxWindowSize, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tZeroCrossingCounter* z = *zc = (_tZeroCrossingCounter*) mpool_alloc(sizeof(_tZeroCrossingCounter), m);
    z->mempool = m;

    z->count = 0;
    z->maxWindowSize = maxWindowSize;
    z->currentWindowSize = maxWindowSize;
    z->invCurrentWindowSize = 1.0f / (float)maxWindowSize;
    z->position = 0;
    z->prevPosition = maxWindowSize;
    z->inBuffer = (float*) mpool_calloc(sizeof(float) * maxWindowSize, m);
    z->countBuffer = (uint16_t*) mpool_calloc(sizeof(uint16_t) * maxWindowSize, m);
}

void    tZeroCrossingCounter_free (tZeroCrossingCounter* const zc)
{
    _tZeroCrossingCounter* z = *zc;
    
    mpool_free((char*)z->inBuffer, z->mempool);
    mpool_free((char*)z->countBuffer, z->mempool);
    mpool_free((char*)z, z->mempool);
}

//returns proportion of zero crossings within window size (0.0 would be none in window, 1.0 would be all zero crossings)
float   tZeroCrossingCounter_tick         (tZeroCrossingCounter* const zc, float input)
{
    _tZeroCrossingCounter* z = *zc;

    z->inBuffer[z->position] = input;
    int futurePosition = ((z->position + 1) % z->currentWindowSize);
    float output = 0.0f;

    //add new value to count
    if ((z->inBuffer[z->position] * z->inBuffer[z->prevPosition]) < 0.0f)
    {
        //zero crossing happened, add it to the count array
        z->countBuffer[z->position] = 1;
        z->count++;
    }
    else
    {
        z->countBuffer[z->position] = 0;
    }

    //remove oldest value from count
    if (z->countBuffer[futurePosition] > 0)
    {
        z->count--;
        if (z->count < 0)
        {
            z->count = 0;
        }
    }

    z->prevPosition = z->position;
    z->position = futurePosition;

    output = z->count * z->invCurrentWindowSize;

    return output;
}


void    tZeroCrossingCounter_setWindowSize        (tZeroCrossingCounter* const zc, float windowSize)
{
    _tZeroCrossingCounter* z = *zc;
    if (windowSize <= z->maxWindowSize)
        {
            z->currentWindowSize = windowSize;
        }
    else
    {
        z->currentWindowSize = z->maxWindowSize;
    }
    z->invCurrentWindowSize = 1.0f / z->currentWindowSize;
}





//===========================================================================
/* Power Follower */
//===========================================================================
void    tPowerFollower_init(tPowerFollower* const pf, float factor, LEAF* const leaf)
{
    tPowerFollower_initToPool(pf, factor, &leaf->mempool);
}

void    tPowerFollower_initToPool   (tPowerFollower* const pf, float factor, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPowerFollower* p = *pf = (_tPowerFollower*) mpool_alloc(sizeof(_tPowerFollower), m);
    p->mempool = m;
    
    p->curr=0.0f;
    p->factor=factor;
    p->oneminusfactor=1.0f-factor;
}

void    tPowerFollower_free (tPowerFollower* const pf)
{
    _tPowerFollower* p = *pf;
    
    mpool_free((char*)p, p->mempool);
}

int     tPowerFollower_setFactor(tPowerFollower* const pf, float factor)
{
    _tPowerFollower* p = *pf;
    
    if (factor<0) factor=0;
    if (factor>1) factor=1;
    p->factor=factor;
    p->oneminusfactor=1.0f-factor;
    return 0;
}

float   tPowerFollower_tick(tPowerFollower* const pf, float input)
{
    _tPowerFollower* p = *pf;
    p->curr = p->factor*input*input+p->oneminusfactor*p->curr;
    return p->curr;
}

float   tPowerFollower_sample(tPowerFollower* const pf)
{
    _tPowerFollower* p = *pf;
    return p->curr;
}




//===========================================================================
/* ---------------- env~ - simple envelope follower. ----------------- */
//===========================================================================

void tEnvPD_init(tEnvPD* const xpd, int ws, int hs, int bs, LEAF* const leaf)
{
    tEnvPD_initToPool(xpd, ws, hs, bs, &leaf->mempool);
}

void    tEnvPD_initToPool       (tEnvPD* const xpd, int ws, int hs, int bs, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tEnvPD* x = *xpd = (_tEnvPD*) mpool_calloc(sizeof(_tEnvPD), m);
    x->mempool = m;
    
    int period = hs, npoints = ws;
    
    int i;
    
    if (npoints < 1) npoints = 1024;
    if (period < 1) period = npoints/2;
    if (period < npoints / MAXOVERLAP + 1)
        period = npoints / MAXOVERLAP + 1;
    
    x->x_npoints = npoints;
    x->x_phase = 0;
    x->x_period = period;
    
    x->windowSize = npoints;
    x->hopSize = period;
    x->blockSize = bs;
    
    for (i = 0; i < MAXOVERLAP; i++) x->x_sumbuf[i] = 0;
    for (i = 0; i < npoints; i++)
        x->buf[i] = (1.0f - cosf((2 * PI * i) / npoints))/npoints;
    for (; i < npoints+INITVSTAKEN; i++) x->buf[i] = 0;
    
    x->x_f = 0;
    
    x->x_allocforvs = INITVSTAKEN;
    
    // ~ ~ ~ dsp ~ ~ ~
    if (x->x_period % x->blockSize)
    {
        x->x_realperiod = x->x_period + x->blockSize - (x->x_period % x->blockSize);
    }
    else
    {
        x->x_realperiod = x->x_period;
    }
    // ~ ~ ~ ~ ~ ~ ~ ~
}

void tEnvPD_free (tEnvPD* const xpd)
{
    _tEnvPD* x = *xpd;
    
    mpool_free((char*)x, x->mempool);
}

float tEnvPD_tick (tEnvPD* const xpd)
{
    _tEnvPD* x = *xpd;
    return powtodb(x->x_result);
}

void tEnvPD_processBlock(tEnvPD* const xpd, float* in)
{
    _tEnvPD* x = *xpd;
    
    int n = x->blockSize;
    
    int count;
    t_sample *sump;
    in += n;
    for (count = x->x_phase, sump = x->x_sumbuf;
         count < x->x_npoints; count += x->x_realperiod, sump++)
    {
        t_sample *hp = x->buf + count;
        t_sample *fp = in;
        t_sample sum = *sump;
        int i;
        
        for (i = 0; i < n; i++)
        {
            fp--;
            sum += *hp++ * (*fp * *fp);
        }
        *sump = sum;
    }
    sump[0] = 0;
    x->x_phase -= n;
    if (x->x_phase < 0)
    {
        x->x_result = x->x_sumbuf[0];
        for (count = x->x_realperiod, sump = x->x_sumbuf;
             count < x->x_npoints; count += x->x_realperiod, sump++)
            sump[0] = sump[1];
        sump[0] = 0;
        x->x_phase = x->x_realperiod - n;
    }
}

//===========================================================================
// ATTACKDETECTION
//===========================================================================
/********Private function prototypes**********/
static void atkdtk_init(tAttackDetection* const a, int blocksize, int atk, int rel);
static void atkdtk_envelope(tAttackDetection* const a, float *in);

/********Constructor/Destructor***************/

void tAttackDetection_init(tAttackDetection* const ad, int blocksize, int atk, int rel, LEAF* const leaf)
{
    tAttackDetection_initToPool(ad, blocksize, atk, rel, &leaf->mempool);
}

void tAttackDetection_initToPool     (tAttackDetection* const ad, int blocksize, int atk, int rel, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tAttackDetection* a = *ad = (_tAttackDetection*) mpool_alloc(sizeof(_tAttackDetection), m);
    a->mempool = m;
    
    atkdtk_init(ad, blocksize, atk, rel);
}

void tAttackDetection_free (tAttackDetection* const ad)
{
    _tAttackDetection* a = *ad;
    
    mpool_free((char*)a, a->mempool);
}

/*******Public Functions***********/


void tAttackDetection_setBlocksize(tAttackDetection* const ad, int size)
{
    _tAttackDetection* a = *ad;
    
    a->blocksize = size;
}

void tAttackDetection_setSamplerate(tAttackDetection* const ad, int inRate)
{
    _tAttackDetection* a = *ad;
    
    a->samplerate = inRate;
    
    //Reset atk and rel to recalculate coeff
    tAttackDetection_setAttack(ad, a->atk);
    tAttackDetection_setRelease(ad, a->rel);
}

void tAttackDetection_setThreshold(tAttackDetection* const ad, float thres)
{
    _tAttackDetection* a = *ad;
    a->threshold = thres;
}

void tAttackDetection_setAttack(tAttackDetection* const ad, int inAtk)
{
    _tAttackDetection* a = *ad;
    a->atk = inAtk;
    a->atk_coeff = powf(0.01f, 1.0f/(a->atk * a->samplerate * 0.001f));
}

void tAttackDetection_setRelease(tAttackDetection* const ad, int inRel)
{
    _tAttackDetection* a = *ad;
    a->rel = inRel;
    a->rel_coeff = powf(0.01f, 1.0f/(a->rel * a->samplerate * 0.001f));
}


int tAttackDetection_detect(tAttackDetection* const ad, float *in)
{
    _tAttackDetection* a = *ad;
    
    int result;
    
    atkdtk_envelope(ad, in);
    
    if(a->env >= a->prevAmp*2) //2 times greater = 6dB increase
        result = 1;
    else
        result = 0;
    
    a->prevAmp = a->env;
    
    return result;
}

/*******Private Functions**********/

static void atkdtk_init(tAttackDetection* const ad, int blocksize, int atk, int rel)
{
    _tAttackDetection* a = *ad;
    LEAF* leaf = a->mempool->leaf;
    
    a->env = 0;
    a->blocksize = blocksize;
    a->threshold = DEFTHRESHOLD;
    a->samplerate = leaf->sampleRate;
    a->prevAmp = 0;
    
    a->env = 0;
    
    tAttackDetection_setAttack(ad, atk);
    tAttackDetection_setRelease(ad, rel);
}

static void atkdtk_envelope(tAttackDetection* const ad, float *in)
{
    _tAttackDetection* a = *ad;
    
    int i = 0;
    float tmp;
    for(i = 0; i < a->blocksize; ++i){
        tmp = fastabsf(in[i]);
        
        if(tmp > a->env)
            a->env = a->atk_coeff * (a->env - tmp) + tmp;
        else
            a->env = a->rel_coeff * (a->env - tmp) + tmp;
    }
    
}

//===========================================================================
// SNAC
//===========================================================================
/******************************************************************************/
/***************************** private procedures *****************************/
/******************************************************************************/

#define REALFFT mayer_realfft
#define REALIFFT mayer_realifft

static void snac_analyzeframe(tSNAC* const s);
static void snac_autocorrelation(tSNAC* const s);
static void snac_normalize(tSNAC* const s);
static void snac_pickpeak(tSNAC* const s);
static void snac_periodandfidelity(tSNAC* const s);
static void snac_biasbuf(tSNAC* const s);
static float snac_spectralpeak(tSNAC* const s, float periodlength);


/******************************************************************************/
/******************************** constructor, destructor *********************/
/******************************************************************************/


void tSNAC_init(tSNAC* const snac, int overlaparg, LEAF* const leaf)
{
    tSNAC_initToPool(snac, overlaparg, &leaf->mempool);
}

void    tSNAC_initToPool    (tSNAC* const snac, int overlaparg, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSNAC* s = *snac = (_tSNAC*) mpool_alloc(sizeof(_tSNAC), m);
    s->mempool = m;
    
    s->biasfactor = DEFBIAS;
    s->timeindex = 0;
    s->periodindex = 0;
    s->periodlength = 0.;
    s->fidelity = 0.;
    s->minrms = DEFMINRMS;
    s->framesize = SNAC_FRAME_SIZE;

    s->inputbuf = (float*) mpool_calloc(sizeof(float) * SNAC_FRAME_SIZE, m);
    s->processbuf = (float*) mpool_calloc(sizeof(float) * (SNAC_FRAME_SIZE * 2), m);
    s->spectrumbuf = (float*) mpool_calloc(sizeof(float) * (SNAC_FRAME_SIZE / 2), m);
    s->biasbuf = (float*) mpool_calloc(sizeof(float) * SNAC_FRAME_SIZE, m);
    
    snac_biasbuf(snac);
    tSNAC_setOverlap(snac, overlaparg);
}

void tSNAC_free (tSNAC* const snac)
{
    _tSNAC* s = *snac;
    
    mpool_free((char*)s->inputbuf, s->mempool);
    mpool_free((char*)s->processbuf, s->mempool);
    mpool_free((char*)s->spectrumbuf, s->mempool);
    mpool_free((char*)s->biasbuf, s->mempool);
    mpool_free((char*)s, s->mempool);
}

/******************************************************************************/
/************************** public access functions****************************/
/******************************************************************************/


void tSNAC_ioSamples(tSNAC* const snac, float *in, float *out, int size)
{
    _tSNAC* s = *snac;
    
    int timeindex = s->timeindex;
    int mask = s->framesize - 1;
    int outindex = 0;
    float *inputbuf = s->inputbuf;
    float *processbuf = s->processbuf;
    
    // call analysis function when it is time
    if(!(timeindex & (s->framesize / s->overlap - 1))) snac_analyzeframe(snac);
    
    while(size--)
    {
        inputbuf[timeindex] = *in++;
        out[outindex++] = processbuf[timeindex++];
        timeindex &= mask;
    }
    s->timeindex = timeindex;
}

void tSNAC_setOverlap(tSNAC* const snac, int lap)
{
    _tSNAC* s = *snac;
    if(!((lap==1)|(lap==2)|(lap==4)|(lap==8))) lap = DEFOVERLAP;
    s->overlap = lap;
}


void tSNAC_setBias(tSNAC* const snac, float bias)
{
    _tSNAC* s = *snac;
    if(bias > 1.) bias = 1.;
    if(bias < 0.) bias = 0.;
    s->biasfactor = bias;
    snac_biasbuf(snac);
    return;
}


void tSNAC_setMinRMS(tSNAC* const snac, float rms)
{
    _tSNAC* s = *snac;
    if(rms > 1.) rms = 1.;
    if(rms < 0.) rms = 0.;
    s->minrms = rms;
    return;
}


float tSNAC_getPeriod(tSNAC* const snac)
{
    _tSNAC* s = *snac;
    return(s->periodlength);
}


float tSNAC_getFidelity(tSNAC* const snac)
{
    _tSNAC* s = *snac;
    return(s->fidelity);
}


/******************************************************************************/
/***************************** private procedures *****************************/
/******************************************************************************/


// main analysis function
static void snac_analyzeframe(tSNAC* const snac)
{
    _tSNAC* s = *snac;
    
    int n, tindex = s->timeindex;
    int framesize = s->framesize;
    int mask = framesize - 1;
    float norm = 1.f / sqrtf((float)(framesize * 2));
    
    float *inputbuf = s->inputbuf;
    float *processbuf = s->processbuf;
    
    // copy input to processing buffers
    for(n=0; n<framesize; n++)
    {
        processbuf[n] = inputbuf[tindex] * norm;
        tindex++;
        tindex &= mask;
    }
    
    // zeropadding
    for(n=framesize; n<(framesize<<1); n++) processbuf[n] = 0.;
    
    // call analysis procedures
    snac_autocorrelation(snac);
    snac_normalize(snac);
    snac_pickpeak(snac);
    snac_periodandfidelity(snac);
}


static void snac_autocorrelation(tSNAC* const snac)
{
    _tSNAC* s = *snac;
    
    int n, m;
    int framesize = s->framesize;
    int fftsize = framesize * 2;
    float *processbuf = s->processbuf;
    float *spectrumbuf = s->spectrumbuf;
    
    REALFFT(fftsize, processbuf);
    
    // compute power spectrum
    processbuf[0] *= processbuf[0];                      // DC
    processbuf[framesize] *= processbuf[framesize];      // Nyquist
    
    for(n=1; n<framesize; n++)
    {
        processbuf[n] = processbuf[n] * processbuf[n]
        + processbuf[fftsize-n] * processbuf[fftsize-n]; // imag coefficients appear reversed
        processbuf[fftsize-n] = 0.f;
    }
    
    // store power spectrum up to SR/4 for possible later use
    for(m=0; m<(framesize>>1); m++)
    {
        spectrumbuf[m] = processbuf[m];
    }
    
    // transform power spectrum to autocorrelation function
    REALIFFT(fftsize, processbuf);
    return;
}


static void snac_normalize(tSNAC* const snac)
{
    _tSNAC* s = *snac;
    
    int framesize = s->framesize;
    int framesizeplustimeindex = s->framesize + s->timeindex;
    int timeindexminusone = s->timeindex - 1;
    int n, m;
    int mask = framesize - 1;
    int seek = framesize * SEEK;
    float *inputbuf = s->inputbuf;
    float *processbuf= s->processbuf;
    float signal1, signal2;
    
    // minimum RMS implemented as minimum autocorrelation at index 0
    // functionally equivalent to white noise floor
    float rms = s->minrms / sqrtf(1.0f / (float)framesize);
    float minrzero = rms * rms;
    float rzero = processbuf[0];
    if(rzero < minrzero) rzero = minrzero;
    double normintegral = (double)rzero * 2.;
    
    // normalize biased autocorrelation function
    // inputbuf is circular buffer: timeindex may be non-zero when overlap > 1
    processbuf[0] = 1;
    for(n=1, m=s->timeindex+1; n<seek; n++, m++)
    {
        signal1 = inputbuf[(n + timeindexminusone)&mask];
        signal2 = inputbuf[(framesizeplustimeindex - n)&mask]; //could this be switched to float resolution without issue? -JS
        normintegral -= (double)(signal1 * signal1 + signal2 * signal2);
        processbuf[n] /= (float)normintegral * 0.5f;
    }
    
    // flush instable function tail
    for(n = seek; n<framesize; n++) processbuf[n] = 0.;
    return;
}


static void snac_periodandfidelity(tSNAC* const snac)
{
    _tSNAC* s = *snac;
    
    float periodlength;
    
    if(s->periodindex)
    {
        periodlength = (float)s->periodindex +
        interpolate3phase(s->processbuf, s->periodindex);
        if(periodlength < 8) periodlength = snac_spectralpeak(snac, periodlength);
        s->periodlength = periodlength;
        s->fidelity = interpolate3max(s->processbuf, s->periodindex);
    }
    return;
}

// select the peak which most probably represents period length
static void snac_pickpeak(tSNAC* const snac)
{
    _tSNAC* s = *snac;
    
    int n, peakindex=0;
    int seek = s->framesize * SEEK;
    float *processbuf= s->processbuf;
    float maxvalue = 0.;
    float biasedpeak;
    float *biasbuf = s->biasbuf;
    
    // skip main lobe
    for(n=1; n<seek; n++)
    {
        if(processbuf[n] < 0.) break;
    }
    
    // find interpolated / biased maximum in SNAC function
    // interpolation finds the 'real maximum'
    // biasing favours the first candidate
    for(; n<seek-1; n++)
    {
        if(processbuf[n] >= processbuf[n-1])
        {
            if(processbuf[n] > processbuf[n+1])     // we have a local peak
            {
                biasedpeak = interpolate3max(processbuf, n) * biasbuf[n];
                
                if(biasedpeak > maxvalue)
                {
                    maxvalue = biasedpeak;
                    peakindex = n;
                }
            }
        }
    }
    s->periodindex = peakindex;
    return;
}


// verify period length via frequency domain (up till SR/4)
// frequency domain is more precise than lag domain for period lengths < 8
// argument 'periodlength' is initial estimation from autocorrelation
static float snac_spectralpeak(tSNAC* const snac, float periodlength)
{
    _tSNAC* s = *snac;
    
    if(periodlength < 4.0f) return periodlength;
    
    float max = 0.;
    int n, startbin, stopbin, peakbin = 0;
    int spectrumsize = s->framesize>>1;
    float *spectrumbuf = s->spectrumbuf;
    float peaklocation = (float)(s->framesize * 2.0f) / periodlength;
    
    startbin = (int)(peaklocation * 0.8f + 0.5f);
    if(startbin < 1) startbin = 1;
    stopbin = (int)(peaklocation * 1.25f + 0.5f);
    if(stopbin >= spectrumsize - 1) stopbin = spectrumsize - 1;
    
    for(n=startbin; n<stopbin; n++)
    {
        if(spectrumbuf[n] >= spectrumbuf[n-1])
        {
            if(spectrumbuf[n] > spectrumbuf[n+1])
            {
                if(spectrumbuf[n] > max)
                {
                    max = spectrumbuf[n];
                    peakbin = n;
                }
            }
        }
    }
    
    // calculate amplitudes in peak region
    for(n=(peakbin-1); n<(peakbin+2); n++)
    {
        spectrumbuf[n] = sqrtf(spectrumbuf[n]);
    }
    
    peaklocation = (float)peakbin + interpolate3phase(spectrumbuf, peakbin);
    periodlength = (float)(s->framesize * 2.0f) / peaklocation;
    
    return periodlength;
}


// modified logarithmic bias function
static void snac_biasbuf(tSNAC* const snac)
{
    _tSNAC* s = *snac;
    
    int n;
    int maxperiod = (int)(s->framesize * (float)SEEK);
    float bias = s->biasfactor / logf((float)(maxperiod - 4));
    float *biasbuf = s->biasbuf;
    
    for(n=0; n<5; n++)    // periods < 5 samples can't be tracked
    {
        biasbuf[n] = 0.0f;
    }
    
    for(n=5; n<maxperiod; n++)
    {
        biasbuf[n] = 1.0f - (float)logf(n - 4.f) * bias;
    }
}

//===========================================================================
// PERIODDETECTION
//===========================================================================
void tPeriodDetection_init (tPeriodDetection* const pd, float* in, float* out, int bufSize, int frameSize, LEAF* const leaf)
{
    tPeriodDetection_initToPool(pd, in, out, bufSize, frameSize, &leaf->mempool);
}

void tPeriodDetection_initToPool (tPeriodDetection* const pd, float* in, float* out, int bufSize, int frameSize, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPeriodDetection* p = *pd = (_tPeriodDetection*) mpool_calloc(sizeof(_tPeriodDetection), m);
    p->mempool = m;
    LEAF* leaf = p->mempool->leaf;
    
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
    
    tEnvPD_initToPool(&p->env, p->windowSize, p->hopSize, p->frameSize, mp);
    
    tSNAC_initToPool(&p->snac, DEFOVERLAP, mp);

    p->history = 0.0f;
    p->alpha = 1.0f;
    p->tolerance = 1.0f;
    p->timeConstant = DEFTIMECONSTANT;
    p->radius = expf(-1000.0f * p->hopSize * leaf->invSampleRate / p->timeConstant);
    p->fidelityThreshold = 0.95f;
}

void tPeriodDetection_free (tPeriodDetection* const pd)
{
    _tPeriodDetection* p = *pd;
    
    tEnvPD_free(&p->env);
    tSNAC_free(&p->snac);
    mpool_free((char*)p, p->mempool);
}

float tPeriodDetection_tick (tPeriodDetection* pd, float sample)
{
    _tPeriodDetection* p = *pd;
    
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
        
        tEnvPD_processBlock(&p->env, &(p->inBuffer[i]));
        
        tSNAC_ioSamples(&p->snac, &(p->inBuffer[i]), &(p->outBuffer[i]), p->frameSize);
        float fidelity = tSNAC_getFidelity(&p->snac);
        // Fidelity threshold recommended by Katja Vetters is 0.95 for most instruments/voices http://www.katjaas.nl/helmholtz/helmholtz.html
        if (fidelity > p->fidelityThreshold)
        {
            p->period = tSNAC_getPeriod(&p->snac);
        }
        
        p->curBlock++;
        if (p->curBlock >= p->framesPerBuffer) p->curBlock = 0;
        p->lastBlock++;
        if (p->lastBlock >= p->framesPerBuffer) p->lastBlock = 0;
    }
    return p->period;
}

float tPeriodDetection_getPeriod(tPeriodDetection* pd)
{
    _tPeriodDetection* p = *pd;
    return p->period;
}

void tPeriodDetection_setHopSize(tPeriodDetection* pd, int hs)
{
    _tPeriodDetection* p = *pd;
    p->hopSize = hs;
}

void tPeriodDetection_setWindowSize(tPeriodDetection* pd, int ws)
{
    _tPeriodDetection* p = *pd;
    p->windowSize = ws;
}

void tPeriodDetection_setFidelityThreshold(tPeriodDetection* pd, float threshold)
{
    _tPeriodDetection* p = *pd;
    p->fidelityThreshold = threshold;
}

void tPeriodDetection_setAlpha            (tPeriodDetection* pd, float alpha)
{
    _tPeriodDetection* p = *pd;
    p->alpha = LEAF_clip(0.0f, alpha, 1.0f);
}

void tPeriodDetection_setTolerance        (tPeriodDetection* pd, float tolerance)
{
    _tPeriodDetection* p = *pd;
    if (tolerance < 0.0f) p->tolerance = 0.0f;
    else p->tolerance = tolerance;
}


void    tZeroCrossingInfo_init  (tZeroCrossingInfo* const zc, LEAF* const leaf)
{
    tZeroCrossingInfo_initToPool(zc, &leaf->mempool);
}

void    tZeroCrossingInfo_initToPool    (tZeroCrossingInfo* const zc, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tZeroCrossingInfo* z = *zc = (_tZeroCrossingInfo*) mpool_alloc(sizeof(_tZeroCrossingInfo), m);
    z->mempool = m;

    z->_leading_edge = INT_MIN;
    z->_trailing_edge = INT_MIN;
    z->_width = 0.0f;
}

void    tZeroCrossingInfo_free  (tZeroCrossingInfo* const zc)
{
    _tZeroCrossingInfo* z = *zc;
    
    mpool_free((char*)z, z->mempool);
}

void    tZeroCrossingInfo_updatePeak(tZeroCrossingInfo* const zc, float s, int pos)
{
    _tZeroCrossingInfo* z = *zc;
    
    z->_peak = fmaxf(s, z->_peak);
    if ((z->_width == 0.0f) && (s < (z->_peak * 0.3f)))
        z->_width = pos - z->_leading_edge;
}

int     tZeroCrossingInfo_period(tZeroCrossingInfo* const zc, tZeroCrossingInfo* const next)
{
    _tZeroCrossingInfo* z = *zc;
    _tZeroCrossingInfo* n = *next;
    
    return n->_leading_edge - z->_leading_edge;
}

float   tZeroCrossingInfo_fractionalPeriod(tZeroCrossingInfo* const zc, tZeroCrossingInfo* const next)
{
    _tZeroCrossingInfo* z = *zc;
    _tZeroCrossingInfo* n = *next;
    
    // Get the start edge
    float prev1 = z->_before_crossing;
    float curr1 = z->_after_crossing;
    float dy1 = curr1 - prev1;
    float dx1 = -prev1 / dy1;
    
    // Get the next edge
    float prev2 = n->_before_crossing;
    float curr2 = n->_after_crossing;
    float dy2 = curr2 - prev2;
    float dx2 = -prev2 / dy2;
    
    // Calculate the fractional period
    float result = n->_leading_edge - z->_leading_edge;
    return result + (dx2 - dx1);
}

int     tZeroCrossingInfo_getWidth(tZeroCrossingInfo* const zc)
{
    _tZeroCrossingInfo* z = *zc;
    
    return z->_width;
}

static inline void update_state(tZeroCrossingCollector* const zc, float s);
static inline void shift(tZeroCrossingCollector* const zc, int n);
static inline void reset(tZeroCrossingCollector* const zc);

void    tZeroCrossingCollector_init  (tZeroCrossingCollector* const zc, int windowSize, float hysteresis, LEAF* const leaf)
{
    tZeroCrossingCollector_initToPool(zc, windowSize, hysteresis, &leaf->mempool);
}

void    tZeroCrossingCollector_initToPool    (tZeroCrossingCollector* const zc, int windowSize, float hysteresis, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tZeroCrossingCollector* z = *zc = (_tZeroCrossingCollector*) mpool_alloc(sizeof(_tZeroCrossingCollector), m);
    z->mempool = m;
    
    z->_hysteresis = -dbtoa(hysteresis);
    int bits = CHAR_BIT * sizeof(unsigned int);
    z->_window_size = fmax(2, (windowSize + bits - 1) / bits) * bits;
    
    int size = z->_window_size / 2;
    
    // Ensure size is a power of 2
    z->_size = pow(2.0, ceil(log2((double)size)));
    z->_mask = z->_size - 1;


    z->_info = (tZeroCrossingInfo*) mpool_calloc(sizeof(tZeroCrossingInfo) * z->_size, m);
    for (unsigned i = 0; i < z->_size; i++)
        tZeroCrossingInfo_initToPool(&z->_info[i], mp);

    z->_pos = 0;
    
    z->_prev = 0.0f;
    z->_state = 0;
    z->_num_edges = 0;
    z->_frame = 0;
    z->_ready = 0;
    z->_peak_update = 0.0f;
    z->_peak = 0.0f;
}

void    tZeroCrossingCollector_free  (tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    for (unsigned i = 0; i < z->_size; i++)
        tZeroCrossingInfo_free(&z->_info[i]);
    mpool_free((char*)z->_info, z->mempool);
    mpool_free((char*)z, z->mempool);
}

int     tZeroCrossingCollector_tick(tZeroCrossingCollector* const zc, float s)
{
    _tZeroCrossingCollector* z = *zc;
    
    // Offset s by half of hysteresis, so that zero cross detection is
    // centered on the actual zero.
    s += z->_hysteresis * 0.5f;
    
    if (z->_num_edges >= (int)z->_size)
        reset(zc);
    
    if ((z->_frame == z->_window_size/2) && (z->_num_edges == 0))
        reset(zc);
    
    update_state(zc, s);
    
    if ((++z->_frame >= z->_window_size) && !z->_state)
    {
        // Remove half the size from _frame, so we can continue seamlessly
        z->_frame -= z->_window_size / 2;
        
        // We need at least two rising edges.
        if (z->_num_edges > 1)
            z->_ready = 1;
        else
            reset(zc);
    }
    
    return z->_state;
}

int     tZeroCrossingCollector_getState(tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    return z->_state;
}

tZeroCrossingInfo const tZeroCrossingCollector_getCrossing(tZeroCrossingCollector* const zc, int index)
{
    _tZeroCrossingCollector* z = *zc;
    
    int i = (z->_num_edges - 1) - index;
    return z->_info[(z->_pos + i) & z->_mask];
}

int     tZeroCrossingCollector_getNumEdges(tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    return z->_num_edges;
}

int     tZeroCrossingCollector_getCapacity(tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    return (int)z->_size;
}

int     tZeroCrossingCollector_getFrame(tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    return z->_frame;
}

int     tZeroCrossingCollector_getWindowSize(tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    return z->_window_size;
}

int     tZeroCrossingCollector_isReady(tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    return z->_ready;
}

float   tZeroCrossingCollector_getPeak(tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    return fmaxf(z->_peak, z->_peak_update);
}

int     tZeroCrossingCollector_isReset(tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    return z->_frame == 0;
}

void    tZeroCrossingCollector_setHysteresis(tZeroCrossingCollector* const zc, float hysteresis)
{
    _tZeroCrossingCollector* z = *zc;
    
    z->_hysteresis = -dbtoa(hysteresis);
}

static inline void update_state(tZeroCrossingCollector* const zc, float s)
{
    _tZeroCrossingCollector* z = *zc;
    
    if (z->_ready)
    {
        shift(zc, z->_window_size / 2);
        z->_ready = 0;
        z->_peak = z->_peak_update;
        z->_peak_update = 0.0f;
    }
    
    if (z->_num_edges >= (int)z->_size)
        reset(zc);
    
    if (s > 0.0f)
    {
        if (!z->_state)
        {
            --z->_pos;
            z->_pos &= z->_mask;
            tZeroCrossingInfo crossing = z->_info[z->_pos];
            crossing->_before_crossing = z->_prev;
            crossing->_after_crossing = s;
            crossing->_peak = s;
            crossing->_leading_edge = (int) z->_frame;
            crossing->_trailing_edge = INT_MIN;
            crossing->_width = 0.0f;
            ++z->_num_edges;
            z->_state = 1;
        }
        else
        {
            tZeroCrossingInfo_updatePeak(&z->_info[z->_pos], s, z->_frame);
        }
        if (s > z->_peak_update)
        {
            z->_peak_update = s;
        }
    }
    else if (z->_state && (s < z->_hysteresis))
    {
        z->_state = 0;
        z->_info[z->_pos]->_trailing_edge = z->_frame;
        if (z->_peak == 0.0f)
            z->_peak = z->_peak_update;
    }
    
    if (z->_frame > z->_window_size * 2)
        reset(zc);
    
    z->_prev = s;
}

static inline void shift(tZeroCrossingCollector* const zc, int n)
{
    _tZeroCrossingCollector* z = *zc;
    
    tZeroCrossingInfo crossing = z->_info[z->_pos];
    
    crossing->_leading_edge -= n;
    if (!z->_state)
        crossing->_trailing_edge -= n;
    int i = 1;
    for (; i != z->_num_edges; ++i)
    {
        int idx = (z->_pos + i) & z->_mask;
        z->_info[idx]->_leading_edge -= n;
        int edge = (z->_info[idx]->_trailing_edge -= n);
        if (edge < 0.0f)
            break;
    }
    z->_num_edges = i;
}

static inline void reset(tZeroCrossingCollector* const zc)
{
    _tZeroCrossingCollector* z = *zc;
    
    z->_num_edges = 0;
    z->_state = 0;
    z->_frame = 0;
}



void    tBitset_init    (tBitset* const bitset, int numBits, LEAF* const leaf)
{
    tBitset_initToPool(bitset, numBits, &leaf->mempool);
}

void    tBitset_initToPool  (tBitset* const bitset, int numBits, tMempool* const mempool)
{
    _tMempool* m = *mempool;
    _tBitset* b = *bitset = (_tBitset*) mpool_alloc(sizeof(_tBitset), m);
    b->mempool = m;
    
    // Size of the array value in bits
    b->_value_size = (CHAR_BIT * sizeof(unsigned int));
    
    // Size of the array needed to store numBits bits
    b->_size = (numBits + b->_value_size - 1) / b->_value_size;
    
    // Siz of the array in bits
    b->_bit_size = b->_size * b->_value_size;
    
    b->_bits = (unsigned int*) mpool_calloc(sizeof(unsigned int) * b->_size, m);
}

void    tBitset_free    (tBitset* const bitset)
{
    _tBitset* b = *bitset;
    
    mpool_free((char*) b->_bits, b->mempool);
    mpool_free((char*) b, b->mempool);
}

int     tBitset_get     (tBitset* const bitset, int index)
{
    _tBitset* b = *bitset;
    
    // Check we don't get past the storage
    if (index > b->_bit_size)
        return -1;
    
    unsigned int mask = 1 << (index % b->_value_size);
    return (b->_bits[index / b->_value_size] & mask) != 0;
}

unsigned int*   tBitset_getData   (tBitset* const bitset)
{
    _tBitset* b = *bitset;
    
    return b->_bits;
}

void     tBitset_set     (tBitset* const bitset, int index, unsigned int val)
{
    _tBitset* b = *bitset;
    
    if (index > b->_bit_size)
        return;
    
    unsigned int mask = 1 << (index % b->_value_size);
    int i = index / b->_value_size;
    b->_bits[i] ^= (-val ^ b->_bits[i]) & mask;
}

void     tBitset_setMultiple (tBitset* const bitset, int index, int n, unsigned int val)
{
    _tBitset* b = *bitset;
    
    // Check that the index (i) does not get past size
    if (index > b->_bit_size)
        return;
    
    // Check that the n does not get past the size
    if ((index+n) > b->_bit_size)
        n = b->_bit_size - index;
    
    // index is the bit index, i is the integer index
    int i = index / b->_value_size;
    
    // Do the first partial int
    int mod = index & (b->_value_size - 1);
    if (mod)
    {
        // mask off the high n bits we want to set
        mod = b->_value_size - mod;
        
        // Calculate the mask
        unsigned int mask = ~(UINT_MAX >> mod);
        
        // Adjust the mask if we're not going to reach the end of this int
        if (n < mod)
            mask &= (UINT_MAX >> (mod - n));
        
        if (val)
            b->_bits[i] |= mask;
        else
            b->_bits[i] &= ~mask;
        
        // Fast exit if we're done here!
        if (n < mod)
            return;
        
        n -= mod;
        ++i;
    }
    
    // Write full ints while we can - effectively doing value_size bits at a time
    if (n >= b->_value_size)
    {
        // Store a local value to work with
        unsigned int val_ = val ? UINT_MAX : 0;
        
        do
        {
            b->_bits[i++] = val_;
            n -= b->_value_size;
        }
        while (n >= b->_value_size);
    }
    
    // Now do the final partial int, if necessary
    if (n)
    {
        mod = n & (b->_value_size - 1);
        
        // Calculate the mask
        unsigned int mask = (1 << mod) - 1;
        
        if (val)
            b->_bits[i] |= mask;
        else
            b->_bits[i] &= ~mask;
    }
}

int     tBitset_getSize (tBitset* const bitset)
{
    _tBitset* b = *bitset;
    
    return b->_bit_size;
}

void    tBitset_clear   (tBitset* const bitset)
{
    _tBitset* b = *bitset;
    
    for (int i = 0; i < b->_size; ++i)
    {
        b->_bits[i] = 0;
    }
}

void    tBACF_init  (tBACF* const bacf, tBitset* const bitset, LEAF* const leaf)
{
    tBACF_initToPool(bacf, bitset, &leaf->mempool);
}

void    tBACF_initToPool    (tBACF* const bacf, tBitset* const bitset, tMempool* const mempool)
{
    _tMempool* m = *mempool;
    _tBACF* b = *bacf = (_tBACF*) mpool_alloc(sizeof(_tBACF), m);
    b->mempool = m;
    
    b->_bitset = *bitset;
    b->_mid_array = ((b->_bitset->_bit_size / b->_bitset->_value_size) / 2) - 1;
}

void    tBACF_free  (tBACF* const bacf)
{
    _tBACF* b = *bacf;
    
    mpool_free((char*) b, b->mempool);
}

int    tBACF_getCorrelation  (tBACF* const bacf, int pos)
{
    _tBACF* b = *bacf;
    
    int value_size = b->_bitset->_value_size;
    const int index = pos / value_size;
    const int shift = pos % value_size;
    
    const unsigned int* p1 = b->_bitset->_bits;
    const unsigned int* p2 = b->_bitset->_bits + index;
    int count = 0;
    
    if (shift == 0)
    {
        for (unsigned i = 0; i != b->_mid_array; ++i)
        {
            // built in compiler popcount functions should be faster but we want this to be portable
            // could try to add some define that call the correct function depending on compiler
            // or let the user pointer popcount() to whatever they want
            // something to look into...
#ifdef __GNUC__
            count += __builtin_popcount(*p1++ ^ *p2++);
#elif _MSC_VER
            count += __popcnt(*p1++ ^ *p2++);
#else
            count += popcount(*p1++ ^ *p2++);
#endif
        }
    }
    else
    {
        const int shift2 = value_size - shift;
        for (unsigned i = 0; i != b->_mid_array; ++i)
        {
            unsigned int v = *p2++ >> shift;
            v |= *p2 << shift2;
#ifdef __GNUC__
            count += __builtin_popcount(*p1++ ^ v++);
#elif _MSC_VER
            count += __popcnt(*p1++ ^ v++);
#else
            count += popcount(*p1++ ^ v++);
#endif
        }
    }
    return count;
}

void    tBACF_set  (tBACF* const bacf, tBitset* const bitset)
{
    _tBACF* b = *bacf;
    
    b->_bitset = *bitset;
    b->_mid_array = ((b->_bitset->_bit_size / b->_bitset->_value_size) / 2) - 1;
}

static inline void set_bitstream(tPeriodDetector* const detector);
static inline void autocorrelate(tPeriodDetector* const detector);

static inline void sub_collector_init(_sub_collector* collector, tZeroCrossingCollector* const crossings, float pdt, int range);
static inline float sub_collector_period_of(_sub_collector* collector, _auto_correlation_info info);
static inline void sub_collector_save(_sub_collector* collector, _auto_correlation_info info);
static inline int sub_collector_try_sub_harmonic(_sub_collector* collector, int harmonic, _auto_correlation_info info, float incoming_period);
static inline int sub_collector_process_harmonics(_sub_collector* collector, _auto_correlation_info info);
static inline void sub_collector_process(_sub_collector* collector, _auto_correlation_info info);
static inline void sub_collector_get(_sub_collector* collector, _auto_correlation_info info, _period_info* result);

void    tPeriodDetector_init    (tPeriodDetector* const detector, float lowestFreq, float highestFreq, float hysteresis, LEAF* const leaf)
{
    tPeriodDetector_initToPool(detector, lowestFreq, highestFreq, hysteresis, &leaf->mempool);
}

void    tPeriodDetector_initToPool  (tPeriodDetector* const detector, float lowestFreq, float highestFreq, float hysteresis, tMempool* const mempool)
{
    _tMempool* m = *mempool;
    _tPeriodDetector* p = *detector = (_tPeriodDetector*) mpool_alloc(sizeof(_tPeriodDetector), m);
    p->mempool = m;
    
    LEAF* leaf = p->mempool->leaf;
    
    tZeroCrossingCollector_initToPool(&p->_zc, (1.0f / lowestFreq) * leaf->sampleRate * 2.0f, hysteresis, mempool);
    p->_min_period = (1.0f / highestFreq) * leaf->sampleRate;
    p->_range = highestFreq / lowestFreq;
    
    int windowSize = tZeroCrossingCollector_getWindowSize(&p->_zc);
    tBitset_initToPool(&p->_bits, windowSize, mempool);
    p->_weight = 2.0f / windowSize;
    p->_mid_point = windowSize / 2;
    p->_periodicity_diff_threshold = p->_mid_point * PERIODICITY_DIFF_FACTOR;
    
    p->_predicted_period = -1.0f;
    p->_edge_mark = 0;
    p->_predict_edge = 0;
    p->_num_pulses = 0;
    p->_half_empty = 0;
    
    tBACF_initToPool(&p->_bacf, &p->_bits, mempool);
}

void    tPeriodDetector_free    (tPeriodDetector* const detector)
{
    _tPeriodDetector* p = *detector;
    
    tZeroCrossingCollector_free(&p->_zc);
    tBitset_free(&p->_bits);
    tBACF_free(&p->_bacf);
    
    mpool_free((char*) p, p->mempool);
}

int   tPeriodDetector_tick    (tPeriodDetector* const detector, float s)
{
    _tPeriodDetector* p = *detector;
    
    // Zero crossing
    int prev = tZeroCrossingCollector_getState(&p->_zc);
    int zc = tZeroCrossingCollector_tick(&p->_zc, s);
    
    if (!zc && prev != zc)
    {
        ++p->_edge_mark;
        p->_predicted_period = -1.0f;
    }
    
    if (tZeroCrossingCollector_isReset(&p->_zc))
    {
        p->_fundamental.period = -1.0f;
        p->_fundamental.periodicity = 0.0f;
    }
    
    if (tZeroCrossingCollector_isReady(&p->_zc))
    {
        set_bitstream(detector);
        autocorrelate(detector);
        return 1;
    }
    return 0;
}

float   tPeriodDetector_getPeriod   (tPeriodDetector* const detector)
{
    _tPeriodDetector* p = *detector;
    
    return p->_fundamental.period;
}

float   tPeriodDetector_getPeriodicity  (tPeriodDetector* const detector)
{
    _tPeriodDetector* p = *detector;
    
    return p->_fundamental.periodicity;
}

float   tPeriodDetector_harmonic    (tPeriodDetector* const detector, int harmonicIndex)
{
    _tPeriodDetector* p = *detector;
    
    if (harmonicIndex > 0)
    {
        if (harmonicIndex == 1)
            return p->_fundamental.periodicity;
        
        float target_period = p->_fundamental.period / (float) harmonicIndex;
        if (target_period >= p->_min_period && target_period < p->_mid_point)
        {
            int count = tBACF_getCorrelation(&p->_bacf, roundf(target_period));
            float periodicity = 1.0f - (count * p->_weight);
            return periodicity;
        }
    }
    return 0.0f;
}

float   tPeriodDetector_predictPeriod   (tPeriodDetector* const detector)
{
    _tPeriodDetector* p = *detector;
    
    if (p->_predicted_period == -1.0f && p->_edge_mark != p->_predict_edge)
    {
        p->_predict_edge = p->_edge_mark;
        int n = tZeroCrossingCollector_getNumEdges(&p->_zc);
        if (n > 1)
        {
            float threshold = tZeroCrossingCollector_getPeak(&p->_zc) * PULSE_THRESHOLD;
            for (int i = n - 1; i > 0; --i)
            {
                tZeroCrossingInfo edge2 = tZeroCrossingCollector_getCrossing(&p->_zc, i);
                if (edge2->_peak >= threshold)
                {
                    for (int j = i-1; j >= 0; --j)
                    {
                        tZeroCrossingInfo edge1 = tZeroCrossingCollector_getCrossing(&p->_zc, j);
                        if (edge1->_peak >= threshold)
                        {
                            float period = tZeroCrossingInfo_fractionalPeriod(&edge1, &edge2);
                            if (period > p->_min_period)
                                return (p->_predicted_period = period);
                        }
                    }
                    return p->_predicted_period = -1.0f;
                }
            }
        }
    }
    return p->_predicted_period;
}

int     tPeriodDetector_isReady (tPeriodDetector* const detector)
{
    _tPeriodDetector* p = *detector;
    
    return tZeroCrossingCollector_isReady(&p->_zc);
}

int     tPeriodDetector_isReset (tPeriodDetector* const detector)
{
    _tPeriodDetector* p = *detector;
    
    return tZeroCrossingCollector_isReset(&p->_zc);
}

void    tPeriodDetector_setHysteresis   (tPeriodDetector* const detector, float hysteresis)
{
    _tPeriodDetector* p = *detector;
    
    return tZeroCrossingCollector_setHysteresis(&p->_zc, hysteresis);
}

static inline void set_bitstream(tPeriodDetector* const detector)
{
    _tPeriodDetector* p = *detector;
    
    float threshold = tZeroCrossingCollector_getPeak(&p->_zc) * PULSE_THRESHOLD;
    unsigned int leading_edge = tZeroCrossingCollector_getWindowSize(&p->_zc);
    unsigned int trailing_edge = 0;
    
    p->_num_pulses = 0;
    tBitset_clear(&p->_bits);
    
    for (int i = 0; i != tZeroCrossingCollector_getNumEdges(&p->_zc); ++i)
    {
        tZeroCrossingInfo info = tZeroCrossingCollector_getCrossing(&p->_zc, i);
        if (info->_peak >= threshold)
        {
            ++p->_num_pulses;
            if (info->_leading_edge < leading_edge)
                leading_edge = info->_leading_edge;
            if (info->_trailing_edge > trailing_edge)
                trailing_edge = info->_trailing_edge;
            int pos = fmax(info->_leading_edge, 0);
            int n = info->_trailing_edge - pos;
            tBitset_setMultiple(&p->_bits, pos, n, 1);
        }
    }
    p->_half_empty = (leading_edge > p->_mid_point) || (trailing_edge < p->_mid_point);
}

static inline void autocorrelate(tPeriodDetector* const detector)
{
    _tPeriodDetector* p = *detector;
    
    float threshold = tZeroCrossingCollector_getPeak(&p->_zc) * PULSE_THRESHOLD;
    
    _sub_collector collect;
    sub_collector_init(&collect, &p->_zc, p->_periodicity_diff_threshold, p->_range);
    
    if (p->_half_empty || p->_num_pulses < 2)
    {
        p->_fundamental.periodicity = -1.0f;
        return;
    }
    else
    {
        int shouldBreak = 0;
        int n = tZeroCrossingCollector_getNumEdges(&p->_zc);
        for (int i = 0; i != n - 1; ++i)
        {
            tZeroCrossingInfo curr = tZeroCrossingCollector_getCrossing(&p->_zc, i);
            if (curr->_peak >= threshold)
            {
                for (int j = i + 1; j != n; ++j)
                {
                    tZeroCrossingInfo next = tZeroCrossingCollector_getCrossing(&p->_zc, j);
                    if (next->_peak >= threshold)
                    {
                        int period = tZeroCrossingInfo_period(&curr, &next);
                        if (period > p->_mid_point)
                            break;
                        if (period >= p->_min_period)
                        {
                            
                            int count = tBACF_getCorrelation(&p->_bacf, period);
                            
                            int mid = p->_bacf->_mid_array * CHAR_BIT * sizeof(unsigned int);
                            
                            int start = period;
                            
                            if ((collect._fundamental._period == -1.0f) && count == 0)
                            {
                                if (tBACF_getCorrelation(&p->_bacf, period / 2.0f) == 0)
                                    count = -1;
                            }
                            else if (period < 32) // Search minimum if the resolution is low
                            {
                                // Search upwards for the minimum autocorrelation count
                                for (int d = start + 1; d < mid; ++d)
                                {
                                    int c = tBACF_getCorrelation(&p->_bacf, d);
                                    if (c > count)
                                        break;
                                    count = c;
                                    period = d;
                                }
                                // Search downwards for the minimum autocorrelation count
                                for (int d = start - 1; d > p->_min_period; --d)
                                {
                                    int c = tBACF_getCorrelation(&p->_bacf, d);
                                    if (c > count)
                                        break;
                                    count = c;
                                    period = d;
                                }
                            }
                            
                            if (count == -1)
                            {
                                shouldBreak = 1;
                                break; // Return early if we have false correlation
                            }
                            float periodicity = 1.0f - (count * p->_weight);
                            _auto_correlation_info info = { i, j, (int) period, periodicity };
                            sub_collector_process(&collect, info);
                            if (count == 0)
                            {
                                shouldBreak = 1;
                                break; // Return early if we have perfect correlation
                            }
                        }
                    }
                }
            }
            if (shouldBreak > 0) break;
        }
    }
    
    // Get the final resuts
    sub_collector_get(&collect, collect._fundamental, &p->_fundamental);
}

static inline void sub_collector_init(_sub_collector* collector, tZeroCrossingCollector* const crossings, float pdt, int range)
{
    collector->_zc = *crossings;
    collector->_harmonic_threshold = HARMONIC_PERIODICITY_FACTOR * 2.0f / (float)collector->_zc->_window_size;
    collector->_periodicity_diff_threshold = pdt;
    collector->_range = range;
    collector->_fundamental._i1 = -1;
    collector->_fundamental._i2 = -1;
    collector->_fundamental._period = -1;
    collector->_fundamental._periodicity = 0.0f;
    collector->_fundamental._harmonic = 0;
}

static inline float sub_collector_period_of(_sub_collector* collector, _auto_correlation_info info)
{
    tZeroCrossingInfo first = tZeroCrossingCollector_getCrossing(&collector->_zc, info._i1);
    tZeroCrossingInfo next = tZeroCrossingCollector_getCrossing(&collector->_zc, info._i2);
    return tZeroCrossingInfo_fractionalPeriod(&first, &next);
}

static inline void sub_collector_save(_sub_collector* collector, _auto_correlation_info info)
{
    collector->_fundamental = info;
    collector->_fundamental._harmonic = 1;
    collector->_first_period = sub_collector_period_of(collector, collector->_fundamental);
}

static inline int sub_collector_try_sub_harmonic(_sub_collector* collector, int harmonic, _auto_correlation_info info, float incoming_period)
{
    if (fabsf(incoming_period - collector->_first_period) < collector->_periodicity_diff_threshold)
    {
        // If incoming is a different harmonic and has better
        // periodicity ...
        if (info._periodicity > collector->_fundamental._periodicity &&
            harmonic != collector->_fundamental._harmonic)
        {
            float periodicity_diff = fabsf(info._periodicity - collector->_fundamental._periodicity);
            
            // If incoming periodicity is within the harmonic
            // periodicity threshold, then replace _fundamental with
            // incoming. Take note of the harmonic for later.
            if (periodicity_diff <= collector->_harmonic_threshold)
            {
                collector->_fundamental._i1 = info._i1;
                collector->_fundamental._i2 = info._i2;
                collector->_fundamental._periodicity = info._periodicity;
                collector->_fundamental._harmonic = harmonic;
            }
            
            // If not, then we save incoming (replacing the current
            // _fundamental).
            else
            {
                sub_collector_save(collector, info);
            }
        }
        return 1;
    }
    return 0;
}

static inline int sub_collector_process_harmonics(_sub_collector* collector, _auto_correlation_info info)
{
    if (info._period < collector->_first_period)
        return 0;
     
    float incoming_period = sub_collector_period_of(collector, info);
    int multiple = fmaxf(1.0f, roundf( incoming_period / collector->_first_period));
    return sub_collector_try_sub_harmonic(collector, fmin(collector->_range, multiple), info, incoming_period/multiple);
}

static inline void sub_collector_process(_sub_collector* collector, _auto_correlation_info info)
{
    if (collector->_fundamental._period == -1.0f)
        sub_collector_save(collector, info);
    
    else if (sub_collector_process_harmonics(collector, info))
        return;
    
    else if (info._periodicity > collector->_fundamental._periodicity)
        sub_collector_save(collector, info);
}

static inline void sub_collector_get(_sub_collector* collector, _auto_correlation_info info, _period_info* result)
{
    if (info._period != -1.0f)
    {
        result->period = sub_collector_period_of(collector, info) / info._harmonic;
        result->periodicity = info._periodicity;
    }
    else
    {
        result->period = -1.0f;
        result->period = 0.0f;
    }
}

static inline float calculate_frequency(tPitchDetector* const detector);
static inline void bias(tPitchDetector* const detector, _pitch_info incoming);

void    tPitchDetector_init (tPitchDetector* const detector, float lowestFreq, float highestFreq, LEAF* const leaf)
{
    tPitchDetector_initToPool(detector, lowestFreq, highestFreq, &leaf->mempool);
}

void    tPitchDetector_initToPool   (tPitchDetector* const detector, float lowestFreq, float highestFreq, tMempool* const mempool)
{
    _tMempool* m = *mempool;
    _tPitchDetector* p = *detector = (_tPitchDetector*) mpool_alloc(sizeof(_tPitchDetector), m);
    p->mempool = m;
    
    tPeriodDetector_initToPool(&p->_pd, lowestFreq, highestFreq, DEFAULT_HYSTERESIS, mempool);
    p->_current.frequency = 0.0f;
    p->_current.periodicity = 0.0f;
    p->_frames_after_shift = 0;
}

void    tPitchDetector_free (tPitchDetector* const detector)
{
    _tPitchDetector* p = *detector;
    
    tPeriodDetector_free(&p->_pd);
    mpool_free((char*) p, p->mempool);
}

int     tPitchDetector_tick    (tPitchDetector* const detector, float s)
{
    _tPitchDetector* p = *detector;
    tPeriodDetector_tick(&p->_pd, s);
    
    if (tPeriodDetector_isReset(&p->_pd))
    {
        p->_current.frequency = 0.0f;
        p->_current.periodicity = 0.0f;
    }
    
    int ready = tPeriodDetector_isReady(&p->_pd);
    if (ready)
    {
        float periodicity = p->_pd->_fundamental.periodicity;
        
        if (periodicity == -1.0f)
        {
            p->_current.frequency = 0.0f;
            p->_current.periodicity = 0.0f;
            return 0;
        }
        
        if (p->_current.frequency == 0.0f)
        {
            if (periodicity >= ONSET_PERIODICITY)
            {
                float f = calculate_frequency(detector);
                if (f > 0.0f)
                {
                    p->_current.frequency = f;
                    p->_current.periodicity = periodicity;
                    p->_frames_after_shift = 0;
                }
            }
        }
        else
        {
            if (periodicity < MIN_PERIODICITY)
                p->_frames_after_shift = 0;
            float f = calculate_frequency(detector);
            if (f > 0.0f)
            {
                _pitch_info info = { f, periodicity };
                bias(detector, info);
            }
        }
    }
    return ready;
}

float   tPitchDetector_getFrequency    (tPitchDetector* const detector)
{
    _tPitchDetector* p = *detector;
    
    return p->_current.frequency;
}

float   tPitchDetector_getPeriodicity  (tPitchDetector* const detector)
{
    _tPitchDetector* p = *detector;
    
    return p->_current.periodicity;
}

float   tPitchDetector_harmonic (tPitchDetector* const detector, int harmonicIndex)
{
    _tPitchDetector* p = *detector;
    
    return tPeriodDetector_harmonic(&p->_pd, harmonicIndex);
}

float   tPitchDetector_predictFrequency (tPitchDetector* const detector)
{
    _tPitchDetector* p = *detector;
    LEAF* leaf = p->mempool->leaf;
    
    float period = tPeriodDetector_predictPeriod(&p->_pd);
    if (period > 0.0f)
       return leaf->sampleRate / period;
   return 0.0f;
}

int     tPitchDetector_indeterminate    (tPitchDetector* const detector)
{
    _tPitchDetector* p = *detector;
    
    return p->_current.frequency == 0.0f;
}

void    tPitchDetector_setHysteresis    (tPitchDetector* const detector, float hysteresis)
{
    _tPitchDetector* p = *detector;
    
    tPeriodDetector_setHysteresis(&p->_pd, hysteresis);
}

static inline float calculate_frequency(tPitchDetector* const detector)
{
    _tPitchDetector* p = *detector;
    LEAF* leaf = p->mempool->leaf;
    
    float period = p->_pd->_fundamental.period;
    if (period > 0.0f)
        return leaf->sampleRate / period;
    return 0.0f;
}

static inline void bias(tPitchDetector* const detector, _pitch_info incoming)
{
    _tPitchDetector* p = *detector;
    
    ++p->_frames_after_shift;
    int shifted = 0;
    
    _pitch_info result;
    
    //=============================================================================
    //_pitch_info result = bias(current, incoming, shift);
    {
        float error = p->_current.frequency / 32.0f; // approx 1/2 semitone
        float diff = fabsf(p->_current.frequency - incoming.frequency);
        int done = 0;
        
        // Try fundamental
        if (diff < error)
        {
            result = incoming;
            done = 1;
        }
        // Try harmonics and sub-harmonics
        else if (p->_frames_after_shift > 1)
        {
            if (p->_current.frequency > incoming.frequency)
            {
                int multiple = roundf(p->_current.frequency / incoming.frequency);
                if (multiple > 1)
                {
                    float f = incoming.frequency * multiple;
                    if (fabsf(p->_current.frequency - f) < error)
                    {
                        result.frequency = f;
                        result.periodicity = incoming.periodicity;
                        done = 1;
                    }
                }
            }
            else
            {
                int multiple = roundf(incoming.frequency / p->_current.frequency);
                if (multiple > 1)
                {
                    float f = incoming.frequency / multiple;
                    if (fabsf(p->_current.frequency - f) < error)
                    {
                        result.frequency = f;
                        result.periodicity = incoming.periodicity;
                        done = 1;
                    }
                }
            }
        }
        // Don't do anything if the latest autocorrelation is not periodic
        // enough. Note that we only do this check on frequency shifts (i.e. at
        // this point, we are looking at a potential frequency shift, after
        // passing through the code above, checking for fundamental and
        // harmonic matches).
        if (!done)
        {
            if (p->_pd->_fundamental.periodicity > MIN_PERIODICITY)
            {
                // Now we have a frequency shift
                shifted = 1;
                result = incoming;
            }
            else result = p->_current;
        }
    }
    
    //=============================================================================
    
    // Don't do anything if incoming is not periodic enough
    // Note that we only do this check on frequency shifts
    if (shifted)
    {
        float periodicity = p->_pd->_fundamental.periodicity;
        if (periodicity >= ONSET_PERIODICITY)
        {
            p->_frames_after_shift = 0;
            p->_current = result;
        }
        else if (periodicity < MIN_PERIODICITY)
        {
            p->_current.frequency = 0.0f;
            p->_current.periodicity = 0.0f;
        }
    }
    else
    {
        p->_current = result;
    }
}

static inline void compute_predicted_frequency(tDualPitchDetector* const detector);

void    tDualPitchDetector_init (tDualPitchDetector* const detector, float lowestFreq, float highestFreq, LEAF* const leaf)
{
    tDualPitchDetector_initToPool(detector, lowestFreq, highestFreq, &leaf->mempool);
}

void    tDualPitchDetector_initToPool   (tDualPitchDetector* const detector, float lowestFreq, float highestFreq, tMempool* const mempool)
{
    _tMempool* m = *mempool;
    _tDualPitchDetector* p = *detector = (_tDualPitchDetector*) mpool_alloc(sizeof(_tDualPitchDetector), m);
    p->mempool = m;
    
    tPitchDetector_initToPool(&p->_pd1, lowestFreq, highestFreq, mempool);
    tPitchDetector_initToPool(&p->_pd2, lowestFreq, highestFreq, mempool);
    p->_current.frequency = 0.0f;
    p->_current.periodicity = 0.0f;
    p->_mean = lowestFreq + ((highestFreq - lowestFreq) / 2.0f);
    p->_predicted_frequency = 0.0f;
    p->_first = 1;
}

void    tDualPitchDetector_free (tDualPitchDetector* const detector)
{
    _tDualPitchDetector* p = *detector;
    
    tPitchDetector_free(&p->_pd1);
    tPitchDetector_free(&p->_pd2);
    
    mpool_free((char*) p, p->mempool);
}

int     tDualPitchDetector_tick    (tDualPitchDetector* const detector, float sample)
{
    _tDualPitchDetector* p = *detector;
    
    int pd1_ready = tPitchDetector_tick(&p->_pd1, sample);
    int pd2_ready = tPitchDetector_tick(&p->_pd2, -sample);
    
    if (pd1_ready || pd2_ready)
    {
        int pd1_indeterminate = tPitchDetector_indeterminate(&p->_pd1);
        int pd2_indeterminate = tPitchDetector_indeterminate(&p->_pd2);
        if (!pd1_indeterminate && !pd2_indeterminate)
        {
            _pitch_info _i1 = p->_pd1->_current;
            _pitch_info _i2 = p->_pd2->_current;
            
            float pd1_diff = fabsf(_i1.frequency - p->_mean);
            float pd2_diff = fabsf(_i2.frequency - p->_mean);
            _pitch_info i = (pd1_diff < pd2_diff) ? _i1 : _i2;
            
            if (p->_first)
            {
                p->_current = i;
                p->_mean = p->_current.frequency;
                p->_first = 0;
                p->_predicted_frequency = 0.0f;
            }
            else
            {
                p->_current = i;
                p->_mean = (0.222222f * p->_current.frequency) + (0.777778f * p->_mean);
                p->_predicted_frequency = 0.0f;
            }
        }
        
        if (pd1_indeterminate && pd2_indeterminate)
        {
            compute_predicted_frequency(detector);
            p->_current.frequency = 0.0f;
            p->_current.periodicity = 0.0f;
        }
    }
    
    return pd1_ready || pd2_ready;
}

float   tDualPitchDetector_getFrequency    (tDualPitchDetector* const detector)
{
    _tDualPitchDetector* p = *detector;
    
    return p->_current.frequency;
}

float   tDualPitchDetector_getPeriodicity  (tDualPitchDetector* const detector)
{
    _tDualPitchDetector* p = *detector;
    
    return p->_current.periodicity;
}

float   tDualPitchDetector_predictFrequency (tDualPitchDetector* const detector)
{
    _tDualPitchDetector* p = *detector;
    
    if (p->_predicted_frequency == 0.0f)
        compute_predicted_frequency(detector);
    return p->_predicted_frequency;
}

void    tDualPitchDetector_setHysteresis    (tDualPitchDetector* const detector, float hysteresis)
{
    _tDualPitchDetector* p = *detector;
    
    tPitchDetector_setHysteresis(&p->_pd1, hysteresis);
    tPitchDetector_setHysteresis(&p->_pd2, hysteresis);
}

static inline void compute_predicted_frequency(tDualPitchDetector* const detector)
{
    _tDualPitchDetector* p = *detector;
    
    float f1 = tPitchDetector_predictFrequency(&p->_pd1);
    if (f1 > 0.0f)
    {
        float f2 = tPitchDetector_predictFrequency(&p->_pd2);
        if (f2 > 0.0f)
        {
            float error = f1 * 0.1f;
            if (fabsf(f1 - f2) < error)
            {
                p->_predicted_frequency = f1;
                return;
            }
        }
    }
    p->_predicted_frequency = 0.0f;
}
