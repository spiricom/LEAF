/*
 * oled.c
 *
 *  Created on: Feb 05, 2020
 *      Author: Matthew Wang
 */
#include "main.h"
#include "oled.h"
#include "ui.h"
#include "ssd1306.h"
#include "gfx.h"
#include "custom_fonts.h"
#include "audiostream.h"
#include "tunings.h"

GFX theGFX;
char oled_buffer[32];

void OLED_init(I2C_HandleTypeDef* hi2c)
{
	  //start up that OLED display
	  ssd1306_begin(hi2c, SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);

	  HAL_Delay(5);

	  //clear the OLED display buffer
	  for (int i = 0; i < 512; i++)
	  {
		  buffer[i] = 0;
	  }
	  initUIFunctionPointers();
	  initModeNames();
	  //display the blank buffer on the OLED
	  //ssd1306_display_full_buffer();

	  //initialize the graphics library that lets us write things in that display buffer
	  GFXinit(&theGFX, 128, 32);

	  //set up the monospaced font

	  //GFXsetFont(&theGFX, &C649pt7b); //funny c64 text monospaced but very large
	  //GFXsetFont(&theGFX, &DINAlternateBold9pt7b); //very serious and looks good - definitely not monospaced can fit 9 Ms
	  //GFXsetFont(&theGFX, &DINCondensedBold9pt7b); // very condensed and looks good - definitely not monospaced can fit 9 Ms
	  GFXsetFont(&theGFX, &EuphemiaCAS9pt7b); //this one is elegant but definitely not monospaced can fit 9 Ms
	  //GFXsetFont(&theGFX, &GillSans9pt7b); //not monospaced can fit 9 Ms
	  //GFXsetFont(&theGFX, &Futura9pt7b); //not monospaced can fit only 7 Ms
	  //GFXsetFont(&theGFX, &FUTRFW8pt7b); // monospaced, pretty, (my old score font) fits 8 Ms
	  //GFXsetFont(&theGFX, &nk57_monospace_cd_rg9pt7b); //fits 12 characters, a little crammed
	  //GFXsetFont(&theGFX, &nk57_monospace_no_rg9pt7b); // fits 10 characters
	  //GFXsetFont(&theGFX, &nk57_monospace_no_rg7pt7b); // fits 12 characters
	  //GFXsetFont(&theGFX, &nk57_monospace_no_bd7pt7b); //fits 12 characters
	  //GFXsetFont(&theGFX, &nk57_monospace_cd_rg7pt7b); //fits 18 characters

	  GFXsetTextColor(&theGFX, 1, 0);
	  GFXsetTextSize(&theGFX, 1);

	  //ssd1306_display_full_buffer();

	  OLEDclear();
	  OLED_writePreset();
	  OLED_draw();
	//sdd1306_invertDisplay(1);
}

void initUIFunctionPointers(void)
{
	buttonActionFunctions[Vocoder] = UIVocoderButtons;
	buttonActionFunctions[Pitchshift] = UIPitchShiftButtons;
	buttonActionFunctions[AutotuneMono] = UINeartuneButtons;
	buttonActionFunctions[AutotunePoly] = UIAutotuneButtons;
	buttonActionFunctions[SamplerButtonPress] = UISamplerBPButtons;
	buttonActionFunctions[SamplerAutoGrab] = UISamplerAutoButtons;
	buttonActionFunctions[Distortion] = UIDistortionButtons;
	buttonActionFunctions[Wavefolder] = UIWaveFolderButtons;
	buttonActionFunctions[BitCrusher] = UIBitcrusherButtons;
	buttonActionFunctions[Delay] = UIDelayButtons;
	buttonActionFunctions[Reverb] = UIReverbButtons;
	buttonActionFunctions[Reverb2] = UIReverb2Buttons;
	buttonActionFunctions[LivingString] = UILivingStringButtons;
	buttonActionFunctions[LivingStringSynth] = UILivingStringSynthButtons;
	buttonActionFunctions[ClassicSynth] = UIClassicSynthButtons;
	buttonActionFunctions[Rhodes] = UIRhodesButtons;
}

void setLED_Edit(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	}
}


void setLED_USB(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
	}
}


void setLED_1(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	}
}

void setLED_2(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	}
}


void setLED_A(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	}
}

void setLED_B(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
	}
}

void setLED_C(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
	}
}

void setLED_leftout_clip(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	}
}

void setLED_rightout_clip(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	}
}

void setLED_leftin_clip(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	}
}

void setLED_rightin_clip(uint8_t onOff)
{
	if (onOff)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	}
}

int getCursorX()
{
	return GFXgetCursorX(&theGFX);
}

