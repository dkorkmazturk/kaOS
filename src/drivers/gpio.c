/*
 * gpio.c
 *
 * This file contains definitions and helper functions of GPIO driver
 */

#include "gpio.h"
#include "kaOS.h"

sem_t gpioMutex[6] = {{1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}};

static inline uint32_t getMutexIndex(const enum GPIO_PORT port)
{
    return ((port <= GPIO_PORT_D) ? port / 0x400 : 4 + port / 0x8400);
}

void GPIO_Init(const enum GPIO_PORT port)
{
    static sem_t gpioInitMutex = {1, 0};
    kaOS_SemWait(&gpioInitMutex);

    SYSCTL_RCGCGPIO_R |= ((port <= GPIO_PORT_D) ? 1 << (port / 0x400) : 0x10 << (port / 0x8400));      // Activate clock for the port

    if(port == GPIO_PORT_F || port == GPIO_PORT_D)
    {
        *((&GPIO_PORTA_LOCK_R) + port) = GPIO_LOCK_KEY;
        *((&GPIO_PORTA_CR_R) + port) = 0xFF;
    }

    kaOS_SemSignal(&gpioInitMutex);
}

void GPIO_SetMode(const enum GPIO_PORT port, const uint32_t pins, const enum GPIO_MODE mode)
{
    register uint32_t mutexIndex = getMutexIndex(port);
    kaOS_SemWait(&gpioMutex[mutexIndex]);

    if(mode == GPIO_INPUT)
        *((&GPIO_PORTA_DIR_R) + port) &= ~pins;
    else
        *((&GPIO_PORTA_DIR_R) + port) |= pins;

    *((&GPIO_PORTA_DATA_R) + port) &= ~pins;
    *((&GPIO_PORTA_AMSEL_R) + port) &= ~pins;
    *((&GPIO_PORTA_AFSEL_R) + port) &= ~pins;
    *((&GPIO_PORTA_DEN_R) + port) |= pins;

    kaOS_SemSignal(&gpioMutex[mutexIndex]);
}

void GPIO_Write(const enum GPIO_PORT port, const uint32_t pins, const uint32_t values)
{
    register uint32_t mutexIndex = getMutexIndex(port);

    kaOS_SemWait(&gpioMutex[mutexIndex]);
    *((&GPIO_PORTA_DATA_R) + port) = (*((&GPIO_PORTA_DATA_R) + port) & ~pins) | (pins & values);
    kaOS_SemSignal(&gpioMutex[mutexIndex]);
}
