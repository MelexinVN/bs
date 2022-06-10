/*
 * rx_433_stm32_hal.c
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

extern UART_HandleTypeDef huart1;

extern volatile uint8_t listening;		//флаг принятого пакета
uint8_t bit_array[SIZE_ARRAY] = {0};		//массив принятых битов
uint8_t dev_addr = 0;									//адрес устройства
uint8_t tx_addr = 0;									//адрес передатчика
uint8_t command = 0;									//команда
uint8_t crc = 0;											//контрольная сумма
uint8_t preamble_count = 0;						//счетчик импульсов преамбулы
uint32_t last_change = 0;							//последнее изменение времени
uint8_t bit_counter = 0;							//счетчик битов
extern char str[];										//строка для вывода данных в порт

void RX433_Int()
{
	uint32_t cur_timestamp = micros();																//получаем текущее значение микросекунд
	uint8_t  cur_status = HAL_GPIO_ReadPin(SIGNAL_PORT, SIGNAL_PIN);	//определение текущего состояния входа
	uint32_t pulse_duration = cur_timestamp - last_change;						//определение длительности прошедшего импульса
	last_change = cur_timestamp;																			//сохраняем значение времени

	  if(preamble_count < PREAM_NUM_IMP)															//если преамбулы еще не было
	  {
	    if(cur_status == 1)																						//если текущее состояние вывода - 1, прошедший импульс был 0
	    {
	      if(((200 < pulse_duration) && (pulse_duration < 400)) || preamble_count == 0)
					{}	//если длительность между 200 и 400 мкс или импульсов преамбулы еще не было, падаем дальше
	      else//иначе
	      {
	        preamble_count = 0;		//обнуляем счетчик импульсов преамбулы
	        return;								//выход из подпрограммы
	      }
	    }	
	    else		//если прошедший импульс был 1
	    {
	      if((200 < pulse_duration) && (pulse_duration < 400))				//если длительность от 200 до 400 мкс
	      {																														//пришел первый импульс преамбулы
	        preamble_count++;																					//считаем импульс преамбулы

	        if(preamble_count == PREAM_NUM_IMP)												//если пришел последний импульс преамбулы
	        {
	          bit_counter = 0;				//обнуляем счетчик битов
	          return;									//выход из подпрограммы
	        }
	      }
	      else												//если пришедший импульс выбивается из диапазона
	      {
	        preamble_count = 0;				//обнуляем счетчик импульсов преамбулы
	        return;										//выход из подпрограммы
	      }
	    }
	  }

	  if(preamble_count == PREAM_NUM_IMP)		//если приамбула принята полностью
	  {
			//snprintf(str, 64, "%d\r\n",bit_counter);
			//HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
	    if(cur_status == 1)//если предыдущий интервал был 0
	    {
 	      if(((200 < pulse_duration) && (pulse_duration < 300)) || bit_counter == 0)  
					{}//если нулевой импульс от 200 до 300 мкс или еще не было информационных битов, падаем дальше
	      else
	      {//если импульс вне пределов 
	        preamble_count = 0;		//обнуляем счетчик импульсов преамбулы
	        return;								//выход из подпрограммы
	      }
	    }
	    else	//если предыдущий импульс был 1
	    {
	      if((200 < pulse_duration) && (pulse_duration < 800))	//если импульс от 200 до 800
	      {
					bit_array[bit_counter] = (pulse_duration < DURATION) ? 0 : 1;  //если импульс короткий, записываем в массив 0, если длинный - 1
	        
					bit_counter++;		//считаем импульс
					
					snprintf(str, 64, "bit = %d\r\n", bit_counter);
					HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
					
					if (bit_counter == SIZE_ARRAY + 1)	//если приняты все байты
					{
						//сохранение адреса устройства
						for(uint8_t i = 0; i < 8; i++)		//8 раз
						{
							dev_addr = (dev_addr << 1) + bit_array[i]; //сдвигаем значение влево и прибавляем текущее значение бита
						}
						//сохранение адреса передатчика
						for(uint8_t i = 9; i < 16; i++)
						{
							tx_addr = (tx_addr << 1) + bit_array[i];
						}
						//сохранение команды
						for(uint8_t i = 17; i < 24; i++)
						{
							command = (command << 1) + bit_array[i];
						}
						//
						for(uint8_t i = 25; i < 32; i++)
						{
							crc = (crc << 1) + bit_array[i];
						}
						
						uint8_t crc_calc = 0;
						crc_calc = (uint8_t)(dev_addr + tx_addr + command);
						crc_calc = ~crc_calc + 1;
						preamble_count = 0;
						snprintf(str, 64, "crc = 0x%X \t crc_calc = 0x%X\r\n", crc,crc_calc);
						HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);
						snprintf(str, 64, "addr: 0x%X \r\n tx_addr: 0x%X \r\n command: 0x%X\r\n crc = 0x%X\r\n", dev_addr, tx_addr, command, crc);
						HAL_UART_Transmit(&huart1,(uint8_t*)str,strlen(str),0x1000);

						if (crc == crc_calc) listening = 1;
						
						HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); // отключаем прерывания 433
	        }
	      }
	      else
	      {
	        preamble_count = 0;
	        return;
	      }
	    }
	  }
}





















