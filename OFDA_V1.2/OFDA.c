//==============================================================================
//
// Title:		OFDA
// Purpose:		直通式、反射式测长，无测干涉臂功能.
//
// Created on:	2018/5/27 at 22:04:10 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <analysis.h>
#include <cvirte.h>
#include <userint.h>
#include <math.h>
#include <string.h>
#include <visa.h>
#include <fileapi.h>
#include "OFDA.h"
#include "toolbox.h"

//==============================================================================
// Constants
const int StatusON = 0;			 //开始测量状态
const int StatusOFF = 1;		 //终止测量状态
const double c = 299792458;		 //光速(m/s)
const double fstart = 100000000; //开始频率
const double fstop = 1100000000; //终止频率
const double RBW = 10000;		 //分辨率带宽
const double RFpower = -5;
const int NOP = 2001; //初始扫描点数
const char CalON[9] = "Cal ON";
const char CalOFF[9] = "Cal OFF";
const char NormON[9] = "Norm ON";
const char NormOFF[9] = "Norm OFF";
const char Ready[6] = "Ready";
const char NotReady[10] = "Not Ready";
const int np[7] = {501, 1001, 2001, 6001, 10001, 20001, 100001};
//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle = 0;
static int Status = 1;	   //记录测量状态
static int NormStatus = 0; //记录归一化状态
static int CalStatus = 1;  //记录校准状态
static int Version = 1;

//==============================================================================
// Static functions
static int CVICALLBACK ThreadFunction1(void *functionData);
static int CVICALLBACK ThreadFunction2(void *functionData);
//==============================================================================
// Global variables

//==============================================================================
// Global functions
double linfit(double *a, double *b, int num);
int IniVNA(void);
void stopMeasure();
void startMeasure();
/// HIFN The main entry-point function.
int main(int argc, char *argv[])
{

	int error = 0;

	FILE *fp;
	fp = fopen("C:\\ProgramData\\ODM\\version.dll", "rb");
	fscanf(fp, "%d\n", &error);
	fclose(fp);

	//================================================================ 调试时将以下版本切换切换注释掉

	if (error != 1)
	{
		SetSystemAttribute(ATTR_TASKBAR_BUTTON_VISIBLE, 0);
		system(".\\OFDA_V6.exe");
		return 0;
	}

	//================================================================ 调试时将以上版本切换切换注释掉
	/* initialize and load resources */
	nullChk(InitCVIRTE(0, argv, 0));
	errChk(panelHandle = LoadPanel(0, "OFDA.uir", PANEL));

	int retflag = IniVNA();
	if (retflag == 0)
		SetCtrlVal(panelHandle, PANEL_TEXTMSG_INI, Ready);

	/* display the panel and run the user interface */
	errChk(DisplayPanel(panelHandle));
	errChk(RunUserInterface());

	if (Version == 5)
		system(".\\OFDA_V6.exe");

Error:
	/* clean up */
	if (panelHandle > 0)
		DiscardPanel(panelHandle);
	return 0;
}

//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB(int panel, int event, void *callbackData,
						int eventData1, int eventData2)
{

	if (event == EVENT_CLOSE)
	{
		stopMeasure();
		FILE *fp;
		fp = fopen("C:\\ProgramData\\ODM\\version.dll", "wb");
		fprintf(fp, "%d\n", 5);
		fclose(fp);
		QuitUserInterface(0);
	}
	return 0;
}

