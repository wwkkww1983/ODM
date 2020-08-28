//==============================================================================
//
// Title:		Initialize.c
// Purpose:		A short description of the implementation.
//
// Created on:	2018/3/22 at 15:27:57 by lishupeng.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include "Initialize.h"
//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

BOOL InitializeS2VNA(void)
{
    DWORD S2VNA_process_id = 0;
    if (!FindProcess(&S2VNA_process_id))
        OpenS2VNA();
    //SetupS2VNA();
    return TRUE;
}

BOOL OpenS2VNA(void)
{
    char szCommandLine[] = "C:\\VNA\\S2VNA\\S2VNA.exe";
    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW; //指定wShowWindow成员有效
    si.wShowWindow = SW_HIDE;          //隐藏window
    BOOL bRet = CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (!bRet)
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return FALSE;
    }
    /*--------------隐藏弹出的Logo---------------*/
    HWND intro = 0;
    do
    {
        intro = FindWindow(NULL, "Intro");
    } while (intro == NULL);
    MoveWindow(intro, 0, 0, 0, 0, FALSE);
    ShowWindow(intro, SW_HIDE);

    /*-------释放不用的句柄--------*/
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return TRUE;
}

BOOL FindProcess(DWORD *p)
{
    HANDLE pHandle;
    PROCESSENTRY32 proc;
    pHandle = CreateToolhelp32Snapshot(0x2, 0x0);
    if (pHandle == INVALID_HANDLE_VALUE)
    {
        printf("Create pHandle failed !\n");
        return FALSE;
    }
    proc.dwSize = sizeof(PROCESSENTRY32);
    while (Process32Next(pHandle, &proc))
    {
        if (strcmp(proc.szExeFile, "S2VNA.exe") == 0)
        {
            *p = proc.th32ProcessID;
            CloseHandle(pHandle);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL KillProcess(DWORD process_id)
{
    HANDLE hProcess = NULL;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
    return TRUE;
}

BOOL SetupS2VNA(void)
{
    FILE *fp;
    fp = fopen("C:\\VNA\\S2VNA\\system\\Setup.dat", "w");
    fprintf(fp, "[Hardware]\n");
    fprintf(fp, "DeviceID=9\n");
    fprintf(fp, "AnalyzerModel=3\n");
    fprintf(fp, "\n");
    fprintf(fp, "[SocketSvrSetup]\n");
    fprintf(fp, "SocketSvrEnabled=1\n");
    fprintf(fp, "\n");
    fclose(fp);
    return TRUE;
}
