#include "semaphore.h"

extern tcb_t* RunPt;

void sem_init(sem_t* const sem, const int32_t val)
{
    __asm__ volatile("CPSID I");

    sem->sem = val;
    sem->blockedQueue = NULL;

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

        node->next = RunPt->next;
        node->previous = RunPt;

        RunPt->next->previous = node;
        RunPt->next = node;
    }

    __asm__ volatile("CPSIE I");
}

void sem_wait(sem_t* const sem)
{
    __asm__ volatile("CPSID I");

    --(sem->sem);

    if(sem->sem < 0)
    {
        RunPt->previous->next = RunPt->next;
        RunPt->next->previous = RunPt->previous;

        RunPt->previous = NULL;

        if(sem->blockedQueue == NULL)
            sem->blockedQueue = RunPt;
        else
        {
            tcb_t* node = sem->blockedQueue;

            while(node->previous != NULL)
                node = node->previous;

            node->previous = RunPt;
        }

        NVIC_ST_CURRENT_R = NVIC_ST_RELOAD_R;
        NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
    }

    __asm__ volatile("CPSIE I");
}
