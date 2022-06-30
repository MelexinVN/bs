/*
 * ���������� ���������� ���������� 433/315 ���
 * �����: MelexinVN / ������� �������� ���������� 
 * ����
 */

#include "rx_433.h"

extern volatile uint8_t f_received;		//���� ������� �������
volatile uint8_t bit_array[SIZE_ARRAY+1] = {0};	//������ �������� �����
uint8_t dev_addr = 0;									//����� ����������
uint8_t tx_addr = 0;									//����� �����������
uint8_t command = 0;									//�������
uint8_t crc = 0;											//����������� �����
uint8_t preamble_count = 0;						//������� ��������� ���������
uint32_t last_change = 0;							//��������� ��������� �������
uint8_t bit_counter = 0;							//������� �����
extern uint32_t ten_micros;

//��������� ������ ������� (����������� �� �������� ����������)
void RX_Input()
{

	wdt_reset();
	//�������� ������� �������� �����������
	uint32_t cur_timestamp = ten_micros;
	//����������� �������� ��������� �����
	uint8_t  cur_status = (RF_PIN_Port & (1<<RF_IN_Pin));
	//����������� ������������ ���������� ���������
	uint32_t pulse_duration = (cur_timestamp - last_change)*10;
	//��������� �������� �������
	last_change = cur_timestamp;
	//���� ��������� ��� �� ����
	if(preamble_count < PREAM_NUM_IMP)
	{//���� ������� ��������� ������ - 1, ��������� �������� ��� 0
		if(cur_status == 1)
		{//���� ������������  ��� ��������� ��������� ��� �� ����,
			if(((PREAM_IMP_DUR - INTERVAL < pulse_duration) && (pulse_duration < PREAM_IMP_DUR + INTERVAL)) || preamble_count == 0)
			{}	//������ ������
			else	//�����
			{
				preamble_count = 0;		//�������� ������� ��������� ���������
				return;								//����� �� ������������
			}
		}
		else		//���� ��������� ������� ��� 1
		{//���� ������������ � ������ ��������
			if((PREAM_IMP_DUR - INTERVAL < pulse_duration) && (pulse_duration < PREAM_IMP_DUR + INTERVAL))
			{//������ ������ ������� ���������
				preamble_count++;																					//������� ������� ���������

				if(preamble_count == PREAM_NUM_IMP)												//���� ������ ��������� ������� ���������
				{
					bit_counter = 0;				//�������� ������� �����
					return;									//����� �� ������������
				}
			}
			else												//���� ��������� ������� ���������� �� ���������
			{
				preamble_count = 0;				//�������� ������� ��������� ���������
				return;										//����� �� ������������
			}
		}
	}

	if(preamble_count == PREAM_NUM_IMP)		//���� ��������� ������� ���������
	{

		if(cur_status == 1)//���� ���������� �������� ��� 0
		{
			if(((ZERO_DUR - INTERVAL < pulse_duration) && (pulse_duration < ZERO_DUR + INTERVAL)) || bit_counter == 0)
			{}//���� ������� ������� � ��������� ��� ��� �� ���� �������������� �����, ������ ������
			else
			{//���� ������� ��� ���������
				preamble_count = 0;		//�������� ������� ��������� ���������
				return;								//����� �� ������������
			}
		}
		else	//���� ���������� ������� ��� 1
		{			//���� ������� � ���������
			if((ZERO_DUR - INTERVAL < pulse_duration) && (pulse_duration < ONE_DUR + INTERVAL))
			{	//���� ������� ��������, ���������� � ������ 0, ���� ������� - 1
				
				bit_array[SIZE_ARRAY - bit_counter - 1] = (pulse_duration < DURATION) ? 0 : 1;

				if (bit_counter >= 7) LED_ON();

				bit_counter++;		//������� �������
				


				if (bit_counter == SIZE_ARRAY)	//���� ������� ��� �����
				{
					//���������� ������ ����������
					for(uint8_t i = 0; i < 8; i++)		//8 ���
					{//�������� �������� ����� � ���������� ������� �������� ����
						crc = (crc << 1) + bit_array[i];
					}
					
					//���������� ������ �����������
					for(uint8_t i = 8; i < 16; i++)
					{//�������� �������� ����� � ���������� ������� �������� ����
						command = (command << 1) + bit_array[i];
					}

/*
					//���������� �������
					for(uint8_t i = 16; i < 24; i++)
					{//�������� �������� ����� � ���������� ������� �������� ����
						tx_addr = (tx_addr << 1) + bit_array[i];
					}*/
					
					//for(uint8_t i = 24; i < 32; i++)
					for(uint8_t i = 16; i < 24; i++)
					{//�������� �������� ����� � ���������� ������� �������� ����
						dev_addr = (dev_addr << 1) + bit_array[i];
					}
					
					uint8_t crc_calc = 0;		//���������� ��� ������� ����������� �����
					//������ ����������� �����
					//crc_calc = (uint8_t)(dev_addr + tx_addr + command);
					crc_calc = (uint8_t)(dev_addr + command);
					crc_calc = ~crc_calc + 1;
					preamble_count = 0;//�������� ������� ���������
					//���������� �������� � ������������ ����������� �����
					if (crc == crc_calc) f_received = 1;	//��������� ���� ������

					cli(); 	// ��������� ���������� 
				}
			}
			else
			{//���� ������� ��� ���������
				preamble_count = 0;	//�������� ������� ���������
				return;							//����� �� ������������
			}
		}
	}
}
