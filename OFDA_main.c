#include "OFDA_V6.h"

//==============================================================================
//
// Title:		OFDA
// Purpose:		Optical fiber transfer delay.
//
// Created on:	2018/5/27 at 22:04:10 by Shupeng Li.
//              2019/7/1  by Guochen Zhao.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

// Include files
#include <windows.h>
#include <formatio.h>
#include <rs232.h>
#include <ansi_c.h>
#include <analysis.h>
#include <cvirte.h>
#include <userint.h>
#include <math.h>
#include <string.h>
#include <visa.h>
#include <time.h>
#include <toolbox.h>
#include "OFDA.h"
#include "TRVNA.h"
#include "DSP.h"
#include "Device.h"
#include "constant.h"
#include "pwctrl.h"
//==============================================================================
// Static global variables
static int panelHandle = 0;
static int calRefractionPanelHandle = 0;
static int timingMeaPanelHandle = 0;
static int devPanelHandle = 0;
static int adminPanelHandle = 0;
static int meaStatus = ON;
static int calStatus = OFF;
static int aveStatus = OFF;
static int devStatus = OFF;
static int Manager = OFF;
static int Caution = OFF;
static int sweepType = 0;
static int subscript = 0;
static double meaDelay = 0.5;
static int meaPoints = 120;
static int comPort = -1;
static double calMag = 0;
static double batchData[MaxPoints][2] = {0};
static double ILData[MaxPoints] = {0};
static double calRefractionIndex = 1;
static char calTemp[5] = {0};
static int year = 0;
static int deltaYear = 0;
static int day1 = 0;
static int day2 = 0;
static int usetime = 0;
static int hour = 0;
static int min = 0;
static int sec = 0;
static int last_day = 0;
static int last_sec = 0;
static int Probation_Days = 0;
static int LDstatus = WL1550;
static int OSstatus = THROUGH;
static int Language = CHN;
static double MaxLength = 100000; //最大量程，改动此处可以改变最大显示数值，但是无法使档位变色，需要同步修改下文的档位颜色设置
static int LimitLengthIndex = 7;  // 量程标记的 index， 100km 是 6；
static double calIL = 0;
static double calIL1550 = 7.8; //1550输出光功率
static double calIL1310 = 7.0; //1310输出光功率
static char Version = 6;
static int PASSWORDCtrl = 0;
static int init = 0;
static int subPanelStatus[4] = {0, 0, 0, 0};
//==============================================================================
double calData[60][2] = {0};
//==============================================================================
// Static functions
static int CVICALLBACK ThreadFunction(void *functionData);
static int CVICALLBACK ThreadFunctionTemp(void *functionData);
static int HelloODM(void);
static void PowerCaution(int comPort, double mean_amp);
static void SaveData(void);
static void DisplayRS232Error(int RS232Error);
static void DispXstopMsg(void);
static void PlotResponse(int sweep_type, double mean_amp, double fiber_length, double range);
static double SetPrecision_D(int, double); // 0.1mm
static double ConvertUnit_M(int, double);  // km,m,mm ---> m
static double ConvertUnit_MS(int, double); //m--->km,m,mm
static void ConvertPrecision_D(int);	   //precision=0.1mm,0.0001m......
static void GetProbTime(void);

//==============================================================================
int main(int argc, char *argv[])
{
	SetSystemAttribute(ATTR_TASKBAR_BUTTON_VISIBLE, 0);

	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	FILE *fp;

	int error = 0;
	int threadFunctionId = 0;
	int permission = 0;

	/* initialize and load resources */
	ssize_t size;
	error = GetFileInfo("C:\\ProgramData\\ODM\\APD\\lic.dll", &size);
	permission = GetFileInfo("C:\\ProgramData\\ODM\\APD\\permission.dll", &size);
	if (error == 0 || (permission == 0 && LimitLengthIndex == 7))
	{
		if (ConfirmPopup("Hello", "Are you the administrator?"))
		{
			if ((adminPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_ADM)) < 0)
				;

			else
			{
				PASSWORDCtrl = PasswordCtrl_ConvertFromString(adminPanelHandle, PANEL_ADM_STRING_ADMIN);
				DisplayPanel(adminPanelHandle);
				subPanelStatus[0] = 1;
				RunUserInterface();
				if (devStatus == OFF)
					return -1;
				//DisplayPanel (devPanelHandle);	   
				//subPanelStatus[1]=1;
			}
		}
		else
			return -1;
	}
	else
	{

		fp = fopen("C:\\ProgramData\\ODM\\APD\\fir.dll", "rb");
		fscanf(fp, "%d\n", &error);
		fclose(fp);
		if (error == 1)
		{
			fp = fopen("C:\\ProgramData\\ODM\\APD\\prob.dll", "wb");
			fprintf(fp, "%d\n", 0);
			fprintf(fp, "%d\n", 1900 + p->tm_year);
			fprintf(fp, "%d\n", p->tm_yday);
			hour = p->tm_hour;
			min = p->tm_min;
			sec = hour * 3600 + min * 60 + p->tm_sec;
			fprintf(fp, "%d\n", sec);
			fclose(fp);
			SetFileAttributes("C:\\ProgramData\\ODM\\APD\\prob.dll", FILE_ATTRIBUTE_HIDDEN);
			fp = fopen("C:\\ProgramData\\ODM\\APD\\fir.dll", "wb");
			fprintf(fp, "%d\n", 0);
			fclose(fp);
			SetFileAttributes("C:\\ProgramData\\ODM\\APD\\fir.dll", FILE_ATTRIBUTE_HIDDEN);
		}
		else
		{

			fp = fopen("C:\\ProgramData\\ODM\\APD\\lic.dll", "rb");
			fscanf(fp, "%d\n", &Probation_Days);
			fclose(fp);
			GetProbTime();
			if (usetime > Probation_Days)
			{
				devPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_DEV);
				PASSWORDCtrl = PasswordCtrl_ConvertFromString(devPanelHandle, PANEL_DEV_STRING_LICENCE);
				DisplayPanel(devPanelHandle);
				subPanelStatus[1] = 1;
				RunUserInterface();
				if (devStatus == OFF)
					return -1;
			}
			if (usetime < last_day || (usetime == last_day && (sec + 60) < last_sec)) //usetime must be increased
			{
				devPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_DEV);
				PASSWORDCtrl = PasswordCtrl_ConvertFromString(devPanelHandle, PANEL_DEV_STRING_LICENCE);
				DisplayPanel(devPanelHandle);
				subPanelStatus[1] = 1;
				MessagePopup("Error", "system time has been modified!");
				RunUserInterface();
				if (devStatus == OFF)
					return -1;
			}
		}
	}

	/* 	error = GetFileInfo(CalFile,&size);
		if (error == 0){
			MessagePopup("Error","校准文件不存在！");
			return -1;
		}
		else{
			FileToArray number of elements means all data
			FileToArray (CalFile, calData, VAL_DOUBLE, 60*2, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_ROWS, VAL_ASCII);
		}
	*/
	nullChk(InitCVIRTE(0, argv, 0));
	errChk(panelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL));
	/* initialize device */

	/* Schedule two thread functions */
	CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, ThreadFunction, NULL, &threadFunctionId);
	/* display the panel and run the user interface */
	errChk(DisplayPanel(panelHandle));
	errChk(RunUserInterface());
	/* Wait for the thread functions to finish executing */
	CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE, threadFunctionId, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
	/* Release thread functions */
	CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE, threadFunctionId);

	if (Version == 1)
	{
		fp = fopen("C:\\ProgramData\\ODM\\APD\\version.dll", "wb");
		fprintf(fp, "%d\n", 1);
		fclose(fp);
		Delay(0.1);

		system(".\\OFDA.exe");

		Delay(0.1);
		fp = fopen("C:\\ProgramData\\ODM\\APD\\version.dll", "wb");
		fprintf(fp, "%d\n", 5);
		fclose(fp);
		SetFileAttributes("C:\\ProgramData\\ODM\\APD\\version.dll", FILE_ATTRIBUTE_HIDDEN);
	}

