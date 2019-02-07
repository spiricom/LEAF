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


/* Poly Handler */
static void nodeListInit(tPoly* poly)
{
    for (int16_t i = 0; i < 128; i++)
    {
        poly->midiNodes[i].midiNote.pitch = i;
        poly->midiNodes[i].midiNote.velocity = 0;
        poly->midiNodes[i].midiNote.on = OFALSE;
    }
}

// Initially everything is off, init as such
static void offListInit(tPoly* poly)
{
    tMidiNode* prevNode = NULL;
    tMidiNode* curNode = &poly->midiNodes[0];
    poly->offListFirst = curNode;
    
    for (int16_t i = 1; i < 128; i++)
    {
        curNode->prev = prevNode;
        curNode->next = &poly->midiNodes[i];
        
        prevNode = curNode;
        curNode = &poly->midiNodes[i];
    }
    // Set the final node
    curNode->prev = prevNode;
    curNode->next = NULL;
}

static void onListInit(tPoly* poly)
{
    poly->onListFirst = NULL;
}

void    tPoly_init(tPoly* const poly)
{
    nodeListInit(poly);
    offListInit(poly);
    onListInit(poly);
}


tMidiNote* tPoly_getMidiNote(tPoly* const poly, int8_t voiceIndex)
{
    tMidiNote* midiNote = NULL;
    
    tMidiNode* currNode = poly->onListFirst;
    
    int8_t i = 0;
    while (i < voiceIndex && currNode != NULL)
    {
        currNode = currNode->next;
        i++;
    }
    
    if (currNode != NULL)
        midiNote = &(currNode->midiNote);
    
    return midiNote;
}

static void removeNoteFromOffList(tPoly* const poly, int8_t midiNoteNumber)
{
    // If this has no prev, this is the first node on the OFF list
    if (poly->midiNodes[midiNoteNumber].prev == NULL)
        poly->offListFirst = poly->midiNodes[midiNoteNumber].next;
    
    // Awkward handling to avoid deref null pointers
    if (poly->midiNodes[midiNoteNumber].prev != NULL)
        poly->midiNodes[midiNoteNumber].prev->next = poly->midiNodes[midiNoteNumber].next;
    
    if (poly->midiNodes[midiNoteNumber].next != NULL)
        poly->midiNodes[midiNoteNumber].next->prev = poly->midiNodes[midiNoteNumber].prev;
    
    poly->midiNodes[midiNoteNumber].next = NULL;
    poly->midiNodes[midiNoteNumber].prev = NULL;
}

static void prependNoteToOnList(tPoly* poly, int midiNoteNumber)
{
    if (poly->onListFirst != NULL)
    {
        poly->midiNodes[midiNoteNumber].next = poly->onListFirst;
        poly->onListFirst->prev = &poly->midiNodes[midiNoteNumber];
    }
    poly->onListFirst = &poly->midiNodes[midiNoteNumber];
}


// TODO: Fail gracefully on odd MIDI situations
//       For example, getting a note off for an already on note and vice-versa
void tPoly_noteOn(tPoly* const poly, int midiNoteNumber, float velocity)
{
    removeNoteFromOffList(poly, midiNoteNumber);
    // Set the MIDI note on accordingly
    poly->midiNodes[midiNoteNumber].midiNote.velocity = velocity;
    poly->midiNodes[midiNoteNumber].midiNote.on = OTRUE;
    prependNoteToOnList(poly, midiNoteNumber);
}

// Unfortunately similar code to removeNoteFromOffList without any clear way of factoring out to a helper function
static void removeNoteFromOnList(tPoly* const poly, int8_t midiNoteNumber)
{
    // If this has no prev, this is the first node on the OFF list
    if (poly->midiNodes[midiNoteNumber].prev == NULL)
        poly->onListFirst = poly->midiNodes[midiNoteNumber].next;
    
    // Awkward handling to avoid deref null pointers
    if (poly->midiNodes[midiNoteNumber].prev != NULL)
        poly->midiNodes[midiNoteNumber].prev->next = poly->midiNodes[midiNoteNumber].next;
    
    if (poly->midiNodes[midiNoteNumber].next != NULL)
        poly->midiNodes[midiNoteNumber].next->prev = poly->midiNodes[midiNoteNumber].prev;
    
    poly->midiNodes[midiNoteNumber].next = NULL;
    poly->midiNodes[midiNoteNumber].prev = NULL;
}

// Unfortunately similar code to prependNoteToOnList without any clear way of factoring out to a helper function
static void prependNoteToOffList(tPoly* const poly, int midiNoteNumber)
{
    if (poly->offListFirst != NULL)
    {
        poly->midiNodes[midiNoteNumber].next = poly->offListFirst;
        poly->offListFirst->prev = &poly->midiNodes[midiNoteNumber];
    }
    poly->offListFirst = &poly->midiNodes[midiNoteNumber];
}


void tPoly_noteOff(tPoly* const poly, int midiNoteNumber)
{
    removeNoteFromOnList(poly, midiNoteNumber);
    // Set the MIDI note on accordingly
    poly->midiNodes[midiNoteNumber].midiNote.velocity = 0;
    poly->midiNodes[midiNoteNumber].midiNote.on = OFALSE;
    prependNoteToOffList(poly, midiNoteNumber);
}


