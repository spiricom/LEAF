/*
 * ui.c
 *
 *  Created on: Feb 05, 2018
 *      Author: jeffsnyder
 */
#include "main.h"
#include "audiostream.h"
#include "sfx.h"
#include "oled.h"
#include "ui.h"
#include "tunings.h"
#include "eeprom.h"
#include "leaf.h"

uint16_t ADC_values[NUM_ADC_CHANNELS] __ATTR_RAM_D2;
float floatADC[NUM_ADC_CHANNELS];
float lastFloatADC[NUM_ADC_CHANNELS];
float floatADCUI[NUM_ADC_CHANNELS];
float adcHysteresisThreshold = 0.001f;

uint8_t buttonValues[NUM_BUTTONS]; // Actual state of the buttons
uint8_t buttonValuesPrev[NUM_BUTTONS];
uint8_t cleanButtonValues[NUM_BUTTONS]; // Button values after hysteresis
uint32_t buttonHysteresis[NUM_BUTTONS];
uint32_t buttonHysteresisThreshold = 5;
uint32_t buttonCounters[NUM_BUTTONS]; // How long a button has been in its current state
uint32_t buttonHoldThreshold = 200;
uint32_t buttonHoldMax = 200;
//uint8_t buttonPressed[NUM_BUTTONS];
//uint8_t buttonReleased[NUM_BUTTONS];

int8_t writeKnobFlag = -1;
int8_t writeButtonFlag = -1;
int8_t writeActionFlag = -1;

#define NUM_CHARACTERS_PER_PRESET_NAME 16
char* modeNames[PresetNil];
char* modeNamesDetails[PresetNil];
char* shortModeNames[PresetNil];

char* controlNames[NUM_ADC_CHANNELS + NUM_BUTTONS];
char* paramNames[PresetNil][NUM_ADC_CHANNELS + NUM_BUTTONS];
int8_t currentParamIndex = -1;
uint8_t orderedParams[8];

uint8_t buttonActionsSFX[NUM_BUTTONS][ActionNil];
uint8_t buttonActionsUI[NUM_BUTTONS][ActionNil];
float knobParams[NUM_ADC_CHANNELS];
int8_t cvAddParam = -1;
char* (*buttonActionFunctions[PresetNil])(VocodecButton, ButtonAction);

VocodecPresetType currentPreset = 0;
VocodecPresetType previousPreset = PresetNil;
uint8_t loadingPreset = 0;