Error:
	/* clean up */
	if (panelHandle > 0)
		DiscardPanel(panelHandle);
	return -1;
}

//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB(int panel, int event, void *callbackData,
						int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{

		meaStatus = OFF; //测量软件关闭时候的操作
		DiscardPanel(panelHandle);
		Delay(0.1);
		Sendbyte(comPort, 0x33); //恢复至反射，主要作用：灭直通灯
		Delay(0.05);
		Sendbyte(comPort, 0x39); //关闭1310光源
		Delay(0.05);
		Sendbyte(comPort, 0x37); //关闭1550光源
		Delay(0.05);
		Sendbyte(comPort, 0x44); //关闭警告灯
		Delay(0.05);
		SetOS2(comPort, REFLECT);
		//	Delay(0.01);
		CloseCom(comPort);
		CloseCom(comPort);
		DisconnectVNA();
		Delay(0.1);
		FILE *fp;
		fp = fopen("C:\\ProgramData\\ODM\\APD\\version.dll", "wb");
		fprintf(fp, "%d\n", 5);
		fclose(fp);
		SetFileAttributes("C:\\ProgramData\\ODM\\APD\\version.dll", FILE_ATTRIBUTE_HIDDEN);
		QuitUserInterface(0);
		exit(0);
	}
	return 0;
}

int CVICALLBACK panelTIMM(int panel, int event, void *callbackData,
						  int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		DiscardPanel(timingMeaPanelHandle);
		subPanelStatus[3] == 0;
	}
	return 0;
}

int CVICALLBACK panelREFR(int panel, int event, void *callbackData,
						  int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		DiscardPanel(calRefractionPanelHandle);
		subPanelStatus[2] = 0;
	}
	return 0;
}

int CVICALLBACK panelDEV(int panel, int event, void *callbackData,
						 int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		DiscardPanel(devPanelHandle);
		subPanelStatus[1] = 0;
		if (usetime > Probation_Days || usetime < last_day || (usetime == last_day && (sec + 60) < last_sec))
		{
			QuitUserInterface(0);
		}
	}
	return 0;
}

int CVICALLBACK panelADM(int panel, int event, void *callbackData,
						 int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_CLOSE:
		DiscardPanel(adminPanelHandle);
		subPanelStatus[0] = 0;
		if (Probation_Days == 0)
		{
			QuitUserInterface(0);
		}
		break;
	}
	return 0;
}

//-----------------MenuBar Callback Function-----------------
void CVICALLBACK menuSave(int menuBar, int menuItem, void *callbackData,
						  int panel)
{
	SaveData();
}

void CVICALLBACK menuPrint(int menuBar, int menuItem, void *callbackData,
						   int panel)
{
	PrintPanel(panelHandle, "", 1, VAL_FULL_PANEL, 1);
}

/*void CVICALLBACK menuRejectCalFile (int menuBar, int menuItem, void *callbackData,
									int panel)
{
	for(int i=0;i<60;i++){
		calData[i][0]=0;
		calData[i][1]=0;
	}
}*/

void CVICALLBACK menuAverage(int menuBar, int menuItem, void *callbackData,
							 int panel)
{
	if (aveStatus == OFF)
		aveStatus = ON;
	else
		aveStatus = OFF;
}

void CVICALLBACK menuCalRefraction(int menuBar, int menuItem, void *callbackData,
								   int panel)
{
	if (subPanelStatus[2] == 1)
	{
		return;
	}
	if ((calRefractionPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_REFR)) < 0)
		return;
	DisplayPanel(calRefractionPanelHandle);
	if (Language == ENG)
	{
		SetCtrlAttribute(calRefractionPanelHandle, PANEL_REFR_NUMERIC, ATTR_LABEL_TEXT, "Refractive Index Correction");
		SetCtrlAttribute(calRefractionPanelHandle, PANEL_REFR_COMMANDBUTTON, ATTR_LABEL_TEXT, "Yes");
	}
	SetCtrlVal(calRefractionPanelHandle, PANEL_REFR_NUMERIC, calRefractionIndex);
	subPanelStatus[2] = 1;
}

void CVICALLBACK menuTimingSet(int menuBar, int menuItem, void *callbackData,
							   int panel)
{
	if (subPanelStatus[3] == 1)
	{
		return;
	}
	if ((timingMeaPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_TIMM)) < 0)
		return;
	DisplayPanel(timingMeaPanelHandle);
	SetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_NUMERIC, meaDelay);
	SetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_NUMERIC_2, meaPoints);
	if (Language == ENG)
	{
		SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_NUMERIC, ATTR_LABEL_TEXT, "Delay");
		SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_NUMERIC_2, ATTR_LABEL_TEXT, "Points");
		SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_COMMANDBUTTON, ATTR_LABEL_TEXT, "Yes");
		SetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_TEXTMSG, "s");
	}
	subPanelStatus[3] = 1;
}

void CVICALLBACK menuDevOpin(int menuBar, int menuItem, void *callbackData,
							 int panel)
{
	if (subPanelStatus[0] == 1)
	{
		return;
	}
	adminPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_ADM);
	PASSWORDCtrl = PasswordCtrl_ConvertFromString(adminPanelHandle, PANEL_ADM_STRING_ADMIN);
	DisplayPanel(adminPanelHandle);
	if (Language == ENG)
	{
		SetCtrlAttribute(adminPanelHandle, PANEL_ADM_STRING_ADMIN, ATTR_LABEL_TEXT, "Password: ");
		SetCtrlAttribute(adminPanelHandle, PANEL_ADM_COMMANDBUTTON, ATTR_LABEL_TEXT, "Yes");
		SetCtrlAttribute(adminPanelHandle, PANEL_ADM_COMMANDBUTTON_2, ATTR_LABEL_TEXT, "Cancel");
	}
	subPanelStatus[0] = 1;
	return;
}

void CVICALLBACK menuRegister(int menuBar, int menuItem, void *callbackData,
							  int panel)
{
	if (subPanelStatus[1] == 1)
	{
		return;
	}
	devPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_DEV);
	PASSWORDCtrl = PasswordCtrl_ConvertFromString(devPanelHandle, PANEL_DEV_STRING_LICENCE);
	DisplayPanel(devPanelHandle);
	if (Language == ENG)
	{
		SetCtrlAttribute(devPanelHandle, PANEL_DEV_NUMERIC_DAYS, ATTR_LABEL_TEXT, " Probation:");
		SetCtrlAttribute(devPanelHandle, PANEL_DEV_STRING_CUSTOMER, ATTR_LABEL_TEXT, "Serial Number:");
		SetCtrlAttribute(devPanelHandle, PANEL_DEV_STRING_LICENCE, ATTR_LABEL_TEXT, "Licence:");
		SetCtrlAttribute(devPanelHandle, PANEL_DEV_COMMANDBUTTON, ATTR_LABEL_TEXT, "Yes");
		SetCtrlVal(devPanelHandle, PANEL_DEV_TEXTMSG, "days");
	}
	subPanelStatus[1] = 1;
	return;
}

//---------------------------control --------------------

int CVICALLBACK OneClickMode(int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		int ring_mode_value;
		GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);
		switch (ring_mode_value)
		{
		case 1:
			// 直通范围是反射的2倍，但暂不使用
			ReplaceListItem(panelHandle, PANEL_RING_Range, 0, "200 m", 1);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 1, "2 km", 2);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 2, "10 km", 3);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 3, "20 km", 4);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 4, "100 km", 5);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 5, "200 km", 6);
			//break;
		case 2:
			ReplaceListItem(panelHandle, PANEL_RING_Range, 0, "100 m", 1);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 1, "1 km", 2);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 2, "5 km", 3);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 3, "10 km", 4);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 4, "50 km", 5);
			ReplaceListItem(panelHandle, PANEL_RING_Range, 5, "100 km", 6);
			break;
		}
		break;
	}
	return 0;
}

int CVICALLBACK OneClickContinue(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlAttribute(panelHandle, PANEL_STRIPCHART, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_GRAPH, ATTR_VISIBLE, 1);
		sweepType = contSweepType;
		break;
	}
	return 0;
}

int CVICALLBACK OneClickHoldMeas(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		sweepType = holdSweepType;
		break;
	}
	return 0;
}

