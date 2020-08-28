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
//==============================================================================

int GetInstrCom(int *comPort)
{
	int sendByte = 0x28,
		error = 0,
		y = 1;
	char getString[25] = {0}; //*********************************************************************************************pan改   修改长度为25
	char ODM[5] = "ODM-";
	for (int i = 1; i < 30; i++)
	{ //*********************************************************************************************pan改  修改i为30.增加扫描范围
		error = OpenComConfig(i, "", 9600, 0, 8, 1, 1, 4);
		if (error == 0)
		{
			FlushInQ(i);
			FlushOutQ(i);
			ComWrtByte(i, sendByte);
			Delay(0.001);
			ComRd(i, getString, 25); //*********************************************************************************************pan改   修改长度为25
			Delay(0.05);			 //*********************************************************************************************pan改   修改延时为0.05
			y = strncmp(ODM, getString, 4);
			if (y == 0)
			{
				*comPort = i;
				//CloseCom (i);
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

int SetOS1(int comPort, int OSstatus)
{
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
		Delay(0.01); //*********************************************************************************************pan改   修改延时为0.01
		ComWrtByte(comPort, sendByte);
	}
	else if (OSstatus == WL1550)
	{
		sendByte = 0x31;
		Delay(0.01); //*********************************************************************************************pan改   修改延时为0.01
		ComWrtByte(comPort, sendByte);
	}
	Delay(0.005); //*********************************************************************************************pan改   修改延时为0.005
				  //	CloseCom (comPort);
	return 1;
}

int SetOS2(int comPort, int OSstatus)
{
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
		Delay(0.01); //*********************************************************************************************pan改   修改延时为0.01
		ComWrtByte(comPort, sendByte);
	}
	else if (OSstatus == REFLECT)
	{
		sendByte = 0x33;
		Delay(0.01); //*********************************************************************************************pan改   修改延时为0.01
		ComWrtByte(comPort, sendByte);
	}
	Delay(0.005); //*********************************************************************************************pan改   修改延时为0.005
				  //	CloseCom (comPort);
	return 1;
}

int Sendbyte(int comPort, int sendByte)
{
	int error;
	error = OpenComConfig(comPort, "", 9600, 0, 8, 1, 1, 4);
	if (error != 0)
		return error;
	FlushInQ(comPort);
	FlushOutQ(comPort);
	ComWrtByte(comPort, sendByte);
	Delay(0.005); //*********************************************************************************************pan改   修改延时为0.005
				  //	CloseCom (comPort);
	return 1;
}
