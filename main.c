#include "stm32f10x.h"                  // Device header
#include "systick_lib.h"
#include "gp_drive.h"

void led_info(uint16_t times);
void reenable_clks(void);
void init_BAK_reg(void);
void write_BAK_reg(uint8_t addr,uint16_t data);
uint16_t read_BAK_reg(uint8_t addr);

volatile uint16_t main_pos_val=1;		//docelowo 1 pos z bak registerze
volatile uint32_t quasi_millis=0;
 uint32_t prev_time=0;
 _Bool ac_state=0;
uint16_t cnt=0;

//1 2 3 // 3 times only readout for a day
int main(void)
{	

	

	PWR->CR |= PWR_CR_CWUF;			/* Clear Wake-up flag */
		PWR->CR |=PWR_CR_CSBF;		//clear standby flag
	PWR->CSR &= 0xFEFF;		//disnable wakeup pin 
	//PWR->CSR &= ~PWR_CSR_EWUP;
	//NVIC_SystemReset();
	
	RCC->APB2ENR |=0x04;	//enable clock for gpio a input
	GPIOA->CRL &= 0xFFFFFFF0;	//CLEAR a0 state config
	GPIOA->CRL |=0x00000008;	//set as input pullupdown
	__disable_irq();		//diable interrupts
	//here we set up the interrupt:
	AFIO->EXTICR[0]=0x00;		//pa0 as interrupt
	EXTI->IMR |=0x1;	//interrupt enable on line 0
	EXTI->RTSR |=0x1;		//ENABLE rising triger on interrupt line0 
	NVIC_EnableIRQ(EXTI0_IRQn); 		//enables interrupt set above at line 0
	__enable_irq();		//ENABLE interrupts on the core
	
	RCC->APB2ENR |=0x10;	//enable apb2 clock for gpioc output

	GPIOC->CRH &=0xFF0FFFFF;	//clear settings for c13
	GPIOC->CRH |=0x00300000;	//c13 as output push pull 50mhz
	GPIOC->ODR |=0x00002000;	
	GPIOC->CRH &= 0xF0FFFFFF;	//CLEAR_BIT c14 status settings
	GPIOC->CRH |=0x08000000;	//input push pullup pulldown c14
	
	init_GP(PA,8,OUT50,O_GP_PP);
	init_GP(PA,9,OUT50,O_GP_PP);
	write_GP(PA,8,LOW);
	write_GP(PA,9,LOW);
	
	
	init_BAK_reg();
		systick_init();
	systick_inter_start();	//co 1 ms przerwania
		if(read_BAK_reg(1) < 4 && read_BAK_reg(1)>0)//check rtc memm
	{
		//write_BAK_reg(1,1);
		if((read_BAK_reg(1)%2)==1)
		{
			write_GP(PA,8,HIGH);	//nieparzysa
		}else{
			write_GP(PA,9,HIGH);	//parzusta
		}
	}else{
		write_BAK_reg(1,1);
	}

	
	
	while(1)
	{

		if((GPIOC->IDR & 0x00004000)>>14)
		{
			__disable_irq();
			SysTick->CTRL &=0x8;	//we need to disable systick otherwise will be active in sleep!!!!
			__enable_irq();
			GPIOC->ODR |=0x00002000;//off
			write_BAK_reg(1,read_BAK_reg(1)+1);	//on cycles count max value 4 when 5 get back to 1 (startiung val)
			//GPIOC->CRH &= 
			write_GP(PA,8,LOW);
			write_GP(PA,9,LOW);
			SCB->SCR |= SCB_SCR_SLEEPDEEP;
			PWR->CR |= PWR_CR_PDDS;			// standby register
			PWR->CR |= PWR_CR_CWUF;	
			PWR->CSR |= PWR_CSR_EWUP;		//enable wakeup pin 
			PWR->CR |= PWR_CR_LPDS;			//DISABLE internal regulator during sleep time

			__force_stores();
			__WFI();
		}else{
			//GPIOC->ODR &=0xFFFFDFFF;	//set low so enable c13 led

			led_info(read_BAK_reg(1));
						}
	}
}

void led_info(uint16_t times)
{
	//!pc13 led_info reverse logic

	if(cnt<times)
	{
		if((quasi_millis-prev_time>200) && !ac_state)
		{
			ac_state=1;		//miganie
			write_GP(PC,13,LOW);	//on
			prev_time=quasi_millis;

		}
		if((quasi_millis-prev_time>200) && ac_state)
		{
			ac_state=0;		//miganie
			write_GP(PC,13,HIGH);	//off
			prev_time=quasi_millis;
						cnt++;
		}
	}else if(quasi_millis-prev_time>3000){
			cnt=0;
	}

}
void reenable_clks(void)
{
	
}
void EXTI0_IRQHandler()
{
		EXTI->PR |=0x1;		//delete pending irq
}

void SysTick_Handler()
{
	quasi_millis++;		//increment every 1 ms
}

void init_BAK_reg(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;		//enable clocks to allow to interface memory, memory worsk withouth that but we wont be able to access it without htis
	PWR->CR |= PWR_CR_DBP; 		//ENABLE access do bak regs and rtc
	
}

void write_BAK_reg(uint8_t addr,uint16_t data)
{
	BKP->DR1=data;
}

uint16_t read_BAK_reg(uint8_t addr)
{
	return (uint16_t)(BKP->DR1);
}
