.thumb
.syntax unified
.align 2

.extern RunPt

.global PendSV_Handler
.global SVCall_Handler
.global kaOS_Start

.text

// SVC #0 ==> Sem_Init
// SVC #1 ==> Sem_Signal
// SVC #2 ==> Sem_Wait
// SVC #3 ==> kaOS_Suspend (Reset SysTick and trigger SysTick interrupt)
// SVC #4 ==> kaOS_Sleep

.thumb_func
SVCall_Handler:
    MRS R3, PSP
    LDR R3, [R3, #24]
    SUB R3, #2
    LDR R3, [R3]
    ANDS R3, #0xFF

    PUSH {LR}
    BL SVCaller
    POP {LR}
    BX LR

.thumb_func
PendSV_Handler:
    CPSID I             // Disable interrupts

    MRS R0, PSP         // Load value of PSP into R0
    STMDB R0!, {R4-R11} // Basically PUSH {R4-R11} using R0
    LDR R1, =RunPt      // Load address of RunPt to R1
    LDR R2, [R1]        // Load value that pointed by RunPt (address of tcb.sp) to R2
    STR R0, [R2]        // Store new PSP into memory location that pointed by tcb.sp

    PUSH {R1, LR}       // Save LR because BL Scheduler will overrite it. Scheduler may also overwrite R1 so save it either.
    BL Scheduler        // Determine the next task
    POP {R1, LR}        // Load LR and R1

    LDR R2, [R1]        // Load value that pointed by RunPt (address of tcb.sp) to R2 (Because RunPt has updated at Scheduler function)
    LDR R0, [R2]        // Load next task's PSP to R0
    LDM R0!, {R4-R11}   // Basically POP {R4-R11} using R0
    MSR PSP, R0         // Load value of R0 into PSP

    CPSIE I             // Enable interrupts
    BX LR               // Branch to LR

.thumb_func
kaOS_Start:
    CPSIE I            // Enable interrupts
    BL SysTick_Start   // Start SysTick

    MOV R0, #3         // Switch to unprivileged mode
    MSR CONTROL, R0
    ISB

    LDR R0, =RunPt  // Load address of RunPt to R0
    LDR R0, [R0]    // Load value that pointed by RunPt (address of tcb.sp) to R0
    LDR SP, [R0]    // Load value that pointed by tcb.sp to SP (PSP)
    POP {R4-R11}
    POP {R0-R3, R12}

    ADD SP, #4      // Skip LR on tcb
    POP {LR}        // Pop PC to LR
    ADD SP, #4      // Skip PSR

    BX LR           // Branch to LR
