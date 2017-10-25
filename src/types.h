#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define STACKSIZE 116

typedef struct tcb_t
{
    uint32_t* sp;
    struct tcb_t* next;
    struct tcb_t* previous;
    uint32_t waitTime;

    uint32_t stack[STACKSIZE];
} tcb_t;

typedef struct
{
    int32_t sem;
    tcb_t* blockedQueue;
} sem_t;


#endif
