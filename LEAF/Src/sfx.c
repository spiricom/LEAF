/*
 * sfx.c
 *
 *  Created on: Dec 23, 2019
 *      Author: josnyder
 */

#include "main.h"
#include "sfx.h"
#include "oled.h"
#include "tunings.h"
#include "MIDI_application.h"

float presetKnobValues[PresetNil][NUM_ADC_CHANNELS];
uint8_t knobActive[NUM_ADC_CHANNELS];

//audio objects
tFormantShifter fs;
tAutotune autotuneMono;
tAutotune autotunePoly;
tRetune retune;
tRetune retune2;
tRamp pitchshiftRamp;
tRamp nearWetRamp;
tRamp nearDryRamp;
tPoly poly;
tRamp polyRamp[NUM_VOC_VOICES];

tRamp comp;

tBuffer buff;
tBuffer buff2;
tSampler sampler;

tEnvelopeFollower envfollow;

tOversampler oversampler;

tLockhartWavefolder wavefolder1;
tLockhartWavefolder wavefolder2;
tLockhartWavefolder wavefolder3;
tLockhartWavefolder wavefolder4;
tHighpass wfHP;
tCrusher crush;
tCrusher crush2;

tTapeDelay delay;
tSVF delayLP;
tSVF delayHP;
tTapeDelay delay2;
tSVF delayLP2;
tSVF delayHP2;
tHighpass delayShaperHp;
tFeedbackLeveler feedbackControl;

tCycle testSine;

tExpSmooth smoother1;
tExpSmooth smoother2;
tExpSmooth smoother3;

tExpSmooth neartune_smoother;

#define NUM_STRINGS 6
tLivingString theString[NUM_STRINGS];

