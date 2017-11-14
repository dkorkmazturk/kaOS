/*
 * uart.c
 *
 * This file contains definitions and helper functions of UART driver
 */

#include "uart.h"
#include "gpio.h"
#include "../kaOS.h"

extern sem_t gpioMutex[6];
static sem_t UARTMutex[] = {{1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}};    // These are used while setting up or using specific UART module
static sem_t UARTClockMutex = {1, 0};

void UART_Init(const enum UART uart, const uint32_t bps)
{
    const float ibrd = kaOS_GetSysClock() / (16.0f * bps);

    kaOS_SemWait(&UARTClockMutex);
    SYSCTL_RCGCUART_R |= 1 << (uart / 0x400);   // Enable clock of specified UART module
    kaOS_SemSignal(&UARTClockMutex);

    // Critical section because another thread may change following memory locations concurrently
    kaOS_SemWait(&UARTMutex[uart / 0x400]);
    if(uart == UART_0)
    {
        GPIO_Init(GPIO_PORT_A); // Enable clock of GPIO Port A

        kaOS_SemWait(&gpioMutex[0]);
        GPIO_PORTA_AMSEL_R &= ~0x03;     // Disable analog functions of PA0 and PA1
        GPIO_PORTA_AFSEL_R |= 0x03;      // Enable alternate functions of PA0 and PA1
        GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011;    // Set PA0 as Rx and PA1 as Tx
        GPIO_PORTA_DEN_R |= 0x03;        // Enable digital functions of PA0 and PA1
        kaOS_SemSignal(&gpioMutex[0]);
    }

    else if(uart == UART_1)
    {
        GPIO_Init(GPIO_PORT_B); // Enable clock of GPIO Port B

        kaOS_SemWait(&gpioMutex[1]);
        GPIO_PORTB_AMSEL_R &= ~0x03;     // Disable analog functions of PB0 and PB1
        GPIO_PORTB_AFSEL_R |= 0x03;      // Enable alternate functions of PB0 and PB1
        GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) |  0x00000011;    // Set PB0 as Rx and PB1 as Tx
        GPIO_PORTB_DEN_R |= 0x03;        // Enable digital functions of PB0 and PB1
        kaOS_SemSignal(&gpioMutex[1]);
    }

    else if(uart == UART_2)
    {
        GPIO_Init(GPIO_PORT_D); // Enable clock of GPIO Port D

        kaOS_SemWait(&gpioMutex[3]);
        GPIO_PORTD_AMSEL_R &= ~0xC0;    // Disable analog functions of PD6 and PD7
        GPIO_PORTD_AFSEL_R |= 0xC0;     // Enable alternate functions of PD6 and PD7
        GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0x00FFFFFF) | 0x11000000;    // Set PD6 as Rx and PD7 as Tx
        GPIO_PORTD_DEN_R |= 0xC0;       // Enable digital functions of PD6 and PD7
        kaOS_SemSignal(&gpioMutex[3]);
    }

    else if(uart == UART_3)
    {
        GPIO_Init(GPIO_PORT_C); // Enable clock of GPIO Port C

        kaOS_SemWait(&gpioMutex[2]);
        GPIO_PORTC_AMSEL_R &= ~0xC0;    // Disable analog functions of PC6 and PC7
        GPIO_PORTC_AFSEL_R |= 0xC0;     // Enable alternate functions of PC6 and PC7
        GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & 0x00FFFFFF) | 0x11000000;    // Set PC6 as Rx and PC7 as Tx
        GPIO_PORTC_DEN_R |= 0xC0;       //  Enable digital functions of PC6 and PC7
        kaOS_SemSignal(&gpioMutex[2]);
    }

    else if(uart == UART_4)
    {
        GPIO_Init(GPIO_PORT_C); // Enable clock of GPIO Port C

        kaOS_SemWait(&gpioMutex[2]);
        GPIO_PORTC_AMSEL_R &= ~0x30;    // Disable analog functions of PC4 and PC5
        GPIO_PORTC_AFSEL_R |= 0x30;     // Enable alternate functions of PC4 and PC5
        GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & 0xFF00FFFF) | 0x00110000;    // Set PC4 as Rx and PC5 as Tx
        GPIO_PORTC_DEN_R |= 0x30;       // Enable digital functions of PC4 and PC5
        kaOS_SemSignal(&gpioMutex[2]);
    }

    else if(uart == UART_5)
    {
        GPIO_Init(GPIO_PORT_E); // Enable clock of GPIO Port E

        kaOS_SemWait(&gpioMutex[4]);
        GPIO_PORTE_AMSEL_R &= ~0x30;    // Disable analog functions of PE4 and PE5
        GPIO_PORTE_AFSEL_R |= 0x30;     // Enable alternate functions of PE4 and PE5
        GPIO_PORTE_ODR_R &= ~0x30;      // Disable open drain for PE4 and PE5 (Might be enabled by I2C_2)
        GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFF00FFFF) | 0x00110000;    // Set PE4 as Rx and PE5 as Tx
        GPIO_PORTE_DEN_R |= 0x30;       // Enable digital functions of PE4 and PE5
        kaOS_SemSignal(&gpioMutex[4]);
    }

    else if(uart == UART_6)
    {
        GPIO_Init(GPIO_PORT_D); // Enable clock of GPIO Port D

        kaOS_SemWait(&gpioMutex[3]);
        GPIO_PORTD_AMSEL_R &= ~0x30;    // Disable analog functions of PD4 and PD5
        GPIO_PORTD_AFSEL_R |= 0x30;     // Enable alternate functions of PD4 and PD5
        GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFF00FFFF) | 0x00110000;    // Set PD4 as Rx and PD5 as Tx
        GPIO_PORTD_DEN_R |= 0x30;       // Enable digital functions of PD4 and PD5
        kaOS_SemSignal(&gpioMutex[3]);
    }

    else
    {
        GPIO_Init(GPIO_PORT_E); // Enable clock of GPIO Port E

        kaOS_SemWait(&gpioMutex[4]);
        GPIO_PORTE_AMSEL_R &= ~0x3;     // Disable analog functions of PE0 and PE1
        GPIO_PORTE_AFSEL_R |= 0x3;      // Enable alternate functions of PE0 and PE1
        GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFFFFFF00) | 0x00000011;    // Set PE0 as Rx and PE1 as Tx
        GPIO_PORTE_DEN_R |= 0x3;            // Enable digital functions of PE0 and PE1
        kaOS_SemSignal(&gpioMutex[4]);
    }

    kaOS_SemSignal(&UARTMutex[uart / 0x400]);

    *((&UART0_CTL_R) + uart) &= ~UART_CTL_UARTEN;   // Disable UART

    *((&UART0_IBRD_R) + uart) = ibrd;   // Set specified baud rate
    *((&UART0_FBRD_R) + uart) = (ibrd - (uint32_t)ibrd) * 64 + 0.5f;
    *((&UART0_LCRH_R) + uart) = 0x70;   // 8-bit data, 1 stop bit, FIFOs enabled
    *((&UART0_CC_R) + uart) = 0x0;      // Use SysClock as clock source

    *((&UART0_CTL_R) + uart) |= UART_CTL_UARTEN;    // Enable UART
}

