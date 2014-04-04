#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

//Uncomment to use the DS1307 related functions
//#include <RTClib.h>
//Uncomment to use the Onewire related functions
//#include <OneWire.h>
#include <MacroDuino.h>

#ifdef _RTCLIB_H_
RTC_DS1307 rtc;
#endif

MacroDuino MacroDuino;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  MacroDuino.setDeviceAddress();

  #ifdef _RTCLIB_H_
  rtc.begin();
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  #endif

}

void loop() {
  if (Serial.available() > 0) {
    MacroDuino.serialInterface();
  }

  MacroDuino.runMacros();
}