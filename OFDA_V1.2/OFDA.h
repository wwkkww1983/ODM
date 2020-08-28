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

#define INI_PANEL 1

#define PANEL 2                      /* callback function: panelCB */
#define PANEL_RING_Range 2           /* control type: ring, callback function: OneClickRange */
#define PANEL_RING_Mode 3            /* control type: ring, callback function: OneClickMode */
#define PANEL_RING_parameter 4       /* control type: ring, callback function: OneClickPar */
#define PANEL_RING_wavelength 5      /* control type: ring, callback function: OneClickWaveLen */
#define PANEL_NUMERIC_Distance_M3 6  /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Loss_DM 7      /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Distance_M2 8  /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Loss_M2 9      /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Distance_M1 10 /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Loss_M1 11     /* control type: numeric, callback function: (none) */
#define PANEL_NUMERIC_Refraction 12  /* control type: numeric, callback function: (none) */
#define PANEL_GRAPH 13               /* control type: graph, callback function: (none) */
#define PANEL_NUMERICSLIDE 14        /* control type: scale, callback function: (none) */
#define PANEL_DECORATION_3 15        /* control type: deco, callback function: (none) */
#define PANEL_DECORATION_2 16        /* control type: deco, callback function: (none) */
#define PANEL_TEXTMSG_INI 17         /* control type: textMsg, callback function: (none) */
#define PANEL_COMMANDBUTTON_STOP 18  /* control type: command, callback function: OneClickStop */
#define PANEL_COMMANDBUTTON_Start 19 /* control type: command, callback function: OneClickStart */
#define PANEL_COMMANDBUTTON_CAL 20   /* control type: command, callback function: OneClickCal */
#define PANEL_COMMANDBUTTON_NORM 21  /* control type: command, callback function: OneClickNorm */
#define PANEL_DECORATION 22          /* control type: deco, callback function: (none) */
#define PANEL_TEXTMSG_AVE 23         /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Wavelength 24  /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Mode 25        /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Range 26       /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Refraction 27  /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Marker 28      /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Marker1 29     /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Marker2 30     /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_DeltaMarker 31 /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Loss 32        /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_Distance 33    /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG 34             /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_NORM 35        /* control type: textMsg, callback function: (none) */
#define PANEL_TEXTMSG_CAL 36         /* control type: textMsg, callback function: (none) */

     /* Control Arrays: */

     /* (no control arrays in the resource file) */

     /* Menu Bars, Menus, and Menu Items: */

#define MENUBAR 1
#define MENUBAR_MENU1 2
#define MENUBAR_MENU1_ITEM1 3 /* callback function: OpenApp */

     /* Callback Prototypes: */

     int CVICALLBACK OneClickCal(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickNorm(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickPar(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickRange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickStart(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickStop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     int CVICALLBACK OneClickWaveLen(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
     void CVICALLBACK OpenApp(int menubar, int menuItem, void *callbackData, int panel);
     int CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);

#ifdef __cplusplus
}
#endif
