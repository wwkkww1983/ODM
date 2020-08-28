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
    int SetOS1(int comPort, int OSstatus);
    int SetOS2(int comPort, int OSstatus);
    int Sendbyte(int comPort, int sendByte);
#ifdef __cplusplus
}
#endif

#endif /* ndef __Device_H__ */
