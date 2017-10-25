#ifndef KAOS_H
#define KAOS_H

#include "types.h"

void kaOS_Init(void);
extern void kaOS_Start(void);
int8_t kaOS_AddThead(void(*thread)(void));
void kaOS_SemInit(sem_t* const sem, const int32_t val);
void kaOS_SemSignal(sem_t* const sem);
void kaOS_SemWait(sem_t* const sem);
void kaOS_Suspend(void);
void kaOS_Sleep(const uint32_t ms);

#endif
