/*

This is an example sketch to use the MacroDuino code. This sketch is designed for interaction via MQTT.

Check out the MacroDuino control api for commands you can send/receive

http://www.practicalmaker.com/content/macroduino-control-api

If you wish to enable additional functionality simply uncomment the libraries below, save, quit arduino, open arduino and open
the sketch again.

It's a pain, but the library needs to get recompiled and that's the easiest way to do it.

It's recommended that you sign up to hiveconnect.io which will be a dedicated app to interact in real time with MacroDuino MQTT.

That way you can simply run the code on your Arduino and you'll already have a free MQTT broker to handle everything for you.

Andrew Oke
andrew [at] practicalmaker [dot] com

*/
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <cc3000_PubSubClient.h>
//#include <RTClib.h>
//#include <OneWire.h>
#include <MacroDuino.h>

//change this to your wifi details
char* wlan_ssid = { "" };
char* wlan_pass = { "" };
//Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
// ie. 0, 1, 2, 3
uint8_t wlan_security = 3;	

//change this to your hiveconnect.io login details
char* MQTT_Username = { "" };
char* MQTT_Password = { "" };

MacroDuino MacroDuino;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  MacroDuino.begin();

}

void loop() {
  MacroDuino.loop();
}

#ifdef cc3000_PubSubClient_h
void cc3000MQTTInterfaceCallback(char* topic, byte* payload, unsigned int length) {
  char mqttReturnData[MQTT_MAX_PACKET_SIZE];
  memset(mqttReturnData, 0, sizeof mqttReturnData);
  char mqttString[160];

  char commandString[length+1];
  for(byte j = 0; j < length; j++) {
    commandString[j] = payload[j];
    commandString[j+1] = '\0';
  }
  MacroDuino.control(mqttReturnData, commandString);
  
  memset(mqttString, 0, sizeof mqttString);
  strcat(mqttString, MQTT_Username);
  strcat(mqttString, "/");
  strcat(mqttString, MacroDuino.deviceID()); 
  strcat(mqttString, "/");  
  strcat(mqttString, strtok(commandString, "/"));
  MacroDuino.mqttPublish(mqttString, mqttReturnData);
}
#endif