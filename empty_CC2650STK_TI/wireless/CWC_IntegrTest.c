//DESCRIPTION/NOTES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Name:			CWC_IntegrTest.c
//		Description:	Testing code for CC2650 board of our design
// 		Author(s):		Konstantin Mikhaylov*, CWC, UOulu
//		Last modified:	2016.06.24
//		Note: 			The commenting style is optimized for automatic documentation generation using DOXYGEN: www.doxygen.org/
//		License:		Refer to Licence.txt file
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//INCLUDES
#include <driverlib/gpio.h>
#include <driverlib/ioc.h>
#include <driverlib/prcm.h>
#include <driverlib/osc.h>
#include <inc/hw_memmap.h>
#include <driverlib/rf_common_cmd.h>
#include <driverlib/rfc.h>

#include "CWC_IntegrTest.h"
//CONSTANTS

//TYPEDEFS

//GLOBAL VARIABLES

//EXTERNAL VARIABLES

//LOCAL VARIABLES

//LOCAL FUNCTION PROTOTYPES

//MACROS

//CODE: PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FunctionName:		Test1
///Description:		Generates pulses on all GPIOs
//Version & Data:	0.01 2016.06.24
//Author(s):		Konstantin Mikhaylov*, CWC, UOulu
//Inputs: 			none
//Outputs:			none
//Dependences:		none
//Notes:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
Test1(void){
	volatile uint32_t u32_cnt = 0;//used as a SW timer

	 // Enable power domains
	PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
	while(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);//NOTE: potential infinite loop
	for(u32_cnt = 0; u32_cnt < 250000; u32_cnt++);

	while(1);


	// Enable peripheral clocks
	PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
	PRCMPeripheralSleepEnable(PRCM_PERIPH_GPIO);
	PRCMPeripheralDeepSleepEnable(PRCM_PERIPH_GPIO);
	PRCMLoadSet();
	while (!PRCMLoadGet());

	{
		IOCPortConfigureSet(IOID_0,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_1,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_2,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_3,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_4,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_5,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_6,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_7,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_8,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_9,IOC_PORT_GPIO,IOC_STD_OUTPUT);

		IOCPortConfigureSet(IOID_10,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_11,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_12,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_13,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_14,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_15,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		//IOCPortConfigureSet(IOID_16,IOC_PORT_GPIO,IOC_STD_OUTPUT);//JTAG
		//IOCPortConfigureSet(IOID_17,IOC_PORT_GPIO,IOC_STD_OUTPUT);//JTAG
		IOCPortConfigureSet(IOID_18,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_19,IOC_PORT_GPIO,IOC_STD_OUTPUT);

		IOCPortConfigureSet(IOID_20,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_21,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_22,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_23,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_24,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_25,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_26,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_27,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_28,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_29,IOC_PORT_GPIO,IOC_STD_OUTPUT);

		IOCPortConfigureSet(IOID_30,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_31,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		//IOCPortConfigureSet(IOID_8,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		/*IOCPortConfigureSet(IOID_5,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_6,IOC_PORT_GPIO,IOC_STD_OUTPUT);
		IOCPortConfigureSet(IOID_9,IOC_PORT_GPIO,IOC_STD_OUTPUT);*/
	}

	//HWREG( GPIO_BASE + GPIO_O_DOE31_0 ) = 0xFFFCFFFF;
	//HWREG( GPIO_BASE + GPIO_O_DOE31_0 ) = 0x000000FF;
	HWREG( GPIO_BASE + GPIO_O_DOE31_0 ) = 0xFFFF00FF;

	while(1){
		for(u32_cnt = 0; u32_cnt < 50000; u32_cnt++);
		HWREG( GPIO_BASE + GPIO_O_DOUTCLR31_0 ) = 0xFFFCFFFF;
		//HWREG( GPIO_BASE + GPIO_O_DOUTCLR31_0 ) = 0x00000100;
		for(u32_cnt = 0; u32_cnt < 50000; u32_cnt++);
		HWREG( GPIO_BASE + GPIO_O_DOUTSET31_0 ) = 0xFFFCFFFF;
		//HWREG( GPIO_BASE + GPIO_O_DOUTSET31_0 ) = 0xFFFCFEFF;
		//HWREG( GPIO_BASE + GPIO_O_DOUTSET31_0 ) = 0x00000100;
	}
}

//CODE: LOCAL FUNCTIONS


















