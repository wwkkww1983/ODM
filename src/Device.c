// Include files
#include <rs232.h>
#include <utility.h>
#include <ansi_c.h>
#include <string.h>
#include "Device.h"

//==============================================================================
//constant
extern const int THROUGH;
extern const int REFLECT;
extern const int ON;
extern const int OFF;
extern const int WL1550;
extern const int WL1310;
extern int noComPort;
//==============================================================================

int GetInstrCom(int *comPort, char ODM_Name[19])
{
    if (noComPort)
        return 1;
    int sendByte = 0x28,
        error = 0,
        y = 1;
    char ODM[5] = "ODM-";
    for (int i = 1; i < 30; i++)
    {
        error = OpenComConfig(i, "", 9600, 0, 8, 1, 1, 4);
        if (error == 0)
        {
            FlushInQ(i);
            FlushOutQ(i);
            SetComTime(i, 0.1); // 读取超时0.1s后不管
            ComWrtByte(i, sendByte);
            Delay(0.001);
            ComRd(i, ODM_Name, 18);

            Delay(0.05);
            y = strncmp(ODM, ODM_Name, 4);
            if (y == 0)
            {
                *comPort = i;
                //CloseCom (i);
                //ODM_Name = getString;

                return 0;
            }
            else
            {
                CloseCom(i);
            }
        }
    }
    return 1;
}

int ReadTemp(int comPort, char *temp_data)
{
    if (noComPort)
        return 1;
    int x = 0;
    char y[2];
    /*	int error=0;
	error=OpenComConfig (comPort, "", 9600, 0, 8, 1, 1, 4);
	if(error!=0)
		return error; */
    FlushInQ(comPort);
    FlushOutQ(comPort);
    ComWrtByte(comPort, 0x29);
    //Delay(0.01);
    //	ComWrtByte (comPort, 0x29);	Delay(0.01); //不成功多发一次

    for (int i = 0; i < 4; i++)
    {
        x = ComRdByte(comPort);
        sprintf(y, "%c", x);
        *(temp_data + i) = y[0];
    }
    //	CloseCom (comPort);
    return 0;
}

int ReadIL(int comPort, char *IL_data)
{
    if (noComPort)
        return 1;
    int x = 0;
    char y[2];
    //	int error=0;
    //	error=OpenComConfig (comPort, "", 9600, 0, 8, 1, 1, 4);
    //	if(error!=0)
    //		return error;
    FlushInQ(comPort);
    FlushOutQ(comPort);
    ComWrtByte(comPort, 0x27);
    //Delay(0.01);
    //	ComWrtByte (comPort, 0x27);Delay(0.01);   //不成功多发一次
    //	ComRd(comPort,IL_data,4);
    for (int i = 0; i < 4; i++)
    {
        x = ComRdByte(comPort);
        sprintf(y, "%c", x);
        *(IL_data + i) = y[0];
    }
    //	CloseCom(comPort);
    //	error=OpenComConfig (comPort, "", 9600, 0, 8, 1, 1, 4);
    //	if(error!=0)
    //		return error;
    return 0;
}

int SetOS1(int comPort, int OSstatus)
{
    if (noComPort)
        return 1;
    int sendByte = 0;
    /*	int error=0;
	error = OpenComConfig (comPort, "", 9600, 0, 8, 1, 1, 4);
	if(error!=0)
		return error;   */
    FlushInQ(comPort);
    FlushOutQ(comPort);
    if (OSstatus == WL1310)
    {
        sendByte = 0x30;
        Delay(0.01);
        ComWrtByte(comPort, sendByte);
    }
    else if (OSstatus == WL1550)
    {
        sendByte = 0x31;
        Delay(0.01);
        ComWrtByte(comPort, sendByte);
    }
    Delay(0.005);
    //	CloseCom (comPort);
    return 1;
}

