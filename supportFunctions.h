float c2f(float cel) {
  return (cel * (9.0/5.0)) + (float)3200;
}

int highValue(int value) {
  return value / 256;
}

int lowValue(int value) {
  return value % 256; 
}

int combineValue(unsigned int lb, unsigned int hb) {
  return ((hb * 256) + lb);
}


void resetMacros(){
  for(unsigned int i = macros_memstart; i <= macros_memend; i++){
    EEPROM.write(i, 0);
    delay(5);
  }
}

void setPinMode(unsigned int pin, unsigned int mode){
  EEPROM.write(pin, mode);
  pinMode(pin, mode);
}

void setPinStatus(unsigned int pin, unsigned int status) {
  if(status <= 1) {
    digitalWrite(pin, status);
  }else if(status > 1) {
    analogWrite(pin, status);
  }
}

void watchDigitalPinDigitalPinOutputMacroSet(unsigned int macro_number, unsigned int macro_type, unsigned int watch_pin, unsigned int watch_state, unsigned int output_pin, unsigned int output_action, unsigned int output_time_on){
  unsigned int memstart = macros_memstart + (macro_number * macros_bytes);
  unsigned int output_time_on_lb = output_time_on % 256;
  unsigned int output_time_on_hb = output_time_on / 256;
	
  EEPROM.write(memstart, macro_type);
  EEPROM.write((memstart + 1), watch_pin);
  EEPROM.write((memstart + 2), watch_state);
  EEPROM.write((memstart + 3), output_pin);
  EEPROM.write((memstart + 4), output_action);  
  EEPROM.write((memstart + 5), output_time_on_lb);
  EEPROM.write((memstart + 6), output_time_on_hb);
}

void runDigitalMacro(unsigned int mem_address){
  if(digitalRead(EEPROM.read(mem_address + 1)) == EEPROM.read(mem_address + 2)) {
    if(EEPROM.read(mem_address + 4) <= 1) {
      digitalWrite(EEPROM.read(mem_address + 3), EEPROM.read(mem_address + 4));
    }else{
      analogWrite(EEPROM.read(mem_address + 3), EEPROM.read(mem_address + 4));		
    }
  }
  delay(combineValue(EEPROM.read(mem_address + 5), EEPROM.read(mem_address + 6)));
}
   
void watchAnalogPinDigitalPinOutputMacroSet(unsigned int macro_number, unsigned int macro_type, unsigned int watch_pin, unsigned int watch_state, unsigned int output_pin, unsigned int output_action, unsigned int output_time_on, unsigned int greater_less_equal){
	unsigned int memstart = macros_memstart + (macro_number * macros_bytes);
	unsigned int output_time_on_lb = lowValue(output_time_on);
	unsigned int output_time_on_hb = highValue(output_time_on);
	unsigned int watch_state_lb = lowValue(watch_state);
	unsigned int watch_state_hb = highValue(watch_state);
	
	EEPROM.write(memstart, macro_type);
	EEPROM.write((memstart + 1), watch_pin);
	EEPROM.write((memstart + 2), watch_state_lb);
	EEPROM.write((memstart + 3), watch_state_hb);	
	EEPROM.write((memstart + 4), output_pin);
	EEPROM.write((memstart + 5), output_action);  
	EEPROM.write((memstart + 6), output_time_on_lb);
	EEPROM.write((memstart + 7), output_time_on_hb);
	EEPROM.write((memstart + 8), greater_less_equal);	
}

void runAnalogMacro(unsigned int mem_address){
	if(EEPROM.read(mem_address + 8) == 1) {
		if(analogRead(EEPROM.read(mem_address + 1)) < combineValue(EEPROM.read(mem_address + 2), EEPROM.read(mem_address + 3))) {
  			if(EEPROM.read(mem_address + 5) <= 1) {
				digitalWrite(EEPROM.read(mem_address + 4), EEPROM.read(mem_address + 5));
			}else if(EEPROM.read(mem_address + 5) > 1) {
				analogWrite(EEPROM.read(mem_address + 4), EEPROM.read(mem_address + 5));
			}
		}
	}else if(EEPROM.read(mem_address + 8) == 2) {
		if(analogRead(EEPROM.read(mem_address + 1)) > combineValue(EEPROM.read(mem_address + 2), EEPROM.read(mem_address + 3))) {
			if(EEPROM.read(mem_address + 5) <= 1) {
				digitalWrite(EEPROM.read(mem_address + 4), EEPROM.read(mem_address + 5));
			}else if(EEPROM.read(mem_address + 5) > 1) {
				analogWrite(EEPROM.read(mem_address + 4), EEPROM.read(mem_address + 5));
			}
		}	
	}else if(EEPROM.read(mem_address + 8) == 3) {
		if(analogRead(EEPROM.read(mem_address + 1)) == combineValue(EEPROM.read(mem_address + 2), EEPROM.read(mem_address + 3))) {
			if(EEPROM.read(mem_address + 5) <= 1) {
				digitalWrite(EEPROM.read(mem_address + 4), EEPROM.read(mem_address + 5));
			}else if(EEPROM.read(mem_address + 5) > 1) {
				analogWrite(EEPROM.read(mem_address + 4), EEPROM.read(mem_address + 5));
			}
		}
	}
}

