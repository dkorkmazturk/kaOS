/*
 * i2c.h
 *
 * This file contains the declarations of I2C driver functions
 */

#ifndef _I2C_H_
#define _I2C_H_

#include "../types.h"

void I2C_InitAsMaster(const enum I2C i2c);
void I2C_BeginTransmission(const enum I2C i2c, const uint8_t slaveAddress);
bool I2C_SendChar(const enum I2C i2c, const uint8_t data);
bool I2C_SenBuf(const enum I2C i2c, const uint8_t* data, uint32_t dataLen);
bool I2C_SendStr(const enum I2C i2c, const char* str);


#endif
