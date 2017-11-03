#include "kaos.h"
#include <stdlib.h>

tcb_t* RunPt = 0;
tcb_t* WaitPt = 0;
static uint32_t sysClockFrequency = 0;

static inline void Timer0_Init(const uint32_t period)
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

static inline void SysTick_Init(void)
{
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 0x00FFFFFF;
    NVIC_ST_CURRENT_R = 0;
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x0000FFFF) | 0x20700000;  // Set SysTick interrupt priority 2 and PendSV priority 7
}

void SysTick_Start(void)
{
    NVIC_ST_CTRL_R = 0x07;
}

static inline void kaOS_SysClock_Init(const enum SysClock sysclock)
{
    // Override some fields of RCC register with RCC2 for additional features (set SYSCTL_RCC2_USERCC2)
    // Bypass PLL during initializing it (set SYSCTL_RCC2_BYPASS2)
    // Use 400 MHz PLL (set SYSCTL_RCC2_DIV400)
    SYSCTL_RCC2_R |= 0xC0000800;

    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~SYSCTL_RCC_XTAL_M) | SYSCTL_RCC_XTAL_16MHZ;   // Select 16 MHz crystal
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~SYSCTL_RCC2_OSCSRC2_M) | SYSCTL_RCC2_OSCSRC2_MO;  // Set main oscillator as source

    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;   // Activate PLL by clearing PWRDN
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) | (sysclock << 22);  // Set the system divider (Sysdiv2)

    while((SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0);    // Wait for the PLL to lock by polling PLLLRIS

    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;  // Enable PLL by clearing BYPASS

    sysClockFrequency = 400000000 / (sysclock + 1);
}

void kaOS_Init(const enum SysClock sysclock)
{
    kaOS_SysClock_Init(sysclock);
    SysTick_Init();
    Watchdog_Timer0_Init();
    Timer0_Init(0x00FFFFFF);
}

//int8_t kaOS_AddThead(void (*thread)(void))
//{
//    static uint8_t index = 0;
//    static uint32_t stacks[4][STACKSIZE];
//    static tcb_t tcbs[4];
//
//    stacks[index][STACKSIZE - 1] = 0x01000000;  // PSR
//    stacks[index][STACKSIZE - 2] = (uint32_t)thread; // PC
//    stacks[index][STACKSIZE - 3] = 0x14141414;  // LR (R14)
//    stacks[index][STACKSIZE - 4] = 0x12121212;  // R12
//    stacks[index][STACKSIZE - 5] = 0x03030303;  // R3
//    stacks[index][STACKSIZE - 6] = 0x02020202;  // R2
//    stacks[index][STACKSIZE - 7] = 0x01010101;  // R1
//    stacks[index][STACKSIZE - 8] = 0x00000000;  // R0
//    stacks[index][STACKSIZE - 9] = 0x11111111;  // R11
//    stacks[index][STACKSIZE - 10] = 0x10101010; // R10
//    stacks[index][STACKSIZE - 11] = 0x09090909; // R9
//    stacks[index][STACKSIZE - 12] = 0x08080808; // R8
//    stacks[index][STACKSIZE - 13] = 0x07070707; // R7
//    stacks[index][STACKSIZE - 14] = 0x06060606; // R6
//    stacks[index][STACKSIZE - 15] = 0x05050505; // R5
//    stacks[index][STACKSIZE - 16] = 0x04040404; // R4
//
//    tcbs[index].sp = &stacks[index][STACKSIZE - 16];
//    tcbs[index].waitTime = 0;
//
//    if(RunPt == 0)
//    {
//        RunPt = &tcbs[index];
//        RunPt->next = RunPt;
//        RunPt->previous = RunPt;
//    }
//
//    else {
//        tcbs[index].previous = RunPt->previous;
//        tcbs[index].next = RunPt;
//
//        RunPt->previous->next = &tcbs[index];
//        RunPt->previous = &tcbs[index];
//    }
//
//    ++index;
//    return 0;
//}

int8_t kaOS_AddThead(void (*thread)(void))
{
    tcb_t* newThread = malloc(sizeof(tcb_t));

    if(newThread == 0)
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

    if(RunPt == 0)
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
    __asm__ volatile("SVC 4");
}

const uint32_t kaOS_GetSysClock(void)
{
    return sysClockFrequency;
}
