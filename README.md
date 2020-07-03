# ArduEEPROM
Arduino Mega-based parallel EEPROM test/programming suite. Accomodates all parallel EEPROMS with up to 16 bits of addressing. For more information and other Z80 tools, see blog [here](https://thavelka.io/2020/07/eeprom-writer-arduino/).

## Capabilities
* Read/write individual pages in the EEPROM
* Write pages from hex/binary keyboard input
* Step through EEPROM pages
* Burn ROM from byte array in code
* Enable/disable SDP
* Clear EEPROM by software command sequence

## Hardware
Instructions for a DIY shield are provided in the [hardware](https://github.com/thavelka/ArduEEPROM/tree/master/hardware) directory, but you could make do with just a breadboard and the 74HC574 latch IC.

![writer](https://i2.wp.com/thavelka.io/wp-content/uploads/2020/06/completewithchip.jpg)

## Software
The sketch can be found in the [software](https://github.com/thavelka/ArduEEPROM/tree/master/software) directory. 

## Instructions
0. Set `ROM_SIZE` and `PAGE_SIZE` constants in sketch to match your EEPROM's data sheet
1. Upload sketch to Arduino Mega
2. Unplug Arduino
3. Insert EEPROM into shield, ensuring correct orientation
4. Plug Arduino back in
5. Open serial monitor
6. Follow prompts to read/write/clear EEPROM
7. Unplug Arduino
8. Remove EEPROM

I recommend disconnecting the Arduino from power before inserting or removing the EEPROM. Backwards or incorrect insertion will smoke the chip.

## Coming soon
* Support for writing byte arrays and input larger than one page size
* Writing multiple pages at once
* Hex file upload over serial
