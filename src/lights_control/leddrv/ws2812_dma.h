// ws2812_dma.h 

#ifndef __WS2812_DMA_H__
#define __WS2812_DMA_H__


//include files
#include <string.h>
#include "slc_register.h"
#include "user_interface.h"
#include "ws2812_defs.h"
#include "../com/mcu_com.h"
#include "osapi.h"

#ifdef __cplusplus
extern "C" {
#endif




// type definition taken from : sdio_slv.h
typedef struct 
{
  uint32_t  blocksize:12;
  uint32_t  datalen:12;
  uint32_t  unused:5;
  uint32_t  sub_sof:1;
  uint32_t  eof:1;
  uint32_t  owner:1;
  uint32_t  buf_ptr;
  uint32_t  next_link_ptr;
} sdio_queue_t;

// -----------------------------------------------------

void ws2812_dma(sdio_queue_t *);


extern uint16_t  SendCnt;
extern uint32_t  i2s_buffer1[NUM_I2S_PIXEL_WORDS1];
extern uint32_t  i2s_send_buffer1[NUM_I2S_PIXEL_WORDS1];
extern uint32_t  i2s_buffer2[NUM_I2S_PIXEL_WORDS2];
extern uint32_t  i2s_send_buffer2[NUM_I2S_PIXEL_WORDS2];
extern sdio_queue_t i2s_pixels_queue1;
extern sdio_queue_t i2s_pixels_queue2;
extern bool      UpdateFlag;

#ifdef __cplusplus
}
#endif


#endif

