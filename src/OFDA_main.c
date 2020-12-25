
//==============================================================================
//
// Title:		OFDA
// Purpose:		Optical fiber transfer delay.
//
// Created on:	2018/5/27 by Shupeng Li.
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
#include "OFDA_V6.h"
#include "TRVNA.h"
#include "DSP.h"
#include "Device.h"
#include "constant.h"
#include "pwctrl.h"
#include "OFDA_V6.h"

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
static int userMode = OFF;                                         // ��ͨ�û���ʹ��ʱ�䱻���ƣ��ⳤ��Χ������
static int suserMode = OFF;                                        // �����û���������ʹ�ã��ⳤ��Χ������
static int adminMode = OFF;                                        // ����Ա�����һ������
static int maxLengthIndex = 3;                                     // ��󿪷����̵�Ĭ��ֵ�� 100km �� 6��admin ״̬����������
static int limitLengthIndex = 1;                                   // �ɱ�����
static double meaDistance[6] = {100, 1e3, 5e3, 10e3, 50e3, 100e3}; // ���̱�
static int powerWarn = OFF;
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
static int activateDays = 0;
static int LDstatus = WL1550;
static int OSstatus = THROUGH;
static int Language = CHN;
static int panelStatus[5] = {0, 0, 0, 0, 0}; // ���״̬��0 ��ʾ�رգ�1��ʾ����.P[0] Ϊ�����
static double calIL = 0;
static double calIL1550 = 7;   //1550����⹦��
static double calIL1310 = 7.0; //1310����⹦��
static char Version = 6;
static char ODM_Name[19] = {0};
static int PASSWORDCtrl = 0;
static int year = 0;
static int day = 0;
static int usetime = 0;
static int hour = 0;
static int min = 0;
static int sec = 0;
static int last_day = 0;
static int last_sec = 0;
static char license[4][5] = {0};
static int threadFunctionId = 0;
static int initStatus = 0;
static int licNum = 0;
//==============================================================================
double calData[60][2] = {0};
int noComPort = 0;
//==============================================================================
// Static functions
static int CVICALLBACK ThreadFunction(void *functionData);
static void checkPower(int comPort, double mean_amp);
static void saveData(void);
static void displayRS232Error(int RS232Error);
static void dispXstopMsg(void);
static void plotResponse(int sweep_type, double mean_amp, double fiber_length, double range);
static double setPrecision(int, double);         // 0.1mm
static double setUnitOfLength(int, double, int); // km,m,mm ---> m
static void setPrecisionOfPanel(int);            //precision=0.1mm,0.0001m......
static void updateUseTime(void);
static int checkLicense(char license[4][5], char ODM_Name[19], int *date, int *licNum);
static void setLanguage(int Language, int menuBar);
static int setVNA();
static int setDevice();
//==============================================================================

int main(int argc, char *argv[])
{
    // ɾ��Ĭ�ϵ�������ͼ��
    SetSystemAttribute(ATTR_TASKBAR_BUTTON_VISIBLE, 0);

    // ����ʱ��ָ�뼰�ļ�ָ��
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    FILE *fp;

    int error = 0;
    ssize_t size;

    // ��ȡ����
    if (GetFileInfo("C:\\ProgramData\\ODM\\conf.dll", &size) != 0)
    {
        fp = fopen("C:\\ProgramData\\ODM\\conf.dll", "r");
        fscanf(fp, "%d\n", &maxLengthIndex);
        fscanf(fp, "%d\n", &noComPort);
        fscanf(fp, "%lf\n", &calIL1550);
        fscanf(fp, "%lf\n", &calIL1310);
        fclose(fp);
        SetFileAttributes("C:\\ProgramData\\ODM\\conf.dll", FILE_ATTRIBUTE_HIDDEN);
    }

    //--------------------------------��ȨУ��------------------------------------

    // ���ǵ�һ��ʹ�ã���ֱ�ӵ���������棬��ʼ��֤
    if (GetFileInfo("C:\\ProgramData\\ODM\\lic.dll", &size) == 0 || GetFileInfo("C:\\ProgramData\\ODM\\licId.dll", &size) == 0)
    {
        devPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_DEV);
        DisplayPanel(devPanelHandle);
        panelStatus[2] = 1;
        RunUserInterface();

        // �����֤ʧ�ܣ���û�� lic �ļ�����������
        // ����ɹ����ͻ����� lic �ļ������½� prob ���ڼ�ʱ
        if (GetFileInfo("C:\\ProgramData\\ODM\\lic.dll", &size) == 0 || GetFileInfo("C:\\ProgramData\\ODM\\licId.dll", &size) == 0)
        {
            return -1;
        }
        else
        {
            fp = fopen("C:\\ProgramData\\ODM\\prob.dll", "wb");
            fprintf(fp, "%d\n", 0);
            fprintf(fp, "%d\n", 1900 + p->tm_year);
            fprintf(fp, "%d\n", p->tm_yday);
            hour = p->tm_hour;
            min = p->tm_min;
            sec = hour * 3600 + min * 60 + p->tm_sec;
            fprintf(fp, "%d\n", sec);
            fclose(fp);
            SetFileAttributes("C:\\ProgramData\\ODM\\prob.dll", FILE_ATTRIBUTE_HIDDEN);
        }
    }
    else
    {
        // �����ǵ�һ��ʹ�ã������ prob �ļ�����ʣʱ�����жԱȣ��������޷���¼�����¼���
        int lastLicNum;
        fp = fopen("C:\\ProgramData\\ODM\\licId.dll", "r");
        fscanf(fp, "%d\n", &lastLicNum);
        fclose(fp);
        fp = fopen("C:\\ProgramData\\ODM\\lic.dll", "rb");
        for (int i = 0; i < 4; i++)
        {
            fscanf(fp, "%s\n", license[i]);
        }
        fscanf(fp, "%s\n", ODM_Name);
        fclose(fp);

        if (checkLicense(license, ODM_Name, &activateDays, &licNum) != 0 || licNum < lastLicNum)
        {
            userMode = OFF;
            devPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_DEV);
            DisplayPanel(devPanelHandle);
            panelStatus[2] = 1;
            RunUserInterface();
        }

        else if (activateDays != 0)
        {

            updateUseTime();

            // ��ʱ���
            if (usetime > activateDays)
            {
                devPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_DEV);
                DisplayPanel(devPanelHandle);
                panelStatus[2] = 1;
                RunUserInterface();
            }
            else if (usetime < last_day || (usetime == last_day && (sec + 60) < last_sec)) // ��ֹ�û���ʱ����ǰ��
            {
                MessagePopup("Error", "system time has been modified!");
            }
            else
            {
                userMode = ON;
            }

            if (userMode == OFF && suserMode == OFF)
            {
                return -1;
            }
        }
        else
        {
            userMode = ON;
            suserMode = ON; //����ʹ�ã����ټ��
        }
    }

    if (userMode == OFF && suserMode == OFF)
    {
        return -1;
    }

    // ------------------------��ȨУ����������ؽ���������߳�-------------------------------------

    nullChk(InitCVIRTE(0, argv, 0));
    errChk(panelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL));
    // Schedule two thread functions
    CmtScheduleThreadPoolFunction(DEFAULT_THREAD_POOL_HANDLE, ThreadFunction, NULL, &threadFunctionId);
    // display the panel and run the user interface
    errChk(DisplayPanel(panelHandle));
    panelStatus[0] = 1;
    errChk(RunUserInterface());

    // ������رպ�鿴�Ƿ�Ϊ�汾�л�
    if (Version == 1)
    {
        fp = fopen("C:\\ProgramData\\ODM\\version.dll", "wb");
        fprintf(fp, "%d\n", 1);
        fclose(fp);
        Delay(0.1);
        system(".\\OFDA.exe");
        Delay(0.1);
        fp = fopen("C:\\ProgramData\\ODM\\version.dll", "wb");
        fprintf(fp, "%d\n", 5);
        fclose(fp);
        SetFileAttributes("C:\\ProgramData\\ODM\\version.dll", FILE_ATTRIBUTE_HIDDEN);
    }

