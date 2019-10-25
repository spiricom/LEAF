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

void    tEnvelopeFollower_init(tEnvelopeFollower* const e, float attackThreshold, float decayCoeff)
{
    e->y = 0.0f;
    e->a_thresh = attackThreshold;
    e->d_coeff = decayCoeff;
}

void tEnvelopeFollower_free(tEnvelopeFollower* const e)
{
    
}

float   tEnvelopeFollower_tick(tEnvelopeFollower* const ef, float x)
{
    if (x < 0.0f ) x = -x;  /* Absolute value. */
    
    if ((x >= ef->y) && (x > ef->a_thresh)) ef->y = x;                      /* If we hit a peak, ride the peak to the top. */
    else                                    ef->y = ef->y * ef->d_coeff;    /* Else, exponential decay of output. */
    
    //ef->y = envelope_pow[(uint16_t)(ef->y * (float)UINT16_MAX)] * ef->d_coeff; //not quite the right behavior - too much loss of precision?
    //ef->y = powf(ef->y, 1.000009f) * ef->d_coeff;  // too expensive
    
    if( ef->y < VSF)   ef->y = 0.0f;
    
    return ef->y;
}

int     tEnvelopeFollower_decayCoeff(tEnvelopeFollower* const ef, float decayCoeff)
{
    return ef->d_coeff = decayCoeff;
}

int     tEnvelopeFollower_attackThresh(tEnvelopeFollower* const ef, float attackThresh)
{
    return ef->a_thresh = attackThresh;
}





//===========================================================================
/* Power Follower */
//===========================================================================
void    tPowerFollower_init(tPowerFollower* const p, float factor)
{
    p->curr=0.0f;
    p->factor=factor;
    p->oneminusfactor=1.0f-factor;
}

void tPowerFollower_free(tPowerFollower* const p)
{
    
}

int     tPowerFollower_setFactor(tPowerFollower* const p, float factor)
{
    if (factor<0) factor=0;
    if (factor>1) factor=1;
    p->factor=factor;
    p->oneminusfactor=1.0f-factor;
    return 0;
}

float   tPowerFollower_tick(tPowerFollower* const p, float input)
{
    p->curr = p->factor*input*input+p->oneminusfactor*p->curr;
    return p->curr;
}

float   tPowerFollower_sample(tPowerFollower* const p)
{
    return p->curr;
}




//===========================================================================
/* ---------------- env~ - simple envelope follower. ----------------- */
//===========================================================================

