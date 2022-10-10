//DESCRIPTION/NOTES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Name:			CWC_CC2650_154Drv.c
//		Description:	OS-less low level driver for CC2650 for IEEE 802.15.4 mode
// 		Author(s):		Konstantin Mikhaylov, CWC, UOulu
//		Last modified:	2016.06.20
//		Note: 			The commenting style is optimized for automatic documentation generation using DOXYGEN: www.doxygen.org/
//		License:		Refer to Licence.txt file
//						partially based on CC2650 code of Contiki
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//INCLUDES
//TI libs
#include <driverlib/ioc.h>
#include <driverlib/prcm.h>
#include <driverlib/osc.h>
//#include "hw_memmap.h"
#include <driverlib/sys_ctrl.h>
#include <driverlib/rf_common_cmd.h>
#include <driverlib/rfc.h>
#include <driverlib/rf_mailbox.h>
#include <driverlib/rf_data_entry.h>

//other stuff
#include "ieee_cmd.h"
#include "CWC_CC2650_154Drv.h"

//CONSTANTS
//Clocks to be launched
#define RF_CORE_CLOCKS_MASK (RFC_PWR_PWMCLKEN_RFC_M | RFC_PWR_PWMCLKEN_CPE_M | RFC_PWR_PWMCLKEN_CPERAM_M)
//Two radio interrupts to be processed: TX end and RX ENTRY_DONE
//swcu117d p.1617: RX_ENTRY_DONE + TX_DONE
#define INT_RF_CPE1ISL_MASK 	(RFC_DBELL_RFCPEISL_RX_ENTRY_DONE| RFC_DBELL_RFCPEISL_TX_DONE)
//swcu117d p.1615: RX_ENTRY_DONE + TX_DONE
#define INT_RF_EN_MASK 			(RFC_DBELL_RFCPEISL_RX_ENTRY_DONE| RFC_DBELL_RFCPEIEN_TX_DONE)
//swcu117d p.1613:
#define INT_RF_CPE1IF_MASK  	(RFC_DBELL_RFCPEISL_RX_ENTRY_DONE| RFC_DBELL_RFCPEIFG_TX_DONE)

//TYPEDEFS
typedef struct{//internal status structure
   uint8_t myChannel;
   uint16_t myAddress;
   uint16_t myPANID;
   CWC_CC2650_154_State_t myState;
   CWC_CC2650_154_BackgroundOperation_t myBackgroundState;
   CWC_CC2650_154_CallbackfuncPtr_t Event_Callback;
}CWC_CC2650_154_Status_Struct_t;

//GLOBAL VARIABLES
volatile uint8_t *rx_read_entry;//pointer to the RX entry to be read

//EXTERNAL VARIABLES

//LOCAL VARIABLES
//Radio commands
//common RF
static volatile rfc_CMD_START_RAT_t rfc_CMD_START_RAT;
static volatile rfc_CMD_FS_t rfc_CMD_FS;
static volatile rfc_CMD_GET_FW_INFO_t rfc_CMD_GET_FW_INFO;
static volatile rfc_CMD_RADIO_SETUP_t rfc_CMD_RADIO_SETUP;
static volatile rfc_CMD_PING_t rfc_CMD_PING;
//IEEE 802.15.4 ones
static volatile rfc_CMD_IEEE_TX_t rfc_CMD_IEEE_TX;//send a packet(forced)
static volatile rfc_CMD_IEEE_RX_t rfc_CMD_IEEE_RX;//start radio in RX (background mode)
static volatile rfc_CMD_IEEE_ABORT_BG_t rfc_CMD_IEEE_ABORT_BG;//stop background mode

//internal status structure
static volatile CWC_CC2650_154_Status_Struct_t my_CC2650_Status;