Error:
    if (panelStatus[0] > 0)
        DiscardPanel(panelHandle);
    return -1;
}

//==============================================================================
// ���ر�ʱ���¼��ص�����
// �����
int CVICALLBACK panelCB(int panel, int event, void *callbackData,
                        int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
    {

        meaStatus = OFF; //��������ر�ʱ��Ĳ���
        initStatus == OFF &&CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE, threadFunctionId, OPT_TP_PROCESS_EVENTS_WHILE_WAITING);
        CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE, threadFunctionId);
        DiscardPanel(panelHandle);
        Delay(0.1);
        Sendbyte(comPort, 0x33); //�ָ������䣬��Ҫ���ã���ֱͨ��
        Delay(0.05);
        Sendbyte(comPort, 0x39); //�ر�1310��Դ
        Delay(0.05);
        Sendbyte(comPort, 0x37); //�ر�1550��Դ
        Delay(0.05);
        Sendbyte(comPort, 0x44); //�رվ����
        Delay(0.05);
        SetOS2(comPort, REFLECT);
        //	Delay(0.01);
        noComPort || CloseCom(comPort);
        noComPort || CloseCom(comPort);
        DisconnectVNA();
        Delay(0.1);
        FILE *fp;
        fp = fopen("C:\\ProgramData\\ODM\\version.dll", "wb");
        fprintf(fp, "%d\n", 5);
        fclose(fp);
        SetFileAttributes("C:\\ProgramData\\ODM\\version.dll", FILE_ATTRIBUTE_HIDDEN);
        QuitUserInterface(0);
        panelStatus[0] = 0;
    }
    return 0;
}

// ��ʱ�����������
int CVICALLBACK panelTIMM(int panel, int event, void *callbackData,
                          int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
    {
        DiscardPanel(timingMeaPanelHandle);
        panelStatus[4] == 0;
    }
    return 0;
}

// ������У��ϵ���������
int CVICALLBACK panelREFR(int panel, int event, void *callbackData,
                          int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
    {
        DiscardPanel(calRefractionPanelHandle);
        panelStatus[3] = 0;
    }
    return 0;
}

// ע�����
int CVICALLBACK panelDEV(int panel, int event, void *callbackData,
                         int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
    {
        DiscardPanel(devPanelHandle);
        panelStatus[2] = 0;
        if (panelStatus[0] == 0 || usetime > activateDays || usetime < last_day || (usetime == last_day && (sec + 60) < last_sec))
        {
            QuitUserInterface(0);
        }
    }
    return 0;
}

// ϵͳ�˻����
int CVICALLBACK panelADM(int panel, int event, void *callbackData,
                         int eventData1, int eventData2)
{
    switch (event)
    {
    case EVENT_CLOSE:
        DiscardPanel(adminPanelHandle);
        panelStatus[1] = 0;

        break;
    }
    return 0;
}

//==============================================================================
// -----------------------------�˵��ص�����------------------------------------

// ------------------------------�˵� - �ļ�------------------------------------
// �ļ� - ��������
void CVICALLBACK menuSave(int menuBar, int menuItem, void *callbackData,
                          int panel)
{
    saveData();
}
// �ļ� - ��ӡ
void CVICALLBACK menuPrint(int menuBar, int menuItem, void *callbackData,
                           int panel)
{
    PrintPanel(panelHandle, "", 1, VAL_FULL_PANEL, 1);
}

// ------------------------------�˵� - ����-------------------------------------
// ����-ƽ��
void CVICALLBACK menuAverage(int menuBar, int menuItem, void *callbackData,
                             int panel)
{
    if (aveStatus == OFF)
        aveStatus = ON;
    else
        aveStatus = OFF;
}
// ����-�л�����ϸɨ��ģʽ
void CVICALLBACK menuOpenAPP(int menuBar, int menuItem, void *callbackData,
                             int panel)
{
    meaStatus = OFF;
    DiscardPanel(panelHandle);
    //Sendbyte(comPort,0x39);    //�л������ʱ�򷢵�ָ��
    CloseCom(comPort);
    CloseCom(comPort);
    DisconnectVNA();
    Delay(0.05);
    QuitUserInterface(0);
    Version = 1;
    return;
}
// ------------------------------�˵� - ����-------------------------------------
// ����-������У��ϵ��
void CVICALLBACK menuCalRefraction(int menuBar, int menuItem, void *callbackData,
                                   int panel)
{
    if (panelStatus[3] == 1)
    {
        return;
    }
    if ((calRefractionPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_REFR)) < 0)
        return;
    DisplayPanel(calRefractionPanelHandle);
    if (Language == ENG)
    {
        SetCtrlAttribute(calRefractionPanelHandle, PANEL_REFR_INPUT_Refraction, ATTR_LABEL_TEXT, "Refractive Index Correction");
        SetCtrlAttribute(calRefractionPanelHandle, PANEL_REFR_BUTTON_OK, ATTR_LABEL_TEXT, "Ok");
    }
    SetCtrlVal(calRefractionPanelHandle, PANEL_REFR_INPUT_Refraction, calRefractionIndex);
    panelStatus[3] = 1;
}

// ����-��ʱ����
void CVICALLBACK menuTimingSet(int menuBar, int menuItem, void *callbackData,
                               int panel)
{
    if (panelStatus[4] == 1)
    {
        return;
    }
    if ((timingMeaPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_TIMM)) < 0)
        return;
    DisplayPanel(timingMeaPanelHandle);
    SetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_INPUT_Delay, meaDelay);
    SetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_INPUT_Point, meaPoints);
    if (Language == ENG)
    {
        SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_INPUT_Delay, ATTR_LABEL_TEXT, "Delay");
        SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_INPUT_Point, ATTR_LABEL_TEXT, "Points");
        SetCtrlAttribute(timingMeaPanelHandle, PANEL_TIMM_BUTTON_OK, ATTR_LABEL_TEXT, "Ok");
        SetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_TEXT_DelayUnit, "s");
    }
    panelStatus[4] = 1;
}

// ------------------------------�˵� - ����-------------------------------------
// ����-����
void CVICALLBACK menuTransCHN(int menuBar, int menuItem, void *callbackData,
                              int panel)
{
    Language = CHN;
    setLanguage(Language, menuBar);
}

