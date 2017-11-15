/*
 * gpio.h
 *
 * This file contains the declarations of GPIO driver functions
 */

#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_

#include "../types.h"

void GPIO_Init(const enum GPIO_PORT port);
void GPIO_SetMode(const enum GPIO_PORT port, const uint32_t pins, const enum GPIO_MODE mode);
void GPIO_Write(const enum GPIO_PORT port, const uint32_t pins, const uint32_t values);
uint32_t GPIO_Read(const enum GPIO_PORT port, const uint32_t pins);
void GPIO_End(const enum GPIO_PORT port);

#endif
