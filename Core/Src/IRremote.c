
#include "IRremote.h"
#include "main.h"
#include "DFPLAYER_MINI.h"
//uint16_t count = 0;

volatile irparams_t irparams;
decode_results results;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	  if (htim->Instance == TIM2)
	  {
		  Pin0Cntr++;
		  Pin1Cntr++;
		  Pin2Cntr++;
		  Pin3Cntr++;
		  Pin4Cntr++;
		  RxBufWaitCntr++;
	  }
	if(htim == &htim4)
	{
		uint8_t irdata = RECIV_PIN; // пин для приёма
		/*count++;
		if(count > 1000)
		{
			count = 0;
			HAL_GPIO_TogglePin(pa3_GPIO_Port, pa3_Pin);
		}*/

		irparams.timer++;  // One more 50uS tick
		if(irparams.rawlen >= RAWBUF) irparams.rcvstate = STATE_OVERFLOW;  // Buffer overflow

		switch(irparams.rcvstate)
		{
			case STATE_IDLE: // In the middle of a gap
				if(irdata == MARK)
				{
					if(irparams.timer < GAP_TICKS)  // Not big enough to be a gap.
					{
						irparams.timer = 0;
					}
					else
					{
						// Gap just ended; Record duration; Start recording transmission
						irparams.overflow = 0;
						irparams.rawlen  = 0;
						irparams.rawbuf[irparams.rawlen++] = irparams.timer;
						irparams.timer = 0;
						irparams.rcvstate = STATE_MARK;
					}
				}

			break;

			case STATE_MARK:  // Timing Mark
				if(irdata == SPACE) // Mark ended; Record time
				{
					irparams.rawbuf[irparams.rawlen++] = irparams.timer;
					irparams.timer = 0;
					irparams.rcvstate = STATE_SPACE;
				}

			break;

			case STATE_SPACE:  // Timing Space
				if(irdata == MARK) // Space just ended; Record time
				{
					irparams.rawbuf[irparams.rawlen++] = irparams.timer;
					irparams.timer = 0;
					irparams.rcvstate = STATE_MARK;

				}
				else if(irparams.timer > GAP_TICKS) // Space
				{
					irparams.rcvstate = STATE_STOP;
				}

			break;

			case STATE_STOP:  // Waiting; Measuring Gap
			 	if(irdata == MARK) irparams.timer = 0;  // Reset gap timer
			break;

			case STATE_OVERFLOW:  // Flag up a read overflow; Stop the State Machine
				irparams.overflow = 1;
				irparams.rcvstate = STATE_STOP;
			break;
		}
	}
}

int16_t my_decode(decode_results *result)
{
	if(irparams.rcvstate != STATE_STOP) return 0;

	result->rawbuf = irparams.rawbuf;
	result->rawlen = irparams.rawlen;
	result->overflow = irparams.overflow;

	if(decodeHash(result)) return 1;
	//my_resume();
	my_enableIRIn();
	return 0;
}

void my_enableIRIn() // initialization
{
	irparams.rcvstate = STATE_IDLE;
	irparams.rawlen = 0;

	__HAL_TIM_SET_COUNTER(&htim4, 0x0000);
	HAL_TIM_Base_Start_IT(&htim4);
}

/*uint8_t my_isIdle() // Return if receiving new IR signals
{
	return (irparams.rcvstate == STATE_IDLE || irparams.rcvstate == STATE_STOP) ? true : false;
}*/

void my_resume() // Restart the ISR state machine
{
	irparams.rcvstate = STATE_IDLE;
	irparams.rawlen = 0;
}

int16_t compare(uint16_t oldval, uint16_t newval)
{
	if(newval < oldval * .8) return 0;
	else if(oldval < newval * .8) return 2;
	else return 1;
}

#define FNV_PRIME_32 16777619
#define FNV_BASIS_32 2166136261

int32_t decodeHash(decode_results *result)
{
	int32_t hash = FNV_BASIS_32;

	if(result->rawlen < 6) return 0;

	for(int16_t i = 1; (i + 2) < result->rawlen; i++)
	{
		int16_t value = compare(result->rawbuf[i], result->rawbuf[i+2]);
		hash = (hash * FNV_PRIME_32) ^ value;
	}

	result->value = hash;
	result->bits = 32;
	return 1;
}









