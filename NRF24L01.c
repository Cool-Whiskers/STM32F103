/*
 * NRF24L01.c
 *
 *  Created on: Mar 12, 2023
 *      Author: Sebastian Colom
 */

#include "stm32f1xx_hal.h"
#include "NRF24L01.h"

extern SPI_HandleTypeDef hspi2;
#define NRF24_SPI &hspi2

#define NRF24_CE_PORT GPIOC
#define NRF24_CE_PIN GPIO_PIN_7

#define NRF24_CSN_PORT GPIOB
#define NRF24_CSN_PIN GPIO_PIN_6

void CS_Select (void)
{
	HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_RESET);
}

void CS_UnSelect (void)
{
	HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET);
}

void CE_Select (void)
{
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_SET);
}

void CE_UnSelect (void)
{
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET);
}


// write single byte to the particular register
void nrf24_WriteReg (uint8_t Reg, uint8_t Data)
{
	uint8_t buf[2];
	buf[0] = Reg|1<<5;
	buf[1] = Data;

	//Pull the CS pin low to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, buf, 2, 1000);

	//Pull the CS HIGH to release the device
	CS_UnSelect();
}

// write multiple bytes starting from a particular register
void nrf24_WriteRegMulti (uint8_t Reg, uint8_t *data, int size)
{
	uint8_t buf[2];
	buf[0] = Reg|1<<5;

	//Pull the CS pin low to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, buf, 1, 100);
	HAL_SPI_Transmit(NRF24_SPI, data, size, 1000);

	//Pull the CS HIGH to release the device
	CS_UnSelect();
}

// Read a single byte from the register
uint8_t nrf24_ReadReg (uint8_t Reg)
{
	uint8_t data = 0;

	//Pull the CS pin low to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100);
	HAL_SPI_Receive(NRF24_SPI, &data, 1, 100);

	//Pull the CS HIGH to release the device
	CS_UnSelect();

	return data;
}

// read multiple bytes from the register
void nrf24_ReadReg_Multi (uint8_t Reg, uint8_t *data, int size)
{
	//Pull the CS pin low to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 1000);
	HAL_SPI_Receive(NRF24_SPI, data, size, 1000);

	//Pull the CS HIGH to release the device
	CS_UnSelect();

//	return data;
}

// send the command to the NRF
void nrfsendCmd (uint8_t cmd)
{
	//Pull the CS pin low to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);

	//Pull the CS HIGH to release the device
	CS_UnSelect();
}

void NRF24_Init (void)
{
	// disable the chip before configuring the device
	CE_UnSelect();

	nrf24_WriteReg(CONFIG, 0);

	nrf24_WriteReg(EN_AA, 0); // No Auto ACK

	nrf24_WriteReg(EN_RXADDR, 0); // Not enabling any data pipe right now

	nrf24_WriteReg(SETUP_AW, 0x03); // 5 Bytes for the TX/RX address

	nrf24_WriteReg(SETUP_RETR, 0); // no retransmission

	nrf24_WriteReg(RF_CH, 0); // will be setup during TX or RX

	nrf24_WriteReg(RF_SETUP, 0x0E); // Power = 0dB, data rate = 2Mbps

	// Enable the chip after configuring the device
	CE_Select();
}

// set up the TX mode

void NRF24_TxMode (uint8_t *Address, uint8_t channel)
{
	// Disable the chip before configuring the device
	CE_UnSelect();

	nrf24_WriteReg(RF_CH, channel); // select the channel

	nrf24_WriteRegMulti(TX_ADDR, Address, 5); // write the TX address

	// power up the device
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1 << 1);
	nrf24_WriteReg(CONFIG, config);

	CE_Select();
}

// transmit the data

uint8_t NRF24_Transmit (uint8_t *data)
{
	uint8_t cmdtosend = 0;

	// select the device
	CS_Select();

	// payload command
	cmdtosend = W_TX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);

	// send payload
	HAL_SPI_Transmit(NRF24_SPI, data, 32, 1000);

	// unselect the device
	CS_UnSelect();

	HAL_Delay(1);

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

	uint8_t fifostatus = nrf24_ReadReg(FIFO_STATUS);

	if ((fifostatus&(1 << 4)) && (!(fifostatus&(1 << 3))))
	{
//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		cmdtosend = FLUSH_TX;
		nrfsendCmd(cmdtosend);

		return 1;
	}

	return 0;
}

