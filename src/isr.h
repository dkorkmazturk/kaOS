/*
 * isr.h
 *
 * This file contains the declarations of ISRs
 */

#ifndef ISR_H_
#define ISR_H_

void SysTick_Handler(void);
void Timer0_Handler(void);
extern void SVCall_Handler(void);
extern void PendSV_Handler(void);

#endif /* ISR_H_ */