static inline void UART_SendChar_Nonblock(const enum UART uart, const char data)
{
    while((*((&UART0_FR_R) + uart)) & UART_FR_TXFF);   // If TX FIFO is full, busy-wait
    *((&UART0_DR_R) + uart) = data;                    // Write data to TX FIFO
}

void UART_SendChar(const enum UART uart, const char data)
{
    kaOS_SemWait(&UARTMutex[uart / 0x400]);
    while((*((&UART0_FR_R) + uart)) & UART_FR_TXFF);   // If TX FIFO is full, busy-wait
    *((&UART0_DR_R) + uart) = data;                    // Write data to TX FIFO
    kaOS_SemSignal(&UARTMutex[uart / 0x400]);
}

void UART_SendBuf(const enum UART uart, const char* data, const uint32_t buflen)
{
    kaOS_SemWait(&UARTMutex[uart / 0x400]);
    for(uint32_t i = 0; i < buflen; ++i)
        UART_SendChar_Nonblock(uart, *(data++));
    kaOS_SemSignal(&UARTMutex[uart / 0x400]);
}

void UART_SendStr(const enum UART uart, const char* str)
{
    kaOS_SemWait(&UARTMutex[uart / 0x400]);
    while(*str)
        UART_SendChar_Nonblock(uart, *(str++));
    kaOS_SemSignal(&UARTMutex[uart / 0x400]);
}

