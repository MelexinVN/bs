#include "nrf24.h"
#include "main.h"
//------------------------------------------------

#define BUT												//����� ����������: BAZ - ����, BUT - ������

#define TX_ADR_WIDTH 3						//������ ������ ��������
#define TX_PLOAD_WIDTH 5					//������ �������� ��������

#ifdef BAS
uint8_t TX_ADDRESS0[TX_ADR_WIDTH] = {0xb7,0xb5,0xa1};	//����� 0
uint8_t TX_ADDRESS1[TX_ADR_WIDTH] = {0xb5,0xb5,0xa1};	//����� 1
#endif

#ifdef BUT
uint8_t TX_ADDRESS0[TX_ADR_WIDTH] = {0xb5,0xb5,0xa1};	//����� 0
uint8_t TX_ADDRESS1[TX_ADR_WIDTH] = {0xb7,0xb5,0xa1};	//����� 1
#endif

uint8_t rx_buf[TX_PLOAD_WIDTH] = {0};			//�������� �����
uint8_t tx_buf[TX_PLOAD_WIDTH] = {0};			
volatile uint8_t rx_flag = 0, tx_flag = 0;	//����� ������ � ��������
extern uint8_t pushed;											//
extern uint8_t f_send;											//���� ��������
extern uint32_t time_ms;
extern uint32_t milisec;
extern uint8_t buf1[20];										//�����
extern char str1[20];												//������ ��� ������ ������
//����������� ������� �������������� ��������
//------------------------------------------------
__STATIC_INLINE void DelayMicro(__IO uint32_t micros)
{
  micros *= (SystemCoreClock / 1000000) / 9;	//�������� (9) ������� ����������������
  /* Wait till done */
  while (micros--) ;
}
//������� ������ �������� ������
uint8_t NRF24_ReadReg(uint8_t addr)
{
  uint8_t dt=0, cmd;												//���������� ������ � �������
  CS_ON;																		//���� cs � �����
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	//���� ���� �� ���������� ���� txe (���������� � ��������)
  LL_SPI_TransmitData8 (SPI1, addr);				//�������� ���� ������ �� spi
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {} //���� ���� �� ���������� ���� rxne (���� ������ �� �����)
	dt = LL_SPI_ReceiveData8(SPI1);						//��������� ������ �� spi
	//���� ����� ����� ����� �������� ������ �� � ���������� ��� ���������	
  if (addr!=STATUS)//� ���� �� �����
  {
    cmd=0xFF;				//������� NOP ��� ��������� ������
		while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	
		LL_SPI_TransmitData8 (SPI1, cmd);					//����� ������ �������
		while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}	
		dt = LL_SPI_ReceiveData8(SPI1);						//�������� ������, 1 ����
  }
  CS_OFF;																			//��������� ���� CS
  return dt;	//������������ ��������
}
//------------------------------------------------
void NRF24_WriteReg(uint8_t addr, uint8_t dt)		//������ ��������
{
  addr |= W_REGISTER;//������� ��� ������ � �����	
  CS_ON;
  while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, addr);				//���������� ����� � �������� ������
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}
	(void) SPI1->DR;													//������ ������� DR, �������� ������, ������� ����������� ���������� ������������ � �������
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, dt);					//������ ������
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;													//�������� ������
  CS_OFF;
}
//------------------------------------------------
void NRF24_ToggleFeatures(void)							//��������� ������ R_RX_PL_WID, W_ACK_PAYLOAD � W_TX_PAYLOAD_NOACK
{	//���� � �������� "��� �����"
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
{//������ ������ (��������� ����)
  CS_ON;
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	
	LL_SPI_TransmitData8 (SPI1, addr);				//������� ����� �����
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;													//��������� �����
	for (uint8_t i = 0 ; i < bytes ; i++) 		//��� ������� ���������� ����
	{																					//
		while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}//
		LL_SPI_TransmitData8 (SPI1, addr);			//���������� ������ ��� �����
		while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}	
		pBuf[i] = LL_SPI_ReceiveData8(SPI1);		//������ ��������� ����
	}
  CS_OFF;
}
//------------------------------------------------
void NRF24_Write_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)	
{//������ ������
  addr |= W_REGISTER;//������� ��� ������ � �����
  CS_ON;
  while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8(SPI1, addr);					//���������� ����� ��������, ������ ����� ������
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  DelayMicro(1);
  for (uint8_t i = 0 ; i < bytes ; i++) 		//��� ������� ���������� ����
	{
		while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	//
		LL_SPI_TransmitData8(SPI1, pBuf[i]);			//���������� ��������� ����
		while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
		(void) SPI1->DR;	
	}
	CS_OFF;
}
//------------------------------------------------
void NRF24_FlushRX(void)
{//������� ������ ������
  uint8_t dt[1] = {FLUSH_RX};
  CS_ON;
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, dt[0]);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  DelayMicro(1);//����� � ������������ ��� ���������� ��������
  CS_OFF;
}
//------------------------------------------------
void NRF24_FlushTX(void)
{//������� ������ ��������
  uint8_t dt[1] = {FLUSH_TX};
  CS_ON;
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, dt[0]);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  DelayMicro(1);//����� � ������������ ��� ���������� ��������
  CS_OFF;
}
//------------------------------------------------
void NRF24L01_RX_Mode(void)
{//����� ���������
  uint8_t regval=0x00;
  regval = NRF24_ReadReg(CONFIG);	//��������� �������� �������� ������������
  //�������� ������ � �������� ��� � ����� ��������, ������� ���� PWR_UP � PRIM_RX
  regval |= (1<<PWR_UP)|(1<<PRIM_RX);
  NRF24_WriteReg(CONFIG,regval);	//���������� �������� �������� �������
	
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS1, TX_ADR_WIDTH);		//����������  ����� �����������
	NRF24_Write_Buf(RX_ADDR_P0, TX_ADDRESS1, TX_ADR_WIDTH);	//���������� ����� ���������
	
  CE_SET;
  DelayMicro(150); //�������� ������� 130 ���
  // Flush buffers
  NRF24_FlushRX();
  NRF24_FlushTX();
}
//------------------------------------------------
void NRF24L01_TX_Mode(uint8_t *pBuf)
{//����� �����������
  NRF24_Write_Buf(TX_ADDR, TX_ADDRESS0, TX_ADR_WIDTH);		//���������� ����� �����������
	NRF24_Write_Buf(RX_ADDR_P0, TX_ADDRESS0, TX_ADR_WIDTH);	//���������� ����� ���������
  CE_RESET;
  // Flush buffers
  NRF24_FlushRX();
  NRF24_FlushTX();
}

