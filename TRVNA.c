//==============================================================================
//
// Title:		TRVNA.c
// Purpose:		A short description of the implementation.
//
// Created on:	2018/8/10 at 20:09:13 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================
//==============================================================================
// Include files
#include <visa.h>
#include <ansi_c.h>
#include <userint.h>
#include "TRVNA.h"

//==============================================================================
//constant
extern const int OFF;
//==============================================================================
// Static functions
//ETRM=TR1300;
static ViSession defaultRM, ETRM;
//==============================================================================
int ConnectVNA(void)
{
	ViStatus status = 0;
	status = viOpenDefaultRM(&defaultRM);
	if (status != VI_SUCCESS)
		return status;
	status = viOpen(defaultRM, "TCPIP::localhost::5025::SOCKET", VI_NULL, VI_NULL, &ETRM);
	if (status != VI_SUCCESS)
		return status;
	status = viSetAttribute(ETRM, VI_ATTR_TMO_VALUE, 1000);		 // Set timeout
	status = viSetAttribute(ETRM, VI_ATTR_TERMCHAR_EN, VI_TRUE); // Enable terminal character
	status = viSetAttribute(ETRM, VI_ATTR_TERMCHAR, '\n');

	return status;
}

int GetReadyStateVNA(void)
{
	int readyState = 0;
	viQueryf(ETRM, "SYST:READ?\n", "%d", &readyState);
	return readyState;
}

int DisconnectVNA(void)
{
	ViStatus status = 0;
	status = viClose(ETRM);
	if (status != VI_SUCCESS)
		return status;
	status = viClose(defaultRM);
	if (status != VI_SUCCESS)
		return status;
	return status;
}

int IniVNA(void)
{
	//double	start_freq	= 980000000;			//start frequency
	//double	stop_freq	= 1000000000;			//stop frequency
	double rbw = 100; //IFBW
	double pow = -5;  //output power
	int nop = 7;	  //number of points
	ViStatus status = 0;
	status = viPrintf(ETRM, "SYST:PRES\n");
	status = viPrintf(ETRM, ":TRIG:SOUR BUS\n");
	//status = viPrintf(ETRM, "SENS:FREQ:STAR %lf\n", start_freq);
	//status = viPrintf(ETRM, "SENS:FREQ:STOP %lf\n", stop_freq);
	status = viPrintf(ETRM, "CALC:PAR:COUN 2\n");
	status = viPrintf(ETRM, "CALC:PAR1:DEF S21\n");
	status = viPrintf(ETRM, "CALC:PAR1:SEL\n");
	status = viPrintf(ETRM, "CALC:FORM MLOG\n");
	status = viPrintf(ETRM, "CALC:PAR2:DEF S21\n");
	status = viPrintf(ETRM, "CALC:PAR2:SEL\n");
	status = viPrintf(ETRM, "CALC:FORM PHAS\n");
	status = viPrintf(ETRM, "SOUR:POW %lf\n", pow);			   //out power
	status = viPrintf(ETRM, "SENS:BAND %lf\n", rbw);		   //IF Bandwidth
	status = viPrintf(ETRM, "SENS:SWE:POIN %d\n", nop);		   //number of points
	status = viPrintf(ETRM, "SENS:SWE:POIN:TIME %lf\n", 5e-4); //measure time delay
	status = viPrintf(ETRM, "SENS:SWE:TYPE SEGM\n");
	status = viPrintf(ETRM, "SENS:SEGM:DATA 5,0,0,0,0,0,4,980E6,980.001E6,2,980.01E6,980.1E6,2,981E6,990E6,2,1E9,1E9,1\n");
	return status;
}

int CalVNA(int Enable)
{
	ViStatus status = 0;
	ViUInt32 temp = 0;

	if (Enable == OFF)
		status = viPrintf(ETRM, "SENS1:CORR:CLE\n");
	else
	{
		status = viPrintf(ETRM, "SENS1:CORR:COLL:THRU 2,1\n");
		status = viQueryf(ETRM, "*OPC?\n", "%d", &temp);
		status = viPrintf(ETRM, "SENS1:CORR:COLL:METH:THRU 2,1\n");
		status = viPrintf(ETRM, "SENS1:CORR:COLL:SAVE\n");
	}
	return status;
}

int GetInfoVNA(int *NOP, double *SweepTime)
{
	ViStatus status = 0;
	ViUInt32 temp = 0;
	status = viQueryf(ETRM, "SENS:SWE:POIN?\n", "%d", NOP);
	//-----------------获取扫描时间----------------------
	double StartTime = 0,
		   StopTime = 0;
	GetCurrentDateTime(&StartTime);
	status = viPrintf(ETRM, ":TRIG:SING\n");
	temp = 0;
	while (temp != 1)
		status = viQueryf(ETRM, "*OPC?\n", "%d", &temp);
	GetCurrentDateTime(&StopTime);
	*SweepTime = StopTime - StartTime;

	return status;
}

int TrigVNA(void)
{
	ViStatus status = 0;
	status = viPrintf(ETRM, ":TRIG:SING\n");
	return status;
}

int ReadData(int NOP, double *freq, double *amp, double *pha)
{
	ViStatus status = 0;
	ViUInt32 retCount = 0, temp = 0;
	int TraceLen = 1;
	status = viQueryf(ETRM, "SENS:SWE:POIN?\n", "%d", &TraceLen);
	if (TraceLen != NOP)
		return status;

	double *data = (double *)malloc(TraceLen * 2 * sizeof(double));
	if (data == NULL)
		return -1;

	temp = 0;
	while (temp != 1)
		status = viQueryf(ETRM, "*OPC?\n", "%d", &temp);
	status = viPrintf(ETRM, "CALC:PAR1:SEL\n");
	retCount = TraceLen;
	status = viQueryf(ETRM, "SENS:FREQ:DATA?\n", "%,#lf", &retCount, freq);
	retCount = TraceLen * 2;
	status = viQueryf(ETRM, "CALC:DATA:FDAT?\n", "%,#lf", &retCount, data);
	for (int i = 0; i < TraceLen; i++)
		amp[i] = data[i * 2];
	status = viPrintf(ETRM, "CALC:PAR2:SEL\n");
	status = viQueryf(ETRM, "CALC:DATA:FDAT?\n", "%,#lf", &retCount, data);
	for (int i = 0; i < TraceLen; i++)
		pha[i] = data[i * 2];
	free(data);
	return status;
}