// ����-Ӣ��
void CVICALLBACK menuTransENG(int menuBar, int menuItem, void *callbackData,
                              int panel)
{

    Language = ENG;
    setLanguage(Language, menuBar);
}

// ------------------------------�˵� - ����-------------------------------------
// ����-����
void CVICALLBACK menuHelp(int menuBar, int menuItem, void *callbackData,
                          int panel)
{
    system("explorer C:\\ProgramData\\ODM\\ODM�͸߾��ȹ���ʱ�������û������ֲ�.pdf");
}

// ����-����
void CVICALLBACK menuAbout(int menuBar, int menuItem, void *callbackData,
                           int panel)
{
    system("explorer http://www.newkeytech.com/");
}

// ����-������ѡ��
void CVICALLBACK menuDevOpin(int menuBar, int menuItem, void *callbackData,
                             int panel)
{
    if (panelStatus[1] == 1)
    {
        return;
    }
    adminPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_ADM);
    PASSWORDCtrl = PasswordCtrl_ConvertFromString(adminPanelHandle, PANEL_ADM_INPUT_AdminPassword);
    DisplayPanel(adminPanelHandle);
    if (Language == ENG)
    {
        SetCtrlAttribute(adminPanelHandle, PANEL_ADM_INPUT_AdminPassword, ATTR_LABEL_TEXT, "Password: ");
        SetCtrlAttribute(adminPanelHandle, PANEL_ADM_BUTTON_OK, ATTR_LABEL_TEXT, "Ok");
        SetCtrlAttribute(adminPanelHandle, PANEL_ADM_BUTTON_EXIT, ATTR_LABEL_TEXT, "Cancel");
    }
    panelStatus[1] = 1;
    return;
}

// ����-ע��
void CVICALLBACK menuRegister(int menuBar, int menuItem, void *callbackData,
                              int panel)
{
    if (panelStatus[2] == 1)
    {
        return;
    }
    devPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_DEV);

    if (Language == ENG)
    {
        SetCtrlAttribute(devPanelHandle, PANEL_DEV_BUTTON_OK, ATTR_LABEL_TEXT, "Ok");
        SetCtrlVal(devPanelHandle, PANEL_DEV_TEXT_License, "Please Input License");
        SetCtrlVal(devPanelHandle, PANEL_DEV_TEXT_RemainTime, "Remaining Time(days): ");
    }

    // ����ʣʱ��չʾ��ע�����Ľ���
    if (suserMode || adminMode)
    {
        if (Language == ENG)
        {
            SetCtrlVal(devPanelHandle, PANEL_DEV_VALUE_RemainTime, "Infinite");
        }
        else
        {
            SetCtrlVal(devPanelHandle, PANEL_DEV_VALUE_RemainTime, "����");
        }
    }
    else
    {
        char str[6] = {0};
        int restTime = activateDays > usetime ? activateDays - usetime : 0;
        sprintf(str, "%d", restTime);
        SetCtrlVal(devPanelHandle, PANEL_DEV_VALUE_RemainTime, str);
    }

    DisplayPanel(devPanelHandle);
    panelStatus[2] = 1;
    return;
}

//================================================================================
//-----------------------------�����ؼ��ص�����---------------------------------

// �л�������ʽ��ֱͨ����
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
            // ԭ������ֱͨ��Χ�Ƿ����2�������ݲ�ʹ��
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

// ��������
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

// ��ͣ����
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

// ���β���
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

// ��ʱ����
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

// У׼ģʽ
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

// �����ť���������ͼ������
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

// ϵͳ�˻�����ȡ����ť
int CVICALLBACK OneClickAdminEXIT(int panel, int control, int event,
                                  void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
    case EVENT_COMMIT:

        DiscardPanel(adminPanelHandle);
        panelStatus[1] = 0;
        break;
    }
    return 0;
}

// ������У��ϵ�����õ�ȷ����ť
int CVICALLBACK OneClickCalRefractionOK(int panel, int control, int event,
                                        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
    case EVENT_COMMIT:
        GetCtrlVal(calRefractionPanelHandle, PANEL_REFR_INPUT_Refraction, &calRefractionIndex);
        DiscardPanel(calRefractionPanelHandle);
        break;
    }
    panelStatus[3] = 0;
    return 0;
}

// ��ʱ�������õ�ȷ����ťOK
int CVICALLBACK OneClickTimingSetOK(int panTel, int control, int event,
                                    void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
    case EVENT_COMMIT:
        int testMeaPoints;
        double testMeaDelay;
        GetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_INPUT_Delay, &testMeaDelay);
        GetCtrlVal(timingMeaPanelHandle, PANEL_TIMM_INPUT_Point, &testMeaPoints);
        if (testMeaPoints > 100000 || testMeaPoints < 1)
        {
            MessagePopup("Error", "��������Ӧ����Ϊ 1 - 100000");
        }
        else
        {
            meaPoints = testMeaPoints;
        }

        if (testMeaDelay > 600.00 || testMeaDelay < 0.10)
        {
            MessagePopup("Error", "�������Ӧ����Ϊ 0.10 - 600s");
        }
        else
        {
            meaDelay = testMeaDelay;
        }
        DiscardPanel(timingMeaPanelHandle);
        break;
    }
    panelStatus[4] = 0;
    return 0;
}

// ϵͳ�˻�ȷ����ť
int CVICALLBACK OneClickAdmOK(int panel, int control, int event,
                              void *callbackData, int eventData1, int eventData2)
{
    char myPassword[11] = "Hello,ODM!";
    char password[20] = {0};
    int y = 1;
    switch (event)
    {
    case EVENT_COMMIT:
        GetCtrlVal(adminPanelHandle, PANEL_ADM_INPUT_AdminPassword, password);
        if ((y = strncmp(password, myPassword, 19)) != 0)
        {
            MessagePopup("Error", "�������");
            SetCtrlVal(adminPanelHandle, PANEL_ADM_INPUT_AdminPassword, "");
        }
        else
        {
            userMode = ON;
            adminMode = ON;
            DiscardPanel(adminPanelHandle);
            panelStatus[1] = 0;
        }
        break;
    }

    return 0;
}

