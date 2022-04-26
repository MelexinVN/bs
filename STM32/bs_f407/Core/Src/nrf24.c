#include "nrf24.h"
#include "main.h"
//------------------------------------------------
#define TX_ADR_WIDTH 3						//размер адреса передачи
#define TX_PLOAD_WIDTH 5					//размер полезной нагрузки

#define BAS							//выбор устройства: BAZ - база, BUT - кнопка

#ifdef BAS
uint8_t TX_ADDRESS0[TX_ADR_WIDTH] = {0xb7,0xb5,0xa1};	//адрес 0
uint8_t TX_ADDRESS1[TX_ADR_WIDTH] = {0xb5,0xb5,0xa1};	//адрес 1
#endif

#ifdef BUT
uint8_t TX_ADDRESS0[TX_ADR_WIDTH] = {0xb5,0xb5,0xa1};	//адрес 0
uint8_t TX_ADDRESS1[TX_ADR_WIDTH] = {0xb7,0xb5,0xa1};	//адрес 1
#endif

uint8_t rx_buf[TX_PLOAD_WIDTH] = {0};				//приемный буфер
volatile uint8_t rx_flag = 0, tx_flag = 0;	//флаги приема и передачи
extern char str[];												//строка для вывода данных
extern uint8_t f_receive;
extern uint32_t but_times[NUM_OF_BUTS];
extern uint8_t but_addrs[];
//самодельная функция микросекундной задержки
//------------------------------------------------
__STATIC_INLINE void DelayMicro(__IO uint32_t micros)
{
  micros *= (SystemCoreClock / 1000000) / 9;	//делитель (9) получен экспериментально
  /* Wait till done */
  while (micros--) ;
}
//функция чтения регистра модуля
uint8_t NRF24_ReadReg(uint8_t addr)
{
  uint8_t dt=0, cmd;												//переменные данных и команды
  CS_ON;																		//ногу cs к земле
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	//ждем пока не поднимется флаг txe (готовность к передаче)
  LL_SPI_TransmitData8 (SPI1, addr);				//передаем байт адреса по spi
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {} //ждем пока не поднимется флаг rxne (есть данные на прием)
	dt = LL_SPI_ReceiveData8(SPI1);						//принимаем данные по spi
	//если адрес равен адрес регистра статус то и возварщаем его состояние	
  if (addr!=STATUS)//а если не равен
  {
    cmd=0xFF;				//команда NOP для получения данных
		while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	
		LL_SPI_TransmitData8 (SPI1, cmd);					//пишем пустую команду
		while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}	
		dt = LL_SPI_ReceiveData8(SPI1);						//получаем данные, 1 байт
  }
	CS_OFF;																			//поднимаем ногу CS
  return dt;	//возвращаемое значение
}
//------------------------------------------------
void NRF24_WriteReg(uint8_t addr, uint8_t dt)		//запись регистра
{
  addr |= W_REGISTER;//включим бит записи в адрес	
  CS_ON;
  while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, addr);				//записываем адрес с командой записи
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}
	(void) SPI1->DR;													//читаем регистр DR, имитация приема, который обязательно происходит одновременно с записью
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, dt);					//запись данных
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;													//имитация чтения
	CS_OFF;
}
//------------------------------------------------
void NRF24_ToggleFeatures(void)							//активация команд R_RX_PL_WID, W_ACK_PAYLOAD и W_TX_PAYLOAD_NOACK
{	//есть в даташите "без плюса"
  uint8_t dt[1] = {ACTIVATE};
  CS_ON;
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, dt[0]);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  DelayMicro(1);
  dt[0] = 0x73;
  while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, dt[0]);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  CS_OFF;
}
//-----------------------------------------------
void NRF24_Read_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{//чтение буфера (несколько байт)
  CS_ON;
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	
	LL_SPI_TransmitData8 (SPI1, addr);				//сначала пишем адрес
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;													//имитируем прием
  DelayMicro(1);	
		for (uint8_t i = 0 ; i < bytes ; i++) 		//для нужного количества байт
	{																					//
		while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}//
		LL_SPI_TransmitData8 (SPI1, addr);			//записываем каждый раз адрес
		while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}	
		pBuf[i] = LL_SPI_ReceiveData8(SPI1);		//читаем очередной байт
	}
  CS_OFF;
}
//------------------------------------------------
void NRF24_Write_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)	
{//запись буфера
  addr |= W_REGISTER;//включим бит записи в адрес
  CS_ON;
  while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8(SPI1, addr);					//записываем адрес регистра, откуда будем читать
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  DelayMicro(1);
  for (uint8_t i = 0 ; i < bytes ; i++) 		//для нужного количества байт
	{
		while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	//
		LL_SPI_TransmitData8(SPI1, pBuf[i]);			//отправляем очередной байт
		while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
		(void) SPI1->DR;	
	}
	CS_OFF;
}
//------------------------------------------------
void NRF24_FlushRX(void)
{//очистка буфера приема
  uint8_t dt[1] = {FLUSH_RX};
  CS_ON;
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, dt[0]);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  DelayMicro(1);//пауза в микросекунду для завершения процесса
  CS_OFF;
}
//------------------------------------------------
void NRF24_FlushTX(void)
{//очистка буфера передачи
  uint8_t dt[1] = {FLUSH_TX};
  CS_ON;
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, dt[0]);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  DelayMicro(1);//пауза в микросекунду для завершения процесса
  CS_OFF;
}
//------------------------------------------------
void NRF24L01_RX_Mode(void)
{//режим приемника
  uint8_t regval=0x00;
  regval = NRF24_ReadReg(CONFIG);	//сохраняем значение регистра конфигурации
  //разбудим модуль и переведём его в режим приёмника, включив биты PWR_UP и PRIM_RX
  regval |= (1<<PWR_UP)|(1<<PRIM_RX);
  NRF24_WriteReg(CONFIG,regval);	//возвращаем значение регистра статуса
	
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS1, TX_ADR_WIDTH);		//записываем  адрес передатчика
	NRF24_Write_Buf(RX_ADDR_P0, TX_ADDRESS1, TX_ADR_WIDTH);	//записываем адрес приемника
	
  CE_SET;
  DelayMicro(150); //Задержка минимум 130 мкс
  // Flush buffers
  NRF24_FlushRX();
  NRF24_FlushTX();
}
//------------------------------------------------
void NRF24L01_TX_Mode(uint8_t *pBuf)
{//режим передатчика
  NRF24_Write_Buf(TX_ADDR, TX_ADDRESS0, TX_ADR_WIDTH);		//записываем адрес передатчика
	NRF24_Write_Buf(RX_ADDR_P0, TX_ADDRESS0, TX_ADR_WIDTH);	//записываем адрес приемника
  CE_RESET;
  // Flush buffers
  NRF24_FlushRX();
  NRF24_FlushTX();
}

