/*==============================================================================

    leaf-midi.c
    Created: 30 Nov 2018 11:29:16am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-midi.h"

#else

#include "../Inc/leaf-midi.h"

#endif

//====================================================================================
/* Stack */
//====================================================================================

void tStack_init(tStack* const stack, LEAF* const leaf)
{
    tStack_initToPool(stack, &leaf->mempool);
}

void    tStack_initToPool           (tStack* const stack, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tStack* ns = *stack = (_tStack*) mpool_alloc(sizeof(_tStack), m);
    ns->mempool = m;
    
    ns->ordered = 0;
    ns->size = 0;
    ns->pos = 0;
    ns->capacity = STACK_SIZE;
    
    for (int i = 0; i < STACK_SIZE; i++) ns->data[i] = -1;
}

void    tStack_free        (tStack* const stack)
{
    _tStack* ns = *stack;
    
    mpool_free((char*)ns, ns->mempool);
}

// If stack contains note, returns index. Else returns -1;
int tStack_contains(tStack const ns, uint16_t noteVal)
{
    for (int i = 0; i < ns->size; i++)
    {
        if (ns->data[i] == noteVal)    return i;
    }
    return -1;
}

void tStack_add(tStack const ns, uint16_t noteVal)
{
    uint8_t j;
    
    int whereToInsert = 0;
    if (ns->ordered)
    {
        for (j = 0; j < ns->size; j++)
        {
            if (noteVal > ns->data[j])
            {
                if ((noteVal < ns->data[j+1]) || (ns->data[j+1] == -1))
                {
                    whereToInsert = j+1;
                    break;
                }
            }
        }
    }
    
    //first move notes that are already in the stack one position to the right
    for (j = ns->size; j > whereToInsert; j--)
    {
        ns->data[j] = ns->data[(j - 1)];
    }
    
    //then, insert the new note into the front of the stack
    ns->data[whereToInsert] = noteVal;
    
    ns->size++;
}

int tStack_addIfNotAlreadyThere(tStack const ns, uint16_t noteVal)
{
    uint8_t j;
    
    int added = 0;
    
    if (tStack_contains(ns, noteVal) == -1)
    {
        int whereToInsert = 0;
        if (ns->ordered)
        {
            for (j = 0; j < ns->size; j++)
            {
                if (noteVal > ns->data[j])
                {
                    if ((noteVal < ns->data[j+1]) || (ns->data[j+1] == -1))
                    {
                        whereToInsert = j+1;
                        break;
                    }
                }
            }
        }
        
        //first move notes that are already in the stack one position to the right
        for (j = ns->size; j > whereToInsert; j--)
        {
            ns->data[j] = ns->data[(j - 1)];
        }
        
        //then, insert the new note into the front of the stack
        ns->data[whereToInsert] = noteVal;
        
        ns->size++;
        
        added = 1;
    }
    
    return added;
}

// Remove noteVal. return 1 if removed, 0 if not
int tStack_remove(tStack const ns, uint16_t noteVal)
{
    uint8_t k;
    int foundIndex = tStack_contains(ns, noteVal);
    int removed = 0;
    
    if (foundIndex >= 0)
    {
        for (k = 0; k < (ns->size - foundIndex); k++)
        {
            if ((k+foundIndex) >= (ns->capacity - 1))
            {
                ns->data[k + foundIndex] = -1;
            }
            else
            {
                ns->data[k + foundIndex] = ns->data[k + foundIndex + 1];
                if ((k + foundIndex) == (ns->size - 1))
                {
                    ns->data[k + foundIndex + 1] = -1;
                }
            }
            
        }
        // in case it got put on the stack multiple times
        foundIndex--;
        ns->size--;
        removed = 1;
    }
    
    return removed;
}

// Doesn't change size of data types
void tStack_setCapacity(tStack const ns, uint16_t cap)
{
    if (cap <= 0)
        ns->capacity = 1;
    else if (cap <= STACK_SIZE)
        ns->capacity = cap;
    else
        ns->capacity = STACK_SIZE;
    
    for (int i = cap; i < STACK_SIZE; i++)
    {
        if ((int)ns->data[i] != -1)
        {
            ns->data[i] = -1;
            ns->size -= 1;
        }
    }
    
    if (ns->pos >= cap)
    {
        ns->pos = 0;
    }
}

