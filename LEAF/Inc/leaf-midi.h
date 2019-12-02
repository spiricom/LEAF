/*==============================================================================
 
 leaf-midi.h
 Created: 30 Nov 2018 11:29:26am
 Author:  airship
 
 ==============================================================================*/

#ifndef LEAF_MIDI_H_INCLUDED
#define LEAF_MIDI_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
    
    //==============================================================================
    
#include "leaf-global.h"
#include "leaf-mempool.h"
#include "leaf-math.h"
#include "leaf-envelopes.h"
    
    //==============================================================================
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    // STACK implementation (fixed size)
#define STACK_SIZE 128
    typedef struct _tStack
    {
        int data[STACK_SIZE];
        uint16_t pos;
        uint16_t size;
        uint16_t capacity;
        oBool ordered;
        
    } _tStack;
    
    typedef _tStack* tStack;
    
    void    tStack_init                 (tStack* const);
    void    tStack_free                 (tStack* const);
    
    void    tStack_setCapacity          (tStack* const, uint16_t cap);
    int     tStack_addIfNotAlreadyThere (tStack* const, uint16_t item);
    void    tStack_add                  (tStack* const, uint16_t item);
    int     tStack_remove               (tStack* const, uint16_t item);
    void    tStack_clear                (tStack* const);
    int     tStack_first                (tStack* const);
    int     tStack_getSize              (tStack* const);
    int     tStack_contains             (tStack* const, uint16_t item);
    int     tStack_next                 (tStack* const);
    int     tStack_get                  (tStack* const, int which);
    
    /* tPoly */
    typedef struct _tPoly
    {
        tStack stack;
        tStack orderStack;
        
        tRamp* ramps;
        float* rampVals;
        oBool* firstReceived;
        float glideTime;
        oBool pitchGlideIsActive;
        
        int numVoices;
        int maxNumVoices;
        
        //int voices[POLY_NUM_MAX_VOICES][2];
        int** voices;
        
        int notes[128][2];
        
        int CCs[128];
        
        uint8_t CCsRaw[128];
        
        int lastVoiceToChange;
        
        float pitchBend;
        tRamp pitchBendRamp;
        
        int currentNote;
        int currentVoice;
        int currentVelocity;
        int maxLength;
        
    } _tPoly;
    
    typedef _tPoly* tPoly;
    
    /* MPoly*/
    void    tPoly_init(tPoly* const, int maxNumVoices);
    void    tPoly_free(tPoly* const);
    
    //ADDING A NOTE
    int     tPoly_noteOn(tPoly* const, int note, uint8_t vel);
    int     tPoly_noteOff(tPoly* const, uint8_t note);
    void    tPoly_orderedAddToStack(tPoly* const, uint8_t noteVal);
    void    tPoly_setNumVoices(tPoly* const, uint8_t numVoices);
    
    void    tPoly_setPitchBend(tPoly* const, float pitchBend);
    void    tPoly_setPitchGlideActive(tPoly* const, oBool isActive);
    void    tPoly_setPitchGlideTime(tPoly* const, float t);
    void    tPoly_tickPitch(tPoly* const);
    void    tPoly_tickPitchGlide(tPoly* const);
    void    tPoly_tickPitchBend(tPoly* const);
    
    int     tPoly_getNumVoices(tPoly* const);
    float   tPoly_getPitch(tPoly* const, uint8_t voice);
    int     tPoly_getKey(tPoly* const, uint8_t voice);
    int     tPoly_getVelocity(tPoly* const, uint8_t voice);
    int     tPoly_isOn(tPoly* const, uint8_t voice);
    
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_MIDI_H_INCLUDED

//==============================================================================


