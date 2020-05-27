/*==============================================================================
 
 leaf-math.c
 Created: 22 Jan 2017 7:02:56pm
 Author:  Michael R Mulshine
 
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
float log2f_approx(float X) {
    float Y, F;
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

float interpolate3max(float *buf, const int peakindex)
{
    float a = buf[peakindex-1];
    float b = buf[peakindex];
    float c = buf[peakindex+1];
    float realpeak;
    
    realpeak = b + (float)0.125 * (c - a) * (c - a) / ((float)2. * b - a - c);
    
    return(realpeak);
}

float interpolate3phase(float *buf, const int peakindex)
{
    float a = buf[peakindex-1];
    float b = buf[peakindex];
    float c = buf[peakindex+1];
    float fraction;
    
    fraction = ((float)0.5 * (c - a)) / ((float)2. * b - a - c);
    
    return(fraction);
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
// REQUIRES: 32 bit floats
float fastabsf(float f)
{
    union
    {
        float f;
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

float fastexpf(float x) {
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

float fasterexpf(float x) {
    x = 1.0f + (x * 0.00390625f);
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x; x *= x; x *= x;
    return x;
}

// fast floating-point exp2 function taken from Robert Bristow Johnson's
// post in the music-dsp list on Date: Tue, 02 Sep 2014 16:50:11 -0400
float fastexp2f(float x)
{
    if (x >= -127.0)
    {
        float accumulator, xPower;
        union {float f; int32_t i;} xBits;
        
        xBits.i = (int32_t)(x + 4096.0f) - 4096L;               /* integer part */
        x -= (float)(xBits.i);                                             /* fractional part */
        
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


