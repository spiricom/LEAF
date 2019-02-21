/*
  ==============================================================================

    leaf-formant.h
    Created: 30 Nov 2018 11:03:37am
    Author:  airship

  ==============================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#include "leaf-globals.h"
#include "leaf-math.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#define FORD 7
#define FORMANT_BUFFER_SIZE 2048

typedef struct _tFormantShifter
{
    int ford;
    float falph;
    float flamb;
    float fk[FORD];
    float fb[FORD];
    float fc[FORD];
    float frb[FORD];
    float frc[FORD];
    float fsig[FORD];
    float fsmooth[FORD];
    float fhp;
    float flp;
    float flpa;
    float fbuff[FORD][FORMANT_BUFFER_SIZE];
    float ftvec[FORD];
    float fmute;
    float fmutealph;
    unsigned int cbi;
    
} tFormantShifter;

void        tFormantShifter_init            (tFormantShifter* const);
void        tFormantShifter_free            (tFormantShifter* const);

float       tFormantShifter_tick            (tFormantShifter* const, float input, float fwarp);
float       tFormantShifter_remove          (tFormantShifter* const, float input);
float       tFormantShifter_add             (tFormantShifter* const, float input, float fwarp);
void        tFormantShifter_ioSamples       (tFormantShifter* const, float* in, float* out, int size, float fwarp);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
#ifdef __cplusplus
}
#endif