int CVICALLBACK OneClickSingleSweep(int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlAttribute(panelHandle, PANEL_GRAPH, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_STRIPCHART, ATTR_VISIBLE, 1);
		sweepType = singleSweepType;
		break;
	}
	return 0;
}

int CVICALLBACK OneClickTimingMea(int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlAttribute(panelHandle, PANEL_GRAPH, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_STRIPCHART, ATTR_VISIBLE, 1);
		sweepType = timingSweepType;
		break;
	}
	return 0;
}

int CVICALLBACK OneClickCal(int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		if (calStatus == OFF)
			calStatus = ON;
		else
			calStatus = OFF;
		break;
	}
	return 0;
}

int CVICALLBACK OneClickClear(int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		subscript = 0;
		for (int i = 0; i < MaxPoints; i++)
		{
			batchData[i][0] = 0;
			batchData[i][1] = 0;
			ILData[i] = 0;
		}
		ClearStripChart(panelHandle, PANEL_STRIPCHART);
		break;
	}
	return 0;
}

int CVICALLBACK AdminEXIT(int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		if (Probation_Days == 0)
		{
			QuitUserInterface(0);
		}
		DiscardPanel(adminPanelHandle);
		subPanelStatus[0] = 0;
		break;
	}
	return 0;
}

int CVICALLBACK calRefractionEXIT(int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(calRefractionPanelHandle, PANEL_REFR_NUMERIC, &calRefractionIndex);
		DiscardPanel(calRefractionPanelHandle);
		break;
	}
	subPanelStatus[2] = 0;
	return 0;
}

int CVICALLBACK timingMeaEXIT(int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		int testMeaPoints;
		double testMeaDelay;
		GetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_NUMERIC, &testMeaDelay);
		GetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_NUMERIC_2, &testMeaPoints);
		if (testMeaPoints > 100000 || testMeaPoints < 1)
		{
			MessagePopup("Error", "测量点数应设置为 1 - 100000");
		}
		else
		{
			meaPoints = testMeaPoints;
		}

		if (testMeaDelay > 600.00 || testMeaDelay < 0.10)
		{
			MessagePopup("Error", "测量间隔应设置为 0.10 - 600s");
		}
		else
		{
			meaDelay = testMeaDelay;
		}
		DiscardPanel(timingMeaPanelHandle);
		break;
	}
	subPanelStatus[3] = 0;
	return 0;
}

int CVICALLBACK OneClickAdmOK(int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	char myPassword[11] = "Hello,ODM!";
	char password[20] = {0};
	int y = 1;
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(adminPanelHandle, PANEL_ADM_STRING_ADMIN, password);
		if ((y = strncmp(password, myPassword, 19)) != 0)
		{
			MessagePopup("Error", "密码错误！");
			SetCtrlVal(adminPanelHandle, PANEL_ADM_STRING_ADMIN, "");
		}
		else
		{
			devStatus = ON;
			Manager = ON;
			if (Probation_Days == 0)
			{
				QuitUserInterface(0);
				devPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_DEV);
				PASSWORDCtrl = PasswordCtrl_ConvertFromString(devPanelHandle, PANEL_DEV_STRING_LICENCE);
			}
			DiscardPanel(adminPanelHandle);
			subPanelStatus[0] = 0;
		}
		break;
	}

	return 0;
}

int CVICALLBACK OneClickDevOK(int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		int days = 0;
		char name[20] = {0};
		char licence[20] = {0};
		FILE *fp = NULL;
		GetCtrlVal(devPanelHandle, PANEL_DEV_NUMERIC_DAYS, &days);
		GetCtrlVal(devPanelHandle, PANEL_DEV_STRING_CUSTOMER, name);
		GetCtrlVal(devPanelHandle, PANEL_DEV_STRING_LICENCE, licence);
		switch (Manager)
		{
		case ON:
			fp = fopen("C:\\ProgramData\\ODM\\APD\\lic.dll", "wb");
			if (fp != NULL)
			{
				fprintf(fp, "%d\n", days);
				fprintf(fp, "%s\n", name);
				fprintf(fp, "%s\n", licence);
			}
			fclose(fp);
			fopen("C:\\ProgramData\\ODM\\APD\\fir.dll", "wb");
			fprintf(fp, "%d\n", 1);
			fclose(fp);
			
			if(LimitLengthIndex == 7)
			{
				fopen("C:\\ProgramData\\ODM\\APD\\permission.dll", "wb");
				fprintf(fp, "%d\n", 1);
				SetFileAttributes("C:\\ProgramData\\ODM\\APD\\permission.dll", FILE_ATTRIBUTE_HIDDEN);
			}
			
			fclose(fp);
			DiscardPanel(devPanelHandle);
			SetFileAttributes("C:\\ProgramData\\ODM\\APD\\fir.dll", FILE_ATTRIBUTE_HIDDEN);
			SetFileAttributes("C:\\ProgramData\\ODM\\APD\\lic.dll", FILE_ATTRIBUTE_HIDDEN);
			
			break;
		case OFF:
			int myDays = 0, y = 1;
			char myName[30] = {0};
			char myLicence[30] = {0};
			fp = fopen("C:\\ProgramData\\ODM\\APD\\lic.dll", "rb");
			if (fp != NULL)
			{
				fscanf(fp, "%d\n", &myDays);
				fscanf(fp, "%s\n", myName);
				fscanf(fp, "%s\n", myLicence);
			}
			fclose(fp);
			if ((y = strncmp(myName, name, 19)) != 0)
			{
				MessagePopup("Error", "序列号错误！");
				break;
			}
			if ((y = strncmp(myLicence, licence, 19)) != 0)
			{
				MessagePopup("Error", "密钥错误！");
				break;
			}
			if (days != myDays)
			{
				MessagePopup("Error", "试用期错误！");
				break;
			}
			devStatus = ON;
			DiscardPanel(devPanelHandle);
			if (usetime > Probation_Days || usetime < last_day || (usetime == last_day && (sec + 60) < last_sec))
			{
				QuitUserInterface(0);
			}
			break;
		}
		break;
	}
	subPanelStatus[1] = 0;
	return 0;
}

int CVICALLBACK ConvertLength(int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	int ring_length_unit;
	double M1, M2, M3;
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlIndex(panelHandle, PANEL_RING_LENGTH, &ring_length_unit);
		GetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M1, &M1);
		GetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M2, &M2);
		GetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M3, &M3);

		M1 = SetPrecision_D(0, M1);
		M2 = SetPrecision_D(0, M2);
		M3 = SetPrecision_D(0, M3);

		if (M1 > MaxLength)
		{
			M1 = MaxLength;
			M3 = MaxLength;
		}

		M1 = ConvertUnit_MS(ring_length_unit, M1);
		M2 = ConvertUnit_MS(ring_length_unit, M2);
		M3 = ConvertUnit_MS(ring_length_unit, M3);

		ConvertPrecision_D(ring_length_unit);

		SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M1S, M1);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M2S, M2);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M3S, M3);
		break;
	}
	return 0;
}

int CVICALLBACK OneClickRange(int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	int ring_mode_value;
	int ring_range_value;
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);
		GetCtrlVal(panelHandle, PANEL_RING_Range, &ring_range_value);
		//************************限制测量范围***************************************************  使100Km变灰色
		if (ring_range_value >= LimitLengthIndex) //***********改动此处的值仅使量程档位显示灰色，但是不能限制最大数值，需修改最上面的量程限制值
		{
			SetCtrlAttribute(panelHandle, PANEL_RING_Range, ATTR_TEXT_COLOR, VAL_GRAY);
		}
		else
		{
			SetCtrlAttribute(panelHandle, PANEL_RING_Range, ATTR_TEXT_COLOR, 0x003399FF);
		}
		//************************限制测量范围*************************************************** 使100Km变灰色
		break;
	}
	return 0;
}

void CVICALLBACK menuHelp(int menuBar, int menuItem, void *callbackData,
						  int panel)
{
	system("explorer C:\\ProgramData\\ODM\\APD\\ODM型高精度光延时测量仪用户操作手册.pdf");
}

