/*
 * uart.c
 *
 * This file contains definitions and helper functions of UART driver
 */

#include "uart.h"
#include "gpio.h"
#include "kaOS.h"

void UART_Init(const enum UART uart, const uint32_t bps)
{
    static sem_t UARTMutex = {1, 0};
    const float ibrd = 16000000 / (16.0f * bps);

    // Critical section because another thread may change following memory locations concurrently
    kaOS_SemWait(&UARTMutex);
    if(uart == UART_0)
    {
        // Enable clock of GPIO Port A and UART0
        GPIO_Init(GPIO_PORT_A);
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;

        GPIO_PORTA_AMSEL_R &= ~0x3;     // Disable analog functions of PA0 and PA1
        GPIO_PORTA_AFSEL_R |= 0x3;      // Enable alternate functions of PA0 and PA1
        GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0xFFFFFF11;    // Set PA0 as Rx and PA1 as Tx
        GPIO_PORTA_DEN_R |= 0x3;        // Enable digital functions of PA0 and PA1
    }

    else if(uart == UART_1)
    {
        // Enable clock of GPIO Port B and UART1
        GPIO_Init(GPIO_PORT_B);
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;

        GPIO_PORTB_AMSEL_R &= ~0x3;     // Disable analog functions of PB0 and PB1
        GPIO_PORTB_AFSEL_R |= 0x3;      // Enable alternate functions of PB0 and PB1
        GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) |  0xFFFFFF11;    // Set PB0 as Rx and PB1 as Tx
        GPIO_PORTB_DEN_R |= 0x3;        // Enable digital functions of PB0 and PB1
    }

    else if(uart == UART_2)
    {
        // Enable clock of GPIO Port D and UART2
        GPIO_Init(GPIO_PORT_D);
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R2;

        GPIO_PORTD_AMSEL_R &= ~0xC0;    // Disable analog functions of PD6 and PD7
        GPIO_PORTD_AFSEL_R |= 0xC0;     // Enable alternate functions of PD6 and PD7
        GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0x00FFFFFF) | 0x11FFFFFF;    // Set PD6 as Rx and PD7 as Tx
        GPIO_PORTD_DEN_R |= 0xC0;       // Enable digital functions of PD6 and PD7
    }

    else if(uart == UART_3)
    {
        // Enable clock of GPIO Port C and UART3
        GPIO_Init(GPIO_PORT_C);
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R3;

        GPIO_PORTC_AMSEL_R &= ~0xC0;    // Disable analog functions of PC6 and PC7
        GPIO_PORTC_AFSEL_R |= 0xC0;     // Enable alternate functions of PC6 and PC7
        GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & 0x00FFFFFF) | 0x11FFFFFF;    // Set PC6 as Rx and PC7 as Tx
        GPIO_PORTC_DEN_R |= 0xC0;       //  Enable digital functions of PC6 and PC7
    }

    else if(uart == UART_4)
    {
        // Enable clock of GPIO Port C and UART4
        GPIO_Init(GPIO_PORT_C);
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R4;

        GPIO_PORTC_AMSEL_R &= ~0x30;    // Disable analog functions of PC4 and PC5
        GPIO_PORTC_AFSEL_R |= 0x30;     // Enable alternate functions of PC4 and PC5
        GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & 0xFF00FFFF) | 0xFF11FFFF;    // Set PC4 as Rx and PC5 as Tx
        GPIO_PORTC_DEN_R |= 0x30;       // Enable digital functions of PC4 and PC5
    }

    else if(uart == UART_5)
    {
        // Enable clock of GPIO Port E and UART5
        GPIO_Init(GPIO_PORT_E);
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R5;

        GPIO_PORTE_AMSEL_R &= ~0x30;    // Disable analog functions of PE4 and PE5
        GPIO_PORTE_AFSEL_R |= 0x30;     // Enable alternate functions of PE4 and PE5
        GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFF00FFFF) | 0xFF11FFFF;    // Set PE4 as Rx and PE5 as Tx
        GPIO_PORTE_DEN_R |= 0x30;       // Enable digital functions of PE4 and PE5
    }

    else if(uart == UART_6)
    {
        // Enable clock of GPIO Port D and UART6
        GPIO_Init(GPIO_PORT_D);
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R6;

        GPIO_PORTD_AMSEL_R &= ~0x30;    // Disable analog functions of PD4 and PD5
        GPIO_PORTD_AFSEL_R |= 0x30;     // Enable alternate functions of PD4 and PD5
        GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFF00FFFF) | 0xFF11FFFF;    // Set PD4 as Rx and PD5 as Tx
        GPIO_PORTD_DEN_R |= 0x30;       // Enable digital functions of PD4 and PD5
    }

    else
    {
        // Enable clock of GPIO Port E and UART7
        GPIO_Init(GPIO_PORT_E);
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R7;

        GPIO_PORTE_AMSEL_R &= ~0x3;     // Disable analog functions of PE0 and PE1
        GPIO_PORTE_AFSEL_R |= 0x3;      // Enable alternate functions of PE0 and PE1
        GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFFFFFF00) | 0xFFFFFF11;    // Set PE0 as Rx and PE1 as Tx
        GPIO_PORTE_DEN_R |= 0x3;            // Enable digital functions of PE0 and PE1
    }
    __asm__ volatile("CPSIE I");

    *((&UART0_CTL_R) + uart) &= ~UART_CTL_UARTEN;   // Disable UART

    *((&UART0_IBRD_R) + uart) = ibrd;   // Set specified baud rate
    *((&UART0_FBRD_R) + uart) = (ibrd - (uint32_t)ibrd) * 64 + 0.5f;
    *((&UART0_LCRH_R) + uart) = 0x70;   // 8-bit data, 1 stop bit, FIFOs enabled
    *((&UART0_CC_R) + uart) = 0x5;      // Use PIOSC as clock source

    *((&UART0_CTL_R) + uart) |= UART_CTL_UARTEN;    // Enable UART
    kaOS_SemSignal(&UARTMutex);
}