void DS1307SetTime(unsigned int hour, unsigned int minute, unsigned int second, unsigned int day, unsigned int dow, unsigned int month, unsigned int year){
  RTC.stop();
  RTC.set(DS1307_SEC, second);        //set the seconds
  RTC.set(DS1307_MIN, minute);     //set the minutes
  RTC.set(DS1307_HR, hour);       //set the hours
  RTC.set(DS1307_DOW, dow);       //set the day of the week
  RTC.set(DS1307_DATE, day);       //set the date
  RTC.set(DS1307_MTH, month);        //set the month
  RTC.set(DS1307_YR, year);         //set the year
  RTC.start();
}

void watchDS1307DigitalOutputMacroSet(unsigned int macro_number, unsigned int macro_type, unsigned int hour_start, unsigned int minute_start, unsigned int hour_stop, unsigned int minute_stop, unsigned int output_pin, unsigned int output_action){
  unsigned int memstart = macros_memstart + (macro_number * macros_bytes);

  EEPROM.write(memstart, macro_type);
  EEPROM.write((memstart + 1), hour_start);
  EEPROM.write((memstart + 2), minute_start);
  EEPROM.write((memstart + 3), hour_stop);
  EEPROM.write((memstart + 4), minute_stop);
  EEPROM.write((memstart + 5), output_pin);
  EEPROM.write((memstart + 6), output_action);
}

void runDS1307Macro(unsigned int mem_address){
  unsigned int time_start;
  unsigned int time_stop;
  unsigned int rtc_time;		
	
  rtc_time = (RTC.get(DS1307_HR,true) * 100) + RTC.get(DS1307_MIN,true);
  time_start = (EEPROM.read(mem_address + 1) * 100) + EEPROM.read(mem_address + 2);
  time_stop = (EEPROM.read(mem_address + 3) * 100) + EEPROM.read(mem_address + 4);
	
  if(rtc_time >= time_start && rtc_time <= time_stop) {
    if(EEPROM.read(mem_address + 6) <= 1) {
      digitalWrite(EEPROM.read(mem_address + 5), EEPROM.read(mem_address + 6));
    } else {
      if(EEPROM.read(mem_address + 6) == 2 && RTC.get(DS1307_HR,false) == EEPROM.read(mem_address + 1)) {
        #if DEBUG == 1 && SERIALINTERFACEON == 1
          Serial.print("Fade In: ");
          Serial.println((RTC.get(DS1307_MIN,true) * 4));
        #endif
        analogWrite(EEPROM.read(mem_address + 5), (RTC.get(DS1307_MIN,false) * 4));
      }else if(EEPROM.read(mem_address + 6) == 2 && (RTC.get(DS1307_HR,false) + 1) == EEPROM.read(mem_address + 3)) {
        #if DEBUG == 1 && SERIALINTERFACEON == 1
          Serial.print("Fade Out: ");
          Serial.println((255 - (RTC.get(DS1307_MIN,true) * 4)));
        #endif
        analogWrite(EEPROM.read(mem_address + 5), (255 - (RTC.get(DS1307_MIN,false) * 4)));
      } else {
        analogWrite(EEPROM.read(mem_address + 5), EEPROM.read(mem_address + 6));
      }
    }
  }else if(rtc_time > time_stop) {
    digitalWrite(EEPROM.read(mem_address + 5), 0);
  }
}

void discoverOneWireDevices() {
  #if ONEWIREENABLED == 1
    OneWire ds(ONEWIRE_PIN);
  #endif    
  
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  unsigned int d = 0;
  unsigned int e;
	
  #if DEBUG == 1 && SERIALINTERFACEON == 1
    Serial.print("Looking for 1-Wire devices...\n\r");
  #endif
  while(ds.search(addr)) {
    #if DEBUG == 1 && SERIALINTERFACEON == 1
      Serial.print("\n\rFound \'1-Wire\' device with address:\n\r");
    #endif
	
    e = onewire_addresses_memstart + (d * onewire_addresses_bytes);
	
    for( i = 0; i < 8; i++) {
      Serial.print(addr[i], HEX);
      Serial.print(", ");
	
      EEPROM.write((e + i), addr[i]);
    }
    Serial.print("\n\r\n\r");
	
    if ( OneWire::crc8( addr, 7) != addr[7]) {
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("CRC is not valid!\n");
      #endif
      return;
    }
    d++;
  }
  EEPROM.write(num_ds18b20_devices, d);
  #if DEBUG == 1 && SERIALINTERFACEON == 1
    Serial.print("\n\r\n\r");
    Serial.print("Total Devices: ");
    Serial.println(d);
  #endif
  ds.reset_search();
  return;
}