int tStack_getSize(tStack const ns)
{
    return ns->size;
}

void tStack_clear(tStack const ns)
{
    for (int i = 0; i < STACK_SIZE; i++)
    {
        ns->data[i] = -1;
    }
    ns->pos = 0;
    ns->size = 0;
}

// Next item in order of addition to stack. Return 0-31 if there is a next item to move to. Returns -1 otherwise.
int tStack_next(tStack const ns)
{
    int step = 0;
    if (ns->size != 0) // if there is at least one note in the stack
    {
        if (ns->pos > 0) // if you're not at the most recent note (first one), then go backward in the array (moving from earliest to latest)
        {
            ns->pos--;
        }
        else
        {
            ns->pos = (ns->size - 1); // if you are the most recent note, go back to the earliest note in the array
        }
        
        step = ns->data[ns->pos];
        return step;
    }
    else
    {
        return -1;
    }
}

int tStack_get(tStack const ns, int which)
{
    return ns->data[which];
}

int tStack_first(tStack const ns)
{
    return ns->data[0];
}


// POLY
void tPoly_init(tPoly* const polyh, int maxNumVoices, LEAF* const leaf)
{
    tPoly_initToPool(polyh, maxNumVoices, &leaf->mempool);
}

void    tPoly_initToPool            (tPoly* const polyh, int maxNumVoices, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tPoly* poly = *polyh = (_tPoly*) mpool_alloc(sizeof(_tPoly), m);
    poly->mempool = m;
    
    poly->numVoices = maxNumVoices;
    poly->maxNumVoices = maxNumVoices;
    poly->lastVoiceToChange = 0;
    
    // Arp mode stuff
    poly->currentVoice = 0;
    poly->maxLength = 128;
    poly->currentNote = -1;
    
    //default learned CCs and notes are just the CCs 1-128 - notes are skipped
    for (int i = 0; i < 128; i++)
    {
        poly->notes[i][0] = 0;
        poly->notes[i][1] = -1;
    }
    
    poly->glideTime = 5.0f;
    
    poly->ramps = (tRamp*) mpool_alloc(sizeof(tRamp) * poly->maxNumVoices, m);
    poly->rampVals = (Lfloat*) mpool_alloc(sizeof(Lfloat) * poly->maxNumVoices, m);
    poly->firstReceived = (int*) mpool_alloc(sizeof(int) * poly->maxNumVoices, m);
    poly->voices = (int**) mpool_alloc(sizeof(int*) * poly->maxNumVoices, m);
    
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        poly->voices[i] = (int*) mpool_alloc(sizeof(int) * 2, m);
        poly->voices[i][0] = -1;
        poly->firstReceived[i] = 0;
        
        tRamp_initToPool(&poly->ramps[i], poly->glideTime, 1, mp);
    }
    
    poly->pitchBend = 0.0f;
    
    tRamp_initToPool(&poly->pitchBendRamp, 1.0f, 1, mp);
    tStack_initToPool(&poly->stack, mp);
    tStack_initToPool(&poly->orderStack, mp);
    
    poly->pitchGlideIsActive = 0;
}

void    tPoly_free  (tPoly* const polyh)
{
    _tPoly* poly = *polyh;
    
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        tRamp_free(&poly->ramps[i]);
        mpool_free((char*)poly->voices[i], poly->mempool);
    }
    tRamp_free(&poly->pitchBendRamp);
    tStack_free(&poly->stack);
    tStack_free(&poly->orderStack);
    
    mpool_free((char*)poly->voices, poly->mempool);
    mpool_free((char*)poly->ramps, poly->mempool);
    mpool_free((char*)poly->rampVals, poly->mempool);
    mpool_free((char*)poly->firstReceived, poly->mempool);
    
    mpool_free((char*)poly, poly->mempool);
}

void tPoly_tickPitch(tPoly polyh)
{
    tPoly_tickPitchGlide(polyh);
    tPoly_tickPitchBend(polyh);
}

