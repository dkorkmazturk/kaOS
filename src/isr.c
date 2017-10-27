/*
 * isr.c
 *
 * This file contains definitions and helper functions of ISRs
 */
#include "isr.h"
#include "types.h"
#include "semaphore.h"

extern tcb_t* RunPt;
extern tcb_t* WaitPt;

void SysTick_Handler(void)
{
    // SysTick timer interrupt handler
    // Priority 2

    //Watchdog_Timer0_Restart();
    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
}

void Scheduler(void) {
    if(RunPt != 0 && RunPt->next == 0)
        RunPt = 0;

    while(RunPt == 0)
        __asm__ volatile("CPSIE I\n\tWFI\n\tCPSID I");

    RunPt = RunPt->next;
}

void Timer0_Handler(void)
{
    // Timer0 interrupt handler
    // Priority 2

    TIMER0_ICR_R = 1;   // Acknowledge the interrupt

    tcb_t* waitingNodePrev = 0;
    tcb_t* waitingNode = WaitPt;

    while(waitingNode != 0)
    {
        --(waitingNode->waitTime);

        if(waitingNode->waitTime == 0)
        {
            if(waitingNode != WaitPt)
                waitingNodePrev->previous = waitingNode->previous;
            else
            {
                WaitPt = WaitPt->previous;
                if(WaitPt == 0)
                {
                    TIMER0_CTL_R &= ~1; // If there is no remaining process that is waiting, it is pointless to use timer, so disable it
                    TIMER0_TAV_R = TIMER0_TAILR_R;
                }
            }

            if(RunPt == 0)
            {
                RunPt = waitingNode;
                RunPt->next = RunPt;
                RunPt->previous = RunPt;
            }
            else
            {
                waitingNode->next = RunPt->next;
                waitingNode->previous = RunPt;
                RunPt->next = waitingNode;
                waitingNode->next->previous = waitingNode;
            }

            if(waitingNodePrev != 0)
                waitingNode = waitingNodePrev->previous;
            else
                waitingNode = WaitPt;
        }
        else
        {
            waitingNodePrev = waitingNode;
            waitingNode = waitingNode->previous;
        }
    }
}

static void thread_sleep(const uint32_t ms)
{
    __asm__ volatile("CPSID I");
    if(RunPt == RunPt->next)
        RunPt->next = 0;
    else
    {
        RunPt->previous->next = RunPt->next;
        RunPt->next->previous = RunPt->previous;
    }

    RunPt->previous = 0;
    RunPt->waitTime = ms;

    if(WaitPt == 0)
    {
        WaitPt = RunPt;
        TIMER0_CTL_R |= 1;   // Start Timer0
    }
    else
    {
        tcb_t* node = WaitPt;

        while(node->previous != 0)
            node = node->previous;

        node->previous = RunPt;
    }

    NVIC_ST_CURRENT_R = NVIC_ST_RELOAD_R;
    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
    __asm__ volatile("CPSIE I");
}

void SVCaller(void *param0, void* param1, void* param2, const uint8_t SVCall_Number) {
    if(SVCall_Number == 0)
        sem_init((sem_t*)param0, (int32_t)param1);
    else if(SVCall_Number == 1)
        sem_signal((sem_t*)param0);
    else if(SVCall_Number == 2)
        sem_wait((sem_t*)param0);
    else if(SVCall_Number == 3) {
        __asm__ volatile("CPSID I");
        NVIC_ST_CURRENT_R = NVIC_ST_RELOAD_R;
        NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
        __asm__ volatile("CPSIE I");
    }
    else if(SVCall_Number == 4)
        thread_sleep((uint32_t)param0);
}
