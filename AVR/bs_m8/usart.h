#ifndef USART_H_
#define USART_H_

#include "main.h"

void USART_Init( unsigned int ubrr);
void USART_Transmit( unsigned char data );
void usart_println(char *str);

#endif /* USART_H_ */