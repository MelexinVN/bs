/*
 * bs_m8.c
 *
 * Created: 11.04.2022 7:58:54
 * Author : VMelehin
 */ 

#include "main.h"

char str[STRING_SIZE] = {0x00};

ISR(USART_RXC_vect)
{

}




int main(void)
{
    int counter = 0;
	
	USART_Init (8); //115200
    sei();
	usart_println("start");


    while (1) 
    {
		sprintf(str, "counter= %d",counter);
		usart_println(str);
		counter++;
		_delay_ms(100);
    }
}

