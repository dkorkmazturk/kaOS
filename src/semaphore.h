#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "types.h"

void sem_init(sem_t* const sem, const int32_t val);
void sem_signal(sem_t* const sem);
void sem_wait(sem_t* const sem);

#endif
