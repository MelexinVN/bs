#include "usart.h"
#include "main.h"

void USART_Init( unsigned int ubrr)//Инициализация модуля USART
{
		//Зададим скорость работы USART
		UBRR0H = (unsigned char)(ubrr>>8);
		UBRR0L = (unsigned char)ubrr;
		
		UCSR0B=(1<<RXEN0)|( 1<<TXEN0); //Включаем прием и передачу по USART
		UCSR0B |= (1<<RXCIE0); //Разрешаем прерывание при передаче
		UCSR0A |= (1<<U2X0); // Для 8 мгц
		UCSR0C = (0<<UMSEL01)|(0<<UMSEL00)|(1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);// Обращаемся именно к регистру UCSRC (URSEL=1),
		//ассинхронный режим (UMSEL=0), без контроля четности (UPM1=0 и UPM0=0),
		//1 стоп-бит (USBS=0), 8-бит посылка (UCSZ1=1 и UCSZ0=1)
}

void USART_Transmit( unsigned char data ) //Функция отправки данных
{
	while ( !(UCSR0A & (1<<UDRE0)) ); //Ожидание опустошения буфера приема
	UDR0 = data; //Начало передачи данных
}

void usart_println(char *str)
{
	for (int i = 0; i < strlen(str); i++)
	{
		USART_Transmit(str[i]); //Функция отправки данных
	}
	USART_Transmit(0x0d);//переход в начало строки
	USART_Transmit(0x0a);//переход на новую строку
}