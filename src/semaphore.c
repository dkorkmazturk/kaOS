#include "semaphore.h"

extern tcb_t* RunPt;

void sem_init(sem_t* const sem, const int32_t val)
{
    __asm__ volatile("CPSID I");

    sem->sem = val;
    sem->blockedQueue = 0;

    __asm__ volatile("CPSIE I");
}

void sem_signal(sem_t* const sem)
{
    __asm__ volatile("CPSID I");

    ++(sem->sem);

    if(sem->sem <= 0)
    {
        tcb_t* node = sem->blockedQueue;
        sem->blockedQueue = sem->blockedQueue->previous;

        if(RunPt == 0)
        {
            RunPt = node;
            RunPt->previous = RunPt;
            RunPt->next = RunPt;
        }
        else
        {
            node->next = RunPt;
            node->previous = RunPt->previous;

            RunPt->previous->next = node;
            RunPt->previous = node;
        }
    }

    __asm__ volatile("CPSIE I");
}

void sem_wait(sem_t* const sem)
{
    __asm__ volatile("CPSID I");

    --(sem->sem);

    if(sem->sem < 0)
    {
        if(RunPt == RunPt->next)
            RunPt->next = 0;
        else
        {
            RunPt->previous->next = RunPt->next;
            RunPt->next->previous = RunPt->previous;
        }

        RunPt->previous = 0;

        if(sem->blockedQueue == 0)
            sem->blockedQueue = RunPt;
        else
        {
            tcb_t* node = sem->blockedQueue;

            while(node->previous != 0)
                node = node->previous;

            node->previous = RunPt;
        }

        NVIC_ST_CURRENT_R = NVIC_ST_RELOAD_R;
        NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
    }

    __asm__ volatile("CPSIE I");
}
