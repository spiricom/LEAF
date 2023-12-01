/*==============================================================================
 
 leaf-math.c
 Created: 22 Jan 2017 7:02:56pm
 
 ==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-math.h"
#include "..\Inc\leaf-tables.h"

#else

#include "../Inc/leaf-math.h"
#include "../Inc/leaf-tables.h"

#endif


#define EXPONENTIAL_TABLE_SIZE 65536

#define log10f_fast(x)  (log2f_approx(x)*0.3010299956639812f)

// This is a fast approximation to log2() found on http://openaudio.blogspot.com/2017/02/faster-log10-and-pow.html credited to this post https://community.arm.com/developer/tools-software/tools/f/armds-forum/4292/cmsis-dsp-new-functionality-proposal/22621#22621
// Y = C[0]*F*F*F + C[1]*F*F + C[2]*F + C[3] + E;
Lfloat log2f_approx(Lfloat X) {
    Lfloat Y, F;
    int E;
    F = frexpf(fabsf(X), &E);
    Y = 1.23149591368684f;
    Y *= F;
    Y += -4.11852516267426f;
    Y *= F;
    Y += 6.02197014179219f;
    Y *= F;
    Y += -3.13396450166353f;
    Y += E;
    return(Y);
}

//another log2Approximation
Lfloat log2f_approx2(Lfloat x)
{
    return (0.1640425613334452f * x*x*x) + (-1.098865286222744f * x*x) + (3.148297929334117f * x) + -2.213475204444817f;
}

Lfloat interpolate3max(Lfloat *buf, const int peakindex)
{
    Lfloat a = buf[peakindex-1];
    Lfloat b = buf[peakindex];
    Lfloat c = buf[peakindex+1];
    Lfloat realpeak;
    
    realpeak = b + (Lfloat)0.125 * (c - a) * (c - a) / ((Lfloat)2. * b - a - c);
    
    return(realpeak);
}

Lfloat interpolate3phase(Lfloat *buf, const int peakindex)
{
    Lfloat a = buf[peakindex-1];
    Lfloat b = buf[peakindex];
    Lfloat c = buf[peakindex+1];
    Lfloat fraction;
    
    fraction = ((Lfloat)0.5 * (c - a)) / ((Lfloat)2. * b - a - c);
    
    return(fraction);
}

float LEAF_map(float value, float istart, float istop, float ostart, float ostop)
{
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}
// from http://www.wild-magic.com - found on music-dsp list
Lfloat fastcosf(Lfloat fAngle)
{
    Lfloat fASqr = fAngle*fAngle;
    Lfloat fResult = -2.605e-07f;
    fResult *= fASqr;
    fResult += 2.47609e-05f;
    fResult *= fASqr;
    fResult -= 1.3888397e-03f;
    fResult *= fASqr;
    fResult += 4.16666418e-02f;
    fResult *= fASqr;
    fResult -= 4.999999963e-01f;
    fResult *= fASqr;
    fResult += 1.0f;
    return fResult;
}

Lfloat fastercosf(Lfloat fAngle)
{
    Lfloat fASqr = fAngle*fAngle;
    Lfloat fResult = 3.705e-02f;
    fResult *= fASqr;
    fResult -= 4.967e-01f;
    fResult *= fASqr;
    fResult += 1.0f;
    return fResult;
}

Lfloat fasttanf (Lfloat fAngle)
{
    Lfloat fASqr = fAngle*fAngle;
    Lfloat fResult = 9.5168091e-03f;
    fResult *= fASqr;
    fResult += 2.900525e-03f;
    fResult *= fASqr;
    fResult += 2.45650893e-02f;
    fResult *= fASqr;
    fResult += 5.33740603e-02f;
    fResult *= fASqr;
    fResult += 1.333923995e-01f;
    fResult *= fASqr;
    fResult += 3.333314036e-01f;
    fResult *= fASqr;
    fResult += 1.0f;
    fResult *= fAngle;
    return fResult;
}

Lfloat fastertanf(Lfloat fAngle)
{
    Lfloat fASqr = fAngle*fAngle;
    Lfloat fResult = 2.033e-01f;
    fResult *= fASqr;
    fResult += 3.1755e-01f;
    fResult *= fASqr;
    fResult += 1.0f;
    fResult *= fAngle;
    return fResult;
}

// from Heng Li, a combination of inverse square root (see wiki) and inversion: https://bits.stephan-brumme.com/inverse.html
Lfloat fastsqrtf(Lfloat x)
{
	union { Lfloat f; uint32_t i; } z = { x };
	z.i  = 0x5f3759df - (z.i >> 1);
	z.f *= (1.5f - (x * 0.5f * z.f * z.f));
	z.i = 0x7EEEEEEE - z.i;
	return z.f;
}

// alternative implementation for abs()
// REQUIRES: 32 bit integers
int fastabs_int(int in){
    unsigned int r;
    int const mask = in >> 31;
    
    r = (in ^ mask) - mask;
    
    return (r);
}

// alternative implementation for abs()
// REQUIRES: 32 bit Lfloats
Lfloat fastabsf(Lfloat f)
{
    union
    {
        Lfloat f;
        unsigned int ui;
    }alias;
    
    alias.f = f;
    alias.ui &= 0x7fffffff;
    return alias.f;
}

double fastexp(double x) {
    x = 1.0 + (x * 0.0009765625);
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x;
    return x;
}

inline Lfloat fastexpf(Lfloat x) {
    x = 1.0f + (x * 0.0009765625f);
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x;
    return x;
}

double fasterexp(double x) {
    x = 1.0 + (x * 0.00390625);
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x; x *= x; x *= x;
    return x;
}

inline Lfloat fasterexpf(Lfloat x) {
    x = 1.0f + (x * 0.00390625f);
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x; x *= x; x *= x;
    return x;
}

Lfloat fast_sinf2(Lfloat x)
{
	Lfloat invert = 1.0f;
	Lfloat out;

	x = x * INV_TWO_PI_TIMES_SINE_TABLE_SIZE;

	if (x < 0.0f)
	{
		x *= -1.0f;
		invert = -1.0f;
	}
	int intX = ((int)x) & 2047;
	int intXNext = (intX + 1) & 2047;
	Lfloat LfloatX = x-intX;

	out = __leaf_table_sinewave[intX] * (1.0f - LfloatX);
	out += __leaf_table_sinewave[intXNext] * LfloatX;
	out *= invert;
	return out;
}
// fast Lfloating-point exp2 function taken from Robert Bristow Johnson's
// post in the music-dsp list on Date: Tue, 02 Sep 2014 16:50:11 -0400
Lfloat fastexp2f(Lfloat x)
{
    if (x >= -127.0)
    {
        Lfloat accumulator, xPower;
        union {Lfloat f; int32_t i;} xBits;
        
        xBits.i = (int32_t)(x + 4096.0f) - 4096L;               /* integer part */
        x -= (Lfloat)(xBits.i);                                             /* fractional part */
        
        accumulator = 1.0f + 0.69303212081966f*x;
        xPower = x*x;
        accumulator += 0.24137976293709f*xPower;
        xPower *= x;
        accumulator += 0.05203236900844f*xPower;
        xPower *= x;
        accumulator += 0.01355574723481f*xPower;
        
        xBits.i += 127;                                                    /* bias integer part */
        xBits.i<<= 23;                                                     /* move biased int part into exponent bits */
        
        return accumulator * xBits.f;
    }
    else
    {
        return 0.0f;
    }
}