int CVICALLBACK OneClickCal(int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:

		if (Status == StatusON)
		{
			MessagePopup("Warning", "Please enter Stop before through calibration!");
			return -1;
		}

		ViStatus status;
		ViSession defaultRM, S5048;
		ViUInt32 temp;
		status = viOpenDefaultRM(&defaultRM);
		if (status != VI_SUCCESS)
		{
			MessagePopup("Error", "Can't initialize VISA");
			return -1;
		}
		status = viOpen(defaultRM, "TCPIP::localhost::5025::SOCKET", VI_NULL, VI_NULL, &S5048);
		if (status != VI_SUCCESS)
		{
			MessagePopup("Error", "Can't open VISA address");
			return -1;
		}
		status = viSetAttribute(S5048, VI_ATTR_TMO_VALUE, 60000);	  // Set timeout
		status = viSetAttribute(S5048, VI_ATTR_TERMCHAR_EN, VI_TRUE); // Enable terminal character
		status = viSetAttribute(S5048, VI_ATTR_TERMCHAR, '\n');
		if (CalStatus == StatusON)
		{
			status = viPrintf(S5048, "SENS1:CORR:CLE\n");
			if (status != VI_SUCCESS)
				return -1;
			SetCtrlVal(panelHandle, PANEL_TEXTMSG_CAL, CalOFF);
			CalStatus = StatusOFF;
			status = viClose(S5048);
			if (status != VI_SUCCESS)
			{
				MessagePopup("Error", "Can't close VNA");
				return -1;
			}
			status = viClose(defaultRM);
			if (status != VI_SUCCESS)
			{
				MessagePopup("Error", "Can't close VISA");
				return -1;
			}
			return 0;
		}
		status = viPrintf(S5048, "SENS1:CORR:COLL:THRU 2,1\n");
		if (status != VI_SUCCESS)
			return -1;
		status = viQueryf(S5048, "*OPC?\n", "%d", &temp);
		if (status != VI_SUCCESS)
			return -1;
		status = viPrintf(S5048, "SENS1:CORR:COLL:METH:THRU 2,1\n");
		if (status != VI_SUCCESS)
			return -1;
		status = viPrintf(S5048, "SENS1:CORR:COLL:SAVE\n");
		if (status != VI_SUCCESS)
			return -1;
		status = viClose(S5048);
		if (status != VI_SUCCESS)
		{
			MessagePopup("Error", "Can't close VNA");
			return -1;
		}
		status = viClose(defaultRM);
		if (status != VI_SUCCESS)
		{
			MessagePopup("Error", "Can't close VISA");
			return -1;
		}
		SetCtrlVal(panelHandle, PANEL_TEXTMSG_CAL, CalON);
		CalStatus = StatusON;
		break;
	}
	return 0;
}

int CVICALLBACK OneClickStart(int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		// 限制量程
		int ring_range_value;
		GetCtrlVal(panelHandle, PANEL_RING_Range, &ring_range_value);

		if (ring_range_value >= 7)
		{
			MessagePopup("Error", "The range has been disabled !");
			return 0;
		}
		startMeasure();

		break;
	}
	return 0;
}

int CVICALLBACK OneClickStop(int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:

		stopMeasure();

		break;
	}
	return 0;
}
//================================thread function=========================================//

