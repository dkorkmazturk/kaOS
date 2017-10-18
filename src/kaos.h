#ifndef KAOS_H
#define KAOS_H

#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"

#define STACKSIZE 100

struct Tcb
{
    uint32_t* sp;
    struct Tcb* next;
    struct Tcb* previous;

    uint32_t stack[100];
};

static Tcb* RunPt = NULL;

static void SysTick_Init(void);
static inline void SysTick_Start(void);
static void SysTick_Handler(void);
static void PendSV_Handler(void) __attribute__ ( ( isr, naked ) );

void kaOS_Init(void);
void kaOS_Start(void);
int8_t kaOS_AddThead(void(*thread)(void));

#endif // KAOS_H