void NRF24_Transmit(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{//�������� ������ � ������
  CE_RESET;
  CS_ON;
	while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}
	LL_SPI_TransmitData8 (SPI1, addr);				//���������� ����� ��������
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
	(void) SPI1->DR;
  DelayMicro(1);
  for (uint8_t i = 0 ; i < bytes ; i++) 
	{
		while(!LL_SPI_IsActiveFlag_TXE(SPI1)) {}	
		LL_SPI_TransmitData8(SPI1, pBuf[i]);		//���������� ������
		while(!LL_SPI_IsActiveFlag_RXNE(SPI1)) {}		
		(void) SPI1->DR;	
	}
  CS_OFF;
  CE_SET;
}
//------------------------------------------------
uint8_t NRF24L01_Send(uint8_t *pBuf)
{//�������� ������ � ����
  uint8_t regval=0x00;						//���������� ��� �������� � ���������������� �������
	NRF24L01_TX_Mode(pBuf);
	regval = NRF24_ReadReg(CONFIG);	//��������� �������� ����������������� �������
	//���� ������ ���� � ������ �����, �� �������� ���, ������� ��� PWR_UP � �������� PRIM_RX
	regval |= (1<<PWR_UP);
	regval &= ~(1<<PRIM_RX);
	NRF24_WriteReg(CONFIG,regval);//���������� ����� �������� ����������������� ��������
	DelayMicro(150); //�������� ������� 130 ���
	//�������� ������ � ������
	NRF24_Transmit(WR_TX_PLOAD, pBuf, TX_PLOAD_WIDTH);//�������� ������
	CE_SET;
	DelayMicro(15); //minimum 10us high pulse (Page 21)
	CE_RESET;
	return 0;
}
//------------------------------------------------
void NRF24L01_Receive(void)
{
	if(rx_flag==1)				//���� ���� ������ ������
	{
		rx_flag = 0;				//�������� ���� ������		
		if (rx_buf[0] == 0xFF)
		{
			pushed = 0;
			time_ms = 0;
			milisec = 0;
			LED_OFF;
		}
		if (rx_buf[0] == BUT_ADDR)
		{
			if (pushed)
			{
				tx_buf[0] = BUT_ADDR;
				(*(unsigned long*)&tx_buf[1]) = time_ms;
				DelayMicro(99);		//��������� ����������������!
				NRF24L01_Send(tx_buf);				
			}
			else
			{
				tx_buf[0] = BUT_ADDR;
				(*(unsigned long*)&tx_buf[1]) = 0xFFFFFFFF;
				DelayMicro(99);		//��������� ����������������!
				NRF24L01_Send(tx_buf);
			}
			if (rx_buf[1] == 0x01)
			{
				if(rx_buf[2] == 0x01) LED_ON;
				if(rx_buf[2] == 0x00) LED_OFF;
			}
		}
	}
}

