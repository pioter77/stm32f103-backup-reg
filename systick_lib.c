#include "stm32f10x.h"
#include "systick_lib.h"



void systick_init(void)
{
	__disable_irq();
	SysTick->CTRL=0;
	SysTick->LOAD=0x00FFFFFF;
	SysTick->VAL=0;
	SysTick->CTRL	|=5;		//enable main clock interrupts when 0 and enable systick
	__enable_irq();
	
}

//oczekiwanie 1 milisekundy jak mamy uns longa 32 bit to 2^32 tyle sekund mozemy czekac to ~50 dni prawie
void delay_milis(void)
{
	SysTick->LOAD=0x11940;	//? nie powinno byc o 1 mniej? blad?
	SysTick->VAL=0;
	while((SysTick->CTRL & 0x00010000)==0);		//sthay here not he most efficeient way of doing the wait activity couse wre stuck at his moment of code
}

void delay_MS(unsigned long t)
{
	for(;t>0;t--)
		delay_milis();
}

void systick_inter_start(void)
{//fcn needed for uart handling interrupt strategy
		__disable_irq();
	SysTick->CTRL=0;
	SysTick->LOAD=72000;	//almost 1 ms delay between interrupts 
	SysTick->VAL=0;
	SysTick->CTRL	|=7;		//7 we are initialising the interrupt
	__enable_irq();
	
}

void systick_inter(unsigned short uart1mgr[],unsigned short uart2mgr[],unsigned short uart3mgr[])
{
	
	if(uart1mgr[0]!=0)
	{
		if(uart1mgr[6]==0)
		{
			uart1mgr[0]=0;		//reinitialize counter of chars
			uart1mgr[1]=1;		//set mmessage ready to process byu main() flag to 1 //readout process has finished
			systick_init();
		}else{
			uart1mgr[6]--;
		}
	}
		else if(uart2mgr[0]!=0)
	{
		if(uart2mgr[6]==0)
		{
			uart2mgr[0]=0;		//reinitialize counter of chars
			uart2mgr[1]=1;		//set mmessage ready to process byu main() flag to 1 //readout process has finished
			systick_init();
		}else{
			uart2mgr[6]--;
		}
	}
		else if(uart3mgr[0]!=0)
	{
		if(uart3mgr[6]==0)
		{
			uart3mgr[0]=0;		//reinitialize counter of chars
			uart3mgr[1]=1;		//set mmessage ready to process byu main() flag to 1 //readout process has finished
			systick_init();		//after this reset systick 
		}else{
			uart3mgr[6]--;
		}
	}
}
