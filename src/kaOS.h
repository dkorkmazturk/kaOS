#ifndef KAOS_H
#define KAOS_H

#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"

#define STACKSIZE 100

typedef struct Tcb
{
    uint32_t* sp;
    struct Tcb* next;
    struct Tcb* previous;

    uint32_t stack[STACKSIZE];
} Tcb;

void kaOS_Init(void);
void kaOS_Start(void)  __attribute__ ( ( flatted, naked ) );
int8_t kaOS_AddThead(void(*thread)(void));

#endif // KAOS_H
