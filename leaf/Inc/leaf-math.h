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
    
    /*!
     @ingroup math
     @{
     */
    
    // Allows for bitwise operations on Lfloats
    union unholy_t { /* a union between a Lfloat and an integer */
        Lfloat f;
        int i;
    };

#define SQRT8 2.82842712475f
#define LEAF_SQRT2 1.41421356237f
#define WSCALE 1.30612244898f
#define PI              (3.14159265358979f)
#define TWO_PI          (6.28318530717958f)
#define HALF_PI         (1.570796326794897f)
#define INV_TWO_PI		(0.159154943091895f)
#define INV_TWO_PI_TIMES_SINE_TABLE_SIZE		(325.949323452201648)
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
    
#define INV_44100      0.00002267573f
#define INV_48000      0.00002083333f
    
    
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

#define TWO_TO_32_INT        4294967296u
#define TWO_TO_31_INT        2147483648u
    
#define ONE_OVER_SQRT2  0.707106781186548f

#define LOGTEN 2.302585092994
    Lfloat log2f_approx(Lfloat X);
    Lfloat log2f_approx2(Lfloat x);
    Lfloat fast_sinf2(Lfloat x);
    // Jones shaper
    Lfloat LEAF_shaper     (Lfloat input, Lfloat m_drive);
    Lfloat LEAF_reedTable  (Lfloat input, Lfloat offset, Lfloat slope);
    
    Lfloat LEAF_reduct (Lfloat input, Lfloat ratio);
    Lfloat LEAF_round (Lfloat input, Lfloat rnd);
    Lfloat LEAF_bitwise_xor(Lfloat input, uint32_t op);
    
    Lfloat LEAF_reduct (Lfloat input, Lfloat ratio);
    Lfloat LEAF_round (Lfloat input, Lfloat rnd);
    Lfloat LEAF_bitwise_xor(Lfloat input, uint32_t op);
    
    Lfloat       LEAF_clip               (Lfloat min, Lfloat val, Lfloat max);
    int         LEAF_clipInt            (int min, int val, int max);
    Lfloat       LEAF_softClip           (Lfloat val, Lfloat thresh);
    int         LEAF_isPrime            (uint64_t number );
    
    Lfloat       LEAF_midiToFrequency    (Lfloat f);
    Lfloat       LEAF_frequencyToMidi(Lfloat f);
    
    void        LEAF_generate_sine     (Lfloat* buffer, int size);
    void        LEAF_generate_sawtooth (Lfloat* buffer, Lfloat basefreq, int size, LEAF* const leaf);
    void        LEAF_generate_triangle (Lfloat* buffer, Lfloat basefreq, int size, LEAF* const leaf);
    void        LEAF_generate_square   (Lfloat* buffer, Lfloat basefreq, int size, LEAF* const leaf);
    
    // dope af
    Lfloat       LEAF_chebyshevT(Lfloat in, int n);
    Lfloat       LEAF_CompoundChebyshevT(Lfloat in, int n, Lfloat* amps);
    
    // Hermite interpolation
    Lfloat LEAF_interpolate_hermite (Lfloat A, Lfloat B, Lfloat C, Lfloat D, Lfloat t);
    Lfloat LEAF_interpolate_hermite_x(Lfloat yy0, Lfloat yy1, Lfloat yy2, Lfloat yy3, Lfloat xx);
    Lfloat LEAF_interpolation_linear (Lfloat A, Lfloat B, Lfloat t);
    
    Lfloat interpolate3max(Lfloat *buf, const int peakindex);
    Lfloat interpolate3phase(Lfloat *buf, const int peakindex);
    

    Lfloat fastcosf(Lfloat fAngle);

    Lfloat fastercosf(Lfloat fAngle);

    Lfloat fasttanf (Lfloat fAngle);


    Lfloat fastertanf(Lfloat fAngle);


    // alternative implementation for abs()
    // REQUIRES: 32 bit integers
    int fastabs_int(int in);
    
    // alternative implementation for abs()
    // REQUIRES: 32 bit Lfloats
    Lfloat fastabsf(Lfloat f);
    
    Lfloat fastexp2f(Lfloat f);
    
    Lfloat fastPowf(Lfloat a, Lfloat b) ;
    double fastPow(double a, double b);

    
    void LEAF_crossfade(Lfloat fade, Lfloat* volumes);

    Lfloat LEAF_tanh(Lfloat x);
    Lfloat LEAF_tanhNoClip(Lfloat x);
    Lfloat fast_tanh(Lfloat x);
    Lfloat fast_tanh2(Lfloat x);
    Lfloat fast_tanh3(Lfloat x);
    Lfloat fast_tanh4(Lfloat x);

    //0.001 base gives a good curve that goes from 1 to near zero
    //1000 gives a good curve from -1.0 to 0.0
    void LEAF_generate_exp(Lfloat* buffer, Lfloat base, Lfloat start, Lfloat end, Lfloat offset, int size);
    void LEAF_generate_table_skew_non_sym(Lfloat* buffer, Lfloat start, Lfloat end, Lfloat center, int size);

    void LEAF_generate_atodb(Lfloat* buffer, int size, Lfloat min, Lfloat max);
    void LEAF_generate_atodbPositiveClipped(Lfloat* buffer, Lfloat lowerThreshold, Lfloat range, int size);
    void LEAF_generate_dbtoa(Lfloat* buffer, int size, Lfloat minDb, Lfloat maxDb);
    void LEAF_generate_mtof(Lfloat* buffer, Lfloat startMIDI, Lfloat endMIDI, int size);
    void LEAF_generate_ftom(Lfloat* buffer, Lfloat startFreq, Lfloat endFreq, int size);
    
    Lfloat LEAF_poly_blep(Lfloat t, Lfloat dt);
    Lfloat LEAF_poly_blamp(Lfloat t, Lfloat dt);
    Lfloat LEAF_midiToFrequency(Lfloat f);


    Lfloat fast_mtof(Lfloat f);

    double fastexp(double x);

    Lfloat fastexpf(Lfloat x);

    double fasterexp(double x);

    Lfloat fasterexpf(Lfloat x);
    
    Lfloat fastsqrtf(Lfloat x);

    Lfloat mtof(Lfloat f);
    
    Lfloat fast_mtof(Lfloat f);

    Lfloat faster_mtof(Lfloat f);

    Lfloat ftom(Lfloat f);
    
    Lfloat powtodb(Lfloat f);
    
    Lfloat rmstodb(Lfloat f);
    
    Lfloat dbtopow(Lfloat f);
    
    Lfloat dbtorms(Lfloat f);
    
    Lfloat atodb(Lfloat a);

    Lfloat fasteratodb(Lfloat a);

    Lfloat dbtoa(Lfloat db);

    Lfloat fastdbtoa(Lfloat db);

    Lfloat fasterdbtoa(Lfloat db);

    Lfloat maximum (Lfloat num1, Lfloat num2);

    Lfloat minimum (Lfloat num1, Lfloat num2);
    
    // built in compiler popcount functions should be faster but we want this to be portable
    // could try to add some define that call the correct function depending on compiler
    // or let the user pointer popcount() to whatever they want
    // something to look into...
    int popcount(unsigned int x);
    
    Lfloat median3f(Lfloat a, Lfloat b, Lfloat c);

#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) place_step_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat scale);
#else
void place_step_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat scale);
#endif
#ifdef ITCMRAM
void __attribute__ ((section(".itcmram"))) __attribute__ ((aligned (32))) place_slope_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat slope_delta);
#else
void place_slope_dd(Lfloat *buffer, int index, Lfloat phase, Lfloat w, Lfloat slope_delta);
#endif
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_MATH_H_INCLUDED

//==============================================================================

