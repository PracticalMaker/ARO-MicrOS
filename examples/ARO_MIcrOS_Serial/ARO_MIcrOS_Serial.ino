/*

This is an example sketch to use the MacroDuino code. This sketch is designed for interaction via the serial port.

Check out the MacroDuino control api for commands you can send/receive

http://www.practicalmaker.com/content/macroduino-control-api

If you wish to enable additional functionality simply uncomment the libraries below, save, quit arduino, open arduino and open
the sketch again.

It's a pain, but the library needs to get recompiled and that's the easiest way to do it.

Andrew Oke
andrew [at] practicalmaker [dot] com

*/
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

//#include <RTClib.h>
//#include <OneWire.h>
#include <ARO_MicrOS.h>

#define OUTPUT SERIAL

ARO_MicrOS os;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  
  os.begin();

}

void loop() {
  os.loop();
}