// MPOLY
void tMPoly_init(tMPoly* const poly, int numVoices)
{
    poly->numVoices = numVoices;
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
    
    for (int i = 0; i < MPOLY_NUM_MAX_VOICES; ++i)
    {
        poly->voices[i][0] = -1;
        poly->firstReceived[i] = 0;
        
        poly->ramp[i] = (tRamp*) leaf_alloc(sizeof(tRamp));
        
        tRamp_init(poly->ramp[i], 5.0f, 1);
    }
    
    poly->glideTime = 5.0f;
    
    poly->pitchBend = 64;
    poly->pitchBendAmount = 2.0f;
    
    poly->stack = (tStack*) leaf_alloc(sizeof(tStack));
    tStack_init(poly->stack);
    
    poly->orderStack = (tStack*) leaf_alloc(sizeof(tStack));
    tStack_init(poly->orderStack);
}

// Only needs to be used for pitch glide
void tMPoly_tick(tMPoly* poly)
{
    for (int i = 0; i < MPOLY_NUM_MAX_VOICES; ++i)
    {
        tRamp_tick(poly->ramp[i]);
    }
}

//instead of including in dacsend, should have a separate pitch bend ramp, that is added when the ramps are ticked and sent to DAC
void tMPoly_pitchBend(tMPoly* const poly, int pitchBend)
{
    poly->pitchBend = pitchBend;
}

int tMPoly_noteOn(tMPoly* const poly, int note, uint8_t vel)
{
    // if not in keymap or already on stack, dont do anything. else, add that note.
    if (tStack_contains(poly->stack, note) >= 0) return -1;
    else
    {
        tMPoly_orderedAddToStack(poly, note);
        tStack_add(poly->stack, note);
        
        int alteredVoice = -1;
        oBool found = OFALSE;
        for (int i = 0; i < poly->numVoices; i++)
        {
            if (poly->voices[i][0] < 0)    // if inactive voice, give this note to voice
            {
                if (poly->firstReceived[i] == 0)
                {
                    tRamp_setVal(poly->ramp[i], note); // can't be 1.0f, causes first note to be off pitch
                    poly->firstReceived[i] = 1;
                }
                else
                {
                    tRamp_setTime(poly->ramp[i], poly->glideTime);
                }
                
                found = OTRUE;
                
                poly->voices[i][0] = note;
                poly->voices[i][1] = vel;
                poly->lastVoiceToChange = i;
                poly->notes[note][0] = vel;
                poly->notes[note][1] = i;
                
                tRamp_setDest(poly->ramp[i], poly->voices[i][0]);
                
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
                    poly->lastVoiceToChange = j;
                    int oldNote = poly->voices[whichVoice][0];
                    poly->voices[whichVoice][0] = note;
                    poly->voices[whichVoice][1] = vel;
                    poly->notes[oldNote][1] = -1; //mark the stolen voice as inactive (in the second dimension of the notes array)
                    poly->notes[note][0] = vel;
                    poly->notes[note][1] = whichVoice;
                    
                    tRamp_setTime(poly->ramp[whichVoice], poly->glideTime);
                    tRamp_setDest(poly->ramp[whichVoice], poly->voices[whichVoice][0]);
                    
                    alteredVoice = whichVoice;
                    
                    break;
                }
            }
        }
        return alteredVoice;
    }
}


int16_t noteToTest = -1;

int tMPoly_noteOff(tMPoly* const poly, uint8_t note)
{
    tStack_remove(poly->stack, note);
    tStack_remove(poly->orderStack, note);
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
        for (int j = 0; j < tStack_getSize(poly->stack); ++j)
        {
            noteToTest = tStack_get(poly->stack, j);
            
            if (poly->notes[noteToTest][1] < 0) //if there is a stolen note waiting (marked inactive but on the stack)
            {
                poly->voices[deactivatedVoice][0] = noteToTest; //set the newly free voice to use the old stolen note
                tRamp_setDest(poly->ramp[deactivatedVoice], poly->voices[deactivatedVoice][0]);
                poly->voices[deactivatedVoice][1] = poly->notes[noteToTest][0]; // set the velocity of the voice to be the velocity of that note
                poly->notes[noteToTest][1] = deactivatedVoice; //mark that it is no longer stolen and is now active
                return -1;
            }
        }
    }
    return deactivatedVoice;
}

void tMPoly_orderedAddToStack(tMPoly* const poly, uint8_t noteVal)
{
    
    uint8_t j;
    int myPitch, thisPitch, nextPitch;
    
    tStack* ns = poly->orderStack;
    
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

void tMPoly_setNumVoices(tMPoly* const poly, uint8_t numVoices)
{
    poly->numVoices = (numVoices > MPOLY_NUM_MAX_VOICES) ? MPOLY_NUM_MAX_VOICES : numVoices;
}

void tMPoly_setPitchGlideTime(tMPoly* const poly, float t)
{
    if (poly->glideTime == t) return;
    poly->glideTime = t;
    for (int i = 0; i < MPOLY_NUM_MAX_VOICES; ++i)
    {
        tRamp_setTime(poly->ramp[i], poly->glideTime);
    }
}

int tMPoly_getNumVoices(tMPoly* const poly)
{
    return poly->numVoices;
}

float tMPoly_getPitch(tMPoly* const poly, uint8_t voice)
{
    //float pitchBend = ((float)(poly->pitchBend - 8192) / 8192.0f) * poly->pitchBendAmount;
    return tRamp_sample(poly->ramp[voice]);// + pitchBend;
    return poly->voices[voice][0];
}

int tMPoly_getVelocity(tMPoly* const poly, uint8_t voice)
{
    return poly->voices[voice][1];
}

int tMPoly_isOn(tMPoly* const poly, uint8_t voice)
{
    return (poly->voices[voice][0] > 0) ? 1 : 0;
}

