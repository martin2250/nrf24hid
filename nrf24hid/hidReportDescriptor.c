#include <avr/pgmspace.h>
#include "usbdrv/usbconfig.h"

const PROGMEM char usbDescriptorHidReport[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
	/* Consumer Control (Sound/Media keys) */
	0x05, 0x0C,									/* usage page (consumer device) */
	0x09, 0x01, 								/* usage -- consumer control */
	0xA1, 0x01, 								/* collection (application) */
	/* 4 Media Keys */
	0x15, 0x00, 								/* logical minimum */
	0x26, 0xFF, 0x03, 							/* logical maximum (3ff) */
	0x19, 0x00, 								/* usage minimum (0) */
	0x2A, 0xFF, 0x03, 							/* usage maximum (3ff) */
	0x95, 0x04, 								/* report count (4) */
	0x75, 0x10, 								/* report size (16) */
	0x81, 0x00, 								/* input */
	0xC0 /* end collection */
};