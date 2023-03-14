/*
 * NRF24L01.h
 *
 *  Created on: Mar 12, 2023
 *      Author: Sebastian Colom
 */

#ifndef INC_NRF24L01_H_
#define INC_NRF24L01_H_

void NRF24_Init (void);

void NRF24_TxMode (uint8_t *Address, uint8_t channel);

uint8_t NRF24_Transmit (uint8_t *data);

/* Memory Map*/
#define CONFIG			0x00
#define EN_AA			0x01
#define EN_RXADDR		0x02
#define SETUP_AW		0x03
#define SETUP_RETR		0x04
#define RF_CH			0x05
#define RF_SETUP		0x06
#define STATUS			0x07
#define OBSERVE_TX		0x08
#define CD				0x09
#define RX_ADDR_P0		0x0A
#define TX_ADDR			0x10
#define FIFO_STATUS		0x17

/* Instruction Mnemonics */
#define R_REGEISTER		0x00
#define W_REGISTER		0x20
#define REGISTER_MASK	0x1F
#define ACTIVATE		0x50
#define R_RX_PL_WID		0x60
#define R_RX_PAYLOAD	0x61
#define W_TX_PAYLOAD	0xA0
#define W_ACK_PAYLOAD	0xA8
#define FLUSH_TX		0xE1
#define FLUSH_RX		0xE2
#define REUSE_TX_PL		0xE3
#define NOP				0XFF

#endif /* INC_NRF24L01_H_ */
