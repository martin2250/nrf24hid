#ifndef SPI_H_
#define SPI_H_

uint8_t transfer(uint8_t data);

void ss_high();
void ss_low();
void ce_high();
void ce_low();
uint8_t irq_get();

#endif