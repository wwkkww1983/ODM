//==============================================================================
//
// Title:		constant.h
// Purpose:		A short description of the interface.
//
// Created on:	2019/1/26 at 21:23:43 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __constant_H__
#define __constant_H__

#ifdef __cplusplus
extern "C"
{
#endif

    //==============================================================================
    // Include files

#include "cvidef.h"
    //==============================================================================
    // Constants
    const int ON = 1;  //start meaStatus
    const int OFF = 0; //stop meaStatus
    const int THROUGH = 1001;
    const int REFLECT = 1002;
    const int WL1550 = 1550;
    const int WL1310 = 1310;
    const int CHN = 0;
    const int ENG = 1;
    const int contSweepType = 1;
    const int holdSweepType = 2;
    const int singleSweepType = 3;
    const int timingSweepType = 4;
    const int aveFactor = 10; //average factor
    const int MaxPoints = 10000;
    const char TextON[3] = "ON";
    const char TextOFF[4] = "OFF";
    const char CalFile[30] = ".\\APD\\CalFile.csv";
    const double c0 = 0.299792458; //light speed

    //==============================================================================
    // Types

    //==============================================================================
    // External variables

    //==============================================================================
    // Global functions

#ifdef __cplusplus
}
#endif

#endif /* ndef __constant_H__ */
