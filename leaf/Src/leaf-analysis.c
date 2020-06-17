/*==============================================================================

    leaf-analysis.c
    Created: 30 Nov 2018 11:56:49am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-analysis.h"
#include "..\Externals\d_fft_mayer.h"

#else

#include "../Inc/leaf-analysis.h"
#include "../Externals/d_fft_mayer.h"

#endif

//===========================================================================
/* Envelope Follower */
//===========================================================================

void    tEnvelopeFollower_init(tEnvelopeFollower* const ef, float attackThreshold, float decayCoeff)
{
    tEnvelopeFollower_initToPool(ef, attackThreshold, decayCoeff, &leaf.mempool);
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

int     tEnvelopeFollower_decayCoeff(tEnvelopeFollower* const ef, float decayCoeff)
{
    _tEnvelopeFollower* e = *ef;
    return e->d_coeff = decayCoeff;
}

int     tEnvelopeFollower_attackThresh(tEnvelopeFollower* const ef, float attackThresh)
{
    _tEnvelopeFollower* e = *ef;
    return e->a_thresh = attackThresh;
}





// zero crossing detector

void    tZeroCrossing_init         (tZeroCrossing* const zc, int maxWindowSize)
{
    tZeroCrossing_initToPool   (zc, maxWindowSize, &leaf.mempool);
}

void    tZeroCrossing_initToPool   (tZeroCrossing* const zc, int maxWindowSize, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tZeroCrossing* z = *zc = (_tZeroCrossing*) mpool_alloc(sizeof(_tZeroCrossing), m);
    z->mempool = m;

    z->count = 0;
    z->maxWindowSize = maxWindowSize;
    z->currentWindowSize = maxWindowSize;
    z->invCurrentWindowSize = 1.0f / maxWindowSize;
    z->position = 0;
    z->prevPosition = maxWindowSize;
    z->inBuffer = (float*) mpool_calloc(sizeof(float) * maxWindowSize, m);
    z->countBuffer = (uint16_t*) mpool_calloc(sizeof(uint16_t) * maxWindowSize, m);
}

void    tZeroCrossing_free (tZeroCrossing* const zc)
{
    _tZeroCrossing* z = *zc;
    
    mpool_free((char*)z->inBuffer, z->mempool);
    mpool_free((char*)z->countBuffer, z->mempool);
    mpool_free((char*)z, z->mempool);
}

//returns proportion of zero crossings within window size (0.0 would be none in window, 1.0 would be all zero crossings)
float   tZeroCrossing_tick         (tZeroCrossing* const zc, float input)
{
    _tZeroCrossing* z = *zc;

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


void    tZeroCrossing_setWindow        (tZeroCrossing* const zc, float windowSize)
{
    _tZeroCrossing* z = *zc;
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
void    tPowerFollower_init(tPowerFollower* const pf, float factor)
{
    tPowerFollower_initToPool(pf, factor, &leaf.mempool);
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

void tEnvPD_init(tEnvPD* const xpd, int ws, int hs, int bs)
{
    tEnvPD_initToPool(xpd, ws, hs, bs, &leaf.mempool);
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

void tAttackDetection_init(tAttackDetection* const ad, int blocksize, int atk, int rel)
{
    tAttackDetection_initToPool(ad, blocksize, atk, rel, &leaf.mempool);
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
    
    if(!((size==64)|(size==128)|(size==256)|(size==512)|(size==1024)|(size==2048)))
        size = DEFBLOCKSIZE;
    a->blocksize = size;
    
    return;
    
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
    a->atk_coeff = pow(0.01, 1.0/(a->atk * a->samplerate * 0.001));
}

void tAttackDetection_setRelease(tAttackDetection* const ad, int inRel)
{
    _tAttackDetection* a = *ad;
    a->rel = inRel;
    a->rel_coeff = pow(0.01, 1.0/(a->rel * a->samplerate * 0.001));
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
    
    a->env = 0;
    a->blocksize = blocksize;
    a->threshold = DEFTHRESHOLD;
    a->samplerate = leaf.sampleRate;
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


void tSNAC_init(tSNAC* const snac, int overlaparg)
{
    tSNAC_initToPool(snac, overlaparg, &leaf.mempool);
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
    float norm = 1. / sqrt((float)(framesize * 2));
    
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
        processbuf[fftsize-n] = 0.;
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
    float rms = s->minrms / sqrt(1.0f / (float)framesize);
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
        signal2 = inputbuf[(framesizeplustimeindex - n)&mask];
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
        biasbuf[n] = 1.0f - (float)logf(n - 4) * bias;
    }
}

//===========================================================================
// PERIODDETECTION
//===========================================================================
void tPeriodDetection_init (tPeriodDetection* const pd, float* in, float* out, int bufSize, int frameSize)
{
    tPeriodDetection_initToPool(pd, in, out, bufSize, frameSize, &leaf.mempool);
}

void tPeriodDetection_initToPool (tPeriodDetection* const pd, float* in, float* out, int bufSize, int frameSize, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPeriodDetection* p = *pd = (_tPeriodDetection*) mpool_calloc(sizeof(_tPeriodDetection), m);
    p->mempool = m;
    
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
    p->radius = expf(-1000.0f * p->hopSize * leaf.invSampleRate / p->timeConstant);
    p->fidelityThreshold = 0.95;
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