void NRF24_Transmit(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{//передача данных в модуль
  CE_RESET;
  CS_ON;
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, addr);				//отправляем адрес регистра
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  DelayMicro(1);
  for (uint8_t i = 0 ; i < bytes ; i++) 
	{
		while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	
		LL_SPI_TransmitData8(SPI1, pBuf[i]);		//отправляем данные
		while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
		(void) SPI1->DR;	
	}
  CS_OFF;
  CE_SET;
}
//------------------------------------------------
uint8_t NRF24L01_Send(uint8_t *pBuf)
{//отправка данных в эфир
  uint8_t regval=0x00;						//переменная для отправки в конфигурационный регистр
	NRF24L01_TX_Mode(pBuf);
	regval = NRF24_ReadReg(CONFIG);	//сохраняем значения конфигурационного региста
	//если модуль ушел в спящий режим, то разбудим его, включив бит PWR_UP и выключив PRIM_RX
	regval |= (1<<PWR_UP);
	regval &= ~(1<<PRIM_RX);
	NRF24_WriteReg(CONFIG,regval);//записываем новое значение конфигурационного регистра
	DelayMicro(150); //Задержка минимум 130 мкс
	//Отправим данные в воздух
	NRF24_Transmit(WR_TX_PLOAD, pBuf, TX_PLOAD_WIDTH);//отправка данных
	CE_SET;
	DelayMicro(15); //minimum 10us high pulse (Page 21)
	CE_RESET;
	//LED_TGL;			
	return 0;
}
//------------------------------------------------

