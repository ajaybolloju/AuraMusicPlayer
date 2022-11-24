/*
 * DFPLAYER_MINI.c
 *
 *  Created on: May 16, 2020
 *      Author: controllerstech
 */


#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "IRremote.h"


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
static uint8_t CurrTrack = 0;
static uint8_t PrevTrack = 0;
static uint8_t SpecificTrack = 0;
static uint8_t SpecificTrackBtn = 0;

uint8_t aRxBuffer = 0;
uint8_t g_buff[200] = {0};
uint8_t RxBufCntr = 0;
uint8_t RxBufWaitCntr = 0;

uint8_t DF_CurrentTrackCMD (void);
void DF_ParseResponse (void);

void SetNextButtonStatus(void)
{
	CurrTrack++;
	NextButton = 1;
}

void SetPrevButtonStatus(void)
{
	CurrTrack--;
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

void SetSpecificTrack(uint8_t Track_Idx)
{
	SpecificTrackBtn = 1;
	SpecificTrack = Track_Idx;
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

uint8_t GetSpecificTrackButtonStatus(void)
{
	if(SpecificTrackBtn)
	{
		SpecificTrackBtn = 0;
		return 1;
	}
	return 0;
}

uint8_t GetSpecifcTrackIndex(void)
{
	return SpecificTrack;
}

void HandleLEDs(void)
{
	  HAL_GPIO_WritePin(LED_GPIO, LED_1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED_GPIO, LED_2_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED_GPIO, LED_3_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED_GPIO, LED_4_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED_GPIO, LED_5_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED_GPIO, LED_6_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED_GPIO, LED_7_Pin, GPIO_PIN_RESET);
	  HAL_Delay(1000);
	  switch(DF_CurrentTrackCMD())
	  {
	    case 1:
	    {
	  	  HAL_GPIO_WritePin(LED_GPIO, LED_1_Pin, GPIO_PIN_SET);
	    }
	    break;
	    case 2:
	    {
	  	  HAL_GPIO_WritePin(LED_GPIO, LED_2_Pin, GPIO_PIN_SET);
	    }
	    break;
	    case 3:
	    {
	  	  HAL_GPIO_WritePin(LED_GPIO, LED_3_Pin, GPIO_PIN_SET);
	    }
	    break;
	    case 4:
	    {
	  	  HAL_GPIO_WritePin(LED_GPIO, LED_4_Pin, GPIO_PIN_SET);
	    }
	    break;
	    case 5:
	    {
	  	  HAL_GPIO_WritePin(LED_GPIO, LED_5_Pin, GPIO_PIN_SET);
	    }
	    break;
	    case 6:
	    {
	  	  HAL_GPIO_WritePin(LED_GPIO, LED_6_Pin, GPIO_PIN_SET);
	    }
	    break;
	    case 7:
	    {
	  	  HAL_GPIO_WritePin(LED_GPIO, LED_7_Pin, GPIO_PIN_SET);
	    }
	    break;
	    default:
	    {
		  	  HAL_GPIO_WritePin(LED_GPIO, LED_1_Pin, GPIO_PIN_SET);
	    }
	    break;
	  }
}

void Read_RemoteInput(uint32_t IR_Rxd_Cmd)
{
	switch(IR_Rxd_Cmd)
	{
		case IR_Power:
		{
			printf("\n Pwr");
			SetStartPauseButtonStatus();
		}
		break;
		case IR_PausePlay:
		{
			printf("\n Pause play");
			SetStartPauseButtonStatus();
		}
		break;
		case IR_Backward:
		{
			printf("\n Backward");
			SetPrevButtonStatus();
		}
		break;
		case IR_Farward:
		{
			printf("\n farward");
			SetNextButtonStatus();
		}
		break;
		case IR_Vol_Dec:
		{
			printf("\n Vol Dec");
			SetVolDecButtonStatus();
		}
		break;
		case IR_Vol_Inc:
		{
			printf("\n Vol Inc");
			SetVolIncButtonStatus();
		}
		break;
		case IR_One:
		{
			printf("\n One");
			SetSpecificTrack(1);
		}
		break;
		case IR_Two:
		{
			printf("\n two");
			SetSpecificTrack(2);
		}
		break;
		case IR_Three:
		{
			printf("\n Three");
			SetSpecificTrack(3);
		}
		break;
		case IR_Four:
		{
			printf("\n four");
			SetSpecificTrack(4);
		}
		break;
		case IR_Five:
		{
			printf("\n five");
			SetSpecificTrack(5);
		}
		break;
		case IR_Six:
		{
			printf("\n six");
			SetSpecificTrack(6);
		}
		break;
		case IR_Seven:
		{
			printf("\n seven");
			SetSpecificTrack(7);
		}
		break;
		default:
			break;
	}
	IR_Rxd_Cmd = 0;
}

void Send_cmd (uint8_t cmd, uint8_t Parameter1, uint8_t Parameter2)
{
	uint16_t Checksum = Version + Cmd_Len + cmd + Feedback + Parameter1 + Parameter2;
	Checksum = 0-Checksum;

	uint8_t CmdSequence[10] = { Start_Byte, Version, Cmd_Len, cmd, Feedback, Parameter1, Parameter2, (Checksum>>8)&0x00ff, (Checksum&0x00ff), End_Byte};

	printf("\n Tx: 0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X \n",
			CmdSequence[0],CmdSequence[1],CmdSequence[2],CmdSequence[3],CmdSequence[4],
			CmdSequence[5],CmdSequence[6],CmdSequence[7],CmdSequence[8],CmdSequence[9]);
	if(HAL_UART_Transmit(DF_UART, CmdSequence, 10, HAL_MAX_DELAY) != HAL_OK)
	{
		return;
	}
}

void DF_PlayFromStart(void)
{
  Send_cmd(0x03,0x00,0x01);
  CurrTrack = 1;
  PrevTrack = 1;
  HandleLEDs();
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

void DF_SpecifcTrack (void)
{
	Send_cmd(0x03,0x00,GetSpecifcTrackIndex());
	HAL_Delay(200);
}

uint8_t DF_CurrentTrackCMD (void)
{
	static uint8_t TrackState = 0;
	switch(TrackState)
	{
		case 0:
		{
			if(CurrTrack == 0)
			{
				RxBufCntr = 0;
				Send_cmd(0x4B, 0, 0);
				HAL_Delay(200);
			}
			TrackState = 1;
		}
		break;
		case 1:
		{
			if(CurrTrack != 0)
			{
				if(CurrTrack > 7)
				{
					CurrTrack = 1;
				}
				return CurrTrack;
			}
			else if(RxBufWaitCntr > 4)
			{
				RxBufWaitCntr = 0;
				TrackState = 0;
				if(CurrTrack != 0)
				{
					return CurrTrack;
				}
			}
		}
		break;
	}
	return CurrTrack;
}

void DF_ParseResponse (void)
{
	static uint8_t TrackState = 0;
	switch(TrackState)
	{
		case 0:
		{
			if(RxBufCntr == 1)
			{
				RxBufWaitCntr = 0;
				TrackState = 1;
			}
		}
		break;
		case 1:
		{
			if((RxBufCntr >= 10) || (RxBufWaitCntr > 2))
			{
				RxBufWaitCntr = 0;
				TrackState = 2;
			}
		}
		break;
		case 2:
		{
			if(RxBufCntr >= 10)
			{
				if((g_buff[0] == Start_Byte)&&(g_buff[9] == End_Byte))
				{
					if((g_buff[3] == 0x4B) && (g_buff[6] != 0x00)) //|| (g_buff[3] == 0x41))
					{
						CurrTrack = g_buff[6];
					}
					else if(g_buff[3] == 0x3D)
					{
						PrevTrack = g_buff[6];
						CurrTrack = PrevTrack;
						if(PrevTrack == 7)
						{
							DF_PlayFromStart();
						}
						else
						{
							SetNextButtonStatus();
						}
					}
				}
				TrackState = 0;
				RxBufCntr = 0;
				memset(g_buff,0,sizeof(g_buff));
			}
			else if(RxBufWaitCntr > 10)
			{
				RxBufWaitCntr = 0;
				if(RxBufCntr >= 10)
				{
					TrackState = 2;
				}
				else
				{
					TrackState = 0;
					RxBufCntr = 0;
					memset(g_buff,0,sizeof(g_buff));
				}
			}
		}
		break;
	}
}

void Check_Key (void)
{
	DF_ParseResponse();
	if (GetNextButtonStatus())
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		DF_Next();
		HandleLEDs();
	}

	if (GetPrevButtonStatus())
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		DF_Previous();
		HandleLEDs();
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

	if (GetSpecificTrackButtonStatus())
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		DF_SpecifcTrack();
	}

	if (GetStartPauseButtonStatus())
	{
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HandleLEDs();
		if (isplaying)
		{
			ispause = 1;
			isplaying = 0;
			DF_Pause();
//			DF_CurrentTrackCMD();
		}

		else if (ispause)
		{
			isplaying = 1;
			ispause = 0;
			DF_Playback();
		}
	}
}















