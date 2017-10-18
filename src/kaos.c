#include "kaos.h"

void SysTick_Init(void)
{
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 0x00FFFFFF;
    NVIC_ST_CURRENT_R = 0;
}

void SysTick_Start(void)
{
    NVIC_ST_CTRL_R = 0x07;
}

void SysTick_Handler(void)
{
    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
}

void PendSV_Handler(void)
{
    __asm__("CPSID I");
    __asm__("PUSH {R4-R11}");
    __asm__("LDR R1, %0" : : "m" (RunPt));
    __asm__("STR SP, [R1]");

    RunPt = RunPt->next;

    __asm__("LDR R1, %0" : : "m" (RunPt));
    __asm__("LDR SP, [R1]");
    __asm__("POP {R4-R11}");
    __asm__("CPSIE I");

    __asm__("BX LR");
}

void kaOS_Init()
{
    SysTick_Init();
}

void kaOS_Start()
{
    SysTick_Start();

    __asm__("LDR R0, %0\n\t"
    "LDR SP, [R0]\n\t"
    "POP {R4-R11}\n\t"
    "POP {R0-R3, R12}\n\t"
    "ADD SP, #4\n\t"
    "POP {LR}\n\t"
    "ADD SP, #4\n\t"
    "CPSIE I\n\t"
    "BX LR"
    :
    :"m" (RunPt));
}

int8_t kaOS_AddThead(void (*thread)(void))
{
    Tcb* newThread = malloc(sizeof(Tcb));

    if(newThread == NULL)
        return -1;

    newThread->stack[STACKSIZE - 1] = 0x01000000;  // PSR
    newThread->stack[STACKSIZE - 2] = (uint32_t)thread;
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
