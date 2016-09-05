#ifndef SPI_H_
#define SPI_H_

uint8_t spi_transfer(uint8_t data);
void spi_init();
void spi_wake();

void ss_high();
void ss_low();
void ce_high();
void ce_low();
uint8_t irq_get();

uint8_t encoder_a_get();
uint8_t encoder_b_get();
uint8_t button_get();

void writeReg(uint8_t reg, uint8_t data);

#define SS_REG PORTB
#define SS_PIN PB3

#define CE_REG PORTD
#define CE_PIN PD2

#define IRQ_REG PINB
#define IRQ_PIN PB4

#define ENCODER_A_REG PINB
#define ENCODER_A_PIN PB0

#define ENCODER_B_REG PINB
#define ENCODER_B_PIN PB1

#define BUTTON_REG PINB
#define BUTTON_PIN PB2


#endif