void CVICALLBACK menuAbout(int menuBar, int menuItem, void *callbackData,
						   int panel)
{
	system("explorer http://www.newkeytech.com/");
}
//================================thread function=========================================//

static int CVICALLBACK ThreadFunction(void *functionData)
{
	if (init == 0)
	{
		init = 1;

		SetCtrlVal(panelHandle, PANEL_TEXTMSG_Caution, "系统正在初始化");
		SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);
		int retflag = 0;

		/*
			retflag = GetInstrCom(&comPort);Delay(0.05);//**********************************************************脱机调试的时候，屏蔽此句，以达到屏蔽串口的效果。还需要屏蔽下文中的串口检测
			if(retflag!=0)
			{
				MessagePopup("Error","设备连接错误！");
				exit(0);
			}
			 */

		// 检测并尝试连接串口，最大 10 秒
		for (int i = 0; i < 20; i++)
		{
			if (i % 2 == 0)
			{
				SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);
			}
			else
			{
				SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_Caution, ATTR_TEXT_COLOR, VAL_BLACK);
			}
			retflag = GetInstrCom(&comPort);
			if (retflag == 0)
			{
				break;
			}
			else if (i == 19)
			{
				MessagePopup("Error", "设备连接超时！");
				exit(0);
			}

			Delay(0.5);
		}

		SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);
		retflag = ConnectVNA();
		if (retflag != 0)
		{
			MessagePopup("Error", "系统连接错误！");
			exit(0);
		}

		//获取 VNA Ready 状态，Ready 状态下才进行下一步设置和测量
		for (int i = 0; i < 24; i++)
		{
			if (i % 2 == 0)
			{
				SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);
			}
			else
			{
				SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_Caution, ATTR_TEXT_COLOR, VAL_BLACK);
			}
			retflag = GetReadyStateVNA();
			if (retflag != 0)
			{
				break;
			}
			else if (i == 23)
			{
				MessagePopup("Error", "系统连接超时！");
				exit(0);
			}

			Delay(0.5);
		}

		retflag = IniVNA();
		if (retflag != 0)
		{
			MessagePopup("Error", "系统初始化错误！");
			exit(0);
		}

		/* 软件开启初始化 */
		Delay(0.01);
		Sendbyte(comPort, 0x44); //关警告灯
		Delay(0.05);
		Sendbyte(comPort, 0x39); //关1310光源
		Delay(0.05);
		Sendbyte(comPort, 0x31); //选择1550通道
		Delay(0.05);
		Sendbyte(comPort, 0x32); //选择直通模式
		Delay(0.2);				 //多等一会
		Sendbyte(comPort, 0x36); //开启1550光源
		Delay(0.01);
		Sendbyte(comPort, 0x41);
		Delay(0.01); //开启VNA供电  **************************默认VNA常闭状态
		/* 软件开启初始化 */

		SetOS1(comPort, WL1550);
		Delay(0.05);
		SetOS2(comPort, THROUGH);
		Delay(0.05);

		SetCtrlVal(panelHandle, PANEL_TEXTMSG_Caution, "接收光功率低！");
		SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_Caution, ATTR_TEXT_COLOR, VAL_BLACK);
	}

	int ring_wl_value,
		ring_mode_value,
		ring_range_value = 1,
		ring_length_unit;
	double refraction_index = 1.467;
	int through_or_reflect = 1;
	char calText[9],
		aveText[4];
	double TimeDelay = 0.0;
	double FiberLength = 0.0;
	double M1, M2, M3;
	double MeanAmp = 0.0;
	double StepTime = 0.01,
		   SweepTime = 0.0;
	int TraceLen = 7;
	int RS232Error = 0,
		RS232CheckNum = 0;
	int DeviceCheckNum = 5;

	double targetLength = 0.0;
	char temp[10] = {0};
	char IL[10] = {0};
	char delta_temp[10] = {0};
	double tempVal = 0.0, calTempVal = 0.0, ILVal = 0.0;
	double range = 0;

	//排序数组
	double sortAmp[TraceLen][aveFactor];
	double sortPhi[TraceLen][aveFactor];

	GetInfoVNA(&TraceLen, &SweepTime);
	SetCtrlAttribute(panelHandle, PANEL_NUMERICSLIDE, ATTR_MIN_VALUE, 0.0);
	SetCtrlAttribute(panelHandle, PANEL_NUMERICSLIDE, ATTR_MAX_VALUE, SweepTime);
	double *freq = (double *)malloc(TraceLen * sizeof(double));

	//申请7个double类数据的内存，数据长度
	if (freq == NULL)
		MessagePopup("Error", "malloc fail!");
	double *amp = (double *)malloc(TraceLen * sizeof(double));
	if (amp == NULL)
		MessagePopup("Error", "malloc fail!");
	double *phi = (double *)malloc(TraceLen * sizeof(double));
	if (phi == NULL)
		MessagePopup("Error", "malloc fail!");
	double *aveAmp = (double *)malloc(TraceLen * sizeof(double));
	if (aveAmp == NULL)
		MessagePopup("Error", "malloc fail!");
	double *avePhi = (double *)malloc(TraceLen * sizeof(double));
	if (avePhi == NULL)
		MessagePopup("Error", "malloc fail!");
	for (int i = 0; i < TraceLen; i++)
	{
		amp[i] = 0;
		phi[i] = 0;
	}
	for (int i = 0; i < TraceLen; i++)
	{
		aveAmp[i] = 0;
		avePhi[i] = 0;
	}

	while (meaStatus != OFF)
	{
		// 每20次检查一遍串口和VNA的连接情况
		//

		++RS232CheckNum; //**********************************************************脱机调试的时候，屏蔽此句，以达到屏蔽串口的效果，，，从此句开始注释
		if (RS232CheckNum % 20 == 0)
		{
			// 检查串口是否连接
			RS232Error = OpenComConfig(comPort, "", 9600, 0, 8, 1, 1, 4);
			if (RS232Error != 0)
			{
				MessagePopup("Error", "串口丢失，请检查后重启软件!");
				exit(0);
			}

			// 检查鉴相模块是否连接
			RS232Error = GetReadyStateVNA();
			if (RS232Error == 0)
			{
				MessagePopup("Error", "系统连接错误，请检查后重启软件!");
				exit(0);
			}
			RS232CheckNum = 0;
		} //**********************************************************脱机调试的时候，屏蔽此句，以达到屏蔽串口的效果，，注释到此结束

		//-------------------Record service time-----------------

		if (devStatus == OFF)
		{
			time_t timep;
			struct tm *p;
			time(&timep);
			p = localtime(&timep);
			FILE *fp;
			GetProbTime();
			if (usetime < last_day || (usetime == last_day && (sec + 60) < last_sec)) //usetime must be increased
			{
				MessagePopup("Error", "system time has been modified!");
				return -1;
			}
			if (usetime > Probation_Days)
			{
				QuitUserInterface(0);
				MessagePopup("Error", "Probation period is over and the program will be closed.\nPlease log in with your account next time");
				return 0;
			}

			fp = fopen("C:\\ProgramData\\ODM\\APD\\prob.dll", "wb");
			fprintf(fp, "%d\n", usetime);
			fprintf(fp, "%d\n", 1900 + p->tm_year);
			fprintf(fp, "%d\n", day2);
			fprintf(fp, "%d\n", sec);
			fclose(fp);
		}

		//-------------------update index-----------------

		GetCtrlVal(panelHandle, PANEL_RING_Wavelength, &ring_wl_value);
		GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);

		// 限制量程范围，请根据需要设置 testRange 的值
		int testRange;
		GetCtrlVal(panelHandle, PANEL_RING_Range, &testRange);
		if (testRange < LimitLengthIndex)
		{
			GetCtrlVal(panelHandle, PANEL_RING_Range, &ring_range_value);
		}

		GetCtrlVal(panelHandle, PANEL_NUMERIC_Refraction, &refraction_index);
		GetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M2S, &M2);

		GetCtrlIndex(panelHandle, PANEL_RING_LENGTH, &ring_length_unit);
		M2 = SetPrecision_D(ring_length_unit, M2);
		targetLength = ConvertUnit_M(ring_length_unit, M2);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M2, targetLength);

		//get IL and Temp
		if (DeviceCheckNum == 3)
		{
			if (ring_wl_value == 1)
			{
				calIL = calIL1550;
			}
			else
			{
				calIL = calIL1310;
			}
			++DeviceCheckNum;
			ReadIL(comPort, IL);
			ILVal = atof(IL);
			sprintf(IL, "%.1f%s", calIL - ILVal, " dB");
			SetCtrlVal(panelHandle, PANEL_TEXTMSG_IL_1, IL);
		}
		else if (DeviceCheckNum > 5)
		{
			DeviceCheckNum = 0;
			ReadTemp(comPort, temp);
			//SetCtrlVal(panelHandle,PANEL_TEXTMSG_TEMP,temp);
			tempVal = atof(temp);
			calTempVal = atof(calTemp);
			sprintf(temp, "%.1f", tempVal);
			SetCtrlVal(panelHandle, PANEL_TEXTMSG_TEMP, temp);
			sprintf(delta_temp, "%.1f", tempVal - calTempVal);
			SetCtrlVal(panelHandle, PANEL_TEXTMSG_TEMP_2, delta_temp);
			if (tempVal - calTempVal > 0 && calStatus == ON)
			{
				SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_TEMP_2, ATTR_TEXT_COLOR, VAL_RED);
				sprintf(delta_temp, "+%.1f", tempVal - calTempVal);
			}
			else if (tempVal - calTempVal < 0 && calStatus == ON)
			{
				SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_TEMP_2, ATTR_TEXT_COLOR, VAL_GREEN);
				sprintf(delta_temp, "%.1f", tempVal - calTempVal);
			}
			else
			{
				SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_TEMP_2, ATTR_TEXT_COLOR, VAL_PANEL_GRAY);
				sprintf(delta_temp, "%.1f", tempVal - calTempVal);
			}
			SetCtrlVal(panelHandle, PANEL_TEXTMSG_TEMP_2, delta_temp);
		}
		else
		{
			++DeviceCheckNum;
		}

		//-------------------set laser and through or reflect--------------------
		switch (ring_wl_value)
		{
		case 1:
			if (LDstatus != WL1550)
			{
				SetOS1(comPort, WL1550);
				Sendbyte(comPort, 0x39);
				Delay(0.01);
				Sendbyte(comPort, 0x36);
				Delay(0.001);
				LDstatus = WL1550;
			}
			break;
		case 2:
			if (LDstatus != WL1310)
			{
				SetOS1(comPort, WL1310);
				Sendbyte(comPort, 0x37);
				Delay(0.01);
				Sendbyte(comPort, 0x38);
				Delay(0.001);
				LDstatus = WL1310;
			}
			break;
		}
		//ReadOS2(comPort,&OSstatus);
		switch (ring_mode_value)
		{
		case 1:
			if (OSstatus != THROUGH)
			{
				SetOS2(comPort, THROUGH);
				OSstatus = THROUGH;
			}
			break;
		case 2:
			if (OSstatus != REFLECT)
			{
				SetOS2(comPort, REFLECT);
				OSstatus = REFLECT;
			}
			break;
		}

		through_or_reflect = ring_mode_value;
		switch (ring_range_value)
		{
		case 1:
			range = 100;
			break;
		case 2:
			range = 1e3;
			break;
		case 3:
			range = 5e3;
			break;
		case 4:
			range = 10e3;
			break;
		case 5:
			range = 50e3;
			break;
		case 6:
			range = 100e3;
			break;
		}

		//-----------calibration------------
		switch (calStatus)
		{
		case ON:
			GetCtrlVal(panelHandle, PANEL_TEXTMSG_CAL, calText);
			if (strcmp(TextON, calText) != 0)
			{
				TrigVNA();
				ReadData(TraceLen, freq, amp, phi);
				calMag = amp[TraceLen - 1];
				CalVNA(ON);
				SetCtrlVal(panelHandle, PANEL_TEXTMSG_CAL, TextON);
				GetCtrlVal(panelHandle, PANEL_TEXTMSG_TEMP, calTemp);
			}
			break;
		case OFF:
			GetCtrlVal(panelHandle, PANEL_TEXTMSG_CAL, calText);
			if (strcmp(TextOFF, calText) != 0)
			{
				calMag = 0;
				CalVNA(OFF);
				SetCtrlVal(panelHandle, PANEL_TEXTMSG_CAL, TextOFF);
				calTemp[0] = '0';
				calTemp[1] = '0';
				calTemp[2] = '.';
				calTemp[3] = '0';
			}
			break;
		}

		//新平均算法,去掉极值

		if (aveStatus == ON)
		{
			for (int i = 0; i < aveFactor; i++)
			{
				TrigVNA();
				for (double t = 0; t < SweepTime; t = t + StepTime)
				{
					SetCtrlVal(panelHandle, PANEL_NUMERICSLIDE, t);
					Delay(StepTime * 1.5);
				}
				ReadData(TraceLen, freq, amp, phi);
				for (int j = 0; j < TraceLen; j++)
				{
					sortAmp[j][i] = amp[j];
					sortPhi[j][i] = phi[j];
				}
				sprintf(aveText, "%d", i + 1);
				SetCtrlVal(panelHandle, PANEL_TEXTMSG_AVE, aveText);
			}

			double t;	  //交换中介
			int flag = 0; //顺序标志
			for (int j = 0; j < TraceLen; j++)
			{
				for (int k = 0; k < aveFactor - 1; k++)
				{
					flag = 0;
					//升序排列
					for (int i = 0; i < aveFactor - 1 - k; i++)
					{
						if (sortAmp[j][i] > sortAmp[j][i + 1])
						{
							t = sortAmp[j][i + 1];
							sortAmp[j][i + 1] = sortAmp[j][i];
							sortAmp[j][i] = t;
							flag++;
						}
					}
					if (flag == 0) //该列排序已经完成
					{
						break;
					}
				}

				for (int k = 0; k < aveFactor - 1; k++)
				{
					flag = 0;
					for (int i = 0; i < aveFactor - 1 - k; i++)
					{
						if (sortPhi[j][i] > sortPhi[j][i + 1])
						{
							t = sortPhi[j][i + 1];
							sortPhi[j][i + 1] = sortPhi[j][i];
							sortPhi[j][i] = t;
							flag++;
						}
					}

					if (flag == 0)
					{
						break;
					}
				}
			}
			for (int i = 2; i < aveFactor - 3; i++) //去掉2个最小，3个最大
			{
				for (int j = 0; j < TraceLen; j++)
				{
					aveAmp[j] = aveAmp[j] + sortAmp[j][i];
					avePhi[j] = avePhi[j] + sortPhi[j][i];
				}
			}
			for (int i = 0; i < TraceLen; i++)
			{
				amp[i] = aveAmp[i] / 5;
				phi[i] = avePhi[i] / 5;
			}
			for (int i = 0; i < TraceLen; i++)
			{
				aveAmp[i] = 0;
				avePhi[i] = 0;
			}
			for (int i = 0; i < aveFactor; i++)
			{
				for (int j = 0; j < TraceLen; j++)
				{
					sortAmp[j][i] = 0;
					sortPhi[j][i] = 0;
				}
			}
		}
		else
		{
			sprintf(aveText, "%d", (int)0);
			SetCtrlVal(panelHandle, PANEL_TEXTMSG_AVE, aveText);
			TrigVNA();
			for (double i = 0; i < SweepTime; i = i + StepTime)
			{
				SetCtrlVal(panelHandle, PANEL_NUMERICSLIDE, i);
				Delay(StepTime * 1.5);
			}
			ReadData(TraceLen, freq, amp, phi);
		}
		FiberDetector(freq, amp, phi, refraction_index * calRefractionIndex, calMag, &TimeDelay, &FiberLength, &MeanAmp);
		PowerCaution(comPort, amp[6] + calMag);
		TimeDelay = TimeDelay / through_or_reflect;
		FiberLength = FiberLength / through_or_reflect;
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M1, FiberLength);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M3, targetLength - FiberLength);
		GetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M1, &M1);
		GetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M3, &M3);

		if (M1 > MaxLength)
		{
			M1 = MaxLength;
			M3 = MaxLength;
			TimeDelay = 0;
			targetLength = 0;
		}
		if (M1 < -MaxLength)
		{
			M1 = -MaxLength;
			M3 = -MaxLength;
			TimeDelay = 0;
			targetLength = 0;
		}

		GetCtrlIndex(panelHandle, PANEL_RING_LENGTH, &ring_length_unit);

		M1 = SetPrecision_D(0, M1);
		M3 = SetPrecision_D(0, M3);

		M1 = ConvertUnit_MS(ring_length_unit, M1);
		M3 = ConvertUnit_MS(ring_length_unit, M3);

		SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M1S, M1);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Distance_M3S, M3);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Delay_M1, TimeDelay);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Delay_M2, targetLength / c0);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_Delay_M3, targetLength / c0 - TimeDelay);

		switch (sweepType)
		{
		case contSweepType:
			PlotResponse(contSweepType, MeanAmp, FiberLength, range);
			break;
		case singleSweepType:
			batchData[subscript][0] = FiberLength;
			batchData[subscript][1] = TimeDelay;
			ILData[subscript] = atof(IL);
			if (subscript > 98)
				subscript = subscript - 99;
			else
				subscript = subscript + 1;
			PlotResponse(singleSweepType, MeanAmp, FiberLength, range);
			sweepType = holdSweepType;
			break;
		case timingSweepType:
			batchData[subscript][0] = FiberLength;
			batchData[subscript][1] = TimeDelay;
			ILData[subscript] = atof(IL);
			if (subscript >= meaPoints)
				break;
			else
				subscript++;
			PlotResponse(timingSweepType, MeanAmp, FiberLength, range);
			Delay(meaDelay - 0.1); //补偿掉软件本身的扫描循环时间
			break;
		case holdSweepType:
			break;
		default:
			PlotResponse(contSweepType, MeanAmp, FiberLength, range);
			break;
		}
	}
	free(freq);
	free(amp);
	free(phi);
	CloseCom(comPort);
	return 0;
}