void tPoly_tickPitchGlide(tPoly poly)
{
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        tRamp_tick(poly->ramps[i]);
    }
}

void tPoly_tickPitchBend(tPoly poly)
{
    tRamp_tick(poly->pitchBendRamp);
}

void tPoly_setPitchBend(tPoly const poly, Lfloat pitchBend)
{
    poly->pitchBend = pitchBend;
    tRamp_setDest(poly->pitchBendRamp, poly->pitchBend);
}

int tPoly_noteOn(tPoly const poly, int note, uint8_t vel)
{
    // if not in keymap or already on stack, dont do anything. else, add that note.
    if (tStack_contains(poly->stack, note) >= 0) return -1;
    else
    {
        tPoly_orderedAddToStack(poly, note);
        tStack_add(poly->stack, note);
        
        int alteredVoice = -1;
        int found = 0;
        for (int i = 0; i < poly->numVoices; i++)
        {
            if (poly->voices[i][0] < 0)    // if inactive voice, give this note to voice
            {
                if (!poly->firstReceived[i] || !poly->pitchGlideIsActive)
                {
                    tRamp_setVal(poly->ramps[i], note);
                    poly->firstReceived[i] = 1;
                }
                
                found = 1;
                
                poly->voices[i][0] = note;
                poly->voices[i][1] = vel;
                poly->lastVoiceToChange = i;
                poly->notes[note][0] = vel;
                poly->notes[note][1] = i;
                
                tRamp_setDest(poly->ramps[i], poly->voices[i][0]);
                
                alteredVoice = i;
                break;
            }
        }
        
        if (!found) //steal
        {
            int whichVoice, whichNote;
            for (int j = tStack_getSize(poly->stack) - 1; j >= 0; j--)
            {
                whichNote = tStack_get(poly->stack, j);
                whichVoice = poly->notes[whichNote][1];
                if (whichVoice >= 0)
                {
                    poly->lastVoiceToChange = whichVoice;
                    int oldNote = poly->voices[whichVoice][0];
                    poly->voices[whichVoice][0] = note;
                    poly->voices[whichVoice][1] = vel;
                    poly->notes[oldNote][1] = -1; //mark the stolen voice as inactive (in the second dimension of the notes array)
                    poly->notes[note][0] = vel;
                    poly->notes[note][1] = whichVoice;
                    
                    if (poly->pitchGlideIsActive)
                    {
                    	tRamp_setTime(poly->ramps[whichVoice], poly->glideTime);
                    }
                    else
                    {
                    	tRamp_setVal(poly->ramps[whichVoice], note);
                    }
                    tRamp_setDest(poly->ramps[whichVoice], poly->voices[whichVoice][0]);
                    
                    alteredVoice = whichVoice;
                    
                    break;
                }
            }
        }
        return alteredVoice;
    }
}


int16_t noteToTest = -1;

int tPoly_noteOff(tPoly const poly, uint8_t note)
{
    tStack_remove(poly->stack, note);
    tStack_remove(poly->orderStack, note);
    poly->notes[note][0] = 0;
    poly->notes[note][1] = -1;
    
    int deactivatedVoice = -1;
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        if (poly->voices[i][0] == note)
        {
            poly->voices[i][0] = -1;
            poly->voices[i][1] = 0;
            poly->lastVoiceToChange = i;
            deactivatedVoice = i;
            break;
        }
    }
    /*
     //monophonic handling
     if ((poly->numVoices == 1) && (tStack_getSize(poly->stack) > 0))
     {
     int oldNote = tStack_first(poly->stack);
     poly->voices[0][0] = oldNote;
     poly->voices[0][1] = poly->notes[oldNote][0];
     poly->lastVoiceToChange = 0;
     }
     */
    
    //grab old notes off the stack if there are notes waiting to replace the free voice
    if (deactivatedVoice >= 0)
    {
        for (int j = 0; j < tStack_getSize(poly->stack); ++j)
        {
            noteToTest = tStack_get(poly->stack, j);
            
            if (poly->notes[noteToTest][1] < 0) //if there is a stolen note waiting (marked inactive but on the stack)
            {
                poly->voices[deactivatedVoice][0] = noteToTest; //set the newly free voice to use the old stolen note
                if (poly->pitchGlideIsActive)
                {
                	tRamp_setTime(poly->ramps[deactivatedVoice], poly->glideTime);
                }
                else
                {
                	tRamp_setVal(poly->ramps[deactivatedVoice], noteToTest);
                }
                tRamp_setDest(poly->ramps[deactivatedVoice], poly->voices[deactivatedVoice][0]);
                poly->voices[deactivatedVoice][1] = poly->notes[noteToTest][0]; // set the velocity of the voice to be the velocity of that note
                poly->notes[noteToTest][1] = deactivatedVoice; //mark that it is no longer stolen and is now active
                return -1;
            }
        }
    }
    return deactivatedVoice;
}

