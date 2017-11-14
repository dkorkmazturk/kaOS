/*
 * gpio.h
 *
 * This file contains the declarations of GPIO driver functions
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "../types.h"

void GPIO_Init(const enum GPIO_PORT port);
void GPIO_SetMode(const enum GPIO_PORT port, const uint32_t pins, const enum GPIO_MODE mode);
void GPIO_Write(const enum GPIO_PORT port, const uint32_t pins, const uint32_t values);
inline uint32_t GPIO_Read(const enum GPIO_PORT port, const uint32_t pins) {return *((&GPIO_PORTA_DATA_R) + port) & pins;}

#endif /* GPIO_H_ */
