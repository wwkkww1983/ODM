//==============================================================================
//
// Title:		Device.h
// Purpose:		A short description of the interface.
//
// Created on:	2019/1/26 at 14:31:49 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __Device_H__
#define __Device_H__

#ifdef __cplusplus
extern "C"
{
#endif

	//==============================================================================
	// Include files

#include "cvidef.h"

	//==============================================================================
	// Constants

	//==============================================================================
	// Types

	//==============================================================================
	// External variables

	//==============================================================================
	// Global functions
	int GetInstrCom(int *comPort);
	int ReadTemp(int comPort, char *temp_data);
	int ReadIL(int comPort, char *IL_data);
	int ReadOS1(int comPort, int *OSstatus);
	int ReadOS2(int comPort, int *OSstatus);
	int SetOS1(int comPort, int OSstatus);
	int SetOS2(int comPort, int OSstatus);
	int ReadLD1(int comPort, int *LDstatus);
	int ReadLD2(int comPort, int *LDstatus);
	int SetIL(int comPort, int IL_status);
	int CautionLight(int comPort, int sendByte);
	int Sendbyte(int comPort, int sendByte);
#ifdef __cplusplus
}
#endif

#endif /* ndef __Device_H__ */
