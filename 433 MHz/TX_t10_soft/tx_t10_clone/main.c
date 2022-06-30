/*
 * tx_t10.c
 *
 * Created: 27.06.2022 7:37:52
 * Author : VMelehin
 */ 

#include "main.h"

volatile uint8_t sleep_m = 0;			//���� ������ ����������������
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

//��������� �������� �������
void send_one(void)
{
	RF_HIGH();			//��������� ����
	_delay_us(750);	//����
	RF_LOW();			//�������� ����
	_delay_us(400);//����
}
//��������� �������� ���� (���������� �������)
void send_zero(void)
{
	RF_HIGH();
	_delay_us(400);
	RF_LOW();
	_delay_us(800);
}

 void send_rf(void)
 {
	_delay_ms(10);	//�����
	//���������
	preamble();
	_delay_us(3600);	//�����
	//������

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

	_delay_ms(10);	//�����
 }

 //��������� ����� � ����� ����������������
 void go_sleep(void)
 {
	 set_sleep_mode(SLEEP_MODE_PWR_DOWN);	//��������� ������ ����������������
	 sleep_enable();						//���������� ������ ����������������
	 sleep_cpu();							//��������� ������ ����������������
 }

 //��������� ��������� ���������� �� ������� ������
 ISR(PCINT0_vect)
 {
	 if (!(PINB & (1<<BUT_Pin))) //���� ������ ������ 2
	 {
		 LED_ON();						//�������� ��������� ���������
		 sleep_m = 0;					//�������� ���� ������ ����������������
		 while(!(PINB & (1<<BUT_Pin)))	//���� ������ �������� �������
		 {//�������� �� ������ ���������� ���������� ������� 2
			 send_rf();
		 }
	 }
	 else //��� ������ ������ ������
	 {
		 LED_OFF();		//��������� ���������
		 sleep_m = 1;	//��������� ���� ����������������
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

	sei();		//���������� ��������� ���� ����������
		
	go_sleep();	//������ � ����� ����������������

    while (1) 
    {
		if (sleep_m)		//���� ������ ���� ����������������
		{
			go_sleep();		//������ � ����� ����������������
			sleep_m = 0;	//�������� ���� ����������������
		}
    }
}

