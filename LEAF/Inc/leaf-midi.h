/*
  ==============================================================================

    leaf-midi.h
    Created: 30 Nov 2018 11:29:26am
    Author:  airship

  ==============================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif
    
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#include "leaf-globals.h"
#include "leaf-math.h"

#include "leaf-utilities.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

typedef struct _tMidiNote
{
    uint8_t pitch;
    uint8_t velocity;
    oBool on;
} tMidiNote;

typedef struct _tMidiNode tMidiNode;

typedef struct _tMidiNode
{
    tMidiNode* prev;
    tMidiNode* next;
    tMidiNote midiNote;
    
} tMidiNode;

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* Polyphonic Handler */
typedef struct _tPoly
{
    tMidiNode midiNodes[128];
    tMidiNode* onListFirst;
    tMidiNode* offListFirst;
    
} tPoly;

void        tPoly_init(tPoly* const);
void        tPoly_free(tPoly* const);

tMidiNote*  tPoly_getMidiNote(tPoly* const, int8_t voiceIndex);
void        tPoly_noteOn(tPoly* poly, int midiNoteNumber, float velocity);
void        tPoly_noteOff(tPoly* poly, int midiNoteNumber);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

/* tMPoly */
typedef struct _tMPoly
{
    tStack* stack;
    tStack* orderStack;
    
    tRamp* ramp[MPOLY_NUM_MAX_VOICES];
    
    float rampVals[MPOLY_NUM_MAX_VOICES];
    uint8_t firstReceived[MPOLY_NUM_MAX_VOICES];
    float glideTime;
    
    int numVoices;
    
    int voices[MPOLY_NUM_MAX_VOICES][2];
    
    int notes[128][2];
    
    int CCs[128];
    
    uint8_t CCsRaw[128];
    
    int lastVoiceToChange;
    
    int32_t pitchBend;
    float pitchBendAmount;
    
    int currentNote;
    int currentVoice;
    int currentVelocity;
    int maxLength;
    
} tMPoly;

/* MPoly*/
void    tMPoly_init(tMPoly* const, int numVoices);
void    tMPoly_free(tMPoly* const);

void    tMPoly_tick(tMPoly* const);

//ADDING A NOTE
int     tMPoly_noteOn(tMPoly* const, int note, uint8_t vel);
int     tMPoly_noteOff(tMPoly* const, uint8_t note);
void    tMPoly_orderedAddToStack(tMPoly* const, uint8_t noteVal);
void    tMPoly_pitchBend(tMPoly* const, int pitchBend);
void    tMPoly_setNumVoices(tMPoly* const, uint8_t numVoices);
void    tMPoly_setPitchGlideTime(tMPoly* const, float t);
int     tMPoly_getNumVoices(tMPoly* const);
float   tMPoly_getPitch(tMPoly* const, uint8_t voice);
int     tMPoly_getVelocity(tMPoly* const, uint8_t voice);
int     tMPoly_isOn(tMPoly* const, uint8_t voice);

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
#ifdef __cplusplus
}
#endif

