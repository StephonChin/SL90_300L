// ws2812_lib.cpp
//
// main library file / contains class implementation
//
// Need to give credits to  Charles Lohr (https://github.com/cnlohr due 
// to his work on his software for driving ws2812 led-strips using 
// the i2s interface of the ESP8266.
//
// This inspired me to create an ESP8266 library for the Arduino IDE
// I've added temporal dithering & gamma-correction
// for a 'more natural' light intensity profile.
//
// No pin-definitions/mapings are required/possible as the library used the I2S 
// output-pin. This pin in it's turn is shared with GPIO3 & RXD0
//
#include "ws2812_i2s.h"

//global parameters
Pixel_t       LedData[LED_TOTAL];
uint8_t       BrightLevel = 1;




//file parameters
Pixel_t       LedDrvData[LED_TOTAL];


//file parameters
uint32_t *i2s_pixels_buffer1;
uint32_t *i2s_pixels_buffer2;



//Init led-string / memory buffers etc.
void Ws2812_Init(void)
{
  uint16_t j;

  //do memory allocation for i2s buffer(s)
  i2s_pixels_buffer1 = i2s_send_buffer1;
  i2s_pixels_buffer2 = i2s_send_buffer2;
  
  //clear the buffer
  for(j=0; j<NUM_I2S_PIXEL_WORDS1; j++)    i2s_pixels_buffer1[j] = 0x0;
  for(j=0; j<NUM_I2S_PIXEL_WORDS2; j++)    i2s_pixels_buffer2[j] = 0x0;

  // set-up DMA descriptors / 1 pair per dither-factor
  i2s_pixels_queue1.owner     = 1;
  i2s_pixels_queue1.eof       = 1;
  i2s_pixels_queue1.sub_sof   = 0;
  i2s_pixels_queue1.datalen   = NUM_I2S_PIXEL_BYTES1;     // Size in bytes
  i2s_pixels_queue1.blocksize = NUM_I2S_PIXEL_BYTES1;     // Size in bytes
  i2s_pixels_queue1.buf_ptr   = (uint32_t)i2s_pixels_buffer1;
  i2s_pixels_queue1.unused    = 0;
  i2s_pixels_queue1.next_link_ptr = (uint32_t)&i2s_pixels_queue2;  // always link to zeros-buffer

  // set-up DMA descriptors / 1 pair per dither-factor
  i2s_pixels_queue2.owner     = 1;
  i2s_pixels_queue2.eof       = 1;
  i2s_pixels_queue2.sub_sof   = 0;
  i2s_pixels_queue2.datalen   = NUM_I2S_PIXEL_BYTES2;     // Size in bytes
  i2s_pixels_queue2.blocksize = NUM_I2S_PIXEL_BYTES2;     // Size in bytes
  i2s_pixels_queue2.buf_ptr   = (uint32_t)i2s_pixels_buffer2;
  i2s_pixels_queue2.unused    = 0;
  i2s_pixels_queue2.next_link_ptr = (uint32_t)&i2s_pixels_queue1;  // always link to zeros-buffer
  
  // call C based helper function
  // I did not really succeed in putting the code from the helper
  // funtion directly into this constructor...has something to do 
  // with  C vs. C++
  // the i2c_writeReg_Mask macro (+ others) failed.. see ws2812_defs.h
  // may be I solve this later

  // parameter = first entry in DMA descriptor list, i.e the descriptor list
  ws2812_dma(&i2s_pixels_queue1);

} 

// left this comment in tact....credit to Charles

// All functions below this line are Public Domain 2015 Charles Lohr.
// this code may be used by anyone in any way without restriction or limitation.
// BIT0 - 001111
// BIT1 - 0001111111

// display the pixels
void Ws2812_Show(void)
{
  // led data adress
  uint8_t   *buffer;
  uint16_t  a,b,c,k;
  uint32_t  *i2s_word_ptr;
  uint8_t   byte_cnt;
  uint8_t   first;

  if (UpdateFlag == true)   return;

  
  for (a = 0;  a < LED_TOTAL; a++)
  {
  	if (LedData[a].DutyR == 0xff)	LedData[a].DutyR = 0xf0;
  	if (LedData[a].DutyG == 0xff)	LedData[a].DutyG = 0xf0;
  	if (LedData[a].DutyB == 0xff)	LedData[a].DutyB = 0xf0;
  	
    LedDrvData[a].DutyR = LedData[a].DutyR / BrightLevel;
    LedDrvData[a].DutyG = LedData[a].DutyG / BrightLevel;
    LedDrvData[a].DutyB = LedData[a].DutyB / BrightLevel;
  }
  
  buffer = (uint8_t *)LedDrvData;
  byte_cnt = 0;
  first = 0;
  i2s_word_ptr = i2s_buffer1;
  *i2s_word_ptr = 0;
  
  

  //first shift data 0x55 + 0xa  ==> save 0x55a or 0x556 to buffer
  //Notication : ESP8266 is Little-Endian
  k = 0x55a;
  for (a = 0; a < 12; a++)
  {

    //00
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    
    //0111
    if((k & 0x800) == 0x800){
      i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
      *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
      i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
      *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
      i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
      *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
      i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    }

    //1111
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    k <<= 1;
  }
 
  
  // save the bytes to word buffer follow the below rule:
  // 4 bytes = 1 word
  // Low - - - High
  // 0   1  2   3
  // B4  B3 B2  B1
  
  //shift the RGB data to buffer
  for (a = 0; a < LED_TOTAL; a++){
    //m1 - high byte, m2 - low byte
    for (b = 0; b < 3; b++){
      k = *buffer++;

      for (c = 0; c < 8; c++){
        //00
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        
        //0111
        if((k & 0x80) == 0x80){
          i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
          *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
          i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
          *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
          i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
          *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
          i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        }

        //11
        *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        k <<= 1;
      }
    }

    //the 25 bit - ensure bit
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
  }

  buffer = (uint8_t *)LedDrvData;
  bool  brkflag = false;
  while (!brkflag)
  {
    //m1 - high byte, m2 - low byte
    for (b = 0; b < 3; b++)
    {
      k = *buffer++;

      for (c = 0; c < 8; c++)
      {
        //0011
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
        i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
        k <<= 1;
      }
    }

    //the 25 bit - ensure bit
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    i2s_word_ptr = Word_Ptr_Get(&first, &byte_cnt, i2s_word_ptr);
    if ((first == 1) && (i2s_word_ptr >= (i2s_buffer2 + NUM_I2S_PIXEL_WORDS2 - 10)))   brkflag = true;
  }

  while(byte_cnt < 32)
  {
    *i2s_word_ptr += (0x00000001 << (31 - byte_cnt));
    byte_cnt++;
  }

  
  i2s_word_ptr++;
  while(i2s_word_ptr < i2s_buffer2 + NUM_I2S_PIXEL_WORDS2)  *i2s_word_ptr++ = 0xffffffff;

  UpdateFlag = TRUE;
}


/**
  * FunctionName  Word_Ptr_Get
  */
uint32_t *Word_Ptr_Get(uint8_t *fst, uint8_t *bitcnt, uint32_t *ptr)
{
  (*bitcnt)++;
  if ((*bitcnt) >= 32){
    (*bitcnt) = 0;
    ptr++;
    if ((*fst == 0) && ((uint32_t)ptr >= (uint32_t)(i2s_buffer1 + (uint32_t)NUM_I2S_PIXEL_WORDS1))){
      *fst = 1;
      ptr = i2s_buffer2;
    }
    *ptr = 0;
  }
  return ptr;
}


// end of file