float myFreq;
float myDetune[NUM_STRINGS];
float synthDetune[NUM_VOC_VOICES][NUM_OSC_PER_VOICE];
//control objects
float notes[128];
float notePeriods[128];
float noteFreqs[128];
int chordArray[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int chromaticArray[12] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int lockArray[12];
float freq[NUM_VOC_VOICES];
float oversamplerArray[OVERSAMPLER_RATIO];


void initGlobalSFXObjects()
{
	calculateNoteArray();

	tPoly_init(&poly, NUM_VOC_VOICES);
	tPoly_setPitchGlideActive(&poly, FALSE);
	tPoly_setBendSamplesPerTick(&poly, 128);
	tPoly_setBendGlideTime(&poly, 1.0f);
	for (int i = 0; i < NUM_VOC_VOICES; i++)
	{
		tRamp_init(&polyRamp[i], 10.0f, 1);
	}

	tRamp_init(&nearWetRamp, 10.0f, 1);
	tRamp_init(&nearDryRamp, 10.0f, 1);
	tRamp_init(&comp, 10.0f, 1);

	// Note that these are the actual knob values
	// not the parameter value
	// (i.e. 0.5 for fine pitch is actually 0.0 fine pitch)
	presetKnobValues[Vocoder][0] = 0.4f; // volume
	presetKnobValues[Vocoder][1] = 0.0f;
	presetKnobValues[Vocoder][2] = 0.0f;
	presetKnobValues[Vocoder][3] = 0.0f;
	presetKnobValues[Vocoder][4] = 0.0f;
	presetKnobValues[Vocoder][5] = 0.0f;

	presetKnobValues[Pitchshift][0] = 1.0f; // pitch
	presetKnobValues[Pitchshift][1] = 0.5f; // fine pitch
	presetKnobValues[Pitchshift][2] = 0.0f; // f amount
	presetKnobValues[Pitchshift][3] = 0.5f; // formant
	presetKnobValues[Pitchshift][4] = 0.0f;
	presetKnobValues[Pitchshift][5] = 0.0f;

	presetKnobValues[AutotuneMono][0] = 1.0f; // fidelity thresh
	presetKnobValues[AutotuneMono][1] = 1.0f; // amount
	presetKnobValues[AutotuneMono][2] = 1.0f; // speed
	presetKnobValues[AutotuneMono][3] = 0.0f;
	presetKnobValues[AutotuneMono][4] = 0.0f;
	presetKnobValues[AutotuneMono][5] = 0.0f;

	presetKnobValues[AutotunePoly][0] = 1.0f; // fidelity thresh
	presetKnobValues[AutotunePoly][1] = 0.0f;
	presetKnobValues[AutotunePoly][2] = 0.0f;
	presetKnobValues[AutotunePoly][3] = 0.0f;
	presetKnobValues[AutotunePoly][4] = 0.0f;
	presetKnobValues[AutotunePoly][5] = 0.0f;

	presetKnobValues[SamplerButtonPress][0] = 0.0f; // start
	presetKnobValues[SamplerButtonPress][1] = 1.0f; // end
	presetKnobValues[SamplerButtonPress][2] = 0.75f; // speed
	presetKnobValues[SamplerButtonPress][3] = 0.25f; // crossfade
	presetKnobValues[SamplerButtonPress][4] = 0.0f;
	presetKnobValues[SamplerButtonPress][5] = 0.0f;

	presetKnobValues[SamplerAutoGrab][0] = 0.95f; // thresh
	presetKnobValues[SamplerAutoGrab][1] = 0.5f; // window
	presetKnobValues[SamplerAutoGrab][2] = 0.0f; // rel thresh
	presetKnobValues[SamplerAutoGrab][3] = 0.25f; // crossfade
	presetKnobValues[SamplerAutoGrab][4] = 0.0f;
	presetKnobValues[SamplerAutoGrab][5] = 0.0f;

	presetKnobValues[Distortion][0] = 0.25f; // gain
	presetKnobValues[Distortion][1] = 0.0f;
	presetKnobValues[Distortion][2] = 0.0f;
	presetKnobValues[Distortion][3] = 0.0f;
	presetKnobValues[Distortion][4] = 0.0f;
	presetKnobValues[Distortion][5] = 0.0f;

	presetKnobValues[Wavefolder][0] = 0.25f; // gain
	presetKnobValues[Wavefolder][1] = 0.5f; // offset1
	presetKnobValues[Wavefolder][2] = 0.5f; // offset2
	presetKnobValues[Wavefolder][3] = 0.5f; // offset3
	presetKnobValues[Wavefolder][4] = 0.0f;
	presetKnobValues[Wavefolder][5] = 0.0f;

	presetKnobValues[BitCrusher][0] = 0.1f; // quality
	presetKnobValues[BitCrusher][1] = 0.5f; // samp ratio
	presetKnobValues[BitCrusher][2] = 0.0f; // rounding
	presetKnobValues[BitCrusher][3] = 0.0f; // operation
	presetKnobValues[BitCrusher][4] = 0.5f; // gain
	presetKnobValues[BitCrusher][5] = 0.0f;

	presetKnobValues[Delay][0] = 0.25f; // delayL
	presetKnobValues[Delay][1] = 0.25f; // delayR
	presetKnobValues[Delay][2] = 0.5f; // feedback
	presetKnobValues[Delay][3] = 1.0f; // lowpass
	presetKnobValues[Delay][4] = 0.0f; // highpass
	presetKnobValues[Delay][5] = 0.0f;

	presetKnobValues[Reverb][0] = 0.5f; // size
	presetKnobValues[Reverb][1] = 0.5f; // in lowpass
	presetKnobValues[Reverb][2] = 0.5f; // in highpass
	presetKnobValues[Reverb][3] = 0.5f; // fb lowpass
	presetKnobValues[Reverb][4] = 0.5f; // fb gain
	presetKnobValues[Reverb][5] = 0.0f;

	presetKnobValues[Reverb2][0] = 0.2f; // size
	presetKnobValues[Reverb2][1] = 0.5f; // lowpass
	presetKnobValues[Reverb2][2] = 0.5f; // highpass
	presetKnobValues[Reverb2][3] = 0.5f; // peak freq
	presetKnobValues[Reverb2][4] = 0.5f; // peak gain
	presetKnobValues[Reverb2][5] = 0.0f;

	presetKnobValues[LivingString][0] = 0.5f; // freq
	presetKnobValues[LivingString][1] = 0.5f; // detune
	presetKnobValues[LivingString][2] = 0.5f; // decay
	presetKnobValues[LivingString][3] = 0.8f; // damping
	presetKnobValues[LivingString][4] = 0.5f; // pick pos
	presetKnobValues[LivingString][5] = 0.0f;

	presetKnobValues[LivingStringSynth][0] = 0.0f;
	presetKnobValues[LivingStringSynth][1] = 0.0f;
	presetKnobValues[LivingStringSynth][2] = 0.5f; // decay
	presetKnobValues[LivingStringSynth][3] = 0.8f; // damping
	presetKnobValues[LivingStringSynth][4] = 0.5f; // pick pos
	presetKnobValues[LivingStringSynth][5] = 0.0f;

	presetKnobValues[ClassicSynth][0] = 1.0f; // volume
	presetKnobValues[ClassicSynth][1] = 1.0f; // lowpass
	presetKnobValues[ClassicSynth][2] = 0.2f; // detune
	presetKnobValues[ClassicSynth][3] = 0.0f;
	presetKnobValues[ClassicSynth][4] = 0.0f;
	presetKnobValues[ClassicSynth][5] = 0.0f;

	presetKnobValues[Rhodes][0] = 0.5f;
	presetKnobValues[Rhodes][1] = 0.25f;
	presetKnobValues[Rhodes][2] = 0.25f;
	presetKnobValues[Rhodes][3] = 0.0f;
	presetKnobValues[Rhodes][4] = 0.0f;
	presetKnobValues[Rhodes][5] = 0.0f;
}

///1 vocoder internal poly

tTalkbox vocoder;
tSawtooth osc[NUM_VOC_VOICES * NUM_OSC_PER_VOICE];

uint8_t numVoices = NUM_VOC_VOICES;
uint8_t internalExternal = 0;

void SFXVocoderAlloc()
{
	tTalkbox_init(&vocoder, 1024);
	tPoly_setNumVoices(&poly, numVoices);
	for (int i = 0; i < NUM_VOC_VOICES; i++)
	{
		tSawtooth_initToPool(&osc[i], &smallPool);
	}
	setLED_A(numVoices == 1);
	setLED_B(internalExternal);
}

void SFXVocoderFrame()
{
	//glideTimeVoc = 5.0f;
	//tPoly_setPitchGlideTime(&poly, glideTimeVoc);
	if (buttonActionsSFX[ButtonA][ActionPress] == 1)
	{
		numVoices = (numVoices > 1) ? 1 : NUM_VOC_VOICES;
		tPoly_setNumVoices(&poly, numVoices);
		buttonActionsSFX[ButtonA][ActionPress] = 0;
		setLED_A(numVoices == 1);
	}
	if (buttonActionsSFX[ButtonB][ActionPress] == 1)
	{
		internalExternal = !internalExternal;
		buttonActionsSFX[ButtonB][ActionPress] = 0;
		setLED_B(internalExternal);
	}

	for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
	{
		tRamp_setDest(&polyRamp[i], (tPoly_getVelocity(&poly, i) > 0));
		calculateFreq(i);
		tSawtooth_setFreq(&osc[i], freq[i]);
	}

	if (tPoly_getNumActiveVoices(&poly) != 0) tRamp_setDest(&comp, 1.0f / tPoly_getNumActiveVoices(&poly));
}

void SFXVocoderTick(float audioIn)
{
	if (internalExternal == 1) sample = rightIn;
	else
	{
		tPoly_tickPitch(&poly);
		knobParams[0] = smoothedADC[0]; //vocoder volume

		for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
		{
			sample += tSawtooth_tick(&osc[i]) * tRamp_tick(&polyRamp[i]);
		}
		sample *= tRamp_tick(&comp);
	}
	sample *= knobParams[0];
	sample = tTalkbox_tick(&vocoder, sample, audioIn);
	sample = tanhf(sample);
	rightOut = sample;
}

void SFXVocoderFree(void)
{
	tTalkbox_free(&vocoder);
	for (int i = 0; i < NUM_VOC_VOICES; i++)
	{
		tSawtooth_freeFromPool(&osc[i], &smallPool);
	}
}

//4 pitch shift

void SFXPitchShiftAlloc()
{
	//tRetune_init(&retune, NUM_RETUNE, 2048, 1024);

	tFormantShifter_initToPool(&fs, 7, &smallPool);
	tRetune_init(&retune, NUM_RETUNE, 1024, 512);
	tRetune_init(&retune2, NUM_RETUNE, 1024, 512);
	tRamp_init(&pitchshiftRamp, 100.0f, 1);
	tRamp_setVal(&pitchshiftRamp, 1.0f);


	tExpSmooth_init(&smoother1, 0.0f, 0.01f);
	tExpSmooth_init(&smoother2, 0.0f, 0.01f);
	tExpSmooth_init(&smoother3, 0.0f, 0.01f);
}

void SFXPitchShiftFrame()
{


}

void SFXPitchShiftTick(float audioIn)
{
	//pitchFactor = (smoothedADC[0]*3.75f)+0.25f;


	float myPitchFactorCoarse = (smoothedADC[0]*2.0f) - 1.0f;
	float myPitchFactorFine = ((smoothedADC[1]*2.0f) - 1.0f) * 0.1f;
	float myPitchFactorCombined = myPitchFactorFine + myPitchFactorCoarse;
	knobParams[0] = myPitchFactorCombined;
	knobParams[1] = myPitchFactorCombined;
	float myPitchFactor = fastexp2f(myPitchFactorCombined);
	tRetune_setPitchFactor(&retune, myPitchFactor, 0);
	tRetune_setPitchFactor(&retune2, myPitchFactor, 0);


	knobParams[2] = LEAF_clip( 0.0f,((smoothedADC[2]) * 3.0f) - 0.2f,3.0f);

	knobParams[3] = fastexp2f((smoothedADC[3]*2.0f) - 1.0f);
	tExpSmooth_setDest(&smoother3, knobParams[2]);
	tFormantShifter_setIntensity(&fs, tExpSmooth_tick(&smoother3)+.1f);
	tFormantShifter_setShiftFactor(&fs, knobParams[3]);
	if (knobParams[2] > 0.01f)
	{
		tRamp_setDest(&pitchshiftRamp, -1.0f);
	}
	else
	{
		tRamp_setDest(&pitchshiftRamp, 1.0f);
	}

	float crossfadeVal = tRamp_tick(&pitchshiftRamp);
	float myGains[2];
	LEAF_crossfade(crossfadeVal, myGains);
	tExpSmooth_setDest(&smoother1, myGains[0]);
	tExpSmooth_setDest(&smoother2, myGains[1]);



	float formantsample = tanhf(tFormantShifter_remove(&fs, audioIn));




	float* samples = tRetune_tick(&retune2, formantsample);
	formantsample = samples[0];
	sample = audioIn;
	samples = tRetune_tick(&retune, sample);
	sample = samples[0];

	formantsample = tanhf(tFormantShifter_add(&fs, formantsample)) * tExpSmooth_tick(&smoother2) ;
	sample = (sample * (tExpSmooth_tick(&smoother1))) +  formantsample;
	rightOut = sample;


}

void SFXPitchShiftFree(void)
{
	tFormantShifter_freeFromPool(&fs, &smallPool);
	tRetune_free(&retune);
	tRetune_free(&retune2);

	tRamp_free(&pitchshiftRamp);

	tExpSmooth_free(&smoother1);
	tExpSmooth_free(&smoother2);
	tExpSmooth_free(&smoother3);
}




//5 neartune
uint8_t autotuneChromatic = 0;

void SFXNeartuneAlloc()
{
	tAutotune_init(&autotuneMono, 1, 512, 256);
	calculateNoteArray();
	tExpSmooth_init(&neartune_smoother, 100.0f, .007f);
	setLED_A(autotuneChromatic);
}

void SFXNeartuneFrame()
{

	if ((tPoly_getNumActiveVoices(&poly) != 0) || (autotuneChromatic == 1))
	{
		tRamp_setDest(&nearWetRamp, 1.0f);
		tRamp_setDest(&nearDryRamp, 0.0f);
	}
	else
	{
		tRamp_setDest(&nearWetRamp, 0.0f);
		tRamp_setDest(&nearDryRamp, 1.0f);
	}

	if (buttonActionsSFX[ButtonA][ActionPress])
	{
		autotuneChromatic = !autotuneChromatic;
		buttonActionsSFX[ButtonA][ActionPress] = 0;
		setLED_A(autotuneChromatic);
	}
}

void SFXNeartuneTick(float audioIn)
{
	knobParams[0] = 0.75f + (smoothedADC[0] * 0.22f);
	tAutotune_setFidelityThreshold(&autotuneMono, knobParams[0]);

	knobParams[1] = LEAF_clip(0.0f, smoothedADC[1] * 1.1f, 1.0f); // amount of forcing to new pitch
	knobParams[2] = smoothedADC[2]; //speed to get to desired pitch shift
	tExpSmooth_setFactor(&neartune_smoother, (knobParams[2] * .01f));

	float detectedPeriod = tAutotune_getInputPeriod(&autotuneMono);
	if (detectedPeriod > 0.0f)
	{
		float detectedNote = LEAF_frequencyToMidi(leaf.sampleRate / detectedPeriod);
		float desiredSnap = nearestNote(detectedPeriod);

		float destinationNote = (desiredSnap * knobParams[1]) + (detectedNote * (1.0f - knobParams[0]));
		float destinationFreq = LEAF_midiToFrequency(destinationNote);
		tExpSmooth_setDest(&neartune_smoother, destinationFreq);
	}
	tAutotune_setFreq(&autotuneMono, tExpSmooth_tick(&neartune_smoother), 0);

	float* samples = tAutotune_tick(&autotuneMono, audioIn);
	//tAutotune_setFreq(&autotuneMono, leaf.sampleRate / nearestPeriod(tAutotune_getInputPeriod(&autotuneMono)), 0);
	sample = samples[0] * tRamp_tick(&nearWetRamp);
	sample += audioIn * tRamp_tick(&nearDryRamp); // crossfade to dry signal if no notes held down.
	rightOut = sample;
}



void SFXNeartuneFree(void)
{
	tAutotune_free(&autotuneMono);
	tExpSmooth_free(&neartune_smoother);
}



//6 autotune
void SFXAutotuneAlloc()
{
	tAutotune_init(&autotunePoly, NUM_AUTOTUNE, 1024, 512);
	tPoly_setNumVoices(&poly, NUM_AUTOTUNE);

	//tAutotune_init(&autotunePoly, NUM_AUTOTUNE, 2048, 1024); //old settings
}

void SFXAutotuneFrame()
{
	for (int i = 0; i < tPoly_getNumVoices(&poly); ++i)
	{
		calculateFreq(i);
	}
	if (tPoly_getNumActiveVoices(&poly) != 0) tRamp_setDest(&comp, 1.0f / tPoly_getNumActiveVoices(&poly));
}

void SFXAutotuneTick(float audioIn)
{
	knobParams[0] = 0.75f + (smoothedADC[0] * 0.22f);
	tAutotune_setFidelityThreshold(&autotunePoly, knobParams[0]);
	tPoly_tickPitch(&poly);

	for (int i = 0; i < tPoly_getNumVoices(&poly); ++i)
	{
		tAutotune_setFreq(&autotunePoly, freq[i], i);
	}

	float* samples = tAutotune_tick(&autotunePoly, audioIn);

	for (int i = 0; i < tPoly_getNumVoices(&poly); ++i)
	{
		sample += samples[i] * tRamp_tick(&polyRamp[i]);
	}
	sample *= tRamp_tick(&comp);
	rightOut = sample;
}

void SFXAutotuneFree(void)
{
	tAutotune_free(&autotunePoly);
}



//7 sampler - button press
int samplePlayStart = 0;
int samplePlayEnd = 0;
float sampleLength = 0.0f;
int crossfadeLength = 0;
float samplerRate = 1.0f;
float maxSampleSizeSeconds = 1.0f;
uint8_t samplePlaying = 1;

void SFXSamplerBPAlloc()
{
	tBuffer_initToPool(&buff, leaf.sampleRate * 172.0f, &largePool);
	tBuffer_setRecordMode(&buff, RecordOneShot);
	tSampler_init(&sampler, &buff);
	tSampler_setMode(&sampler, PlayLoop);
}

void SFXSamplerBPFrame()
{

}

void SFXSamplerBPTick(float audioIn)
{
	int recordPosition = tBuffer_getRecordPosition(&buff);

	sampleLength = recordPosition * leaf.invSampleRate;
	knobParams[0] = smoothedADC[0] * sampleLength;
	knobParams[1] = smoothedADC[1] * sampleLength;
	knobParams[2] = (smoothedADC[2] - 0.5f) * 4.0f;
	knobParams[3] = smoothedADC[3] * 4000.0f;

	samplePlayStart = smoothedADC[0] * recordPosition;
	samplePlayEnd = smoothedADC[1] * recordPosition;
	samplerRate = knobParams[2];
	crossfadeLength = knobParams[3];
	tSampler_setStart(&sampler, samplePlayStart);
	tSampler_setEnd(&sampler, samplePlayEnd);
	tSampler_setRate(&sampler, samplerRate);
	tSampler_setCrossfadeLength(&sampler, crossfadeLength);

	if (buttonActionsSFX[ButtonDown][ActionPress])
	{
		if (samplePlaying)
		{
			samplePlaying = 0;
			tSampler_stop(&sampler);
		}
		else
		{
			samplePlaying = 1;
			tSampler_play(&sampler);
		}
		buttonActionsSFX[ButtonDown][ActionPress] = 0;
	}

	if (buttonActionsSFX[ButtonA][ActionPress])
	{
		tSampler_stop(&sampler);
		tBuffer_record(&buff);
		buttonActionsSFX[ButtonA][ActionPress] = 0;
		setLED_A(1);
	}
	if (buttonActionsSFX[ButtonA][ActionRelease])
	{
		tBuffer_stop(&buff);
		if (samplePlaying) tSampler_play(&sampler);
		tSampler_setStart(&sampler, samplePlayStart);
		tSampler_setEnd(&sampler, samplePlayEnd);
		tSampler_setRate(&sampler, samplerRate);
		tSampler_setCrossfadeLength(&sampler, crossfadeLength);
		buttonActionsSFX[ButtonA][ActionRelease] = 0;
		setLED_A(0);
	}

	tBuffer_tick(&buff, audioIn);
	sample = tanhf(tSampler_tick(&sampler));
	rightOut = sample;
}

void SFXSamplerBPFree(void)
{
	tBuffer_freeFromPool(&buff, &largePool);
	tSampler_free(&sampler);
}






//8 sampler - auto

volatile float currentPower = 0.0f;
volatile float previousPower = 0.0f;
float samp_thresh = 0.0f;
volatile int samp_triggered = 0;
uint32_t sample_countdown = 0;
PlayMode samplerMode = PlayLoop;
uint32_t powerCounter = 0;
uint8_t triggerChannel = 0;
uint8_t firstTrigger = 0;

void SFXSamplerAutoAlloc()
{
	tBuffer_initToPool(&buff2, leaf.sampleRate * 2.0f, &largePool);
	tBuffer_setRecordMode(&buff2, RecordOneShot);
	tSampler_init(&sampler, &buff2);
	tSampler_setMode(&sampler, PlayLoop);
	tEnvelopeFollower_init(&envfollow, 0.05f, 0.9999f);
	setLED_A(samplerMode == PlayBackAndForth);
	setLED_B(triggerChannel);
	firstTrigger = 1;
}

void SFXSamplerAutoFrame()
{

}

void SFXSamplerAutoTick(float audioIn)
{
	if (triggerChannel > 0) currentPower = tEnvelopeFollower_tick(&envfollow, rightIn);
	else currentPower = tEnvelopeFollower_tick(&envfollow, audioIn);

	samp_thresh = 1.0f - smoothedADC[0];
	knobParams[0] = samp_thresh;
	int window_size = smoothedADC[1] * 10000.0f;
	knobParams[1] = window_size;
	knobParams[3] = smoothedADC[3] * 1000.0f;
	crossfadeLength = knobParams[3];

	tSampler_setCrossfadeLength(&sampler, crossfadeLength);

	if ((currentPower > (samp_thresh)) && (currentPower > previousPower + 0.001f) && (samp_triggered == 0) && (sample_countdown == 0))
	{
		samp_triggered = 1;
		setLED_1(1);
		tBuffer_record(&buff2);
		if (firstTrigger)
		{
			tSampler_play(&sampler);
			firstTrigger = 0;
		}
		buff2->recordedLength = buff2->bufferLength;
		sample_countdown = window_size + 24;//arbitrary extra time to avoid resampling while playing previous sample - better solution would be alternating buffers and crossfading
		powerCounter = 1000;
	}

	if (sample_countdown > 0)
	{
		sample_countdown--;
	}


	tSampler_setEnd(&sampler,window_size);
	tBuffer_tick(&buff2, audioIn);
	//on it's way down
	if (currentPower <= previousPower)
	{
		if (powerCounter > 0)
		{
			powerCounter--;
		}
		else if (samp_triggered == 1)
		{
			setLED_1(0);
			samp_triggered = 0;
		}
	}
	if (buttonActionsSFX[ButtonA][ActionPress])
	{
		if (samplerMode == PlayLoop)
		{
			tSampler_setMode(&sampler, PlayBackAndForth);
			samplerMode = PlayBackAndForth;
			setLED_A(1);
			buttonActionsSFX[ButtonA][ActionPress] = 0;
		}
		else if (samplerMode == PlayBackAndForth)
		{
			tSampler_setMode(&sampler, PlayLoop);
			samplerMode = PlayLoop;
			setLED_A(0);
			buttonActionsSFX[ButtonA][ActionPress] = 0;
		}
	}
	if (buttonActionsSFX[ButtonB][ActionPress])
	{
		triggerChannel = (triggerChannel > 0) ? 0 : 1;
		buttonActionsSFX[ButtonB][ActionPress] = 0;
		setLED_B(triggerChannel);
	}
	sample = tSampler_tick(&sampler);
	rightOut = sample;
	previousPower = currentPower;
}

void SFXSamplerAutoFree(void)
{
	tBuffer_freeFromPool(&buff2, &largePool);
	tSampler_free(&sampler);
	tEnvelopeFollower_free(&envfollow);
}

//10 distortion tanh
uint8_t distortionMode = 0;

void SFXDistortionAlloc()
{
	leaf.clearOnAllocation = 1;
	tOversampler_init(&oversampler, OVERSAMPLER_RATIO, OVERSAMPLER_HQ);
	setLED_A(distortionMode);
	leaf.clearOnAllocation = 0;
}

void SFXDistortionFrame()
{
	if (buttonActionsSFX[ButtonA][ActionPress])
	{
		distortionMode = !distortionMode;
		buttonActionsSFX[ButtonA][ActionPress] = 0;
		setLED_A(distortionMode);
	}
}

void SFXDistortionTick(float audioIn)
{
	//knob 0 = gain
	sample = audioIn;
	knobParams[0] = ((smoothedADC[0] * 20.0f) + 1.0f); // 15.0f
	sample = sample * knobParams[0];

	tOversampler_upsample(&oversampler, sample, oversamplerArray);
	for (int i = 0; i < OVERSAMPLER_RATIO; i++)
	{
		if (distortionMode > 0) oversamplerArray[i] = LEAF_shaper(oversamplerArray[i], 1.0f);
		else oversamplerArray[i] = tanhf(oversamplerArray[i]);
	}
	sample = tOversampler_downsample(&oversampler, oversamplerArray);
	sample *= .65f; // .75f
	rightOut = sample;

	//sample = tOversampler_tick(&oversampler, sample, &tanhf);
}

void SFXDistortionFree(void)
{
	tOversampler_free(&oversampler);
}

//12 distortion wave folder
void SFXWaveFolderAlloc()
{
	leaf.clearOnAllocation = 1;
	tLockhartWavefolder_init(&wavefolder1);
	tLockhartWavefolder_init(&wavefolder2);
	tLockhartWavefolder_init(&wavefolder3);
	tLockhartWavefolder_init(&wavefolder4);
	tHighpass_init(&wfHP, 20.0f);
	tOversampler_init(&oversampler, 2, FALSE);
	leaf.clearOnAllocation = 0;
}

void SFXWaveFolderFrame()
{
}

void SFXWaveFolderTick(float audioIn)
{
	//knob 0 = gain
	sample = audioIn;
	knobParams[0] = (smoothedADC[0] * 20.0f) + 1.0f;

	knobParams[1] = (smoothedADC[1] * 2.0f) - 1.0f;

	knobParams[2] = (smoothedADC[2] * 2.0f) - 1.0f;

	//knobParams[3] = (smoothedADC[3] * 2.0f) - 1.0f;

	float gain = knobParams[0];


	sample = sample * gain * 0.33f;
	sample = sample + knobParams[1];

	tOversampler_upsample(&oversampler, sample, oversamplerArray);
	for (int i = 0; i < 2; i++)
	{
		oversamplerArray[i] = tLockhartWavefolder_tick(&wavefolder1, oversamplerArray[i]);
		//sample = sample * gain;
		oversamplerArray[i] = sample + knobParams[2];
		oversamplerArray[i] = tLockhartWavefolder_tick(&wavefolder2, oversamplerArray[i]);
		//oversamplerArray[i] = sample + knobParams[3];
		//sample *= .6f;
		//oversamplerArray[i] = tLockhartWavefolder_tick(&wavefolder3, oversamplerArray[i]);
		//sample = tLockhartWavefolder_tick(&wavefolder4, sample);
		oversamplerArray[i] *= .8f;
		oversamplerArray[i] = tanhf(oversamplerArray[i]);
	}
	sample = tHighpass_tick(&wfHP, tOversampler_downsample(&oversampler, oversamplerArray));
	rightOut = sample;

	/*
	sample = tLockhartWavefolder_tick(&wavefolder1, sample);
	//sample = sample * gain;
	sample = sample + knobParams[2];
	sample = tLockhartWavefolder_tick(&wavefolder2, sample);
	sample = sample + knobParams[3];
	//sample *= .6f;
	sample = tLockhartWavefolder_tick(&wavefolder3, sample);
	//sample = tLockhartWavefolder_tick(&wavefolder4, sample);
	sample *= .8f;
	sample = tanhf(sample);
	rightOut = sample;

	*/
}

void SFXWaveFolderFree(void)
{
	tLockhartWavefolder_free(&wavefolder1);
	tLockhartWavefolder_free(&wavefolder2);
	tLockhartWavefolder_free(&wavefolder3);
	tLockhartWavefolder_free(&wavefolder4);
	tHighpass_free(&wfHP);
	tOversampler_free(&oversampler);
}


//13 bitcrusher
void SFXBitcrusherAlloc()
{
	tCrusher_init(&crush);
	tCrusher_init(&crush2);
}

void SFXBitcrusherFrame()
{
}

void SFXBitcrusherTick(float audioIn)
{
	knobParams[0] = smoothedADC[0];
	tCrusher_setQuality (&crush, smoothedADC[0]);
	tCrusher_setQuality (&crush2, smoothedADC[0]);
	knobParams[1] = smoothedADC[1];
	tCrusher_setSamplingRatio (&crush, smoothedADC[1]);
	tCrusher_setSamplingRatio (&crush2, smoothedADC[1]);
	knobParams[2] = smoothedADC[2];
	tCrusher_setRound (&crush, smoothedADC[2]);
	tCrusher_setRound (&crush2, smoothedADC[2]);
	knobParams[3] = smoothedADC[3];
	tCrusher_setOperation (&crush, smoothedADC[3]);
	tCrusher_setOperation (&crush2, smoothedADC[3]);
	knobParams[4] = smoothedADC[4] + 1.0f;
	sample = tCrusher_tick(&crush, tanhf(audioIn * (smoothedADC[4] + 1.0f))) * .9f;
	rightOut = tCrusher_tick(&crush2, tanhf(rightIn * (smoothedADC[4] + 1.0f))) * .9f;

}

void SFXBitcrusherFree(void)
{
	tCrusher_free(&crush);
	tCrusher_free(&crush2);
}


//delay
int delayShaper = 0;

void SFXDelayAlloc()
{
	leaf.clearOnAllocation = 1;
	tTapeDelay_init(&delay, 2000, 30000);
	tTapeDelay_init(&delay2, 2000, 30000);
	tSVF_init(&delayLP, SVFTypeLowpass, 16000.f, .7f);
	tSVF_init(&delayHP, SVFTypeHighpass, 20.f, .7f);

	tSVF_init(&delayLP2, SVFTypeLowpass, 16000.f, .7f);
	tSVF_init(&delayHP2, SVFTypeHighpass, 20.f, .7f);

	tHighpass_init(&delayShaperHp, 20.0f);
	tFeedbackLeveler_init(&feedbackControl, .99f, 0.01, 0.125f, 0);
	delayShaper = 0;
	setLED_A(delayShaper);
	leaf.clearOnAllocation = 0;
}

void SFXDelayFrame()
{

	if (buttonActionsSFX[ButtonA][ActionPress])
	{
		delayShaper = !delayShaper;
		buttonActionsSFX[ButtonA][ActionPress] = 0;
		setLED_A(delayShaper);
	}
}
float delayFB1;
float delayFB2;

void SFXDelayTick(float audioIn)
{
	knobParams[0] = smoothedADC[0] * 30000.0f;
	knobParams[1] = smoothedADC[1] * 30000.0f;
	knobParams[2] = smoothedADC[2] * 1.1f;
	knobParams[3] = faster_mtof((smoothedADC[3] * 128) + 10.0f);
	knobParams[4] = faster_mtof((smoothedADC[4] * 128) + 10.0f);

	tSVF_setFreq(&delayLP, knobParams[3]);
	tSVF_setFreq(&delayLP2, knobParams[3]);
	tSVF_setFreq(&delayHP, knobParams[4]);
	tSVF_setFreq(&delayHP2, knobParams[4]);

	//swap tanh for shaper and add cheap fixed highpass after both shapers

	float input1, input2;

	if (delayShaper == 0)
	{
		input1 = tFeedbackLeveler_tick(&feedbackControl, tanhf(audioIn + (delayFB1 * knobParams[2])));
		input2 = tFeedbackLeveler_tick(&feedbackControl, tanhf(audioIn + (delayFB2 * knobParams[2])));
	}
	else
	{
		input1 = tFeedbackLeveler_tick(&feedbackControl, tHighpass_tick(&delayShaperHp, LEAF_shaper(audioIn + (delayFB1 * knobParams[2] * 0.5f), 0.5f)));
		input2 = tFeedbackLeveler_tick(&feedbackControl, tHighpass_tick(&delayShaperHp, LEAF_shaper(audioIn + (delayFB2 * knobParams[2] * 0.5f), 0.5f)));
	}
	tTapeDelay_setDelay(&delay, knobParams[0]);

	delayFB1 = tTapeDelay_tick(&delay, input1);

	tTapeDelay_setDelay(&delay2, knobParams[1]);

	delayFB2 = tTapeDelay_tick(&delay2, input2);

	delayFB1 = tSVF_tick(&delayLP, delayFB1) * 0.95f;
	delayFB2 = tSVF_tick(&delayLP2, delayFB2)* 0.95f;

	delayFB1 = tanhf(tSVF_tick(&delayHP, delayFB1)* 0.95f);
	delayFB2 = tanhf(tSVF_tick(&delayHP2, delayFB2)* 0.95f);

	sample = delayFB1;
	rightOut = delayFB2;

}

void SFXDelayFree(void)
{
	tTapeDelay_free(&delay);
	tTapeDelay_free(&delay2);
	tSVF_free(&delayLP);
	tSVF_free(&delayHP);
	tSVF_free(&delayLP2);
	tSVF_free(&delayHP2);

	tHighpass_free(&delayShaperHp);
	tFeedbackLeveler_free(&feedbackControl);
}



//reverb
uint32_t freeze = 0;

tDattorroReverb reverb;

void SFXReverbAlloc()
{
	leaf.clearOnAllocation = 1;
	tDattorroReverb_init(&reverb);
	tDattorroReverb_setMix(&reverb, 1.0f);
	freeze = 0;
	leaf.clearOnAllocation = 0;
}

void SFXReverbFrame()
{
	knobParams[1] = faster_mtof(smoothedADC[1]*135.0f);
	tDattorroReverb_setInputFilter(&reverb, knobParams[1]);
	knobParams[2] =  faster_mtof(smoothedADC[2]*128.0f);
	tDattorroReverb_setHP(&reverb, knobParams[2]);
	knobParams[3] = faster_mtof(smoothedADC[3]*135.0f);
	tDattorroReverb_setFeedbackFilter(&reverb, knobParams[3]);

}

void SFXReverbTick(float audioIn)
{
	float stereo[2];

	if (buttonActionsSFX[ButtonA][ActionPress])
	{
		if (freeze == 0)
		{
			freeze = 1;
			tDattorroReverb_setFreeze(&reverb, 1);
			setLED_1(1);
		}
		else
		{
			freeze = 0;
			tDattorroReverb_setFreeze(&reverb, 0);
			setLED_1(0);
		}
		buttonActionsSFX[ButtonA][ActionPress] = 0;
	}

	//tDattorroReverb_setInputDelay(&reverb, smoothedADC[1] * 200.f);
	audioIn *= 4.0f;
	knobParams[0] = smoothedADC[0];
	tDattorroReverb_setSize(&reverb, knobParams[0]);
	knobParams[4] = smoothedADC[4];
	tDattorroReverb_setFeedbackGain(&reverb, knobParams[4]);
	tDattorroReverb_tickStereo(&reverb, audioIn, stereo);
	sample = tanhf(stereo[0]) * 0.99f;
	rightOut = tanhf(stereo[1]) * 0.99f;
}

void SFXReverbFree(void)
{
	tDattorroReverb_free(&reverb);
}


//reverb2

tNReverb reverb2;
tSVF lowpass;
tSVF highpass;
tSVF bandpass;
tSVF lowpass2;
tSVF highpass2;
tSVF bandpass2;

void SFXReverb2Alloc()
{
	leaf.clearOnAllocation = 1;
	tNReverb_init(&reverb2, 1.0f);
	tNReverb_setMix(&reverb2, 1.0f);
	tSVF_init(&lowpass, SVFTypeLowpass, 18000.0f, 0.75f);
	tSVF_init(&highpass, SVFTypeHighpass, 40.0f, 0.75f);
	tSVF_init(&bandpass, SVFTypeBandpass, 2000.0f, 1.0f);
	tSVF_init(&lowpass2, SVFTypeLowpass, 18000.0f, 0.75f);
	tSVF_init(&highpass2, SVFTypeHighpass, 40.0f, 0.75f);
	tSVF_init(&bandpass2, SVFTypeBandpass, 2000.0f, 1.0f);
	freeze = 0;
	leaf.clearOnAllocation = 0;
}

void SFXReverb2Frame()
{


}


void SFXReverb2Tick(float audioIn)
{
	float stereoOuts[2];

	knobParams[0] = smoothedADC[0] * 4.0f;
	if (!freeze)
	{
		tNReverb_setT60(&reverb2, knobParams[0]);

	}
	else
	{
		tNReverb_setT60(&reverb2, 1000.0f);
		audioIn = 0.0f;
	}

	knobParams[1] = faster_mtof(smoothedADC[1]*135.0f);
	tSVF_setFreq(&lowpass, knobParams[1]);
	tSVF_setFreq(&lowpass2, knobParams[1]);
	knobParams[2] = faster_mtof(smoothedADC[2]*128.0f);
	tSVF_setFreq(&highpass, knobParams[2]);
	tSVF_setFreq(&highpass2, knobParams[2]);
	knobParams[3] = faster_mtof(smoothedADC[3]*128.0f);
	tSVF_setFreq(&bandpass, knobParams[3]);
	tSVF_setFreq(&bandpass2, knobParams[3]);

	knobParams[4] = (smoothedADC[4] * 4.0f) - 2.0f;

	if (buttonActionsSFX[ButtonA][ActionPress])
	{
		if (freeze == 0)
		{
			freeze = 1;
			setLED_1(1);
		}
		else
		{
			freeze = 0;
			setLED_1(0);
		}
		buttonActionsSFX[ButtonA][ActionPress] = 0;
	}


	tNReverb_tickStereo(&reverb2, audioIn, stereoOuts);
	float leftOut = tSVF_tick(&lowpass, stereoOuts[0]);
	leftOut = tSVF_tick(&highpass, leftOut);
	leftOut += tSVF_tick(&bandpass, leftOut) * knobParams[4];

	float rightOutTemp = tSVF_tick(&lowpass2, stereoOuts[1]);
	rightOutTemp = tSVF_tick(&highpass2, rightOutTemp);
	rightOutTemp += tSVF_tick(&bandpass, rightOutTemp) * knobParams[4];
	sample = tanhf(leftOut);
	rightOut = tanhf(rightOutTemp);

}

void SFXReverb2Free(void)
{
	tNReverb_free(&reverb2);
	tSVF_free(&lowpass);
	tSVF_free(&highpass);
	tSVF_free(&bandpass);
	tSVF_free(&lowpass2);
	tSVF_free(&highpass2);
	tSVF_free(&bandpass2);
}


//Living String
void SFXLivingStringAlloc()
{
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		myFreq = (randomNumber() * 300.0f) + 60.0f;
		myDetune[i] = (randomNumber() * 0.3f) - 0.15f;
		//tLivingString_init(&theString[i],  myFreq, 0.4f, 0.0f, 16000.0f, .999f, .5f, .5f, 0.1f, 0);
		tLivingString_init(&theString[i], 440.f, 0.2f, 0.f, 9000.f, 1.0f, 0.3f, 0.01f, 0.125f, 0);
	}
}

