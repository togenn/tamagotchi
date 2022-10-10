//DESCRIPTION/NOTES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Name:			comm_lib.c
//		Description:	Functions for communication atop 6LoWPAN
// 		Author(s):		Konstantin Mikhaylov, CWC, UOulu & Teemu Leppanen, UBIComp, UOulu
//		Last modified:	2016.10.17
//		Note: 			The commenting style is optimized for automatic documentation generation using DOXYGEN: www.doxygen.org/
//		License:		Refer to Licence.txt file
//						partially based on CC2650 code of Contiki
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IEEE80154_PANID				0x1337
#define IEEE80154_CHANNEL			0x0C

// JTKJ: Replace here the value 0x8000 with your network address (=the number in your box)
//       E.g. box number is 123 -> set address below as 0x0123
#define IEEE80154_MY_ADDR			0x8000