void tEnvPD_init(tEnvPD* const x, int ws, int hs, int bs)
{
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

void tEnvPD_free (tEnvPD* const x)
{
    
}

float tEnvPD_tick (tEnvPD* const x)
{
    return powtodb(x->x_result);
}

void tEnvPD_processBlock(tEnvPD* const x, float* in)
{
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

void tAttackDetection_init(tAttackDetection* const a, int blocksize)
{
    atkdtk_init(a, blocksize, DEFATTACK, DEFRELEASE);
}

void tAttackDetection_init_expanded(tAttackDetection* const a, int blocksize, int atk, int rel)
{
    atkdtk_init(a, blocksize, atk, rel);
}

void tAttackDetection_free(tAttackDetection *a)
{
    
}

/*******Public Functions***********/


void tAttackDetection_setBlocksize(tAttackDetection* const a, int size)
{
    
    if(!((size==64)|(size==128)|(size==256)|(size==512)|(size==1024)|(size==2048)))
        size = DEFBLOCKSIZE;
    a->blocksize = size;
    
    return;
    
}

void tAttackDetection_setSamplerate(tAttackDetection* const a, int inRate)
{
    a->samplerate = inRate;
    
    //Reset atk and rel to recalculate coeff
    tAttackDetection_setAtk(a, a->atk);
    tAttackDetection_setRel(a, a->rel);
    
    return;
}

void tAttackDetection_setThreshold(tAttackDetection* const a, float thres)
{
    a->threshold = thres;
    return;
}

void tAttackDetection_setAtk(tAttackDetection* const a, int inAtk)
{
    a->atk = inAtk;
    a->atk_coeff = pow(0.01, 1.0/(a->atk * a->samplerate * 0.001));
    
    return;
}

void tAttackDetection_setRel(tAttackDetection* const a, int inRel)
{
    a->rel = inRel;
    a->rel_coeff = pow(0.01, 1.0/(a->rel * a->samplerate * 0.001));
    
    return;
}


int tAttackDetection_detect(tAttackDetection* const a, float *in)
{
    int result;
    
    atkdtk_envelope(a, in);
    
    if(a->env >= a->prevAmp*2) //2 times greater = 6dB increase
        result = 1;
    else
        result = 0;
    
    a->prevAmp = a->env;
    
    return result;
    
}

/*******Private Functions**********/

static void atkdtk_init(tAttackDetection* const a, int blocksize, int atk, int rel)
{
    a->env = 0;
    a->blocksize = blocksize;
    a->threshold = DEFTHRESHOLD;
    a->samplerate = leaf.sampleRate;
    a->prevAmp = 0;
    
    a->env = 0;
    
    tAttackDetection_setAtk(a, atk);
    tAttackDetection_setRel(a, rel);
}

static void atkdtk_envelope(tAttackDetection* const a, float *in)
{
    int i = 0;
    float tmp;
    for(i = 0; i < a->blocksize; ++i){
        tmp = fastabs(in[i]);
        
        if(tmp > a->env)
            a->env = a->atk_coeff * (a->env - tmp) + tmp;
        else
            a->env = a->rel_coeff * (a->env - tmp) + tmp;
    }
    
}

//===========================================================================
// PERIODDETECTION
//===========================================================================
void    tPeriodDetection_init    (tPeriodDetection* const p, float* in, float* out, int bufSize, int frameSize)
{
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
    
    tEnvPD_init(&p->env, p->windowSize, p->hopSize, p->frameSize);
    
    tSNAC_init(&p->snac, DEFOVERLAP);
    
    p->timeConstant = DEFTIMECONSTANT;
    p->radius = expf(-1000.0f * p->hopSize * leaf.invSampleRate / p->timeConstant);
}

void tPeriodDetection_free (tPeriodDetection* const p)
{
    tEnvPD_free(&p->env);
    tSNAC_free(&p->snac);
}

float tPeriodDetection_findPeriod (tPeriodDetection* p, float sample)
{
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
        p->period = tSNAC_getPeriod(&p->snac);
        
        p->curBlock++;
        if (p->curBlock >= p->framesPerBuffer) p->curBlock = 0;
        p->lastBlock++;
        if (p->lastBlock >= p->framesPerBuffer) p->lastBlock = 0;
    }
    
    // changed from period to p->period
    return p->period;
}

void tPeriodDetection_setHopSize(tPeriodDetection* p, int hs)
{
    p->hopSize = hs;
}

void tPeriodDetection_setWindowSize(tPeriodDetection* p, int ws)
{
    p->windowSize = ws;
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


void tSNAC_init(tSNAC* const s, int overlaparg)
{
    s->biasfactor = DEFBIAS;
    s->timeindex = 0;
    s->periodindex = 0;
    s->periodlength = 0.;
    s->fidelity = 0.;
    s->minrms = DEFMINRMS;
    s->framesize = SNAC_FRAME_SIZE;
    
    s->inputbuf = (float*) leaf_alloc(sizeof(float) * SNAC_FRAME_SIZE);
    s->processbuf = (float*) leaf_alloc(sizeof(float) * (SNAC_FRAME_SIZE * 2));
    s->spectrumbuf = (float*) leaf_alloc(sizeof(float) * (SNAC_FRAME_SIZE / 2));
    s->biasbuf = (float*) leaf_alloc(sizeof(float) * SNAC_FRAME_SIZE);
    
    snac_biasbuf(s);
    tSNAC_setOverlap(s, overlaparg);
}

void tSNAC_free(tSNAC* const s)
{
    leaf_free(s->inputbuf);
    leaf_free(s->processbuf);
    leaf_free(s->spectrumbuf);
    leaf_free(s->biasbuf);
}
/******************************************************************************/
/************************** public access functions****************************/
/******************************************************************************/


void tSNAC_ioSamples(tSNAC* const s, float *in, float *out, int size)
{
    int timeindex = s->timeindex;
    int mask = s->framesize - 1;
    int outindex = 0;
    float *inputbuf = s->inputbuf;
    float *processbuf = s->processbuf;
    
    // call analysis function when it is time
    if(!(timeindex & (s->framesize / s->overlap - 1))) snac_analyzeframe(s);
    
    while(size--)
    {
        inputbuf[timeindex] = *in++;
        out[outindex++] = processbuf[timeindex++];
        timeindex &= mask;
    }
    s->timeindex = timeindex;
}

void tSNAC_setOverlap(tSNAC* const s, int lap)
{
    if(!((lap==1)|(lap==2)|(lap==4)|(lap==8))) lap = DEFOVERLAP;
    s->overlap = lap;
}


void tSNAC_setBias(tSNAC* const s, float bias)
{
    if(bias > 1.) bias = 1.;
    if(bias < 0.) bias = 0.;
    s->biasfactor = bias;
    snac_biasbuf(s);
    return;
}


void tSNAC_setMinRMS(tSNAC* const s, float rms)
{
    if(rms > 1.) rms = 1.;
    if(rms < 0.) rms = 0.;
    s->minrms = rms;
    return;
}


float tSNAC_getPeriod(tSNAC* const s)
{
    return(s->periodlength);
}


float tSNAC_getFidelity(tSNAC* const s)
{
    return(s->fidelity);
}


/******************************************************************************/
/***************************** private procedures *****************************/
/******************************************************************************/


// main analysis function
static void snac_analyzeframe(tSNAC* const s)
{
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
    snac_autocorrelation(s);
    snac_normalize(s);
    snac_pickpeak(s);
    snac_periodandfidelity(s);
}


static void snac_autocorrelation(tSNAC* const s)
{
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


static void snac_normalize(tSNAC* const s)
{
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


static void snac_periodandfidelity(tSNAC* const s)
{
    float periodlength;
    
    if(s->periodindex)
    {
        periodlength = (float)s->periodindex +
        interpolate3phase(s->processbuf, s->periodindex);
        if(periodlength < 8) periodlength = snac_spectralpeak(s, periodlength);
        s->periodlength = periodlength;
        s->fidelity = interpolate3max(s->processbuf, s->periodindex);
    }
    return;
}

// select the peak which most probably represents period length
static void snac_pickpeak(tSNAC* const s)
{
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
static float snac_spectralpeak(tSNAC* const s, float periodlength)
{
    if(periodlength < 4.) return periodlength;
    
    float max = 0.;
    int n, startbin, stopbin, peakbin = 0;
    int spectrumsize = s->framesize>>1;
    float *spectrumbuf = s->spectrumbuf;
    float peaklocation = (float)(s->framesize * 2.) / periodlength;
    
    startbin = (int)(peaklocation * 0.8 + 0.5);
    if(startbin < 1) startbin = 1;
    stopbin = (int)(peaklocation * 1.25 + 0.5);
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
        spectrumbuf[n] = sqrt(spectrumbuf[n]);
    }
    
    peaklocation = (float)peakbin + interpolate3phase(spectrumbuf, peakbin);
    periodlength = (float)(s->framesize * 2.0f) / peaklocation;
    
    return periodlength;
}


// modified logarithmic bias function
static void snac_biasbuf(tSNAC* const s)
{
    int n;
    int maxperiod = (int)(s->framesize * (float)SEEK);
    float bias = s->biasfactor / log((float)(maxperiod - 4));
    float *biasbuf = s->biasbuf;
    
    for(n=0; n<5; n++)    // periods < 5 samples can't be tracked
    {
        biasbuf[n] = 0.;
    }
    
    for(n=5; n<maxperiod; n++)
    {
        biasbuf[n] = 1.0f - (float)log(n - 4) * bias;
    }
}