Lfloat fastPowf(Lfloat a, Lfloat b) {
    union 
    { 
        Lfloat d; int x; 
    } 
    u = { a };

    u.x = (int)(b * (u.x - 1064866805) + 1064866805);
    return u.d;
}


double fastPow(double a, double b) {
    union {
        double d;
        int x[2];
    } u = { a };

    u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
    
    u.x[0] = 0;
    return u.d;
}





/*
 you pass in a Lfloat array to get back two indexes representing the volumes of the left (index 0) and right (index 1) channels
 when t is -1, volumes[0] = 0, volumes[1] = 1
 when t = 0, volumes[0] = 0.707, volumes[1] = 0.707 (equal-power cross fade)
 when t = 1, volumes[0] = 1, volumes[1] = 0
 */

void LEAF_crossfade(Lfloat fade, Lfloat* volumes) {
    volumes[0] = sqrtf(0.5f * (1.0f + fade));
    volumes[1] = sqrtf(0.5f * (1.0f - fade));
}

// dope af
Lfloat LEAF_chebyshevT(Lfloat in, int n){
    if (n == 0) return 1;
    else if (n == 1) return in;
    else return 2.0f * in * LEAF_chebyshevT(in, n-1) - LEAF_chebyshevT(in, n-2);
}

#if !(_WIN32 || _WIN64)
Lfloat LEAF_CompoundChebyshevT(Lfloat in, int n, Lfloat* amps){
    Lfloat T[n+1];
    T[0] = 1.0f;
    T[1] = in;
    for (int i = 2; i <= n; ++i)
        T[i] = 2*in*T[i-1] - T[i-2];
    Lfloat out = 0;
    Lfloat amp = 0;
    for (int i = 0; i < n; ++i){
        out += amps[i]*T[i+1];
        amp += amps[i];
    }
    return out / amp ;
}
#endif

