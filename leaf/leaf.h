/** BEGIN_JUCE_MODULE_DECLARATION
 
 ID:            leaf
 vendor:
 version:        0.0.1
 name:
 description:
 website:
 license:        
 
 dependencies:
 
 END_JUCE_MODULE_DECLARATION
 */
/*
 ==============================================================================
 
 leaf.h
 Created: 20 Jan 2017 12:07:26pm
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#ifndef LEAF_H_INCLUDED
#define LEAF_H_INCLUDED

#define LEAF_DEBUG 0

#if LEAF_DEBUG
#include "../LEAF_JUCEPlugin/JuceLibraryCode/JuceHeader.h"
#endif

#if _WIN32 || _WIN64

#include ".\Inc\leaf-global.h"
#include ".\Inc\leaf-math.h"
#include ".\Inc\leaf-mempool.h"
#include ".\Inc\leaf-tables.h"
#include ".\Inc\leaf-distortion.h"
#include ".\Inc\leaf-oscillators.h"
#include ".\Inc\leaf-filters.h"
#include ".\Inc\leaf-delay.h"
#include ".\Inc\leaf-reverb.h"
#include ".\Inc\leaf-effects.h"
#include ".\Inc\leaf-envelopes.h"
#include ".\Inc\leaf-dynamics.h"
#include ".\Inc\leaf-analysis.h"
#include ".\Inc\leaf-instruments.h"
#include ".\Inc\leaf-midi.h"
#include ".\Inc\leaf-sampling.h"
#include ".\Inc\leaf-physical.h"
#include ".\Inc\leaf-electrical.h"

#else

#include "./Inc/leaf-global.h"
#include "./Inc/leaf-math.h"
#include "./Inc/leaf-mempool.h"
#include "./Inc/leaf-tables.h"
#include "./Inc/leaf-distortion.h"
#include "./Inc/leaf-dynamics.h"
#include "./Inc/leaf-oscillators.h"
#include "./Inc/leaf-filters.h"
#include "./Inc/leaf-delay.h"
#include "./Inc/leaf-reverb.h"
#include "./Inc/leaf-effects.h"
#include "./Inc/leaf-envelopes.h"
#include "./Inc/leaf-analysis.h"
#include "./Inc/leaf-instruments.h"
#include "./Inc/leaf-midi.h"
#include "./Inc/leaf-sampling.h"
#include "./Inc/leaf-physical.h"
#include "./Inc/leaf-electrical.h"

#endif

/*! @mainpage LEAF
 *
 * @section intro_sec Introduction
 *
 * This is the work in progress documentation of LEAF.
 *
 * @section install_sec Installation
 *
 * @subsection step1 Step 1: Opening the box
 *
 * etc...
 */

/*!
 * @defgroup leaf LEAF
 * @defgroup oscillators Oscillators
 * @defgroup filters Filters
 * @defgroup delay Delay
 * @defgroup reverb Reverb
 * @defgroup distortion Distortion
 * @defgroup effects Effects
 * @defgroup envelopes Envelopes
 * @defgroup dynamics Dynamics
 * @defgroup analysis Analysis
 * @defgroup instruments Instruments
 * @defgroup midi MIDI
 * @defgroup sampling Sampling
 * @defgroup physical Physical Models
 * @defgroup electrical Electrical Models
 * @defgroup mempool Mempool
 * @defgroup math Math
 * @defgroup tables Tables
 */

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @ingroup leaf
     @{
     */
    
    //! Initialize the LEAF instance.
    /*!
     @param sampleRate The audio sample rate.
     @param blockSize The audio block size.
     @param memory A pointer to the memory that will make up the default mempool of a LEAF instance.
     @param memorySize The size of the memory that will make up the default mempool of a LEAF instance.
     @param random A pointer to a random number function. Should return a float >= 0 and < 1.
     */
    void        LEAF_init            (LEAF* const leaf, float sampleRate, int blockSize, char* memory, size_t memorySize, float(*random)(void));
    
    //! Set the sample rate of LEAF.
    /*!
     @param sampleRate The new audio sample rate.
     */
    void        LEAF_setSampleRate   (LEAF* const leaf, float sampleRate);
    
    //! Get the sample rate of LEAF.
    /*!
     @return The current sample rate as a float.
     */
    float       LEAF_getSampleRate   (LEAF* const leaf);
    
    //! The default callback function for LEAF errors.
    /*!
     @param errorType The type of the error that has occurred.
     */
    void        LEAF_defaultErrorCallback(LEAF* const leaf, LEAFErrorType errorType);
    
    void        LEAF_internalErrorCallback(LEAF* const leaf, LEAFErrorType whichone);
    
    //! Set the callback function for LEAF errors.
    /*!
     @param callback A pointer to the callback function.
     */
    void LEAF_setErrorCallback(LEAF* const leaf, void (*callback)(LEAF* const, LEAFErrorType));
    
    /*! @} */
    
#ifdef __cplusplus
}
#endif

#endif  // LEAF_H_INCLUDED

