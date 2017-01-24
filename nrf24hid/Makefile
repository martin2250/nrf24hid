MCU = attiny861a
F_CPU =  12000000UL

SRC_C := $(wildcard *.c) $(wildcard usbdrv/*.c)
SRC_S := $(wildcard usbdrv/*.S)
OBJECTS := $(SRC_C:%.c=$(OBJDIR)%.o) $(SRC_S:%.S=$(OBJDIR)%.o)

all: main.hex

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude

CFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -std=gnu11

flash: all
	$(AVRDUDE) -c usbtiny -B 2 -p t861 -U flash:w:main.hex:i

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

main.elf: $(OBJECTS)
	$(CC) $(CFLAGS) -o main.elf $(OBJECTS)

main.hex: main.elf
	$(OBJCOPY) -j .text -j .data -O ihex main.elf main.hex