//--------------------------myFunction------------------------//

void SaveData(void)
{
	int fileflag = 0;
	char SavePath[1000];
	FILE *fp;
	fileflag = FileSelectPopupEx("", "*.csv", "", "", VAL_SAVE_BUTTON, 0, 0, SavePath);
	if (fileflag > 0)
	{
		fp = fopen(SavePath, "w");
		for (int i = 0; i < meaPoints; i++)
			fprintf(fp, "%d,%.4lf, %.4lf, %.2f \n", i + (int)1, batchData[i][0], batchData[i][1], ILData[i]);
		fclose(fp);
	};
}

void DisplayRS232Error(int RS232Error)
{
	char ErrorMessage[200];
	switch (RS232Error)
	{
	default:
		if (RS232Error < 0)
		{
			Fmt(ErrorMessage, "%s<RS232 error number %i", RS232Error);
			MessagePopup("RS232 Message", ErrorMessage);
		}
		break;
	case 0:
		MessagePopup("RS232 Message", "No errors.");
		break;
	case -2:
		Fmt(ErrorMessage, "%s", "Invalid port number (must be in the "
								"range 1 to 8).");
		MessagePopup("RS232 Message", ErrorMessage);
		break;
	case -3:
		Fmt(ErrorMessage, "%s", "No port is open.\n"
								"Check COM Port setting in Configure.");
		MessagePopup("RS232 Message", ErrorMessage);
		break;
	case -99:
		Fmt(ErrorMessage, "%s", "Timeout error.\n\n"
								"Either increase timeout value,\n"
								"       check COM Port setting, or\n"
								"       check device.");
		MessagePopup("RS232 Message", ErrorMessage);
		break;
	}
}

