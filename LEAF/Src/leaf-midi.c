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
void tPoly_init(tPoly* const poly, int maxNumVoices)
{
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

void tPoly_free(tPoly* const poly)
{
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
}

void tPoly_tickPitch(tPoly* poly)
{
    tPoly_tickPitchGlide(poly);
    tPoly_tickPitchBend(poly);
}

void tPoly_tickPitchGlide(tPoly* poly)
{
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        tRamp_tick(&poly->ramps[i]);
    }
}

void tPoly_tickPitchBend(tPoly* poly)
{
    tRamp_tick(&poly->pitchBendRamp);
}

//instead of including in dacsend, should have a separate pitch bend ramp, that is added when the ramps are ticked and sent to DAC
void tPoly_setPitchBend(tPoly* const poly, float pitchBend)
{
    poly->pitchBend = pitchBend;
    tRamp_setDest(&poly->pitchBendRamp, poly->pitchBend);
}

int tPoly_noteOn(tPoly* const poly, int note, uint8_t vel)
{
    // if not in keymap or already on stack, dont do anything. else, add that note.
    if (tStack_contains(&poly->stack, note) >= 0) return -1;
    else
    {
        tPoly_orderedAddToStack(poly, note);
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

int tPoly_noteOff(tPoly* const poly, uint8_t note)
{
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

void tPoly_orderedAddToStack(tPoly* const poly, uint8_t noteVal)
{
    
    uint8_t j;
    int myPitch, thisPitch, nextPitch;
    
    tStack ns = poly->orderStack;
    
    int whereToInsert = 0;
    
    for (j = 0; j < ns.size; j++)
    {
        myPitch = noteVal;
        thisPitch = ns.data[j];
        nextPitch = ns.data[j+1];
        
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
    for (j = ns.size; j > whereToInsert; j--)
    {
        ns.data[j] = ns.data[(j - 1)];
    }
    
    //then, insert the new note into the front of the stack
    ns.data[whereToInsert] =  noteVal;
    
    ns.size++;
    
}

void tPoly_setNumVoices(tPoly* const poly, uint8_t numVoices)
{
    poly->numVoices = (numVoices > poly->maxNumVoices) ? poly->maxNumVoices : numVoices;
}

void tPoly_setPitchGlideActive(tPoly* const poly, oBool isActive)
{
    poly->pitchGlideIsActive = isActive;
}

void tPoly_setPitchGlideTime(tPoly* const poly, float t)
{
    poly->glideTime = t;
    for (int i = 0; i < poly->maxNumVoices; ++i)
    {
        tRamp_setTime(&poly->ramps[i], poly->glideTime);
    }
}

int tPoly_getNumVoices(tPoly* const poly)
{
    return poly->numVoices;
}

float tPoly_getPitch(tPoly* const poly, uint8_t voice)
{
    return tRamp_sample(&poly->ramps[voice]) + tRamp_sample(&poly->pitchBendRamp);
}

int tPoly_getKey(tPoly* const poly, uint8_t voice)
{
    return poly->voices[voice][0];
}

int tPoly_getVelocity(tPoly* const poly, uint8_t voice)
{
    return poly->voices[voice][1];
}

int tPoly_isOn(tPoly* const poly, uint8_t voice)
{
    return (poly->voices[voice][0] > 0) ? 1 : 0;
}

