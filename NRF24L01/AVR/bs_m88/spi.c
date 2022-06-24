#include "spi.h"
#include "main.h"
//������������� SPI
void spi_init(void)
{
	DDRB |= ((1<<PB2)|(1<<PB3)|(1<<PB5)); //����� SPI �� �����
	PORTB &= ~((1<<PB2)|(1<<PB3)|(1<<PB5)); //������ �������
	SPCR=(0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);
}
//�������� ����� 
void spi_sendByte(uint8_t byte)
{
	SPDR = byte;				//���������� ���� � �������
	while(!(SPSR & (1<<SPIF)));	//�������� ���� ������ �����������
}
//�����/�������� �����
uint8_t spi_changeByte(uint8_t byte)
{
	SPDR = byte;				//���������� ���� � �������
	while(!(SPSR & (1<<SPIF)));	//�������� ���� ������ ����������� (����������)
	return SPDR;				//���������� �������� ��������
}