Lfloat LEAF_frequencyToMidi(Lfloat f)
{
    return (69.0f + 12.0f * log2f(f * INV_440));
}

// Jones shaper
Lfloat LEAF_shaper(Lfloat input, Lfloat m_drive)
{
    Lfloat fx = input * 2.0f;    // prescale
    Lfloat w, c, xc, xc2, xc4;
    
    xc = LEAF_clip(-SQRT8, fx, SQRT8);
    xc2 = xc*xc;
    c = 0.5f*fx*(3.0f - (xc2));
    xc4 = xc2 * xc2;
    w = (1.0f - xc2*0.25f + xc4*0.015625f) * WSCALE;
    Lfloat shaperOut = w*(c+ 0.05f*xc2)*(m_drive + 0.75f);
    shaperOut *= 0.5f;    // post_scale
    return shaperOut;
}

// round input to nearest rnd
Lfloat LEAF_round (Lfloat input, Lfloat rnd)
{
    rnd = fabsf(rnd);
    
    if (rnd <= 0.0000001f) return input;
    
    Lfloat scale = 1.f / rnd;
    
    return roundf(input * scale) * rnd;
}



Lfloat LEAF_bitwise_xor(Lfloat input, uint32_t op)
{
    union unholy_t unholy;
    unholy.f = input;
    unholy.i = (unholy.i ^ op);
    
    return unholy.f;
}

Lfloat LEAF_reedTable(Lfloat input, Lfloat offset, Lfloat slope)
{
    Lfloat output = offset + (slope * input);
    if ( output > 1.0f) output = 1.0f;
    if ( output < -1.0f) output = -1.0f;
    return output;
}

Lfloat   LEAF_softClip(Lfloat val, Lfloat thresh)
{
    Lfloat x;
    
    if(val > thresh)
    {
        x = thresh / val;
        return (1.0f - x) * (1.0f - thresh) + thresh;
    }
    else if(val < -thresh)
    {
        x = -thresh / val;
        return -((1.0f - x) * (1.0f - thresh) + thresh);
    }
    else
    {
        return val;
    }
}

#ifdef ITCMRAM
Lfloat __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) LEAF_clip(Lfloat min, Lfloat val, Lfloat max)
#else
Lfloat LEAF_clip(Lfloat min, Lfloat val, Lfloat max)
#endif
{

    if (val < min)
    {
        return min;
    }
    else if (val > max)
    {
        return max;
    }
    else
    {
        return val;
    }
}

int   LEAF_clipInt(int min, int val, int max)
{
    int tempmin = min;
    int tempmax = max;
    if (min > max)
    {
        tempmin = max;
        tempmax = min;
    }
    if (val < tempmin) {
        return tempmin;
    } else if (val > tempmax) {
        return tempmax;
    } else {
        return val;
    }
}

int     LEAF_isPrime(uint64_t number )
{
    if ( number == 2 ) return 1;
    if ( number & 1 ) {
        for ( int i=3; i<(int)sqrt((double)number)+1; i+=2 )
            if ( (number % i) == 0 ) return 0;
        return 1; // prime
    }
    else return 0; // even
}