static int CVICALLBACK ThreadFunction1(void *functionData)
{
	int ring_wavelength_value,
		ring_mode_value,
		ring_range_value,
		ring_parameter_value;

	int yHandle = 0,
		textHandle = 0;

	double xCurs, yCurs;

	double refraction_index = 1.467,
		   through_or_reflect = 1.0;

	char text[9];
	int MaxIndex = 0,
		MinIndex = 0;
	double MaxValue = 0,
		   MinValue = 0;

	GetCtrlVal(panelHandle, PANEL_RING_wavelength, &ring_wavelength_value);
	GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);
	GetCtrlVal(panelHandle, PANEL_RING_Range, &ring_range_value);
	GetCtrlVal(panelHandle, PANEL_RING_parameter, &ring_parameter_value);
	GetCtrlVal(panelHandle, PANEL_NUMERIC_Refraction, &refraction_index);

	ViStatus status; // Error checking
	ViSession defaultRM, S5048;
	ViUInt32 retCount, // Return count from string I/O
		temp = 0;

	status = viOpenDefaultRM(&defaultRM);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't initialize VISA");
		return -1;
	}
	status = viOpen(defaultRM, "TCPIP::localhost::5025::SOCKET", VI_NULL, VI_NULL, &S5048);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't open VISA address");
		return -1;
	}
	status = viSetAttribute(S5048, VI_ATTR_TMO_VALUE, 60000);	  // Set timeout
	status = viSetAttribute(S5048, VI_ATTR_TERMCHAR_EN, VI_TRUE); // Enable terminal character
	status = viSetAttribute(S5048, VI_ATTR_TERMCHAR, '\n');

	int TraceLen = 0;
	switch (ring_range_value)
	{
	case 1:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == np[0])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", np[0]);
			break;
		}
	}
	case 2:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == np[1])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", np[1]);
			break;
		}
	}
	case 3:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == np[2])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", np[2]);
			break;
		}
	}
	case 4:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == np[3])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", np[3]);
			break;
		}
	}
	case 5:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == np[4])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", np[4]);
			break;
		}
	}
	case 6:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == np[5])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", np[5]);
			break;
		}
	}
	case 7:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == np[6])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", np[6]);
			break;
		}
	}
	}

	status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
	if (status != 0)
	{
		MessagePopup("Error", "Get number of points error!");
		status = viClose(S5048);
		status = viClose(defaultRM);
		return 0;
	}
	double StartTime = 0,
		   StopTime = 0,
		   StepTime = 0.01,
		   SweepTime = 0;

	SetCtrlAttribute(panelHandle, PANEL_NUMERICSLIDE, ATTR_MIN_VALUE, 0.0);
	SetCtrlAttribute(panelHandle, PANEL_NUMERICSLIDE, ATTR_MAX_VALUE, 1.0);
	GetCurrentDateTime(&StartTime);
	status = viPrintf(S5048, ":TRIG:SING\n");
	if (status != VI_SUCCESS)
		return -1;
	while (temp != 1)
	{
		status = viQueryf(S5048, "*OPC?\n", "%d", &temp);
		if (status != VI_SUCCESS)
			return -1;
	}
	GetCurrentDateTime(&StopTime);
	SweepTime = StopTime - StartTime;
	SetCtrlAttribute(panelHandle, PANEL_NUMERICSLIDE, ATTR_MAX_VALUE, SweepTime);

	double *freq = (double *)malloc(TraceLen * sizeof(double));
	if (freq == NULL)
		MessagePopup("Error", "malloc fail!");
	double *data = (double *)malloc(TraceLen * 2 * sizeof(double));
	if (data == NULL)
		MessagePopup("Error", "malloc fail!");
	double *amp = (double *)malloc(TraceLen * sizeof(double));
	if (amp == NULL)
		MessagePopup("Error", "malloc fail!");
	double *uph = (double *)malloc(TraceLen * sizeof(double));
	if (uph == NULL)
		MessagePopup("Error", "malloc fail!");
	double *x = (double *)malloc(TraceLen * sizeof(double));
	if (x == NULL)
		MessagePopup("Error", "malloc fail!");
	double *y = (double *)malloc(TraceLen * sizeof(double));
	if (y == NULL)
		MessagePopup("Error", "malloc fail!");
	double *TransformResponseInTimeDomain = (double *)malloc(TraceLen * sizeof(double));
	if (TransformResponseInTimeDomain == NULL)
		MessagePopup("Error", "malloc fail!");

	double slope = 0;
	double TimeDelay = 0;
	double FiberLength = 0,
		   SystemLength = 0;
	double sum = 0,
		   mean_amp = 0;

	while (Status != StatusOFF)
	{
		status = viPrintf(S5048, ":TRIG:SING\n");
		if (status != VI_SUCCESS)
			return -1;
		for (double ii = 0; ii <= SweepTime; ii = ii + StepTime)
		{
			SetCtrlVal(panelHandle, PANEL_NUMERICSLIDE, ii);
			Delay(StepTime);
		}
		while (temp != 1)
		{
			status = viQueryf(S5048, "*OPC?\n", "%d", &temp);
			if (status != VI_SUCCESS)
				return -1;
		}
		temp = 0;
		status = viPrintf(S5048, "CALC:PAR1:SEL\n");
		if (status != VI_SUCCESS)
			return -1;
		retCount = TraceLen;
		status = viQueryf(S5048, "SENS:FREQ:DATA?\n", "%,#lf", &retCount, freq);
		if (status != VI_SUCCESS)
			return -1;
		retCount = TraceLen * 2;
		status = viQueryf(S5048, "CALC:DATA:FDAT?\n", "%,#lf", &retCount, data);
		if (status != VI_SUCCESS)
			return -1;
		for (int i = 0; i < TraceLen; i++)
			amp[i] = data[i * 2];
		status = viPrintf(S5048, "CALC:PAR2:SEL\n");
		if (status != VI_SUCCESS)
			return -1;
		status = viQueryf(S5048, "CALC:DATA:FDAT?\n", "%,#lf", &retCount, data);
		if (status != VI_SUCCESS)
			return -1;
		for (int i = 0; i < TraceLen; i++)
			uph[i] = data[i * 2];

		slope = linfit(freq, uph, TraceLen);
		TimeDelay = slope / (-360) / through_or_reflect;
		if (NormStatus == StatusON)
		{
			SystemLength = TimeDelay * c / refraction_index;
			NormStatus = StatusOFF;
		}
		else if (NormStatus == StatusOFF)
		{
			GetCtrlVal(panelHandle, PANEL_TEXTMSG_NORM, text);
			if (!strcmp(NormOFF, text))
				SystemLength = 0;
		}

		FiberLength = TimeDelay * c / refraction_index - SystemLength;
		Sum1D(amp, TraceLen, &sum);
		mean_amp = sum / TraceLen;
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Loss_M1, mean_amp);
		if (ring_parameter_value == 1)
			SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M1, FiberLength);
		else if (ring_parameter_value == 2)
			SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M1, FiberLength / c * refraction_index * 1000000000);
		for (int i = 0; i < TraceLen; i++)
		{
			x[i] = amp[i] * cos(uph[i]);
			y[i] = amp[i] * sin(uph[i]);
		}
		InvFFT(x, y, TraceLen);
		for (int i = 0; i < TraceLen; i++)
		{
			TransformResponseInTimeDomain[i] = sqrt(x[i] * x[i] + y[i] * y[i]);
		}
		MaxMin1D(TransformResponseInTimeDomain, TraceLen, &MaxValue, &MaxIndex, &MinValue, &MinIndex);

		if ((yHandle != 0) || (textHandle != 0))
		{
			SetCtrlAttribute(panelHandle, PANEL_GRAPH, ATTR_NUM_CURSORS, 0);
			DeleteGraphPlot(panelHandle, PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
		}
		yHandle = PlotY(panelHandle, PANEL_GRAPH, TransformResponseInTimeDomain, TraceLen, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, 0x0000CCFF);
		SetAxisRange(panelHandle, PANEL_GRAPH, VAL_NO_CHANGE, 0.0, 1.0, VAL_MANUAL, MinValue, MaxValue + 0.1 * (MaxValue - MinValue));
		SetCtrlAttribute(panelHandle, PANEL_GRAPH, ATTR_NUM_CURSORS, 1);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CURSOR_MODE, VAL_SNAP_TO_POINT);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CURSOR_COLOR, VAL_RED);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CURSOR_POINT_STYLE, VAL_EMPTY_CIRCLE);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CURSOR_ENABLED, 0);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CROSS_HAIR_STYLE, VAL_NO_CROSS);
		SetGraphCursor(panelHandle, PANEL_GRAPH, 1, MaxIndex, MaxValue);

		GetGraphCursor(panelHandle, PANEL_GRAPH, 1, &xCurs, &yCurs);
		textHandle = PlotText(panelHandle, PANEL_GRAPH, xCurs, yCurs, "Marker 1", VAL_APP_META_FONT, VAL_RED, VAL_TRANSPARENT);
	}
	NormStatus = StatusOFF;
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_NORM, NormOFF);
	SetCtrlAttribute(panelHandle, PANEL_GRAPH, ATTR_NUM_CURSORS, 0);
	DeleteGraphPlot(panelHandle, PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);

	free(freq);
	free(data);
	free(amp);
	free(uph);
	free(x);
	free(y);
	free(TransformResponseInTimeDomain);

	status = viClose(S5048);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't close VNA");
		return -1;
	}
	status = viClose(defaultRM);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't close VISA");
		return -1;
	}

	return 0;
}

