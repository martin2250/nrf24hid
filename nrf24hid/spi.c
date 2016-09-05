#include <avr/io.h>
#include <inttypes.h>

#define SS_REG PORTA
#define SS_PIN PA2

#define CE_REG PORTA
#define CE_PIN PA1

#define IRQ_REG PINA
#define IRQ_PIN PA0

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

void spi_init()
{
	PORTA = 0;
	PORTB = 0;

	DDRA = _BV(PA1) | _BV(PA2);
	DDRB = _BV(PB1) | _BV(PB2);

	PORTA = _BV(PA0) | _BV(PA2);	//IRQ pullup, SS

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