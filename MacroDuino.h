#ifndef MACRODUINO_H
#define MACRODUINO_H

#include <Arduino.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Adafruit_CC3000.h>
#include <cc3000_PubSubClient.h>
#include <OneWire.h>

extern RTC_DS1307 rtc;
extern	cc3000_PubSubClient mqttclient;
extern char* MQTT_Username;
extern char mqttString[160];

class MacroDuino {
private:
	char _deviceID[33];
	Adafruit_CC3000* _cc3000;
	void DS1307SetTime(int year, byte month, byte day, byte hour, byte minute, byte second);
	int discoverOneWireDevices();
	int getDS18B20Temp(int device_num);
	bool configureISE(byte pin, int value1, int value2);
	int highValue(int value);
	int lowValue(int value);
	int combineValue(unsigned int lb, unsigned int hb);
	float readISE(byte pin);

public:
	MacroDuino();
	char *control(char *returnData, char *commandString);
	void pinModeSet(byte pin, byte mode);
	void setPinStatus(byte pin, byte pinstatus);
	void resetMacros(void);
	void setDeviceAddress(void);
	uint16_t cc3000CheckFirmwareVersion(Adafruit_CC3000& cc3000);
	bool displayConnectionDetails(Adafruit_CC3000& cc3000);
	char* deviceID();
};


#endif
