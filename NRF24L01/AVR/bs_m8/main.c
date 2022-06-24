/*
 * bs_m8.c
 *
 * Created: 11.04.2022 7:58:54
 * Author : VMelehin
 */ 

#include "main.h"
#include "spi.h"

//char str[STRING_SIZE] = {0x00};
//uint8_t dt_reg = 0;
//uint8_t buf[5]={0};						
volatile uint32_t miliseconds = 0;		//счетчик милисекунд
volatile uint8_t f_pushed = 0;			//флаг нажати€
volatile uint32_t time_ms = 0;			//сохраненное врем€ мс
volatile uint8_t f_reset = 0;			//сохраненное врем€ мс
volatile uint8_t adc_res = 0;			//сохраненное врем€ мс

void port_init(void)//»нициализаци€ портов 
{
	// Input/Output Ports initialization
	// Port B initialization
	// Function: Bit7=In Bit6=In Bit5=Out Bit4=In Bit3=Out Bit2=Out Bit1=In Bit0=Out
	DDRB=(0<<DDB7) | (0<<DDB6) | (1<<DDB5) | (0<<DDB4) | (1<<DDB3) | (1<<DDB2) | (0<<DDB1) | (1<<DDB0);
	// State: Bit7=T Bit6=T Bit5=0 Bit4=T Bit3=0 Bit2=0 Bit1=T Bit0=0
	PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (1<<PORTB0);

	// Port C initialization
	// Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
	DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
	// State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
	PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

	// Port D initialization
	// Function: Bit7=Out Bit6=Out Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
	DDRD=(1<<DDD7) | (1<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
	// State: Bit7=0 Bit6=0 Bit5=T Bit4=T Bit3=P Bit2=T Bit1=T Bit0=T
	PORTD=(1<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (1<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);
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

void adc_init(void)
{
	// ADC initialization
	// ADC Clock frequency: 62,500 kHz
	// ADC Voltage Reference: AVCC pin
	ADMUX=ADC_VREF_TYPE;
	ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADFR) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	SFIOR=(0<<ACME);
}

void interrupt_init(void)
{
	// External Interrupt(s) initialization
	// INT0: On
	// INT0 Mode: Falling Edge
	// INT1: On
	// INT1 Mode: Falling Edge
	GICR|=(1<<INT1) | (1<<INT0);
	MCUCR=(1<<ISC11) | (0<<ISC10) | (1<<ISC01) | (0<<ISC00);
	GIFR=(1<<INTF1) | (1<<INTF0);
	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (1<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (1<<TOIE0);
}

// Read the 8 most significant bits
// of the AD conversion result
unsigned char read_adc(unsigned char adc_input)
{
	ADMUX=adc_input | ADC_VREF_TYPE;
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(10);
	// Start the AD conversion
	ADCSRA|=(1<<ADSC);
	// Wait for the AD conversion to complete
	while ((ADCSRA & (1<<ADIF))==0);
	ADCSRA|=(1<<ADIF);
	return ADCH;
}

ISR(USART_RXC_vect)
{
	
}

ISR(INT0_vect)
{
	IRQ_Callback();
}

ISR(INT1_vect)
{
	if(!f_pushed)							//если опущен флаг нажати€
	{
		f_pushed = 1;						//поднимаем флаг нажати€
		time_ms = miliseconds;				//сохран€ем количество мс
	}
}

ISR(TIMER1_COMPA_vect)
{
	miliseconds++;							//считаем мс
	if (!(miliseconds % 100)) 
	{
		adc_res = read_adc(6);
		/* дл€ расчета процентов зар€да дл€ LiPo аккумул€торов
		if (adc_res >= 170) adc_res = (adc_res - 170)*100/40 //расчет в процентах дл€ LiPo
		else adc_res = 0;
		*/
	}
}

int main(void)
{
	interrupt_init();						//инициализаци€ прерываний
	port_init();							//инициализаци€ портов
	timer_init();							//инициализаци€ таймера
	spi_init();								//инициализаци€ SPI
	USART_Init (8);							//инициализаци€ USART 115200 бод
	nrf24_init();							//инициализаци€ радиомодул€
	adc_init();
    sei();									//глобальное разрешение прерываний
	usart_println("start");					//отправка стартовой строки в порт
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
	uint8_t blink_counter = 5;
	while (blink_counter)
	{
		LED_ON();
		_delay_ms(50);
		LED_OFF();
		_delay_ms(50);
		blink_counter--;
	}
	wdt_enable(WDTO_120MS);
    while (1) 
    {
		nrf24l01_receive();			//процедура приема радиомодул€
		//if (!f_reset) wdt_reset();
    }
}