float fastPowf(float a, float b) {
    union 
    { 
        float d; int x; 
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
 you pass in a float array to get back two indexes representing the volumes of the left (index 0) and right (index 1) channels
 when t is -1, volumes[0] = 0, volumes[1] = 1
 when t = 0, volumes[0] = 0.707, volumes[1] = 0.707 (equal-power cross fade)
 when t = 1, volumes[0] = 1, volumes[1] = 0
 */

void LEAF_crossfade(float fade, float* volumes) {
    volumes[0] = sqrtf(0.5f * (1.0f + fade));
    volumes[1] = sqrtf(0.5f * (1.0f - fade));
}

// dope af
float LEAF_chebyshevT(float in, int n){
    if (n == 0) return 1;
    else if (n == 1) return in;
    else return 2.0f * in * LEAF_chebyshevT(in, n-1) - LEAF_chebyshevT(in, n-2);
}

#if !(_WIN32 || _WIN64)
float LEAF_CompoundChebyshevT(float in, int n, float* amps){
    float T[n+1];
    T[0] = 1.0f;
    T[1] = in;
    for (int i = 2; i <= n; ++i)
        T[i] = 2*in*T[i-1] - T[i-2];
    float out = 0;
    float amp = 0;
    for (int i = 0; i < n; ++i){
        out += amps[i]*T[i+1];
        amp += amps[i];
    }
    return out / amp ;
}
#endif

float LEAF_frequencyToMidi(float f)
{
    return (69.0f + 12.0f * log2f(f * INV_440));
}

// Jones shaper
float LEAF_shaper(float input, float m_drive)
{
    float fx = input * 2.0f;    // prescale
    float w, c, xc, xc2, xc4;
    
    xc = LEAF_clip(-SQRT8, fx, SQRT8);
    xc2 = xc*xc;
    c = 0.5f*fx*(3.0f - (xc2));
    xc4 = xc2 * xc2;
    w = (1.0f - xc2*0.25f + xc4*0.015625f) * WSCALE;
    float shaperOut = w*(c+ 0.05f*xc2)*(m_drive + 0.75f);
    shaperOut *= 0.5f;    // post_scale
    return shaperOut;
}

// round input to nearest rnd
float LEAF_round (float input, float rnd)
{
    rnd = fabsf(rnd);
    
    if (rnd <= 0.0000001f) return input;
    
    float scale = 1.f / rnd;
    
    return roundf(input * scale) / scale;
}



float LEAF_bitwise_xor(float input, uint32_t op)
{
    union unholy_t unholy;
    unholy.f = input;
    unholy.i = (unholy.i ^ op);
    
    return unholy.f;
}

float LEAF_reedTable(float input, float offset, float slope)
{
    float output = offset + (slope * input);
    if ( output > 1.0f) output = 1.0f;
    if ( output < -1.0f) output = -1.0f;
    return output;
}

float   LEAF_softClip(float val, float thresh)
{
    float x;
    
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

float   LEAF_clip(float min, float val, float max)
{
    float tempmin = min;
    float tempmax = max;
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

oBool     LEAF_isPrime(uint64_t number )
{
    if ( number == 2 ) return OTRUE;
    if ( number & 1 ) {
        for ( int i=3; i<(int)sqrt((double)number)+1; i+=2 )
            if ( (number % i) == 0 ) return OFALSE;
        return OTRUE; // prime
    }
    else return OFALSE; // even
}

// Adapted from MusicDSP: http://www.musicdsp.org/showone.php?id=238
float LEAF_tanh(float x)
{
    
    if( x < -3.0f )
        return -1.0f;
    else if( x > 3.0f )
        return 1.0f;
    else
        return x * ( 27.0f + x * x ) / ( 27.0f + 9.0f * x * x );
}


void LEAF_generate_sine(float* buffer, int size)
{
    float phase;
    for (int i = 0; i < size; i++)
    {
        phase = (float) i / (float) size;
        buffer[i] = sinf(phase * TWO_PI);
    }
}

void LEAF_generate_sawtooth(float* buffer, float basefreq, int size)
{
    int harmonic = 1;
    float phase = 0.0f;
    float freq = harmonic * basefreq;
    float amp;
    
    while (freq < (leaf.sampleRate * 0.5))
    {
        amp = 1.0f / harmonic;
        for (int i = 0; i < size; i++)
        {
            phase = (float) i / (float) size;
            buffer[i] += (amp * sinf(harmonic * phase * TWO_PI));
        }
        
        harmonic++;
        freq = harmonic * basefreq;
    }
}


void LEAF_generate_triangle(float* buffer, float basefreq, int size)
{
    int harmonic = 1;
    float phase = 0.0f;
    float freq = harmonic * basefreq;
    float amp = 1.0f;
    
    int count = 0;
    float mult = 1.0f;
    
    while (freq < (leaf.sampleRate * 0.5))
    {
        amp = 1.0f / (float)(harmonic * harmonic);
        
        if (count % 2)  mult = -1.0f;
        else            mult =  1.0f;
        
        for (int i = 0; i < size; i++)
        {
            phase = (float) i / (float) size;
            buffer[i] += (mult * amp * sinf(harmonic * phase * TWO_PI));
        }
        
        count++;
        harmonic += 2;
        freq = harmonic * basefreq;
    }
}

void LEAF_generate_square(float* buffer, float basefreq, int size)
{
    int harmonic = 1;
    float phase = 0.0f;
    float freq = harmonic * basefreq;
    float amp = 1.0f;
    
    while (freq < (leaf.sampleRate * 0.5))
    {
        amp = 1.0f / (float)(harmonic);
        
        for (int i = 0; i < size; i++)
        {
            phase = (float) i / (float) size;
            buffer[i] += (amp * sinf(harmonic * phase * TWO_PI));
        }
        
        harmonic += 2;
        freq = harmonic * basefreq;
    }
}


//0.001 base gives a good curve that goes from 1 to near zero
void LEAF_generate_exp(float* buffer, float base, float start, float end, float offset, int size)
{
	float increment = (end - start) / (float)size;
	float x = start;
	for (int i = 0; i < size; i++)
	{
		buffer[i] = powf(base, x) + offset;
		x += increment;
    }
}


// http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/
// http://www.kvraudio.com/forum/viewtopic.php?t=375517
// t = phase, dt = inc, assuming 0-1 phase
// assumes discontinuity at 0, so offset inputs as needed
float LEAF_poly_blep(float t, float dt)
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
    
//
//    float y = 0.0f;
//    if (t < 2.0f * dt)
//    {
//        float x = t / dt;
//        float u = 2.0f - x;
//        u *= u;
//        u *= u;
//        y += u;
//        if (t < dt) {
//            float v = 1.0f - x;
//            v *= v;
//            v *= v;
//            y -= 4.0f * v;
//        }
//    }
//    else if (t > 1.0f - (2.0f * dt))
//    {
//        float x = (t - 1.0f) / dt;
//        float u = 2.0f - x;
//        u *= u;
//        u *= u;
//        y += u;
//        if (t > 1.0f - dt) {
//            float v = 1.0f - x;
//            v *= v;
//            v *= v;
//            y += 4.0f * v;
//        }
//    }
//    return y / 12.0f;
}


//-----------------------------------------------------------------------------
// name: mtof()
// desc: midi to freq, from PD source
//-----------------------------------------------------------------------------
float LEAF_midiToFrequency(float f)
{
    if( f <= -1500.0f ) return (0);
    else if( f > 1499.0f ) return (LEAF_midiToFrequency(1499.0f));
    else return ( powf(2.0f, (f - 69.0f) * 0.083333333333333f) * 440.0f );
}


// alpha, [0.0, 1.0]
float LEAF_interpolate_hermite (float A, float B, float C, float D, float alpha)
{
    alpha = LEAF_clip(0.0f, alpha, 1.0f);
    
    float a = -A*0.5f + (3.0f*B)*0.5f - (3.0f*C)*0.5f + D*0.5f;
    float b = A - (5.0f*B)*0.5f + 2.0f*C - D * 0.5f;
    float c = -A*0.5f + C*0.5f;
    float d = B;
    
    return a*alpha*alpha*alpha + b*alpha*alpha + c*alpha + d;
}


// from http://www.musicdsp.org/archive.php?classid=5#93
//xx is alpha (fractional part of sample value)
//grabbed this from Tom Erbe's Delay pd code
float LEAF_interpolate_hermite_x(float yy0, float yy1, float yy2, float yy3, float xx)
{
    // 4-point, 3rd-order Hermite (x-form)
    float c0 = yy1;
    float c1 = 0.5f * (yy2 - yy0);
    float y0my1 = yy0 - yy1;
    float c3 = (yy1 - yy2) + 0.5f * (yy3 - y0my1 - yy2);
    float c2 = y0my1 + c1 - c3;
    
    return ((c3 * xx + c2) * xx + c1) * xx + c0;
}

// alpha, [0.0, 1.0]
float LEAF_interpolation_linear (float A, float B, float alpha)
{
    alpha = LEAF_clip(0.0f, alpha, 1.0f);
    
    float omAlpha = 1.0f - alpha;
    
    // First 1/2 of interpolation
    float out = A * omAlpha;
    
    out += B * alpha;
    
    return out;
}

#define LOGTEN 2.302585092994

float mtof(float f)
{
    if (f <= -1500.0f) return(0);
    else if (f > 1499.0f) return(mtof(1499.0f));
    else return (8.17579891564f * expf(0.0577622650f * f));
}

float fast_mtof(float f)
{
    return (8.17579891564f * fastexpf(0.0577622650f * f));
}

float faster_mtof(float f)
{
    return (8.17579891564f * fastexpf(0.0577622650f * f));
}

float ftom(float f)
{
    return (f > 0 ? 17.3123405046f * logf(.12231220585f * f) : -1500.0f);
}

float powtodb(float f)
{
    if (f <= 0) return (0);
    else
    {
        float val = 100.0f + 10.0f/LOGTEN * logf(f);
        return (val < 0.0f ? 0.0f : val);
    }
}

float rmstodb(float f)
{
    if (f <= 0) return (0);
    else
    {
        float val = 100 + 20.f/LOGTEN * log(f);
        return (val < 0 ? 0 : val);
    }
}

float dbtopow(float f)
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

float dbtorms(float f)
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


float atodb(float a)
{
    return 20.0f*log10f(a);
}

float dbtoa(float db)
{
    return powf(10.0f, db * 0.05f);
    //return expf(0.115129254649702f * db); //faster version from http://openaudio.blogspot.com/2017/02/faster-log10-and-pow.html
}


float fastdbtoa(float db)
{
    //return powf(10.0f, db * 0.05f);
    return expf(0.115129254649702f * db); //faster version from http://openaudio.blogspot.com/2017/02/faster-log10-and-pow.html
}


float maximum (float num1, float num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

float minimum (float num1, float num2)
{
    return (num1 < num2 ) ? num1 : num2;
}