void OLED_process(void)
{
	if (writeKnobFlag >= 0)
	{
		OLED_writeKnobParameter(writeKnobFlag);
		writeKnobFlag = -1;
	}
	if (writeButtonFlag >= 0 && writeActionFlag >= 0) //These should always be set together
	{
		OLED_writeButtonAction(writeButtonFlag, writeActionFlag);
		writeButtonFlag = -1;
		writeActionFlag = -1;
	}
//	OLED_draw();
}

void OLED_writePreset()
{
	GFXsetFont(&theGFX, &EuphemiaCAS9pt7b);
	OLEDclear();
	char tempString[24];
	itoa((currentPreset+1), tempString, 10);
	strcat(tempString, ":");
	strcat(tempString, modeNames[currentPreset]);
	int myLength = strlen(tempString);
	//OLEDwriteInt(currentPreset+1, 2, 0, FirstLine);
	//OLEDwriteString(":", 1, 20, FirstLine);
	//OLEDwriteString(modeNames[currentPreset], 12, 24, FirstLine);
	OLEDwriteString(tempString, myLength, 0, FirstLine);
	GFXsetFont(&theGFX, &EuphemiaCAS7pt7b);
	OLEDwriteString(modeNamesDetails[currentPreset], strlen(modeNamesDetails[currentPreset]), 0, SecondLine);
	//save new preset to flash memory
}

void OLED_writeEditScreen()
{
	GFXsetFont(&theGFX, &EuphemiaCAS7pt7b);
	OLEDclear();
	char* first = "KNOB:SET CV PED";
	if (cvAddParam >= 0) first = "DOWN:CLR CV PED";
	OLEDwriteString(first, strlen(first), 0, FirstLine);
	OLEDwriteString("C:SET KEY CENTER", 16, 0, SecondLine);
}

void OLED_writeKnobParameter(uint8_t whichParam)
{
	// Knob params
	if (whichParam < NUM_ADC_CHANNELS)
	{
		floatADCUI[whichParam] = smoothedADC[whichParam];
		int len = strlen(paramNames[currentPreset][whichParam]);
		if (len > 0)
		{
			GFXsetFont(&theGFX, &EuphemiaCAS7pt7b);
			OLEDclearLine(SecondLine);
			OLEDwriteString(paramNames[currentPreset][whichParam], len, 0, SecondLine);
			OLEDwriteString(" ", 1, getCursorX(), SecondLine);
			OLEDwriteFloat(knobParams[whichParam], getCursorX(), SecondLine);
			//OLEDwriteString(paramNames[currentPreset][whichParam], strlen(paramNames[currentPreset][whichParam]), 0, SecondLine);
		}
	}
}

void OLED_writeButtonAction(uint8_t whichButton, uint8_t whichAction)
{
	// Could change this so that buttonActionFunctions does the actual OLEDwrite
	// if we want more flexibility on what buttons display
	char* str = buttonActionFunctions[currentPreset](whichButton, whichAction);
	int len = strlen(str);
	if (len > 0)
	{
		GFXsetFont(&theGFX, &EuphemiaCAS7pt7b);
		OLEDclearLine(SecondLine);
		OLEDwriteString(str, len, 0, SecondLine);
	}
}

void OLED_writeTuning()
{
	GFXsetFont(&theGFX, &EuphemiaCAS7pt7b);
	OLEDclearLine(SecondLine);
	OLEDwriteString("T ", 2, 0, SecondLine);
	OLEDwriteInt(currentTuning, 2, 12, SecondLine);
	OLEDwriteString(tuningNames[currentTuning], 6, 40, SecondLine);
}

void OLED_draw()
{
	ssd1306_display_full_buffer();
}

/// OLED Stuff

void OLEDdrawPoint(int16_t x, int16_t y, uint16_t color)
{
	GFXwritePixel(&theGFX, x, y, color);
	//ssd1306_display_full_buffer();
}

void OLEDdrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	GFXwriteLine(&theGFX, x0, y0, x1, y1, color);
	//ssd1306_display_full_buffer();
}

void OLEDdrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	GFXfillCircle(&theGFX, x0, y0, r, color);
	//ssd1306_display_full_buffer();
}


void OLEDclear()
{
	GFXfillRect(&theGFX, 0, 0, 128, 32, 0);
	//ssd1306_display_full_buffer();
}

void OLEDclearLine(OLEDLine line)
{
	GFXfillRect(&theGFX, 0, (line%2)*16, 128, 16*((line/2)+1), 0);
	//ssd1306_display_full_buffer();
}

void OLEDwriteString(char* myCharArray, uint8_t arrayLength, uint8_t startCursor, OLEDLine line)
{
	uint8_t cursorX = startCursor;
	uint8_t cursorY = 15 + (16 * (line%2));
	GFXsetCursor(&theGFX, cursorX, cursorY);

	GFXfillRect(&theGFX, startCursor, line*16, arrayLength*12, (line*16)+16, 0);
	for (int i = 0; i < arrayLength; ++i)
	{
		GFXwrite(&theGFX, myCharArray[i]);
	}
	//ssd1306_display_full_buffer();
}