void tPoly_orderedAddToStack(tPoly const poly, uint8_t noteVal)
{
    uint8_t j;
    int myPitch, thisPitch, nextPitch;
    
    tStack ns = poly->orderStack;
    
    int whereToInsert = 0;
    
    for (j = 0; j < ns->size; j++)
    {
        myPitch = noteVal;
        thisPitch = ns->data[j];
        nextPitch = ns->data[j+1];
        
        if (myPitch > thisPitch)
        {
            if ((myPitch < nextPitch) || (nextPitch == -1))
            {
                whereToInsert = j+1;
                break;
            }
        }
    }
    
    //first move notes that are already in the stack one position to the right
    for (j = ns->size; j > whereToInsert; j--)
    {
        ns->data[j] = ns->data[(j - 1)];
    }
    
    //then, insert the new note into the front of the stack
    ns->data[whereToInsert] =  noteVal;
    
    ns->size++;
    
}

void tPoly_setNumVoices(tPoly const poly, uint8_t numVoices)
{
    poly->numVoices = (numVoices > poly->maxNumVoices) ? poly->maxNumVoices : numVoices;
}

void tPoly_setPitchGlideActive(tPoly const poly, int isActive)
{
    poly->pitchGlideIsActive = isActive;
}

void tPoly_setPitchGlideTime(tPoly const poly, Lfloat t)
{
    poly->glideTime = t;
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        tRamp_setTime(poly->ramps[i], poly->glideTime);
    }
}

void tPoly_setBendGlideTime(tPoly const poly, Lfloat t)
{
    tRamp_setTime(poly->pitchBendRamp, t);
}

void tPoly_setBendSamplesPerTick(tPoly const poly, Lfloat t)
{
	 poly->pitchBendRamp->samples_per_tick =  t;
}

int tPoly_getNumVoices(tPoly const poly)
{
    return poly->numVoices;
}

int tPoly_getNumActiveVoices(tPoly const poly)
{
    return LEAF_clip(0, tStack_getSize(poly->stack), poly->numVoices);
}

Lfloat tPoly_getPitch(tPoly const poly, uint8_t voice)
{
    return tRamp_sample(poly->ramps[voice]) + tRamp_sample(poly->pitchBendRamp);
}

int tPoly_getKey(tPoly const poly, uint8_t voice)
{
    return poly->voices[voice][0];
}

int tPoly_getVelocity(tPoly const poly, uint8_t voice)
{
    return poly->voices[voice][1];
}

int tPoly_isOn(tPoly const poly, uint8_t voice)
{
    return (poly->voices[voice][0] > 0) ? 1 : 0;
}

void tPoly_setSampleRate(tPoly const poly, Lfloat sr)
{
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        tRamp_setSampleRate(poly->ramps[i], sr);
    }
    tRamp_setSampleRate(poly->pitchBendRamp, sr);
}


//tSimplePoly = much more efficient implementation without ramps and glide


// SIMPLE POLY
void tSimplePoly_init(tSimplePoly* const polyh, int maxNumVoices, LEAF* const leaf)
{
    tSimplePoly_initToPool(polyh, maxNumVoices, &leaf->mempool);
}

