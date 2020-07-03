# EEPROM writer shield for Arduino Mega
This is a DIY EEPROM writer shield for an Arduino Mega created on stripboard. 
Fritzing files are not available, as this was an old project, but photos have been attached. 
More information about this project [here](https://thavelka.io/2020/07/eeprom-writer-arduino/).

## Software
The sketch can be found in my Z80 Utils repo [here](https://github.com/thavelka/Z80Utils/blob/master/eeprom_util/).
When running, the sketch can read and write pages, write data from serial input, clear the EEPROM, and enable and disable data protection.

## Construction
The design can be easily modified to accomodate any 28-32 pin EEPROM since only the leftmost pins vary. 
Designs for a 32Kx8 and 128Kx8 EEPROM are attached below, as well as pictures of the finished 32Kx8 writer.
The data and A8-A15 wires have been omitted from the diagram to not block everything else out. Wires need to be connected from Arduino A0-A7 to EEPROM A8-15 and from Arduino C0-C7 to EEPROM D0-D7.

## Components
* Stripboard, 26 x 34 holes (Cheap, from Amazon)
* Stackable headers, 10 pin x 1, 8 pin x 1, and 6 pin x 1
* ZIF socket, 40 pin to fit any EEPROM
* 74574 octal latch

## Photos
### 32Kx8 writer (28C256 type EEPROM)
![32Kx8 writer](https://github.com/thavelka/Z80Boards/blob/master/eeprom_writer/eeprom_writer_32k.jpg)

### 128Kx8 writer (28C010 type EEPROM)
![128Kx8 writer](https://github.com/thavelka/Z80Boards/blob/master/eeprom_writer/eeprom_writer_128k.jpg)

### Complete 32Kx8 writer
![32Kx8 top](https://github.com/thavelka/Z80Boards/blob/master/eeprom_writer/complete32k.jpg)

### Complete 32Kx8 writer bottom
![32Kx8 bottom](https://github.com/thavelka/Z80Boards/blob/master/eeprom_writer/complete32kbottom.jpg)
