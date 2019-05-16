/*******************************************************************
  *
  * file name: mcu_com.c
  * brief: communicate with mcu
  *
********************************************************************/
#include "mcu_com.h"
#include <string.h>


//file parameters
uart_t        *uart0 = NULL;

/** Uart_Init **/
void Uart_Init(void)
{
  //Initialize the uart0
  uart0 = uart_init(UART0, UART0_BAUDRATE, UART_8N1, UART_FULL, 1, UART0_RCV_BUFFER_MAX);
  uart_set_pins(uart0, 15, 13);
}






