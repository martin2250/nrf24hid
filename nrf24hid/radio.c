#include <inttypes.h>
#include "spi.h"
#include "nrf24.h"

void writeReg(uint8_t reg, uint8_t value)
{
	ss_low();
	
	transfer(reg | W_REGISTER);
	transfer(value);
	
	ss_high();
}