void SFXLivingStringFrame()
{
	knobParams[0] = mtof((smoothedADC[0] * 135.0f)); //freq
	knobParams[1] = smoothedADC[1]; //detune
	knobParams[2] = ((smoothedADC[2] * 0.09999999f) + 0.9f);
	knobParams[3] = mtof((smoothedADC[3] * 130.0f)+12.0f); //lowpass
	knobParams[4] = (smoothedADC[4] * 0.5) + 0.02f;//pickPos
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		tLivingString_setFreq(&theString[i], (i + (1.0f+(myDetune[i] * knobParams[1]))) * knobParams[0]);
		tLivingString_setDecay(&theString[i], knobParams[2]);
		tLivingString_setDampFreq(&theString[i], knobParams[3]);
		tLivingString_setPickPos(&theString[i], knobParams[4]);
	}

}


void SFXLivingStringTick(float audioIn)
{
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		sample += tLivingString_tick(&theString[i], audioIn);
	}
	sample *= 0.0625f;
	rightOut = sample;


}

void SFXLivingStringFree(void)
{
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		tLivingString_free(&theString[i]);
	}
}


//Living String
void SFXLivingStringSynthAlloc()
{
	tPoly_setNumVoices(&poly, NUM_STRINGS);
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		tLivingString_init(&theString[i], 440.f, 0.2f, 0.f, 9000.f, 1.0f, 0.0f, 0.01f, 0.125f, 1);
	}
}