char UART_RecvChar(const enum UART uart)
{
    if((*((&UART0_FR_R) + uart)) & UART_FR_RXFE)   // If RX FIFO is not empty
        return *((&UART0_DR_R) + uart) & 0xFF;
    else
        return '\0';
}

void UART_RecvBuf(const enum UART uart, char *str, const uint32_t strlen)
{
    kaOS_SemWait(&UARTMutex[uart / 0x400]);
    for(uint32_t i = 0; i < strlen; ++i)
    {
        while((*((&UART0_FR_R) + uart)) & UART_FR_RXFE);
        *(str + i) = *((&UART0_DR_R) + uart) & 0xFF;
    }
    kaOS_SemSignal(&UARTMutex[uart / 0x400]);
}

void UART_RecvStr(const enum UART uart, char *str, const uint32_t strlen)
{
    UART_RecvBuf(uart, str, strlen);
    *(str + strlen) = '\0';
}

void UART_End(const enum UART uart)
{
    // Disable UART clock and use dedicated pins as GPIO
    kaOS_SemWait(&UARTMutex[uart / 0x400]);

    if(uart == UART_0)
    {
        kaOS_SemWait(&gpioMutex[0]);
        GPIO_PORTA_AFSEL_R &= ~0x3;      // Disable alternate functions of PA0 and PA1
        GPIO_PORTA_PCTL_R &= 0xFFFFFF00;    // Clear alternate function selection
        kaOS_SemSignal(&gpioMutex[0]);
    }

    else if(uart == UART_1)
    {
        kaOS_SemWait(&gpioMutex[1]);
        GPIO_PORTB_AFSEL_R &= ~0x3;      // Disable alternate functions of PB0 and PB1
        GPIO_PORTB_PCTL_R &= 0xFFFFFF00;    // Clear alternate function selection
        kaOS_SemSignal(&gpioMutex[1]);
    }

    else if(uart == UART_2)
    {
        kaOS_SemWait(&gpioMutex[3]);
        GPIO_PORTD_AFSEL_R &= ~0xC0;     // Disable alternate functions of PD6 and PD7
        GPIO_PORTD_PCTL_R &= 0x00FFFFFF;    // Clear alternate function selection
        kaOS_SemSignal(&gpioMutex[3]);
    }

    else if(uart == UART_3)
    {
        kaOS_SemWait(&gpioMutex[2]);
        GPIO_PORTC_AFSEL_R &= ~0xC0;     // Disable alternate functions of PC6 and PC7
        GPIO_PORTC_PCTL_R &= 0x00FFFFFF;    // Clear alternate function selection
        kaOS_SemSignal(&gpioMutex[2]);
    }

    else if(uart == UART_4)
    {
        kaOS_SemWait(&gpioMutex[2]);
        GPIO_PORTC_AFSEL_R &= ~0x30;     // Disable alternate functions of PC4 and PC5
        GPIO_PORTC_PCTL_R &= 0xFF00FFFF;    // Clear alternate function selection
        kaOS_SemSignal(&gpioMutex[2]);
    }

    else if(uart == UART_5)
    {
        kaOS_SemWait(&gpioMutex[4]);
        GPIO_PORTE_AFSEL_R &= ~0x30;     // Disable alternate functions of PE4 and PE5
        GPIO_PORTE_PCTL_R &= 0xFF00FFFF;    // Clear alternate function selection
        kaOS_SemSignal(&gpioMutex[4]);
    }

    else if(uart == UART_6)
    {
        kaOS_SemWait(&gpioMutex[3]);
        GPIO_PORTD_AFSEL_R &= ~0x30;     // Disable alternate functions of PD4 and PD5
        GPIO_PORTD_PCTL_R &= 0xFF00FFFF;    // Clear alternate function selection
        kaOS_SemSignal(&gpioMutex[3]);
    }

    else
    {
        kaOS_SemWait(&gpioMutex[4]);
        GPIO_PORTE_AFSEL_R &= ~0x3;      // Disable alternate functions of PE0 and PE1
        GPIO_PORTE_PCTL_R &= 0xFFFFFF00;    // Clear alternate function selection
        kaOS_SemSignal(&gpioMutex[4]);
    }

    kaOS_SemWait(&UARTClockMutex);
    SYSCTL_RCGCUART_R &= ~(1 << (uart / 0x400)); // Disable clock of UART0
    kaOS_SemSignal(&UARTClockMutex);

    kaOS_SemSignal(&UARTMutex[uart / 0x400]);
}