//Some specific configs for IEEE 802.15.4 mode
static uint32_t ieee_overrides[] = {//NOTE: by some reason cannot be const
		  0x00354038, /* Synth: Set RTRIM (POTAILRESTRIM) to 5 */
		  0x4001402D, /* Synth: Correct CKVD latency setting (address) */
		  0x00608402, /* Synth: Correct CKVD latency setting (value) */
		//  0x4001405D, /* Synth: Set ANADIV DIV_BIAS_MODE to PG1 (address) */
		//  0x1801F800, /* Synth: Set ANADIV DIV_BIAS_MODE to PG1 (value) */
		  0x000784A3, /* Synth: Set FREF = 3.43 MHz (24 MHz / 7) */
		  0xA47E0583, /* Synth: Set loop bandwidth after lock to 80 kHz (K2) */
		  0xEAE00603, /* Synth: Set loop bandwidth after lock to 80 kHz (K3, LSB) */
		  0x00010623, /* Synth: Set loop bandwidth after lock to 80 kHz (K3, MSB) */
		  0x002B50DC, /* Adjust AGC DC filter */
		  0x05000243, /* Increase synth programming timeout */
		  0x002082C3, /* Increase synth programming timeout */
		  0xFFFFFFFF, /* End of override list */
};

//Frequency maps
const CWC_CC2650_ChannelMap_t ChannelMap[] = {
		   CWC_CC2650_154_Channel11_freq,
		   CWC_CC2650_154_Channel12_freq,
		   CWC_CC2650_154_Channel13_freq,
		   CWC_CC2650_154_Channel14_freq,
		   CWC_CC2650_154_Channel15_freq,
		   CWC_CC2650_154_Channel16_freq,
		   CWC_CC2650_154_Channel17_freq,
		   CWC_CC2650_154_Channel18_freq,
		   CWC_CC2650_154_Channel19_freq,
		   CWC_CC2650_154_Channel20_freq,
		   CWC_CC2650_154_Channel21_freq,
		   CWC_CC2650_154_Channel22_freq,
		   CWC_CC2650_154_Channel23_freq,
		   CWC_CC2650_154_Channel24_freq,
		   CWC_CC2650_154_Channel25_freq,
		   CWC_CC2650_154_Channel26_freq
};

//Templates of the radio doorbell commands
//Common mode commands
static rfc_CMD_FS_t RF_cmdFs = {
        .commandNo = 0x0803,
        .status = 0x0000,
        .pNextOp = 0,
        .startTime = 0x00000000,
        .startTrigger.triggerType = 0x0,
        .startTrigger.bEnaCmd = 0x0,
        .startTrigger.triggerNo = 0x0,
        .startTrigger.pastTrig = 0x0,
        .condition.rule = 0x1,
        .condition.nSkip = 0x0,
        .frequency = 0x0965,//2405
        .fractFreq = 0x0000,
        .synthConf.bTxMode = 0x0,
        .synthConf.refFreq = 0x0,
        .__dummy0 = 0x00,
        //.midPrecal = 0x00,
        //.ktPrecal = 0x00,
        //.tdcPrecal = 0x0000
};

const rfc_CMD_PING_t RF_cmdPing = {
		.commandNo = CMD_PING
};

const rfc_CMD_START_RAT_t RF_cmdStartRat = {
		.commandNo = CMD_START_RAT
};

const rfc_CMD_GET_FW_INFO_t RF_cmdGetFvInfo ={
		.commandNo = CMD_GET_FW_INFO
};

const rfc_CMD_RADIO_SETUP_t RF_cmdRadioSetup ={
		.commandNo = 0x0802,
		.status = 0x0000,
		.pNextOp = 0,
		.startTime = 0x00000000,
		.startTrigger.triggerType = 0x0,
		.startTrigger.bEnaCmd = 0x0,
		.startTrigger.triggerNo = 0x0,
		.startTrigger.pastTrig = 0x0,
		.condition.rule = 0x1,
		.condition.nSkip = 0x0,
		.mode = 0x01,
		.__dummy0 = 0x00,
		.config.frontEndMode = 0x00,
		.config.biasMode = 0x00,
		.config.bNoFsPowerUp = 0x00,
		.txPower = 0x3161,
		.pRegOverride = &ieee_overrides[0]
};

//IEEE mode commands
const rfc_CMD_IEEE_TX_t IEEE_TX ={
        .commandNo = CMD_IEEE_TX,
        .status = 0x0000,
        .pNextOp = 0,
        .startTime = 0x00000000,
        .startTrigger.triggerType = 0x0,
        .startTrigger.bEnaCmd = 0x0,
        .startTrigger.triggerNo = 0x0,
        .startTrigger.pastTrig = 0x0,
        .condition.rule = 0x1,
        .condition.nSkip = 0x0,
		.txOpt.bIncludePhyHdr=0,
		.txOpt.bIncludeCrc=0,
		.txOpt.payloadLenMsb=0,
        .payloadLen=0,
        .pPayload=0,
        .timeStamp=0
};