void DispXstopMsg(void)
{
	int ring_mode_value = -1,
		ring_range_value = -1;
	GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);
	// 限制量程范围
	int testRange;
	GetCtrlVal(panelHandle, PANEL_RING_Range, &testRange);
	if (testRange < LimitLengthIndex)
	{
		GetCtrlVal(panelHandle, PANEL_RING_Range, &ring_range_value);
	}

	switch (ring_range_value)
	{
	case 1:
		switch (ring_mode_value)
		{
		case 1:
			// SetAxisScalingMode (panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 200);
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 100);
			break;
		case 2:
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 100);
			break;
		}
		break;
	case 2:
		switch (ring_mode_value)
		{
		case 1:
			//SetAxisScalingMode (panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 2000);
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 1000);
			break;
		case 2:
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 1000);
			break;
		}
		break;
	case 3:
		switch (ring_mode_value)
		{
		case 1:
			//SetAxisScalingMode (panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 10000);
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 5000);
			break;
		case 2:
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 5000);
			break;
		}
		break;

	case 4:
		switch (ring_mode_value)
		{
		case 1:
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 10000);
			break;
		case 2:
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 10000);
			break;
		}
		break;
	case 5:
		switch (ring_mode_value)
		{
		case 1:
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 50000);
			break;
		case 2:
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 50000);
			break;
		}
		break;

	case 6:
		switch (ring_mode_value)
		{
		case 1:
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 100000);
			break;
		case 2:
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 100000);
			break;
		}
		break;

	default:
		/*switch(ring_mode_value)
			{
				case 1:
					SetAxisScalingMode (panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 200);
					break;
				case 2:
					SetAxisScalingMode (panelHandle, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 100);
					break;
			}*/

		break;
	}
}

void PlotResponse(int sweep_type, double mean_amp, double fiber_length, double range)
{
	double xCurs, yCurs;
	int MaxIndex = 0,
		MinIndex = 0;
	double MaxValue = 0,
		   MinValue = 0;
	int Xmode, Ymode;
	double Xmin,
		Xmax,
		Ymin,
		Ymax;
	double x[101] = {0};
	double ht[101] = {-50};
	char markerLabel[30] = {0};
	switch (sweep_type)
	{
	case contSweepType:
		DeleteGraphPlot(panelHandle, PANEL_GRAPH, -1, VAL_DELAYED_DRAW);
		TimeDomain(mean_amp, fiber_length, range, ht, &MaxIndex, &MaxValue, &MinIndex, &MinValue);
		for (int i = 0; i < 101; i++)
			x[i] = i * range / 100;
		DispXstopMsg();
		PlotXY(panelHandle, PANEL_GRAPH, x, ht, 101, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, 0x0000CCFF);
		if (MaxValue > 0)
		{
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, floor(MaxValue / 10) * 10 - 40, floor(MaxValue / 10) * 10 + 10);
		}
		else
		{
			SetAxisScalingMode(panelHandle, PANEL_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, -50.0, 0.0);
		}
		SetGraphCursor(panelHandle, PANEL_GRAPH, 1, MaxIndex, MaxValue);
		GetGraphCursor(panelHandle, PANEL_GRAPH, 1, &xCurs, &yCurs);
		Fmt(markerLabel, "%s<Marker1     %f[p1] dB", mean_amp / 2);
		PlotText(panelHandle, PANEL_GRAPH, xCurs, yCurs, markerLabel, VAL_APP_META_FONT, VAL_RED, VAL_TRANSPARENT);
		break;
	case holdSweepType:
		break;
	case singleSweepType:
		PlotStripChartPoint(panelHandle, PANEL_STRIPCHART, fiber_length);
		GetAxisRange(panelHandle, PANEL_STRIPCHART, &Xmode, &Xmin, &Xmax, &Ymode, &Ymin, &Ymax);
		break;
	case timingSweepType:
		PlotStripChartPoint(panelHandle, PANEL_STRIPCHART, fiber_length);
		GetAxisRange(panelHandle, PANEL_STRIPCHART, &Xmode, &Xmin, &Xmax, &Ymode, &Ymin, &Ymax);
		break;
	}
}

