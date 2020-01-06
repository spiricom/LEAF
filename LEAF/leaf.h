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

#ifdef __cplusplus
extern "C" {
#endif
    
void        LEAF_init            (float sampleRate, int blocksize, char* memory, size_t memorysize, float(*randomNumberFunction)(void));
void        LEAF_setSampleRate   (float sampleRate);
float       LEAF_getSampleRate   (void);

#ifdef __cplusplus
}
#endif

#endif  // LEAF_H_INCLUDED
