/*
 * EEPROM UTIL
 * By Tim Havelka, 2020
 * github.com/thavelka
 * 
 * Description:
 * A sketch that can read, write, and clear up to a 64Kx8 parallel EEPROM using an Arduino Mega and
 * one octal latch, such as a 74HC574.
 * A compatible DIY EEPROM writer shield can be found here: https://thavelka.io/2020/07/eeprom-writer-arduino/
 * Although most EEPROMs should be compatible, ensure the software protection enable, disable, and erase
 * codes for your EEPROM match the ones in this sketch. See your EEPROM's data sheet for the codes.
 * 
 * Hardware Setup:
 * PORTA to latch in, then latch out to EEPROM A0-A7
 * PORTA also to EEPROM A8-A15
 * PORTC to EEPROM D0-D7
 * Digital pin 47 to latch clock
 * Digital pin 49 to CE#
 * Digital pin 51 to OE#
 * Digital pin 53 to WE#
 * 
 * Software setup:
 * Set ROM_SIZE constant to number of KB in EEPROM (e.g. 32 for a chip listed as 256K or 32Kx8).
 * Set PAGE_SIZE constant to page register size in bytes (likely 64 or 128).
 * Set ROM to program to write, up to one page in size.
 */


#define CLK 47
#define CE 49
#define OE 51
#define WE 53


const int ROM_SIZE = 32; // EEPROM size in KB. (e.g. 32Kx8 EEPROM = 32)
const int PAGE_SIZE = 64; // EEPROM page register size in bytes

/*
 * Example program. Reads in byte from input port 0 and writes to output port 0. 
 */
const byte ROM[PAGE_SIZE] = {
  0xDB, 0x00,       // IN A, 0: Load val from port 0 into accumulator (11011011, 00000000)
  0xD3, 0x00,       // OUT 0, A: Write accumulator to port 0 (11010011, 00000000)
  0xC3, 0x00, 0x00  // JP 0: Jump to start (11000011, 00000000, 00000000)
};


const int MAX_PAGE = ((32 * 1024) - PAGE_SIZE) / PAGE_SIZE - 1; // Calculated maximum page index
const String MENU_PROMPT = String("\nMain Menu:")
  + "\n1. Go to page"
  + "\n2. Step page"
  + "\n3. Read page"
  + "\n4. Write page"
  + "\n5. Burn ROM"
  + "\n6. Enable SDP"
  + "\n7. Disable SDP"
  + "\n8. Erase chip"
  + "\n";

unsigned int address = 0x00;
unsigned int currentPage = 0x00;
byte val = 0x00;
boolean ran = false;

void setup() {
  Serial.begin(115200);
  pinMode(CE, OUTPUT);
  digitalWrite(CE, HIGH);
  pinMode(WE, OUTPUT);
  digitalWrite(WE, HIGH);
  pinMode(CLK, OUTPUT);
  digitalWrite(CLK, LOW);
  pinMode(OE, OUTPUT);
  digitalWrite(OE, HIGH);
  
  DDRA = 0xFF; // Address port
  DDRC = 0x00; // Data port
  PORTA = 0x00;

  delay(1000);
}

void loop() {
    delay(5);
    mainMenu();
}

// Main functions
void mainMenu() {
  flushSerial();
  Serial.println("\n\nPage: " + String(currentPage, DEC));
  Serial.println(MENU_PROMPT);
  while (Serial.available() == 0) {}
  char selection = Serial.read();
  Serial.println(String("> ") + String(selection));
  switch(selection) {
    case '1':
      goToPage();
      break;
    case '2':
      stepPage();
      break;
    case '3':
      readPage();
      break;
    case '4':
      writePage();
      break;
    case '5':
      burnROM();
      break;
    case '6':
      enableWrite();
      enableSDP();
      disableWrite();
      Serial.println("\nEnabled SDP.\n");
      break;
    case '7':
      disableSDP();
      break;
    case '8':
      eraseChip();
      break;
    default:
      Serial.println("Invalid selection");
  }
}

// Menu item 1
void goToAddress() {
  Serial.println("\n\nSelect address (Max 0xffff)");
  flushSerial();
  while(Serial.available() == 0) {}
  unsigned long int newAddress = readInt('\n');
  if (newAddress >= 0 && newAddress <= 0xffff) {
    address = newAddress;
    writeAddress(address);
  } else {
    Serial.println("Invalid address");
  }
}

