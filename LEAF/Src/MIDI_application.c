/*
 * MIDI_application.c
 *
 *  Created on: 6 d�c. 2014
 *      Author: Xavier Halgand
 *
 *	Modified on: 9/12/16 by C.P. to handle the MIDI_IDLE state properly, and
 *	added required code to be compatible with "NucleoSynth"
 *
 *	11/8/17 by C.P.: Version 0.7.7 - Use for Casio CTK-6200 Keyboard
 */

/* Includes ------------------------------------------------------------------*/
#include "audiostream.h"
#include "MIDI_application.h"
#include "usbh_core.h"
#include "usbh_MIDI.h"
#include "usb_host.h"
#include "sfx.h"

MIDI_ApplicationTypeDef MIDI_Appli_state = MIDI_APPLICATION_READY;
extern ApplicationTypeDef Appli_state;
extern USBH_HandleTypeDef hUsbHostFS;
uint8_t MIDI_RX_Buffer[RX_BUFF_SIZE]__ATTR_RAM_D2; // MIDI reception buffer

uint8_t key, velocity, ctrl, data, sustainInverted;

uint8_t CCs[128];
/* Private define ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief  Main routine for MIDI application, looped in main.c
 * @param  None
 * @retval none
 */
void MIDI_Application(void)
{
	if(Appli_state == APPLICATION_READY)
	{
		if(MIDI_Appli_state == MIDI_APPLICATION_READY)
		{

			USBH_MIDI_Receive(&hUsbHostFS, MIDI_RX_Buffer, RX_BUFF_SIZE); // just once at the beginning, start the first reception
			MIDI_Appli_state = MIDI_APPLICATION_RUNNING;
		}
	}
	if(Appli_state == APPLICATION_DISCONNECT)
	{
		MIDI_Appli_state = MIDI_APPLICATION_READY;
		USBH_MIDI_Stop(&hUsbHostFS);
	}
}

/*-----------------------------------------------------------------------------*/
void ProcessReceivedMidiDatas(uint32_t myLength)
{
	uint16_t numberOfPackets;
	uint8_t *ptr = MIDI_RX_Buffer;
	midi_package_t pack;

	numberOfPackets = myLength >> 2; //each USB midi package is 4 bytes long

	if (numberOfPackets != 0)
	{
		while(numberOfPackets--)
		{
			pack.cin_cable = *ptr ; ptr++ ;
			pack.evnt0 = *ptr ; ptr++ ;
			pack.evnt1 = *ptr ; ptr++ ;
			pack.evnt2 = *ptr ; ptr++ ;

			// Handle MIDI messages
			switch(pack.evnt0)
			{
				case (0x80): // Note Off
					key = pack.evnt1;
					velocity = pack.evnt2;

					noteOff(key, velocity);

					break;
				case (0x90): // Note On
					key = pack.evnt1;
					velocity = pack.evnt2;

					noteOn(key, velocity);

					break;
				case (0xA0):
					break;
				case (0xB0):
					ctrl = pack.evnt1;
					data = pack.evnt2;
					CCs[ctrl] = data;
					switch(ctrl)
					{
						case (0x01):
							break;
						case (0x02):
							break;
						case (0x03):
							break;
						case (0x04):
							break;
						case (0x0D):
							break;
						case (0x4B):
							break;
						case (0x4C):
							break;
						case (0x5C):
							break;
						case (0x5F):
							break;
						case (0x49):
							break;
						case (0x48):
							break;
						case (0x5B):
							break;
						case (0x5D):
							break;
						case (0x4A):
							break;
						case (0x47):
							break;
						case (0x05):
							break;
						case (0x54):
							break;
						case (0x10):
							break;
						case (0x11):
							break;
						case (0x12):
							break;
						case (0x07):
							break;
						case (0x13):
							break;
						case (0x14):
							break;
						case (64): // sustain
							if (data)
							{
								if (sustainInverted) 	sustainOff();
								else					sustainOn();
							}
							else
							{
								if (sustainInverted) 	sustainOn();
								else					sustainOff();
							}
							break;
					}


          break;
				case (0xC0): // Program Change
					break;
				case (0xD0): // Mono Aftertouch
					break;
				case (0xE0): // Pitch Bend
					pitchBend((pack.evnt1) + (pack.evnt2 << 7));
					break;
				case (0xF0):
					break;
			}
		}
	}
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief  MIDI data receive callback.
 * @param  phost: Host handle
 * @retval None
 */
void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost, uint32_t myLength)
{
	ProcessReceivedMidiDatas(myLength);
	USBH_MIDI_Receive(&hUsbHostFS, MIDI_RX_Buffer, RX_BUFF_SIZE); // start a new reception
}

