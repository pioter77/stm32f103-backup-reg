#include "gp_drive.h"

////port literowo,pin numer,kierunek in out, tryb kierunku
void init_GP(unsigned short port,unsigned short pin,unsigned short dir,unsigned short opt)
{
		volatile unsigned long *CR=0;
		unsigned short tPIN=pin;
		unsigned short offset=0; //variable for determinging low or high registere
	
	//tutaj sprawdzamy czy wiekszy niz 7 to wtedy offest bedzie potrzebny do obslugi rejestrru high
	if(pin>7)
	{
		tPIN-=8;
		offset=0x01;
	}
	//tutaj wlaczamy zegar na dane porty
	if(port==1)	//port A
	{
		RCC_APB2ENR |=4;			//enabling PORT A
		CR=(volatile unsigned long *)(&GPIO_A+offset);
	}
	else if(port==2)	//port B
	{
		RCC_APB2ENR |=8;			//enabling PORT B
		CR=(volatile unsigned long *)(&GPIO_B+offset);
	}
	else if(port==3)	//port C
	{
		RCC_APB2ENR |=0x10;			//enabling PORT C
		CR=(volatile unsigned long *)(&GPIO_C+offset);
	}
	//teraz jak mamy wlaczone zegary na tych portach to mozemy sie odnosci do tych rejestrów A B C i je konfigurowac jako wejscia/wyjscia:
	*CR &= ~((unsigned long)0xf<<(tPIN)*4);	//we reset the target pin setup myk z inwersja zmienia zera na dany pin ktory zeujemy i na reszce sa jedynki przesuwany o 4 bo 4 bity na kazdy pi n konfigurujemy
	*CR |=((dir<<(tPIN*4)) | (opt<<(tPIN*4+2)));	////set up the direction and the option of the pin eg. output and push pull
	//najpierw dir jako in out a potem opt jako push full floating input 
}

int read_GP(unsigned short port,unsigned short pin)
{
	volatile unsigned long *IDR=0;
	unsigned long offset=0x02;//offset 2 bo 2 pierwsze bloki to sa rejestry ustawiania typu portu idR jest zaraz po nich wiec 2 miejsca od poczatku musimy sie przesunac
	int state;	//the reurned readout state value
	
	if(port==1)
	{
		IDR=(volatile unsigned long *) (&GPIO_A + offset);
	}
	else if(port==2)
	{
		IDR=(volatile unsigned long *) (&GPIO_B + offset);
	}
	else if(port==3)
	{
		IDR=(volatile unsigned long *) (&GPIO_C + offset);
	}
	state=((*IDR & ((unsigned long)1<<pin))>>pin);	//to drugie przesuniecie to jest dlatego zeby miec znowu wartosc 1 bit a nie np 1 ale na 6 pozycji bo odczytujem tylko stan danego pinu
	return state;
}

void write_GP(unsigned short port,unsigned short pin,unsigned short state)
{
	volatile unsigned long *ODR=0;
	unsigned long offset=0x03;//offset 2 bo 2 pierwsze bloki to sa rejestry ustawiania typu portu idR jest zaraz po nich wiec 2 miejsca od poczatku musimy sie przesunac
		
	if(port==1)
	{
		ODR=(volatile unsigned long *) (&GPIO_A + offset);
	}
	else if(port==2)
	{
		ODR=(volatile unsigned long *) (&GPIO_B + offset);
	}
	else if(port==3)
	{
		ODR=(volatile unsigned long *) (&GPIO_C + offset);
	}
	state ? (*ODR |= ((unsigned long)state<<pin)) : (*ODR &= ~((unsigned long)1<<pin)); 	//jak chemy wstawic zero na dany pin to robimy maske 1 pod danym bitem i invertujemy maske wtedy iloczyn z zerem jes tylko na danym pinie
	
}

void toggle_GP(unsigned short port,unsigned short pin)
{
	if(read_GP(port,pin))
		write_GP(port,pin,0);
	else
		write_GP(port,pin,1);
}