void initModeNames(void)
{
	controlNames[0] = "1";
	controlNames[1] = "2";
	controlNames[2] = "3";
	controlNames[3] = "4";
	controlNames[4] = "5";
	controlNames[5] = "6";
	controlNames[ButtonA] = "A";
	controlNames[ButtonB] = "B";
	for (int i = 0; i < NUM_ADC_CHANNELS; i++)
	{
		floatADCUI[i] = -1.0f;
		orderedParams[i] = i;
	}
	orderedParams[6] = ButtonA;
	orderedParams[7] = ButtonB;

	modeNames[Vocoder] = "VOCODER";
	shortModeNames[Vocoder] = "VC";
	modeNamesDetails[Vocoder] = "";
	paramNames[Vocoder][0] = "VOLUME";
	paramNames[Vocoder][1] = "";
	paramNames[Vocoder][2] = "";
	paramNames[Vocoder][3] = "";
	paramNames[Vocoder][4] = "";
	paramNames[Vocoder][5] = "";
	paramNames[Vocoder][NUM_ADC_CHANNELS + ButtonA] = "POLY MONO";
	paramNames[Vocoder][NUM_ADC_CHANNELS + ButtonB] = "SOURCE";

	modeNames[Pitchshift] = "PITCHSHIFT";
	shortModeNames[Pitchshift] = "PS";
	modeNamesDetails[Pitchshift] = "";
	paramNames[Pitchshift][0] = "PITCH";
	paramNames[Pitchshift][1] = "FINE PITCH";
	paramNames[Pitchshift][2] = "F AMT";
	paramNames[Pitchshift][3] = "FORMANT";
	paramNames[Pitchshift][4] = "";
	paramNames[Pitchshift][5] = "";
	paramNames[Pitchshift][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[Pitchshift][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[AutotuneMono] = "NEARTUNE";
	shortModeNames[AutotuneMono] = "NT";
	modeNamesDetails[AutotuneMono] = "";
	paramNames[AutotuneMono][0] = "FID THRESH";
	paramNames[AutotuneMono][1] = "AMOUNT";
	paramNames[AutotuneMono][2] = "SPEED";
	paramNames[AutotuneMono][3] = "";
	paramNames[AutotuneMono][4] = "";
	paramNames[AutotuneMono][5] = "";
	paramNames[AutotuneMono][NUM_ADC_CHANNELS + ButtonA] = "AUTOCHRM ON";
	paramNames[AutotuneMono][NUM_ADC_CHANNELS + ButtonB] = "AUTOCHRM OFF";

	modeNames[AutotunePoly] = "AUTOTUNE";
	shortModeNames[AutotunePoly] = "AT";
	modeNamesDetails[AutotunePoly] = "";
	paramNames[AutotunePoly][0] = "FID THRESH";
	paramNames[AutotunePoly][1] = "";
	paramNames[AutotunePoly][2] = "";
	paramNames[AutotunePoly][3] = "";
	paramNames[AutotunePoly][4] = "";
	paramNames[AutotunePoly][5] = "";
	paramNames[AutotunePoly][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[AutotunePoly][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[SamplerButtonPress] = "SAMPLER BP";
	shortModeNames[SamplerButtonPress] = "SB";
	modeNamesDetails[SamplerButtonPress] = "PRESS BUTTON A";
	paramNames[SamplerButtonPress][0] = "START";
	paramNames[SamplerButtonPress][1] = "END";
	paramNames[SamplerButtonPress][2] = "SPEED";
	paramNames[SamplerButtonPress][3] = "CROSSFADE";
	paramNames[SamplerButtonPress][4] = "";
	paramNames[SamplerButtonPress][5] = "";
	paramNames[SamplerButtonPress][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[SamplerButtonPress][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[SamplerAutoGrab] = "AUTOSAMPLE";
	shortModeNames[SamplerAutoGrab] = "AS";
	modeNamesDetails[SamplerAutoGrab] = "";
	paramNames[SamplerAutoGrab][0] = "THRESHOLD";
	paramNames[SamplerAutoGrab][1] = "WINDOW";
	paramNames[SamplerAutoGrab][2] = "REL THRESH";
	paramNames[SamplerAutoGrab][3] = "CROSSFADE";
	paramNames[SamplerAutoGrab][4] = "";
	paramNames[SamplerAutoGrab][5] = "";
	paramNames[SamplerAutoGrab][NUM_ADC_CHANNELS + ButtonA] = "PLAY MODE";
	paramNames[SamplerAutoGrab][NUM_ADC_CHANNELS + ButtonB] = "TRIGGER CH";

	modeNames[Distortion] = "DISTORTION";
	shortModeNames[Distortion] = "DT";
	modeNamesDetails[Distortion] = "";
	paramNames[Distortion][0] = "GAIN";
	paramNames[Distortion][1] = "";
	paramNames[Distortion][2] = "";
	paramNames[Distortion][3] = "";
	paramNames[Distortion][4] = "";
	paramNames[Distortion][5] = "";
	paramNames[Distortion][NUM_ADC_CHANNELS + ButtonA] = "MODE";
	paramNames[Distortion][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[Wavefolder] = "WAVEFOLD";
	shortModeNames[Wavefolder] = "WF";
	modeNamesDetails[Wavefolder] = "SERGE STYLE";
	paramNames[Wavefolder][0] = "GAIN";
	paramNames[Wavefolder][1] = "OFFSET1";
	paramNames[Wavefolder][2] = "OFFSET2";
	paramNames[Wavefolder][3] = "";
	paramNames[Wavefolder][4] = "";
	paramNames[Wavefolder][5] = "";
	paramNames[Wavefolder][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[Wavefolder][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[BitCrusher] = "BITCRUSH";
	shortModeNames[BitCrusher] = "BC";
	modeNamesDetails[BitCrusher] = "AHH HALP ME";
	paramNames[BitCrusher][0] = "QUALITY";
	paramNames[BitCrusher][1] = "SAMP RATIO";
	paramNames[BitCrusher][2] = "ROUNDING";
	paramNames[BitCrusher][3] = "OPERATION";
	paramNames[BitCrusher][4] = "GAIN";
	paramNames[BitCrusher][5] = "";
	paramNames[BitCrusher][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[BitCrusher][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[Delay] = "DELAY";
	shortModeNames[Delay] = "DL";
	modeNamesDetails[Delay] = "";
	paramNames[Delay][0] = "DELAY_L";
	paramNames[Delay][1] = "DELAY_R";
	paramNames[Delay][2] = "FEEDBACK";
	paramNames[Delay][3] = "LOWPASS";
	paramNames[Delay][4] = "HIGHPASS";
	paramNames[Delay][5] = "";
	paramNames[Delay][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[Delay][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[Reverb] = "REVERB";
	shortModeNames[Reverb] = "RV";
	modeNamesDetails[Reverb] = "DATTORRO ALG";
	paramNames[Reverb][0] = "SIZE";
	paramNames[Reverb][1] = "IN LOPASS";
	paramNames[Reverb][2] = "IN HIPASS";
	paramNames[Reverb][3] = "FB LOPASS";
	paramNames[Reverb][4] = "FB GAIN";
	paramNames[Reverb][5] = "";
	paramNames[Reverb][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[Reverb][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[Reverb2] = "REVERB2";
	shortModeNames[Reverb2] = "RV";
	modeNamesDetails[Reverb2] = "NREVERB ALG";
	paramNames[Reverb2][0] = "SIZE";
	paramNames[Reverb2][1] = "LOWPASS";
	paramNames[Reverb2][2] = "HIGHPASS";
	paramNames[Reverb2][3] = "PEAK_FREQ";
	paramNames[Reverb2][4] = "PEAK_GAIN";
	paramNames[Reverb2][5] = "";
	paramNames[Reverb2][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[Reverb2][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[LivingString] = "STRING";
	shortModeNames[LivingString] = "LS";
	modeNamesDetails[LivingString] = "LIVING STRING";
	paramNames[LivingString][0] = "FREQ";
	paramNames[LivingString][1] = "DETUNE";
	paramNames[LivingString][2] = "DECAY";
	paramNames[LivingString][3] = "DAMPING";
	paramNames[LivingString][4] = "PICK_POS";
	paramNames[LivingString][5] = "";
	paramNames[LivingString][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[LivingString][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[LivingStringSynth] = "STRING SYNTH";
	shortModeNames[LivingStringSynth] = "SS";
	modeNamesDetails[LivingStringSynth] = "LIVING STRING";
	paramNames[LivingStringSynth][0] = "";
	paramNames[LivingStringSynth][1] = "";
	paramNames[LivingStringSynth][2] = "DECAY";
	paramNames[LivingStringSynth][3] = "DAMPING";
	paramNames[LivingStringSynth][4] = "PICK_POS";
	paramNames[LivingStringSynth][5] = "";
	paramNames[LivingStringSynth][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[LivingStringSynth][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[ClassicSynth] = "CLASSIC SYNTH";
	shortModeNames[ClassicSynth] = "CS";
	modeNamesDetails[ClassicSynth] = "VCO+VCF";
	paramNames[ClassicSynth][0] = "VOLUME";
	paramNames[ClassicSynth][1] = "LOWPASS";
	paramNames[ClassicSynth][2] = "";
	paramNames[ClassicSynth][3] = "";
	paramNames[ClassicSynth][4] = "";
	paramNames[ClassicSynth][5] = "";
	paramNames[ClassicSynth][NUM_ADC_CHANNELS + ButtonA] = "POLY MONO";
	paramNames[ClassicSynth][NUM_ADC_CHANNELS + ButtonB] = "";

	modeNames[Rhodes] = "RHODES";
	shortModeNames[Rhodes] = "RD";
	modeNamesDetails[Rhodes] = "PRETTY";
	paramNames[Rhodes][0] = "";
	paramNames[Rhodes][1] = "";
	paramNames[Rhodes][2] = "";
	paramNames[Rhodes][3] = "";
	paramNames[Rhodes][4] = "";
	paramNames[Rhodes][5] = "";
	paramNames[Rhodes][NUM_ADC_CHANNELS + ButtonA] = "";
	paramNames[Rhodes][NUM_ADC_CHANNELS + ButtonB] = "";
}

void buttonCheck(void)
{
	if (codecReady)
	{
		buttonValues[0] = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13); //edit
		buttonValues[1] = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12); //left
		buttonValues[2] = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14); //right
		buttonValues[3] = !HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11); //down
		buttonValues[4] = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15); //up
		buttonValues[5] = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);  // A
		buttonValues[6] = !HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7);  // B
		buttonValues[7] = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11); // C
		buttonValues[8] = !HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11); // D
		buttonValues[9] = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10); // E

		for (int i = 0; i < NUM_BUTTONS; i++)
		{
			if (buttonValues[i] != buttonValuesPrev[i])
			{
				buttonHysteresis[i]++;
			}
			if (cleanButtonValues[i] == 1)
			{
				buttonActionsSFX[i][ActionHoldContinuous] = TRUE;
				buttonActionsUI[i][ActionHoldContinuous] = TRUE;
				writeButtonFlag = i;
				writeActionFlag = ActionHoldContinuous;
			}
			if (buttonHysteresis[i] < buttonHysteresisThreshold)
			{
				if (buttonCounters[i] < buttonHoldMax) buttonCounters[i]++;
				if ((buttonCounters[i] >= buttonHoldThreshold) && (cleanButtonValues[i] == 1))
				{
					buttonActionsSFX[i][ActionHoldInstant] = TRUE;
					buttonActionsUI[i][ActionHoldInstant] = TRUE;
					writeButtonFlag = i;
					writeActionFlag = ActionHoldInstant;
				}
			}
			else
			{
				cleanButtonValues[i] = buttonValues[i];
				buttonHysteresis[i] = 0;
				buttonCounters[i] = 0;

				if (cleanButtonValues[i] == 1)
				{
					buttonActionsSFX[i][ActionPress] = TRUE;
					buttonActionsUI[i][ActionPress] = TRUE;
					writeButtonFlag = i;
					writeActionFlag = ActionPress;
				}
				else if (cleanButtonValues[i] == 0)
				{
					buttonActionsSFX[i][ActionRelease] = TRUE;
					buttonActionsUI[i][ActionRelease] = TRUE;
					writeButtonFlag = i;
					writeActionFlag = ActionRelease;
				}
				buttonValuesPrev[i] = buttonValues[i];
			}
		}

		// make some if statements if you want to find the "attack" of the buttons (getting the "press" action)
		// we'll need if statements for each button  - maybe should go to functions that are dedicated to each button?

		// TODO: buttons C and E are connected to pins that are used to set up the codec over I2C - we need to reconfigure those pins in some kind of button init after the codec is set up. not done yet.

		/// DEFINE GLOBAL BUTTON BEHAVIOR HERE

		if (buttonActionsUI[ButtonLeft][ActionPress] == 1)
		{
			previousPreset = currentPreset;

			if (currentPreset <= 0) currentPreset = PresetNil - 1;
			else currentPreset--;

			loadingPreset = 1;
			OLED_writePreset();
			writeCurrentPresetToFlash();
			clearButtonActions();
		}
		// right press
		if (buttonActionsUI[ButtonRight][ActionPress] == 1)
		{
			previousPreset = currentPreset;
			if (currentPreset >= PresetNil - 1) currentPreset = 0;
			else currentPreset++;

			loadingPreset = 1;
			OLED_writePreset();
			writeCurrentPresetToFlash();
			clearButtonActions();
		}
		if (buttonActionsUI[ButtonC][ActionPress] == 1)
		{
			//GFXsetFont(&theGFX, &DINCondensedBold9pt7b);

			if (buttonActionsUI[ButtonEdit][ActionHoldContinuous] == 0) buttonActionsUI[ButtonC][ActionPress] = 0;
		}
		if (buttonActionsUI[ButtonD][ActionPress] == 1)
		{
			if (currentTuning == 0)
			{
				currentTuning = NUM_TUNINGS - 1;
			}
			else
			{
				currentTuning = (currentTuning - 1);
			}
			changeTuning();
			OLED_writeTuning();
			buttonActionsUI[ButtonD][ActionPress] = 0;
		}
		if (buttonActionsUI[ButtonE][ActionPress] == 1)
		{

			currentTuning = (currentTuning + 1) % NUM_TUNINGS;
			changeTuning();
			OLED_writeTuning();
			buttonActionsUI[ButtonE][ActionPress] = 0;
		}

		if (buttonActionsUI[ButtonEdit][ActionPress])
		{
			OLED_writeEditScreen();
			buttonActionsUI[ButtonEdit][ActionPress] = 0;
		}
		if (buttonActionsUI[ButtonEdit][ActionHoldContinuous] == 1)
		{
			if (buttonActionsUI[ButtonC][ActionPress] == 1)
			{
				keyCenter = (keyCenter + 1) % 12;
				OLEDclearLine(SecondLine);
				OLEDwriteString("KEY: ", 5, 0, SecondLine);
				OLEDwritePitchClass(keyCenter+60, 64, SecondLine);
				buttonActionsUI[ButtonC][ActionPress] = 0;
			}
			if (buttonActionsUI[ButtonDown][ActionPress])
			{
				cvAddParam = -1;
				buttonActionsUI[ButtonDown][ActionPress] = 0;
			}
//			OLEDdrawFloatArray(audioDisplayBuffer, -1.0f, 1.0f, 128, displayBufferIndex, 0, BothLines);
		}
		if (buttonActionsUI[ButtonEdit][ActionRelease] == 1)
		{
			OLED_writePreset();
			buttonActionsUI[ButtonEdit][ActionRelease] = 0;
		}
		// Trying out an audio display
//		if (buttonActionsUI[ButtonEdit][ActionPress] == 1)
//		{
//			currentParamIndex++;
//			if (currentParamIndex > 7) currentParamIndex = 0;
//			int controlLen = strlen(controlNames[currentParamIndex]);
//			OLEDwriteString(controlNames[currentParamIndex], controlLen, 0, SecondLine);
//			OLEDwriteString(" ", 1, getCursorX(), SecondLine);
//			OLEDwriteString(paramNames[orderedParams[currentParamIndex]], getCursorX(), SecondLine);
//			buttonActionsUI[ButtonEdit][ActionPress] = 0;
//		}
	}
}

void adcCheck()
{
	//read the analog inputs and smooth them with ramps
	for (int i = 0; i < 6; i++)
	{
		//floatADC[i] = (float) (ADC_values[i]>>8) * INV_TWO_TO_8;
		floatADC[i] = (float) (ADC_values[i]>>6) * INV_TWO_TO_10;
	}
	for (int i = 0; i < 6; i++)
	{
		if (cvAddParam >= 0 && i == 5) continue;
		if (cvAddParam == i) floatADC[i] += floatADC[5];

		if (fastabsf(floatADC[i] - lastFloatADC[i]) > adcHysteresisThreshold)
		{
			if (buttonActionsUI[ButtonEdit][ActionHoldContinuous])
			{
				if (i != 5) cvAddParam = i;
				buttonActionsUI[ButtonEdit][ActionHoldContinuous] = 0;
			}
			lastFloatADC[i] = floatADC[i];
			writeKnobFlag = i;
			knobActive[i] = 1;
		}
		// only do the following check after the knob has already passed the above
		// check once and floatADCUI has been set in OLED_writeKnobParameter
		if (floatADCUI[i] >= 0.0f)
		{
			if (fastabsf(smoothedADC[i] - floatADCUI[i]) > adcHysteresisThreshold)
			{
				writeKnobFlag = i;
			}
		}
		if (knobActive[i]) tRamp_setDest(&adc[i], floatADC[i]);
	}
}

void clearButtonActions()
{
	for (int b = 0; b < ButtonNil; b++)
	{
		for (int a = 0; a < ActionNil; a++)
		{
			buttonActionsUI[b][a] = 0;
			buttonActionsSFX[b][a] = 0;
			writeButtonFlag = -1;
			writeActionFlag = -1;
		}
	}
}

void changeTuning()
{
	for (int i = 0; i < 12; i++)
	{
		centsDeviation[i] = tuningPresets[currentTuning][i];

	}
	if (currentTuning == 0)
	{
		//setLED_C(0);
	}
	else
	{
		///setLED_C(1);
	}
	if (currentPreset == AutotuneMono)
	{
		calculateNoteArray();
	}
}

void writeCurrentPresetToFlash(void)
{
	if((EE_WriteVariable(VirtAddVarTab[0],  currentPreset)) != HAL_OK)
	{
		Error_Handler();
	}
}

char* UIVocoderButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	if (buttonActionsUI[ButtonA][ActionPress] == 1)
	{
		writeString = (numVoices > 1) ? "POLY" : "MONO";
		buttonActionsUI[ButtonA][ActionPress] = 0;
	}
	if (buttonActionsUI[ButtonB][ActionPress] == 1)
	{
		writeString = internalExternal ? "EXTERNAL" : "INTERNAL";
		buttonActionsUI[ButtonB][ActionPress] = 0;
	}
	return writeString;
}

char* UIPitchShiftButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	return writeString;
}

char* UINeartuneButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	if (buttonActionsUI[ButtonA][ActionPress])
	{
		writeString = autotuneChromatic ? "AUTOCHROM ON" : "AUTOCHROM OFF";
		buttonActionsUI[ButtonA][ActionPress] = 0;
	}
	return writeString;
}

char* UIAutotuneButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	return writeString;
}

char* UISamplerBPButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	if (buttonActionsUI[ButtonDown][ActionPress])
	{
		OLEDclearLine(SecondLine);
		OLEDwriteFloat(sampleLength, 0, SecondLine);
		OLEDwriteString(samplePlaying ? "PLAYING" : "STOPPED", 7, 48, SecondLine);
		buttonActionsUI[ButtonDown][ActionPress] = 0;
	}
	if (buttonActionsUI[ButtonA][ActionHoldContinuous])
	{
		OLEDclearLine(SecondLine);
		OLEDwriteFloat(sampleLength, 0, SecondLine);
		OLEDwriteString("RECORDING", 9, 48, SecondLine);
		buttonActionsUI[ButtonA][ActionHoldContinuous] = 0;
	}
	if (buttonActionsUI[ButtonA][ActionRelease])
	{
		OLEDclearLine(SecondLine);
		OLEDwriteFloat(sampleLength, 0, SecondLine);
		OLEDwriteString(samplePlaying ? "PLAYING" : "STOPPED", 7, 48, SecondLine);
		buttonActionsUI[ButtonA][ActionRelease] = 0;
	}
	return writeString;
}

char* UISamplerAutoButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	if (buttonActionsUI[ButtonA][ActionPress])
	{
		if (samplerMode == PlayLoop)
		{
			writeString = "LOOP";
		}
		else if (samplerMode == PlayBackAndForth)
		{
			writeString = "BACK'N'FORTH";
		}
		buttonActionsUI[ButtonA][ActionPress] = 0;
	}
	if (buttonActionsUI[ButtonB][ActionPress])
	{
		writeString = triggerChannel ? "CH2 TRIG" : "CH1 TRIG";
		buttonActionsUI[ButtonB][ActionPress] = 0;
	}
	return writeString;
}

char* UIDistortionButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	if (buttonActionsUI[ButtonA][ActionPress])
	{
		writeString = distortionMode ? "SHAPER" : "TANH";
		buttonActionsUI[ButtonA][ActionPress] = 0;
	}
	return writeString;
}

char* UIWaveFolderButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	return writeString;
}

char* UIBitcrusherButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	return writeString;
}

char* UIDelayButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	if (buttonActionsUI[ButtonA][ActionPress])
	{
		writeString = delayShaper ? "SHAPER ON" : "SHAPER OFF";
		buttonActionsUI[ButtonA][ActionPress] = 0;
	}
	return writeString;
}

char* UIReverbButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	return writeString;
}

char* UIReverb2Buttons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	if (buttonActionsUI[ButtonA][ActionPress])
	{
		writeString = freeze ? "FREEZE" : "UNFREEZE";
		buttonActionsUI[ButtonA][ActionPress] = 0;
	}
	return writeString;
}

char* UILivingStringButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	return writeString;
}

char* UILivingStringSynthButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	return writeString;
}

char* UIClassicSynthButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	if (buttonActionsUI[ButtonA][ActionPress] == 1)
	{
		writeString = (numVoices > 1) ? "POLY" : "MONO";
		buttonActionsUI[ButtonA][ActionPress] = 0;
	}
	return writeString;
}

char* UIRhodesButtons(VocodecButton button, ButtonAction action)
{
	char* writeString = "";
	return writeString;
}