void UART_SendChar(const enum UART uart, const char data)
{
    while((*((&UART0_FR_R) + uart)) & UART_FR_TXFF);   // If TX FIFO is full, busy-wait
    *((&UART0_DR_R) + uart) = data;                    // Write data to TX FIFO
}

void UART_SendBuf(const enum UART uart, const char* data, const uint32_t buflen)
{
    for(uint32_t i = 0; i < buflen; ++i)
        UART_SendChar(uart, *(data++));
}

void UART_SendStr(const enum UART uart, const char* str)
{
    while(*str)
        UART_SendChar(uart, *(str++));
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
    for(uint32_t i = 0; i < strlen; ++i)
    {
        while((*((&UART0_FR_R) + uart)) & UART_FR_RXFE);
        *(str + i) = *((&UART0_DR_R) + uart) & 0xFF;
    }
}

void UART_RecvStr(const enum UART uart, char *str, const uint32_t strlen)
{
    UART_RecvBuf(uart, str, strlen);
    *(str + strlen) = '\0';
}

void UART_End(const enum UART uart)
{
    // Disable UART clock and use dedicated pins as GPIO
    __asm__ volatile("CPSID I");
    if(uart == UART_0)
    {
        GPIO_PORTA_AFSEL_R &= ~0x3;      // Disable alternate functions of PA0 and PA1
        GPIO_PORTA_PCTL_R &= 0xFFFFFF00;    // Clear alternate function selection

        SYSCTL_RCGCUART_R &= ~SYSCTL_RCGCUART_R0; // Disable clock of UART0
    }

    else if(uart == UART_1)
    {
        GPIO_PORTB_AFSEL_R &= ~0x3;      // Disable alternate functions of PB0 and PB1
        GPIO_PORTB_PCTL_R &= 0xFFFFFF00;    // Clear alternate function selection

        SYSCTL_RCGCUART_R &= ~SYSCTL_RCGCUART_R1; // Disable clock of UART1
    }

    else if(uart == UART_2)
    {
        GPIO_PORTD_AFSEL_R &= ~0xC0;     // Disable alternate functions of PD6 and PD7
        GPIO_PORTD_PCTL_R &= 0x00FFFFFF;    // Clear alternate function selection

        GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY;
        GPIO_PORTD_CR_R = 0;
        GPIO_PORTD_LOCK_R = 1;

        SYSCTL_RCGCUART_R &= ~SYSCTL_RCGCUART_R2; // Disable clock of UART2
    }

    else if(uart == UART_3)
    {
        GPIO_PORTC_AFSEL_R &= ~0xC0;     // Disable alternate functions of PC6 and PC7
        GPIO_PORTC_PCTL_R &= 0x00FFFFFF;    // Clear alternate function selection

        SYSCTL_RCGCUART_R &= ~SYSCTL_RCGCUART_R3; // Disable clock of UART3
    }

    else if(uart == UART_4)
    {
        GPIO_PORTC_AFSEL_R &= ~0x30;     // Disable alternate functions of PC4 and PC5
        GPIO_PORTC_PCTL_R &= 0xFF00FFFF;    // Clear alternate function selection

        SYSCTL_RCGCUART_R &= ~SYSCTL_RCGCUART_R4; // Disable clock of UART4
    }

    else if(uart == UART_5)
    {
        GPIO_PORTE_AFSEL_R &= ~0x30;     // Disable alternate functions of PE4 and PE5
        GPIO_PORTE_PCTL_R &= 0xFF00FFFF;    // Clear alternate function selection

        SYSCTL_RCGCUART_R &= ~SYSCTL_RCGCUART_R5; // Disable clock of UART5
    }

    else if(uart == UART_6)
    {
        GPIO_PORTD_AFSEL_R &= ~0x30;     // Disable alternate functions of PD4 and PD5
        GPIO_PORTD_PCTL_R &= 0xFF00FFFF;    // Clear alternate function selection

        SYSCTL_RCGCUART_R &= ~SYSCTL_RCGCUART_R6; // Disable clock of UART6
    }

    else
    {
        GPIO_PORTE_AFSEL_R &= ~0x3;      // Disable alternate functions of PE0 and PE1
        GPIO_PORTE_PCTL_R &= 0xFFFFFF00;    // Clear alternate function selection

        SYSCTL_RCGCUART_R &= ~SYSCTL_RCGCUART_R7; // Disable clock of UART7
    }
    __asm__ volatile("CPSIE I");
}
