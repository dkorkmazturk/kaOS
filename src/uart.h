/*
 * uart.h
 *
 * This file contains the declarations of UART driver functions
 */

#ifndef UART_H_
#define UART_H_

#include "types.h"

void UART_Init(const enum UART uart, const uint32_t bps);
void UART_SendChar(const enum UART uart, const char data);
void UART_SendString(const enum UART uart, const char* str);

#endif /* UART_H_ */
