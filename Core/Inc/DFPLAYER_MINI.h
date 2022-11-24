/*
 * DFPLAYER_MINI.h
 *
 *  Created on: May 16, 2020
 *      Author: controllerstech
 */

#ifndef INC_DFPLAYER_MINI_H_
#define INC_DFPLAYER_MINI_H_

extern uint8_t aRxBuffer;
extern uint8_t g_buff[200];
extern uint8_t RxBufCntr;
extern uint8_t RxBufWaitCntr;

void Send_cmd (uint8_t cmd, uint8_t Parameter1, uint8_t Parameter2);
void DF_PlayFromStart(void);
void DF_Init (uint8_t volume);
void DF_Next (void);
void DF_Pause (void);
void DF_Previous (void);
void DF_Playback (void);

void Check_Key (void);
void SetNextButtonStatus(void);
void SetPrevButtonStatus(void);
void SetStartPauseButtonStatus(void);
void SetVolIncButtonStatus(void);
void SetVolDecButtonStatus(void);
void Read_RemoteInput(uint32_t IR_Rxd_Cmd);

#endif /* INC_DFPLAYER_MINI_H_ */