//IEEE commands
const rfc_CMD_IEEE_RX_t IEEE_RX ={
		.commandNo = CMD_IEEE_RX,
		.status = 0x0000,
		.pNextOp = 0,
		.startTime = 0x00000000,
		.startTrigger.triggerType = TRIG_NOW,
		.startTrigger.bEnaCmd = 0x0,
		.startTrigger.triggerNo = 0x0,
		.startTrigger.pastTrig = 0x0,
		.condition.rule = COND_NEVER,
		.condition.nSkip = 0x0,
		//channel
		.channel = 0,//Use existing channel
		//basic configs - append everything
		.rxConfig.bAutoFlushCrc = 0x01,
		.rxConfig.bAutoFlushIgn = 0x01,
		.rxConfig.bIncludePhyHdr = 0x01,
		.rxConfig.bIncludeCrc = 0x01,
		.rxConfig.bAppendRssi = 0x01,
		.rxConfig.bAppendCorrCrc = 0x01,
		.rxConfig.bAppendSrcInd = 0x01,
		.rxConfig.bAppendTimestamp = 0x01,
		//receive queue
		.pRxQ=0x00,
		.pOutput=0x00,
		//filtering configuration
		.frameFiltOpt.frameFiltEn=0x01,//Enable frame filtering
		//.frameFiltOpt.frameFiltEn=0x00,//Disable frame filtering
		.frameFiltOpt.frameFiltStop=0x01,//Stop receiving frame once frame filtering has caused the frame to be rejected.
		.frameFiltOpt.autoAckEn=1,//Enable auto ACK.
		.frameFiltOpt.slottedAckEn=0,//Non-slotted ACK
		.frameFiltOpt.autoPendEn=0,//Auto-pend disabled
		.frameFiltOpt.defaultPend=0,//The value of the pending data bit in auto ACK packets that are not subject to auto-pend
		.frameFiltOpt.bPendDataReqOnly=1,//Use auto-pend for data request packets only
		.frameFiltOpt.bPanCoord=0,//Device is not PAN coordinator
		.frameFiltOpt.maxFrameVersion=1,//Reject frames where the frame version field in the FCF is greater than this value
		.frameFiltOpt.fcfReservedMask=0,//Value to be AND-ed with the reserved part of the FCF; frame rejected if result is non-zero
		.frameFiltOpt.modifyFtFilter=0,//No modification
		.frameFiltOpt.bStrictLenFilter=1,//Accept acknowledgement frames of any length >= 5
		//accepted frames
		.frameTypes.bAcceptFt0Beacon=0,//Treatment of frames with frame type 000 (beacon): Reject
		.frameTypes.bAcceptFt1Data=1,//Treatment of frames with frame type 001 (data): Accept
		.frameTypes.bAcceptFt2Ack=0,//Treatment of frames with frame type 010 (ACK): Reject
		.frameTypes.bAcceptFt3MacCmd=0,//Treatment of frames with frame type 011 (MAC command): Reject
		.frameTypes.bAcceptFt4Reserved=0,//Treatment of frames with frame type 100 (reserved): Reject
		.frameTypes.bAcceptFt5Reserved=0,//Treatment of frames with frame type 101 (reserved): Reject
		.frameTypes.bAcceptFt6Reserved=0,//Treatment of frames with frame type 110 (reserved): Reject
		.frameTypes.bAcceptFt7Reserved=0,//Treatment of frames with frame type 111 (reserved): Reject
		//CCA
		.ccaOpt.ccaEnEnergy=1,//Enable energy scan as CCA source
		.ccaOpt.ccaEnCorr=0,//Enable correlator based carrier sense as CCA source
		.ccaOpt.ccaEnSync=0,//Enable sync found based carrier sense as CCA source
		.ccaOpt.ccaCorrOp=0,//Report busy channel if either ccaEnergy or ccaCorr are busy
		.ccaOpt.ccaSyncOp=0,//Always report busy channel if ccaSync is busy
		.ccaOpt.ccaCorrThr=1,//Threshold for number of correlation peaks in correlator based carrier sense
		.ccaRssiThr=0xA6,//
		.__dummy0=0,
		//seems the following things are for white list (if 0 - no filtering, see page 1629 of the SWCU117E)
		.numExtEntries=0,//Number of extended address entries
		.numShortEntries=0,//Number of short address entries
		.pExtEntryList=NULL,//Pointer to list of extended address entries
		.localExtAddr=0xC0FEFEEDDEADBEEF,//The extended address of the local device
		.localShortAddr=0xBEEF,//The short address of the local device
		.localPanID=0xDEAD,//The PAN ID of the local device
		//end trigger
		.endTrigger.triggerType = TRIG_NEVER,
		.endTime = 0x00000000,
};

