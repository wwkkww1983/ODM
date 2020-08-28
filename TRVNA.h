//==============================================================================
//
// Title:		TRVNA.h
// Purpose:		A short description of the interface.
//
// Created on:	2018/8/10 at 20:09:13 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __TRVNA_H__
#define __TRVNA_H__

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

    int ConnectVNA(void);
    int GetReadyStateVNA(void);
    int IniVNA(void);
    int CalVNA(int Enable);
    int GetInfoVNA(int *NOP, double *SweepTime);
    int TrigVNA(void);
    int ReadData(int NOP, double *freq, double *amp, double *pha);
    int DisconnectVNA(void);

#ifdef __cplusplus
}
#endif

#endif /* ndef __TRVNA_H__ */
