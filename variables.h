//#define DEBUG_SERIAL

// Which commands do you want enabled?
#define COMMAND_IDENTIFY_ENABLED
#define COMMAND_PINMODE_ENABLED
#define COMMAND_SETPIN_ENABLED
#define COMMAND_RESETMACROS_ENABLED
#define COMMAND_ANALOGPINREAD_ENABLED
#define COMMAND_DIGITALPINREAD_ENABLED
#define COMMAND_SETDS1307_ENABLED
#define COMMAND_GETDS1307UNITTIME_ENABLED
#define COMMAND_DISCOVERONEWIREDEVICES_ENABLED
#define COMMAND_GETDS18B20TEMP_ENABLED
#define COMMAND_CONFIGUREISE_ENABLED
#define COMMAND_READISE_ENABLED
#define COMMAND_PUBLISHDS18B20TEMP_ENABLED
#define COMMAND_UNPUBLISHDS18B20TEMP_ENABLED

//#define PCF8574AENABLED
//#define SENDTOCOSMENABLED
//uncomment to use 1wire parasitic power mode
//#define ONEWIRE_PARASITIC_POWER_MODE

#define ONEWIRE_PIN 7
#define PH_PIN 3
#define ORP_PIN 2

#define BUFFERSIZE 60

#ifdef PCF8574AENABLED
const byte pcf8574a_addresses[] = {
  B0111000,
  B0111001,
  B0111010,
  B0111011,
  B0111100,
  B0111101,
  B0111110,
  B0111111,    
};

byte pcf8574a_states[] = {
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
};  
#endif


/* 
EEPROM LOCATIONS
*/
#define ADDRESS_START 0
#define ADDRESS_END 24

#define DIGITAL_PIN_START 34
#define DIGITAL_PIN_END 47
#define NUM_ONEWIRE_DEVICES 48
#define ONEWIRE_ADDRESSES_START 49
#define ONEWIRE_ADDRESSES_END 129
#define ISE_CONF_START 130
#define ISE_CONF_END 166

#define PUBLISH_ONEWIRE_TEMP_START 167
#define PUBLISH_ONEWIRE_TEMP_END 177

#define MACROS_START 211
#define MACROS_END 511

#define ONEWIRE_ADDRESSES_BYTES 8
#define ISE_CONF_BYTES 6
#define MACRO_BYTE_LENGTH 20

#define ISE_CONFIGURE_MATCHCOUNT 1

const byte display_time_as = 16;
const byte display_ds18b20_temp1 = 21;
const byte ds18b20_temp1_col = 22;
const byte ds18b20_temp1_row = 23;
const byte display_ds18b20_temp2 = 24;
const byte ds18b20_temp2_col = 25;
const byte ds18b20_temp2_row = 26;
const byte display_ds18b20_temp3 = 27;
const byte ds18b20_temp3_col = 28;
const byte ds18b20_temp3_row = 29;
const byte ds18b20_temp1_sensor = 30;
const byte ds18b20_temp2_sensor = 31;
const byte ds18b20_temp3_sensor = 32;
const byte display_ph = 33;
const byte display_ph_row = 34;
const byte display_ph_col = 35;

const byte display_ORP = 36;
const byte display_ORP_row = 37;
const byte display_ORP_col = 38;

#ifdef SENDTOCOSMENABLED
unsigned long COSM_LAST_UPDATE;
#endif


// REWRITE VARIABLES TO DEFINE
uint8_t degree_symbol = 0xDF;

unsigned long currentMillis = millis();
unsigned long previousMillis = 0;
unsigned long interval = 1000;

byte celsius = 0;
