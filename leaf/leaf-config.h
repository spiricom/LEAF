/*
  ==============================================================================

    leaf-config.h
    Created: 18 Dec 2020 1:24:49pm
    Author:  Matthew Wang

  ==============================================================================
*/

#ifndef LEAF_CONFIG_H_INCLUDED
#define LEAF_CONFIG_H_INCLUDED

/*!
 @file leaf-config.h
 @brief LEAF configuration file. Contains defines for controlling inclusion of various tables.
 */

//==============================================================================

//! Include FIR tables required to use tOversampler and tWaveTableS which uses tOversampler. 
#define LEAF_INCLUDE_OVERSAMPLER_TABLES 1

// Unused
#define LEAF_INCLUDE_SHAPER_TABLE 0

// Unused
#define LEAF_INCLUDE_MTOF_TABLE 0

//! Include table required to use tEfficientSVF.
#define LEAF_INCLUDE_FILTERTAN_TABLE 1

// Unused
#define LEAF_INCLUDE_TANH_TABLE 0

// Unused
#define LEAF_INCLUDE_ADC_TABLE 0

//! Include tables required to use tEnvelope and tADSR (but not tADSRT and tADSRS).
#define LEAF_INCLUDE_ADSR_TABLES 1

//! Include wave table required to use tCycle.
#define LEAF_INCLUDE_SINE_TABLE 1

//! Include wave table required to use tTriangle.
#define LEAF_INCLUDE_TRIANGLE_TABLE 1

//! Include wave table required to use tSquare.
#define LEAF_INCLUDE_SQUARE_TABLE 1

//! Include wave table required to use tSawtooth.
#define LEAF_INCLUDE_SAWTOOTH_TABLE 1

//! Include tables for minblep insertion, required for all tMB objects.
#define LEAF_INCLUDE_MINBLEP_TABLES 1

#define LEAF_NO_DENORMAL_CHECK 0

#define LEAF_USE_CMSIS 0

//! Use stdlib malloc() and free() internally instead of LEAF's normal mempool behavior for when you want to avoid being limited to and managing mempool a fixed mempool size. Usage of all object remains essentially the same.
#define LEAF_USE_DYNAMIC_ALLOCATION 0

//==============================================================================

#endif // LEAF_CONFIG_H_INCLUDED