//------------------------------------------------
void NRF24_init(void)
{//�������������
	CE_RESET;						//�������� � ����� ����� ce
  DelayMicro(5000);		//�������� 5 ��
	//���������� ���������������� ����, 
	NRF24_WriteReg(CONFIG, 0x0a); // Set PWR_UP bit, enable CRC(1 byte) &Prim_RX:0 (Transmitter)
  DelayMicro(5000);
	NRF24_WriteReg(EN_AA, 0x01); // Enable Pipe0
	NRF24_WriteReg(EN_RXADDR, 0x01); // Enable Pipe0
	NRF24_WriteReg(SETUP_AW, 0x01); // Setup address width=3 bytes
	NRF24_WriteReg(SETUP_RETR, 0x5F); // // 1500us, 15 retrans
	NRF24_ToggleFeatures();//��������� ������
	NRF24_WriteReg(FEATURE, 0);//��������� ����������� �������� �������� FEATURE 
	NRF24_WriteReg(DYNPD, 0);//���������� ������������� ������� �������� ��������
	NRF24_WriteReg(STATUS, 0x70); //Reset flags for IRQ
	NRF24_WriteReg(RF_CH, 76); // ������� 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x06); //TX_PWR:0dBm, Datarate:1Mbps
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS0, TX_ADR_WIDTH);//������ ������ ��������
	NRF24_Write_Buf(RX_ADDR_P1, TX_ADDRESS0, TX_ADR_WIDTH);//������ ������ ������
	NRF24_WriteReg(RX_PW_P0, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data pipe 1
 //���� ������ � ����� ��������
  NRF24L01_RX_Mode();	//����� ������
  LED_OFF;
}
//--------------------------------------------------
void IRQ_Callback(void)
{
	uint8_t status=0x01;	//���������� ������
  DelayMicro(10);
  status = NRF24_ReadReg(STATUS);	//������ �������� �������� �������
  if(status & RX_DR)							//���� ���� ������ �� �����
  {
    NRF24_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);//������ ������
    NRF24_WriteReg(STATUS, 0x40);	//������ � ������� ������� 1 � ������ ���, ��������� ���������
    rx_flag = 1;									//��������� ���� ������
  }
  if(status & TX_DS) //������ ������� ����������
  {
    NRF24_WriteReg(STATUS, 0x20);	//������� ���� ����� ����� ������
    NRF24L01_RX_Mode();						//������� � ����� ������
    tx_flag = 1;									//��������� ���� ��������
  }
  else if(status & MAX_RT)//���������� ���������� ������� ��������
  {
    NRF24_WriteReg(STATUS, 0x10);//��������� ���� ��������� �����, ����� 4��
    NRF24_FlushTX();			//������� ������ ��������
    //������ � ����� ��������
    NRF24L01_RX_Mode();
  }
}
