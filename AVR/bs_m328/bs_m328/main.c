#include <atmel_start.h>
#include <util/delay.h>

#include <stdio.h>
#include <string.h>
#include <atmel_start.h>
#include <usart_basic_example.h>
#include <usart_basic.h>
#include <atomic.h>

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	/* Replace with your application code */
	while (1) 
	{
		_delay_ms(1000);
		USART_0_test_usart_basic();
		PORTB_toggle_pin_level(5);
	}
}