// ע�����ȷ����ť
int CVICALLBACK OneClickDevOK(int panel, int control, int event,
                              void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
    case EVENT_COMMIT:

        if (suserMode == ON)
        {
            MessagePopup("Tip", "���ѻ���������");
            DiscardPanel(devPanelHandle);
            panelStatus[2] = 0;
            break;
        }

        // �����ؼ����ݲ�����ʽУ��
        char lastLicense[4][5] = {0};
        int licenseElement[4] = {PANEL_DEV_INPUT_License_1, PANEL_DEV_INPUT_License_2, PANEL_DEV_INPUT_License_3, PANEL_DEV_INPUT_License_4};
        for (int i = 0; i < 4; i++)
        {
            GetCtrlVal(devPanelHandle, licenseElement[i], license[i]);
            if (strlen(license[i]) != 4)
            {
                MessagePopup("Error", "Invalid License");
                return -1;
            }
            for (int j = 0; j < 4; j++)
            {
                if (license[i][j] < 65 || license[i][j] > 90)
                {
                    MessagePopup("Error", "Invalid License");
                    return -1;
                }
            }
        }

        // ������к��Ƿ����ڵ�ǰ ODM�����������ں���Կ���
        int date, licNum, lastLicNum;

        // ��ȡ�������к�
        if (!noComPort && GetInstrCom(&comPort, ODM_Name) != 0 && userMode == OFF)
        {
            MessagePopup("Error", "�豸���Ӵ���");
            return -1;
        }

        if (checkLicense(license, ODM_Name, &date, &licNum) != 0)
        {
            return -1;
        }

        // ��ȡ���м����룬���б�ŶԱȣ����ֻ�ܵ���
        // �������ڣ���ֱ�Ӵ���
        FILE *fp;
        fp = fopen("C:\\ProgramData\\ODM\\lic.dll", "rb");
        if (fp)
        {
            for (int i = 0; i < 4; i++)
            {
                fscanf(fp, "%s\n", lastLicense[i]);
            }
            fclose(fp);

            if (checkLicense(lastLicense, ODM_Name, &activateDays, &lastLicNum) != 0)
            {
                return -1;
            }

            if (licNum <= lastLicNum)
            {
                MessagePopup("Error", "Invalid License");
                return -1;
            }

            // �����ɹ������µļ�����д�� lic �ļ������� prob �ļ�
            fp = fopen("C:\\ProgramData\\ODM\\lic.dll", "wb");
            for (int i = 0; i < 4; i++)
            {
                fprintf(fp, "%s\n", license[i]);
            }

            fprintf(fp, "%s\n", ODM_Name);
            fclose(fp);

            time_t timep;
            struct tm *p;
            time(&timep);
            p = localtime(&timep);
            updateUseTime();
            fp = fopen("C:\\ProgramData\\ODM\\prob.dll", "wb");
            fprintf(fp, "%d\n", usetime > activateDays ? 0 : usetime - activateDays);
            fprintf(fp, "%d\n", 1900 + p->tm_year);
            fprintf(fp, "%d\n", day);
            fprintf(fp, "%d\n", sec);
            fclose(fp);
        }
        else
        {
            // �����ɹ������µļ�����д�� lic �ļ������� prob �ļ�
            fp = fopen("C:\\ProgramData\\ODM\\lic.dll", "wb");
            for (int i = 0; i < 4; i++)
            {
                fprintf(fp, "%s\n", license[i]);
            }
            fprintf(fp, "%s\n", ODM_Name);
            fclose(fp);
        }

        activateDays = date;
        if (activateDays == 0)
        {
            suserMode = ON;
        }
        else
        {
            userMode = ON;
        }

        DiscardPanel(devPanelHandle);
        panelStatus[2] = 0;

        fp = fopen("C:\\ProgramData\\ODM\\licId.dll", "wb");
        fprintf(fp, "%d\n", licNum);
        fclose(fp);

        SetFileAttributes("C:\\ProgramData\\ODM\\licId.dll", FILE_ATTRIBUTE_HIDDEN);
        SetFileAttributes("C:\\ProgramData\\ODM\\lic.dll", FILE_ATTRIBUTE_HIDDEN);
        if (panelStatus[0] == 0 || usetime > activateDays || usetime < last_day || (usetime == last_day && (sec + 60) < last_sec))
        {
            QuitUserInterface(0);
        }
    }
    return 0;
}

// �޸ĳ��ȵ�λ������
int CVICALLBACK OneClickLength(int panel, int control, int event,
                               void *callbackData, int eventData1, int eventData2)
{
    int ring_length_unit;
    double M1, M2, M3;
    switch (event)
    {
    case EVENT_COMMIT:
        GetCtrlIndex(panelHandle, PANEL_RING_Length, &ring_length_unit);
        GetCtrlVal(panelHandle, PANEL_VALUE_Length_M1, &M1);
        GetCtrlVal(panelHandle, PANEL_VALUE_Length_M2, &M2);
        GetCtrlVal(panelHandle, PANEL_VALUE_Length_M3, &M3);

        M1 = setPrecision(0, M1);
        M2 = setPrecision(0, M2);
        M3 = setPrecision(0, M3);

        if (M1 > meaDistance[limitLengthIndex - 1])
        {
            M1 = meaDistance[limitLengthIndex - 1];
            M3 = meaDistance[limitLengthIndex - 1];
        }

        M1 = setUnitOfLength(ring_length_unit, M1, 1);
        M2 = setUnitOfLength(ring_length_unit, M2, 1);
        M3 = setUnitOfLength(ring_length_unit, M3, 1);

        setPrecisionOfPanel(ring_length_unit);

        SetCtrlVal(panelHandle, PANEL_VALUE_Length_M1S, M1);
        SetCtrlVal(panelHandle, PANEL_VALUE_Length_M2S, M2);
        SetCtrlVal(panelHandle, PANEL_VALUE_Length_M3S, M3);
        break;
    }
    return 0;
}

// �޸Ĳ�����Χ
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
        // ���Ʋ�����Χ��ʹ��Ӧ������ʾ��Ϊ��ɫ
        if (ring_range_value > maxLengthIndex && adminMode == OFF)
        {
            SetCtrlAttribute(panelHandle, PANEL_RING_Range, ATTR_TEXT_COLOR, VAL_GRAY);
        }
        else
        {
            limitLengthIndex = ring_range_value;
            SetCtrlAttribute(panelHandle, PANEL_RING_Range, ATTR_TEXT_COLOR, 0x003399FF);
        }
        break;
    }
    return 0;
}

//================================thread function=========================================//
//===================================�����߳�==============================================//

