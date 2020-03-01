/*
 * MIDI_application.h
 *  Created on: 6 d�c. 2014
 *      Author: Xavier Halgand
 *
 *	Modified on: 9/12/16 by C.P. to handle the MIDI_IDLE state properly, and
 *	added required code to be compatible with "NucleoSynth"
 *
 */

#ifndef MIDI_APPLICATION_H_
#define MIDI_APPLICATION_H_

/* Includes ------------------------------------------------------------------*/

#include "stdio.h"
#include "usbh_core.h"
#include "usbh_MIDI.h"
#include "main.h"

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

/*------------------------------------------------------------------------------*/
typedef enum
{
	MIDI_APPLICATION_IDLE = 0,
	MIDI_APPLICATION_START,
	MIDI_APPLICATION_READY,
	MIDI_APPLICATION_RUNNING,
	MIDI_APPLICATION_DISCONNECT
}
MIDI_ApplicationTypeDef;


#define RX_BUFF_SIZE   64  /* Max Received data 64 bytes */

extern uint8_t MIDI_RX_Buffer[RX_BUFF_SIZE];// __ATTR_RAM_D2; // MIDI reception buffer
extern uint8_t CCs[128];

extern uint8_t MIDIStartOfFrame;

/* Exported functions ------------------------------------------------------- */
void MIDI_Application(void);
void LocalMidiHandler(uint8_t param, uint8_t data);
void ProcessReceivedMidiDatas(uint32_t myLength);

/*------------------------------------------------------------------------------*/
#endif /* MIDI_APPLICATION_H_ */
