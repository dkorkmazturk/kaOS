#ifndef KAOS_H
#define KAOS_H

#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"

#define STACKSIZE 100

typedef struct tcb_t
{
    uint32_t* sp;
    struct tcb_t* next;
    struct tcb_t* previous;

    uint32_t stack[STACKSIZE];
} tcb_t;

typedef struct
{
    int32_t sem;
    tcb_t* blockedQueue;
} sem_t;

void kaOS_Init(void);
extern void kaOS_Start(void);
int8_t kaOS_AddThead(void(*thread)(void));
void kaOS_SemInit(sem_t* const sem, const int32_t val);
void kaOS_SemSignal(sem_t* const sem);
void kaOS_SemWait(sem_t* const sem);

#endif
