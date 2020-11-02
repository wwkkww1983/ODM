//==============================================================================
//
// Title:		DSP.c
// Purpose:		A short description of the implementation.
//
// Created on:	2018/8/10 at 23:17:24 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#include <userint.h>
#include <ansi_c.h>
#include <analysis.h>
#include <math.h>
#include "DSP.h"
//==============================================================================
extern double calData[60][2];
// Constants
extern const double c0;
//==============================================================================
int FiberDetector(double *freq, double *amp, double *phi, double refracnion_index, double calmag,
				  double *tau, double *length, double *MeanAmp)
{
	double theta[7] = {0};
	double y = 0,
		   theta_1GHz = 0;

	for (int i = 0; i < 7; i++)
		theta[i] = phi[i];
	theta[6] = phi[6];

	if (abs(theta[1] - theta[0]) >= 180)
	{
		theta[1] = theta[1] - 360;
		if (abs(theta[1] - theta[0]) >= 180)
			theta[1] = theta[1] + 360;
	}
	for (int i = 2; i < 7; i++)
	{
		y = (theta[i - 1] - theta[0]) / (freq[i - 1] - freq[0]) * (freq[i] - freq[1]) + theta[0];
		while (abs(theta[i] - y) >= 180)
			theta[i] = theta[i] + sign(theta[i - 1] - theta[i - 2]) * 360;
	}
	y = (theta[6] - theta[0]) / (freq[6] - freq[0]) * (freq[6] - freq[1] + freq[0]);
	theta_1GHz = theta[6] + sign(y) * 360 * floor((abs(theta[6] - y) / (double)360));
	while (abs(theta_1GHz - y) >= 180)
		theta_1GHz = theta_1GHz + 360 * sign(y);
	*tau = theta_1GHz / (-360);
	*length = theta_1GHz / (-360) * c0 / refracnion_index;
	*MeanAmp = amp[6];

	return 0;
}

int TimeDomain(double mean_amp, double length, double range, double ht[101], int *MaxIndex, double *MaxValue, int *MinIndex, double *MinValue)
{
	int delay = 0;
	int xc = 0;
	for (int i = 0; i < 101; i++)
		ht[i] = -50;
	delay = abs(floor(length / range * 100));
	xc = (int)abs(length);
	if (delay >= 100 || delay < 0) //delay=100
	{
		delay = 0;
		xc = 0;
	}
	if (mean_amp / 2 < -50)
		ht[delay] = -50;
	else
		ht[delay] = mean_amp / 2;
	*MinValue = -50;
	*MinIndex = 0;
	*MaxValue = ht[delay];
	*MaxIndex = xc;
	return 0;
}

int sign(double value)
{
	if (value > 0)
		return 1;
	else if (value < 0)
		return -1;
	return 0;
}

double PhaseErr(int nFreq, double mag, double calMag)
{
	if (calData[59][0] == 0)
		return 0.0;
	int nr = (int)calData[59][0]; //default reference point
	double calPhase = 0, interVal = 0;
	double calMagData[59] = {0}, calPhaseData[59] = {0}, x[59] = {0}, y[59] = {0}, y2[59] = {0};
	for (int i = 0; i < 59; i++)
	{
		calMagData[i] = calData[i][2 * nFreq];
		calPhaseData[i] = calData[i][2 * nFreq + 1] - calData[nr][1];
	}
	SpInterp(calMagData, calPhaseData, y2, 59, mag + calMag, &calPhase);
	if (calMag == 0)
	{
		SpInterp(calMagData, calPhaseData, y2, 59, mag, &calPhase);
		return calPhase;
	}
	SpInterp(calMagData, calPhaseData, y2, 59, calMag, &calPhase);
	/* dynamic reference point --> normalize */
	for (int i = 0; i < 59; i++)
	{
		x[i] = calData[i][2 * nFreq];
		y[i] = calPhaseData[i] - calPhase;
	}
	SpInterp(x, y, y2, 59, mag + calMag, &interVal);
	return interVal;
}
