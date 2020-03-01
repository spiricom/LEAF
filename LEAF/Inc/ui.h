/*
 * ui.h
 *
 *  Created on: Aug 30, 2019
 *      Author: jeffsnyder
 */
#ifndef UI_H_
#define UI_H_

#define NUM_ADC_CHANNELS 6
#define NUM_BUTTONS 10

//PresetNil is used as a counter for the size of the enum
typedef enum _VocodecPresetType
{
	Vocoder = 0,
	Pitchshift,
	AutotuneMono,
	AutotunePoly,
	SamplerButtonPress,
	SamplerAutoGrab,
	Distortion,
	Wavefolder,
	BitCrusher,
	Delay,
	Reverb,
	Reverb2,
	LivingString,
	LivingStringSynth,
	ClassicSynth,
	Rhodes,
	PresetNil
} VocodecPresetType;

typedef enum _VocodecButton
{
	ButtonEdit = 0,
	ButtonLeft,
	ButtonRight,
	ButtonDown,
	ButtonUp,
	ButtonA,
	ButtonB,
	ButtonC,
	ButtonD,
	ButtonE,
	ButtonNil
} VocodecButton;

typedef enum _ButtonAction
{
	ActionPress = 0,
	ActionRelease,
	ActionHoldInstant,
	ActionHoldContinuous,
	ActionNil
} ButtonAction;

extern uint16_t ADC_values[NUM_ADC_CHANNELS];

extern uint8_t buttonValues[NUM_BUTTONS];
//extern uint8_t buttonPressed[NUM_BUTTONS];
//extern uint8_t buttonReleased[NUM_BUTTONS];

extern int8_t writeKnobFlag;
extern int8_t writeButtonFlag;
extern int8_t writeActionFlag;

extern float floatADCUI[NUM_ADC_CHANNELS];

extern uint8_t currentPreset;
extern uint8_t previousPreset;
extern uint8_t loadingPreset;
// Display values
extern char* modeNames[PresetNil];
extern char* modeNamesDetails[PresetNil];
extern char* shortModeNames[PresetNil];
extern char* paramNames[PresetNil][NUM_ADC_CHANNELS + NUM_BUTTONS];
extern float knobParams[NUM_ADC_CHANNELS];
extern int8_t cvAddParam;
extern uint8_t buttonActionsSFX[NUM_BUTTONS][ActionNil];
extern char* (*buttonActionFunctions[PresetNil])(VocodecButton, ButtonAction);

void initModeNames(void);

void buttonCheck(void);

void adcCheck(void);

void clearButtonActions(void);

void changeTuning(void);

void writeCurrentPresetToFlash(void);

char* UIVocoderButtons(VocodecButton button, ButtonAction action);
char* UIPitchShiftButtons(VocodecButton button, ButtonAction action);
char* UINeartuneButtons(VocodecButton button, ButtonAction action);
char* UIAutotuneButtons(VocodecButton button, ButtonAction action);
char* UISamplerBPButtons(VocodecButton button, ButtonAction action);
char* UISamplerAutoButtons(VocodecButton button, ButtonAction action);
char* UIDistortionButtons(VocodecButton button, ButtonAction action);
char* UIWaveFolderButtons(VocodecButton button, ButtonAction action);
char* UIBitcrusherButtons(VocodecButton button, ButtonAction action);
char* UIDelayButtons(VocodecButton button, ButtonAction action);
char* UIReverbButtons(VocodecButton button, ButtonAction action);
char* UIReverb2Buttons(VocodecButton button, ButtonAction action);
char* UILivingStringButtons(VocodecButton button, ButtonAction action);
char* UILivingStringSynthButtons(VocodecButton button, ButtonAction action);
char* UIClassicSynthButtons(VocodecButton button, ButtonAction action);
char* UIRhodesButtons(VocodecButton button, ButtonAction action);



#endif /* UI_H_ */