void SFXLivingStringSynthFrame()
{
	//knobParams[0] = mtof((smoothedADC[0] * 135.0f)); //freq
	//knobParams[1] = smoothedADC[1]; //detune
	knobParams[2] = ((smoothedADC[2] * 0.09999999f) + 0.9f);
	knobParams[3] = mtof((smoothedADC[3] * 130.0f)+12.0f); //lowpass
	knobParams[4] = (smoothedADC[4] * 0.5) + 0.02f;//pickPos
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		//tLivingString_setFreq(&theString[i], (i + (1.0f+(myDetune[i] * knobParams[1]))) * knobParams[0]);
		tLivingString_setDecay(&theString[i], knobParams[2]);
		tLivingString_setDampFreq(&theString[i], knobParams[3]);
		tLivingString_setPickPos(&theString[i], knobParams[4]);
	}

	for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
	{
		//tRamp_setDest(&polyRamp[i], (tPoly_getVelocity(&poly, i) > 0));
		calculateFreq(i);
		tLivingString_setFreq(&theString[i], freq[i]);
		tLivingString_setTargetLev(&theString[i],(tPoly_getVelocity(&poly, i) > 0));
	}

}


void SFXLivingStringSynthTick(float audioIn)
{

	for (int i = 0; i < NUM_STRINGS; i++)
	{
		sample += tLivingString_tick(&theString[i], audioIn);
	}
	sample *= 0.0625f;
	rightOut = sample;
}

