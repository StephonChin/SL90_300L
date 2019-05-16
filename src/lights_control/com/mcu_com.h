#ifndef _MCU_COM_H_
#define _MCU_COM_H_

#ifdef __cplusplus
extern "C" {
#endif

//include files
#include <stdint.h>
#include <stdbool.h>
#include <user_interface.h>
#include "uart_register.h"
#include "uart.h"

//define
#define     UART0_RCV_BUFFER_MAX        12
#define     UART0_BAUDRATE              9600



//exported functions
void Uart_Init(void);
void print_string(const uint8_t *src);

extern uart_t        *uart0;


#ifdef __cplusplus
}
#endif

#endif
