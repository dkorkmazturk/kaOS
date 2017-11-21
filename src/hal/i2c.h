/*
 * i2c.h
 *
 * This file contains the declarations of I2C driver functions
 */

#ifndef HAL_I2C_H_
#define HAL_I2C_H_

#include "../types.h"

void I2C_InitAsMaster(const enum I2C i2c, const enum I2C_Clock clock);
bool I2C_SendChar(const enum I2C i2c, const uint8_t slaveAddr, const uint8_t data);
bool I2C_SendBuf(const enum I2C i2c, const uint8_t slaveAddr, const uint8_t* data, uint32_t dataLen);
bool I2C_SendStr(const enum I2C i2c, const uint8_t slaveAddr, const char* const str);
void I2C_Request(const enum I2C i2c, const uint8_t slaveAddr, uint32_t quantity, uint8_t* readBuf);

#endif