static int CVICALLBACK ThreadFunction(void *functionData)
{
    // ----------------------�����豸������ʼ�����ڽ����Ͻ�����ʾ--------------------------
    if (setDevice() != 0)
    {
        return -1;
    }

    if (setVNA() != 0)
    {
        return -1;
    }

    // �豸��ʼ��
    Delay(0.01);
    Sendbyte(comPort, 0x44); //�ؾ����
    Delay(0.05);
    Sendbyte(comPort, 0x39); //��1310��Դ
    Delay(0.05);
    Sendbyte(comPort, 0x31); //ѡ��1550ͨ��
    Delay(0.05);
    Sendbyte(comPort, 0x32); //ѡ��ֱͨģʽ
    Delay(0.2);              //���һ��
    Sendbyte(comPort, 0x36); //����1550��Դ
    Delay(0.01);
    Sendbyte(comPort, 0x41);
    Delay(0.01); //����VNA����  **************************Ĭ��VNA����״̬

    initStatus = OFF;
    // ----------------------------------��ʼ������-------------------------------------
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
    int DeviceCheckNum = 5; // ���������豸��Ϣ��������

    double targetLength = 0.0;
    char temp[10] = {0};
    char IL[10] = {0};
    char delta_temp[10] = {0};
    double tempVal = 0.0, calTempVal = 0.0, ILVal = 0.0;
    double range = 0;

    // ƽ��ֵ����ʱ�������������
    double sortAmp[TraceLen][aveFactor];
    double sortPhi[TraceLen][aveFactor];

    GetInfoVNA(&TraceLen, &SweepTime);
    SetCtrlAttribute(panelHandle, PANEL_ProgressBar, ATTR_MIN_VALUE, 0.0);
    SetCtrlAttribute(panelHandle, PANEL_ProgressBar, ATTR_MAX_VALUE, SweepTime);

    double *freq = (double *)malloc(TraceLen * sizeof(double));
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

    // ---------------------------ѭ������---------------------------------
    while (meaStatus != OFF)
    {
        // ÿ20��ѭ�����һ�鴮�ں� VNA ���������
        if (!noComPort && ++RS232CheckNum && RS232CheckNum % 20 == 0)
        {
            RS232CheckNum = 0;
            // ��鴮���Ƿ�����
            RS232Error = OpenComConfig(comPort, "", 9600, 0, 8, 1, 1, 4);
            if (RS232Error != 0)
            {
                //MessagePopup("Error", "���ڶ�ʧ����������!");
                if (setDevice() != 0)
                {
                    return -1;
                }
            }

            // ������ģ���Ƿ�����
            RS232Error = GetReadyStateVNA();

            if (RS232Error == 0)
            {
                //MessagePopup("Error", "ϵͳ���Ӵ��󣬳�������!");
                if (setVNA() != 0)
                {
                    return -1;
                }
            }
        }

        // ---------------------�û�ʹ��ʱ����--------------------------
        if (suserMode == OFF && adminMode == OFF)
        {
            time_t timep;
            struct tm *p;
            time(&timep);
            p = localtime(&timep);
            FILE *fp;
            updateUseTime();
            if (usetime < last_day || (usetime == last_day && (sec + 60) < last_sec)) //usetime must be increased
            {
                MessagePopup("Error", "system time has been modified!");
                return -1;
            }

            if (usetime > activateDays)
            {
                QuitUserInterface(0);
                MessagePopup("Error", "Probation period is over and the program will be closed.\nPlease log in with your account next time");
                return 0;
            }

            fp = fopen("C:\\ProgramData\\ODM\\prob.dll", "wb");
            fprintf(fp, "%d\n", usetime);
            fprintf(fp, "%d\n", 1900 + p->tm_year);
            fprintf(fp, "%d\n", day);
            fprintf(fp, "%d\n", sec);
            fclose(fp);
        }

        // -------------------------�����û���������-------------------------------
        GetCtrlVal(panelHandle, PANEL_RING_Wavelength, &ring_wl_value);
        GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);

        // testRange���м���������ڼ�����̷�Χ�Ƿ��ܵ�����
        int testRange;
        GetCtrlVal(panelHandle, PANEL_RING_Range, &testRange);
        if (testRange <= maxLengthIndex || adminMode == ON)
        {
            GetCtrlVal(panelHandle, PANEL_RING_Range, &ring_range_value);
        }
        GetCtrlVal(panelHandle, PANEL_INPUT_Refraction, &refraction_index);
        GetCtrlVal(panelHandle, PANEL_VALUE_Length_M2S, &M2);
        GetCtrlIndex(panelHandle, PANEL_RING_Length, &ring_length_unit);
        M2 = setPrecision(ring_length_unit, M2);
        targetLength = setUnitOfLength(ring_length_unit, M2, 0);
        SetCtrlVal(panelHandle, PANEL_VALUE_Length_M2, targetLength);

        // -----------------------�����豸�¶������------------------------------
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
            SetCtrlVal(panelHandle, PANEL_VALUE_IL, IL);
        }
        else if (DeviceCheckNum > 5)
        {
            DeviceCheckNum = 0;
            ReadTemp(comPort, temp);
            tempVal = atof(temp);
            calTempVal = atof(calTemp);
            sprintf(temp, "%.1f", tempVal);
            SetCtrlVal(panelHandle, PANEL_VALUE_Temp, temp);
            sprintf(delta_temp, "%.1f", tempVal - calTempVal);
            SetCtrlVal(panelHandle, PANEL_VALUE_TempDiffer, delta_temp);
            if (tempVal - calTempVal > 0 && calStatus == ON)
            {
                SetCtrlAttribute(panelHandle, PANEL_VALUE_TempDiffer, ATTR_TEXT_COLOR, VAL_RED);
                sprintf(delta_temp, "+%.1f", tempVal - calTempVal);
            }
            else if (tempVal - calTempVal < 0 && calStatus == ON)
            {
                SetCtrlAttribute(panelHandle, PANEL_VALUE_TempDiffer, ATTR_TEXT_COLOR, VAL_GREEN);
                sprintf(delta_temp, "%.1f", tempVal - calTempVal);
            }
            else
            {
                SetCtrlAttribute(panelHandle, PANEL_VALUE_TempDiffer, ATTR_TEXT_COLOR, VAL_PANEL_GRAY);
                sprintf(delta_temp, "%.1f", tempVal - calTempVal);
            }
            SetCtrlVal(panelHandle, PANEL_VALUE_TempDiffer, delta_temp);
        }
        else
        {
            ++DeviceCheckNum;
        }

        //------------------------���ò���-----------------------
        switch (ring_wl_value)
        {
        case 1:
            if (LDstatus != WL1550)
            {
                SetOS1(comPort, WL1550);
                Sendbyte(comPort, 0x39);
                Delay(0.001);
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
                Delay(0.001);
                Sendbyte(comPort, 0x38);
                Delay(0.001);
                LDstatus = WL1310;
            }
            break;
        }

        // -----------------------����ֱͨ/����ģʽ---------------------
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

        //------------------------У׼ģʽ����----------------------------
        switch (calStatus)
        {
        case ON:
            GetCtrlVal(panelHandle, PANEL_VALUE_Calibration, calText);
            if (strcmp(TextON, calText) != 0)
            {
                TrigVNA();
                ReadData(TraceLen, freq, amp, phi);
                calMag = amp[TraceLen - 1];
                CalVNA(ON);
                SetCtrlVal(panelHandle, PANEL_VALUE_Calibration, TextON);
                GetCtrlVal(panelHandle, PANEL_VALUE_TempDiffer, calTemp);
                printf("%s\n", calTemp);
            }
            break;
        case OFF:
            GetCtrlVal(panelHandle, PANEL_VALUE_Calibration, calText);
            if (strcmp(TextOFF, calText) != 0)
            {
                calMag = 0;
                CalVNA(OFF);
                SetCtrlVal(panelHandle, PANEL_VALUE_Calibration, TextOFF);
                calTemp[0] = '0';
                calTemp[1] = '0';
                calTemp[2] = '.';
                calTemp[3] = '0';
            }
            break;
        }

        // ------------------------�Ƿ�Ϊƽ������---------------------------
        if (aveStatus == ON)
        {
            for (int i = 0; i < aveFactor; i++)
            {
                TrigVNA();
                for (double t = 0; t < SweepTime; t = t + StepTime)
                {
                    SetCtrlVal(panelHandle, PANEL_ProgressBar, t);
                    Delay(StepTime * 1);
                }
                ReadData(TraceLen, freq, amp, phi);
                for (int j = 0; j < TraceLen; j++)
                {
                    sortAmp[j][i] = amp[j];
                    sortPhi[j][i] = phi[j];
                }
                sprintf(aveText, "%d", i + 1);
                SetCtrlVal(panelHandle, PANEL_VALUE_Average, aveText);
            }

            double t;     //�����н�
            int flag = 0; //˳���־
            for (int j = 0; j < TraceLen; j++)
            {
                for (int k = 0; k < aveFactor - 1; k++)
                {
                    flag = 0;
                    //��������
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
                    if (flag == 0) //���������Ѿ����
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
            for (int i = 2; i < aveFactor - 3; i++) //ȥ��2����С��3�����
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
            SetCtrlVal(panelHandle, PANEL_VALUE_Average, aveText);
            TrigVNA();
            for (double i = 0; i < SweepTime; i = i + StepTime)
            {
                SetCtrlVal(panelHandle, PANEL_ProgressBar, i);
                Delay(StepTime * 1);
            }
            ReadData(TraceLen, freq, amp, phi);
        }

        // ------------------------���㳤�ȼ���ʱ���Ա�׼��λ�洢�����صĿؼ���-------------------------------
        FiberDetector(freq, amp, phi, refraction_index * calRefractionIndex, calMag, &TimeDelay, &FiberLength, &MeanAmp);
        checkPower(comPort, amp[6] + calMag);
        TimeDelay = TimeDelay / through_or_reflect;
        FiberLength = FiberLength / through_or_reflect;
        SetCtrlVal(panelHandle, PANEL_VALUE_Length_M1, FiberLength);
        SetCtrlVal(panelHandle, PANEL_VALUE_Length_M3, targetLength - FiberLength);
        GetCtrlVal(panelHandle, PANEL_VALUE_Length_M1, &M1);
        GetCtrlVal(panelHandle, PANEL_VALUE_Length_M3, &M3);

        // ---------------------------�������̡���λ�����ȶԲ���ֵ����ת������ʾ--------------------------------
        if (M1 > meaDistance[limitLengthIndex - 1])
        {
            M1 = meaDistance[limitLengthIndex - 1];
            M3 = meaDistance[limitLengthIndex - 1];
            TimeDelay = 0;
            targetLength = 0;
        }
        if (M1 < -meaDistance[limitLengthIndex - 1])
        {
            M1 = -meaDistance[limitLengthIndex - 1];
            M3 = -meaDistance[limitLengthIndex - 1];
            TimeDelay = 0;
            targetLength = 0;
        }

        GetCtrlIndex(panelHandle, PANEL_RING_Length, &ring_length_unit);

        M1 = setPrecision(0, M1);
        M3 = setPrecision(0, M3);

        M1 = setUnitOfLength(ring_length_unit, M1, 1);
        M3 = setUnitOfLength(ring_length_unit, M3, 1);

        SetCtrlVal(panelHandle, PANEL_VALUE_Length_M1S, M1);
        SetCtrlVal(panelHandle, PANEL_VALUE_Length_M3S, M3);
        SetCtrlVal(panelHandle, PANEL_VALUE_Delay_M1, TimeDelay);
        SetCtrlVal(panelHandle, PANEL_VALUE_Delay_M2, targetLength / c0);
        SetCtrlVal(panelHandle, PANEL_VALUE_Delay_M3, targetLength / c0 - TimeDelay);

        // ------------------------���ݲ���ģʽ���Ļ�ͼ��ʽ---------------------------
        switch (sweepType)
        {
        case contSweepType:
            plotResponse(contSweepType, MeanAmp, FiberLength, range);
            break;
        case singleSweepType:
            batchData[subscript][0] = FiberLength;
            batchData[subscript][1] = TimeDelay;
            ILData[subscript] = atof(IL);
            if (subscript > 98)
                subscript = subscript - 99;
            else
                subscript = subscript + 1;
            plotResponse(singleSweepType, MeanAmp, FiberLength, range);
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
            plotResponse(timingSweepType, MeanAmp, FiberLength, range);
            Delay(meaDelay - 0.1); // �������������ɨ��ѭ��ʱ��
            break;
        case holdSweepType:
            break;
        default:
            plotResponse(contSweepType, MeanAmp, FiberLength, range);
            break;
        }
    }
    free(freq);
    free(amp);
    free(phi);
    noComPort || CloseCom(comPort);
    return 0;
}

