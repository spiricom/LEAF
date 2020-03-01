/*==============================================================================

    leaf-tables.h
    Created: 4 Dec 2016 9:42:41pm
    Author:  Michael R Mulshine

==============================================================================*/

#ifndef LEAF_TABLES_H_INCLUDED
#define LEAF_TABLES_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
//==============================================================================

#include "leaf-math.h"

//==============================================================================


#define SINE_TABLE_SIZE 2048
#define SAW_TABLE_SIZE 2048
#define SQR_TABLE_SIZE 2048
#define TRI_TABLE_SIZE 2048
#define EXP_DECAY_TABLE_SIZE 65536
#define ATTACK_DECAY_INC_TABLE_SIZE 65536
#define TANH1_TABLE_SIZE 65536
#define DECAY_COEFF_TABLE_SIZE 4096
#define MTOF1_TABLE_SIZE 4096
#define FILTERTAN_TABLE_SIZE 4096
    
#define SHAPER1_TABLE_SIZE 65536
extern const float shaper1[SHAPER1_TABLE_SIZE];
    
#define COEFFS_SIZE 32
extern const float* firCoeffs[COEFFS_SIZE];
extern const float firNumTaps[COEFFS_SIZE];
extern const float fir2XLow[32];
extern const float fir4XLow[64];
extern const float fir8XLow[64];
extern const float fir16XLow[128];
extern const float fir32XLow[256];
extern const float fir64XLow[256];
extern const float fir2XHigh[128];
extern const float fir4XHigh[256];
extern const float fir8XHigh[256];
extern const float fir16XHigh[512];
extern const float fir32XHigh[512];
extern const float fir64XHigh[1024];

typedef enum TableName
{
    T20 = 0,
    T40,
    T80,
    T160,
    T320,
    T640,
    T1280,
    T2560,
    T5120,
    T10240,
    T20480,
    TableNameNil
} TableName;

// mtof lookup table based on input range [0.0,1.0) in 4096 increments - midi frequency values scaled between m25 and m134 (from the Snyderphonics DrumBox code)

extern const float exp_decay[EXP_DECAY_TABLE_SIZE];
extern const float attack_decay_inc[ATTACK_DECAY_INC_TABLE_SIZE];

extern const float filtertan[FILTERTAN_TABLE_SIZE];

extern const float mtof1[MTOF1_TABLE_SIZE];
extern const float decayCoeffTable[DECAY_COEFF_TABLE_SIZE];

extern const float tanh1[TANH1_TABLE_SIZE];

//==============================================================================

/* Sine wave table ripped from http://aquaticus.info/pwm-sine-wave. */
extern const float sinewave[SINE_TABLE_SIZE];

extern const float sawtooth[11][SAW_TABLE_SIZE];

extern const float triangle[11][TRI_TABLE_SIZE];

extern const float squarewave[11][SQR_TABLE_SIZE];

//==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_TABLES_H_INCLUDED

//==============================================================================
