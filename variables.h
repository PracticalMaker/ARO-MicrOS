#define DEBUG

// Which commands do you want enabled?
#define COMMAND_IDENTIFY_ENABLED
#define COMMAND_PINMODE_ENABLED
#define COMMAND_SETPIN_ENABLED
#define COMMAND_RESETMACROS_ENABLED
#define COMMAND_ANALOGPINREAD_ENABLED
#define COMMAND_DIGITALPINREAD_ENABLED
//#define COMMAND_SETDS1307_ENABLED
//#define COMMAND_GETDS1307UNIXTIME_ENABLED
//#define COMMAND_DISCOVERONEWIREDEVICES_ENABLED
#//define COMMAND_GETDS18B20TEMP_ENABLED
#define COMMAND_CONFIGUREISE_ENABLED
#define COMMAND_READISE_ENABLED
//#define COMMAND_PUBLISHDS18B20TEMP_ENABLED
//#define COMMAND_UNPUBLISHDS18B20TEMP_ENABLED
#define COMMAND_EC_CONFIGURE
#define COMMAND_READ_EC

//#define PCF8574AENABLED
//uncomment to use 1wire parasitic power mode
//#define ONEWIRE_PARASITIC_POWER_MODE

#define BUFFERSIZE 60

/* 
EEPROM LOCATIONS
*/
#define ADDRESS_START 0
#define ADDRESS_END 24
#define SMART_CONFIG_COMPLETE 25

#define DIGITAL_PIN_START 34
#define DIGITAL_PIN_END 47
#define NUM_ONEWIRE_DEVICES 48

#define ONEWIRE_ADDRESSES_START 49
#define ONEWIRE_ADDRESSES_END 129


#define PUBLISH_ONEWIRE_TEMP_START 167
#define PUBLISH_ONEWIRE_TEMP_END 177
#define MQTT_USERNAME_START 178
#define MQTT_USERNAME_END 208
#define MQTT_PASSWORD_START 209
#define MQTT_PASSWORD_END 238

#define CONFIGURATION_START 130
#define CONFIGURATION_END 220
#define CONFIGURATION_LENGTH 40

#define MACROS_START 239
#define MACROS_END 511
#define MACRO_BYTE_LENGTH 20


// REWRITE VARIABLES TO DEFINE
//uint8_t degree_symbol = 0xDF;

unsigned long currentMillis = millis();
unsigned long previousMillis = 0;
unsigned long interval = 1000;

byte celsius = 0;

#define EC_SAMPLES 1000
#define EC_MATCH_COUNT 20

#define ISE_CONFIGURE_MATCHCOUNT 2