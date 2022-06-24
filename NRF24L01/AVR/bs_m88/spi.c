#include "spi.h"
#include "main.h"
//инициализация SPI
void spi_init(void)
{
	DDRB |= ((1<<PB2)|(1<<PB3)|(1<<PB5)); //ножки SPI на выход
	PORTB &= ~((1<<PB2)|(1<<PB3)|(1<<PB5)); //низкий уровень
	SPCR=(0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);
}
//отправка байта 
void spi_sendByte(uint8_t byte)
{
	SPDR = byte;				//записываем байт в регистр
	while(!(SPSR & (1<<SPIF)));	//подождем пока данные передадутся
}
//прием/отправка байта
uint8_t spi_changeByte(uint8_t byte)
{
	SPDR = byte;				//записываем байт в регистр
	while(!(SPSR & (1<<SPIF)));	//подождем пока данные передадутся (обменяются)
	return SPDR;				//возвращаем принятое значение
}