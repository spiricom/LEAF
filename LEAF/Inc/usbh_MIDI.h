/**
 ******************************************************************************
 * @file    usbh_MIDI.h
 * @author  Xavier Halgand
 * @version 
 * @date    
 * @brief   This file contains all the prototypes for the usbh_MIDI.c
 ******************************************************************************
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_MIDI_CORE_H
#define __USBH_MIDI_CORE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
//#include "stm32f7xx_nucleo_144.h"

/*-------------------------------------------------------------------------------*/
// buffer size (should be at least >= MIOS32_USB_MIDI_DESC_DATA_*_SIZE/4)
#define USB_MIDI_RX_BUFFER_SIZE   64 // packages
#define USB_MIDI_TX_BUFFER_SIZE   64 // packages

// size of IN/OUT pipe
#define USB_MIDI_DATA_IN_SIZE           64
#define USB_MIDI_DATA_OUT_SIZE          64

// endpoint assignments (don't change!)
#define USB_MIDI_DATA_OUT_EP 0x02
#define USB_MIDI_DATA_IN_EP  0x81
/** @defgroup USBH_MIDI_CORE_Exported_Defines
 * @{
 */
#define USB_AUDIO_CLASS                                 0x01
#define USB_MIDISTREAMING_SubCLASS                      0x03
#define USB_MIDI_DESC_SIZE                                 9
#define USBH_MIDI_CLASS    &MIDI_Class

/*-------------------------------------------------------------------------------*/

extern USBH_ClassTypeDef  MIDI_Class;

/* -------------------- Exported_Types ------------------------------------------*/

typedef enum {
	NoteOff       = 0x8,
	NoteOn        = 0x9,
	PolyPressure  = 0xa,
	CC            = 0xb,
	ProgramChange = 0xc,
	Aftertouch    = 0xd,
	PitchBend     = 0xe
} midi_event_t;


typedef enum {
	Chn1,
	Chn2,
	Chn3,
	Chn4,
	Chn5,
	Chn6,
	Chn7,
	Chn8,
	Chn9,
	Chn10,
	Chn11,
	Chn12,
	Chn13,
	Chn14,
	Chn15,
	Chn16
} midi_chn_t;

#pragma anon_unions

typedef union {
	struct {
		uint32_t ALL;
	};
	struct {
		uint8_t cin_cable;
		uint8_t evnt0;
		uint8_t evnt1;
		uint8_t evnt2;
	};
	struct {
		uint8_t type:4;
		uint8_t cable:4;
		uint8_t chn:4; // mios32_midi_chn_t
		uint8_t event:4; // mios32_midi_event_t
		uint8_t value1;
		uint8_t value2;
	};

	// C++ doesn't allow to redefine names in anonymous unions
	// as a simple workaround, we rename these redundant names
	struct {
		uint8_t cin:4;
		uint8_t dummy1_cable:4;
		uint8_t dummy1_chn:4; // mios32_midi_chn_t
		uint8_t dummy1_event:4; // mios32_midi_event_t
		uint8_t note:8;
		uint8_t velocity:8;
	};
	struct {
		uint8_t dummy2_cin:4;
		uint8_t dummy2_cable:4;
		uint8_t dummy2_chn:4; // mios32_midi_chn_t
		uint8_t dummy2_event:4; // mios32_midi_event_t
		uint8_t cc_number:8;
		uint8_t value:8;
	};
	struct {
		uint8_t dummy3_cin:4;
		uint8_t dummy3_cable:4;
		uint8_t dummy3_chn:4; // mios32_midi_chn_t
		uint8_t dummy3_event:4; // mios32_midi_event_t
		uint8_t program_change:8;
		uint8_t dummy3:8;
	};
} midi_package_t;

/* States for MIDI State Machine */
typedef enum
{
	MIDI_IDLE= 0,
	MIDI_SEND_DATA,
	MIDI_SEND_DATA_WAIT,
	MIDI_RECEIVE_DATA,
	MIDI_RECEIVE_DATA_WAIT,
}
MIDI_DataStateTypeDef;

typedef enum
{
	MIDI_IDLE_STATE= 0,
	MIDI_TRANSFER_DATA,
	MIDI_ERROR_STATE,
}
MIDI_StateTypeDef;

/* Structure for MIDI process */
typedef struct _MIDI_Process
{
	MIDI_StateTypeDef			state;
	uint8_t			InPipe;
	uint8_t			OutPipe;
	uint8_t			OutEp;
	uint8_t			InEp;
	uint16_t		OutEpSize;
	uint16_t		InEpSize;

	uint8_t			*pTxData;
	uint8_t			*pRxData;
	uint16_t		TxDataLength;
	uint16_t		RxDataLength;
	MIDI_DataStateTypeDef		data_tx_state;
	MIDI_DataStateTypeDef		data_rx_state;
	uint8_t						Rx_Poll;
	//uint8_t			buff[8];
	//MIDI_DataItfTypedef                DataItf;
	//CDC_InterfaceDesc_Typedef         CDC_Desc;
}
MIDI_HandleTypeDef;

/*---------------------------Exported_FunctionsPrototype-------------------------------------*/

USBH_StatusTypeDef  USBH_MIDI_Transmit(USBH_HandleTypeDef *phost,
                                      uint8_t *pbuff,
                                      uint16_t length);

USBH_StatusTypeDef  USBH_MIDI_Receive(USBH_HandleTypeDef *phost,
                                     uint8_t *pbuff,
                                     uint16_t length);


uint32_t            USBH_MIDI_GetLastReceivedDataSize(USBH_HandleTypeDef *phost);

USBH_StatusTypeDef  USBH_MIDI_Stop(USBH_HandleTypeDef *phost);

void USBH_MIDI_TransmitCallback(USBH_HandleTypeDef *phost);

void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost, uint32_t myLength);

/*-------------------------------------------------------------------------------------------*/
#endif /* __USBH_MIDI_CORE_H */


/*****************************END OF FILE*************************************************************/

