#ifndef NRF24_H_
#define NRF24_H_
//------------------------------------------------
#include "main.h"
//------------------------------------------------
#define CS_ON LL_GPIO_ResetOutputPin(CSN_GPIO_Port, CSN_Pin)
#define CS_OFF LL_GPIO_SetOutputPin(CSN_GPIO_Port, CSN_Pin)
#define CE_RESET LL_GPIO_ResetOutputPin(CE_GPIO_Port, CE_Pin)
#define CE_SET LL_GPIO_SetOutputPin(CE_GPIO_Port, CE_Pin)
#define LED_ON LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin)
#define LED_OFF LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin)
#define LED_TGL LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin)
#define IRQ LL_GPIO_IsInputPinSet(IRQ_GPIO_Port, IRQ_Pin)
//------------------------------------------------
#define ACTIVATE 		0x50 //
#define RD_RX_PLOAD 0x61 // Define RX payload register address
#define WR_TX_PLOAD 0xA0 // Define TX payload register address
#define FLUSH_TX 		0xE1
#define FLUSH_RX 		0xE2
//------------------------------------------------
#define CONFIG 			0x00 //'Config' register address
#define EN_AA 			0x01 //'Enable Auto Acknowledgment' register address
#define EN_RXADDR 	0x02 //'Enabled RX addresses' register address
#define SETUP_AW 		0x03 //'Setup address width' register address
#define SETUP_RETR 	0x04 //'Setup Auto. Retrans' register address
#define RF_CH 			0x05 //'RF channel' register address
#define RF_SETUP 		0x06 //'RF setup' register address
#define STATUS 			0x07 //'Status' register address
#define OBSERVE_TX 	0x08 //'Transmit observe' register
#define RX_ADDR_P0 	0x0A //'RX address pipe0' register address
#define RX_ADDR_P1 	0x0B //'RX address pipe1' register address
#define TX_ADDR 		0x10 //'TX address' register address
#define RX_PW_P0 		0x11 //'RX payload width, pipe0' register address
#define RX_PW_P1 		0x12 //'RX payload width, pipe1' register address
#define FIFO_STATUS 0x17 //'FIFO Status Register' register address
#define DYNPD 			0x1C
#define FEATURE 		0x1D
//------------------------------------------------
#define PRIM_RX 		0x00 //RX/TX control (1: PRX, 0: PTX)
#define PWR_UP 			0x01 //1: POWER UP, 0:POWER DOWN
#define RX_DR 			0x40 //Data Ready RX FIFO interrupt
#define TX_DS 			0x20 //Data Sent TX FIFO interrupt
#define MAX_RT 			0x10 //Maximum number of TX retransmits interrupt
//------------------------------------------------
#define W_REGISTER 	0x20 //������ � �������
//------------------------------------------------
void NRF24_init(void);
uint8_t NRF24_ReadReg(uint8_t addr);
void NRF24_Read_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes);
uint8_t NRF24L01_Send(uint8_t *pBuf);
void NRF24L01_Receive(void);
void IRQ_Callback(void);

//------------------------------------------------
#endif /* NRF24_H_ */
