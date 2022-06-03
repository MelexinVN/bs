/*
 * rx_433_stm32_hal.c.c
 * организация приема данных по радиоканалу 433/315 Мгц
 * формат посылки:
 * - преамбула 8-16 импульсов NUM_PREAM_IMP длительностью DUR_PRAM_IMP
 * - байты согласно протоколу (адрес приемника, адрес передатчика, команда)
 * - контрольная сумма intel hex (инвертированная сумма байтов + 1)
 *      
 * MelexinVN
 *
 */
 
#include "rx_433_stm32_hal.h"

extern volatile uint8_t hcs_listening;

uint8_t BitArray[SIZE_ARRAY] = {0,};
uint8_t Repeat = 0;
uint8_t BatteryLow = 0;
//uint8_t Btn2 = 0;
//uint8_t Btn1 = 0;
//uint8_t Btn0 = 0;
//uint8_t Btn3 = 0;
unsigned long SerialNum = 0;
unsigned long  Encrypt = 0;
uint8_t Button = 0;

uint8_t HCS_preamble_count = 0;
uint32_t HCS_last_change = 0;
uint8_t HCS_bit_counter = 0;



void RX433_Int()
{
	  uint32_t cur_timestamp = micros();
	  uint8_t  cur_status = HAL_GPIO_ReadPin(SIGNAL_PORT, SIGNAL_PIN);
	  uint32_t pulse_duration = cur_timestamp - HCS_last_change;
	  HCS_last_change = cur_timestamp;

	  if(HCS_preamble_count < 12)
	  {
	    if(cur_status == 1)
	    {
	      if(((150 < pulse_duration) && (pulse_duration < 500)) || HCS_preamble_count == 0){}
	      else
	      {
	        HCS_preamble_count = 0;
	        return;
	      }
	    }
	    else
	    {
	      if((300 < pulse_duration) && (pulse_duration < 600))
	      {
	        HCS_preamble_count++;

	        if(HCS_preamble_count == 12)
	        {
	          HCS_bit_counter = 0;
	          return;
	        }
	      }
	      else
	      {
	        HCS_preamble_count = 0;
	        return;
	      }
	    }
	  }


	  if(HCS_preamble_count == 12)
	  {
	    if(cur_status == 1)
	    {
	      if(((250 < pulse_duration) && (pulse_duration < 900)) || HCS_bit_counter == 0)  {}
	      else
	      {
	        HCS_preamble_count = 0;
	        return;
	      }
	    }
	    else
	    {
	      if((250 < pulse_duration) && (pulse_duration < 900))
	      {
	    	BitArray[65 - HCS_bit_counter] = (pulse_duration > HCS_Te2_3) ? 0 : 1;  //если длительность порога бит = 0, если меньше = 1
	        HCS_bit_counter++;

	        if(HCS_bit_counter == 66)
	        {
	        	Repeat = BitArray[0];
				BatteryLow = BitArray[1];
				//Btn2 = BitArray[2];
				//Btn1 = BitArray[3];
				//Btn0 = BitArray[4];
				//Btn3 = BitArray[5];

				for(uint8_t i = 2; i < 6; i++)
				{
					Button = (Button << 1) + BitArray[i];
				}

				for(uint8_t i = 6; i < 34; i++)
				{
					SerialNum = (SerialNum << 1) + BitArray[i];
				}

				for(uint8_t i = 34; i < 66; i++)
				{
					Encrypt = (Encrypt << 1) + BitArray[i];
				}

				HCS_preamble_count = 0;
				hcs_listening = 1;
				HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); // отключаем прерывания 433
	        }
	      }
	      else
	      {

	        HCS_preamble_count = 0;
	        return;
	      }
	    }
	  }
}





















