#include "codec.h"
#include "main.h"
#include "gpio.h"
#include "audiostream.h"

uint16_t i2cDataSize = 2;
uint8_t myI2cData[2] = {0,0};
uint32_t I2Ctimeout = 2000;

//Settings for CS4271
// translated from the datasheet by JS

// 01h mode control 1
//
// M1 M0 ratio1 ratio0 m/s dac_dif2 dac_dif1 dac_dif0   // unsure if setting bit selects master means 1
// for 48k == 0 1 0 0 0 0 0 1    ( ratio bit chosen as 0 0 based on assumption of 256X master clock)
// == 0x41
// for 96k == 1 0 0 0 0 0 0 1    ( ratio bit chosen as 0 0 based on assumption of <= 256X master clock)
// == 0x81
// for 192k == 1 1 1 0 0 0 0 1   ( ratio bit chosen as 1 0 based on assumption of 256X master clock, choose 0 0 (full word would be 0xc1 instead of 0xe1) instead if using something like 64X)
// == 0xe1

// 02h DAC control
// AMUTE, FILT_SEL, DEM1, DEM0, RMP_UP, RMP_DN, INV_A, INV_B
// 0 0 0 0 1 1 0 0
// == 0x0c if you want a slow ramp up when the part turns on
// == 0x00 if you want quick start up (which may click)

// 03h dac volume and mixing
// (NA) B=A soft zerocross ATAPI3 ATAPI2 ATAPI1 ATAPI0   (B=A makes channel volumes not independent)
// 0 1 1 1 1 0 0 1  (the last four bits set channel L to go to output A and channel R to go to output B, but this is easily changed with mixing or reversal)
// == 0x79
//or without soft ramp for any programmed volume changes // == 0x39

// 04h Volume A
// MUTE vol6 vol5 vol4 vol3 vol2 vol1 vol0
// 0 0 0 0 0 0 0 0
// == 0x00
//or 0x14 for -20db

// 05h Volume B
// MUTE vol6 vol5 vol4 vol3 vol2 vol1 vol0
// 0 0 0 0 0 0 0 0
// == 0x00
//or 0x14 for -20db

// 06h ADC control
// (NA) (NA) Dither16 ADC_DIF MUTEA MUTEB HPFDisableA HPFDisableB
// 0 0 0 1 0 0 0 0
// ==0x10

// 07h mode control 2
// (NA) (NA) (NA) LOOP MUTECA=B FREEZE CPEN PDN
// for power down version == 0 0 0 0 0 0 1 1
// == 0x03
// to send dry ADC input into DAC (it seems this will mute the normal DAC output) == 0 0 0 1 0 0 1 0
// == 0x12
// for normal operation == 0 0 0 0 0 0 1 0
// == 0x02

HAL_StatusTypeDef testVal;


volatile int blankCount = 0;
uint16_t addressCounter = 0;


void AudioCodec_init(I2C_HandleTypeDef* hi2c) {

	//pull codec reset pin high to start codec working
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);

	HAL_Delay(2);
	//enable control port and put part in power-down mode while loading registers
	myI2cData[0] = 0x07;
	myI2cData[1] = 0x03;
	testVal = HAL_I2C_Master_Transmit(hi2c, CODEC_I2C_ADDRESS, myI2cData, i2cDataSize, I2Ctimeout);
	//HAL_Delay(2);

	myI2cData[0] = 0x01;
	myI2cData[1] = 0x41;
	testVal = HAL_I2C_Master_Transmit(hi2c, CODEC_I2C_ADDRESS, myI2cData, i2cDataSize, I2Ctimeout);

	//HAL_Delay(1); // might not be necessary

	myI2cData[0] = 0x02;
	myI2cData[1] = 0x00;
	testVal = HAL_I2C_Master_Transmit(hi2c, CODEC_I2C_ADDRESS, myI2cData, i2cDataSize, I2Ctimeout);

	//HAL_Delay(1); // might not be necessary

	myI2cData[0] = 0x03;
	myI2cData[1] = 0x79;
	testVal = HAL_I2C_Master_Transmit(hi2c, CODEC_I2C_ADDRESS, myI2cData, i2cDataSize, I2Ctimeout);

	//HAL_Delay(1); // might not be necessary

	myI2cData[0] = 0x04;
	myI2cData[1] = 0x01;
	testVal = HAL_I2C_Master_Transmit(hi2c, CODEC_I2C_ADDRESS, myI2cData, i2cDataSize, I2Ctimeout);

	//HAL_Delay(1); // might not be necessary

	myI2cData[0] = 0x05;
	myI2cData[1] = 0x01;
	testVal = HAL_I2C_Master_Transmit(hi2c, CODEC_I2C_ADDRESS, myI2cData, i2cDataSize, I2Ctimeout);

	//HAL_Delay(1); // might not be necessary

	myI2cData[0] = 0x06;
	myI2cData[1] = 0x10;
	testVal = HAL_I2C_Master_Transmit(hi2c, CODEC_I2C_ADDRESS, myI2cData, i2cDataSize, I2Ctimeout);

	//HAL_Delay(1); // might not be necessary

	//turn off power down bit to start things cookin'
	myI2cData[0] = 0x07;
	myI2cData[1] = 0x02;
	HAL_I2C_Master_Transmit(hi2c, CODEC_I2C_ADDRESS, myI2cData, i2cDataSize, I2Ctimeout);


	//HAL_Delay(1); // might not be necessary
	codecReady = 1;

}
