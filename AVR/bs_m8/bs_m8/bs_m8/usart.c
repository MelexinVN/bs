#include "usart.h"
#include "main.h"

void USART_Init( unsigned int ubrr)//Инициализация модуля USART
{
		//Зададим скорость работы USART
		UBRRH = (unsigned char)(ubrr>>8);
		UBRRL = (unsigned char)ubrr;
		
		UCSRB=(1<<RXEN)|( 1<<TXEN); //Включаем прием и передачу по USART
		UCSRB |= (1<<RXCIE); //Разрешаем прерывание при передаче
		UCSRA |= (1<<U2X); // Для 8 мгц
		UCSRC = (1<<URSEL)|(1<<USBS)|(1<<UCSZ1)|(1<<UCSZ0);// Обращаемся именно к регистру UCSRC (URSEL=1),
		//ассинхронный режим (UMSEL=0), без контроля четности (UPM1=0 и UPM0=0),
		//1 стоп-бит (USBS=0), 8-бит посылка (UCSZ1=1 и UCSZ0=1)
}

void USART_Transmit( unsigned char data ) //Функция отправки данных
{
	while ( !(UCSRA & (1<<UDRE)) ); //Ожидание опустошения буфера приема
	UDR = data; //Начало передачи данных
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