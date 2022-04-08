#include <atmel_start.h>
#include <stdio.h>
#include <string.h>
#include <atmel_start.h>
#include <usart_basic_example.h>
#include <usart_basic.h>
#include <atomic.h>
#include <util/delay.h>

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	/* Replace with your application code */
	while (1) 
	{
		USART_0_test_usart_basic();
		_delay_ms(500);
		PORTD_toggle_pin_level(6);
	}
}