double SetPrecision_D(int choice, double X)
{
	long long t;
	double x;
	switch (choice)
	{
	case 0:
		if (X >= 0)
			t = (long long)(X * 10e4 + 0.5);
		else
			t = (long long)(X * 10e4 - 0.5);
		x = t;
		X = x / 10e4;
		break;
	case 1:
		if (X >= 0)
			t = (long long)(X * 10e7 + 0.5);
		else
			t = (long long)(X * 10e7 - 0.5);
		x = t;
		X = x / 10e7;
		break;
	case 2:
		if (X >= 0)
			t = (long long)(X * 100 + 0.5);
		else
			t = (long long)(X * 100 - 0.5);
		x = t;
		X = x / 100;
		break;
	}
	return X;
}

double ConvertUnit_M(int choice, double X)
{
	switch (choice)
	{
	case 0:
		X = X;
		break;
	case 1:
		X = X * 1000.0;
		break;
	case 2:
		X = X / 1000.0;
		break;
	}
	return X;
}
double ConvertUnit_MS(int choice, double X)
{
	switch (choice)
	{
	case 0:
		X = X;
		break;
	case 1:
		X = X / 1000.0;
		break;
	case 2:
		X = X * 1000.0;
		break;
	}
	return X;
}
void ConvertPrecision_D(int choice)
{
	switch (choice)
	{
	case 0:
		SetCtrlAttribute(panelHandle, PANEL_NUMERIC_Distance_M1S, ATTR_PRECISION, 5);
		SetCtrlAttribute(panelHandle, PANEL_NUMERIC_Distance_M2S, ATTR_PRECISION, 5);
		SetCtrlAttribute(panelHandle, PANEL_NUMERIC_Distance_M3S, ATTR_PRECISION, 5);
		break;
	case 1:
		SetCtrlAttribute(panelHandle, PANEL_NUMERIC_Distance_M1S, ATTR_PRECISION, 7);
		SetCtrlAttribute(panelHandle, PANEL_NUMERIC_Distance_M2S, ATTR_PRECISION, 7);
		SetCtrlAttribute(panelHandle, PANEL_NUMERIC_Distance_M3S, ATTR_PRECISION, 7);
		break;
	case 2:
		SetCtrlAttribute(panelHandle, PANEL_NUMERIC_Distance_M1S, ATTR_PRECISION, 2);
		SetCtrlAttribute(panelHandle, PANEL_NUMERIC_Distance_M2S, ATTR_PRECISION, 2);
		SetCtrlAttribute(panelHandle, PANEL_NUMERIC_Distance_M3S, ATTR_PRECISION, 2);
		break;
	}
}
void PowerCaution(int comPort, double mean_amp)
{
	int sendByte, cautionStatus;
	if (mean_amp / 2 <= -25)
	{
		cautionStatus = ON;
		SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);
		sendByte = 0x43;
		Delay(0.005);
	}
	else
	{
		cautionStatus = OFF;
		SetCtrlAttribute(panelHandle, PANEL_TEXTMSG_Caution, ATTR_TEXT_COLOR, VAL_BLACK);
		sendByte = 0x44;
		Delay(0.005);
	}
	if (Caution != cautionStatus)
	{
		Caution = cautionStatus;
		Sendbyte(comPort, sendByte);
	}
}
void GetProbTime()
{
	FILE *fp;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	fp = fopen("C:\\ProgramData\\ODM\\APD\\prob.dll", "rb");
	fscanf(fp, "%d\n", &usetime);
	fscanf(fp, "%d\n", &year);
	fscanf(fp, "%d\n", &day2);
	fscanf(fp, "%d\n", &sec);
	fclose(fp);

	deltaYear = (1900 + p->tm_year) - year;
	day1 = day2 - usetime;
	last_day = usetime;
	last_sec = sec;
	day2 = p->tm_yday;
	usetime = day2 - day1 + 365 * deltaYear;
	hour = p->tm_hour;
	min = p->tm_min;
	sec = hour * 3600 + min * 60 + p->tm_sec;
}

void CVICALLBACK OpenAPP(int menuBar, int menuItem, void *callbackData,
						 int panel)
{
	meaStatus = OFF;
	DiscardPanel(panelHandle);
	//Sendbyte(comPort,0x39);    //切换软件的时候发的指令
	CloseCom(comPort);
	CloseCom(comPort);
	DisconnectVNA();
	Delay(0.05);
	QuitUserInterface(0);
	Version = 1;
	return;
}

//文字翻译部分，平常不用关心
void CVICALLBACK TransENG(int menuBar, int menuItem, void *callbackData,
						  int panel)
{
	int height = 0;
	GetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON1, ATTR_HEIGHT, &height);
	int textSize = 15 * height / 39;

	Language = ENG;
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON1, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON2, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON3, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON4, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON5, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON6, ATTR_LABEL_POINT_SIZE, textSize);
	//测量按钮部分
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON1, ATTR_LABEL_TEXT, "Continuous");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON2, ATTR_LABEL_TEXT, "Pause");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON3, ATTR_LABEL_TEXT, "Single");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON4, ATTR_LABEL_TEXT, "Timing");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON5, ATTR_LABEL_TEXT, "Clear");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON6, ATTR_LABEL_TEXT, "Calibration");
	//参数、状态文本部分
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_TITLE, "Optical Delay Information");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Wavelength, "Wavelength(nm)");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Mode, "Mode");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Range, "Range");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Refraction, "Refractive Index");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Marker, "Marker");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Delay, "Delay(ns)");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_CAL_2, "Cal");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_AVE_2, "Ave");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_TEMP_3, "TempDiffer");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Caution, "Low Power !");
	ReplaceListItem(panelHandle, PANEL_RING_Mode, 0, "Through", 1);
	ReplaceListItem(panelHandle, PANEL_RING_Mode, 1, "Reflect", 2);
	ReplaceListItem(panelHandle, PANEL_RING_LENGTH, 0, "Length(m)", 1);
	ReplaceListItem(panelHandle, PANEL_RING_LENGTH, 1, "Length(km)", 2);
	ReplaceListItem(panelHandle, PANEL_RING_LENGTH, 2, "Length(mm)", 3);

	//菜单部分
	SetMenuBarAttribute(menuBar, MENUBAR_File, ATTR_MENU_NAME, "File");
	SetMenuBarAttribute(menuBar, MENUBAR_File_ITEM1_1, ATTR_ITEM_NAME, "Save Data");
	SetMenuBarAttribute(menuBar, MENUBAR_File_ITEM1_3, ATTR_ITEM_NAME, "Print");

	SetMenuBarAttribute(menuBar, MENUBAR_FUNCTION, ATTR_MENU_NAME, "Tool");
	SetMenuBarAttribute(menuBar, MENUBAR_FUNCTION_ITEM2_1, ATTR_ITEM_NAME, "Average");
	SetMenuBarAttribute(menuBar, MENUBAR_FUNCTION_ITEM2_2, ATTR_ITEM_NAME, "Back to ODM_v1");

	SetMenuBarAttribute(menuBar, MENUBAR_SET, ATTR_MENU_NAME, "Options");
	SetMenuBarAttribute(menuBar, MENUBAR_SET_ITEM3_1, ATTR_ITEM_NAME, "Refractive Index Correction");
	SetCtrlAttribute(calRefractionPanelHandle, PANEL_REFR_NUMERIC, ATTR_LABEL_TEXT, "Refractive Index Correction");
	SetCtrlAttribute(calRefractionPanelHandle, PANEL_REFR_COMMANDBUTTON, ATTR_LABEL_TEXT, "Yes");
	SetMenuBarAttribute(menuBar, MENUBAR_SET_ITEM3_2, ATTR_ITEM_NAME, "Timing Setting");
	SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_NUMERIC, ATTR_LABEL_TEXT, "Delay");
	SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_NUMERIC_2, ATTR_LABEL_TEXT, "Points");
	SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_COMMANDBUTTON, ATTR_LABEL_TEXT, "Yes");
	SetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_TEXTMSG, "s");

	SetMenuBarAttribute(menuBar, MENUBAR_LANGUAGE, ATTR_MENU_NAME, "Language");
	SetMenuBarAttribute(menuBar, MENUBAR_LANGUAGE_ITEM4_1, ATTR_ITEM_NAME, "Chinese");
	SetMenuBarAttribute(menuBar, MENUBAR_LANGUAGE_ITEM4_2, ATTR_ITEM_NAME, "English");

	SetMenuBarAttribute(menuBar, MENUBAR_OTHER, ATTR_MENU_NAME, "Other");
	SetMenuBarAttribute(menuBar, MENUBAR_OTHER_ITEM5_1, ATTR_ITEM_NAME, "Help");
	SetMenuBarAttribute(menuBar, MENUBAR_OTHER_ITEM5_2, ATTR_ITEM_NAME, "About");
	SetMenuBarAttribute(menuBar, MENUBAR_OTHER_ITEM5_3, ATTR_ITEM_NAME, "Developer options");
	SetCtrlAttribute(adminPanelHandle, PANEL_ADM_STRING_ADMIN, ATTR_LABEL_TEXT, "Password: ");
	SetCtrlAttribute(adminPanelHandle, PANEL_ADM_COMMANDBUTTON, ATTR_LABEL_TEXT, "Yes");
	SetCtrlAttribute(adminPanelHandle, PANEL_ADM_COMMANDBUTTON_2, ATTR_LABEL_TEXT, "Cancel");
	SetMenuBarAttribute(menuBar, MENUBAR_OTHER_ITEM5_4, ATTR_ITEM_NAME, "Register and login");
	SetCtrlAttribute(devPanelHandle, PANEL_DEV_NUMERIC_DAYS, ATTR_LABEL_TEXT, "Probation:");
	SetCtrlAttribute(devPanelHandle, PANEL_DEV_STRING_CUSTOMER, ATTR_LABEL_TEXT, "Serial Number:");
	SetCtrlAttribute(devPanelHandle, PANEL_DEV_STRING_LICENCE, ATTR_LABEL_TEXT, "Licence:");
	SetCtrlAttribute(devPanelHandle, PANEL_DEV_COMMANDBUTTON, ATTR_LABEL_TEXT, "Yes");
	SetCtrlVal(devPanelHandle, PANEL_DEV_TEXTMSG, "days");
}