// Adapted from MusicDSP: http://www.musicdsp.org/showone.php?id=238
Lfloat LEAF_tanh(Lfloat x)
{
    
    if( x < -3.0f )
        return -1.0f;
    else if( x > 3.0f )
        return 1.0f;
    else
        return x * ( 27.0f + x * x ) / ( 27.0f + 9.0f * x * x );
}



// Adapted from MusicDSP: http://www.musicdsp.org/showone.php?id=238
Lfloat LEAF_tanhNoClip(Lfloat x)
{
        return x * ( 27.0f + x * x ) / ( 27.0f + 9.0f * x * x );
}

// https://math.stackexchange.com/questions/107292/rapid-approximation-of-tanhx
Lfloat fast_tanh(Lfloat x){
  Lfloat x2 = x * x;
  Lfloat a = x * (135135.0f + x2 * (17325.0f + x2 * (378.0f + x2)));
  Lfloat b = 135135.0f + x2 * (62370.0f + x2 * (3150.0f + x2 * 28.0f));
  return a / b;
}

Lfloat fast_tanh2(Lfloat x)
{
    return x*(2027025.0f+270270.0f*x*x+6930.0f*x*x*x*x+36.0f*x*x*x*x*x*x)/(2027025.0f+945945.0f*x*x+51975.0f*x*x*x*x+630.0f*x*x*x*x*x*x+x*x*x*x*x*x*x*x);
}

Lfloat fast_tanh3(Lfloat x)
{
    Lfloat exp2x = fastexpf(2.0f*x);
    return(exp2x - 1.0f) / (exp2x + 1.0f);
}

//from antto on KVR forum
Lfloat   fast_tanh4 (Lfloat x)
{
    Lfloat xa = fabsf(x);
    Lfloat x2 = xa * xa;
    Lfloat x3 = xa * x2;
    Lfloat x4 = x2 * x2;
    Lfloat x7 = x3 * x4;
    Lfloat res = (1.0f - 1.0f / (1.0f + xa + x2 + 0.58576695f * x3 + 0.55442112f * x4 + 0.057481508f * x7));
    Lfloat   result = (x < 0) ? -res : res;

    return (result);
}

//from raphx on this post : https://www.kvraudio.com/forum/viewtopic.php?t=332930&start=30
Lfloat fast_tanh5(Lfloat x)
{
	Lfloat a = x + 0.16489087f * x*x*x + 0.00985468f * x*x*x*x*x;
	return a / sqrtf(1.0f + a * a);
}

Lfloat leaf_softClip(Lfloat sample)
{
    if (sample < -1.0f)
    {
        sample = -1.0f;
    } else if (sample > 1.0f)
    {
        sample = 1.0f;
    }
    return (1.5f * (sample - (((sample * sample * sample))* 0.3333333f)));
}

//from Olli Niemitalo
//https://dsp.stackexchange.com/questions/46629/finding-polynomial-approximations-of-a-sine-wave
Lfloat fastSine(Lfloat x)
{
	Lfloat term1 = x * 1.570034357f;
	Lfloat term2 = x * x * x * -0.6425216143f;
	Lfloat term3 = x * x * x * x * x * 0.07248725712f;
	return term1+term2+term3;
}

void LEAF_generate_sine(Lfloat* buffer, int size)
{
    Lfloat phase;
    for (int i = 0; i < size; i++)
    {
        phase = (Lfloat) i / (Lfloat) size;
        buffer[i] = sinf(phase * TWO_PI);
    }
}

void LEAF_generate_sawtooth(Lfloat* buffer, Lfloat basefreq, int size, LEAF* const leaf)
{
    int harmonic = 1;
    Lfloat phase = 0.0f;
    Lfloat freq = harmonic * basefreq;
    Lfloat amp;
    
    while (freq < (leaf->sampleRate * 0.5))
    {
        amp = 1.0f / harmonic;
        for (int i = 0; i < size; i++)
        {
            phase = (Lfloat) i / (Lfloat) size;
            buffer[i] += (amp * sinf(harmonic * phase * TWO_PI));
        }
        
        harmonic++;
        freq = harmonic * basefreq;
    }
}


