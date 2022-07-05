/*
 * RX_t13.c
 * fuse LOW: 0x7A, HIGH: 0xFF, Ext: 0x00, Lock: 0xFF
 * Created: 29.06.2022 8:38:53
 * Author : VMelehin
 */ 


#include "main.h"

volatile uint8_t f_received = 0;			//флаг прихода послыки
extern uint8_t bit_array[SIZE_ARRAY];		//массив принятых битов
extern uint8_t dev_addr;					//адрес устройства
extern uint8_t tx_addr;						//адрес передатчика
extern uint8_t command;						//команда
extern uint8_t crc;							//контрольная сумма
volatile uint32_t ten_micros = 0;							//


ISR(PCINT0_vect)
{
	RX_Input();
}

ISR(TIM0_COMPA_vect)
{
	ten_micros++;
}

void react(void)
{
	LED_ON();
	_delay_ms(20);
	LED_OFF();
	_delay_ms(20);
	LED_ON();
	_delay_ms(20);
	LED_OFF();
	_delay_ms(20);
	LED_ON();
	_delay_ms(20);
	LED_OFF();
	_delay_ms(20);
	LED_ON();
	_delay_ms(20);
	LED_OFF();
	_delay_ms(20);
}

int main(void)
{
	// Input/Output Ports initialization
	// Port B initialization
	// Function: Bit5=In Bit4=Out Bit3=In Bit2=In Bit1=In Bit0=In
	DDRB=(0<<DDB5) | (1<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
	// State: Bit5=T Bit4=0 Bit3=T Bit2=T Bit1=T Bit0=T
	PORTB=(0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);


	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 9600,000 kHz
	// Mode: CTC top=OCR0A
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	// Timer Period: 10 us
	TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (1<<WGM01) | (0<<WGM00);
	TCCR0B=(0<<WGM02) | (0<<CS02) | (0<<CS01) | (1<<CS00);
	TCNT0=0x00;
	OCR0A=0x5F;
	OCR0B=0x00;

	// Timer/Counter 0 Interrupt(s) initialization
	TIMSK0=(0<<OCIE0B) | (1<<OCIE0A) | (0<<TOIE0);

	// External Interrupt(s) initialization
	// INT0: Off
	// Interrupt on any change on pins PCINT0-5: On
	GIMSK=(0<<INT0) | (1<<PCIE);
	MCUCR=(0<<ISC01) | (0<<ISC00);
	PCMSK=(0<<PCINT5) | (0<<PCINT4) | (1<<PCINT3) | (0<<PCINT2) | (0<<PCINT1) | (0<<PCINT0);
	GIFR=(0<<INTF0) | (1<<PCIF);

	wdt_enable(WDTO_8S);
	//react();

	sei();

    while (1) 
    {
		if(f_received == 1) //если пришёл пакет
		{
			//if (dev_addr == 0xAA) 
			react();
			ten_micros = 0;
			//опускаем флаг приема
			f_received = 0;
			//dev_addr = 0;
			sei();   // включаем прерывания 
		}
		wdt_reset();
    }
}

