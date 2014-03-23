#include <SPI.h>

#define SPI_READ_CMD 0x03
#define CS_PIN 10

unsigned int num_bytes;
unsigned int i;

void setup() {
  Serial.begin(115200);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  SPI.begin();
}

void read_eeprom(unsigned int num_bytes) {
  unsigned int addr;
  byte resp;

  digitalWrite(CS_PIN, LOW);

  /* transmit read command with 3 byte start address */
  SPI.transfer(SPI_READ_CMD);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);

  for (addr = 0; addr < num_bytes; addr++) {
    resp = SPI.transfer(0xff);
    Serial.write(resp);
  }
  digitalWrite(CS_PIN, HIGH);
}

void loop() {

  /* wait for the integer with the requested number of bytes */
  if (Serial.available() == 4) {
    num_bytes = 0;

    /* merge four bytes to single integer */
    for (i = 0; i < 4; i++)
      num_bytes |=  Serial.read() << (i * 8);

    read_eeprom(num_bytes);
  }
}

