#include "spi.h"
#include "main.h"

void spi_init(void)
{
	DDRB |= ((1<<PORTB2)|(1<<PORTB3)|(1<<PORTB5)); //ножки SPI на выход
	PORTB &= ~((1<<PORTB2)|(1<<PORTB3)|(1<<PORTB5)); //низкий уровень
	SPCR=(0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);
}

void spi_sendByte(uint8_t byte)
{
	SPDR = byte;
	while(!(SPSR & (1<<SPIF)));//подождем пока данные передадутся
}

uint8_t spi_changeByte(uint8_t byte)
{
	SPDR = byte;
	while(!(SPSR & (1<<SPIF)));//подождем пока данные передадутся (обменяются)
	return SPDR;
}