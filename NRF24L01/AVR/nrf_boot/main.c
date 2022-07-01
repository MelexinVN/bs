/*
 * nrf_boot.c
 *
 * Created: 01.07.2022 13:49:22
 * Author : VMelehin
 */
  
#include "main.h"
 
uint8_t gBuffer[SPM_PAGESIZE];

void port_init(void)//Инициализация портов
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

ISR(INT0_vect)
{
	IRQ_Callback();
}

static inline void eraseFlash(void)
{
	// erase only main section (bootloader protection)
	uint32_t addr = 0;
	while (APP_END > addr) {
		boot_page_erase(addr);		// Perform page erase
		boot_spm_busy_wait();		// Wait until the memory is erased.
		addr += SPM_PAGESIZE;
	}
	boot_rww_enable();
}


static inline uint16_t writeFlashPage(uint16_t waddr, pagebuf_t size)
{
	uint32_t pagestart = (uint32_t)waddr<<1;
	uint32_t baddr = pagestart;
	uint16_t data;
	uint8_t *tmp = gBuffer;

	do {
		data = *tmp++;
		data |= *tmp++ << 8;
		boot_page_fill(baddr, data);	// call asm routine.

		baddr += 2;			// Select next word in memory
		size -= 2;			// Reduce number of bytes to write by two
	} while (size);				// Loop until all bytes written

	boot_page_write(pagestart);
	boot_spm_busy_wait();
	boot_rww_enable();		// Re-enable the RWW section

	return baddr>>1;
}

static void (*jump_to_app)(void) = 0x0000;

int main(void)
{
    interrupt_init();						//инициализация прерываний
    port_init();							//инициализация портов
    spi_init();								//инициализация SPI
    nrf24_init();							//инициализация радиомодуля

	uint8_t blink_counter = 2;
	while (blink_counter)
	{
		LED_ON();
		_delay_ms(50);
		LED_OFF();
		_delay_ms(50);
		blink_counter--;
	}						

	_delay_ms(500);

	jump_to_app();

    while (1) 
    {
    }
}