void goToPage() {
  Serial.println("\n\nSelect page (Max = " + String(MAX_PAGE) + ")");
  flushSerial();
  while(Serial.available() == 0) {}
  unsigned long int page = readInt('\n');
  if (page >= 0 && page <= MAX_PAGE) {
    currentPage = page;
    writeAddress(page * PAGE_SIZE);
  } else {
    Serial.println("Invalid page");
  }
}

// Menu item 2
void stepAddress() {
  if (address < 0xffff) {
    writeAddress(++address);
  }
}

void stepPage() {
  if (currentPage < MAX_PAGE) {
    writeAddress((++currentPage) * PAGE_SIZE);
  }
}

// Menu item 3
void readWord() {
  DDRC = 0x00;
  byte value = PINC;
  Serial.println("\nAddress = 0x" + String(address, HEX));
  Serial.println("Value = 0b" + String(value, BIN));
}

void readPage() {
  enableRead();
  readPage(currentPage);
  disableRead();
}

// Menu item 4
void writeWord() {
  Serial.println("\n\nSet value (Max 0xff)");
  flushSerial();
  while(Serial.available() == 0) {}
  unsigned long int value = readInt('\n');
  if (value >= 0 && value <= 0xff) {
    writeData(lowByte(value));
    Serial.println("Wrote 0b" + String(value, BIN) + " to address 0x" + String(address, HEX));
  } else {
    Serial.println("Invalid value");
  }
}

void writePage() {
  Serial.println("\n\nSet bytes, separated with commas and/or spaces (Max length = " + String(PAGE_SIZE) + ")");
  Serial.println("Bytes can be written in hexadecimal with the format 0xFF or binary with the format 0b11111111.");
  Serial.println("Format will default to hexadecimal if no prefix present.");
  byte data[PAGE_SIZE];
  flushSerial();
  while(Serial.available() == 0) {}
  String input = Serial.readString();
  input.trim();
  int count = 0;
  int start = 0;
  int myCount = 0;
  char myWord[11];
  Serial.println("input = " + input);
  for (int i = 0; i < input.length(); i++) {
    char c = input.charAt(i);
    if (isAlphaNumeric(c)) {
      if (myCount < 10) {
        myWord[myCount] = c;
        myCount++;
      }
    } else {
      myWord[myCount] = '\0';
      if (myCount > 0) {
        data[count] = lowByte(parseIntString(myWord));
        count++;
      }
      myCount = 0;
    }
  }
  myWord[myCount] = '\0';
      if (myCount > 0) {
        data[count] = lowByte(parseIntString(myWord));
        count++;
      }
      myCount = 0;
      Serial.println("Parsed data:");
  for (int j = 0; j < count; j++) {
    Serial.print(String(data[j], HEX));
    Serial.print((j + 1) % 8 == 0 ? '\n' : '\t');
  }
  if (count > 0) {
    enableWrite();
    writePage(currentPage, data, count);
    disableWrite();
  }
}

// Menu item 5
void burnROM() {
  Serial.println("Writing ROM to page 0");
  int len = sizeof(ROM) / sizeof(byte);
  for (int i = 0; i < len; i++) {
    Serial.print(String(ROM[i], HEX));
    Serial.print((i + 1) % 8 == 0 ? '\n' : '\t');
  }
  enableWrite();
  writePage(0, ROM, len);
  disableWrite();
  Serial.println("\nDone.\n");
}

// Menu item 8
void eraseChip() {
  enableWrite();
  writeByte(0x5555, 0xAA);
  writeByte(0x2AAA, 0x55);
  writeByte(0x5555, 0x80);
  writeByte(0x5555, 0xAA);
  writeByte(0x2AAA, 0x55);
  writeByte(0x5555, 0x10);
  disableWrite();
  Serial.println("\nErased chip.\n");
}

// System utilities

// Sets CE# and OE# low. Call before reading page.
void enableRead() {
  DDRC = 0x00;
  digitalWrite(CE, LOW);
  digitalWrite(OE, LOW);
}

// Sets CE# high. Call after finishing read.
void disableRead() {
  digitalWrite(CE, HIGH);
}