void SFXLivingStringSynthFree(void)
{
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		tLivingString_free(&theString[i]);
	}
}


//17 living string
void SFXClassicSynthAlloc()
{
	tPoly_setNumVoices(&poly, numVoices);
	for (int i = 0; i < NUM_VOC_VOICES; i++)
	{
		for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
		{
			tSawtooth_initToPool(&osc[(i * NUM_OSC_PER_VOICE) + j], &smallPool);
			synthDetune[i][j] = (leaf.random() * 0.2f) - 0.1f;
		}
	}

	tSVF_init(&delayLP, SVFTypeLowpass, 6000.0f, 1.0f);
	setLED_A(numVoices == 1);
}

void SFXClassicSynthFrame()
{
	if (buttonActionsSFX[ButtonA][ActionPress] == 1)
		{
			numVoices = (numVoices > 1) ? 1 : NUM_VOC_VOICES;
			tPoly_setNumVoices(&poly, numVoices);
			buttonActionsSFX[ButtonA][ActionPress] = 0;
			setLED_A(numVoices == 1);
		}

}

//make detuning more independant - maybe based on midi instead of frequency
//add filters
//waveshaper?

void SFXClassicSynthTick(float audioIn)
{
	tPoly_tickPitch(&poly);
	knobParams[0] = smoothedADC[0]; //synth volume
	knobParams[1] = faster_mtof(smoothedADC[1] * 128.0f); //lowpass cutoff

	knobParams[2] = smoothedADC[2]; //detune

	for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
	{
		tRamp_setDest(&polyRamp[i], (tPoly_getVelocity(&poly, i) > 0));
		float myMidiNote = calculateTunedMidiNote(tPoly_getPitch(&poly, i));
		float myFrequency = LEAF_midiToFrequency(myMidiNote);
		for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
		{
			//tSawtooth_setFreq(&osc[i + (j*NUM_VOC_VOICES)], LEAF_midiToFrequency(myMidiNote + synthDetune[i][j]));
			tSawtooth_setFreq(&osc[i + (j*NUM_VOC_VOICES)], myFrequency * (1.0f + (synthDetune[i][j] * knobParams[2] * 0.1f)));
		}
	}

	if (tPoly_getNumActiveVoices(&poly) != 0) tRamp_setDest(&comp, 1.0f / tPoly_getNumActiveVoices(&poly));

	for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
	{
		float amplitudeTemp = tRamp_tick(&polyRamp[i]);
		sample += tSawtooth_tick(&osc[i]) * amplitudeTemp;
		sample += tSawtooth_tick(&osc[i + NUM_VOC_VOICES]) * amplitudeTemp;
		sample += tSawtooth_tick(&osc[i] + (NUM_VOC_VOICES * 2)) * amplitudeTemp;
	}
	sample *= 0.125f * knobParams[0];
	tSVF_setFreq(&delayLP, knobParams[1]);
	sample = tSVF_tick(&delayLP, sample);
	sample = tanhf(sample);
}

