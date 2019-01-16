/*
  ==============================================================================

    LEAFMath.h
    Created: 22 Jan 2017 7:02:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef LEAFMATH_H_INCLUDED
#define LEAFMATH_H_INCLUDED

#include "math.h"
#include "stdint.h"
#include "stdlib.h"

typedef enum oBool
{
    OTRUE  = 1,
    OFALSE = 0
}oBool;

#define SQRT8 2.82842712475f
#define WSCALE 1.30612244898f
#define PI              (3.14159265358979f)
#define TWO_PI          (2 * PI)

#define VSF             1.0e-38f

#define MAX_DELAY       8192
#define INV_128         0.0078125f

#define INV_20         0.05f
#define INV_40         0.025f
#define INV_80         0.0125f
#define INV_160        0.00625f
#define INV_320        0.003125f
#define INV_640        0.0015625f
#define INV_1280       0.00078125f
#define INV_2560       0.000390625f
#define INV_5120       0.0001953125f
#define INV_10240      0.00009765625f
#define INV_20480      0.000048828125f


#define INV_TWELVE 				0.0833333333f
#define INV_440 					0.0022727273f

#define LOG2 							0.3010299957f
#define INV_LOG2 					3.321928095f

#define SOS_M 						343.0f

#define TWO_TO_7 					128.f
#define INV_TWO_TO_7 			0.0078125f
#define TWO_TO_8 					256.f
#define INV_TWO_TO_8 			0.00390625f
#define TWO_TO_5 					32.0f
#define INV_TWO_TO_5 			0.03125f
#define TWO_TO_12 				4096.f
#define INV_TWO_TO_12 		0.00024414062f
#define TWO_TO_15 				32768.f
#define TWO_TO_16 				65536.f
#define INV_TWO_TO_15 		0.00003051757f
#define INV_TWO_TO_16 		0.00001525878f
#define TWO_TO_16_MINUS_ONE 65535.0f
#define TWO_TO_23		8388608.0f
#define INV_TWO_TO_23	0.00000011920929f
#define TWO_TO_31		2147483648.0f
#define INV_TWO_TO_31	0.000000000465661f

// Jones shaper
float LEAF_shaper     (float input, float m_drive);
float LEAF_reedTable  (float input, float offset, float slope);

float       LEAF_clip               (float min, float val, float max);
float   	LEAF_softClip						(float val, float thresh);
oBool       LEAF_isPrime            (uint64_t number );

float       LEAF_midiToFrequency    (float f);
float       LEAF_frequencyToMidi(float f);

void        LEAF_generate_sine     (float* buffer, int size);
void        LEAF_generate_sawtooth (float* buffer, float basefreq, int size);
void        LEAF_generate_triangle (float* buffer, float basefreq, int size);
void        LEAF_generate_square   (float* buffer, float basefreq, int size);

// dope af
float       LEAF_chebyshevT(float in, int n);
float       LEAF_CompoundChebyshevT(float in, int n, float* amps);


// Hermite interpolation
float LEAF_interpolate_hermite (float A, float B, float C, float D, float t);
float LEAF_interpolation_linear (float A, float B, float t);

static inline float interpolate3max(float *buf, const int peakindex)
{
    float a = buf[peakindex-1];
    float b = buf[peakindex];
    float c = buf[peakindex+1];
    float realpeak;
    
    realpeak = b + (float)0.125 * (c - a) * (c - a) / ((float)2. * b - a - c);
    
    return(realpeak);
}

static inline float interpolate3phase(float *buf, const int peakindex)
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
static inline int fastabs_int(int in){
    unsigned int r;
    int const mask = in >> 31;
    
    r = (in ^ mask) - mask;
    
    return (r);
}

// alternative implementation for abs()
// REQUIRES: 32 bit floats
static inline float fastabs(float f)
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

#endif  // LEAFMATH_H_INCLUDED