void LEAF_generate_triangle(Lfloat* buffer, Lfloat basefreq, int size, LEAF* const leaf)
{
    int harmonic = 1;
    Lfloat phase = 0.0f;
    Lfloat freq = harmonic * basefreq;
    Lfloat amp = 1.0f;
    
    int count = 0;
    Lfloat mult = 1.0f;
    
    while (freq < (leaf->sampleRate * 0.5))
    {
        amp = 1.0f / (Lfloat)(harmonic * harmonic);
        
        if (count % 2)  mult = -1.0f;
        else            mult =  1.0f;
        
        for (int i = 0; i < size; i++)
        {
            phase = (Lfloat) i / (Lfloat) size;
            buffer[i] += (mult * amp * sinf(harmonic * phase * TWO_PI));
        }
        
        count++;
        harmonic += 2;
        freq = harmonic * basefreq;
    }
}

void LEAF_generate_square(Lfloat* buffer, Lfloat basefreq, int size, LEAF* const leaf)
{
    int harmonic = 1;
    Lfloat phase = 0.0f;
    Lfloat freq = harmonic * basefreq;
    Lfloat amp = 1.0f;
    
    while (freq < (leaf->sampleRate * 0.5))
    {
        amp = 1.0f / (Lfloat)(harmonic);
        
        for (int i = 0; i < size; i++)
        {
            phase = (Lfloat) i / (Lfloat) size;
            buffer[i] += (amp * sinf(harmonic * phase * TWO_PI));
        }
        
        harmonic += 2;
        freq = harmonic * basefreq;
    }
}


//0.001 base gives a good curve that goes from 1 to near zero
void LEAF_generate_exp(Lfloat* buffer, Lfloat base, Lfloat start, Lfloat end, Lfloat offset, int size)
{
    Lfloat increment = (end - start) / (Lfloat)(size-1);
    Lfloat x = start;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = powf(base, x) + offset;
        x += increment;
    }
}

//


void LEAF_generate_table_skew_non_sym_double(Lfloat* buffer, Lfloat start, Lfloat end, Lfloat center, int size)
{
    double skew = log (0.5) / log ((center - start) / (end - start));
    double increment = 1.0 / (double)(size-1);
    double x = 0.0000000001;
    double proportion = 0.0;
    for (int i = 0; i < size; i++)
    {
        proportion = exp (log(x) / skew);
        buffer[i] = (Lfloat)(start + (end - start) * proportion);
        x += increment;
    }
}


void LEAF_generate_table_skew_non_sym(Lfloat* buffer, Lfloat start, Lfloat end, Lfloat center, int size)
{
    Lfloat skew = logf (0.5) / logf ((center - start) / (end - start));
    Lfloat increment = 1.0f / (Lfloat)(size-1);
    Lfloat x = 0.0000000001f;
    Lfloat proportion = 0.0;
    for (int i = 0; i < size; i++)
    {
        proportion = expf (logf(x) / skew);
        buffer[i] = (Lfloat)(start + (end - start) * proportion);
        x += increment;
    }
}


void LEAF_generate_atodb(Lfloat* buffer, int size, Lfloat min, Lfloat max)
{
    Lfloat increment = (max-min) / (Lfloat)(size-1);
    Lfloat x = min;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = atodb(x);
        x += increment;
    }
}

void LEAF_generate_dbtoa(Lfloat* buffer, int size, Lfloat minDb, Lfloat maxDb)
{
    Lfloat increment = (maxDb-minDb) / (Lfloat)(size-1);
    Lfloat x = minDb;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = dbtoa(x);
        x += increment;
    }
}


void LEAF_generate_atodbPositiveClipped(Lfloat* buffer, Lfloat lowerThreshold, Lfloat range, int size)
{
    Lfloat increment = 1.0f / (Lfloat)(size-1);
    Lfloat x = 0.000000001f;
    Lfloat scalar = range / fastabsf(lowerThreshold);
    for (int i = 0; i < size; i++)
    {
        Lfloat temp = atodb(x);
        temp = LEAF_clip(lowerThreshold, temp, 0.0f);
        buffer[i] = (temp-lowerThreshold) * scalar;
        x += increment;
    }
}


