#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "nrf24.h"
#include "spi.h"

int8_t volume = 0, scroll = 0;
uint8_t play = 0;

struct
{
	int8_t volume;
	int8_t scroll;
	uint8_t play;
}transmission;

int main(void)
{
	cli();
	spi_init();
	ce_low();

	_delay_ms(200);		//nrf24l01+ power on reset is 100ms
	
	writeReg(CONFIG, _BV(EN_CRC));
	writeReg(EN_AA, 0x3F);
	writeReg(EN_RXADDR, 0x01);
	writeReg(SETUP_AW, 0x03);
	writeReg(SETUP_RETR, 0x1F);		//15 retransmissions with 500u delay
	writeReg(RF_CH, 0x25);
	writeReg(RF_SETUP, _BV(RF_DR_LOW) | RF_PWR);
	writeReg(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));

	ss_low();
	
	spi_transfer(TX_ADDR | W_REGISTER);
	spi_transfer(0x01);
	spi_transfer(0x54);
	spi_transfer(0x45);
	spi_transfer(0x50);
	spi_transfer(0x25);
	
	ss_high();

	_delay_us(10);

	ss_low();
	
	spi_transfer(RX_ADDR_P0 | W_REGISTER);
	spi_transfer(0x01);
	spi_transfer(0x54);
	spi_transfer(0x45);
	spi_transfer(0x50);
	spi_transfer(0x25);
	
	ss_high();
	
	GIMSK = _BV(PCIE0);
	PCMSK0 = _BV(ENCODER_A_PIN) | _BV(BUTTON_PIN) | _BV(IRQ_PIN);
	
	sei();
	
	uint8_t prev_button_state = button_get();
	uint8_t was_turned = 0;
	uint8_t prev_encoder_state = encoder_a_get();
	uint8_t ispoweron = 0;
	
	while(1)
	{
		EIFR = _BV(PCIF0);	//clear interrupt flag
		
		cli();
		power_all_disable();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		sei();
		sleep_cpu();
		sleep_disable();
		
		if(prev_button_state)	//was not pressed
		{
			if(button_get())	//still not pressed
			{
				if(prev_encoder_state != encoder_a_get())	//did encoder trigger int?
				{
					prev_encoder_state = encoder_a_get();

					if(!encoder_a_get() ^ encoder_b_get())
						volume++;		//volume up
					else
						volume--;		//volume down
				}
			}
			else	//pressed
			{
				prev_button_state = 0;
			}
		}
		else					//was pressed
		{
			if(button_get())	//released
			{
				prev_button_state = 1;
				
				if(was_turned)
				{
					was_turned = 0;
				}
				else
				{
					play |= 1;	//short press, play/pause
				}
			}
			else	//still pressed
			{
				if(prev_encoder_state != encoder_a_get())	//did encoder trigger int?
				{
					prev_encoder_state = encoder_a_get();

					if(!encoder_a_get() ^ encoder_b_get())
						scroll++;
					else
						scroll--;
				}
				was_turned |= 0x01;
			}
		}

		if(((volume / 2) != 0) || ((scroll / 2) != 0) || (play != 0))
		{
			if(!ispoweron)
			{
				transmission.volume = (volume / 2);
				volume = 0;
				transmission.scroll = (scroll / 2);
				scroll = 0;
				transmission.play = play;
				play = 0;

				spi_wake();

				ss_low();

				spi_transfer(W_TX_PAYLOAD);

				for(uint8_t i = 0; i < 3; i++)
				{
					spi_transfer(((uint8_t*)&transmission)[i]);
				}

				ss_high();

				writeReg(CONFIG, _BV(PWR_UP) | _BV(EN_CRC));

				ce_high();

				ispoweron = 1;
			}			
		}

		if(!irq_get())
		{
			spi_wake();
			ce_low();

			writeReg(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));	//clear IRQ
			writeReg(CONFIG, _BV(EN_CRC));								//power down

			ss_low();	//flush TX
			spi_transfer(FLUSH_TX);
			ss_high();

			ispoweron = 0;
		}


	}
}

EMPTY_INTERRUPT(PCINT0_vect);