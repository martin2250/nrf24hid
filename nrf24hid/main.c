#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "usbdrv/usbdrv.h"
#include "keys.h"
#include "nrf24.h"
#include "spi.h"
#include "radio.h"

typedef struct {
	uint16_t keycode[4];
} keyboard_report_t;

keyboard_report_t report;
uint8_t idleRate;

uint8_t usbFunctionSetup(uint8_t data[8])
{
	usbRequest_t *rq = (void *)data;

	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS)
	{
		switch(rq->bRequest)
		{
			case USBRQ_HID_GET_REPORT:
				usbMsgPtr = (void *)&report;
				memset(&report, 0, sizeof report);
				return sizeof(report);

			case USBRQ_HID_GET_IDLE:
				usbMsgPtr = &idleRate;
				return 1;

			case USBRQ_HID_SET_IDLE:
				idleRate = rq->wValue.bytes[1];
				return 0;
		}
	}
	
	return 0;
}

int8_t volume = 0, scroll = 0;
uint8_t play = 0;
uint8_t data_available = 0;

struct
{
	int8_t volume;
	int8_t scroll;
	uint8_t play;
}transmission;

int main(void)
{
	spi_init();
	usbInit();
	
	usbDeviceDisconnect();
	_delay_ms(250);
	usbDeviceConnect();

	radio_init();

	sei();

    while (1) 
    {
		usbPoll();

		data_available |= !irq_get();

		if(data_available)
		{
			writeReg(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));

			if(read_pw() != 3)
			{
				flushRX();
				data_available = 0;
			}
			else
			{
				ss_low();

				spi_transfer(R_RX_PAYLOAD);

				for(uint8_t i = 0; i < 3; i++)
				{
					((uint8_t*)&transmission)[i] = spi_transfer(0x00);
				}

				ss_high();

				volume += transmission.volume;
				scroll += transmission.scroll;
				play |= transmission.play;

				data_available = !(readReg(0x17) & 0x01);
			}
		}

		if(usbInterruptIsReady())
		{
			if(volume != 0 && report.keycode[0] == 0)
			{
				if(volume > 0)
				{
					report.keycode[0] = MEDIA_VOL_UP;
					volume--;
				}
				else
				{
					report.keycode[0] = MEDIA_VOL_DOWN;
					volume++;
				}				
			}
			else if(report.keycode[0] != 0)
			{
				report.keycode[0] = 0;
			}

			if(scroll != 0 && report.keycode[1] == 0)
			{
				if(scroll > 0)
				{
					report.keycode[1] = MEDIA_NEXT;
					scroll--;
				}
				else
				{
					report.keycode[1] = MEDIA_PREV;
					scroll++;
				}
			}
			else if(report.keycode[1] != 0)
			{
				report.keycode[1] = 0;
			}

			if(play != 0 && report.keycode[2] == 0)
			{
				report.keycode[2] = MEDIA_PLAY_PAUSE;
				play = 0;
			}
			else if(report.keycode[2] != 0)
			{
				report.keycode[2] = 0;
			}

			usbSetInterrupt((void *)&report, sizeof(report));
		}
    }
}