void SFXClassicSynthFree(void)
{
	for (int i = 0; i < NUM_VOC_VOICES; i++)
	{
		for (int j = 0; j < NUM_OSC_PER_VOICE; j++)
		{
			tSawtooth_freeFromPool(&osc[(i * NUM_OSC_PER_VOICE) + j], &smallPool);
		}
	}
	tSVF_free(&delayLP);
}

tCycle FM_sines[NUM_VOC_VOICES][6];
float FM_freqRatios[6] = {1.0f, 3.0001f, 0.4999f, 3.0f, 4.0f, 5.0f};
float FM_indices[6] = {1000.0f, 100.0f, 300.0f, 300.0f, 800.0f, 900.0f};
float FM_decays[6] = {2000.0f, 300.0f, 800.0f, 3000.0f, 340.0f, 50.0f};
tEnvelope leakers[NUM_VOC_VOICES][2];
tADSR FM_envs[NUM_VOC_VOICES][6];
float feedback_output = 0.0f;

tCycle tremolo;

//FM Rhodes
void SFXRhodesAlloc()
{
	for (int i = 0; i < NUM_VOC_VOICES; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			tCycle_init(&FM_sines[i][j]);
			tADSR_init(&FM_envs[i][j], 7.0f, FM_decays[j], 0.3f, 100.0f);

		}
		tEnvelope_init(&leakers[i][0], 7.0f, 4000.0f, 0);
		tEnvelope_init(&leakers[i][1], 7.0f, 4000.0f, 0);
	}
	tCycle_init(&tremolo);
	tCycle_setFreq(&tremolo, 3.0f);
	tPoly_setNumVoices(&poly, NUM_VOC_VOICES);
	setLED_A(numVoices == 1);

}
void SFXRhodesFrame()
{
	if (buttonActionsSFX[ButtonA][ActionPress] == 1)
		{
			numVoices = (numVoices > 1) ? 1 : NUM_VOC_VOICES;
			tPoly_setNumVoices(&poly, numVoices);
			buttonActionsSFX[ButtonA][ActionPress] = 0;
			setLED_A(numVoices == 1);
		}
	tPoly_tickPitch(&poly);
	for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
	{

		calculateFreq(i);

	}

}
float lastsamp = 0.0f;
volatile uint8_t checkMe = 0;
void SFXRhodesTick(float audioIn)
{
	knobParams[0] = smoothedADC[0] * 2.0f; //brightness

	knobParams[1] = smoothedADC[1];  //tremolo amount
	knobParams[2] = smoothedADC[2] * 12.0f;  //tremolo speed

	tCycle_setFreq(&tremolo, knobParams[2]);

	for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
	{
		//tRamp_setDest(&polyRamp[i], (tPoly_getVelocity(&poly, i) > 0));
		//float myMidiNote = calculateTunedMidiNote(tPoly_getPitch(&poly, i));
		float myFrequency = freq[i];
		tCycle_setFreq(&FM_sines[i][5], (myFrequency  * FM_freqRatios[5]) + (FM_indices[5] * feedback_output * knobParams[0]));
		feedback_output = tCycle_tick(&FM_sines[i][5]);
		tCycle_setFreq(&FM_sines[i][4], (myFrequency  * FM_freqRatios[4]) + (FM_indices[4] * feedback_output * knobParams[0] * tADSR_tick(&FM_envs[i][5])));
		tCycle_setFreq(&FM_sines[i][3], (myFrequency  * FM_freqRatios[3]) + (FM_indices[3] * knobParams[0] * tCycle_tick(&FM_sines[i][4]) * tADSR_tick(&FM_envs[i][4])));
		tCycle_setFreq(&FM_sines[i][2], (myFrequency  * FM_freqRatios[2]) + (FM_indices[2] * knobParams[0] * tCycle_tick(&FM_sines[i][3]) * tADSR_tick(&FM_envs[i][3])));
		tCycle_setFreq(&FM_sines[i][1], myFrequency  * FM_freqRatios[1]);
		tCycle_setFreq(&FM_sines[i][0],( myFrequency  * FM_freqRatios[0]) + (FM_indices[0] * knobParams[0] * tCycle_tick(&FM_sines[i][1]) * tADSR_tick(&FM_envs[i][1])));

	}

	//if (tPoly_getNumActiveVoices(&poly) != 0) tRamp_setDest(&comp, 1.0f / tPoly_getNumActiveVoices(&poly));
	for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
	{
		//float amplitudeTemp = tRamp_tick(&polyRamp[i]);
		//amplitudeTemp = 1.0f;

		sample += tCycle_tick(&FM_sines[i][2]) * tADSR_tick(&FM_envs[i][2]) * tEnvelope_tick(&leakers[i][0]);
		sample += tCycle_tick(&FM_sines[i][0]) * tADSR_tick(&FM_envs[i][0]) * tEnvelope_tick(&leakers[i][1]);

	}
	float tremoloSignal = ((tCycle_tick(&tremolo) * 0.5f) + 0.5f) * knobParams[1];
	sample = sample * (tremoloSignal + (1.0f - knobParams[1]));

	sample *= 0.125f;
	//sample = LEAF_shaper(sample, 1.0f);
	//sample *= 0.8f;
}