void LEAF_generate_mtof(Lfloat* buffer, Lfloat startMIDI, Lfloat endMIDI, int size)
{
    Lfloat increment = 1.0f / (Lfloat)(size-1);
    Lfloat x = 0.0f;
    Lfloat scalar = (endMIDI-startMIDI);
    for (int i = 0; i < size; i++)
    {
        Lfloat midiVal = (x * scalar) + startMIDI;
        buffer[i] = mtof(midiVal);
        x += increment;
    }
}

void LEAF_generate_ftom(Lfloat* buffer, Lfloat startFreq, Lfloat endFreq, int size)
{
    Lfloat increment = 1.0f / (Lfloat)(size-1);
    Lfloat x = 0.0f;
    Lfloat scalar = (endFreq-startFreq);
    for (int i = 0; i < size; i++)
    {
        Lfloat midiVal = (x * scalar) + startFreq;
        buffer[i] = ftom(midiVal);
        x += increment;
    }
}

// http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/
// http://www.kvraudio.com/forum/viewtopic.php?t=375517
// t = phase, dt = inc, assuming 0-1 phase
// assumes discontinuity at 0, so offset inputs as needed
Lfloat LEAF_poly_blep(Lfloat t, Lfloat dt)
{
    
    // 0 <= t < 1
    if (t < dt) {
        t /= dt;
        return t+t - t*t - 1.0f;
    }
    // -1 < t < 0
    else if (t > 1.0f - dt) {
        t = (t - 1.0f) / dt;
        return t*t + t+t + 1.0f;
    }
    // 0 otherwise
    else return 0.0f;
    
//this version is from this discussion: https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
//    dt = -1.0f*dt;
//    Lfloat y = 0.0f;
//    if (t < 2.0f * dt)
//    {
//        Lfloat x = t / dt;
//        Lfloat u = 2.0f - x;
//        u *= u;
//        u *= u;
//        y += u;
//        if (t < dt) {
//            Lfloat v = 1.0f - x;
//            v *= v;
//            v *= v;
//            y -= 4.0f * v;
//        }
//    }
//    else if (t > 1.0f - (2.0f * dt))
//    {
//        Lfloat x = (t - 1.0f) / dt;
//        Lfloat u = 2.0f - x;
//        u *= u;
//        u *= u;
//        y += u;
//        if (t > 1.0f - dt) {
//            Lfloat v = 1.0f - x;
//            v *= v;
//            v *= v;
//            y += 4.0f * v;
//        }
//    }
//    return y * 0.083333333333333f; // divide by 12
}

//this is messed up right now, don't use -- JS
Lfloat LEAF_poly_blepInt(uint32_t t, uint32_t dt)
{

    // 0 <= t < 1
	uint32_t maxInt32minusDt = 4294967295u-dt;
    if (t < dt) {
    	float tF = (((float) t) / ((float) dt));
        return tF+tF - tF*tF - 1.0f;
    }
    // -1 < t < 0
    else if (t > maxInt32minusDt) {
        float tF = (((float)(t - maxInt32minusDt)) / (float)dt);
        return tF*tF + tF+tF + 1.0f;
    }
    // 0 otherwise
    else return 0.0f;
}

//this version is from this discussion: https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
Lfloat LEAF_poly_blamp(Lfloat t, Lfloat dt)
{

    Lfloat y = 0;
    if ((0 <= t) && (t < (2.0f * dt)))
    {
        Lfloat x = (t / dt);
        Lfloat u = 2.0f - x;
        Lfloat u2 = u * u;
        u *= u2 * u2;
        y -= u;
        if (t < dt)
        {
        	Lfloat v = 1.0f - x;
        	Lfloat v2 = v * v;
            v *= v2 * v2;
            y += 4.0f * v;
        }
    }
    return y * dt / 0.066666666666667f; // divide by 15

}

