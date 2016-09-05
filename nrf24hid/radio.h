#ifndef RADIO_H_
#define RADIO_H_

void writeReg(uint8_t reg, uint8_t value);
uint8_t readReg(uint8_t reg);
uint8_t read_pw();
void flushRX();
void radio_init();

typedef struct
{
	unsigned int tx_full : 1;
	unsigned int rx_p_no : 3;
	unsigned int max_rt  : 1;
	unsigned int tx_ds  : 1;
	unsigned int rx_dr  : 1;
}status_t;

extern status_t status;

#endif