// Sets CE# low and OE# high. Call before writing page.
void enableWrite() {
  DDRC = 0xFF;
  digitalWrite(OE, HIGH);
  digitalWrite(CE, LOW);
}

// Sets CE# high. Call after write.
void disableWrite() {
  digitalWrite(CE, HIGH);
}

// Latches low address byte then puts high byte on PORTA
void writeAddress(unsigned int addr) {
  PORTA = lowByte(addr);
  delayMicroseconds(5);
  digitalWrite(CLK, HIGH);
  delayMicroseconds(5);
  digitalWrite(CLK, LOW);
  PORTA = highByte(addr);
}

// Sets address then reads in from PORTC. Call enableRead() before calling this.
byte readByte(unsigned int addr) {
  writeAddress(addr);
  delayMicroseconds(5);
  return PINC;
}

// Sets address then writes data to PORTC. Call enableWrite() before calling this.
void writeByte(unsigned int addr, byte data) {
  writeAddress(addr);
  delayMicroseconds(5);
  
  // Write data
  digitalWrite(WE, LOW);
  delayMicroseconds(5);
  PORTC = lowByte(data);
  delayMicroseconds(5);
  digitalWrite(WE, HIGH);
  delayMicroseconds(5);
}

// Reads and prints the whole page at the specified index.  Call enableRead() before calling this.
void readPage(unsigned int page) {
  unsigned int p = page <= MAX_PAGE ? page : MAX_PAGE;
  Serial.println("Dumping page " + String(p, DEC));
  unsigned int addr = page * PAGE_SIZE;

  for (int i = 0; i < PAGE_SIZE; i++) {
    byte val = readByte(addr + i);
    Serial.print(String(val, HEX));
    Serial.print((i + 1) % 8 == 0 ? '\n' : '\t');
  }
}

// Writes a whole page of data at the specified index.  Call enableWrite() before calling this.
void writePage(unsigned int page, byte data[], int len) {
  int addr = page * PAGE_SIZE;
  enableSDP();
  for (int i = 0; i < len; i++) {
    writeByte(addr + i, data[i]);
  }
}

/*  
 *  Disables write protection to allow a page to be written. Call enableWrite() before calling this.
 *  Write protection will be enabled again after the page write completes.
 */
void enableSDP() {
  writeByte(0x5555, 0xAA);
  writeByte(0x2AAA, 0x55);
  writeByte(0x5555, 0xA0);
}

// Disables software write protection on the chip.
void disableSDP() {
  enableWrite();
  writeByte(0x5555, 0xAA);
  writeByte(0x2AAA, 0x55);
  writeByte(0x5555, 0x80);
  writeByte(0x5555, 0xAA);
  writeByte(0x2AAA, 0x55);
  writeByte(0x5555, 0x20);
  disableWrite();
  Serial.println("\nDisabled SDP.\n");
}

void writeData(byte data) {
  delay(100);
  digitalWrite(OE, HIGH);
  digitalWrite(WE, HIGH);
  delay(5);
  DDRC = 0xff;
  PORTC = data;
  delay(5);
  digitalWrite(WE, LOW);
  delay(10);
  digitalWrite(WE, HIGH);
  delay(10);
  digitalWrite(OE, LOW);
  delay(5);
  DDRC = 0x00;
}

// Inteface/helper utilities
long int readInt(char delim) {
  char data[18];
  int count = 0;
  int base;
  if (Serial.available() > 0) {
    count = Serial.readBytesUntil(delim, data, 18);
    int endIndex = count == 18 ? 17 : count;
    data[endIndex] = '\0';
    return parseIntString(data);
  }
  return -1;
}

long int parseIntString(const char* str) {
  int len = strlen(str);
  // Check for hex prefix
  if (len >= 2 && (strncmp(str,"0x", 2) == 0 || strncmp(str,"0X", 2) == 0)) {
    return strtol(&str[2], NULL, 16);
  }

  // Check for bin prefix
  if (len >= 2 && (strncmp(str,"0b", 2) == 0 || strncmp(str,"0B", 2) == 0)) {
    return strtol(&str[2], NULL, 2);
  }

  // Default to hex
  return strtol(str, NULL, 16);
}

void flushSerial() {
  while(Serial.available() > 0) {
    Serial.read();
  }
}