static int CVICALLBACK ThreadFunction2(void *functionData)
{
	int ring_wavelength_value,
		ring_mode_value,
		ring_range_value,
		ring_parameter_value;

	int yHandle = 0,
		textHandle = 0;

	double xCurs, yCurs;

	double refraction_index = 1.467,
		   through_or_reflect = 2.0;

	char text[9];
	int MaxIndex = 0,
		MinIndex = 0;
	double MaxValue = 0,
		   MinValue = 0;

	GetCtrlVal(panelHandle, PANEL_RING_wavelength, &ring_wavelength_value);
	GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);
	GetCtrlVal(panelHandle, PANEL_RING_Range, &ring_range_value);
	GetCtrlVal(panelHandle, PANEL_RING_parameter, &ring_parameter_value);
	GetCtrlVal(panelHandle, PANEL_NUMERIC_Refraction, &refraction_index);

	ViStatus status; // Error checking
	ViSession defaultRM, S5048;
	ViUInt32 retCount, // Return count from string I/O
		temp = 0;

	status = viOpenDefaultRM(&defaultRM);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't initialize VISA");
		return -1;
	}
	status = viOpen(defaultRM, "TCPIP::localhost::5025::SOCKET", VI_NULL, VI_NULL, &S5048);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't open VISA address");
		return -1;
	}
	status = viSetAttribute(S5048, VI_ATTR_TMO_VALUE, 60000);	  // Set timeout
	status = viSetAttribute(S5048, VI_ATTR_TERMCHAR_EN, VI_TRUE); // Enable terminal character
	status = viSetAttribute(S5048, VI_ATTR_TERMCHAR, '\n');

	int TraceLen = 0;
	switch (ring_range_value)
	{
	case 1:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == (int)2 * np[0])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", (int)2 * np[0]);
			break;
		}
	}
	case 2:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == (int)2 * np[1])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", (int)2 * np[1]);
			break;
		}
	}
	case 3:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == (int)2 * np[2])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", (int)2 * np[2]);
			break;
		}
	}
	case 4:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == (int)2 * np[3])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", (int)2 * np[3]);
			break;
		}
	}
	case 5:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == (int)2 * np[4])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", (int)2 * np[4]);
			break;
		}
	}
	case 6:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == (int)2 * np[5])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", (int)2 * np[5]);
			break;
		}
	}
	case 7:
	{
		status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
		if (TraceLen == (int)2 * np[6])
			break;
		else
		{
			status = viPrintf(S5048, "SENS:SWE:POIN %d\n", (int)2 * np[6]);
			break;
		}
	}
	default:
		break;
	}

	status = viQueryf(S5048, "SENS:SWE:POIN?\n", "%d", &TraceLen); // 获取当前迹线的点数
	if (status != 0)
	{
		MessagePopup("Error", "Get number of points error!");
		status = viClose(S5048);
		status = viClose(defaultRM);
		return 0;
	}
	double StartTime = 0,
		   StopTime = 0,
		   StepTime = 0.01,
		   SweepTime = 0;

	SetCtrlAttribute(panelHandle, PANEL_NUMERICSLIDE, ATTR_MIN_VALUE, 0.0);
	SetCtrlAttribute(panelHandle, PANEL_NUMERICSLIDE, ATTR_MAX_VALUE, 1.0);
	GetCurrentDateTime(&StartTime);
	status = viPrintf(S5048, ":TRIG:SING\n");
	if (status != VI_SUCCESS)
		return -1;
	while (temp != 1)
	{
		status = viQueryf(S5048, "*OPC?\n", "%d", &temp);
		if (status != VI_SUCCESS)
			return -1;
	}
	GetCurrentDateTime(&StopTime);
	SweepTime = StopTime - StartTime;
	SetCtrlAttribute(panelHandle, PANEL_NUMERICSLIDE, ATTR_MAX_VALUE, SweepTime);

	double *freq = (double *)malloc(TraceLen * sizeof(double));
	if (freq == NULL)
		MessagePopup("Error", "malloc fail!");
	double *data = (double *)malloc(TraceLen * 2 * sizeof(double));
	if (data == NULL)
		MessagePopup("Error", "malloc fail!");
	double *amp = (double *)malloc(TraceLen * sizeof(double));
	if (amp == NULL)
		MessagePopup("Error", "malloc fail!");
	double *uph = (double *)malloc(TraceLen * sizeof(double));
	if (uph == NULL)
		MessagePopup("Error", "malloc fail!");
	double *x = (double *)malloc(TraceLen * sizeof(double));
	if (x == NULL)
		MessagePopup("Error", "malloc fail!");
	double *y = (double *)malloc(TraceLen * sizeof(double));
	if (y == NULL)
		MessagePopup("Error", "malloc fail!");
	double *TransformResponseInTimeDomain = (double *)malloc(TraceLen * sizeof(double));
	if (TransformResponseInTimeDomain == NULL)
		MessagePopup("Error", "malloc fail!");

	double slope = 0;
	double TimeDelay = 0;
	double FiberLength = 0,
		   SystemLength = 0;
	double sum = 0,
		   mean_amp = 0;

	while (Status != StatusOFF)
	{
		status = viPrintf(S5048, ":TRIG:SING\n");
		if (status != VI_SUCCESS)
			return -1;
		for (double ii = 0; ii <= SweepTime; ii = ii + StepTime)
		{
			SetCtrlVal(panelHandle, PANEL_NUMERICSLIDE, ii);
			Delay(StepTime);
		}
		while (temp != 1)
		{
			status = viQueryf(S5048, "*OPC?\n", "%d", &temp);
			if (status != VI_SUCCESS)
				return -1;
		}
		temp = 0;
		status = viPrintf(S5048, "CALC:PAR1:SEL\n");
		if (status != VI_SUCCESS)
			return -1;
		retCount = TraceLen;
		status = viQueryf(S5048, "SENS:FREQ:DATA?\n", "%,#lf", &retCount, freq);
		if (status != VI_SUCCESS)
			return -1;
		retCount = TraceLen * 2;
		status = viQueryf(S5048, "CALC:DATA:FDAT?\n", "%,#lf", &retCount, data);
		if (status != VI_SUCCESS)
			return -1;
		for (int i = 0; i < TraceLen; i++)
			amp[i] = data[i * 2];
		status = viPrintf(S5048, "CALC:PAR2:SEL\n");
		if (status != VI_SUCCESS)
			return -1;
		status = viQueryf(S5048, "CALC:DATA:FDAT?\n", "%,#lf", &retCount, data);
		if (status != VI_SUCCESS)
			return -1;
		for (int i = 0; i < TraceLen; i++)
			uph[i] = data[i * 2];

		slope = linfit(freq, uph, TraceLen);
		TimeDelay = slope / (-360) / through_or_reflect;
		if (NormStatus == StatusON)
		{
			SystemLength = TimeDelay * c / refraction_index;
			NormStatus = StatusOFF;
		}
		else if (NormStatus == StatusOFF)
		{
			GetCtrlVal(panelHandle, PANEL_TEXTMSG_NORM, text);
			if (!strcmp(NormOFF, text))
				SystemLength = 0;
		}

		FiberLength = TimeDelay * c / refraction_index - SystemLength;
		Sum1D(amp, TraceLen, &sum);
		mean_amp = sum / TraceLen;
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Loss_M1, mean_amp);
		if (ring_parameter_value == 1)
			SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M1, FiberLength);
		else if (ring_parameter_value == 2)
			SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M1, FiberLength / c * refraction_index * 1000000000);
		for (int i = 0; i < TraceLen; i++)
		{
			x[i] = amp[i] * cos(uph[i]);
			y[i] = amp[i] * sin(uph[i]);
		}
		InvFFT(x, y, TraceLen);
		for (int i = 0; i < TraceLen; i++)
		{
			TransformResponseInTimeDomain[i] = sqrt(x[i] * x[i] + y[i] * y[i]);
		}
		MaxMin1D(TransformResponseInTimeDomain, TraceLen, &MaxValue, &MaxIndex, &MinValue, &MinIndex);

		if ((yHandle != 0) || (textHandle != 0))
		{
			SetCtrlAttribute(panelHandle, PANEL_GRAPH, ATTR_NUM_CURSORS, 0);
			DeleteGraphPlot(panelHandle, PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
		}
		yHandle = PlotY(panelHandle, PANEL_GRAPH, TransformResponseInTimeDomain, TraceLen, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, 0x0000CCFF);
		SetAxisRange(panelHandle, PANEL_GRAPH, VAL_NO_CHANGE, 0.0, 1.0, VAL_MANUAL, MinValue, MaxValue + 0.1 * (MaxValue - MinValue));
		SetCtrlAttribute(panelHandle, PANEL_GRAPH, ATTR_NUM_CURSORS, 1);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CURSOR_MODE, VAL_SNAP_TO_POINT);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CURSOR_COLOR, VAL_RED);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CURSOR_POINT_STYLE, VAL_EMPTY_CIRCLE);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CURSOR_ENABLED, 0);
		SetCursorAttribute(panelHandle, PANEL_GRAPH, 1, ATTR_CROSS_HAIR_STYLE, VAL_NO_CROSS);
		SetGraphCursor(panelHandle, PANEL_GRAPH, 1, MaxIndex, MaxValue);

		GetGraphCursor(panelHandle, PANEL_GRAPH, 1, &xCurs, &yCurs);
		textHandle = PlotText(panelHandle, PANEL_GRAPH, xCurs, yCurs, "Marker 1", VAL_APP_META_FONT, VAL_RED, VAL_TRANSPARENT);
	}
	NormStatus = StatusOFF;
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_NORM, NormOFF);
	SetCtrlAttribute(panelHandle, PANEL_GRAPH, ATTR_NUM_CURSORS, 0);
	DeleteGraphPlot(panelHandle, PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);

	free(freq);
	free(data);
	free(amp);
	free(uph);
	free(x);
	free(y);
	free(TransformResponseInTimeDomain);

	status = viClose(S5048);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't close VNA");
		return -1;
	}
	status = viClose(defaultRM);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't close VISA");
		return -1;
	}

	return 0;
}