void SFXRhodesFree(void)
{
	for (int i = 0; i < NUM_VOC_VOICES; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			tCycle_free(&FM_sines[i][j]);
			tADSR_free(&FM_envs[i][j]);
		}

		tEnvelope_free(&leakers[i][0]);
		tEnvelope_free(&leakers[i][1]);
	}
	tCycle_free(&tremolo);

}



// midi functions


void calculateFreq(int voice)
{
	float tempNote = tPoly_getPitch(&poly, voice);
	float tempPitchClass = ((((int)tempNote) - keyCenter) % 12 );
	float tunedNote = tempNote + centsDeviation[(int)tempPitchClass];
	freq[voice] = LEAF_midiToFrequency(tunedNote);
}

float calculateTunedMidiNote(float tempNote)
{
	float tempPitchClass = ((((int)tempNote) - keyCenter) % 12 );
	return (tempNote + centsDeviation[(int)tempPitchClass]);
}

void calculateNoteArray()
{
	for (int i = 0; i < 128; i++)
	{
		float tempNote = i;
		float tempPitchClass = ((((int)tempNote) - keyCenter) % 12 );
		float tunedNote = tempNote + centsDeviation[(int)tempPitchClass];
		notes[i] = tunedNote;
	}
}


float nearestNote(float period)
{
	float note = LEAF_frequencyToMidi(leaf.sampleRate / period);
	float leastDifference = fabsf(note - notes[0]);
	float difference;
	int index = 0;
	int* chord;

	if (autotuneChromatic > 0)
	{
		chord = chromaticArray;
	}
	else
	{
		chord = chordArray;
	}
	//if (autotuneLock > 0) chord = lockArray;

	for(int i = 1; i < 128; i++)
	{
		if (chord[i%12] > 0)
		{
			difference = fabsf(note - notes[i]);
			if(difference < leastDifference)
			{
				leastDifference = difference;
				index = i;
			}
		}
	}

	return notes[index];

}


