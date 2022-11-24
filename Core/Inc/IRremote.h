
#include "main.h"

extern TIM_HandleTypeDef htim4; // таймер для приёма

#define RECIV_PIN (HAL_GPIO_ReadPin(IR_GPIO, IR_Pin)) // пин для приёма recive_IR

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RAWBUF 256

#define STATE_IDLE      2
#define STATE_MARK      3
#define STATE_SPACE     4
#define STATE_STOP      5
#define STATE_OVERFLOW  6

#define GAP_TICKS       100

// IR detector output is active low
#define MARK   0
#define SPACE  1

typedef struct // The fields are ordered to reduce memory over caused by struct-padding
{ 
	uint8_t       rcvstate;        // State Machine state
	uint8_t       rawlen;          // counter of entries in rawbuf
	uint16_t      timer;           // State timer, counts 50uS ticks.
	uint16_t      rawbuf[RAWBUF];  // raw data
	uint8_t       overflow;        // Raw buffer overflow occurred
} irparams_t;



// Results returned from the decoder
typedef struct 
{
		uint32_t value; // Decoded value [max 32-bits]
		int16_t bits; // Number of bits in decoded value
		volatile uint16_t *rawbuf; // Raw intervals in 50uS ticks
		int16_t rawlen; // Number of records in rawbuf
		int16_t overflow; // true iff IR raw code too long
} decode_results;


typedef enum
{
  IR_Power     = 0xef3b295b,
  IR_Mode      = 0xd7c4fb7f,
  IR_Mute      = 0xf1ffe9fb,
  IR_PausePlay = 0x69c6fa7d,
  IR_Backward  = 0xde4001f,
  IR_Farward   = 0x592fa519,
  IR_Equalizer = 0x4da7a0bf,
  IR_Vol_Dec   = 0x3e3d6f9,
  IR_Vol_Inc   = 0xfb54ea5b,
  IR_Zero      = 0x26b9c4dd,
  IR_Repeat    = 0x87e5c91f,
  IR_USB_SD    = 0xef881e99,
  IR_One       = 0x264c7d03,
  IR_Two       = 0x2d607981,
  IR_Three     = 0x87eaa93d,
  IR_Four      = 0x92da21e3,
  IR_Five      = 0x9f8ecfd5,
  IR_Six       = 0x40876b7f,
  IR_Seven     = 0xf6027943,
  IR_Eight     = 0xc264bdb9,
  IR_Nine      = 0xb50e0a1d
}IR_Codes;

int16_t my_decode(decode_results *result);
int32_t decodeHash(decode_results *result);
void my_enableIRIn();
void my_resume();
//uint8_t my_isIdle();