//-----------------------------------------------------------------------------
// name: mtof()
// desc: midi to freq, from PD source
//-----------------------------------------------------------------------------
Lfloat LEAF_midiToFrequency(Lfloat f)
{
    if( f <= -1500.0f ) return (0);
    else if( f > 1499.0f ) return (LEAF_midiToFrequency(1499.0f));
    else return ( powf(2.0f, (f - 69.0f) * 0.083333333333333f) * 440.0f );
}


// alpha, [0.0, 1.0]
Lfloat LEAF_interpolate_hermite (Lfloat A, Lfloat B, Lfloat C, Lfloat D, Lfloat alpha)
{
    alpha = LEAF_clip(0.0f, alpha, 1.0f);
    
    Lfloat a = -A*0.5f + (3.0f*B)*0.5f - (3.0f*C)*0.5f + D*0.5f;
    Lfloat b = A - (5.0f*B)*0.5f + 2.0f*C - D * 0.5f;
    Lfloat c = -A*0.5f + C*0.5f;
    Lfloat d = B;
    
    return a*alpha*alpha*alpha + b*alpha*alpha + c*alpha + d;
}


// from http://www.musicdsp.org/archive.php?classid=5#93
//xx is alpha (fractional part of sample value)
//grabbed this from Tom Erbe's Delay pd code
Lfloat LEAF_interpolate_hermite_x(Lfloat yy0, Lfloat yy1, Lfloat yy2, Lfloat yy3, Lfloat xx)
{
    // 4-point, 3rd-order Hermite (x-form)
    Lfloat c0 = yy1;
    Lfloat c1 = 0.5f * (yy2 - yy0);
    Lfloat y0my1 = yy0 - yy1;
    Lfloat c3 = (yy1 - yy2) + 0.5f * (yy3 - y0my1 - yy2);
    Lfloat c2 = y0my1 + c1 - c3;
    
    return ((c3 * xx + c2) * xx + c1) * xx + c0;
}

//this is a direct way of computing the thing. Another method is suggested in this paper:
//https://www.researchgate.net/publication/224312927_A_computationally_efficient_coefficient_update_technique_for_Lagrange_fractional_delay_filters
//but the advantages of that only happen above 3rd-order. For 3rd-order interpolation, the mults and adds would be the same.
Lfloat LEAF_interpolate_lagrange(Lfloat y0, Lfloat y1, Lfloat y2, Lfloat y3, Lfloat d)
{
    //
	Lfloat dp1 = d+1.0f;
	Lfloat dm1 = d-1.0f;
	Lfloat dm2 = d-2.0f;

	Lfloat pdp1d = dp1*d;
	Lfloat pddm1 = d*dm1;
	Lfloat pdm1dm2 = dm1*dm2;

	Lfloat h0 = -0.166666666666667f * pddm1 * dm2;
	Lfloat h1 = 0.5f * dp1 * pdm1dm2;
	Lfloat h2 = -0.5f * pdp1d * dm2;
	Lfloat h3 = 0.166666666666667f * pdp1d * dm2;

    return ((h3*y3) + (h2*y2) + (h1*y1)+ (h0*y0));
}



// alpha, [0.0, 1.0]
Lfloat LEAF_interpolation_linear (Lfloat A, Lfloat B, Lfloat alpha)
{
    alpha = LEAF_clip(0.0f, alpha, 1.0f);
    
    Lfloat omAlpha = 1.0f - alpha;
    
    // First 1/2 of interpolation
    Lfloat out = A * omAlpha;
    
    out += B * alpha;
    
    return out;
}

#define LOGTEN 2.302585092994

Lfloat mtof(Lfloat f)
{
    if (f <= -1500.0f) return(0);
    else if (f > 1499.0f) return(mtof(1499.0f));
    else return (8.17579891564f * expf(0.0577622650f * f));
}

Lfloat fast_mtof(Lfloat f)
{
    return (8.17579891564f * fastexpf(0.0577622650f * f));
}

Lfloat faster_mtof(Lfloat f)
{
    return (8.17579891564f * fasterexpf(0.0577622650f * f));
}

Lfloat ftom(Lfloat f)
{
    return (f > 0 ? 17.3123405046f * logf(.12231220585f * f) : -1500.0f);
}

