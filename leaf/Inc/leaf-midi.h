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
    
    /*!
     * @internal
     * Header.
     * @include basic-oscillators.h
     * @example basic-oscillators.c
     * An example.
     */
    
    //==============================================================================
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

    /*!
     @defgroup tstack tStack
     @ingroup midi
     @brief A basic stack of integers with a fixed capacity of 128, used by tPoly to keep track of MIDI notes.
     @{
     
     @fn void    tStack_init                 (tStack* const stack, LEAF* const leaf)
     @brief Initialize a tStack to the default mempool of a LEAF instance.
     @param stack A pointer to the tStack to initialize.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tStack_initToPool           (tStack* const stack, tMempool* const mempool)
     @brief Initialize a tStack to a specified mempool.
     @param stack A pointer to the tStack to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tStack_free                 (tStack* const stack)
     @brief Free a tStack from its mempool.
     @param stack A pointer to the tStack to free.
     
     @fn void    tStack_setCapacity          (tStack* const stack, uint16_t cap)
     @brief Set the capacity of the stack.
     @param stack A pointer to the relevant tStack.
     @param cap The new capacity.
     
     @fn int     tStack_addIfNotAlreadyThere (tStack* const stack, uint16_t item)
     @brief Add a value to the stack only if that value is not already in the stack.
     @param stack A pointer to the relevant tStack.
     @param item The value to be added.
     
     @fn void    tStack_add                  (tStack* const stack, uint16_t item)
     @brief Add a value to the stack.
     @param stack A pointer to the relevant tStack.
     @param item The value to be added.
     
     @fn int     tStack_remove               (tStack* const stack, uint16_t item)
     @brief Remove a single instance of a value from the stack.
     @param stack A pointer to the relevant tStack.
     @param item The value to be removed.
     
     @fn void    tStack_clear                (tStack* const stack)
     @brief Clear the stack.
     @param stack A pointer to the relevant tStack.
     
     @fn int     tStack_first                (tStack* const stack)
     @brief Get the first value in the stack.
     @param stack A pointer to the relevant tStack.
     @return The first value in the stack.
     
     @fn int     tStack_getSize              (tStack* const stack)
     @brief Get the current size of the stack.
     @param stack A pointer to the relevant tStack.
     @return The current size of the stack.
     
     @fn int     tStack_contains             (tStack* const stack, uint16_t item)
     @brief Check if the stack contains a value, and if it does, get the index of that value.
     @param stack A pointer to the relevant tStack.
     @param item The value to check against the stack.
     @return The index of the value or -1 if the stack does not contain the value.
     
     @fn int     tStack_next                 (tStack* const stack)
     @brief Get the next value in the stack, starting from the earliest added values.
     @param stack A pointer to the relevant tStack.
     @return The next value in the stack or -1 if there are no values in the stack.
     
     @fn int     tStack_get                  (tStack* const stack, int index)
     @brief Get the value at a given index of the stack.
     @param stack A pointer to the relevant tStack.
     @param index The index of the stack from which to get a value.
     @return The value at the given index.
     
     @} */
    
