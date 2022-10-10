//DESCRIPTION/NOTES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Name:			CWC_CC2650_154Drv.h
//		Description:	CC2650 low level driver for IEEE 802.15.4 mode
// 		Author(s):		Konstantin Mikhaylov, CWC, UOulu
//		Last modified:	2016.06.20
//		Note: 			The commenting style is optimized for automatic documentation generation using DOXYGEN: www.doxygen.org/
//		License:		Refer to Licence.txt file
//						partially based on CC2650 code of Contiki
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef WIRELESS_COM_ORIG_CWC_CC2650_154DRV_H_
#define WIRELESS_COM_ORIG_CWC_CC2650_154DRV_H_

//INCLUDES
#include <xdc/std.h> // Teemu
#include <inc/hw_types.h>
#include <driverlib/rf_data_entry.h>
#include <driverlib/interrupt.h>

//CONSTANTS
#define IEEE_802_15_4_FRAME_OVERHEAD			9//FCS - automatically added
#define CC2650_RX_ENTRY_HEADER_OVERHEAD_BYTES	8//size of header for an entry see Table 23-10 on page 1584 of SWCU117E
//NOTE: the following defs are based on the RX command parameters and needs to be changed if RX command is modified
#define CC2650_RX_ENTRY_ELEMENTLENGTH_BYTES 	1
#define CC2650_RX_ENTRY_PHYHEADER_BYTES 		1
#define CC2650_RX_ENTRY_FCS_BYTES 				2
#define CC2650_RX_ENTRY_RSSI_BYTES 				1
#define CC2650_RX_ENTRY_STATUS_BYTES 			1
#define CC2650_RX_ENTRY_SRCINDEX_BYTES 			1
#define CC2650_RX_ENTRY_TIMESTAMP_BYTES 		4
//NOTE: it is not clear from the documentation how the element length is calculated. it seems, the length of the element length field itself is not included.
#define CC2650_RX_ENTRY_OVERHEAD_BYTES			(CC2650_RX_ENTRY_PHYHEADER_BYTES+CC2650_RX_ENTRY_FCS_BYTES+CC2650_RX_ENTRY_RSSI_BYTES+CC2650_RX_ENTRY_STATUS_BYTES+CC2650_RX_ENTRY_SRCINDEX_BYTES+CC2650_RX_ENTRY_TIMESTAMP_BYTES)

//TYPEDEFS

typedef enum{//states of the state machine
	CWC_CC2650_154_STATE_OFF          = 0,
	CWC_CC2650_154_STATE_IDLE         = 1,
	CWC_CC2650_154_STATE_TX           = 2,
	CWC_CC2650_154_STATE_RX           = 3,
	CWC_CC2650_154_STATE_UNINIT       = 255,
}CWC_CC2650_154_State_t;

typedef enum{//background operations
	CWC_CC2650_154_Background_UNINIT	= 0,
	CWC_CC2650_154_Background_IDLE      = 1,
	CWC_CC2650_154_Background_RX      	= 2,
}CWC_CC2650_154_BackgroundOperation_t;

typedef enum {
   CWC_CC2650_154_Channel11_freq      = 2405,
   CWC_CC2650_154_Channel12_freq      = 2410,
   CWC_CC2650_154_Channel13_freq      = 2415,
   CWC_CC2650_154_Channel14_freq      = 2420,
   CWC_CC2650_154_Channel15_freq      = 2425,
   CWC_CC2650_154_Channel16_freq      = 2430,
   CWC_CC2650_154_Channel17_freq      = 2435,
   CWC_CC2650_154_Channel18_freq      = 2440,
   CWC_CC2650_154_Channel19_freq      = 2445,
   CWC_CC2650_154_Channel20_freq      = 2450,
   CWC_CC2650_154_Channel21_freq      = 2455,
   CWC_CC2650_154_Channel22_freq      = 2460,
   CWC_CC2650_154_Channel23_freq      = 2465,
   CWC_CC2650_154_Channel24_freq      = 2470,
   CWC_CC2650_154_Channel25_freq      = 2475,
   CWC_CC2650_154_Channel26_freq      = 2480
} CWC_CC2650_ChannelMap_t;


//typedefs to be used by startup_ccs.c
// #define cc26xx_rf_cpe0_isr RFCCPE0IntHandler
// #define cc26xx_rf_cpe1_isr RFCCPE1IntHandler
// Teemu
Void RFCCPE0IntHandler(UArg arg0);
Void RFCCPE1IntHandler(UArg arg0);

typedef enum{//events
	CWC_CC2650_154_EVENT_TXD_OK          = 0x10,
	CWC_CC2650_154_EVENT_RXD_OK			 = 0x20,
	CWC_CC2650_154_EVENT_RXD_NOK		 = 0x21,
}CWC_CC2650_154_Events_t;

typedef void (*CWC_CC2650_154_CallbackfuncPtr_t)(CWC_CC2650_154_Events_t);//callback function (NOTE: called from an interrupt!)

typedef struct{//init structure
   uint8_t Channel;
   uint16_t myAddress;
   uint16_t myPANID;
   CWC_CC2650_154_CallbackfuncPtr_t Event_Callback;
}CWC_CC2650_154_Init_struct_t;

typedef struct __attribute__((__packed__)){//NOTE: not sure if "__packed__" works here as intended
	uint16_t FCS;
	uint8_t Seq;
	uint16_t DstPAN;
	uint16_t DstAddr;
	uint16_t SrcAddr;
}CWC_CC2650_IEEE154_simple_header_struct_t;

typedef struct __attribute__((__packed__)){//NOTE: not sure if "__packed__" works here as intended
	CWC_CC2650_IEEE154_simple_header_struct_t str_Header;
	uint8_t u8_Payload[116];
}CWC_CC2650_IEEE154_simple_packet_struct_t;

typedef struct __attribute__((__packed__)){//NOTE: not sure if "__packed__" works here as intended
	uint8_t *ptr_ElementLength;
	uint8_t *ptr_PHYheader;
	CWC_CC2650_IEEE154_simple_packet_struct_t *ptr_MACdata;
	uint16_t *ptr_FCS;
	uint8_t *ptr_RSSI;
	uint8_t *ptr_Status;
	uint8_t *ptr_SourceIdx;
	uint8_t *ptr_TimeStamp;
}CWC_CC2650_RX_Entry_struct_t;//see Fig 23-6 on page 1626 of SWCU117E

//VARIABLES
extern volatile uint8_t *rx_read_entry;

//MACROS

//PUBLIC FUNCTION PROTOTYPES
uint8_t CWC_CC2650_154_Init(CWC_CC2650_154_Init_struct_t *ptr_Init_Data);//initialize the radio
uint8_t CWC_CC2650_154_SendDataPacket_Forced(uint16_t DestAddr, uint8_t *ptr_Payload, uint8_t u8_length);//sent a radio packet in forced mode (i.e. without CCA)
uint8_t CWC_CC2650_154_ReceiveStart(void);//start receive mode

//Enable radio IRQs. Should work from each possible state.
__STATIC_INLINE void
CWC_CC2650_154_EnableRadioIRQs(void){
    IntPendClear(INT_RFC_CPE_0);
    IntPendClear(INT_RFC_CPE_1);
    IntEnable(INT_RFC_CPE_0);
    IntEnable(INT_RFC_CPE_1);
    IntMasterEnable();
}
//Disable radio IRQs.
__STATIC_INLINE void
CWC_CC2650_154_DisableRadioIRQs(void){
    IntDisable(INT_RFC_CPE_0);
    IntDisable(INT_RFC_CPE_1);
}

#endif //RF_IEEE154_H_


