//--------------------------�Զ��庯��------------------------//
// �˵�����������
void saveData(void)
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

// ���ڱ���
void displayRS232Error(int RS232Error)
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

// ������������ͼ��ĺ�����
void dispXstopMsg(void)
{
    int ring_mode_value = -1,
        ring_range_value = -1;
    GetCtrlVal(panelHandle, PANEL_RING_Mode, &ring_mode_value);

    // �������̷�Χ
    int testRange;
    GetCtrlVal(panelHandle, PANEL_RING_Range, &testRange);
    if (testRange <= maxLengthIndex || adminMode == ON)
    {
        GetCtrlVal(panelHandle, PANEL_RING_Range, &ring_range_value);
    }

    switch (ring_range_value)
    {
    case 1:
        switch (ring_mode_value)
        {
        case 1:
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
    }
}

// �������ͼ
void plotResponse(int sweep_type, double mean_amp, double fiber_length, double range)
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
        dispXstopMsg();
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
        PlotStripChartPoint(panelHandle, PANEL_STRIPCHART, fabs(fiber_length) <= fabs(meaDistance[limitLengthIndex - 1]) ? fiber_length : meaDistance[limitLengthIndex - 1]);
        GetAxisRange(panelHandle, PANEL_STRIPCHART, &Xmode, &Xmin, &Xmax, &Ymode, &Ymin, &Ymax);
        break;
    case timingSweepType:
        PlotStripChartPoint(panelHandle, PANEL_STRIPCHART, fabs(fiber_length) <= fabs(meaDistance[limitLengthIndex - 1]) ? fiber_length : meaDistance[limitLengthIndex - 1]);
        GetAxisRange(panelHandle, PANEL_STRIPCHART, &Xmode, &Xmin, &Xmax, &Ymode, &Ymin, &Ymax);
        break;
    }
}

// ���ݲ�ͬ��λ���þ��ȣ������ڲ�����
double setPrecision(int choice, double value)
{
    long long t;
    double x;
    switch (choice)
    {
    case 0:
        if (value >= 0)
            t = (long long)(value * 10e4 + 0.5);
        else
            t = (long long)(value * 10e4 - 0.5);
        x = t;
        value = x / 10e4;
        break;
    case 1:
        if (value >= 0)
            t = (long long)(value * 10e7 + 0.5);
        else
            t = (long long)(value * 10e7 - 0.5);
        x = t;
        value = x / 10e7;
        break;
    case 2:
        if (value >= 0)
            t = (long long)(value * 100 + 0.5);
        else
            t = (long long)(value * 100 - 0.5);
        x = t;
        value = x / 100;
        break;
    }
    return value;
}

// ��ͬ��λ��ĳ���ת��
double setUnitOfLength(int choice, double value, int toggle)
{

    switch (toggle)
    {
    case 0:
        switch (choice)
        {
        case 0:
            value = value;
            break;
        case 1:
            value = value * 1000.0;
            break;
        case 2:
            value = value / 1000.0;
            break;
        }
        break;
    case 1:
        switch (choice)
        {
        case 0:
            value = value;
            break;
        case 1:
            value = value / 1000.0;
            break;
        case 2:
            value = value * 1000.0;
            break;
        }
        break;
    }

    return value;
}