#define STACK_SIZE 128
    typedef struct _tStack
    {
        
        tMempool mempool;
        int data[STACK_SIZE];
        uint16_t pos;
        uint16_t size;
        uint16_t capacity;
        int ordered;
    } _tStack;
    
    typedef _tStack* tStack;
    
    void    tStack_init                 (tStack* const stack, LEAF* const leaf);
    void    tStack_initToPool           (tStack* const stack, tMempool* const pool);
    void    tStack_free                 (tStack* const stack);
    
    void    tStack_setCapacity          (tStack const stack, uint16_t cap);
    int     tStack_addIfNotAlreadyThere (tStack const stack, uint16_t item);
    void    tStack_add                  (tStack const stack, uint16_t item);
    int     tStack_remove               (tStack const stack, uint16_t item);
    void    tStack_clear                (tStack const stack);
    int     tStack_first                (tStack const stack);
    int     tStack_getSize              (tStack const stack);
    int     tStack_contains             (tStack const stack, uint16_t item);
    int     tStack_next                 (tStack const stack);
    int     tStack_get                  (tStack const stack, int index);
    
    /*! @} 
     @defgroup tpoly tPoly
     @ingroup midi
     @brief Polyphony handler with glide and pitch bend options.
     @{
     
     @fn void    tPoly_init                  (tPoly* const poly, int maxNumVoices, LEAF* const leaf)
     @brief Initialize a tPoly to the default mempool of a LEAF instance.
     @param poly A pointer to the tPoly to initialize.
     @param maxNumVoices The maximum number of voices this tPoly can handle at once.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tPoly_initToPool            (tPoly* const poly, int maxNumVoices, tMempool* const pool)
     @brief Initialize a tPoly to a specified mempool.
     @param poly A pointer to the tPoly to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tPoly_free                  (tPoly* const poly)
     @brief Free a tPoly from its mempool.
     @param poly A pointer to the tPoly to free.
     
     @fn int     tPoly_noteOn                (tPoly* const poly, int note, uint8_t vel)
     @brief Add a note with a given velocity to the poly handler.
     @param poly A pointer to the relevant tPoly.
     @param note The MIDI note number to add.
     @param vel The MIDI velocity of the note to add.
     @return The voice that will play the note.
     
     @fn int     tPoly_noteOff               (tPoly* const poly, uint8_t note)
     @brief Remove a note from the poly handler.
     @param poly A pointer to the relevant tPoly.
     @param note The MIDI note number to remove.
     @return The voice that was playing the removed note.
     
     @fn void    tPoly_orderedAddToStack     (tPoly* const poly, uint8_t note)
     @brief
     @param
     
     @fn void    tPoly_setNumVoices          (tPoly* const poly, uint8_t numVoices)
     @brief Set the number of voices available to play notes.
     @param poly A pointer to the relevant tPoly.
     @param numVoices The new number of available voices. Cannot be greater than the max number voices given in tPoly_init().
     
     @fn void    tPoly_setPitchGlideActive   (tPoly* const poly, int isActive)
     @brief Set whether pitch glide over note changes in voices is active.
     @param poly A pointer to the relevant tPoly.
     @param isActive Whether pitch glide should be active or not.
     
     @fn void    tPoly_setPitchGlideTime     (tPoly* const poly, Lfloat t)
     @brief Set how long pitch glide over note changes in voices takes.
     @param poly A pointer to the relevant tPoly.
     @param t The time to glide in milliseconds.
     
     @fn void    tPoly_setPitchBend          (tPoly* const poly, Lfloat pitchBend)
     @brief Set the amount of pitch bend
     @param poly A pointer to the relevant tPoly.
     @param pitchBend The new amount of pitch bend.
     
     @fn void     tPoly_setBendGlideTime        (tPoly* const poly, Lfloat t)
     @brief
     @param
     
     @fn void     tPoly_setBendSamplesPerTick    (tPoly* const poly, Lfloat t)
     @brief
     @param
     
     @fn void    tPoly_tickPitch             (tPoly* const poly)
     @brief Execute all tick-rate changes in the poly handler's pitch, including glide and bend.
     @param poly A pointer to the relevant tPoly.
     
     @fn void    tPoly_tickPitchGlide        (tPoly* const poly)
     @brief Execute the tick-rate change of the poly handler's pitch glide.
     @param poly A pointer to the relevant tPoly.
     
     @fn void    tPoly_tickPitchBend         (tPoly* const poly)
     @brief Execute the tick-rate change of the poly handler's pitch bend.
     @param poly A pointer to the relevant tPoly.
     
     @fn int     tPoly_getNumVoices          (tPoly* const poly)
     @brief Get the current number of voices available to play notes.
     @param poly A pointer to the relevant tPoly.
     @return The current number of voices available to play notes.
     
     @fn int     tPoly_getNumActiveVoices    (tPoly* const poly)
     @brief Get the number of voices currently playing notes.
     @param poly A pointer to the relevant tPoly.
     @return The number of voices currently playing notes.
     
     @fn Lfloat   tPoly_getPitch              (tPoly* const poly, uint8_t voice)
     @brief Get the current pitch of a given voice.
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the pitch of.
     @return The current pitch of the given voice as a fractional MIDI note number.
     
     @fn int     tPoly_getKey                (tPoly* const poly, uint8_t voice)
     @brief Get the current MIDI note number of a given voice.
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the MIDI note number of.
     @return The MIDI note number of the given voice.
     
     @fn int     tPoly_getVelocity           (tPoly* const poly, uint8_t voice)
     @brief Get the current MIDI velocity of a given voice.
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the MIDI velocity of.
     @return The current MIDI velocity of the given voice.
     
     @fn int     tPoly_isOn                  (tPoly* const poly, uint8_t voice)
     @brief Get the current play state of a given voice.
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the state of.
     @return The current play state of the given voice.
     
     @} */

    typedef struct _tPoly
    {
        
        tMempool mempool;
        
        tStack stack;
        tStack orderStack;
        
        tRamp* ramps;
        Lfloat* rampVals;
        int* firstReceived;
        Lfloat glideTime;
        int pitchGlideIsActive;
        
        int numVoices;
        int maxNumVoices;
        
        //int voices[POLY_NUM_MAX_VOICES][2];
        int** voices;
        
        int notes[128][2];
        
        int CCs[128];
        
        uint8_t CCsRaw[128];
        
        int lastVoiceToChange;
        
        Lfloat pitchBend;
        tRamp pitchBendRamp;
        
        int currentNote;
        int currentVoice;
        int currentVelocity;
        int maxLength;
    } _tPoly;
    
    typedef _tPoly* tPoly;
    
    void    tPoly_init                  (tPoly* const poly, int maxNumVoices, LEAF* const leaf);
    void    tPoly_initToPool            (tPoly* const poly, int maxNumVoices, tMempool* const pool);
    void    tPoly_free                  (tPoly* const poly);

    void    tPoly_tickPitch             (tPoly const poly);
    void    tPoly_tickPitchGlide        (tPoly const poly);
    void    tPoly_tickPitchBend         (tPoly const poly);

    int     tPoly_noteOn                (tPoly const poly, int note, uint8_t vel);
    int     tPoly_noteOff               (tPoly const poly, uint8_t note);
    void    tPoly_orderedAddToStack     (tPoly const poly, uint8_t note);
    void    tPoly_setNumVoices          (tPoly const poly, uint8_t numVoices);
    void    tPoly_setPitchGlideActive   (tPoly const poly, int isActive);
    void    tPoly_setPitchGlideTime     (tPoly const poly, Lfloat t);
    void    tPoly_setPitchBend          (tPoly const poly, Lfloat pitchBend);
    void 	tPoly_setBendGlideTime		(tPoly const poly, Lfloat t);
    void 	tPoly_setBendSamplesPerTick	(tPoly const poly, Lfloat t);
    int     tPoly_getNumVoices          (tPoly const poly);
    int     tPoly_getNumActiveVoices    (tPoly const poly);
    Lfloat  tPoly_getPitch              (tPoly const poly, uint8_t voice);
    int     tPoly_getKey                (tPoly const poly, uint8_t voice);
    int     tPoly_getVelocity           (tPoly const poly, uint8_t voice);
    int     tPoly_isOn                  (tPoly const poly, uint8_t voice);
    void    tPoly_setSampleRate         (tPoly const poly, Lfloat sr);
    
    //==============================================================================

    /*! @}
     @defgroup tsimplepoly tSimplePoly
     @ingroup midi
     @brief Polyphony handler.
     @{
     
     @fn void    tSimplePoly_init                  (tSimplePoly* const poly, int maxNumVoices, LEAF* const leaf)
     @brief Initialize a tSimplePoly to the default mempool of a LEAF instance.
     @param poly A pointer to the tSimplePoly to initialize.
     @param maxNumVoices The maximum number of voices this tSimplePoly can handle at once.
     @param leaf A pointer to the leaf instance.
     
     @fn void    tSimplePoly_initToPool            (tSimplePoly* const poly, int maxNumVoices, tMempool* const pool)
     @brief Initialize a tSimplePoly to a specified mempool.
     @param poly A pointer to the tSimplePoly to initialize.
     @param mempool A pointer to the tMempool to use.
     
     @fn void    tSimplePoly_free                  (tSimplePoly* const poly)
     @brief Free a tSimplePoly from its mempool.
     @param poly A pointer to the tSimplePoly to free.
     
     @fn int     tSimplePoly_noteOn                (tSimplePoly* const poly, int note, uint8_t vel)
     @brief Add a note with a given velocity to the poly handler.
     @param poly A pointer to the relevant tSimplePoly.
     @param note The MIDI note number to add.
     @param vel The MIDI velocity of the note to add.
     @return The voice that will play the note.
     
     @fn int     tSimplePoly_noteOff               (tSimplePoly* const poly, uint8_t note)
     @brief Remove a note from the poly handler.
     @param poly A pointer to the relevant tSimplePoly.
     @param note The MIDI note number to remove.
     @return The voice that was playing the removed note.
     
     @fn void tSimplePoly_deactivateVoice(tSimplePoly* const polyh, uint8_t voice)
     @brief
     @param
     
     @fn int tSimplePoly_markPendingNoteOff(tSimplePoly* const polyh, uint8_t note)
     @brief
     @param
     
     @fn int tSimplePoly_findVoiceAssignedToNote(tSimplePoly* const polyh, uint8_t note)
     @brief Find if there is a voice with that note -- useful for note offs where you want to wait to remove it from the poly until the release phase of the envelope is finished
     @param
     
     @fn void    tSimplePoly_setNumVoices          (tSimplePoly* const poly, uint8_t numVoices)
     @brief Set the number of voices available to play notes.
     @param poly A pointer to the relevant tSimplePoly.
     @param numVoices The new number of available voices. Cannot be greater than the max number voices given on initialization.
     
     @fn int     tSimplePoly_getNumVoices          (tSimplePoly* const poly)
     @brief Get the current number of voices available to play notes.
     @param poly A pointer to the relevant tSimplePoly.
     @return The current number of voices available to play notes.
     
     @fn int     tSimplePoly_getNumActiveVoices    (tSimplePoly* const poly)
     @brief Get the number of voices currently playing notes.
     @param poly A pointer to the relevant tSimplePoly.
     @return The number of voices currently playing notes.
     
     @fn int   tSimplePoly_getPitch              (tSimplePoly* const poly, uint8_t voice)
     @brief Get the current MIDI note number of a given voice.
     @param poly A pointer to the relevant tSimplePoly.
     @param voice The voice to get the MIDI note number of.
     @return The MIDI note number of the given voice.
     
     @fn int tSimplePoly_getPitchAndCheckActive(tSimplePoly* const polyh, uint8_t voice)
     @brief Returns negative one if the voice is inactive
     
     @fn int     tSimplePoly_getVelocity           (tSimplePoly* const poly, uint8_t voice)
     @brief Get the current MIDI velocity of a given voice.
     @param poly A pointer to the relevant tSimplePoly.
     @param voice The voice to get the MIDI velocity of.
     @return The current MIDI velocity of the given voice.
     
     @fn int     tSimplePoly_isOn                  (tSimplePoly* const poly, uint8_t voice)
     @brief Get the current play state of a given voice.
     @param poly A pointer to the relevant tSimplePoly.
     @param voice The voice to get the state of.
     @return The current play state of the given voice.
     
     @} */
    typedef struct _tSimplePoly
    {
        tMempool mempool;
        
        tStack stack;

        int numVoices;
        int maxNumVoices;
        int** voices;
        int stealing_on;
        int recover_stolen;

        int notes[128][2];
    } _tSimplePoly;

    typedef _tSimplePoly* tSimplePoly;

    void    tSimplePoly_init                    (tSimplePoly* const poly, int maxNumVoices, LEAF* const leaf);
    void    tSimplePoly_initToPool              (tSimplePoly* const poly, int maxNumVoices, tMempool* const pool);
    void    tSimplePoly_free                    (tSimplePoly* const poly);
    
    int     tSimplePoly_noteOn                  (tSimplePoly const poly, int note, uint8_t vel);
    int     tSimplePoly_noteOff                 (tSimplePoly const poly, uint8_t note);
    void    tSimplePoly_deactivateVoice         (tSimplePoly const polyh, uint8_t voice);
    int     tSimplePoly_markPendingNoteOff      (tSimplePoly const polyh, uint8_t note);
    int     tSimplePoly_findVoiceAssignedToNote (tSimplePoly const polyh, uint8_t note);
    void    tSimplePoly_setNumVoices            (tSimplePoly const poly, uint8_t numVoices);
    int     tSimplePoly_getNumVoices            (tSimplePoly const poly);
    int     tSimplePoly_getNumActiveVoices      (tSimplePoly const poly);
    int     tSimplePoly_getPitch                (tSimplePoly const poly, uint8_t voice);
    int     tSimplePoly_getPitchAndCheckActive  (tSimplePoly const polyh, uint8_t voice);
    int     tSimplePoly_getVelocity             (tSimplePoly const poly, uint8_t voice);
    int     tSimplePoly_isOn                    (tSimplePoly const poly, uint8_t voice);
    int tSimplePoly_allNotesOff(tSimplePoly const poly);
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_MIDI_H_INCLUDED

//==============================================================================


