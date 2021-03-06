# EEPROMancer
Arduino Mega-based parallel EEPROM test/programming suite. Accomodates all parallel EEPROMS with up to 16 bits of addressing. For more information and other Z80 tools, see blog [here](https://thavelka.io/2020/07/eeprom-writer-arduino/).

## Capabilities
* Read/write individual pages in the EEPROM
* Write pages from hex/binary keyboard input
* Step through EEPROM pages
* Burn ROM from byte array in code
* Enable/disable SDP
* Clear EEPROM by software command sequence

## Hardware
Instructions and diagrams for a DIY shield are provided in the [hardware](https://github.com/thavelka/EEPROMancer/tree/master/hardware) directory, but you could make do with just a breadboard and the 74HC574 latch IC.

#### Stripboard shield
![writer](https://i2.wp.com/thavelka.io/wp-content/uploads/2020/06/completewithchip.jpg?resize=768%2C576&ssl=1)

#### Breadboard implementation
* PORTA to latch in, then latch out to EEPROM A0-A7
* PORTA also to EEPROM A8-A15
* PORTC to EEPROM D0-D7
* Digital pin 47 to latch clock
* Digital pin 49 to CE#
* Digital pin 51 to OE#
* Digital pin 53 to WE#

## Software
The sketch can be found in the [software](https://github.com/thavelka/ArduEEPROM/tree/master/software) directory. 

![software](https://i2.wp.com/thavelka.io/wp-content/uploads/2020/06/romwrite.png?resize=768%2C575&ssl=1)

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
