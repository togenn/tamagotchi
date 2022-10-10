//DESCRIPTION/NOTES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Name:			comm_lib.c
//		Description:	Functions for communication atop 6LoWPAN
// 		Author(s):		Teemu Leppanen, UBIComp, UOulu & Konstantin Mikhaylov, CWC, UOulu
//		Last modified:	2016.10.17
//		Note: 			The commenting style is optimized for automatic documentation generation using DOXYGEN: www.doxygen.org/
//		License:		Refer to Licence.txt file
//						partially based on CC2650 code of Contiki
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <driverlib/pwr_ctrl.h>
#include <ti/sysbios/hal/Hwi.h>

#include "wireless/comm_lib.h"
#include "wireless/CWC_CC2650_154Drv.h"
#include "wireless/CWC_IntegrTest.h"

#define APP_ADVERTISE_PERIOD	250000

__STATIC_INLINE int16_t CC2650_RXEntry_Decode(uint8_t *ptr_DataStart,CWC_CC2650_RX_Entry_struct_t *ptr_CC2650_RXQueueStruct);
__STATIC_INLINE int16_t CC2650_RXEntry_Release(uint8_t *ptr_Data);

static volatile uint8_t u8_TXd_Flag = false;
static volatile uint8_t u8_RXd_Flag = false;
static volatile uint8_t u8_RX_Error_Flag = false;
int8_t rssi = 0;

Hwi_Params cpe0Params;
Hwi_Handle cpe0Handle;
Hwi_Params cpe1Params;
Hwi_Handle cpe1Handle;

char debug_str[20];

uint8_t GetTXFlag(void) {
	return u8_TXd_Flag;
}

uint8_t GetRXFlag(void) {
	return u8_RXd_Flag;
}

uint16_t GetAddr6LoWPAN(void) {

	return IEEE80154_MY_ADDR;
}

int8_t GetRSSI(void) {
    return rssi;
}

void Init6LoWPAN(void) {

    if (IEEE80154_MY_ADDR == 0x8000) {
        System_abort("Error: Device network address not set!\n");
    }

	 // Enable power domains
	PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
	while (PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON) { //NOTE: potential infinite loop
	}

	//populate radio's init structure
	CWC_CC2650_154_Init_struct_t str_Radio_Init;
	str_Radio_Init.Channel=IEEE80154_CHANNEL;
	str_Radio_Init.Event_Callback=&Radio_IRQ;
	str_Radio_Init.myPANID=IEEE80154_PANID;
	str_Radio_Init.myAddress=IEEE80154_MY_ADDR;

	//init the radio
	int32_t result=CWC_CC2650_154_Init(&str_Radio_Init);
	if(result!=1) {
		System_abort("Error in Radio\n");
	}

    // INT_RFC_CPE_0
    Hwi_Params_init(&cpe0Params);
    cpe0Handle = Hwi_create(INT_RFC_CPE_0, RFCCPE0IntHandler, &cpe0Params, NULL);
    if (cpe0Handle == NULL) {
    	System_abort("RFCCPE0 create failed!");
    }

    // INT_RFC_CPE_1
    Hwi_Params_init(&cpe1Params);
    cpe1Handle = Hwi_create(INT_RFC_CPE_1, RFCCPE1IntHandler, &cpe1Params, NULL);
    if (cpe1Handle == NULL) {
    	System_abort("RFCCPE1 create failed!");
    }
}

int8_t StartReceive6LoWPAN(void) {

	return CWC_CC2650_154_ReceiveStart();
}

void Send6LoWPAN(uint16_t DestAddr, uint8_t *ptr_Payload, uint8_t u8_length) {

	volatile uint32_t u32_cnt = 0;

	uint8_t result = CWC_CC2650_154_SendDataPacket_Forced(DestAddr, ptr_Payload, u8_length);

	for(u32_cnt = 0; u32_cnt < APP_ADVERTISE_PERIOD; u32_cnt++){//SW timer
		if(u8_TXd_Flag){//in case if defined - leave the loop once radio informs about TX end
			u8_TXd_Flag=0;
			break;
		}
	}

	/*
	sprintf(debug_str,"Send msg to 0x%4X: %s\n", DestAddr, ptr_Payload );
	System_printf(debug_str);
	System_flush();
	*/
}

