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


typedef struct _tlookuptable
{
    uint32_t tableSize;
    float* table;
    tMempool mempool;
} _tlookuptable;
typedef _tlookuptable* tlookuptable;

void tLookupTable_init(tlookuptable*  const tlookuptable, float start, float end, float center, int size, LEAF* const leaf);

void tLookupTable_initToPool(tlookuptable*  const tlookuptable, float start, float end, float center, int size, tMempool* const mempool);

    /*!
     @ingroup tables
     @{
     */
    
#define COEFFS_SIZE 32
    extern const Lfloat* __leaf_tableref_firCoeffs[COEFFS_SIZE];
    extern const uint_fast16_t __leaf_tablesize_firNumTaps[COEFFS_SIZE];
    extern const Lfloat __leaf_table_fir2XLow[32];
    extern const Lfloat __leaf_table_fir4XLow[64];
    extern const Lfloat __leaf_table_fir8XLow[64];
    extern const Lfloat __leaf_table_fir16XLow[128];
    extern const Lfloat __leaf_table_fir32XLow[256];
    extern const Lfloat __leaf_table_fir64XLow[256];
    extern const Lfloat __leaf_table_fir2XHigh[128];
    extern const Lfloat __leaf_table_fir4XHigh[256];
    extern const Lfloat __leaf_table_fir8XHigh[256];
    extern const Lfloat __leaf_table_fir16XHigh[512];
    extern const Lfloat __leaf_table_fir32XHigh[512];
    extern const Lfloat __leaf_table_fir64XHigh[1024];
    
//    typedef enum TableName
//    {
//        T20 = 0,
//        T40,
//        T80,
//        T160,
//        T320,
//        T640,
//        T1280,
//        T2560,
//        T5120,
//        T10240,
//        T20480,
//        TableNameNil
//    } TableName;
    
    
#define SHAPER1_TABLE_SIZE 65536
    extern const Lfloat __leaf_table_shaper1[SHAPER1_TABLE_SIZE];
   
    // mtof lookup table based on input range [0.0,1.0) in 4096 increments - midi frequency values scaled between m25 and m134 (from the Snyderphonics DrumBox code)
#define MTOF1_TABLE_SIZE 4096
    extern const Lfloat __leaf_table_mtof1[MTOF1_TABLE_SIZE];

#define EXP_DECAY_TABLE_SIZE 65536
    extern const Lfloat __leaf_table_exp_decay[EXP_DECAY_TABLE_SIZE];
    
#define ATTACK_DECAY_INC_TABLE_SIZE 65536
    extern const Lfloat __leaf_table_attack_decay_inc[ATTACK_DECAY_INC_TABLE_SIZE];
    
#define FILTERTAN_TABLE_SIZE 4096
    //extern const Lfloat __leaf_table_filtertan[FILTERTAN_TABLE_SIZE];
    extern const Lfloat __filterTanhTable_48000[4096];
    extern const Lfloat __filterTanhTable_96000[4096];
#define TANH1_TABLE_SIZE 65536
    extern const Lfloat __leaf_table_tanh1[TANH1_TABLE_SIZE];
    
    //==============================================================================
    
    /* Sine wave table ripped from http://aquaticus.info/pwm-sine-wave. */
#define SINE_TABLE_SIZE 2048
    extern const Lfloat __leaf_table_sinewave[SINE_TABLE_SIZE];
    
#define TRI_TABLE_SIZE 2048
    extern const Lfloat __leaf_table_triangle[11][TRI_TABLE_SIZE];
    
#define SQR_TABLE_SIZE 2048
    extern const Lfloat __leaf_table_squarewave[11][SQR_TABLE_SIZE];
    
#define SAW_TABLE_SIZE 2048
    extern const Lfloat __leaf_table_sawtooth[11][SAW_TABLE_SIZE];
    
    //==============================================================================
    
    
    // From https://github.com/MrBlueXav/Dekrispator_v2 minblep_tables.h
    
    /* minBLEP constants */
    /* minBLEP table oversampling factor (must be a power of two): */
#define MINBLEP_PHASES          64
    /* MINBLEP_PHASES minus one: */
#define MINBLEP_PHASE_MASK      63
    /* length in samples of (truncated) step discontinuity delta: */
#define STEP_DD_PULSE_LENGTH    72
    /* length in samples of (truncated) slope discontinuity delta: */
#define SLOPE_DD_PULSE_LENGTH   71
    /* the longer of the two above: */
#define LONGEST_DD_PULSE_LENGTH STEP_DD_PULSE_LENGTH
    /* delay between start of DD pulse and the discontinuity, in samples: */
#define DD_SAMPLE_DELAY          4
    
    typedef struct { Lfloat value, delta; } Lfloat_value_delta;
    
    /* in minblep_tables.c: */
    extern const Lfloat_value_delta step_dd_table[];
    extern const  Lfloat             slope_dd_table[];
    
    /*! @} */
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_TABLES_H_INCLUDED

//==============================================================================