void OLEDwriteLine(char* myCharArray, uint8_t arrayLength, OLEDLine line)
{
	if (line == FirstLine)
	{
		GFXfillRect(&theGFX, 0, 0, 128, 16, 0);
		GFXsetCursor(&theGFX, 4, 15);
	}
	else if (line == SecondLine)
	{
		GFXfillRect(&theGFX, 0, 16, 128, 16, 0);
		GFXsetCursor(&theGFX, 4, 31);
	}
	else if (line == BothLines)
	{
		GFXfillRect(&theGFX, 0, 0, 128, 32, 0);
		GFXsetCursor(&theGFX, 4, 15);
	}
	for (int i = 0; i < arrayLength; ++i)
	{
		GFXwrite(&theGFX, myCharArray[i]);
	}
}

void OLEDwriteInt(uint32_t myNumber, uint8_t numDigits, uint8_t startCursor, OLEDLine line)
{
	int len = OLEDparseInt(oled_buffer, myNumber, numDigits);

	OLEDwriteString(oled_buffer, len, startCursor, line);
}

void OLEDwriteIntLine(uint32_t myNumber, uint8_t numDigits, OLEDLine line)
{
	int len = OLEDparseInt(oled_buffer, myNumber, numDigits);

	OLEDwriteLine(oled_buffer, len, line);
}

void OLEDwritePitch(float midi, uint8_t startCursor, OLEDLine line)
{
	int len = OLEDparsePitch(oled_buffer, midi);

	OLEDwriteString(oled_buffer, len, startCursor, line);
}

void OLEDwritePitchClass(float midi, uint8_t startCursor, OLEDLine line)
{
	int len = OLEDparsePitchClass(oled_buffer, midi);

	OLEDwriteString(oled_buffer, len, startCursor, line);
}

void OLEDwritePitchLine(float midi, OLEDLine line)
{
	int len = OLEDparsePitch(oled_buffer, midi);

	OLEDwriteLine(oled_buffer, len, line);
}

void OLEDwriteFixedFloat(float input, uint8_t numDigits, uint8_t numDecimal, uint8_t startCursor, OLEDLine line)
{
	int len = OLEDparseFixedFloat(oled_buffer, input, numDigits, numDecimal);

	OLEDwriteString(oled_buffer, len, startCursor, line);
}

void OLEDwriteFixedFloatLine(float input, uint8_t numDigits, uint8_t numDecimal, OLEDLine line)
{
	int len = OLEDparseFixedFloat(oled_buffer, input, numDigits, numDecimal);

	OLEDwriteLine(oled_buffer, len, line);
}


void OLEDwriteFloat(float input, uint8_t startCursor, OLEDLine line)
{
	int numDigits = 5;
	int numDecimal = 1;

	if (fastabsf(input)<1.0f)
	{
		numDigits = 3;
		numDecimal = 2;
	}

	else if (fastabsf(input)<10.0f)
	{
		numDigits = 4;
		numDecimal = 2;
	}

	else if (fastabsf(input)<100.0f)
	{
		numDigits = 5;
		numDecimal = 2;
	}

	else if (fastabsf(input)<1000.0f)
	{
		numDigits = 5;
		numDecimal = 1;
	}
	else if (fastabsf(input)<10000.0f)
	{
		numDigits = 5;
		numDecimal = 0;
	}
	else if (fastabsf(input)<100000.0f)
	{
		numDigits = 6;
		numDecimal = 0;
	}
	else if (fastabsf(input)<1000000.0f)
	{
		numDigits = 7;
		numDecimal = 0;
	}
	else if (fastabsf(input)<10000000.0f)
	{
		numDigits = 8;
		numDecimal = 0;
	}

	int len = OLEDparseFixedFloat(oled_buffer, input, numDigits, numDecimal);

	OLEDwriteString(oled_buffer, len, startCursor, line);
}

void OLEDdrawFloatArray(float* input, float min, float max, uint8_t size, uint8_t offset, uint8_t startCursor, OLEDLine line)
{
	uint8_t baseline = 0;
	if (line == SecondLine) baseline = 16;
	uint8_t height = 16;
	if (line == BothLines) height = 32;

	GFXfillRect(&theGFX, startCursor, (line%2)*16, size, 16*((line/2)+1), 0);

	for (int i = 0; i < size; i++)
	{
		int h = ((float)(height) / (max - min)) * (input[i] - min);
		GFXwritePixel(&theGFX, startCursor + size - 1 - ((i + offset) % size), baseline + h, 1);
//		GFXwriteFastVLine(&theGFX, startCursor + size - ((i + offset) % size), center - (h/2), 1, 1);
	}
}
