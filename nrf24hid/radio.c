#include <inttypes.h>
#include "spi.h"
#include "nrf24.h"
#include <avr/io.h>
#include "radio.h"

status_t status;

void writeReg(uint8_t reg, uint8_t value)
{
	ss_low();
	
	*((uint8_t*)&status) = spi_transfer(reg | W_REGISTER);
	spi_transfer(value);
	
	ss_high();
}

uint8_t readReg(uint8_t reg)
{
	ss_low();

	*((uint8_t*)&status) = spi_transfer(reg | R_REGISTER);
	uint8_t value = spi_transfer(0x00);

	ss_high();

	return value;
}

uint8_t read_pw()
{
	ss_low();

	*((uint8_t*)&status) = spi_transfer(R_RX_PL_WID);
	uint8_t value = spi_transfer(0x00);

	ss_high();

	return value;
}

void flushRX()
{
	ss_low();

	*((uint8_t*)&status) = spi_transfer(FLUSH_RX);

	ss_high();
}

void radio_init()
{
	ce_low();
	
	writeReg(CONFIG, _BV(PRIM_RX) | _BV(PWR_UP) | _BV(EN_CRC));
	
	writeReg(EN_AA, _BV(ENAA_P0));
	writeReg(EN_RXADDR, _BV(ERX_P0));
	writeReg(SETUP_AW, 0x03);
	writeReg(RF_CH, 0x25);
	writeReg(RF_SETUP, _BV(RF_DR_LOW) | RF_PWR);
	writeReg(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));

	writeReg(RX_PW_P0, 0x03);

	ss_low();
	
	spi_transfer(RX_ADDR_P0 | W_REGISTER);
	spi_transfer(0x01);
	spi_transfer(0x54);
	spi_transfer(0x45);
	spi_transfer(0x50);
	spi_transfer(0x25);
	
	ss_high();

	ce_high();
}