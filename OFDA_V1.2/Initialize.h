//==============================================================================
//
// Title:		Initialize.h
// Purpose:		A short description of the interface.
//
// Created on:	2018/3/22 at 15:27:57 by lishupeng.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __Initialize_H__
#define __Initialize_H__

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

    BOOL OpenS2VNA(void);
    BOOL FindProcess(DWORD *p);
    BOOL KillProcess(DWORD process_id);
    BOOL SetupS2VNA(void);
    BOOL InitializeS2VNA(void);

#ifdef __cplusplus
}
#endif

#endif /* ndef __Initialize_H__ */
