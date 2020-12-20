#ifndef SYSTICK_LIB_H
#define SYSTICK_LIB_H
#include "stm32f10x.h"

volatile extern uint32_t systick_sim_millis;

void systick_init(void);
void delay_milis(void);
void delay_MS(unsigned long t);
void systick_inter_start(void);
void systick_inter(unsigned short uart1mgr[],unsigned short uart2mgr[],unsigned short uart3mgr[]);

#endif
