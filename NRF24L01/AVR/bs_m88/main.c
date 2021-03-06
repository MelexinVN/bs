/*
 * bs_m88.c
 *
 * Created: 13.04.2022 11:42:01
 * Author : VMelehin
 */ 

#include "main.h"

volatile uint32_t miliseconds = 0;		//??????? ??????????
volatile uint8_t f_pushed = 0;			//???? ???????
volatile uint32_t time_ms = 0;			//??????????? ????? ??

void port_init(void)//????????????? ??????
{
	// Input/Output Ports initialization
	// Port B initialization
	// Function: Bit7=In Bit6=In Bit5=Out Bit4=In Bit3=Out Bit2=Out Bit1=In Bit0=Out
	DDRB=(0<<DDB7) | (0<<DDB6) | (1<<DDB5) | (0<<DDB4) | (1<<DDB3) | (1<<DDB2) | (0<<DDB1) | (1<<DDB0);
	// State: Bit7=T Bit6=T Bit5=0 Bit4=T Bit3=0 Bit2=0 Bit1=T Bit0=0
	PORTB=(0<<PB7) | (0<<PB6) | (0<<PB5) | (0<<PB4) | (0<<PB3) | (0<<PB2) | (0<<PB1) | (1<<PB0);

	// Port C initialization
	// Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
	DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
	// State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
	PORTC=(0<<PC6) | (0<<PC5) | (0<<PC4) | (0<<PC3) | (0<<PC2) | (0<<PC1) | (0<<PC0);

	// Port D initialization
	// Function: Bit7=Out Bit6=Out Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
	DDRD=(1<<DDD7) | (1<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
	// State: Bit7=0 Bit6=0 Bit5=T Bit4=T Bit3=P Bit2=T Bit1=T Bit0=T
	PORTD=(1<<PD7) | (0<<PD6) | (0<<PD5) | (0<<PD4) | (1<<PD3) | (0<<PD2) | (0<<PD1) | (0<<PD0);
}

void timer_init(void)
{
	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: 125,000 kHz
	// Mode: CTC top=OCR1A
	// OC1A output: Disconnected
	// OC1B output: Disconnected
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer Period: 1 ms
	// Timer1 Overflow Interrupt: Off
	// Input Capture Interrupt: Off
	// Compare A Match Interrupt: On
	// Compare B Match Interrupt: Off
	TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
	TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (1<<CS10);
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H=0x00;
	ICR1L=0x00;
	OCR1AH=0x00;
	OCR1AL=0x7C;
	OCR1BH=0x00;
	OCR1BL=0x00;

}

void interrupt_init(void)
{
	// External Interrupt(s) initialization
	// INT0: On
	// INT0 Mode: Falling Edge
	// INT1: On
	// INT1 Mode: Falling Edge
	// Interrupt on any change on pins PCINT0-7: Off
	// Interrupt on any change on pins PCINT8-14: Off
	// Interrupt on any change on pins PCINT16-23: Off
	EICRA=(1<<ISC11) | (0<<ISC10) | (1<<ISC01) | (0<<ISC00);
	EIMSK=(1<<INT1) | (1<<INT0);
	EIFR=(1<<INTF1) | (1<<INTF0);
	PCICR=(0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);
	// Timer/Counter 1 Interrupt(s) initialization
	TIMSK1=(0<<ICIE1) | (0<<OCIE1B) | (1<<OCIE1A) | (0<<TOIE1);
}

ISR(USART_RX_vect)
{
	
}

ISR(INT0_vect)
{
	IRQ_Callback();
}

ISR(INT1_vect)
{
	if(!f_pushed)							//???? ?????? ???? ???????
	{
		f_pushed = 1;						//????????? ???? ???????
		time_ms = miliseconds;				//????????? ?????????? ??
	}
}

ISR(TIMER1_COMPA_vect)
{
	miliseconds++;							//??????? ??
}


int main(void)
{
	interrupt_init();						//????????????? ??????????
	port_init();							//????????????? ??????
	timer_init();							//????????????? ???????
	spi_init();								//????????????? SPI
	USART_Init (8);							//????????????? USART 115200 ???
	nrf24_init();							//????????????? ???????????
    sei();									//?????????? ?????????? ??????????
	usart_println("start");					//???????? ????????? ?????? ? ????
	/*
	dt_reg = NRF24_ReadReg(CONFIG);			
	sprintf(str,"CONFIG: 0x%02X",dt_reg);
	usart_println(str);
	dt_reg = NRF24_ReadReg(EN_AA);
	sprintf(str,"EN_AA: 0x%02X",dt_reg);
	usart_println(str);
	dt_reg = NRF24_ReadReg(EN_RXADDR);
	sprintf(str,"EN_RXADDR: 0x%02X",dt_reg);
	usart_println(str);
	dt_reg = NRF24_ReadReg(STATUS);
	sprintf(str,"STATUS: 0x%02X",dt_reg);
	usart_println(str);
	dt_reg = NRF24_ReadReg(RF_SETUP);
	sprintf(str,"RF_SETUP: 0x%02X",dt_reg);
	usart_println(str);
	NRF24_Read_Buf(TX_ADDR,buf,3);
	sprintf(str,"TX_ADDR: 0x%02X, 0x%02X, 0x%02X",buf[0],buf[1],buf[2]);
	usart_println(str);
	NRF24_Read_Buf(RX_ADDR_P1,buf,3);
	sprintf(str,"RX_ADDR: 0x%02X, 0x%02X, 0x%02X",buf[0],buf[1],buf[2]);
	usart_println(str);
	*/
    while (1) 
    {
		nrf24l01_receive();			//????????? ?????? ???????????
    }
}