Lfloat powtodb(Lfloat f)
{
    if (f <= 0) return (0);
    else
    {
        Lfloat val = 100.0f + 10.0f/LOGTEN * logf(f);
        return (val < 0.0f ? 0.0f : val);
    }
}

Lfloat rmstodb(Lfloat f)
{
    if (f <= 0) return (0);
    else
    {
        Lfloat val = 100 + 20.f/LOGTEN * log(f);
        return (val < 0 ? 0 : val);
    }
}

Lfloat dbtopow(Lfloat f)
{
    if (f <= 0)
        return(0);
    else
    {
        if (f > 870.0f)
            f = 870.0f;
        return (expf((LOGTEN * 0.1f) * (f-100.0f)));
    }
}

Lfloat dbtorms(Lfloat f)
{
    if (f <= 0)
        return(0);
    else
    {
        if (f > 485.0f)
            f = 485.0f;
    }
    return (expf((LOGTEN * 0.05f) * (f-100.0f)));
}


Lfloat atodb(Lfloat a)
{
    return 20.0f*log10f(a);
}
Lfloat fasteratodb(Lfloat a)
{
	return 20.0f*log10f_fast(a);
}

Lfloat dbtoa(Lfloat db)
{
    return powf(10.0f, db * 0.05f);
}


Lfloat fastdbtoa(Lfloat db)
{
    //return powf(10.0f, db * 0.05f);
    return expf(0.115129254649702f * db); //faster version from http://openaudio.blogspot.com/2017/02/faster-log10-and-pow.html
}

Lfloat fasterdbtoa(Lfloat db)
{
    //return powf(10.0f, db * 0.05f);
    return fasterexpf(0.115129254649702f * db); //faster version from http://openaudio.blogspot.com/2017/02/faster-log10-and-pow.html
}


Lfloat maximum (Lfloat num1, Lfloat num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

Lfloat minimum (Lfloat num1, Lfloat num2)
{
    return (num1 < num2 ) ? num1 : num2;
}

// built in compiler popcount functions should be faster but we want this to be portable
// could try to add some define that call the correct function depending on compiler
// or let the user pointer popcount() to whatever they want
// something to look into...
int popcount(unsigned int x)
{
//    int c = 0;
//    for (; x != 0; x &= x - 1)
//        c++;
//    return c;
    unsigned long long y;
    y = x * 0x0002000400080010ULL;
    y = y & 0x1111111111111111ULL;
    y = y * 0x1111111111111111ULL;
    y = y >> 60;
    return (int) y;
}

Lfloat median3f(Lfloat a, Lfloat b, Lfloat c)
{
    return fmax(fmin(a, b), fmin(fmax(a, b), c));
}

#if LEAF_INCLUDE_MINBLEP_TABLES
/// MINBLEPS
// https://github.com/MrBlueXav/Dekrispator_v2 blepvco.c
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) place_step_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat scale)
#else
void place_step_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat scale)
#endif
{
	Lfloat r;
	long i;

	r = MINBLEP_PHASES * phase / w;
	i = lrintf(r - 0.5f);
	r -= (Lfloat)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */

	while (i < MINBLEP_PHASES * STEP_DD_PULSE_LENGTH) {
		buffer[index] += scale * (step_dd_table[i].value + r * step_dd_table[i].delta);
		i += MINBLEP_PHASES;
		index++;
	}
}




#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) place_slope_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat slope_delta)
#else
void place_slope_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat slope_delta)
#endif
{
	Lfloat r;
	long i;

	r = MINBLEP_PHASES * phase / w;
	i = lrintf(r - 0.5f);
	r -= (Lfloat)i;
	i &= MINBLEP_PHASE_MASK;  /* extreme modulation can cause i to be out-of-range */

	slope_delta *= w;

	while (i < MINBLEP_PHASES * SLOPE_DD_PULSE_LENGTH) {
		buffer[index] += slope_delta * (slope_dd_table[i] + r * (slope_dd_table[i + 1] - slope_dd_table[i]));
		i += MINBLEP_PHASES;
		index++;
	}
}
#endif // LEAF_INCLUDE_MINBLEP_TABLES
    /*! @} */

