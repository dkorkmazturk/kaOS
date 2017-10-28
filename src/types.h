#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define STACKSIZE 116

enum SysClock {SysClock_80Mhz = 0x4, SysClock_66p67MHz = 0x5, SysClock_50MHz = 0x7, SysClock_44p44MHz = 0x8, SysClock_40MHz = 0x9, SysClock_36p36MHz = 0xa, SysClock_33p33MHz = 0xb, SysClock_30p77MHz = 0xc, SysClock_28p57MHz = 0xd, SysClock_26p67MHz = 0xe, SysClock_25MHz = 0xf, SysClock_23p53MHz = 0x10, SysClock_22p22MHz = 0x11, SysClock_21p05MHz = 0x12, SysClock_20MHz = 0x13, SysClock_19p05MHz = 0x14, SysClock_18p18MHz = 0x15, SysClock_17p39MHz = 0x16, SysClock_16p67MHz = 0x17, SysClock_16MHz = 0x18, SysClock_15p38MHz = 0x19, SysClock_14p81MHz = 0x1a, SysClock_14p29MHz = 0x1b, SysClock_13p79MHz = 0x1c, SysClock_13p33MHz = 0x1d, SysClock_12p9MHz = 0x1e, SysClock_12p5MHz = 0x1f, SysClock_12p12MHz = 0x20, SysClock_11p76MHz = 0x21, SysClock_11p43MHz = 0x22, SysClock_11p11MHz = 0x23, SysClock_10p81MHz = 0x24, SysClock_10p53MHz = 0x25, SysClock_10p26MHz = 0x26, SysClock_10MHz = 0x27, SysClock_9p756MHz = 0x28, SysClock_9p524MHz = 0x29, SysClock_9p302MHz = 0x2a, SysClock_9p091MHz = 0x2b, SysClock_8p889MHz = 0x2c, SysClock_8p696MHz = 0x2d, SysClock_8p511MHz = 0x2e, SysClock_8p333MHz = 0x2f, SysClock_8p163MHz = 0x30, SysClock_8MHz = 0x31, SysClock_7p843MHz = 0x32, SysClock_7p692MHz = 0x33, SysClock_7p547MHz = 0x34, SysClock_7p407MHz = 0x35, SysClock_7p273MHz = 0x36, SysClock_7p143MHz = 0x37, SysClock_7p018MHz = 0x38, SysClock_6p897MHz = 0x39, SysClock_6p78MHz = 0x3a, SysClock_6p667MHz = 0x3b, SysClock_6p557MHz = 0x3c, SysClock_6p452MHz = 0x3d, SysClock_6p349MHz = 0x3e, SysClock_6p25MHz = 0x3f, SysClock_6p154MHz = 0x40, SysClock_6p061MHz = 0x41, SysClock_5p97MHz = 0x42, SysClock_5p882MHz = 0x43, SysClock_5p797MHz = 0x44, SysClock_5p714MHz = 0x45, SysClock_5p634MHz = 0x46, SysClock_5p556MHz = 0x47, SysClock_5p479MHz = 0x48, SysClock_5p405MHz = 0x49, SysClock_5p333MHz = 0x4a, SysClock_5p263MHz = 0x4b, SysClock_5p195MHz = 0x4c, SysClock_5p128MHz = 0x4d, SysClock_5p063MHz = 0x4e, SysClock_5MHz = 0x4f, SysClock_4p938MHz = 0x50, SysClock_4p878MHz = 0x51, SysClock_4p819MHz = 0x52, SysClock_4p762MHz = 0x53, SysClock_4p706MHz = 0x54, SysClock_4p651MHz = 0x55, SysClock_4p598MHz = 0x56, SysClock_4p545MHz = 0x57, SysClock_4p494MHz = 0x58, SysClock_4p444MHz = 0x59, SysClock_4p396MHz = 0x5a, SysClock_4p348MHz = 0x5b, SysClock_4p301MHz = 0x5c, SysClock_4p255MHz = 0x5d, SysClock_4p211MHz = 0x5e, SysClock_4p167MHz = 0x5f, SysClock_4p124MHz = 0x60, SysClock_4p082MHz = 0x61, SysClock_4p04MHz = 0x62, SysClock_4MHz = 0x63, SysClock_3p96MHz = 0x64, SysClock_3p922MHz = 0x65, SysClock_3p883MHz = 0x66, SysClock_3p846MHz = 0x67, SysClock_3p81MHz = 0x68, SysClock_3p774MHz = 0x69, SysClock_3p738MHz = 0x6a, SysClock_3p704MHz = 0x6b, SysClock_3p67MHz = 0x6c, SysClock_3p636MHz = 0x6d, SysClock_3p604MHz = 0x6e, SysClock_3p571MHz = 0x6f, SysClock_3p54MHz = 0x70, SysClock_3p509MHz = 0x71, SysClock_3p478MHz = 0x72, SysClock_3p448MHz = 0x73, SysClock_3p419MHz = 0x74, SysClock_3p39MHz = 0x75, SysClock_3p361MHz = 0x76, SysClock_3p333MHz = 0x77, SysClock_3p306MHz = 0x78, SysClock_3p279MHz = 0x79, SysClock_3p252MHz = 0x7a, SysClock_3p226MHz = 0x7b, SysClock_3p2MHz = 0x7c, SysClock_3p175MHz = 0x7d, SysClock_3p15MHz = 0x7e, SysClock_3p125MHz = 0x7f, SysClock_MAX = 0x4, SysClock_MIN = 0x7f};
enum UART {UART_0 = 0x000, UART_1 = 0x400, UART_2 = 0x800, UART_3 = 0xC00, UART_4 = 0x1000, UART_5 = 0x1400, UART_6 = 0x1800, UART_7 = 0x1C00};

typedef struct tcb_t
{
    uint32_t* sp;
    struct tcb_t* next;
    struct tcb_t* previous;
    uint32_t waitTime;

    uint32_t stack[STACKSIZE];
} tcb_t;

typedef struct
{
    int32_t sem;
    tcb_t* blockedQueue;
} sem_t;


#endif
