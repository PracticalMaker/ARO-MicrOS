/*
Andrew Oke - andrew@practicalmaker.com

V0.3 - Initial Release
V0.4 - 2011/06/07 - Added Support for Celsius
V0.4.1 2011/06/08 - Rewrote interface and wrote ethernet interface. Much easier to add new interfaces as all all you do is grab the command line and pass it to the control function

*/
#define DEBUG 1
#define DEBUGFREEMEMORY 0
#define DEBUGETHERNETQUERYSTRING 1

#define SPIINTERFACEON 1
#define SERIALINTERFACEON 1
#define ETHERNETWIZNETW5100INTERFACEON 1

#define DIGITALENABLED 1
#define ANALOGENABLED 1
#define DS1307ENABLED 1
#define I2CLCDENABLED 1
#define ONEWIREENABLED 1
#define DS18B20ENABLED 1
#define PHENABLED 1

#define CELSIUS 0

#define BUFFERSIZE 32
#define ARDUINO_MEM_ADDR 0
#define ONEWIRE_PIN 2
#define ARDUINO_VOLTAGE 5.06
#define PH_PIN 2
#define PH_GAIN 9.6525


byte digital_pin_mem_start = 2;
byte digital_pin_mem_end = 13;

unsigned int macros_memstart = 100;
unsigned int macros_memend = 400;
unsigned int macros_bytes = 10;
unsigned int num_macros = 40;

byte display_time_as = 16;
byte i2clcd_time_display = 17;
byte i2clcd_cursor_col = 18;
byte i2clcd_cursor_row = 19;
byte num_ds18b20_devices = 20;
byte display_ds18b20_temp1 = 21;
byte ds18b20_temp1_col = 22;
byte ds18b20_temp1_row = 23;
byte display_ds18b20_temp2 = 24;
byte ds18b20_temp2_col = 25;
byte ds18b20_temp2_row = 26;
byte display_ds18b20_temp3 = 27;
byte ds18b20_temp3_col = 28;
byte ds18b20_temp3_row = 29;
byte ds18b20_temp1_sensor = 30;
byte ds18b20_temp2_sensor = 31;
byte ds18b20_temp3_sensor = 32;
byte display_ph = 33;
byte display_ph_row = 34;
byte display_ph_col = 35;

unsigned int onewire_addresses_memstart = 401;
unsigned int onewire_addresses_memend = 500;
unsigned int onewire_addresses_bytes = 8;

uint8_t degree_symbol = 0xDF;

#include <SPI.h>
#include <Client.h>
#include <Ethernet.h>
#include <Server.h>
#include <Wire.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <I2CLCD.h>
#include <DS1307.h>
#include <OneWire.h>
#include "supportFunctions.h"
#include "control.h"
#include "ethernetInterface.h"
#include "serialInterface.h"
#include "freemem.h"
#include "LCDPrint.h"


#if ETHERNETWIZNETW5100INTERFACEON == 1
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  byte ip[] = { 192,168,1, 28 };

  Server server(80);
#endif

/**************
 * Setup devices
 **************/
#if I2CLCDENABLED == 1
  I2CLCD lcd = I2CLCD(0x12, 20, 4);
#endif

#if ONEWIREENABLED == 1
  OneWire ds(ONEWIRE_PIN);
#endif


void setup() {
  #if SERIALINTERFACEON == 1
    Serial.begin(9600);  
  #endif
  
  #if ETHERNETWIZNETW5100INTERFACEON == 1
    Ethernet.begin(mac, ip);
    server.begin();  
  #endif
  
  #if I2CLCDENABLED == 1
    lcd.init();
    lcd.backlight(0);
    lcd.cursorOff();
    lcd.clear();
  #endif  
  

  //set pinmodes from eeprom
  for(int i = digital_pin_mem_start; i <= digital_pin_mem_end; i++){
    pinMode(i, EEPROM.read(i));
  }  
}


void loop() {
  runMacros();
  
  #if SERIALINTERFACEON == 1
    serialInterface();
  #endif  

  #if ETHERNETWIZNETW5100INTERFACEON == 1
    ethernetWiznetW5100Interface();
  #endif
  
  #if I2CLCDENABLED == 1
    printToLCD();
  #endif
  
  #if DEBUG == 1 && DEBUGFREEMEMORY == 1
    Serial.print("Free Memory: ");
    Serial.println(availableMemory());
    delay(500);
  #endif  
  
  delay(100);
}