int8_t Receive6LoWPAN(uint16_t *senderAddr, char *payload, uint8_t maxLen) {

	rfc_dataEntryGeneral_t *entry;
	int16_t i16_MACPDU_length;
	CWC_CC2650_RX_Entry_struct_t CC2650_RXQueueStruct;

	u8_RXd_Flag=0;//think twice before moving this line!

	if(rx_read_entry==NULL) {
		System_abort("Error in Radio");
	}

	//process RX entry from radio
	entry = (rfc_dataEntryGeneral_t *)rx_read_entry;
	if(entry->status!=DATA_ENTRY_FINISHED) {
		System_abort("Error in Radio");
	}

	//decode the data
	int32_t result = i16_MACPDU_length=CC2650_RXEntry_Decode(rx_read_entry+CC2650_RX_ENTRY_HEADER_OVERHEAD_BYTES,&CC2650_RXQueueStruct);
	if(result==0) {
		System_abort("Error in Radio\n");
	}

	// sender address??
	*senderAddr = CC2650_RXQueueStruct.ptr_MACdata->str_Header.SrcAddr;

	// RRSI
	rssi = CC2650_RXQueueStruct.ptr_RSSI;

	// no overflow
	if(i16_MACPDU_length >= maxLen) {
		return -1;
	}

	// copy to buffer
	result = memcpy(payload, CC2650_RXQueueStruct.ptr_MACdata->u8_Payload, i16_MACPDU_length);

	//release the entry
	CC2650_RXEntry_Release(rx_read_entry);
	rx_read_entry = entry->pNextEntry;

	//entry=(rfc_dataEntryGeneral_t*)rx_read_entry;
	//entry=entry->pNextEntry;

	return i16_MACPDU_length;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FunctionName:		Radio_IRQ
///Description:		Radio IRQ callback function
//Version & Data:	0.01 2016.06.14
//Author(s):		Konstantin Mikhaylov
//Inputs: 			CWC_CC2650_154_Events_t Event - shows the event which has caused call of this funciton
//Outputs:			none
//Dependences:
//Notes:			this function is called by the radio driver from an IRQ
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Radio_IRQ(CWC_CC2650_154_Events_t Event) {

	rfc_dataEntryGeneral_t *tmp;

	switch(Event){
		case CWC_CC2650_154_EVENT_TXD_OK:
			u8_TXd_Flag=1;
			break;
		case CWC_CC2650_154_EVENT_RXD_OK:
			{
				rfc_dataEntryGeneral_t *entry,*nxt_entry;
				entry = (rfc_dataEntryGeneral_t *)rx_read_entry;
				nxt_entry=(rfc_dataEntryGeneral_t *)entry->pNextEntry;
				while(nxt_entry->status>1){
					tmp=entry;
					entry = entry->pNextEntry;
					nxt_entry = entry->pNextEntry;
					CC2650_RXEntry_Release(tmp);
				}
				rx_read_entry=entry;
				u8_RXd_Flag=1;
			}
			break;
		case CWC_CC2650_154_EVENT_RXD_NOK:
			{
				rfc_dataEntryGeneral_t *entry,*nxt_entry;
				entry = (rfc_dataEntryGeneral_t *)rx_read_entry;
				nxt_entry=(rfc_dataEntryGeneral_t *)entry->pNextEntry;
				while(nxt_entry->status>1){
					tmp=entry;
					entry = entry->pNextEntry;
					nxt_entry = entry->pNextEntry;
					CC2650_RXEntry_Release(tmp);
				}
				rx_read_entry=entry;
				u8_RXd_Flag=1;
			}
			break;
		default:
			break;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FunctionName:		CC2650_RXEntry_Decode
///Description:		converts the RX entry to a more readable form
//Version & Data:	0.01 2016.06.14
//Author(s):		Konstantin Mikhaylov
//Inputs: 			uint8_t *ptr_DataStart - pointer to the RX entry start
//					CWC_CC2650_RX_Entry_struct_t *ptr_CC2650_RXQueueStruct - pointer to the structure to be filled in
//Outputs:			int16_t - length of the MAC payload, 0 - error
//Dependences:		none
//Notes:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__STATIC_INLINE int16_t
CC2650_RXEntry_Decode(uint8_t *ptr_DataStart,CWC_CC2650_RX_Entry_struct_t *ptr_CC2650_RXQueueStruct){
	uint8_t *ptr_u8;
	uint8_t u8_length;
	if((ptr_CC2650_RXQueueStruct==NULL)||(ptr_DataStart==NULL)) return 0;
	ptr_u8=ptr_DataStart;
	u8_length=*ptr_u8;
	ptr_CC2650_RXQueueStruct->ptr_ElementLength=ptr_u8;
	ptr_u8+=CC2650_RX_ENTRY_ELEMENTLENGTH_BYTES;
	ptr_CC2650_RXQueueStruct->ptr_PHYheader=ptr_u8;
	ptr_u8+=CC2650_RX_ENTRY_PHYHEADER_BYTES;
	ptr_CC2650_RXQueueStruct->ptr_MACdata=ptr_u8;
	ptr_u8+=u8_length-CC2650_RX_ENTRY_OVERHEAD_BYTES;
	ptr_CC2650_RXQueueStruct->ptr_FCS=ptr_u8;
	ptr_u8+=CC2650_RX_ENTRY_FCS_BYTES;
	ptr_CC2650_RXQueueStruct->ptr_RSSI=ptr_u8;
	ptr_u8+=CC2650_RX_ENTRY_RSSI_BYTES;
	ptr_CC2650_RXQueueStruct->ptr_Status=ptr_u8;
	ptr_u8+=CC2650_RX_ENTRY_STATUS_BYTES;
	ptr_CC2650_RXQueueStruct->ptr_SourceIdx=ptr_u8;
	ptr_u8+=CC2650_RX_ENTRY_SRCINDEX_BYTES;
	ptr_CC2650_RXQueueStruct->ptr_TimeStamp=ptr_u8;
	ptr_u8+=CC2650_RX_ENTRY_TIMESTAMP_BYTES;
	return u8_length-CC2650_RX_ENTRY_OVERHEAD_BYTES-IEEE_802_15_4_FRAME_OVERHEAD;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FunctionName:		CC2650_RXEntry_Release
///Description:		releases an RX data entry
//Version & Data:	0.01 2016.06.14
//Author(s):		Konstantin Mikhaylov
//Inputs: 			uint8_t *ptr_Data - pointer to the memory where structure to be released is located
//Outputs:			0 - error
//Dependences:
//Notes:			More or less mimics the respective function of Contiki
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__STATIC_INLINE int16_t
CC2650_RXEntry_Release(uint8_t *ptr_Data){
	rfc_dataEntryGeneral_t *ptr_CC2650_DataEntry;
	if(ptr_Data==NULL)return 0;//wrong input formatting
	ptr_CC2650_DataEntry=ptr_Data;
	memset(ptr_Data+CC2650_RX_ENTRY_HEADER_OVERHEAD_BYTES,0x00,CC2650_RX_ENTRY_ELEMENTLENGTH_BYTES);//clear length
	ptr_CC2650_DataEntry->status=DATA_ENTRY_PENDING;//update status
	return 1;
}