int getDS18B20Temp(int device_num) {
  #if ONEWIREENABLED == 1
    OneWire ds(ONEWIRE_PIN);
  #endif   
  
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
	
  for(i = 0; i < 8; i++){
    addr[i] = EEPROM.read((onewire_addresses_memstart + (onewire_addresses_bytes * device_num) + i)); 
  }
	
  #if DEBUG == 1 && SERIALINTERFACEON == 1 && SERIALINTERFACEON == 1
    if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
    }
  #endif
	
  #if DEBUG == 1 && SERIALINTERFACEON == 1
    if ( addr[0] != 0x28) {
      Serial.print("Device is not a DS18S20 family device.\n");
    }
  #endif
	
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);
	
  //delay(1000);
	
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);
	
  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();
  }
	
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

  #if CELSIUS == 0
    Tc_100 = c2f(Tc_100);
  #endif 
	
  return Tc_100;
}

void watchDS18B20DigitalOutputMacroSet(unsigned int macro_number, unsigned int macro_type, unsigned int sensor_num, unsigned int less_greater_equal, unsigned int watch_state, unsigned int output_pin, unsigned int output_action) {
  unsigned int memstart = macros_memstart + (macro_number * macros_bytes);
	
  EEPROM.write(memstart, macro_type);
  EEPROM.write((memstart + 1), sensor_num);
  EEPROM.write((memstart + 2), less_greater_equal);
  EEPROM.write((memstart + 3), lowValue(watch_state));
  EEPROM.write((memstart + 4), highValue(watch_state));
  EEPROM.write((memstart + 5), output_pin);
  EEPROM.write((memstart + 6), output_action);
}

float getPHValue(unsigned int pin) {
  int analogreadings;
  unsigned int millivolts;
  float ph;
  unsigned int i;

  analogreadings = 0;
  for(i = 1; i < 50; i++){
    analogreadings += analogRead(pin);
    delay(10);
  }
  analogreadings /= i;
	
  millivolts = ((analogreadings * ARDUINO_VOLTAGE) / 1024) * 1000;
	
  ph = ((millivolts / PH_GAIN) / 59.2) + 7;
	
  return ph; 
}

void runDS18B20Macro(unsigned int mem_address) {
  unsigned int temp;
	
  temp = getDS18B20Temp(EEPROM.read(mem_address + 1));
	
  if(EEPROM.read(mem_address + 2) == 1) {
    if(temp < combineValue(EEPROM.read(mem_address + 3), EEPROM.read(mem_address + 4))) {
      if(EEPROM.read(mem_address + 6) <= 1) {
        digitalWrite(EEPROM.read(mem_address + 5), EEPROM.read(mem_address + 6));
      } else {
        analogWrite(EEPROM.read(mem_address + 5), EEPROM.read(mem_address + 6));			
      }
    }
  } else if(EEPROM.read(mem_address + 2) == 2) {
    if(temp > combineValue(EEPROM.read(mem_address + 3), EEPROM.read(mem_address + 4))) {
      if(EEPROM.read(mem_address + 6) <= 1) {
        digitalWrite(EEPROM.read(mem_address + 5), EEPROM.read(mem_address + 6));
      } else {
        analogWrite(EEPROM.read(mem_address + 5), EEPROM.read(mem_address + 6));			
      }
    }	
  } else if(EEPROM.read(mem_address + 2) == 3) {
    if(temp == combineValue(EEPROM.read(mem_address + 3), EEPROM.read(mem_address + 4))) {
      if(EEPROM.read(mem_address + 6) <= 1) {
        digitalWrite(EEPROM.read(mem_address + 5), EEPROM.read(mem_address + 6));
      } else {
        analogWrite(EEPROM.read(mem_address + 5), EEPROM.read(mem_address + 6));			
      }
    }	
  }
}












void runMacros() {
  unsigned int macro_mem_address_start;
  for(unsigned int i = 0; i < num_macros; i++) {
    macro_mem_address_start = macros_memstart + (i * macros_bytes);
    if(EEPROM.read(macro_mem_address_start) == 1) {
      #if DIGITALENABLED == 1
        runDigitalMacro(macro_mem_address_start);
      #endif
    }else if(EEPROM.read(macro_mem_address_start) == 2) {
      #if ANALOGENABLED == 1
        runAnalogMacro(macro_mem_address_start);
      #endif
    }else if(EEPROM.read(macro_mem_address_start) == 3) {
      #if DS1307ENABLED == 1
        runDS1307Macro(macro_mem_address_start);
      #endif
    }else if(EEPROM.read(macro_mem_address_start) == 4) {
      #if DS18B20ENABLED == 1
        runDS18B20Macro(macro_mem_address_start);
      #endif
    }
  }
}