// �޸Ľ������ݵ���ʾ����
void setPrecisionOfPanel(int choice)
{
    switch (choice)
    {
    case 0:
        SetCtrlAttribute(panelHandle, PANEL_VALUE_Length_M1S, ATTR_PRECISION, 5);
        SetCtrlAttribute(panelHandle, PANEL_VALUE_Length_M2S, ATTR_PRECISION, 5);
        SetCtrlAttribute(panelHandle, PANEL_VALUE_Length_M3S, ATTR_PRECISION, 5);
        break;
    case 1:
        SetCtrlAttribute(panelHandle, PANEL_VALUE_Length_M1S, ATTR_PRECISION, 7);
        SetCtrlAttribute(panelHandle, PANEL_VALUE_Length_M2S, ATTR_PRECISION, 7);
        SetCtrlAttribute(panelHandle, PANEL_VALUE_Length_M3S, ATTR_PRECISION, 7);
        break;
    case 2:
        SetCtrlAttribute(panelHandle, PANEL_VALUE_Length_M1S, ATTR_PRECISION, 2);
        SetCtrlAttribute(panelHandle, PANEL_VALUE_Length_M2S, ATTR_PRECISION, 2);
        SetCtrlAttribute(panelHandle, PANEL_VALUE_Length_M3S, ATTR_PRECISION, 2);
        break;
    }
}

// ���ʹ��ͼ��
void checkPower(int comPort, double mean_amp)
{
    int sendByte, cautionStatus;
    if (mean_amp / 2 <= -25)
    {
        cautionStatus = ON;
        SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);
        sendByte = 0x43;
        Delay(0.005);
    }
    else
    {
        cautionStatus = OFF;
        SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_BLACK);
        sendByte = 0x44;
        Delay(0.005);
    }
    if (powerWarn != cautionStatus)
    {
        powerWarn = cautionStatus;
        Sendbyte(comPort, sendByte);
    }
}

// ����ʹ��ʱ�估��ز���
void updateUseTime()
{
    FILE *fp;
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    fp = fopen("C:\\ProgramData\\ODM\\prob.dll", "rb");
    fscanf(fp, "%d\n", &usetime);
    fscanf(fp, "%d\n", &year);
    fscanf(fp, "%d\n", &day);
    fscanf(fp, "%d\n", &sec);
    fclose(fp);

    int initDay;
    initDay = day - usetime;
    last_day = usetime;
    last_sec = sec;
    day = p->tm_yday;
    usetime = day - initDay + 365 * (1900 + p->tm_year - year);
    hour = p->tm_hour;
    min = p->tm_min;
    sec = hour * 3600 + min * 60 + p->tm_sec;
}

// ��鲢�������֤�����������ں����
int checkLicense(char license[4][5], char ODM_Name[19], int *date, int *licNum)
{
    // ������·�����
    char id[7] = {0};
    for (int i = 0; i < 3; i++)
    {
        id[2 * i] = license[i][0] - license[i][1] - 1 + '0';
        id[2 * i + 1] = license[i][2] - license[i][3] - 1 + '0';
    }
    // ��ȡ������·�����
    char localId[7] = {0};

    noComPort &&strcpy(ODM_Name, "ODM-S-D1000-190808"); // �޸�ODM_Name,������

    for (int i = 0; i < 6; i++)
    {
        localId[i] = ODM_Name[i + 12];
    }

    // �ȶ����к�
    if (strncmp(id, localId, 6) != 0)
    {
        MessagePopup("Error", "Invalid License");
        return -1;
    }

    // ���к���ȷ�����������ر�ź�����
    *licNum = (license[3][2] - 65 - license[0][1] % 10) * 16 + (license[3][3] - 65 - license[0][3] % 10);
    *date = (license[3][0] - 65 - license[1][1] % 6) * 20 + (license[3][1] - 65 - license[1][3] % 6);

    return 0;
}

