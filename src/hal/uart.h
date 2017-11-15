/*
 * uart.h
 *
 * This file contains the declarations of UART driver functions
 */

#ifndef HAL_UART_H_
#define HAL_UART_H_

#include "../types.h"

void UART_Init(const enum UART uart, const uint32_t bps);
void UART_SendChar(const enum UART uart, const char data);
void UART_SendBuf(const enum UART uart, const char* data, const uint32_t buflen);
void UART_SendStr(const enum UART uart, const char* str);
char UART_RecvChar(const enum UART uart);
void UART_RecvBuf(const enum UART uart, char *str, const uint32_t strlen);
void UART_RecvStr(const enum UART uart, char *str, const uint32_t strlen);
void UART_End(const enum UART uart);

#endif
