//==============================================================================
//
// Title:		DP.h
// Purpose:		A short description of the interface.
//
// Created on:	2018/8/10 at 23:17:24 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __DP_H__
#define __DP_H__

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

    int FiberDetector(double *freq, double *amp, double *phi, double refraction_index, double calmag,
                      double *tau, double *length, double *MeanAmp);
    int TimeDomain(double mean_amp, double length, double range, double ht[101], int *MaxIndex, double *MaxValue, int *MinIndex, double *MinValue);
    int sign(double value);
    double PhaseErr(int nFreq, double mag, double calMag);
#ifdef __cplusplus
}
#endif

#endif /* ndef __DP_H__ */
