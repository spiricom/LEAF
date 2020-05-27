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
     * @defgroup tstack tStack
     * @ingroup midi
     * @brief A basic stack of integers with a fixed max size of 128, used by tPoly to keep track of MIDI notes.
     * @{
     */
    
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
    
    //! Initialize a tStack to the default LEAF mempool.
    /*!
     @param stack A pointer to the tStack to be initialized.
     */
    void    tStack_init                 (tStack* const stack);
    
    
    //! Free a tStack from the default LEAF mempool.
    /*!
     @param stack A pointer to the tStack to be freed.
     */
    void    tStack_free                 (tStack* const stack);
    
    
    //! Initialize a tStack to a specified mempool.
    /*!
     @param stack A pointer to the tStack to be initialized.
     @param pool A pointer to the tMempool to which the tStack should be initialized.
     */
    void    tStack_initToPool           (tStack* const stack, tMempool* const pool);
    
    
    //! Free a tStack from a specified mempool.
    /*!
     @param stack A pointer to the tStack to be freed.
     @param pool A pointer to the tMempool from which the tStack should be freed.
     */
    void    tStack_freeFromPool         (tStack* const stack, tMempool* const pool);
    
    //! Set the capacity of the stack.
    /*!
     @param stack A pointer to the relevant tStack.
     @param cap The new capacity.
     */
    void    tStack_setCapacity          (tStack* const stack, uint16_t cap);
    
    
    //! Add a value to the stack only if that value is not already in the stack.
    /*!
     @param stack A pointer to the relevant tStack.
     @param item The value to be added.
     */
    int     tStack_addIfNotAlreadyThere (tStack* const stack, uint16_t item);
    
    
    //! Add a value to the stack.
    /*!
     @param stack A pointer to the relevant tStack.
     @param item The value to be added.
     */
    void    tStack_add                  (tStack* const stack, uint16_t item);
    
    
    //! Remove a single instance of a value from the stack.
    /*!
     @param stack A pointer to the relevant tStack.
     @param item The value to be removed.
     */
    int     tStack_remove               (tStack* const stack, uint16_t item);
    
    
    //! Clear the stack.
    /*!
     @param stack A pointer to the relevant tStack.
     */
    void    tStack_clear                (tStack* const stack);
    
    
    //! Get the first value in the stack.
    /*!
     @param stack A pointer to the relevant tStack.
     @return The first value in the stack.
     */
    int     tStack_first                (tStack* const stack);
    
    
    //! Get the current size of the stack.
    /*!
     @param stack A pointer to the relevant tStack.
     @return The current size of the stack.
     */
    int     tStack_getSize              (tStack* const stack);
    
    //! Check if the stack contains a value, and if it does, get the index of that value.
    /*!
     @param stack A pointer to the relevant tStack.
     @param item The value to check against the stack.
     @return The index of the value or -1 if the stack does not contain the value.
     */
    int     tStack_contains             (tStack* const stack, uint16_t item);
    
    //! Get the next value in the stack, starting from the earliest added values.
    /*!
     @param stack A pointer to the relevant tStack.
     @return The next value in the stack or -1 if there are no values in the stack.
     */
    int     tStack_next                 (tStack* const stack);
    
    //! Get the value at a given index of the stack.
    /*!
     @param stack A pointer to the relevant tStack.
     @param index The index of the stack from which to get a value.
     @return The value at the given index.
     */
    int     tStack_get                  (tStack* const stack, int index);
    
    /*! @} */
    
    /*!
     * @defgroup tpoly tPoly
     * @ingroup midi
     * @brief An object for polyphonic handling.
     * @{
     */
    
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
    //! Initialize a tPoly to the default LEAF mempool.
    /*!
     @param poly A pointer to the tPoly to be initialized.
     @param maxNumVoices The maximum number of voices this tPoly can handle at once.
     */
    void    tPoly_init                  (tPoly* const poly, int maxNumVoices);
    
    
    //! Free a tPoly from the default LEAF mempool.
    /*!
     @param poly A pointer to the tPoly to be freed.
     */
    void    tPoly_free                  (tPoly* const poly);
    
    
    //! Initialize a tPoly to a specified mempool.
    /*!
     @param poly A pointer to the tPoly to be initialized.
     @param pool A pointer to the tMempool to which the tPoly should be initialized.
     */
    void    tPoly_initToPool            (tPoly* const poly, int maxNumVoices, tMempool* const pool);
    
    
    //! Free a tPoly from a specified mempool.
    /*!
     @param poly A pointer to the tPoly to be freed.
     @param pool A pointer to the tMempool from which the tPoly should be freed.
     */
    void    tPoly_freeFromPool          (tPoly* const poly, tMempool* const pool);
    
    //! Add a note with a given velocity to the poly handler.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param note The MIDI note number to add.
     @param vel The MIDI velocity of the note to add.
     @return The voice that will play the note.
     */
    int     tPoly_noteOn                (tPoly* const poly, int note, uint8_t vel);
    
    
    //! Remove a note from the poly handler.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param note The MIDI note number to remove.
     @return The voice that was playing the removed note.
     */
    int     tPoly_noteOff               (tPoly* const poly, uint8_t note);
    
    void    tPoly_orderedAddToStack     (tPoly* const poly, uint8_t note);
    
    
    //! Set the number of voices available to play notes.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param numVoices The new number of available voices. Cannot be greater than the max number voices given in tPoly_init().
     */
    void    tPoly_setNumVoices          (tPoly* const poly, uint8_t numVoices);
    
    //! Set whether pitch glide over note changes in voices is active.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param isActive Whether pitch glide should be active or not.
     */
    void    tPoly_setPitchGlideActive   (tPoly* const poly, oBool isActive);
    
    //! Set how long pitch glide over note changes in voices takes.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param t The time to glide in milliseconds.
     */
    void    tPoly_setPitchGlideTime     (tPoly* const poly, float t);
    
    //! Set the amount of pitch bend
    /*!
     @param poly A pointer to the relevant tPoly.
     @param pitchBend The new amount of pitch bend.
     */
    void    tPoly_setPitchBend          (tPoly* const poly, float pitchBend);
    void 	tPoly_setBendGlideTime		(tPoly* const poly, float t);
    void 	tPoly_setBendSamplesPerTick	(tPoly* const poly, float t);

    
    //! Execute all tick-rate changes in the poly handler's pitch, including glide and bend.
    /*!
     @param poly A pointer to the relevant tPoly.
     */
    void    tPoly_tickPitch             (tPoly* const poly);
    
    
    //! Execute the tick-rate change of the poly handler's pitch glide.
    /*!
     @param poly A pointer to the relevant tPoly.
     */
    void    tPoly_tickPitchGlide        (tPoly* const poly);
    
    
    //! Execute the tick-rate change of the poly handler's pitch bend.
    /*!
     @param poly A pointer to the relevant tPoly.
     */
    void    tPoly_tickPitchBend         (tPoly* const poly);
    
    
    //! Get the current number of voices available to play notes.
    /*!
     @param poly A pointer to the relevant tPoly.
     @return The current number of voices available to play notes.
     */
    int     tPoly_getNumVoices          (tPoly* const poly);
    
    //! Get the number of voices currently playing notes.
    /*!
     @param poly A pointer to the relevant tPoly.
     @return The number of voices currently playing notes.
     */
    int     tPoly_getNumActiveVoices    (tPoly* const poly);
    
    //! Get the current pitch of a given voice.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the pitch of.
     @return The current pitch of the given voice as a fractional MIDI note number.
     */
    float   tPoly_getPitch              (tPoly* const poly, uint8_t voice);
    
    
    //! Get the current MIDI note number of a given voice.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the MIDI note number of.
     @return The MIDI note number of the given voice.
     */
    int     tPoly_getKey                (tPoly* const poly, uint8_t voice);
    
    
    //! Get the current MIDI velocity of a given voice.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the MIDI velocity of.
     @return The current MIDI velocity of the given voice.
     */
    int     tPoly_getVelocity           (tPoly* const poly, uint8_t voice);
    
    
    //! Get the current play state of a given voice.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the state of.
     @return The current play state of the given voice.
     */
    int     tPoly_isOn                  (tPoly* const poly, uint8_t voice);
    
    /*! @} */
    
    //==============================================================================

    
    /* tPoly */
    typedef struct _tSimplePoly
    {
        tStack stack;

        int numVoices;
        int maxNumVoices;
        int** voices;
        int stealing_on;
        int recover_stolen;

        int notes[128][2];
    } _tSimplePoly;

    typedef _tSimplePoly* tSimplePoly;

    //! Initialize a tPoly to the default LEAF mempool.
    /*!
     @param poly A pointer to the tPoly to be initialized.
     @param maxNumVoices The maximum number of voices this tPoly can handle at once.
     */
    void    tSimplePoly_init                  (tSimplePoly* const poly, int maxNumVoices);


    //! Free a tPoly from the default LEAF mempool.
    /*!
     @param poly A pointer to the tPoly to be freed.
     */
    void    tSimplePoly_free                  (tSimplePoly* const poly);


    //! Initialize a tPoly to a specified mempool.
    /*!
     @param poly A pointer to the tPoly to be initialized.
     @param pool A pointer to the tMempool to which the tPoly should be initialized.
     */
    void    tSimplePoly_initToPool            (tSimplePoly* const poly, int maxNumVoices, tMempool* const pool);


    //! Free a tPoly from a specified mempool.
    /*!
     @param poly A pointer to the tPoly to be freed.
     @param pool A pointer to the tMempool from which the tPoly should be freed.
     */
    void    tSimplePoly_freeFromPool          (tSimplePoly* const poly, tMempool* const pool);

    //! Add a note with a given velocity to the poly handler.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param note The MIDI note number to add.
     @param vel The MIDI velocity of the note to add.
     @return The voice that will play the note.
     */
    int     tSimplePoly_noteOn                (tSimplePoly* const poly, int note, uint8_t vel);


    //! Remove a note from the poly handler.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param note The MIDI note number to remove.
     @return The voice that was playing the removed note.
     */
    int     tSimplePoly_noteOff               (tSimplePoly* const poly, uint8_t note);



    void tSimplePoly_deactivateVoice(tSimplePoly* const polyh, uint8_t voice);

    int tSimplePoly_markPendingNoteOff(tSimplePoly* const polyh, uint8_t note);


    //find if there is a voice with that note -- useful for note offs where you want to wait to remove it from the poly until the release phase of the envelope is finished
    int tSimplePoly_findVoiceAssignedToNote(tSimplePoly* const polyh, uint8_t note);

    //! Set the number of voices available to play notes.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param numVoices The new number of available voices. Cannot be greater than the max number voices given in tPoly_init().
     */
    void    tSimplePoly_setNumVoices          (tSimplePoly* const poly, uint8_t numVoices);

    //! Set whether pitch glide over note changes in voices is active.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param isActive Whether pitch glide should be active or not.
     */

    //! Get the current number of voices available to play notes.
    /*!
     @param poly A pointer to the relevant tPoly.
     @return The current number of voices available to play notes.
     */
    int     tSimplePoly_getNumVoices          (tSimplePoly* const poly);

    //! Get the number of voices currently playing notes.
    /*!
     @param poly A pointer to the relevant tPoly.
     @return The number of voices currently playing notes.
     */
    int     tSimplePoly_getNumActiveVoices    (tSimplePoly* const poly);

    //! Get the current MIDI note number of a given voice.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the MIDI note number of.
     @return The MIDI note number of the given voice.
     */

    int   tSimplePoly_getPitch              (tSimplePoly* const poly, uint8_t voice);


    //this one returns negative one if the voice is inactive
    int tSimplePoly_getPitchAndCheckActive(tSimplePoly* const polyh, uint8_t voice);


    //! Get the current MIDI velocity of a given voice.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the MIDI velocity of.
     @return The current MIDI velocity of the given voice.
     */
    int     tSimplePoly_getVelocity           (tSimplePoly* const poly, uint8_t voice);


    //! Get the current play state of a given voice.
    /*!
     @param poly A pointer to the relevant tPoly.
     @param voice The voice to get the state of.
     @return The current play state of the given voice.
     */
    int     tSimplePoly_isOn                  (tSimplePoly* const poly, uint8_t voice);

    /*! @} */
    //==============================================================================
    
#ifdef __cplusplus
}
#endif

#endif // LEAF_MIDI_H_INCLUDED

//==============================================================================