int SetOS2(int comPort, int OSstatus)
{
    if (noComPort)
        return 1;
    int sendByte = 0;
    /*	int error=0;
	error = OpenComConfig (comPort, "", 9600, 0, 8, 1, 1, 4);
	if(error!=0)
		return error; */
    FlushInQ(comPort);
    FlushOutQ(comPort);
    if (OSstatus == THROUGH)
    {
        sendByte = 0x32;
        Delay(0.01);
        ComWrtByte(comPort, sendByte);
    }
    else if (OSstatus == REFLECT)
    {
        sendByte = 0x33;
        Delay(0.01);
        ComWrtByte(comPort, sendByte);
    }
    Delay(0.005);
    //	CloseCom (comPort);
    return 1;
}

int Sendbyte(int comPort, int sendByte)
{
    if (noComPort)
        return 1;
    int error;
    error = OpenComConfig(comPort, "", 9600, 0, 8, 1, 1, 4);
    if (error != 0)
        return error;
    FlushInQ(comPort);
    FlushOutQ(comPort);
    ComWrtByte(comPort, sendByte);
    Delay(0.005);
    //	CloseCom (comPort);
    return 1;
}

//---------------以下皆为过期功能，未调用----------------
int ReadLD1(int comPort, int *LDstatus)
{
    int sendByte, getByte = 0;
    int error = 0;
    sendByte = 0x45;
    error = OpenComConfig(comPort, "", 9600, 0, 8, 1, 1, 4);
    if (error != 0)
        return error;
    FlushInQ(comPort);
    FlushOutQ(comPort);
    ComWrtByte(comPort, sendByte);
    Delay(0.001);
    getByte = ComRdByte(comPort);
    Delay(0.001);
    CloseCom(comPort);
    if (getByte == 0x02)
        *LDstatus = ON;
    else if (getByte == 0x03)
        *LDstatus = OFF;
    return 0;
}

int ReadLD2(int comPort, int *LDstatus)
{
    int sendByte, getByte = 0;
    int error = 0;
    sendByte = 0x37;
    error = OpenComConfig(comPort, "", 9600, 0, 8, 1, 1, 4);
    if (error != 0)
        return error;
    FlushInQ(comPort);
    FlushOutQ(comPort);
    ComWrtByte(comPort, sendByte);
    Delay(0.001);
    getByte = ComRdByte(comPort);
    Delay(0.001);
    CloseCom(comPort);
    if (getByte == 0x02)
        *LDstatus = ON;
    else if (getByte == 0x03)
        *LDstatus = OFF;
    return 0;
}

int CautionLight(int comPort, int sendByte)
{
    int error;
    error = OpenComConfig(comPort, "", 9600, 0, 8, 1, 1, 4);
    if (error != 0)
        return error;
    FlushInQ(comPort);
    FlushOutQ(comPort);
    ComWrtByte(comPort, sendByte);
    Delay(0.001);
    CloseCom(comPort);
    return 1;
}

int ReadOS1(int comPort, int *OSstatus)
{
    int sendByte, getByte = 0;
    int error = 0;
    sendByte = 0x35;
    error = OpenComConfig(comPort, "", 9600, 0, 8, 1, 1, 4);
    if (error != 0)
        return error;
    FlushInQ(comPort);
    FlushOutQ(comPort);
    ComWrtByte(comPort, sendByte);
    Delay(0.001);
    getByte = ComRdByte(comPort);
    Delay(0.001);
    CloseCom(comPort);
    if (getByte == 0x02)
        *OSstatus = WL1310;
    else if (getByte == 0x03)
        *OSstatus = WL1550;
    return 0;
}

int ReadOS2(int comPort, int *OSstatus)
{
    int sendByte, getByte[2] = {0};
    int error = 0;
    sendByte = 0x34;
    error = OpenComConfig(comPort, "", 9600, 0, 8, 1, 1, 4);
    if (error != 0)
        return error;
    FlushInQ(comPort);
    FlushOutQ(comPort);
    ComWrtByte(comPort, sendByte);
    Delay(0.001);
    for (int i = 0; i < 2; i++)
    {
        getByte[i] = ComRdByte(comPort);
        Delay(0.001);
    }
    CloseCom(comPort);
    if (getByte[1] == 0x01)
        *OSstatus = THROUGH;
    else if (getByte[1] == 0x00)
        *OSstatus = REFLECT;
    return 0;
}