void CVICALLBACK TransCHN(int menuBar, int menuItem, void *callbackData,
						  int panel)
{
	int height = 0;
	GetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON1, ATTR_HEIGHT, &height);
	int textSize = 17 * height / 39;

	Language = CHN;
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON1, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON2, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON3, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON4, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON5, ATTR_LABEL_POINT_SIZE, textSize);
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON6, ATTR_LABEL_POINT_SIZE, textSize);
	//测量按钮部分
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON1, ATTR_LABEL_TEXT, "连续测量");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON2, ATTR_LABEL_TEXT, "暂停测量");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON3, ATTR_LABEL_TEXT, "单次测量");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON4, ATTR_LABEL_TEXT, "定时测量");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON5, ATTR_LABEL_TEXT, "清除");
	SetCtrlAttribute(panelHandle, PANEL_COMMANDBUTTON6, ATTR_LABEL_TEXT, "校准");
	//参数、状态文本部分
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_TITLE, "光时延信息图");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Wavelength, "波长(nm)");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Mode, "测量方式");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Range, "范围");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Refraction, "折射率系数");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Marker, "标记");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Delay, "时延(ns)");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_CAL_2, "校准");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_AVE_2, "平均");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_TEMP_3, "温差");
	SetCtrlVal(panelHandle, PANEL_TEXTMSG_Caution, "接收光功率低！");
	ReplaceListItem(panelHandle, PANEL_RING_Mode, 0, "直通", 1);
	ReplaceListItem(panelHandle, PANEL_RING_Mode, 1, "反射", 2);
	ReplaceListItem(panelHandle, PANEL_RING_LENGTH, 0, "长度(m)", 1);
	ReplaceListItem(panelHandle, PANEL_RING_LENGTH, 1, "长度(km)", 2);
	ReplaceListItem(panelHandle, PANEL_RING_LENGTH, 2, "长度(mm)", 3);

	//菜单部分
	SetMenuBarAttribute(menuBar, MENUBAR_File, ATTR_MENU_NAME, "文件");
	SetMenuBarAttribute(menuBar, MENUBAR_File_ITEM1_1, ATTR_ITEM_NAME, "保存数据");
	SetMenuBarAttribute(menuBar, MENUBAR_File_ITEM1_3, ATTR_ITEM_NAME, "打印");

	SetMenuBarAttribute(menuBar, MENUBAR_FUNCTION, ATTR_MENU_NAME, "功能");
	SetMenuBarAttribute(menuBar, MENUBAR_FUNCTION_ITEM2_1, ATTR_ITEM_NAME, "平均");
	SetMenuBarAttribute(menuBar, MENUBAR_FUNCTION_ITEM2_2, ATTR_ITEM_NAME, "切换至精细扫描模式");

	SetMenuBarAttribute(menuBar, MENUBAR_SET, ATTR_MENU_NAME, "设置");
	SetMenuBarAttribute(menuBar, MENUBAR_SET_ITEM3_1, ATTR_ITEM_NAME, "折射率校正系数");
	SetCtrlAttribute(calRefractionPanelHandle, PANEL_REFR_NUMERIC, ATTR_LABEL_TEXT, "折射率校正系数");
	SetCtrlAttribute(calRefractionPanelHandle, PANEL_REFR_COMMANDBUTTON, ATTR_LABEL_TEXT, "确定");
	SetMenuBarAttribute(menuBar, MENUBAR_SET_ITEM3_2, ATTR_ITEM_NAME, "定时设置");
	SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_NUMERIC, ATTR_LABEL_TEXT, "测量延迟");
	SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_NUMERIC_2, ATTR_LABEL_TEXT, "测量点数");
	SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_COMMANDBUTTON, ATTR_LABEL_TEXT, "确定");
	SetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_TEXTMSG, "秒");

	SetMenuBarAttribute(menuBar, MENUBAR_LANGUAGE, ATTR_MENU_NAME, "语言");
	SetMenuBarAttribute(menuBar, MENUBAR_LANGUAGE_ITEM4_1, ATTR_ITEM_NAME, "中文");
	SetMenuBarAttribute(menuBar, MENUBAR_LANGUAGE_ITEM4_2, ATTR_ITEM_NAME, "英文");

	SetMenuBarAttribute(menuBar, MENUBAR_OTHER, ATTR_MENU_NAME, "其他");
	SetMenuBarAttribute(menuBar, MENUBAR_OTHER_ITEM5_1, ATTR_ITEM_NAME, "帮助");
	SetMenuBarAttribute(menuBar, MENUBAR_OTHER_ITEM5_2, ATTR_ITEM_NAME, "关于");
	SetMenuBarAttribute(menuBar, MENUBAR_OTHER_ITEM5_3, ATTR_ITEM_NAME, "开发者选项");
	SetCtrlAttribute(adminPanelHandle, PANEL_ADM_STRING_ADMIN, ATTR_LABEL_TEXT, "密码");
	SetCtrlAttribute(adminPanelHandle, PANEL_ADM_COMMANDBUTTON, ATTR_LABEL_TEXT, "确定");
	SetCtrlAttribute(adminPanelHandle, PANEL_ADM_COMMANDBUTTON_2, ATTR_LABEL_TEXT, "取消");
	SetMenuBarAttribute(menuBar, MENUBAR_OTHER_ITEM5_4, ATTR_ITEM_NAME, "注册/登录");
	SetCtrlAttribute(devPanelHandle, PANEL_DEV_NUMERIC_DAYS, ATTR_LABEL_TEXT, "使用时长：");
	SetCtrlAttribute(devPanelHandle, PANEL_DEV_STRING_CUSTOMER, ATTR_LABEL_TEXT, "ODM 序列号");
	SetCtrlAttribute(devPanelHandle, PANEL_DEV_STRING_LICENCE, ATTR_LABEL_TEXT, "许可证密钥");
	SetCtrlAttribute(devPanelHandle, PANEL_DEV_COMMANDBUTTON, ATTR_LABEL_TEXT, "确定");
	SetCtrlVal(devPanelHandle, PANEL_DEV_TEXTMSG, "天");
}