void    tSimplePoly_initToPool            (tSimplePoly* const polyh, int maxNumVoices, tMempool* const mp)
{
    _tMempool* m = *mp;
    _tSimplePoly* poly = *polyh = (_tSimplePoly*) mpool_alloc(sizeof(_tSimplePoly), m);
    poly->mempool = m;

    poly->numVoices = maxNumVoices;
    poly->maxNumVoices = maxNumVoices;

    for (int i = 0; i < 128; i++)
    {
        poly->notes[i][0] = -1;
        poly->notes[i][1] = 0;
    }
    poly->stealing_on = 1;
    poly->recover_stolen = 1;
    poly->voices = (int**) mpool_calloc(sizeof(int*) * poly->maxNumVoices, m);

    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        poly->voices[i] = (int*) mpool_calloc(sizeof(int) * 3, m);
        poly->voices[i][0] = -1;
    }
    tStack_initToPool(&poly->stack, mp);

}

void    tSimplePoly_free  (tSimplePoly* const polyh)
{
    _tSimplePoly* poly = *polyh;

    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        mpool_free((char*)poly->voices[i], poly->mempool);
    }
    tStack_free(&poly->stack);
    mpool_free((char*)poly->voices, poly->mempool);
    mpool_free((char*)poly, poly->mempool);
}

int tSimplePoly_noteOn(tSimplePoly const poly, int note, uint8_t vel)
{
    int whichVoice, whichNote, oldNote, alteredVoice;
    // if not in keymap or already on stack, dont do anything. else, add that note.
    if (tStack_contains(poly->stack, note) >= 0) return -1;
    else
    {
        alteredVoice = -1;
        int found = 0;
        for (int i = 0; i < poly->numVoices; i++)
        {
            if (poly->voices[i][0] == -1)    // if inactive voice, give this note to voice
            {

                found = 1;

                poly->voices[i][0] = note;
                poly->voices[i][1] = vel;
                poly->notes[note][0] = i;
                poly->notes[note][1] = vel;
                poly->voices[i][2] = note; // voices[i][2] is the output midi note, (avoiding the -1 when a voice is inactive)

                alteredVoice = i;
                tStack_add(poly->stack, note);
                break;
            }
        }
        if (!found)
        {
            //second preference is grabbing one that is in release phase but not finished sounding yet
            for (int i = 0 ; i < poly->numVoices; i++)
            {
                if (poly->voices[i][0] == -2)    // if voice is released but still sounding, take over this voice
                {

                    found = 1;

                    poly->voices[i][0] = note;
                    poly->voices[i][1] = vel;
                    poly->notes[note][0] = i;
                    poly->notes[note][1] = vel;
                    poly->voices[i][2] = note; // voices[i][2] is the output midi note, (avoiding the -1 when a voice is inactive)

                    alteredVoice = i;
                    tStack_add(poly->stack, note);
                    break;
                }
            }
        }
        if ((!found) && (poly->stealing_on)) //steal
        {
            for (int j = tStack_getSize(poly->stack) - 1; j >= 0; j--)
            {
                whichNote = tStack_get(poly->stack, j);
                whichVoice = poly->notes[whichNote][0];
                if (whichVoice >= 0)
                {
                    oldNote = poly->voices[whichVoice][0];
                    poly->voices[whichVoice][0] = note;
                    poly->voices[whichVoice][1] = vel;
                    poly->notes[oldNote][0] = -3; //mark the stolen voice as stolen (in the second dimension of the notes array)
                    poly->notes[note][0] = whichVoice;
                    poly->notes[note][1] = vel;

                    poly->voices[whichVoice][2] = note;

                    alteredVoice = whichVoice;
                    tStack_add(poly->stack, note);
                    break;
                }
            }
        }
        return alteredVoice;
    }
}