//IEEE strutures
//IEEE packet header const
const CWC_CC2650_IEEE154_simple_header_struct_t IEEE154_header={
		0x9841,//FCS:Data frame, no ACK, no pending, PAN ID compressed, 2 byte DST address, Frame 15.4, 2 byte SRC address
		0x00,//Seq
		0xFFFF,//DstAddr:broadcast
		0xBEEF,//PANID
		0xC0FE,//SrcAddr
};

//IEEE packet structure (NOTE: only one active link is supported at a time)
static CWC_CC2650_IEEE154_simple_packet_struct_t IEEE154_packet={
		{
		0x9841,//FCS:Data frame, no ACK, no pending, PAN ID compressed, 2 byte DST address, Frame 15.4, 2 byte SRC address
		0x00,//Seq
		0xFFFF,//DstAddr:broadcast
		0xBEEF,//PANID
		0xC0FE,//SrcAddr
		}
};

//data buffers & RX queue
static uint8_t rx_buf_0[150] __attribute__ ((aligned (4)));
static uint8_t rx_buf_1[150] __attribute__ ((aligned (4)));
static dataQueue_t rx_data_queue = { 0 };

//LOCAL FUNCTION PROTOTYPES

//MACROS

//CODE: PUBLIC FUNCTIONS

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FunctionName:		CWC_CC2650_154_Init
///Description:		Initializes the radio and puts it to idle mode
//Version & Data:	0.03 2016.06.14
//Author(s):		Konstantin Mikhaylov, CWC, UOulu
//Inputs: 			CWC_CC2650_154_Init_struct_t *ptr_Init_Data - structure with the init data
//Outputs:			1 - all is ok, 0 - fail
//Dependences:		none
//Notes:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t
CWC_CC2650_154_Init(CWC_CC2650_154_Init_struct_t *ptr_Init_Data){//initialize the radio
	volatile int result = 0;

	{//check all the input data
		if(ptr_Init_Data==NULL)return 0;//fail - init struct is missing
		if(ptr_Init_Data->Event_Callback==NULL)return 0;//fail - callback function is missing
		if((ptr_Init_Data->Channel<11)||(ptr_Init_Data->Channel>26))return 0;//fail - impossible (i.e., non-IEEE 802.15.4) radio channel
		if(ptr_Init_Data->myAddress==0xFFFF)return 0;//fail - cannot use broadcast address as end device address
	}

	{//fill in the internal structure(s)
		my_CC2650_Status.myState=CWC_CC2650_154_STATE_UNINIT;//just in case
		my_CC2650_Status.myBackgroundState=CWC_CC2650_154_Background_UNINIT;//just in case
		my_CC2650_Status.Event_Callback=ptr_Init_Data->Event_Callback;
		my_CC2650_Status.myChannel=ptr_Init_Data->Channel;
		my_CC2650_Status.myAddress=ptr_Init_Data->myAddress;
		my_CC2650_Status.myPANID=ptr_Init_Data->myPANID;
		memcpy((uint8_t *)&IEEE154_packet, &IEEE154_header, sizeof(IEEE154_header));//copy the default header
		//TX packet structure
		IEEE154_packet.str_Header.DstPAN=my_CC2650_Status.myPANID;//update my PANID
		IEEE154_packet.str_Header.SrcAddr=my_CC2650_Status.myAddress;//update my address
		//dual RX ring buffer - based on https://github.com/contiki-os/contiki/blob/master/cpu/cc26xx-cc13xx/rf-core/ieee-mode.c
		rfc_dataEntry_t *entry;
		entry = (rfc_dataEntry_t *)rx_buf_0;
		entry->pNextEntry = rx_buf_1;
		entry->config.lenSz = 1;
		entry->length = sizeof(rx_buf_0) - 8;
		entry = (rfc_dataEntry_t *)rx_buf_1;
		entry->pNextEntry = rx_buf_0;
		entry->config.lenSz = 1;
		entry->length = sizeof(rx_buf_1) - 8;
		rx_data_queue.pCurrEntry = rx_buf_0;
		rx_data_queue.pLastEntry = NULL;
		rx_read_entry = rx_buf_0;
	}

	{//HW init sequence
		//based on the following links the radio mode switch needs to be done with RF core powered down
		//https://e2e.ti.com/support/wireless_connectivity/proprietary_sub_1_ghz_simpliciti/f/156/p/471529/1695042
		//https://e2e.ti.com/support/wireless_connectivity/proprietary_sub_1_ghz_simpliciti/f/156/t/499475
		PRCMPowerDomainOff(PRCM_DOMAIN_RFCORE);
		ROM_PRCMPeripheralRunDisable(PRCM_DOMAIN_RFCORE);
		//turn the high speed oscillator on
	    OSCHF_TurnOnXosc();
	    do{}while(!OSCHF_AttemptToSwitchToXosc());  //NOTE:pontial infinite loop
	    //set radio mode for 15.4 mode
	    HWREG(PRCM_BASE + PRCM_O_RFCMODESEL) = PRCM_RFCMODESEL_CURR_MODE5;//taken from Contiki init sequence
		//enable radio power domain
	    PRCMPowerDomainOn(PRCM_DOMAIN_RFCORE);
	    while (PRCMPowerDomainStatus(PRCM_DOMAIN_RFCORE) != PRCM_DOMAIN_POWER_ON);  //NOTE:pontial infinite loop
	    //not sure if the next ones are really needed
	    RFCClockEnable();//enable radio clock
	    RFCAckIntClear();

	    memcpy((rfc_CMD_START_RAT_t *)&rfc_CMD_START_RAT, &RF_cmdStartRat, sizeof(rfc_CMD_START_RAT_t));
	    result= RFCDoorbellSendTo((unsigned long)&rfc_CMD_START_RAT);

	    memcpy((rfc_CMD_PING_t *)&rfc_CMD_PING, &RF_cmdPing, sizeof(rfc_CMD_PING_t));
	    result= RFCDoorbellSendTo((unsigned long)&rfc_CMD_PING);

	    memcpy((rfc_CMD_GET_FW_INFO_t *)&rfc_CMD_GET_FW_INFO, &RF_cmdGetFvInfo, sizeof(rfc_CMD_GET_FW_INFO_t));
	    result= RFCDoorbellSendTo((unsigned long)&rfc_CMD_GET_FW_INFO);
	    if(result!=0x01)return 0;//something goes wrong

	/*    memcpy((rfc_CMD_FS_t *)&rfc_CMD_FS, &RF_cmdFs, sizeof(rfc_CMD_FS_t));
	    //RF_cmdFs.frequency=ChannelMap[my_CC2650_Status.myChannel-11];//update frequency
	    result= RFCDoorbellSendTo((unsigned long)&rfc_CMD_FS);
	    if(result!=0x01)return 0;//something goes wrong*/

	    memcpy((rfc_CMD_RADIO_SETUP_t *)&rfc_CMD_RADIO_SETUP, &RF_cmdRadioSetup, sizeof(rfc_CMD_RADIO_SETUP_t));
	    result= RFCDoorbellSendTo((unsigned long)&rfc_CMD_RADIO_SETUP);
	    if(result!=0x01)return 0;//something goes wrong

	   /* memcpy((rfc_CMD_FS_t *)&rfc_CMD_FS, &RF_cmdFs, sizeof(rfc_CMD_FS_t));//not really needed since the data should be there from the very beginning. just in case if previous code got changed.
	    rfc_CMD_FS.synthConf.bTxMode = 0;//Start synthesizer in RX mode.
	    rfc_CMD_FS.frequency=ChannelMap[my_CC2650_Status.myChannel-11];//update frequency
	    result= RFCDoorbellSendTo((unsigned long)&rfc_CMD_FS);
	    if(result!=0x01)return 0;//something goes wrong
	    while(rfc_CMD_FS.status < 3);//wait for synthesizer to get calibrated   //NOTE:pontial infinite loop*/

	    //prepare the RX command structure
		memcpy((rfc_CMD_IEEE_RX_t *)&rfc_CMD_IEEE_RX, &IEEE_RX, sizeof(rfc_CMD_IEEE_RX_t));
		rfc_CMD_IEEE_RX.channel=my_CC2650_Status.myChannel;
		rfc_CMD_IEEE_RX.pRxQ=&rx_data_queue;
		rfc_CMD_IEEE_RX.localPanID=my_CC2650_Status.myPANID;
		rfc_CMD_IEEE_RX.localShortAddr=my_CC2650_Status.myAddress;
	}

    {//configure and start the IRQs
		//map interrupts to INT_RF_CPE0 or INT_RF_CPE1, 0 - means mapped to INT_RF_CPE0, 1 - INT_RF_CPE1
		HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEISL) = INT_RF_CPE1ISL_MASK;
		//clear all IRQ flags
		HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = 0x0;
		//enable
		HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) = INT_RF_EN_MASK;
		CWC_CC2650_154_EnableRadioIRQs();
    }

    {//update state variables
		my_CC2650_Status.myState=CWC_CC2650_154_STATE_IDLE;
		my_CC2650_Status.myBackgroundState=CWC_CC2650_154_Background_IDLE;
    }
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FunctionName:		CWC_CC2650_154_SendDataPacket_Forced
///Description:		Sends a packet imidiately
//Version & Data:	0.02 2016.06.14
//Author(s):		Konstantin Mikhaylov, CWC, UOulu
//Inputs: 			DestAddr - destantion address, ptr_Payload - payload to data to be sent, u8_length - length of the data to be sent
//Outputs:			1 - all is ok (i.e., sending is in process), 0 - fail
//Dependences:		none
//Notes:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t
CWC_CC2650_154_SendDataPacket_Forced(uint16_t DestAddr, uint8_t *ptr_Payload, uint8_t u8_length){
	volatile int result = 0;
	//check the input data
	if(ptr_Payload==NULL)return 0;//fail - pointer to data missing
	if(u8_length>116)return 0;//invalid length - fragmentation not supported

	//check the status
	switch(my_CC2650_Status.myState){
		case CWC_CC2650_154_STATE_IDLE:
		case CWC_CC2650_154_STATE_RX:
			{
				if(my_CC2650_Status.myBackgroundState==CWC_CC2650_154_Background_IDLE){//seems, we need to start the synthesizer
					memcpy((rfc_CMD_FS_t *)&rfc_CMD_FS, &RF_cmdFs, sizeof(rfc_CMD_FS_t));//not really needed since the data should be there from the very beginning. just in case if previous code got changed.
					rfc_CMD_FS.synthConf.bTxMode = 1;//Start synthesizer in TX mode.
					rfc_CMD_FS.frequency=ChannelMap[my_CC2650_Status.myChannel-11];//update frequency
					result= RFCDoorbellSendTo((unsigned long)&rfc_CMD_FS);
					if(result!=0x01)return 0;//something goes wrong
					while(rfc_CMD_FS.status < 3);//wait for synthesizer to get calibrated //NOTE:pontial infinite loop
				}
				//prepare the IEEE 802.15.4 compatible packet
				IEEE154_packet.str_Header.DstAddr=DestAddr;
				IEEE154_packet.str_Header.Seq++;
				memcpy(&IEEE154_packet.u8_Payload[0], ptr_Payload, u8_length);
				//prepare the TX command
				memcpy((rfc_CMD_IEEE_TX_t *)&rfc_CMD_IEEE_TX, &IEEE_TX, sizeof(rfc_CMD_IEEE_TX_t));
				rfc_CMD_IEEE_TX.startTrigger.triggerType = TRIG_NOW;
				rfc_CMD_IEEE_TX.startTrigger.pastTrig = 0;
				rfc_CMD_IEEE_TX.startTime = 0;
				rfc_CMD_IEEE_TX.pPayload = &IEEE154_packet;
				rfc_CMD_IEEE_TX.payloadLen = u8_length+IEEE_802_15_4_FRAME_OVERHEAD;
				result= RFCDoorbellSendTo((unsigned long)&rfc_CMD_IEEE_TX);
				if(result==1){
					my_CC2650_Status.myState=CWC_CC2650_154_STATE_TX;
					return 1;
				}
				else return 0;
				break;//should never get here anyway
			}
		default:
			return 0;//invalid status for starting TX
			break;//should never get here anyway
	}
	return 0;//should never get here anyway
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FunctionName:		CWC_CC2650_154_ReceiveStart
///Description:		Enables the radio in receive mode
//Version & Data:	0.01 2016.06.14
//Author(s):		Konstantin Mikhaylov, CWC, UOulu
//Inputs: 			none
//Outputs:			1 - all is ok (i.e., receive started), 0 - fail
//Dependences:		none
//Notes:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t
CWC_CC2650_154_ReceiveStart(void){
	volatile int result = 0;
	//check the status
	switch(my_CC2650_Status.myState){
		case CWC_CC2650_154_STATE_IDLE:
		{
			//CWC_CC2650_154_EnableRadioIRQs();//just in case - enable the IRQs
			result=RFCDoorbellSendTo((unsigned long)&rfc_CMD_IEEE_RX);
			if(result==1){
				my_CC2650_Status.myState=CWC_CC2650_154_STATE_RX;
				my_CC2650_Status.myBackgroundState=CWC_CC2650_154_Background_RX;
				return 1;
			}
			else return 0;
		}
		default:
			//cannot handle
			return 0;
	}
}


//INTERRUPTS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FunctionName:		cc26xx_rf_cpe0_isr
///Description:		radio interrupt 0: currently NOT used
//Version & Data:	0.01 2016.06.14
//Author(s):		Konstantin Mikhaylov, CWC, UOulu
//Inputs: 			none
//Outputs:			none
//Dependences:		none
//Notes:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
// cc26xx_rf_cpe0_isr(void){
// Teemu
RFCCPE0IntHandler(UArg arg0){
	IntMasterDisable();//not sure if needed
	HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~(INT_RF_EN_MASK);//see NOTE on page 1476 of swcu117d
	IntMasterEnable();//not sure if needed
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FunctionName:		cc26xx_rf_cpe1_isr
///Description:		radio interrupt 1: currently used for TX_DONE & RX_ENTRY_DONE interupts
//Version & Data:	0.01 2016.06.14
//Author(s):		Konstantin Mikhaylov, CWC, UOulu
//Inputs: 			none
//Outputs:			none
//Dependences:		none
//Notes:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Void
// cc26xx_rf_cpe1_isr(void){
// Teemu
RFCCPE1IntHandler(UArg arg0){
	uint32_t u32_IRQ;
	u32_IRQ = HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG);
	IntMasterDisable();//not sure if needed
	if(u32_IRQ&RFC_DBELL_RFCPEIFG_TX_DONE){
		CWC_CC2650_154_Events_t CurrentEvent=CWC_CC2650_154_EVENT_TXD_OK;
		my_CC2650_Status.Event_Callback(CurrentEvent);//call callback
		if(my_CC2650_Status.myBackgroundState==CWC_CC2650_154_Background_RX)my_CC2650_Status.myState=CWC_CC2650_154_STATE_RX;
		else my_CC2650_Status.myState=CWC_CC2650_154_STATE_IDLE;
		HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~(RFC_DBELL_RFCPEIFG_TX_DONE);//see NOTE on page 1476 of swcu117d
	}
	else if(u32_IRQ&RFC_DBELL_RFCPEIFG_RX_OK){
		CWC_CC2650_154_Events_t CurrentEvent=CWC_CC2650_154_EVENT_RXD_OK;
		my_CC2650_Status.Event_Callback(CurrentEvent);//call callback
		//NOTE: radio continues in RX
		HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~(RFC_DBELL_RFCPEIFG_RX_ENTRY_DONE);//see NOTE on page 1476 of swcu117d
	}
	else{
		CWC_CC2650_154_Events_t CurrentEvent=CWC_CC2650_154_EVENT_RXD_NOK;
		my_CC2650_Status.Event_Callback(CurrentEvent);//call callback
		//NOTE: radio continues in RX
		HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~(RFC_DBELL_RFCPEIFG_RX_ENTRY_DONE);//see NOTE on page 1476 of swcu117d
	}
	IntMasterEnable();//not sure if needed
}
