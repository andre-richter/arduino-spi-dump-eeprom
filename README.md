# arduino-spi-read-eeprom


Dump an SPI EEPROM in raw format to your PC via an Arduino.

## Howto

1. Connect the EEPROM to your Arduino's SPI header.
    * __The sketch uses Pin 10 as the default Chip Select pin__. You can change it in the sketch if you need to.
2. Connect the Arduino to your PC via the USB __programming port__ and upload the sketch.
3. Compile the C program with a simple `make`
4. Run the program and supply the following arguments:
    * The tty device name of your Arduino e.g. `-t /dev/ttyACM0`
    * The baudrate, which is 115200 by default: `-b 115200`
    * The number of bytes you want to read from the EEPROM: `-n 10`

The read bytes will be stored in a file called __eeprom.bin__.

## Compatibility

I have tested the program with an Arduino Due on Mac OS X and Linux, but it should work with every POSIX compatible OS.

### TTY device names

* In Mac OS X, the Due shows up as `/dev/cu.usbmodem14931`.
* In Linux, it is `/dev/ttyACM0`. __To connect from Linux, you need to be root!__

Best you check it for yourself with a `dmesg` after connecting the Arduino, because Arduinos other than the Due will have different tty device names.

### Compatible EEPROMs

The Arduino sketch is compatible with `ST Micro M45PExx series` EEPROMs,
`Sanyo LE25FW series` EEPROMs and every other SPI EEPROM that is compatible to the used read protocol:

1. one byte read instruction (`0x03`)
2. three bytes address
3. keeping chip select low makes consecutive reads starting from the supplied address.

## Example

    $ make
    $ ./spi_read_eeprom -t /dev/cu.usbmodem14931 -b 115200 -n 5
    /dev/cu.usbmodem14931 open.
    tty successfully configured.
    0x00000000: 0x18
    0x00000001: 0xe7
    0x00000002: 0x00
    0x00000003: 0x00
    0x00000004: 0x00

Afterwards, you can edit the data with your favourite hex editor.

## License
The MIT License (MIT)

Copyright (c) 2014 Andre Richter

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
