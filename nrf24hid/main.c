#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usbdrv/usbdrv.h"

uint8_t usbFunctionWrite(uint8_t * data, uint8_t len)
{
	return 1;
}

uint8_t usbFunctionRead(uint8_t * data, uint8_t len)
{
	return 0;
}

uint8_t usbFunctionSetup(uint8_t data[8])
{
	return 0;
}

typedef struct {
	uint16_t keycode[4];
} keyboard_report_t;

keyboard_report_t report;

int main(void)
{
	usbInit();

	usbDeviceDisconnect();
	_delay_ms(100);
	usbDeviceConnect();

	sei();

    while (1) 
    {
		usbPoll();

		if(usbInterruptIsReady())
		{
			report.keycode[0] ^= 0xCD;
			usbSetInterrupt((void *)&report, sizeof(report));
		}
    }
}

