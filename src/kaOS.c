#include <stdlib.h>
#include "kaos.h"
#include "semaphore.h"

tcb_t* RunPt = NULL;
tcb_t* WaitPt = 0;

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
}

static void Timer0_Init(const uint32_t period)
{
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;

    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x20000000;

    TIMER0_CTL_R = 0;   // Disable Timer0
    TIMER0_CFG_R = 0;   // 32-bit timer
    TIMER0_TAMR_R = 0x2;    // Periodic-mode
    TIMER0_TAILR_R = period;    // Period
    TIMER0_IMR_R = 1;   // Timeouts will trigger interrupt

    NVIC_EN0_R |= (1 << 19);    // Enable 19th IRQ
}

void Timer0_Handler(void)
{
    // Timer0 interrupt handler
    // Priority 2

    TIMER0_ICR_R = 1;   // Acknowledge the interrupt

    tcb_t* waitingNodePrev = NULL;
    tcb_t* waitingNode = WaitPt;

    while(waitingNode != NULL)
    {
        --(waitingNode->waitTime);

        if(waitingNode->waitTime == 0)
        {
            if(waitingNode != WaitPt)
                waitingNodePrev->previous = waitingNode->previous;
            else
            {
                WaitPt = WaitPt->previous;
                if(WaitPt == NULL)
                {
                    TIMER0_CTL_R &= ~1; // If there is no remaining process that is waiting, it is pointless to use timer, so disable it
                    TIMER0_TAV_R = TIMER0_TAILR_R;
                }
            }

            waitingNode->next = RunPt->next;
            waitingNode->previous = RunPt;
            RunPt->next = waitingNode;
            waitingNode->next->previous = waitingNode;

            if(waitingNodePrev != NULL)
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

static void Watchdog_Timer0_Init(void)
{
    SYSCTL_RCGCWD_R |= SYSCTL_RCGCWD_R0;
    __asm__ volatile("NOP\n\tNOP\n\tNOP\n\tNOP");
    WATCHDOG0_LOAD_R = 0x1000060;
    WATCHDOG0_CTL_R |= WDT_CTL_RESEN;
}

static inline void Watchdog_Timer0_Start(void)
{
    WATCHDOG0_CTL_R |= WDT_CTL_INTEN;
}

static inline void Watchdog_Timer0_Restart(void)
{
    WATCHDOG0_LOAD_R = 0x1000060;
}

static void SysTick_Init(void)
{
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 0x00FFFFFF;
    NVIC_ST_CURRENT_R = 0;
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000;
}

void SysTick_Start(void)
{
    NVIC_ST_CTRL_R = 0x07;
}

void SysTick_Handler(void)
{
    // SysTick timer interrupt handler
    // Priority 2

    Watchdog_Timer0_Restart();
    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
}

void Scheduler(void) {
    if(RunPt == NULL)
        __asm__ volatile("WFI");

    RunPt = RunPt->next;
}

void kaOS_Init(void)
{
    SysTick_Init();
    Watchdog_Timer0_Init();
    Timer0_Init(0x00FFFFFF);
}

int8_t kaOS_AddThead(void (*thread)(void))
{
    tcb_t* newThread = malloc(sizeof(tcb_t));

    if(newThread == NULL)
        return -1;

    newThread->stack[STACKSIZE - 1] = 0x01000000;  // PSR
    newThread->stack[STACKSIZE - 2] = (uint32_t)thread; // PC
    newThread->stack[STACKSIZE - 3] = 0x14141414;  // LR (R14)
    newThread->stack[STACKSIZE - 4] = 0x12121212;  // R12
    newThread->stack[STACKSIZE - 5] = 0x03030303;  // R3
    newThread->stack[STACKSIZE - 6] = 0x02020202;  // R2
    newThread->stack[STACKSIZE - 7] = 0x01010101;  // R1
    newThread->stack[STACKSIZE - 8] = 0x00000000;  // R0
    newThread->stack[STACKSIZE - 9] = 0x11111111;  // R11
    newThread->stack[STACKSIZE - 10] = 0x10101010; // R10
    newThread->stack[STACKSIZE - 11] = 0x09090909; // R9
    newThread->stack[STACKSIZE - 12] = 0x08080808; // R8
    newThread->stack[STACKSIZE - 13] = 0x07070707; // R7
    newThread->stack[STACKSIZE - 14] = 0x06060606; // R6
    newThread->stack[STACKSIZE - 15] = 0x05050505; // R5
    newThread->stack[STACKSIZE - 16] = 0x04040404; // R4

    newThread->sp = &(newThread->stack[STACKSIZE - 16]);
    newThread->waitTime = 0;

    if(RunPt == NULL)
    {
        RunPt = newThread;
        RunPt->next = RunPt;
        RunPt->previous = RunPt;
    }

    else {
        newThread->previous = RunPt->previous;
        newThread->next = RunPt;

        RunPt->previous->next = newThread;
        RunPt->previous = newThread;
    }

    return 0;
}

void kaOS_SemInit(sem_t* const sem, const int32_t val)
{
    __asm__ volatile("SVC 0");
}

void kaOS_SemSignal(sem_t* const sem)
{
    __asm__ volatile("SVC 1");
}

void kaOS_SemWait(sem_t* const sem)
{
    __asm__ volatile("SVC 2");
}

void kaOS_Suspend(void)
{
    __asm__ volatile("SVC 3");
}

void kaOS_Sleep(const uint32_t ms)
{
    RunPt->previous->next = RunPt->next;
    RunPt->next->previous = RunPt->previous;
    RunPt->previous = NULL;
    RunPt->waitTime = ms;

    if(WaitPt == NULL)
    {
        WaitPt = RunPt;
        TIMER0_CTL_R |= 1;   // Start Timer0
    }
    else
    {
        tcb_t* node = WaitPt;

        while(node->previous != NULL)
            node = node->previous;

        node->previous = RunPt;
    }

    kaOS_Suspend();
}