// �޸����Է���
void setLanguage(int Language, int menuBar)
{
    int height;
    GetCtrlAttribute(panelHandle, PANEL_BUTTON_Continuous, ATTR_HEIGHT, &height);
    int textSize;
    if (Language == ENG)
    {
        textSize = 15 * height / 39;
    }
    else
    {
        textSize = 17 * height / 39;
    }

    // �����ģ��
    int mainPanelInput[] = {PANEL_BUTTON_Continuous, PANEL_BUTTON_Pause, PANEL_BUTTON_Single, PANEL_BUTTON_Timing, PANEL_BUTTON_Clear, PANEL_BUTTON_Calibration};
    int mainPanelText[] = {PANEL_TEXT_GraphTitle, PANEL_TEXT_Wavelength, PANEL_TEXT_Mode, PANEL_TEXT_Range, PANEL_TEXT_Refraction,
                           PANEL_TEXT_Marker, PANEL_TEXT_Delay, PANEL_TEXT_Calibration, PANEL_TEXT_Average, PANEL_TEXT_TempDiffer, PANEL_TEXT_Caution};
    int mainPanelRing[][2] = {{PANEL_RING_Mode, 2}, {PANEL_RING_Length, 3}};
    int menu[] = {MENUBAR_File, MENUBAR_Function, MENUBAR_Set, MENUBAR_Language, MENUBAR_Other};
    int menuList[] = {MENUBAR_File_SaveData, MENUBAR_File_Print, MENUBAR_Function_Average, MENUBAR_Function_SwitchVersion, MENUBAR_Set_SetRefraction, MENUBAR_Set_SetInterval,
                      MENUBAR_Language_Chinese, MENUBAR_Language_English, MENUBAR_Other_Help, MENUBAR_Other_About, MENUBAR_Other_Admin, MENUBAR_Other_Register};
    // ע�����
    int devPanelInput[] = {PANEL_DEV_BUTTON_OK};
    int devPanelText[] = {PANEL_DEV_TEXT_License, PANEL_DEV_TEXT_RemainTime, PANEL_DEV_VALUE_RemainTime};
    // �����������
    int timingMeaPanelInput[] = {PANEL_TIMM_BUTTON_OK, PANEL_TIMM_INPUT_Delay, PANEL_TIMM_INPUT_Point};
    int timingMeaPanelText[] = {PANEL_TIMM_TEXT_DelayUnit};
    // ����Ա���
    int adminPanelInput[] = {PANEL_ADM_BUTTON_OK, PANEL_ADM_BUTTON_EXIT, PANEL_ADM_INPUT_AdminPassword};
    // ������ϵ�����
    int calRefractionPanelInput[] = {PANEL_REFR_INPUT_Refraction, PANEL_REFR_BUTTON_OK};

    // ��Ӧ�ķ���
    char *mainPanelInputInfo[2][6] = {{"��������", "��ͣ����", "���β���", "��ʱ����", "���", "У׼"},
                                      {"Continuous", "Pause", "Single", "Timing", "Clear", "Calibration"}};
    char *mainPandelTextInfo[2][11] = {{"��ʱ����Ϣͼ", "���� (nm)", "������ʽ", "��Χ", "������ϵ��",
                                        "���", "ʱ�� (ns)", "У׼", "ƽ��", "�²�", "���չ⹦�ʵͣ�"},
                                       {"Optical Delay Information", "WaveLength(nm)", "Mode", "Range", "Refractive Index",
                                        "Marker", "Delay(ns)", "Cal", "Ave", "TempDiffer", "Low Power !"}};
    char *mainPanelRing_Mode[2][2] = {{"ֱͨ", "����"}, {"Through", "Reflect"}};
    char *mainPanelRing_Length[2][3] = {{"���� (m)", "���� (km)", "���� (mm)"}, {"Length(m)", "Length(km)", "Length(mm)"}};
    char **mainPanelRingInfo[2] = {mainPanelRing_Mode[Language], mainPanelRing_Length[Language]};

    char *menuInfo[2][5] = {{"�ļ�", "����", "����", "����", "����"}, {"File", "Tool", "Options", "Language", "Other"}};
    char *menuListInfo[2][12] = {{"��������", "��ӡ", "ƽ��", "�л�����ϸɨ��ģʽ", "������У��ϵ��",
                                  "��ʱ����", "����", "Ӣ��", "����", "����", "������ѡ��", "ע��"},
                                 {"Save Data", "Print", "Average", "Back to ODM_v1", "Refractive Index Correction",
                                  "Timing Setting", "Chinese", "English", "Help", "About", "Developer options", "Register"}};
    char *devPanelInputInfo[2][2] = {{"ȷ��"}, {"OK"}};
    char *devPanelTextInfo[2][3] = {{"�����뼤����", "ʣ��ʹ��ʱ��(��)��", "����"},
                                    {"Please Input License", "Remaining Time(days): ", "Infinite"}};
    char *timingMeaPanelInputInfo[2][3] = {{"ȷ��", "�����ӳ�", "��������"}, {"OK", "Delay", "Points"}};
    char *timingMeaPanelTextInfo[2][1] = {{"��"}, {"s"}};
    char *adminPanelInputInfo[2][3] = {{"ȷ��", "ȡ��", "����"}, {"OK", "Cancel", "Password"}};
    char *calRefractionPanelInputInfo[2][2] = {{"������У��ϵ��", "ȷ��"}, {"Refractive Index Correction", "OK"}};

    for (int i = 0; i < 6; i++)
    {
        SetCtrlAttribute(panelHandle, mainPanelInput[i], ATTR_LABEL_POINT_SIZE, textSize);
        SetCtrlAttribute(panelHandle, mainPanelInput[i], ATTR_LABEL_TEXT, mainPanelInputInfo[Language][i]);
    }
    for (int i = 0; i < 11; i++)
    {
        SetCtrlVal(panelHandle, mainPanelText[i], mainPandelTextInfo[Language][i]);
    }

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < mainPanelRing[i][1]; j++)
        {
            ReplaceListItem(panelHandle, mainPanelRing[i][0], j, mainPanelRingInfo[i][j], j + 1);
        }
    }

    for (int i = 0; i < 5; i++)
    {
        SetMenuBarAttribute(menuBar, menu[i], ATTR_MENU_NAME, menuInfo[Language][i]);
    }
    for (int i = 0; i < 12; i++)
    {
        SetMenuBarAttribute(menuBar, menuList[i], ATTR_ITEM_NAME, menuListInfo[Language][i]);
    }
    for (int i = 0; i < 1; i++)
    {
        SetCtrlAttribute(devPanelHandle, devPanelInput[i], ATTR_LABEL_TEXT, devPanelInputInfo[Language][i]);
    }
    for (int i = 0; i < 2; i++)
    {
        SetCtrlVal(devPanelHandle, devPanelText[i], devPanelTextInfo[Language][i]);
    }
    // ����Ȩ�󣬽����ÿ���չʾ��ע�����Ľ���
    if (suserMode || adminMode)
    {
        SetCtrlVal(devPanelHandle, devPanelText[2], devPanelTextInfo[Language][2]);
    }

    for (int i = 0; i < 3; i++)
    {
        SetCtrlAttribute(timingMeaPanelHandle, timingMeaPanelInput[i], ATTR_LABEL_TEXT, timingMeaPanelInputInfo[Language][i]);
    }
    for (int i = 0; i < 1; i++)
    {
        SetCtrlVal(timingMeaPanelHandle, timingMeaPanelText[i], timingMeaPanelTextInfo[Language][i]);
    }

    for (int i = 0; i < 3; i++)
    {
        SetCtrlAttribute(adminPanelHandle, adminPanelInput[i], ATTR_LABEL_TEXT, adminPanelInputInfo[Language][i]);
    }
    for (int i = 0; i < 2; i++)
    {
        SetCtrlAttribute(calRefractionPanelHandle, calRefractionPanelInput[i], ATTR_LABEL_TEXT, calRefractionPanelInputInfo[Language][i]);
    }
}

// ��ʼ�����Ӵ���
int setDevice()
{
    SetCtrlVal(panelHandle, PANEL_TEXT_Caution, "ϵͳ���ڳ�ʼ��");
    SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);

    // ��Ⲣ�������Ӵ��ڣ���� 10 ��
    if (!noComPort)
    {
        for (int i = 0; i < 20; i++)
        {
            if (i % 2 == 0)
            {
                SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);
            }
            else
            {
                SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_BLACK);
            }

            if (GetInstrCom(&comPort, ODM_Name) == 0)
            {
                break;
            }
            else if (i == 19)
            {
                MessagePopup("Error", "�豸���ӳ�ʱ��");
                return -1;
            }

            Delay(0.5);
        }
    }

    if (checkLicense(license, ODM_Name, &activateDays, &licNum) != 0)
    {
        MessagePopup("Error", "�豸δ�����Ȩ");
        devPanelHandle = LoadPanel(0, "OFDA_V6.uir", PANEL_DEV);
        DisplayPanel(devPanelHandle);
        panelStatus[2] = 1;
        return -1;
    }

    SetCtrlVal(panelHandle, PANEL_TEXT_Caution, "���չ⹦�ʵͣ�");
    SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_BLACK);
    return 0;
}

// ���� VNA
int setVNA()
{
    SetCtrlVal(panelHandle, PANEL_TEXT_Caution, "ϵͳ���ڳ�ʼ��");
    SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);
    // ��� VNA ����
    if (ConnectVNA() != 0)
    {
        MessagePopup("Error", "ϵͳ���Ӵ���");
        return -1;
    }

    // ��ȡ VNA Ready ״̬��Ready ��ɺ��ٽ�����һ�����úͲ���
    for (int i = 0; i < 24; i++)
    {
        if (i % 2 == 0)
        {
            SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_YELLOW);
        }
        else
        {
            SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_BLACK);
        }

        if (GetReadyStateVNA() != 0)
        {
            break;
        }
        else if (i == 23)
        {
            MessagePopup("Error", "ϵͳ���ӳ�ʱ��");
            return -1;
        }

        Delay(0.5);
    }

    if (IniVNA() != 0)
    {
        MessagePopup("Error", "ϵͳ��ʼ������");
        return -1;
    }

    SetCtrlVal(panelHandle, PANEL_TEXT_Caution, "���չ⹦�ʵͣ�");
    SetCtrlAttribute(panelHandle, PANEL_TEXT_Caution, ATTR_TEXT_COLOR, VAL_BLACK);

    return 0;
}
