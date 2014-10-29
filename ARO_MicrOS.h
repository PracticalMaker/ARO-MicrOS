#ifndef ARO_MicrOS_H
#define ARO_MicrOS_H

#if defined(ARDUINO) && ARDUINO > 100
#include <Arduino.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Adafruit_CC3000.h>
#include <cc3000_PubSubClient.h>
#include <OneWire.h>
#endif

#ifdef STM32F10X_MD
#include "application.h"
#include "PubSubClient.h"
#include "I2C_EEPROM.h"
#include "OneWire.h"
#include "RTClib.h"
#endif

#if defined(STM32F10X_MD) || (defined(ADAFRUIT_CC3000_H) && defined(cc3000_PubSubClient_h))
static byte server_ip[4] = { 192, 99, 3, 185 };
extern void mqttInterfaceCallback(char* topic, byte* payload, unsigned int length);
#endif

#ifdef STM32F10X_MD
static I2C_EEPROM EEPROM;

extern char* itoa(int a, char* buffer, unsigned char radix);

static TCPClient tcpClient;
static PubSubClient mqttclient(server_ip, 1883, mqttInterfaceCallback, tcpClient);
#endif

#ifdef ADAFRUIT_CC3000_H
#define aref_voltage 3.3
#define ADAFRUIT_CC3000_IRQ 3
#define ADAFRUIT_CC3000_VBAT 5
#define ADAFRUIT_CC3000_CS 10
static Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);
static Adafruit_CC3000_Client client;
#endif

#ifdef _RTCLIB_H_
static RTC_DS1307 rtc;
#endif

#ifdef cc3000_PubSubClient_h
union ArrayToIp {
	byte array[4];
	uint32_t ip;
};
static ArrayToIp server = { 185, 3, 99, 192 };
// We're going to set our broker IP and union it to something we can use
static cc3000_PubSubClient mqttclient(server.ip, 1883, mqttInterfaceCallback, client, cc3000);
#endif

extern char* wlan_ssid;
extern char* wlan_pass;
extern uint8_t wlan_security;
extern char* MQTT_Username;
extern char* MQTT_Password;

class ARO_MicrOS {
private:
	#ifdef OneWire_h
	int discoverOneWireDevices();
	int getDS18B20Temp(int device_num);
	void runDS18B20Macro(int mem_address);
	#endif

	#ifdef _RTCLIB_H_
	void DS1307SetTime(int year, byte month, byte day, byte hour, byte minute, byte second);
	#endif
	
	char _deviceID[24];
	unsigned long checkinCurrentMillis;
	unsigned long checkinPreviousMillis;		
	char mqttString[160];
	
	bool configureISE(byte configuration_number, byte pin, char *value1, char *value2);
	int highValue(unsigned int value);
	int lowValue(unsigned int value);
	int combineValue(unsigned int lb, unsigned int hb);
	float readISE(byte pin);
	void setDeviceAddress(void);
	void eepromWrite(unsigned int address, byte value);
	byte eepromRead(unsigned int address);	
	void configureEC(byte configuration_num, byte enable_pin, byte read_pin, char *char_calibration_units, char *char_calibration_scale, char *calibration_solution_1, char *calibration_solution_2);
	float readEC(byte configuration_num);
	void output(const char* string);
	void output(byte string);	
	void outputln(const char* string);
	void outputln(byte string);	
	
public:
	ARO_MicrOS();
	void begin();
	void loop();
	#ifdef ADAFRUIT_CC3000_H
	uint16_t cc3000CheckFirmwareVersion(Adafruit_CC3000& cc3000);
	bool displayConnectionDetails(Adafruit_CC3000& cc3000);
	bool connectCC3000(Adafruit_CC3000& cc3000, Adafruit_CC3000_Client& client, const char* wlan_ssid, const char* wlan_pass, uint8_t wlan_security, bool reconnect);
	bool connectMQTT(Adafruit_CC3000& cc3000, Adafruit_CC3000_Client& client, cc3000_PubSubClient& mqttclient, char* MQTT_Username, char* MQTT_Password, uint32_t server); 
	#endif
	
	#ifdef STM32F10X_MD
	bool sparkConnectMQTT(char* MQTT_Username, char* MQTT_Password);
	#endif

	#if (defined(cc3000_PubSubClient_h) && defined(ADAFRUIT_CC3000_H)) || defined(STM32F10X_MD)
	void publishCheckin();
	#endif

	char *control(char *returnData, char *commandString);
	void pinModeSet(byte pin, byte mode);
	void setPinStatus(byte pin, byte pinstatus);
	void resetMacros(void);

	void serialInterface();
	char* deviceID();
	void runMacros();
	void mqttPublish(char* topic, char* payload);
};


#endif
