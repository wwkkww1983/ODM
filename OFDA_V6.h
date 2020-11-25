/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
extern "C"
{
#endif

     /* Panels and Controls: */

#define PANEL 1                     /* callback function: panelCB */
#define PANEL_RING_Length 2         /* control type: ring, callback function: OneClickLength */
#define PANEL_RING_Range 3          /* control type: ring, callback function: OneClickRange */
#define PANEL_RING_Mode 4           /* control type: ring, callback function: OneClickMode */
#define PANEL_RING_Wavelength 5     /* control type: ring, callback function: (none) */
#define PANEL_VALUE_Length_M3 6     /* control type: numeric, callback function: (none) */
#define PANEL_VALUE_Length_M3S 7    /* control type: numeric, callback function: (none) */
#define PANEL_VALUE_Delay_M3 8      /* control type: numeric, callback function: (none) */
#define PANEL_VALUE_Length_M2S 9    /* control type: numeric, callback function: (none) */
#define PANEL_VALUE_Length_M2 10    /* control type: numeric, callback function: (none) */
#define PANEL_VALUE_Delay_M2 11     /* control type: numeric, callback function: (none) */
#define PANEL_VALUE_Length_M1S 12   /* control type: numeric, callback function: (none) */
#define PANEL_VALUE_Length_M1 13    /* control type: numeric, callback function: (none) */
#define PANEL_VALUE_Delay_M1 14     /* control type: numeric, callback function: (none) */
#define PANEL_INPUT_Refraction 15   /* control type: numeric, callback function: (none) */
#define PANEL_GRAPH 16              /* control type: graph, callback function: (none) */
#define PANEL_ProgressBar 17        /* control type: scale, callback function: (none) */
#define PANEL_TEXT_TempUnit 18      /* control type: textMsg, callback function: (none) */
#define PANEL_VALUE_Temp 19         /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Wavelength 20    /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Mode 21          /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Range 22         /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Refraction 23    /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Marker 24        /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Marker2 25       /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Delay 26         /* control type: textMsg, callback function: (none) */
#define PANEL_VALUE_Average 27      /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_GraphTitle 28    /* control type: textMsg, callback function: (none) */
#define PANEL_BUTTON_Pause 29       /* control type: command, callback function: OneClickHoldMeas */
#define PANEL_BUTTON_Single 30      /* control type: command, callback function: OneClickSingleSweep */
#define PANEL_BUTTON_Timing 31      /* control type: command, callback function: OneClickTimingMea */
#define PANEL_BUTTON_Clear 32       /* control type: command, callback function: OneClickClear */
#define PANEL_BUTTON_Continuous 33  /* control type: command, callback function: OneClickContinue */
#define PANEL_TEXT_Calibration 34   /* control type: textMsg, callback function: (none) */
#define PANEL_VALUE_Calibration 35  /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Average 36       /* control type: textMsg, callback function: (none) */
#define PANEL_VALUE_IL 37           /* control type: textMsg, callback function: (none) */
#define PANEL_VALUE_TempDiffer 38   /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_TempDiffer 39    /* control type: textMsg, callback function: (none) */
#define PANEL_GraphTitleBG 40       /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Caution 41       /* control type: textMsg, callback function: (none) */
#define PANEL_STRIPCHART 42         /* control type: strip, callback function: (none) */
#define PANEL_AREA_SetParameter 43  /* control type: deco, callback function: (none) */
#define PANEL_TEXT_MarkerDiffer 44  /* control type: textMsg, callback function: (none) */
#define PANEL_TEXT_Marker1 45       /* control type: textMsg, callback function: (none) */
#define PANEL_BUTTON_Calibration 46 /* control type: command, callback function: OneClickCal */
#define PANEL_AREA_Main 47          /* control type: deco, callback function: (none) */
#define PANEL_TEXT_IL 48            /* control type: textMsg, callback function: (none) */

#define PANEL_ADM 2                     /* callback function: panelADM */
#define PANEL_ADM_BUTTON_EXIT 2         /* control type: command, callback function: OneClickAdminEXIT */
#define PANEL_ADM_BUTTON_OK 3           /* control type: command, callback function: OneClickAdmOK */
#define PANEL_ADM_INPUT_AdminPassword 4 /* control type: string, callback function: (none) */

#define PANEL_DEV 3                  /* callback function: panelDEV */
#define PANEL_DEV_INPUT_License_1 2  /* control type: string, callback function: (none) */
#define PANEL_DEV_INPUT_License_2 3  /* control type: string, callback function: (none) */
#define PANEL_DEV_INPUT_License_3 4  /* control type: string, callback function: (none) */
#define PANEL_DEV_INPUT_License_4 5  /* control type: string, callback function: (none) */
#define PANEL_DEV_BUTTON_OK 6        /* control type: command, callback function: OneClickDevOK */
#define PANEL_DEV_TEXT_License 7     /* control type: textMsg, callback function: (none) */
#define PANEL_DEV_VALUE_RemainTime 8 /* control type: textMsg, callback function: (none) */
#define PANEL_DEV_TEXT_RemainTime 9  /* control type: textMsg, callback function: (none) */

#define PANEL_REFR 4                  /* callback function: panelREFR */
#define PANEL_REFR_INPUT_Refraction 2 /* control type: numeric, callback function: (none) */
#define PANEL_REFR_BUTTON_OK 3        /* control type: command, callback function: OneClickCalRefractionOK */

#define PANEL_TIMM 5                /* callback function: panelTIMM */
#define PANEL_TIMM_INPUT_Point 2    /* control type: numeric, callback function: (none) */
#define PANEL_TIMM_INPUT_Delay 3    /* control type: numeric, callback function: (none) */
#define PANEL_TIMM_TEXT_DelayUnit 4 /* control type: textMsg, callback function: (none) */
#define PANEL_TIMM_BUTTON_OK 5      /* control type: command, callback function: OneClickTimingSetOK */

     /* Control Arrays: */

     /* (no control arrays in the resource file) */

     /* Menu Bars, Menus, and Menu Items: */

#define MENUBAR 1
#define MENUBAR_File 2
#define MENUBAR_File_SaveData 3 /* callback function: menuSave */
#define MENUBAR_File_Print 4    /* callback function: menuPrint */
#define MENUBAR_Function 5
#define MENUBAR_Function_Average 6       /* callback function: menuAverage */
#define MENUBAR_Function_SwitchVersion 7 /* callback function: menuOpenAPP */
#define MENUBAR_Set 8
#define MENUBAR_Set_SetRefraction 9 /* callback function: menuCalRefraction */
#define MENUBAR_Set_SetInterval 10  /* callback function: menuTimingSet */
#define MENUBAR_Language 11
#define MENUBAR_Language_Chinese 12 /* callback function: menuTransCHN */
#define MENUBAR_Language_English 13 /* callback function: menuTransENG */
#define MENUBAR_Other 14
#define MENUBAR_Other_Help 15     /* callback function: menuHelp */
#define MENUBAR_Other_About 16    /* callback function: menuAbout */
#define MENUBAR_Other_Admin 17    /* callback function: menuDevOpin */
#define MENUBAR_Other_Register 18 /* callback function: menuRegister */

     /* Callback Prototypes: */

     void CVICALLBACK menuAbout(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuAverage(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuCalRefraction(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuDevOpin(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuHelp(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuOpenAPP(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuPrint(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuRegister(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuSave(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuTimingSet(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuTransCHN(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuTransENG(int menubar, int menuItem, void *callbackData, int panel);
     int CVICALLBACK OneClickAdminEXIT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickAdmOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickCal(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickCalRefractionOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickClear(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickContinue(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickDevOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickHoldMeas(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickLength(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickRange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickSingleSweep(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickTimingMea(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickTimingSetOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK panelADM(int panel, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK panelDEV(int panel, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK panelREFR(int panel, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK panelTIMM(int panel, int event, void *callbackData, int eventData1, int eventData2);

#ifdef __cplusplus
}
#endif
