/*
 * TX_tiny13.c
 * fuse LOW: 0x7A, HIGH: 0xFF, Ext: 0x00, Lock: 0xFF	
 * Created: 23.06.2022 13:22:25
 * Author : MelexinVN
 */ 

#include "tx_433.h"

//подключаемые библиотеки
#include <avr/io.h>
#include <util/delay.h>

//выход  на передатчик
#define RF_OUT_Pin PORTB3		//пин выхода радиомодуля
#define RF_GPIO_Port PORTB		//порт выхода радиомодуля

//управление выходом на передатчик
#define RF_HIGH()		RF_GPIO_Port|=(1<<RF_OUT_Pin)
#define RF_LOW()		RF_GPIO_Port&=~(1<<RF_OUT_Pin)

int main(void)
{

// Input/Output Ports initialization
// Port B initialization
// Function: Bit5=In Bit4=In Bit3=Out Bit2=In Bit1=In Bit0=In
DDRB=(0<<DDB5) | (0<<DDB4) | (1<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit5=T Bit4=T Bit3=0 Bit2=T Bit1=T Bit0=T
PORTB=(0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);


    /* Replace with your application code */
    while (1) 
    {

		send_rf(DEV_ADDR, CMND);
		//_delay_ms(10);

    }
}

