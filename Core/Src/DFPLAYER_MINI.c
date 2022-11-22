/*
 * DFPLAYER_MINI.c
 *
 *  Created on: May 16, 2020
 *      Author: controllerstech
 */


#include "stm32f1xx_hal.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;
#define DF_UART &huart1

#define Source      0x02  // TF CARD

#define Previous_Key   GPIO_PIN_1
#define Previous_Port  GPIOA
#define Pause_Key      GPIO_PIN_2
#define Pause_Port     GPIOA
#define Next_Key       GPIO_PIN_3
#define Next_Port      GPIOA

/*************************************** NO CHANGES AFTER THIS *************************************************/

int ispause =0;
int isplaying=1;


# define Start_Byte 0x7E
# define End_Byte   0xEF
# define Version    0xFF
# define Cmd_Len    0x06
# define Feedback   0x01    //If need for Feedback: 0x01,  No Feedback: 0


static uint8_t NextButton = 0;
static uint8_t PrevButton = 0;
static uint8_t VolIncButton = 0;
static uint8_t VolDecButton = 0;
static uint8_t StartPauseButton = 0;
static uint8_t MuteButton = 0;


void SetNextButtonStatus(void)
{
	NextButton = 1;
}

void SetPrevButtonStatus(void)
{
	PrevButton = 1;
}

void SetVolIncButtonStatus(void)
{
	VolIncButton = 1;
}

void SetVolDecButtonStatus(void)
{
	VolDecButton = 1;
}

void SetStartPauseButtonStatus(void)
{
	StartPauseButton = 1;
}

//void SetMuteButtonStatus(void)
//{
//	MuteButton = 1;
//}

uint8_t GetNextButtonStatus(void)
{
	if(NextButton)
	{
		NextButton = 0;
		return 1;
	}
	return 0;
}

uint8_t GetPrevButtonStatus(void)
{
	if(PrevButton)
	{
		PrevButton = 0;
		return 1;
	}
	return 0;
}

uint8_t GetVolIncButtonStatus(void)
{
	if(VolIncButton)
	{
		VolIncButton = 0;
		return 1;
	}
	return 0;
}

uint8_t GetVolDecButtonStatus(void)
{
	if(VolDecButton)
	{
		VolDecButton = 0;
		return 1;
	}
	return 0;
}

uint8_t GetStartPauseButtonStatus(void)
{
	if(StartPauseButton)
	{
		StartPauseButton = 0;
		return 1;
	}
	return 0;
}

uint8_t GetMuteButtonStatus(void)
{
	if(MuteButton)
	{
		MuteButton = 0;
		return 1;
	}
	return 0;
}

void Send_cmd (uint8_t cmd, uint8_t Parameter1, uint8_t Parameter2)
{
	uint16_t Checksum = Version + Cmd_Len + cmd + Feedback + Parameter1 + Parameter2;
	Checksum = 0-Checksum;

	uint8_t CmdSequence[10] = { Start_Byte, Version, Cmd_Len, cmd, Feedback, Parameter1, Parameter2, (Checksum>>8)&0x00ff, (Checksum&0x00ff), End_Byte};

	if(HAL_UART_Transmit(DF_UART, CmdSequence, 10, HAL_MAX_DELAY) != HAL_OK)
	{
		return;
	}
}

void DF_PlayFromStart(void)
{
  Send_cmd(0x03,0x00,0x01);
  HAL_Delay(200);
}


void DF_Init (uint8_t volume)
{
	Send_cmd(0x3F, 0x00, Source);
	HAL_Delay(200);
	Send_cmd(0x06, 0x00, volume);
	HAL_Delay(500);
}

void DF_Next (void)
{
	Send_cmd(0x01, 0x00, 0x00);
	HAL_Delay(200);
}

void DF_Pause (void)
{
	Send_cmd(0x0E, 0, 0);
	HAL_Delay(200);
}

void DF_Previous (void)
{
	Send_cmd(0x02, 0, 0);
	HAL_Delay(200);
}

void DF_Playback (void)
{
	Send_cmd(0x0D, 0, 0);
	HAL_Delay(200);
}

void DF_VolInc (void)
{
	Send_cmd(0x04, 0, 0);
	HAL_Delay(200);
}

void DF_VolDec (void)
{
	Send_cmd(0x05, 0, 0);
	HAL_Delay(200);
}

//void DF_VolMute (void)
//{
//	Send_cmd(0x06, 0, 0);
//	HAL_Delay(200);
//}

void Check_Key (void)
{
	if (GetNextButtonStatus())
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		DF_Next();
	}

	if (GetPrevButtonStatus())
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		DF_Previous();
	}

	if (GetVolIncButtonStatus())
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		DF_VolInc();
	}

	if (GetVolDecButtonStatus())
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		DF_VolDec();
	}

	if (GetStartPauseButtonStatus())
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		if (isplaying)
		{
			ispause = 1;
			isplaying = 0;
			DF_Pause();
		}

		else if (ispause)
		{
			isplaying = 1;
			ispause = 0;
			DF_Playback();
		}
	}
}















