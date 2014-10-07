#ifndef ARO_MICROS_H
#define ARO_MICROS_H

#include <Arduino.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Adafruit_CC3000.h>
#include <cc3000_PubSubClient.h>
#include <OneWire.h>

#ifdef _RTCLIB_H_
extern RTC_DS1307 rtc;
#endif

#ifdef cc3000_PubSubClient_h
extern	cc3000_PubSubClient mqttclient;
#endif
extern char* MQTT_Username;
extern char mqttString[160];

class ARO_MicrOS {
private:
	#ifdef ADAFRUIT_CC3000_H
	Adafruit_CC3000* _cc3000;
	Adafruit_CC3000_Client* _client;
	#endif

	#ifdef OneWire_h
	int getDS18B20Temp(int device_num);
	#endif
	
	char _deviceID[33];

	void DS1307SetTime(int year, byte month, byte day, byte hour, byte minute, byte second);
	int discoverOneWireDevices();

	bool configureISE(byte pin, int value1, int value2);
	int highValue(int value);
	int lowValue(int value);
	int combineValue(unsigned int lb, unsigned int hb);
	float readISE(byte pin);
	unsigned long checkinCurrentMillis;
	unsigned long checkinPreviousMillis;	

public:
	ARO_MicrOS();

	#ifdef ADAFRUIT_CC3000_H
	uint16_t cc3000CheckFirmwareVersion(Adafruit_CC3000& cc3000);
	bool displayConnectionDetails(Adafruit_CC3000& cc3000);
	bool connectCC3000(Adafruit_CC3000& cc3000, Adafruit_CC3000_Client& client, const char* wlan_ssid, const char* wlan_pass, uint8_t wlan_security, bool reconnect);
	bool connectMQTT(Adafruit_CC3000& cc3000, Adafruit_CC3000_Client& client, cc3000_PubSubClient& mqttclient, char* MQTT_Username, char* MQTT_Password, uint32_t server); 
	#endif

	#if defined(cc3000_PubSubClient_h) && defined(ADAFRUIT_CC3000_H)
	void publishCheckin();
	#endif

	char *control(char *returnData, char *commandString);
	void pinModeSet(byte pin, byte mode);
	void setPinStatus(byte pin, byte pinstatus);
	void resetMacros(void);
	void setDeviceAddress(void);
	

	
	// really want to figure out how to move this out of main sketch file...
	//void cc3000MQTTInterfaceCallback(char* topic, byte* payload, unsigned int length);
	void serialInterface();
	char* deviceID();

	void runMacros();
};


#endif
