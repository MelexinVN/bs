#include "usart.h"
#include "main.h"

void USART_Init( unsigned int ubrr)//������������� ������ USART
{
		//������� �������� ������ USART
		UBRR0H = (unsigned char)(ubrr>>8);
		UBRR0L = (unsigned char)ubrr;
		
		UCSR0B=(1<<RXEN0)|( 1<<TXEN0); //�������� ����� � �������� �� USART
		UCSR0B |= (1<<RXCIE0); //��������� ���������� ��� ��������
		UCSR0A |= (1<<U2X0); // ��� 8 ���
		UCSR0C = (0<<UMSEL01)|(0<<UMSEL00)|(1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);// ���������� ������ � �������� UCSRC (URSEL=1),
		//������������ ����� (UMSEL=0), ��� �������� �������� (UPM1=0 � UPM0=0),
		//1 ����-��� (USBS=0), 8-��� ������� (UCSZ1=1 � UCSZ0=1)
}

void USART_Transmit( unsigned char data ) //������� �������� ������
{
	while ( !(UCSR0A & (1<<UDRE0)) ); //�������� ����������� ������ ������
	UDR0 = data; //������ �������� ������
}

void usart_println(char *str)
{
	for (int i = 0; i < strlen(str); i++)
	{
		USART_Transmit(str[i]); //������� �������� ������
	}
	USART_Transmit(0x0d);//������� � ������ ������
	USART_Transmit(0x0a);//������� �� ����� ������
}