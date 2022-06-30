/*
 * tx_t10.c
 *
 * Created: 27.06.2022 7:37:52
 * Author : VMelehin
 */ 

#include "main.h"

volatile uint8_t sleep_m = 0;			//флаг режима энергосбережения
//volatile uint8_t data[66] = {1,1,0,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,0,1,1,1,0,1,1,0,0,1,1,0,1,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,1,1,0,1,0,0,0,0,1,1,1,0,1,1,0,1,1,0,0};			

void preamble(void)
{
	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);

	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);

	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);

	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);
	
	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);
	
	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);
	
	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);
	
	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);
	
	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);
	
	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);
	
	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);
	
	RF_HIGH();
	_delay_us(350);
	RF_LOW();
	_delay_us(400);
}

//процедура отправки единицы
void send_one(void)
{
	RF_HIGH();			//поднимаем ногу
	_delay_us(750);	//ждем
	RF_LOW();			//опускаем ногу
	_delay_us(400);//ждем
}
//процедура отправки нуля (аналогично единице)
void send_zero(void)
{
	RF_HIGH();
	_delay_us(400);
	RF_LOW();
	_delay_us(800);
}

 void send_rf(void)
 {
	_delay_ms(10);	//пауза
	//преамбула
	preamble();
	_delay_us(3600);	//пауза
	//данные

		send_one();		
		send_one();			
		send_zero();
		send_one();
		send_one();
		send_one();
		send_one();
		send_one();
		send_one();
		send_one();	
		send_zero();
		send_one();
		send_one();
		send_zero();
		send_one();
		send_one();
		send_one();
		send_one();
		send_zero();
		send_zero();
		send_one();
		send_one();
		send_one();
		send_zero();
		send_one();
		send_one();
		send_zero();
		send_zero();
		send_one();
		send_one();
		send_zero();
		send_one();	
		send_one();	
		send_zero();
		send_zero();
		send_zero();
		send_zero();
		send_one();	
		send_zero();
		send_zero();
		send_one();
		send_zero();
		send_one();
		send_zero();
		send_zero();
		send_one();
		send_one();
		send_one();
		send_one();	
		send_zero();
		send_one();
		send_zero();
		send_zero();
		send_zero();
		send_zero();
		send_one();
		send_one();
		send_one();
		send_zero();
		send_one();
		send_one();
		send_zero();
		send_one();
		send_one();
		send_zero();
		send_zero();

	_delay_ms(10);	//пауза
 }

 //процедура входа в режим энергосбережения
 void go_sleep(void)
 {
	 set_sleep_mode(SLEEP_MODE_PWR_DOWN);	//установка режима энергосбережения
	 sleep_enable();						//разрешение режима энергосбережения
	 sleep_cpu();							//включение режима энергосбережения
 }

 //процедура обработки прерывания по нажатию кнопок
 ISR(PCINT0_vect)
 {
	 if (!(PINB & (1<<BUT_Pin))) //если нажата кнопка 2
	 {
		 LED_ON();						//включаем отдельный светодиод
		 sleep_m = 0;					//опускаем флаг режима энергосбережения
		 while(!(PINB & (1<<BUT_Pin)))	//пока кнопка остается нажатой
		 {//отсылаем по адресу выбранного устройства команду 2
			 send_rf();
		 }
	 }
	 else //как только кнопка отжата
	 {
		 LED_OFF();		//отключаем светодиод
		 sleep_m = 1;	//поднимаем флаг энергосбережения
	 }
 }


int main(void)
{
    // Main Clock source: Calibrated Internal 8 MHz Osc.
    CCP=0xd8;
    CLKMSR=(0<<CLKMS1) | (0<<CLKMS0);
    // Clock Prescaler division factor: 1
    CCP=0xd8;
    CLKPSR=(0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);

	// Input/Output Ports initialization
	// Port B initialization
	// Pull-up initialization:
	// Bit3=Off Bit2=Off Bit1=Off Bit0=On
	PUEB=(0<<PUEB3) | (0<<PUEB2) | (0<<PUEB1) | (1<<PUEB0);
	// Function: Bit3=In Bit2=Out Bit1=Out Bit0=In
	DDRB=(0<<DDB3) | (1<<DDB2) | (1<<DDB1) | (0<<DDB0);
	// State: Bit3=T Bit2=0 Bit1=0 Bit0=P
	PORTB=(0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (1<<PORTB0);

	// External Interrupt(s) initialization
	// INT0: Off
	// Interrupt on any change on pins PCINT0-3: On
	EICRA=(0<<ISC01) | (0<<ISC00);
	EIMSK=(0<<INT0);
	PCMSK=(0<<PCINT3) | (0<<PCINT2) | (0<<PCINT1) | (1<<PCINT0);
	PCICR=(1<<PCIE0);
	PCIFR=(1<<PCIF0);

	sei();		//глобальное включение всех прерываний
		
	go_sleep();	//уходим в режим энергосбережения

    while (1) 
    {
		if (sleep_m)		//если поднят флаг энергосбережения
		{
			go_sleep();		//уходим в режим энергосбережения
			sleep_m = 0;	//опускаем флаг энергосбережения
		}
    }
}