int tSimplePoly_noteOff(tSimplePoly const poly, uint8_t note)
{
    int16_t noteToTest = -1;

    tStack_remove(poly->stack, note);
    poly->notes[note][0] = -1;

    int deactivatedVoice = -1;
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        if (poly->voices[i][0] == note)
        {
            poly->voices[i][0] = -1;
            poly->voices[i][1] = 0;
            deactivatedVoice = i;
            break;
        }
    }

    if (poly->recover_stolen)
    {
        //grab old notes off the stack if there are notes waiting to replace the free voice
        if (deactivatedVoice >= 0)
        {
            for (int j = 0; j < tStack_getSize(poly->stack); ++j)
            {
                noteToTest = tStack_get(poly->stack, j);

                if (poly->notes[noteToTest][0] == -3) //if there is a stolen note waiting (marked inactive but on the stack)
                {
                    poly->voices[deactivatedVoice][0] = noteToTest; //set the newly free voice to use the old stolen note
                    poly->voices[deactivatedVoice][1] = poly->notes[noteToTest][1]; // set the velocity of the voice to be the velocity of that note
                    poly->voices[deactivatedVoice][2] = noteToTest;
                    poly->notes[noteToTest][0] = deactivatedVoice; //mark that it is no longer stolen and is now active
                    return -1;
                }
            }
        }
    }
    return deactivatedVoice;
}


void tSimplePoly_deactivateVoice(tSimplePoly const poly, uint8_t voice)
{
    if (poly->voices[voice][0] == -2) //only do this if the voice is waiting for deactivation (not already reassigned while waiting)
    {
        poly->voices[voice][0] = -1;
        poly->voices[voice][1] = 0;
        if (poly->recover_stolen)
        {
            //grab old notes off the stack if there are notes waiting to replace the free voice
            for (int j = 0; j < tStack_getSize(poly->stack); ++j)
            {
                noteToTest = tStack_get(poly->stack, j); //note to check if it is waiting to be recovered

                if (poly->notes[noteToTest][0] == -3) //if there is a stolen note waiting (marked inactive but on the stack)
                {
                    poly->voices[voice][0] = noteToTest; //set the newly free voice to use the old stolen note
                    poly->voices[voice][1] = poly->notes[noteToTest][1]; // set the velocity of the voice to be the velocity of that note
                    poly->voices[voice][2] = noteToTest;
                    poly->notes[noteToTest][0] = voice; //mark that it is no longer stolen and is now active
                    //poly->notes[][0] = -1;
                    break;
                }
            }
        }
    }
}

int tSimplePoly_findVoiceAssignedToNote(tSimplePoly const poly, uint8_t note)
{
    int voiceWithThatNote = -1;
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        if (poly->voices[i][0] == note)
        {
            voiceWithThatNote = i;
            break;
        }
    }
    return voiceWithThatNote;
}


int tSimplePoly_markPendingNoteOff(tSimplePoly const poly, uint8_t note)
{
    int deactivatedVoice = -1;

    if (tStack_remove(poly->stack, note))
    {
        poly->notes[note][0] = -2;
        
        for (int i = 0; i < poly->maxNumVoices; i++)
        {
            if (poly->voices[i][0] == note)
            {
                poly->voices[i][0] = -2;
                poly->voices[i][1] = 0;
                deactivatedVoice = i;
                break;
            }
        }
    }
    return deactivatedVoice;
}

void tSimplePoly_setNumVoices(tSimplePoly const poly, uint8_t numVoices)
{
    poly->numVoices = (numVoices > poly->maxNumVoices) ? poly->maxNumVoices : numVoices;
}


int tSimplePoly_getNumVoices(tSimplePoly const poly)
{
    return poly->numVoices;
}

int tSimplePoly_getNumActiveVoices(tSimplePoly const poly)
{
    return LEAF_clip(0, tStack_getSize(poly->stack), poly->numVoices);
}


int tSimplePoly_getPitch(tSimplePoly const poly, uint8_t voice)
{
    return poly->voices[voice][2];
}

//this one returns negative one if the voice is inactive
int tSimplePoly_getPitchAndCheckActive(tSimplePoly const poly, uint8_t voice)
{
    return poly->voices[voice][0];
}

int tSimplePoly_getVelocity(tSimplePoly const poly, uint8_t voice)
{
    return poly->voices[voice][1];
}

int tSimplePoly_isOn(tSimplePoly const poly, uint8_t voice)
{
    return (poly->voices[voice][0] > 0) ? 1 : 0;
}
