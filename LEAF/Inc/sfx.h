/*
 * sfx.h
 *
 *  Created on: Dec 23, 2019
 *      Author: josnyder
 */
#ifndef SFX_H_
#define SFX_H_

#include "audiostream.h"
#include "ui.h"

#define NUM_VOC_VOICES 8
#define NUM_OSC_PER_VOICE 3
#define NUM_VOC_OSC 1
#define INV_NUM_VOC_VOICES 0.125
#define INV_NUM_VOC_OSC 1
#define NUM_AUTOTUNE 2
#define NUM_RETUNE 1
#define OVERSAMPLER_RATIO 8
#define OVERSAMPLER_HQ FALSE

extern float presetKnobValues[PresetNil][NUM_ADC_CHANNELS];
extern uint8_t knobActive[NUM_ADC_CHANNELS];

extern tPoly poly;
extern tRamp polyRamp[NUM_VOC_VOICES];
extern tSawtooth osc[NUM_VOC_VOICES * NUM_OSC_PER_VOICE];

extern PlayMode samplerMode;
extern float sampleLength;

extern uint32_t freeze;

void initGlobalSFXObjects();

//1 vocoder internal poly
extern uint8_t numVoices;
extern uint8_t internalExternal;

void SFXVocoderAlloc();
void SFXVocoderFrame();
void SFXVocoderTick(float audioIn);
void SFXVocoderFree(void);

//4 pitch shift
void SFXPitchShiftAlloc();
void SFXPitchShiftFrame();
void SFXPitchShiftTick(float audioIn);
void SFXPitchShiftFree(void);

//5 neartune
extern uint8_t autotuneChromatic;

void SFXNeartuneAlloc();
void SFXNeartuneFrame();
void SFXNeartuneTick(float audioIn);
void SFXNeartuneFree(void);

//6 autotune
void SFXAutotuneAlloc();
void SFXAutotuneFrame();
void SFXAutotuneTick(float audioIn);
void SFXAutotuneFree(void);

//7 sampler - button press
extern uint8_t samplePlaying;

void SFXSamplerBPAlloc();
void SFXSamplerBPFrame();
void SFXSamplerBPTick(float audioIn);
void SFXSamplerBPFree(void);


//8 sampler - auto ch1
extern uint8_t triggerChannel;

void SFXSamplerAutoAlloc();
void SFXSamplerAutoFrame();
void SFXSamplerAutoTick(float audioIn);
void SFXSamplerAutoFree(void);

//10 distortion tanh
extern uint8_t distortionMode;

void SFXDistortionAlloc();
void SFXDistortionFrame();
void SFXDistortionTick(float audioIn);
void SFXDistortionFree(void);

//12 distortion wave folder
void SFXWaveFolderAlloc();
void SFXWaveFolderFrame();
void SFXWaveFolderTick(float audioIn);
void SFXWaveFolderFree(void);


//13 bitcrusher
void SFXBitcrusherAlloc();
void SFXBitcrusherFrame();
void SFXBitcrusherTick(float audioIn);
void SFXBitcrusherFree(void);


//14 delay
extern int delayShaper;

void SFXDelayAlloc();
void SFXDelayFrame();
void SFXDelayTick(float audioIn);
void SFXDelayFree(void);


//15 reverb
void SFXReverbAlloc();
void SFXReverbFrame();
void SFXReverbTick(float audioIn);
void SFXReverbFree(void);

//16 reverb2
void SFXReverb2Alloc();
void SFXReverb2Frame();
void SFXReverb2Tick(float audioIn);
void SFXReverb2Free(void);

//17 living string
void SFXLivingStringAlloc();
void SFXLivingStringFrame();
void SFXLivingStringTick(float audioIn);
void SFXLivingStringFree(void);

//17 living string
void SFXLivingStringSynthAlloc();
void SFXLivingStringSynthFrame();
void SFXLivingStringSynthTick(float audioIn);
void SFXLivingStringSynthFree(void);


//17 living string
void SFXClassicSynthAlloc();
void SFXClassicSynthFrame();
void SFXClassicSynthTick(float audioIn);
void SFXClassicSynthFree(void);


//17 living string
void SFXRhodesAlloc();
void SFXRhodesFrame();
void SFXRhodesTick(float audioIn);
void SFXRhodesFree(void);


// MIDI FUNCTIONS
void noteOn(int key, int velocity);
void noteOff(int key, int velocity);
void pitchBend(int data);
void sustainOn(void);
void sustainOff(void);
void toggleBypass(void);
void toggleSustain(void);

void calculateFreq(int voice);

float calculateTunedMidiNote(float tempNote);


void calculateNoteArray(void);
//void calculatePeriodArray(void);
float nearestNote(float period);
//float nearestPeriod(float period);

void clearNotes(void);

void ctrlInput(int ctrl, int value);


#endif /* SFX_H_ */
