CC = gcc
CFLAGS = -Wall

all:
	$(CC) -o spi_read_eeprom $(CFLAGS) spi_read_eeprom.c