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

// POLY
void tPoly_init(tPoly* const polyh, int maxNumVoices)
{
    _tPoly* poly = *polyh = (_tPoly*) leaf_alloc(sizeof(_tPoly));
    
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
    
    poly->ramps = (tRamp*) leaf_alloc(sizeof(tRamp) * poly->maxNumVoices);
    poly->rampVals = (float*) leaf_alloc(sizeof(float) * poly->maxNumVoices);
    poly->firstReceived = (oBool*) leaf_alloc(sizeof(oBool) * poly->maxNumVoices);
    poly->voices = (int**) leaf_alloc(sizeof(int*) * poly->maxNumVoices);
    
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        poly->voices[i] = (int*) leaf_alloc(sizeof(int) * 2);
        poly->voices[i][0] = -1;
        poly->firstReceived[i] = OFALSE;
        
        tRamp_init(&poly->ramps[i], poly->glideTime, 1);
    }
    
    poly->pitchBend = 0.0f;
    
    tRamp_init(&poly->pitchBendRamp, 1.0f, 1);
    tStack_init(&poly->stack);
    tStack_init(&poly->orderStack);
    
    poly->pitchGlideIsActive = OFALSE;
}

void tPoly_free(tPoly* const polyh)
{
    _tPoly* poly = *polyh;
    
    for (int i = 0; i < poly->maxNumVoices; i++)
    {
        tRamp_free(&poly->ramps[i]);
        leaf_free(poly->voices[i]);
    }
    tRamp_free(&poly->pitchBendRamp);
    tStack_free(&poly->stack);
    tStack_free(&poly->orderStack);
    
    leaf_free(poly->voices);
    leaf_free(poly->ramps);
    leaf_free(poly->rampVals);
    leaf_free(poly->firstReceived);
    
    leaf_free(poly);
}

void tPoly_tickPitch(tPoly* polyh)
{
    tPoly_tickPitchGlide(polyh);
    tPoly_tickPitchBend(polyh);
}

void tPoly_tickPitchGlide(tPoly* polyh)
{
    _tPoly* poly = *polyh;
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        tRamp_tick(&poly->ramps[i]);
    }
}

void tPoly_tickPitchBend(tPoly* polyh)
{
    _tPoly* poly = *polyh;
    tRamp_tick(&poly->pitchBendRamp);
}

//instead of including in dacsend, should have a separate pitch bend ramp, that is added when the ramps are ticked and sent to DAC
void tPoly_setPitchBend(tPoly* const polyh, float pitchBend)
{
    _tPoly* poly = *polyh;
    poly->pitchBend = pitchBend;
    tRamp_setDest(&poly->pitchBendRamp, poly->pitchBend);
}

int tPoly_noteOn(tPoly* const polyh, int note, uint8_t vel)
{
    _tPoly* poly = *polyh;
    
    // if not in keymap or already on stack, dont do anything. else, add that note.
    if (tStack_contains(&poly->stack, note) >= 0) return -1;
    else
    {
        tPoly_orderedAddToStack(polyh, note);
        tStack_add(&poly->stack, note);
        
        int alteredVoice = -1;
        oBool found = OFALSE;
        for (int i = 0; i < poly->numVoices; i++)
        {
            if (poly->voices[i][0] < 0)    // if inactive voice, give this note to voice
            {
                if (!poly->firstReceived[i] || !poly->pitchGlideIsActive)
                {
                    tRamp_setVal(&poly->ramps[i], note);
                    poly->firstReceived[i] = OTRUE;
                }
                
                found = OTRUE;
                
                poly->voices[i][0] = note;
                poly->voices[i][1] = vel;
                poly->lastVoiceToChange = i;
                poly->notes[note][0] = vel;
                poly->notes[note][1] = i;
                
                tRamp_setDest(&poly->ramps[i], poly->voices[i][0]);
                
                alteredVoice = i;
                break;
            }
        }
        
        if (!found) //steal
        {
            int whichVoice, whichNote;
            for (int j = tStack_getSize(&poly->stack) - 1; j >= 0; j--)
            {
                whichNote = tStack_get(&poly->stack, j);
                whichVoice = poly->notes[whichNote][1];
                if (whichVoice >= 0)
                {
                    poly->lastVoiceToChange = j;
                    int oldNote = poly->voices[whichVoice][0];
                    poly->voices[whichVoice][0] = note;
                    poly->voices[whichVoice][1] = vel;
                    poly->notes[oldNote][1] = -1; //mark the stolen voice as inactive (in the second dimension of the notes array)
                    poly->notes[note][0] = vel;
                    poly->notes[note][1] = whichVoice;
                    
                    tRamp_setTime(&poly->ramps[whichVoice], poly->glideTime);
                    tRamp_setDest(&poly->ramps[whichVoice], poly->voices[whichVoice][0]);
                    
                    alteredVoice = whichVoice;
                    
                    break;
                }
            }
        }
        return alteredVoice;
    }
}


int16_t noteToTest = -1;