int IniVNA(void)
{
	ViStatus status; // Error checking
	ViSession defaultRM, S5048;
	status = viOpenDefaultRM(&defaultRM);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't initialize VISA");
		return -1;
	}
	status = viOpen(defaultRM, "TCPIP::localhost::5025::SOCKET", VI_NULL, VI_NULL, &S5048);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't open VISA address");
		return -1;
	}
	status = viSetAttribute(S5048, VI_ATTR_TMO_VALUE, 10000); // Set timeout
	if (status != VI_SUCCESS)
		return -1;
	status = viSetAttribute(S5048, VI_ATTR_TERMCHAR_EN, VI_TRUE); // Enable terminal character
	if (status != VI_SUCCESS)
		return -1;
	status = viSetAttribute(S5048, VI_ATTR_TERMCHAR, '\n');
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "SYST:PRES\n");
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, ":TRIG:SOUR BUS\n");
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "SOUR:POW %lf\n", RFpower); //******************************射频功率out power
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "SENS:FREQ:STAR %lf\n", fstart);
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "SENS:FREQ:STOP %lf\n", fstop);
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "SENS:BAND %lf\n", RBW); //IF Bandwidth
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "SENS:SWE:POIN %d\n", NOP);
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "CALC:PAR:COUN 2\n");
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "CALC:PAR1:DEF S21\n");
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "CALC:PAR1:SEL\n");
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "CALC:FORM MLOG\n");
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "CALC:PAR2:DEF S21\n");
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "CALC:PAR2:SEL\n");
	if (status != VI_SUCCESS)
		return -1;
	status = viPrintf(S5048, "CALC:FORM UPH\n");
	if (status != VI_SUCCESS)
		return -1;
	status = viClose(S5048);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't close VNA");
		return -1;
	}
	status = viClose(defaultRM);
	if (status != VI_SUCCESS)
	{
		MessagePopup("Error", "Can't close VISA");
		return -1;
	}
	return 0;
}