void nrf24l01_receive(void)
{//Обработка приема радиомодуля
	if(rx_flag == 1)				//если флаг приема поднят
	{
		if ((*(unsigned long*)&rx_buf[1]) != NOT_PUSH_CMD)
		{//если время, присланное кнопкой не максимально возможное (кнопка нажата)
			sprintf(str,"%X\t",rx_buf[0]);				//
			USART_TX((uint8_t*)str,strlen(str));	//отправляем в порт первый байт (адрес кнопки)
			unsigned long time = *(unsigned long*)&rx_buf[1];	//преобразуем оставшиеся байты во время
			sprintf(str,"%lu\r\n",time);//передаем принятое в порт
			USART_TX((uint8_t*)str,strlen(str));
			//определяем по адресу номер кнопки
			for (uint8_t i = 0; i < NUM_OF_BUTS; i++)
			{//перебираем все кнопки
				if (rx_buf[0] == but_addrs[i]) 	//если найден принятый адрес
				{
					but_times[i] = time;					//записываем время в массив времен
				}//не выходим из цикла при найденной кнопке чтобы для всех обработок были одинаковые тайминги
			}
		}
		else
		{//отправляем в порт, посылка от какой кнопки принята и что она не нажата
			sprintf(str,"%X\t np\r\n",rx_buf[0]);				//
			USART_TX((uint8_t*)str,strlen(str));
			for (uint8_t i = 0; i < NUM_OF_BUTS; i++)
			{//перебираем все кнопки
				if (rx_buf[0] == but_addrs[i]) 	//если найден принятый адрес
				{
					but_times[i] = MAX_TIME;			 	//записываем максимальное время в массив времен
				}//не выходим из цикла при найденной кнопке чтобы для всех обработок были одинаковые тайминги
			}
		}
		rx_flag = 0;		//опускаем флаг приема
	}
}

//------------------------------------------------
void NRF24_init(void)
{//инициализация
	CE_RESET;						//опускаем к земле вывод ce
  DelayMicro(5000);		//задержка 5 мс
	//записываем конфигурационный байт, 
	NRF24_WriteReg(CONFIG, 0x0a); // Set PWR_UP bit, enable CRC(1 byte) &Prim_RX:0 (Transmitter)
  DelayMicro(5000);
	NRF24_WriteReg(EN_AA, 0x00); // 0x01 Enable Pipe0 0x01
	NRF24_WriteReg(EN_RXADDR, 0x01); // Enable Pipe0
	NRF24_WriteReg(SETUP_AW, 0x01); // Setup address width=3 bytes
	NRF24_WriteReg(SETUP_RETR, 0x00); // // 0x5F 1500us, 15 retrans 
	NRF24_ToggleFeatures();//активация команд
	NRF24_WriteReg(FEATURE, 0x07);// 0 установка стандартных значений регистра FEATURE 
	NRF24_WriteReg(DYNPD, 0);//отключение динамического размера полезной нагрузки
	NRF24_WriteReg(STATUS, 0x70); //Reset flags for IRQ
	NRF24_WriteReg(RF_CH, 76); // частота 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x06); // 0x06 - TX_PWR:0dBm, Datarate:1Mbps
	//0x04 - TX_PWR:-6dBm / 0x02 - TX_PWR:-12dBm / 0x00 - TX_PWR:-18dBm
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS0, TX_ADR_WIDTH);//запись адреса передачи
	NRF24_Write_Buf(RX_ADDR_P1, TX_ADDRESS0, TX_ADR_WIDTH);//запись адреса приема
	NRF24_WriteReg(RX_PW_P0, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data pipe 1
 //пока уходим в режим приёмника
  NRF24L01_RX_Mode();	//режим приема
	LED_OFF;
}
//--------------------------------------------------
void IRQ_Callback(void)
{
	uint8_t status=0x01;	//переменная статус
  DelayMicro(10);
  status = NRF24_ReadReg(STATUS);	//чтение значения регистра статуса
	if(status & RX_DR)							//если есть данные на прием
  {
		NRF24_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);//чтение буфера
		NRF24_WriteReg(STATUS, 0x40);	//запись в регистр статуса 1 в шестой бит, обнуление остальных
    rx_flag = 1;									//поднимаем флаг приема
  }
	
  if(status & TX_DS) //данные успешно отправлены
  {
    //USART_TX((uint8_t*)"tx ok\r\n",7);
		NRF24_WriteReg(STATUS, 0x20);	//очистка всех битов кроме пятого
    NRF24L01_RX_Mode();						//переход в режим приема
  }
  
	else if(status & MAX_RT)//превышение количества попыток отправки
  {
		USART_TX((uint8_t*)"tx fl\r\n",7);
    NRF24_WriteReg(STATUS, 0x10);//однуление всех остальных битов, кроме 4го
    NRF24_FlushTX();			//очистка буфера отправки
    //Уходим в режим приёмника
    NRF24L01_RX_Mode();
  }
}
//--------------------------------------------------
void TIM2_Callback(void)
{

}
//--------------------------------------------------