int tPoly_noteOff(tPoly* const polyh, uint8_t note)
{
    _tPoly* poly = *polyh;
    
    tStack_remove(&poly->stack, note);
    tStack_remove(&poly->orderStack, note);
    poly->notes[note][0] = 0;
    poly->notes[note][1] = -1;
    
    int deactivatedVoice = -1;
    for (int i = 0; i < poly->numVoices; i++)
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
        for (int j = 0; j < tStack_getSize(&poly->stack); ++j)
        {
            noteToTest = tStack_get(&poly->stack, j);
            
            if (poly->notes[noteToTest][1] < 0) //if there is a stolen note waiting (marked inactive but on the stack)
            {
                poly->voices[deactivatedVoice][0] = noteToTest; //set the newly free voice to use the old stolen note
                tRamp_setDest(&poly->ramps[deactivatedVoice], poly->voices[deactivatedVoice][0]);
                poly->voices[deactivatedVoice][1] = poly->notes[noteToTest][0]; // set the velocity of the voice to be the velocity of that note
                poly->notes[noteToTest][1] = deactivatedVoice; //mark that it is no longer stolen and is now active
                return -1;
            }
        }
    }
    return deactivatedVoice;
}

void tPoly_orderedAddToStack(tPoly* const polyh, uint8_t noteVal)
{
    _tPoly* poly = *polyh;
    
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

void tPoly_setNumVoices(tPoly* const polyh, uint8_t numVoices)
{
    _tPoly* poly = *polyh;
    poly->numVoices = (numVoices > poly->maxNumVoices) ? poly->maxNumVoices : numVoices;
}

void tPoly_setPitchGlideActive(tPoly* const polyh, oBool isActive)
{
    _tPoly* poly = *polyh;
    poly->pitchGlideIsActive = isActive;
}

void tPoly_setPitchGlideTime(tPoly* const polyh, float t)
{
    _tPoly* poly = *polyh;
    poly->glideTime = t;
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        tRamp_setTime(&poly->ramps[i], poly->glideTime);
    }
}

int tPoly_getNumVoices(tPoly* const polyh)
{
    _tPoly* poly = *polyh;
    return poly->numVoices;
}

int tPoly_getNumActiveVoices(tPoly* const polyh)
{
    _tPoly* poly = *polyh;
    return tStack_getSize(&poly->stack);
}

float tPoly_getPitch(tPoly* const polyh, uint8_t voice)
{
    _tPoly* poly = *polyh;
    return tRamp_sample(&poly->ramps[voice]) + tRamp_sample(&poly->pitchBendRamp);
}

int tPoly_getKey(tPoly* const polyh, uint8_t voice)
{
    _tPoly* poly = *polyh;
    return poly->voices[voice][0];
}

int tPoly_getVelocity(tPoly* const polyh, uint8_t voice)
{
    _tPoly* poly = *polyh;
    return poly->voices[voice][1];
}

int tPoly_isOn(tPoly* const polyh, uint8_t voice)
{
    _tPoly* poly = *polyh;
    return (poly->voices[voice][0] > 0) ? 1 : 0;
}


//====================================================================================
/* Stack */
//====================================================================================

void tStack_init(tStack* const stack)
{
    _tStack* ns = *stack = (_tStack*) leaf_alloc(sizeof(_tStack));
    
    ns->ordered = OFALSE;
    ns->size = 0;
    ns->pos = 0;
    ns->capacity = STACK_SIZE;
    
    for (int i = 0; i < STACK_SIZE; i++) ns->data[i] = -1;
}

void tStack_free(tStack* const stack)
{
    _tStack* ns = *stack;
    
    leaf_free(ns);
}

// If stack contains note, returns index. Else returns -1;
int tStack_contains(tStack* const stack, uint16_t noteVal)
{
    _tStack* ns = *stack;
    for (int i = 0; i < ns->size; i++)
    {
        if (ns->data[i] == noteVal)    return i;
    }
    return -1;
}

void tStack_add(tStack* const stack, uint16_t noteVal)
{
    _tStack* ns = *stack;
    
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

int tStack_addIfNotAlreadyThere(tStack* const stack, uint16_t noteVal)
{
    _tStack* ns = *stack;
    
    uint8_t j;
    
    int added = 0;
    
    if (tStack_contains(stack, noteVal) == -1)
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
int tStack_remove(tStack* const stack, uint16_t noteVal)
{
    _tStack* ns = *stack;
    
    uint8_t k;
    int foundIndex = tStack_contains(stack, noteVal);
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
void tStack_setCapacity(tStack* const stack, uint16_t cap)
{
    _tStack* ns = *stack;
    
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

int tStack_getSize(tStack* const stack)
{
    _tStack* ns = *stack;
    
    return ns->size;
}

void tStack_clear(tStack* const stack)
{
    _tStack* ns = *stack;
    
    for (int i = 0; i < STACK_SIZE; i++)
    {
        ns->data[i] = -1;
    }
    ns->pos = 0;
    ns->size = 0;
}

// Next item in order of addition to stack. Return 0-31 if there is a next item to move to. Returns -1 otherwise.
int tStack_next(tStack* const stack)
{
    _tStack* ns = *stack;
    
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

int tStack_get(tStack* const stack, int which)
{
    _tStack* ns = *stack;
    return ns->data[which];
}

int tStack_first(tStack* const stack)
{
    _tStack* ns = *stack;
    return ns->data[0];
}