int CVICALLBACK OneClickPar(int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		stopMeasure();
		if (Status == StatusON)
		{
			MessagePopup("Warning", "Please enter Stop before choosing measurement parameter!");
			return 0;
		}

		else
		{
			int par = 1;
			GetCtrlVal(panelHandle, PANEL_RING_parameter, &par);
			if (par == 1)
				SetCtrlVal(panelHandle, PANEL_TEXTMSG_Distance, "Distance (m)");
			else if (par == 2)
				SetCtrlVal(panelHandle, PANEL_TEXTMSG_Distance, "Delay (ns)");
		}
		break;
	}
	return 0;
}

int CVICALLBACK OneClickNorm(int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		int ring_mode_value = 0;
		char text[9];
		GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);
		if (ring_mode_value == 3)
			break;
		GetCtrlVal(panelHandle, PANEL_TEXTMSG_NORM, text);
		if (!strcmp(NormON, text))
		{
			SetCtrlVal(panelHandle, PANEL_TEXTMSG_NORM, NormOFF);
			NormStatus = StatusOFF;
		}
		else
		{
			SetCtrlVal(panelHandle, PANEL_TEXTMSG_NORM, NormON);
			NormStatus = StatusON;
		}
		break;
	}
	return 0;
}

int CVICALLBACK OneClickWaveLen(int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:

		if (Status == StatusON)
			MessagePopup("Warning", "Please enter Stop before choosing wavelength!");

		break;
	}
	return 0;
}

