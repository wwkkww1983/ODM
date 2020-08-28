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

#define PANEL 1                       /* callback function: panelCB */
#define PANEL_RING_LENGTH 2           /* control type: ring, callback function: ConvertLength */
#define PANEL_RING_Range 3            /* control type: ring, callback function: OneClickRange */
#define PANEL_RING_Mode 4             /* control type: ring, callback function: OneClickMode */
#define PANEL_RING_Wavelength 5       /* control type: ring, callback function: (none) */
#define PANEL_NUMERIC_Distance_M3 6   /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Distance_M3S 7  /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Delay_M3 8      /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Distance_M2S 9  /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Distance_M2 10  /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Delay_M2 11     /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Distance_M1S 12 /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Distance_M1 13  /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Delay_M1 14     /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Refraction 15   /* control type: numeric, callback function: (none) */
#define PANEL_GRAPH 16                /* control type: graph, callback function: (none) */
#define PANEL_NUMERICSLIDE 17         /* control type: scale, callback function: (none) */
#define PANEL_TEXTMSG_Sheshidu 18     /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_TEMP 19         /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Wavelength 20   /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Mode 21         /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Range 22        /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Refraction 23   /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Marker 24       /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Marker2 25      /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Delay 26        /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_AVE 27          /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_TITLE 28        /* control type: textMsg, callback function: (none) */
#define PANEL_COMMANDBUTTON2 29       /* control type: command, callback function: OneClickHoldMeas */
#define PANEL_COMMANDBUTTON3 30       /* control type: command, callback function: OneClickSingleSweep */
#define PANEL_COMMANDBUTTON4 31       /* control type: command, callback function: OneClickTimingMea */
#define PANEL_COMMANDBUTTON5 32       /* control type: command, callback function: OneClickClear */
#define PANEL_COMMANDBUTTON1 33       /* control type: command, callback function: OneClickContinue */
#define PANEL_TEXTMSG_CAL_2 34        /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_CAL 35          /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_AVE_2 36        /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_IL_1 37         /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_TEMP_2 38       /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_TEMP_3 39       /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_BG 40           /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Caution 41      /* control type: textMsg, callback function: (none) */
#define PANEL_STRIPCHART 42           /* control type: strip, callback function: (none) */
#define PANEL_DECORATION_2 43         /* control type: deco, callback function: (none) */
#define PANEL_TEXTMSG_DeltaMarker 44  /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Marker1 45      /* control type: textMsg, callback function: (none) */
#define PANEL_COMMANDBUTTON6 46       /* control type: command, callback function: OneClickCal */
#define PANEL_DECORATION_3 47         /* control type: deco, callback function: (none) */
#define PANEL_TEXTMSG_IL_2 48         /* control type: textMsg, callback function: (none) */

#define PANEL_ADM 2                 /* callback function: panelADM */
#define PANEL_ADM_COMMANDBUTTON_2 2 /* control type: command, callback function: AdminEXIT */
#define PANEL_ADM_COMMANDBUTTON 3   /* control type: command, callback function: OneClickAdmOK */
#define PANEL_ADM_STRING_ADMIN 4    /* control type: string, callback function: (none) */

#define PANEL_DEV 3                 /* callback function: panelDEV */
#define PANEL_DEV_STRING_CUSTOMER 2 /* control type: string, callback function: (none) */
#define PANEL_DEV_NUMERIC_DAYS 3    /* control type: numeric, callback function: (none) */
#define PANEL_DEV_COMMANDBUTTON 4   /* control type: command, callback function: OneClickDevOK */
#define PANEL_DEV_TEXTMSG 5         /* control type: textMsg, callback function: (none) */
#define PANEL_DEV_STRING_LICENCE 6  /* control type: string, callback function: (none) */

#define PANEL_REFR 4               /* callback function: panelREFR */
#define PANEL_REFR_NUMERIC 2       /* control type: numeric, callback function: (none) */
#define PANEL_REFR_COMMANDBUTTON 3 /* control type: command, callback function: calRefractionEXIT */

#define PANEL_TIMM 5               /* callback function: panelTIMM */
#define PANEL_TIMM_NUMERIC_2 2     /* control type: numeric, callback function: (none) */
#define PANEL_TIMM_NUMERIC 3       /* control type: numeric, callback function: (none) */
#define PANEL_TIMM_TEXTMSG 4       /* control type: textMsg, callback function: (none) */
#define PANEL_TIMM_COMMANDBUTTON 5 /* control type: command, callback function: timingMeaEXIT */

     /* Control Arrays: */

#define CTRLARRAY 1

     /* Menu Bars, Menus, and Menu Items: */

#define MENUBAR 1
#define MENUBAR_File 2
#define MENUBAR_File_ITEM1_1 3 /* callback function: menuSave */
#define MENUBAR_File_ITEM1_3 4 /* callback function: menuPrint */
#define MENUBAR_FUNCTION 5
#define MENUBAR_FUNCTION_ITEM2_1 6 /* callback function: menuAverage */
#define MENUBAR_FUNCTION_ITEM2_2 7 /* callback function: OpenAPP */
#define MENUBAR_SET 8
#define MENUBAR_SET_ITEM3_1 9  /* callback function: menuCalRefraction */
#define MENUBAR_SET_ITEM3_2 10 /* callback function: menuTimingSet */
#define MENUBAR_LANGUAGE 11
#define MENUBAR_LANGUAGE_ITEM4_1 12 /* callback function: TransCHN */
#define MENUBAR_LANGUAGE_ITEM4_2 13 /* callback function: TransENG */
#define MENUBAR_OTHER 14
#define MENUBAR_OTHER_ITEM5_1 15 /* callback function: menuHelp */
#define MENUBAR_OTHER_ITEM5_2 16 /* callback function: menuAbout */
#define MENUBAR_OTHER_ITEM5_3 17 /* callback function: menuDevOpin */
#define MENUBAR_OTHER_ITEM5_4 18 /* callback function: menuRegister */

     /* Callback Prototypes: */

     int CVICALLBACK AdminEXIT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK calRefractionEXIT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK ConvertLength(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     void CVICALLBACK menuAbout(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuAverage(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuCalRefraction(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuDevOpin(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuHelp(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuPrint(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuRegister(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuSave(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK menuTimingSet(int menubar, int menuItem, void *callbackData, int panel);
     int CVICALLBACK OneClickAdmOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickCal(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickClear(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickContinue(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickDevOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickHoldMeas(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickRange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickSingleSweep(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickTimingMea(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     void CVICALLBACK OpenAPP(int menubar, int menuItem, void *callbackData, int panel);
     int CVICALLBACK panelADM(int panel, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK panelDEV(int panel, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK panelREFR(int panel, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK panelTIMM(int panel, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK timingMeaEXIT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     void CVICALLBACK TransCHN(int menubar, int menuItem, void *callbackData, int panel);
     void CVICALLBACK TransENG(int menubar, int menuItem, void *callbackData, int panel);

#ifdef __cplusplus
}
#endif
