/*==============================================================================
 
 leaf-math.h
 Created: 22 Jan 2017 7:02:56pm
 Author:  Michael R Mulshine
 
 ==============================================================================*/

#ifndef LEAF_MATH_H_INCLUDED
#define LEAF_MATH_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
#include "leaf-global.h"
#include "math.h"
#include "stdint.h"
#include "stdlib.h"
#include "limits.h"
    
    //==============================================================================
    
    //==============================================================================
    
    typedef enum oBool
    {
        OTRUE  = 1,
        OFALSE = 0
    }oBool;
    
    // Allows for bitwise operations on floats
    union unholy_t { /* a union between a float and an integer */
        float f;
        int i;
    };

#define SQRT8 2.82842712475f
#define WSCALE 1.30612244898f
#define PI              (3.14159265358979f)
#define TWO_PI          (6.28318530717958f)
#define HALF_PI			(1.570796326794897f)

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
    
    
#define INV_TWELVE                 0.0833333333f
#define INV_440                     0.0022727273f
    
#define LOG2                             0.3010299957f
#define INV_LOG2                     3.321928095f
    
#define SOS_M                         343.0f
#define TWO_TO_5                     32.0f
#define INV_TWO_TO_5             0.03125f
#define TWO_TO_7                     128.f
#define INV_TWO_TO_7             0.0078125f
#define TWO_TO_8                     256.f
#define INV_TWO_TO_8             0.00390625f
#define TWO_TO_9                     512.f
#define INV_TWO_TO_9             0.001953125f
#define TWO_TO_10                 1024.f
#define INV_TWO_TO_10         0.0009765625f
#define TWO_TO_11                 2048.f
#define INV_TWO_TO_11         0.00048828125f
#define TWO_TO_12                 4096.f
#define INV_TWO_TO_12         0.00024414062f
#define TWO_TO_15                 32768.f
#define TWO_TO_16                 65536.f
#define INV_TWO_TO_15         0.00003051757f
#define INV_TWO_TO_16         0.00001525878f
#define TWO_TO_16_MINUS_ONE 65535.0f
#define TWO_TO_23        8388608.0f
#define INV_TWO_TO_23    0.00000011920929f
#define TWO_TO_31        2147483648.0f
#define INV_TWO_TO_31    0.000000000465661f
#define TWO_TO_32        4294967296.0f
#define INV_TWO_TO_32    0.000000000232831f
    
#define ONE_OVER_SQRT2	0.707106781186548f

#define LOGTEN 2.302585092994

    // Jones shaper
    float LEAF_shaper     (float input, float m_drive);
    float LEAF_reedTable  (float input, float offset, float slope);
    
    float LEAF_reduct (float input, float ratio);
    float LEAF_round (float input, float rnd);
    float LEAF_bitwise_xor(float input, uint32_t op);
    
    float LEAF_reduct (float input, float ratio);
    float LEAF_round (float input, float rnd);
    float LEAF_bitwise_xor(float input, uint32_t op);
    
    float       LEAF_clip               (float min, float val, float max);
    int         LEAF_clipInt            (int min, int val, int max);
    float       LEAF_softClip            (float val, float thresh);
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
    float LEAF_interpolate_hermite_x(float yy0, float yy1, float yy2, float yy3, float xx);
    float LEAF_interpolation_linear (float A, float B, float t);
    
    float interpolate3max(float *buf, const int peakindex);
    float interpolate3phase(float *buf, const int peakindex);
    

    float fastcosf(float fAngle);

    float fastercosf(float fAngle);
    // alternative implementation for abs()
    // REQUIRES: 32 bit integers
    int fastabs_int(int in);
    
    // alternative implementation for abs()
    // REQUIRES: 32 bit floats
    float fastabsf(float f);
    
    float fastexp2f(float f);
    
    float fastPowf(float a, float b) ;
    double fastPow(double a, double b);

    
    void LEAF_crossfade(float fade, float* volumes);


    float LEAF_tanh(float x);
    void LEAF_generate_sine(float* buffer, int size);
    void LEAF_generate_sawtooth(float* buffer, float basefreq, int size);
    void LEAF_generate_triangle(float* buffer, float basefreq, int size);
    void LEAF_generate_square(float* buffer, float basefreq, int size);

    //0.001 base gives a good curve that goes from 1 to near zero
    //1000 gives a good curve from -1.0 to 0.0
    void LEAF_generate_exp(float* buffer, float base, float start, float end, float offset, int size);


    void LEAF_generate_atodb(float* buffer, int size);
    void LEAF_generate_atodbPositiveClipped(float* buffer, float lowerThreshold, float range, int size);

    
    float LEAF_poly_blep(float t, float dt);
    float LEAF_midiToFrequency(float f);


    float fast_mtof(float f);

    double fastexp(double x);

    float fastexpf(float x);

    double fasterexp(double x);

    float fasterexpf(float x);

    float mtof(float f);
    
    float fast_mtof(float f);

    float faster_mtof(float f);

    float ftom(float f);
    
    float powtodb(float f);
    
    float rmstodb(float f);
    
    float dbtopow(float f);
    
    float dbtorms(float f);
    
    float atodb(float a);

    float dbtoa(float db);

    float fastdbtoa(float db);

    float fasterdbtoa(float db);

    float maximum (float num1, float num2);

    float minimum (float num1, float num2);
    
    // built in compiler popcount functions should be faster but we want this to be portable
    // could try to add some define that call the correct function depending on compiler
    // or let the user pointer popcount() to whatever they want
    // something to look into...
    int popcount(unsigned int x);


    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_MATH_H_INCLUDED

//==============================================================================

