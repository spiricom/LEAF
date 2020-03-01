#include "stm32h7xx_hal.h"

#define CODEC_I2C_ADDRESS (0x10 << 1) // 7-bit address goes one bit over to the left to make room for R/W bit

void AudioCodec_init(I2C_HandleTypeDef* hi2c);






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
// == 0x0c

// 03h dac volume and mixing
// (NA) B=A soft zerocross ATAPI3 ATAPI2 ATAPI1 ATAPI0   (B=A makes channel volumes not independent)
// 0 1 1 1 1 0 0 1  (the last four bits set channel L to go to output A and channel R to go to output B, but this is easily changed with mixing or reversal)
// == 0x79

// 04h Volume A
// MUTE vol6 vol5 vol4 vol3 vol2 vol1 vol0
// 0 0 0 0 0 0 0 0
// == 0x00

// 05h Volume B
// MUTE vol6 vol5 vol4 vol3 vol2 vol1 vol0
// 0 0 0 0 0 0 0 0
// == 0x00

// 06h ADC control
// (NA) (NA) Dither16 ADC_DIF MUTEA MUTEB HPFDisableA HPFDisableB
// 0 0 0 1 0 0 0 0
// ==0x10

// 07h mode control 2
// (NA) (NA) (NA) LOOP MUTECA=B FREEZE CPEN PDN
// for power down version == 0 0 0 0 0 1 1 1
// == 0x07
// to send dry ADC input into DAC == 0 0 0 1 0 0 1 0
// == 0x12
// for normal operation == 0 0 0 0 0 0 1 0
// == 0x02

/*


#ifndef ADCHPD
  #define ADCHPD 0
#elif (ADCHPD == 0)||(ADCHPD == 1)
#else
  #error ADCHPD value not defined
#endif

#ifndef ADCS
  #define ADCS 2
#elif (ADCS >=0)&&(ADCS <= 2)
#else
  #error ADCS value not defined
#endif

#ifndef HYST
  #define HYST 32
#elif (HYST >= 0)&&(HYST <= 255)
#else
  #error HYST value not defined
#endif

#ifndef LINVOL
  #define LINVOL 0x17
#elif (LINVOL >= 0) && (LINVOL <= 0x1f)
#else
  #error LINVOL value not defined
#endif

#ifndef RINVOL
  #define RINVOL 0x17
#elif (RINVOL >= 0) && (RINVOL <= 0x1f)
#else
  #error RINVOL value not defined
#endif

#ifndef LHPVOL
  #define LHPVOL 127
#elif (LHPVOL == 0) || ((LHPVOL >= 0x30) && (LHPVOL <= 0x7f))
#else
  #error LHPVOL value not defined
#endif

#ifndef RHPVOL
  #define RHPVOL 127
#elif (RHPVOL == 0) || ((RHPVOL >= 0x30) && (RHPVOL <= 0x7f))
#else
  #error RHPVOL value not defined
#endif

#ifndef MICBOOST
  #define MICBOOST 0
#elif (MICBOOST == 0)||(MICBOOST == 1)
#else
  #error MICBOOST value not defined
#endif

	// 1 = muted
#ifndef MUTEMIC
  #define MUTEMIC 1
#elif (MUTEMIC == 0)||(MUTEMIC == 1)
#else
  #error MUTEMIC value not defined
#endif

	// 0 = line inputs, 1 = mic in
#ifndef INSEL
  #define INSEL 0
#elif (INSEL == 0)||(INSEL == 1)
#else
  #error INSEL value not defined
#endif

#ifndef BYPASS
  #define BYPASS 0 //setting this to 1 passes the line input straight to the line output
#elif (BYPASS == 0)||(BYPASS == 1)
#else
  #error BYPASS value not defined
#endif

#ifndef DACSEL
  #define DACSEL 1
#elif (DACSEL == 0)||(DACSEL == 1)
#else
  #error DACSEL value not defined
#endif

#ifndef SIDETONE
  #define SIDETONE 0 // setting this to 1 passes the mic input straight to the line output
#elif (SIDETONE == 0)||(SIDETONE == 1)
#else
  #error SIDETONE value not defined
#endif

#ifndef SIDEATT
  #define SIDEATT 0
#elif (SIDEATT >= 0)&&(SIDEATT <= 3)
#else
  #error SIDEATT value not defined
#endif

*/