void noteOn(int key, int velocity)
{
	if (!velocity)
	{
		if (chordArray[key%12] > 0) chordArray[key%12]--;

		int voice = tPoly_noteOff(&poly, key);
		if (voice >= 0)
		{
			tRamp_setDest(&polyRamp[voice], 0.0f);
			if (currentPreset == Rhodes)
			{
				for (int j = 0; j < 6; j++)
				{
					tADSR_off(&FM_envs[voice][j]);
				}

			}
		}

		for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
		{
			if (tPoly_isOn(&poly, i) == 1)
			{
				tRamp_setDest(&polyRamp[i], 1.0f);
				calculateFreq(i);


			}



		}

		setLED_USB(0);
	}
	else
	{
		chordArray[key%12]++;



		int whichVoice = tPoly_noteOn(&poly, key, velocity);
		if (whichVoice >= 0)
		{
			if (currentPreset == Rhodes)
			{
				for (int j = 0; j < 6; j++)
				{
					tADSR_on(&FM_envs[whichVoice][j], velocity * 0.0078125f);

				}
				tEnvelope_on(&leakers[whichVoice][0], 1.0f);
				tEnvelope_on(&leakers[whichVoice][1], 1.0f);
			}
		}
		for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
		{
			if (tPoly_isOn(&poly, i) == 1)
			{
				tRamp_setDest(&polyRamp[i], 1.0f);
				calculateFreq(i);
			}
		}


		setLED_2(1);
	}
}

void noteOff(int key, int velocity)
{
	if (chordArray[key%12] > 0) chordArray[key%12]--;

	int voice = tPoly_noteOff(&poly, key);
	if (voice >= 0)
	{
		tRamp_setDest(&polyRamp[voice], 0.0f);
		if (currentPreset == Rhodes)
		{
			for (int j = 0; j < 6; j++)
			{
				tADSR_off(&FM_envs[voice][j]);
			}
		}
	}


	for (int i = 0; i < tPoly_getNumVoices(&poly); i++)
	{
		if (tPoly_isOn(&poly, i) == 1)
		{
			tRamp_setDest(&polyRamp[i], 1.0f);
			calculateFreq(i);


		}

	}

	setLED_2(0);
}


void pitchBend(int data)
{
	tPoly_setPitchBend(&poly, (data - 8192) * 0.000244140625f);
}


void sustainOff()
{

}

void sustainOn()
{

}

void toggleBypass()
{

}

void toggleSustain()
{

}

void ctrlInput(int ctrl, int value)
{

}

