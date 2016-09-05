#include <avr/io.h>
#include <inttypes.h>
#include "nrf24.h"
#include "spi.h"

uint8_t spi_transfer(uint8_t data)
{	
	USIDR = data;
	USISR = (1<< USIOIF); //Clear the USI counter overflow flag

	do
	{
		USICR = (1<< USIWM0) | (1<< USICLK) | (1<< USICS1) | (1<< USITC);
	} while((USISR & (1<< USIOIF)) == 0);

	return USIDR;
}

#define SCK PB7
#define MOSI PB6
#define CSN PB3		//SPI chip select
#define CE PD2		//Chip enable

void spi_init()
{
	PORTA = 0;
	PORTB = _BV(CSN);
	PORTD = 0;

	DDRB = _BV(SCK) | _BV(MOSI) | _BV(CSN);
	DDRD = _BV(CE);
	DDRA = 0;
}

void spi_wake()
{
	PRR &= ~3;
	USICR = _BV(USIWM0);
}

void ss_high() 
{
	SS_REG |= _BV(SS_PIN);
}

void ss_low()
{
	SS_REG &= ~_BV(SS_PIN);
}

void ce_high()
{
	CE_REG |= _BV(CE_PIN);
}

void ce_low()
{
	CE_REG &= ~_BV(CE_PIN);
}

uint8_t irq_get()
{
	return (IRQ_REG >> IRQ_PIN) & 0x01;
}

void writeReg(uint8_t reg, uint8_t data)
{
	ss_low();
	spi_transfer(W_REGISTER | reg);
	spi_transfer(data);
	ss_high();
}

uint8_t encoder_a_get()
{
	return (ENCODER_A_REG >> ENCODER_A_PIN) & 0x01;
}

uint8_t encoder_b_get()
{
	return (ENCODER_B_REG >> ENCODER_B_PIN) & 0x01;
}

uint8_t button_get()
{
	return (BUTTON_REG >> BUTTON_PIN) & 0x01;
}