int CVICALLBACK OneClickMode(int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:

		if (Status == StatusON)
		{
			MessagePopup("Warning", "Please enter Stop before choosing mode!");
			break;
		}

		break;
	}
	return 0;
}

int CVICALLBACK OneClickRange(int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		int ring_range_value;
		GetCtrlVal(panelHandle, PANEL_RING_Range, &ring_range_value);

		if (ring_range_value >= 7)
		{
			SetCtrlAttribute(panelHandle, PANEL_RING_Range, ATTR_TEXT_COLOR, VAL_GRAY);
		}
		else
		{
			SetCtrlAttribute(panelHandle, PANEL_RING_Range, ATTR_TEXT_COLOR, 0x003399FF);
		}

		if (Status == StatusON)
			MessagePopup("Warning", "Please enter Stop before choosing range!");

		break;
	}
	return 0;
}

int CVICALLBACK OneClickRefrIndex(int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:

		if (Status == StatusON)
			MessagePopup("Warning", "Please enter Stop!");

		break;
	}
	return 0;
}

double linfit(double *a, double *b, int num)
{
	double freq[num], data[num], z[num];
	double slope, intercept, mse;
	for (int i = 0; i < num; i++)
	{
		freq[i] = *(a + i);
		data[i] = *(b + i);
	}
	LinFit(freq, data, num, z, &slope, &intercept, &mse);
	return (slope);
}

