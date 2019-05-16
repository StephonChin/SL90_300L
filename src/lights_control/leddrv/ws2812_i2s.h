// ws2812_lib.h

#ifndef __WS2812_I2S_H__
#define __WS2812_I2S_H__

//include files
#include <stdint.h>
#include "ws2812_defs.h"
#include "ws2812_dma.h"


//-----------------------------------------------------------
//21 NOV 2018 Modify by DS.Chin
typedef struct
{
  uint8_t DutyR; // R,G,B order is determined by WS2812B
  uint8_t DutyG;
  uint8_t DutyB;
} Pixel_t;

#define    DRV_METHORD_IDLE     (uint8_t)0      /* idle : all data set to be 0xff */
#define    DRV_METHORD_NORMAL   (uint8_t)1      /* normally drive */
#define    DRV_METHORD_ONCE     (uint8_t)2      /* update data once, example : steady mode */
#define    DRV_METHORD_ONCE_AG  (uint8_t)3      /* send twince when steady */
#define    DRV_METHORD_ALL      (uint8_t)4      /* work in test mode when all leds are the save data */



void Ws2812_Init(void);
void Ws2812_Show(void);


//exported parameters
extern Pixel_t        LedData[];
extern uint8_t        BrightLevel;

//file function
uint32_t *Word_Ptr_Get(uint8_t *fst, uint8_t *bitcnt, uint32_t *ptr);




#endif

// end of file
