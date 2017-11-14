/*
 * i2c.c
 *
 * This file contains definitions and helper functions of I2C driver
 */

#include "i2c.h"
#include "gpio.h"
#include "../kaOS.h"

extern sem_t gpioMutex[6];
static sem_t I2CMutex[4] = {{1, 0}, {1, 0}, {1, 0}, {1, 0}};    // These are used while setting up or using specific I2C module
static sem_t I2CClockMutex = {1, 0};

void I2C_InitAsMaster(const enum I2C i2c)
{
    kaOS_SemWait(&I2CClockMutex);
    SYSCTL_RCGCI2C_R |= 1 << (i2c / 0x400); // Enable clock of specified I2C module
    kaOS_SemSignal(&I2CClockMutex);

    if(i2c == I2C_0)
    {
        GPIO_Init(GPIO_PORT_B); // Enable clock of GPIO Port B

        kaOS_SemWait(&gpioMutex[1]);
        GPIO_PORTB_AMSEL_R &= ~0x0C;    // Disable analog functions of PB2 and PB3
        GPIO_PORTB_AFSEL_R |= 0x0C;     // Enable alternate functions of PB2 and PB3
        GPIO_PORTB_ODR_R |= 0x08;       // Enable PB3 for open drain operation (SDA)
        GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF00FF) | 0x00003300;  // Set PB2 as SCL and PB3 as SDA
        GPIO_PORTB_DEN_R |= 0x0C;       // Enable digital functions of PB2 and PB3
        kaOS_SemSignal(&gpioMutex[1]);
    }

    else if(i2c == I2C_1)
    {
        GPIO_Init(GPIO_PORT_A);

        kaOS_SemWait(&gpioMutex[0]);
        GPIO_PORTA_AMSEL_R &= ~0xC0;    // Disable analog functions of PA6 and PA7
        GPIO_PORTA_AFSEL_R |= 0xC0;     // Enable alternate functions of PA6 and PA7
        GPIO_PORTA_ODR_R |= 0x80;       // Enable PA7 for open drain operation (SDA)
        GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0x00FFFFFF) | 0x33000000;  // Set PA6 as SCL and PA7 as SDA
        GPIO_PORTA_DEN_R = 0xC0;        // Enable digital functions of PA6 and PA7
        kaOS_SemSignal(&gpioMutex[0]);
    }

    else if(i2c == I2C_2)
    {
        GPIO_Init(GPIO_PORT_E);

        kaOS_SemWait(&gpioMutex[4]);
        GPIO_PORTE_AMSEL_R &= ~0x30;    // Disable analog functions of PE4 and PE5
        GPIO_PORTE_AFSEL_R |= 0x30;     // Enable alternate functions of PE4 and PE5
        GPIO_PORTE_ODR_R |= 0x20;       // Enable PE5 for open drain operation (SDA)
        GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFF00FFFF) | 0x00330000;  // Set PE4 as SCL and PE5 as SDA
        GPIO_PORTE_DEN_R |= 0x30;       // Enable digital functions of PE4 and PE5
        kaOS_SemSignal(&gpioMutex[4]);
    }

    else
    {
        GPIO_Init(GPIO_PORT_D);

        kaOS_SemWait(&gpioMutex[3]);
        GPIO_PORTD_AMSEL_R &= ~0x03;    // Disable analog functions of PD0 and PD1
        GPIO_PORTD_AFSEL_R |= 0x03;     // Enable alternate functions of PD0 and PD1
        GPIO_PORTD_ODR_R |= 0x02;       // Enable PD1 for open drain operation (SDA)
        GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFFFFFF00) | 0x00000033;  // Set PD0 as SCL and PD1 as SDA
        GPIO_PORTD_DEN_R |= 0x03;       // Enable digital functions of PD0 and PD1
        kaOS_SemSignal(&gpioMutex[3]);
    }


    *((&I2C0_MCR_R) + i2c) = I2C_MCR_MFE;
    *((&I2C0_MTPR_R) + i2c) = (kaOS_GetSysClock() / (2 * (6 + 4) * 100000)) - 1;
}

void I2C_BeginTransmission(const enum I2C i2c, const uint8_t slaveAddress)
{
    *((&I2C0_MSA_R) + i2c) = slaveAddress << 1; /* Set receiver slave address and transmit mode */
}

bool I2C_SendChar(const enum I2C i2c, const uint8_t data)
{
    kaOS_SemWait(&I2CMutex[i2c / 0x400]);
    *((&I2C0_MDR_R) + i2c) = data;

    while(((*((&I2C0_MCS_R) + i2c)) & I2C_MCS_BUSBSY) != 0); // If bus is busy, wait

    *((&I2C0_MCS_R) + i2c) = 7;

    while(((*((&I2C0_MCS_R) + i2c)) & I2C_MCS_BUSY) != 0); // Wait while transmitting
    kaOS_SemSignal(&I2CMutex[i2c / 0x400]);

    return ((*((&I2C0_MCS_R) + i2c)) & 0x02);
}

bool I2C_SenBuf(const enum I2C i2c, const uint8_t* data, uint32_t dataLen)
{
    kaOS_SemWait(&I2CMutex[i2c / 0x400]);
    if(dataLen == 1)
        return I2C_SendChar(i2c, *data);

    else if(data != 0)
    {
        *((&I2C0_MDR_R) + i2c) = *(data++);
        --dataLen;

        while(((*((&I2C0_MCS_R) + i2c)) & I2C_MCS_BUSBSY) != 0); // If bus is busy, wait

        *((&I2C0_MCS_R) + i2c) = 3; // I2C_MCS_START | I2C_MCS_RUN

        while(1)
        {
            while(((*((&I2C0_MCS_R) + i2c)) & I2C_MCS_BUSY) != 0); // Wait while transmitting

            if((*((&I2C0_MCS_R) + i2c)) & I2C_MCS_ERROR)     // If there is an error
            {
                if(((*((&I2C0_MCS_R) + i2c)) & I2C_MCS_ARBLST) == 0)
                    *((&I2C0_MCS_R) + i2c) = I2C_MCS_STOP;

                kaOS_SemSignal(&I2CMutex[i2c / 0x400]); // I wish I had std::lock_guard
                return false;
            }

            *((&I2C0_MDR_R) + i2c) = *(data++);
            --dataLen;

            if(dataLen >= 1)
                *((&I2C0_MCS_R) + i2c) = 1;
            else
            {
                *((&I2C0_MCS_R) + i2c) = 5;
                break;
            }
        }

        while(((*((&I2C0_MCS_R) + i2c)) & I2C_MCS_BUSY) != 0); // Wait while transmitting
        kaOS_SemSignal(&I2CMutex[i2c / 0x400]);

        return ((*((&I2C0_MCS_R) + i2c)) & 0x02);
    }

    return true;
}

bool I2C_SendStr(const enum I2C i2c, const char* str)
{
    register uint32_t _strlen = 0;
    register const char* ptr = str;

    while(*ptr != '\0')
    {
        ++ptr;
        ++_strlen;
    }

    return I2C_SenBuf(i2c, (uint8_t*)str, _strlen);
}