void stopMeasure()
{
	double SweepTime = 0, CurrentTime = 0;
	GetCtrlAttribute(panelHandle, PANEL_NUMERICSLIDE, ATTR_MAX_VALUE, &SweepTime);
	GetCtrlVal(panelHandle, PANEL_NUMERICSLIDE, &CurrentTime);
	if ((Status == StatusON) && (CurrentTime < SweepTime))
		Delay(SweepTime - CurrentTime);
	Status = StatusOFF;
}

void startMeasure()
{
	if (Status == StatusON)
	{
		MessagePopup("Warning", "Please enter Stop!");
		return;
	}
	Status = StatusON;
	int ring_mode_value = 1;
	GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);
	switch (ring_mode_value)
	{
	case 1: //Through
		int threadFunctionId1 = 0;
		CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, ThreadFunction1, NULL, &threadFunctionId1);
		CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE, threadFunctionId1, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
		CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE, threadFunctionId1);
		break;
	case 2: //Reflect 1 port
		int threadFunctionId2 = 0;
		CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, ThreadFunction2, NULL, &threadFunctionId2);
		CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE, threadFunctionId2, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
		CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE, threadFunctionId2);
		break;
	case 3: //Reflect 2 port
		break;
	}
}

void CVICALLBACK OpenApp(int menuBar, int menuItem, void *callbackData,
						 int panel)
{
	Version = 5;
	FILE *fp;
	fp = fopen("C:\\ProgramData\\ODM\\APD\\version.dll", "wb");
	fprintf(fp, "%d\n", 5);
	fclose(fp);
	stopMeasure();
	DiscardPanel(panelHandle);
	QuitUserInterface(0);
	SetSystemAttribute(ATTR_TASKBAR_BUTTON_VISIBLE, 0);
}
