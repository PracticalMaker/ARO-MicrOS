#include "ARO_MicrOS.h"
#include "variables.h"

ARO_MicrOS::ARO_MicrOS() {

}

void ARO_MicrOS::begin() {
	#if OUTPUT == SERIAL
    this->outputln("Begin");
    #endif
	this->setDeviceAddress();
	
	#ifdef _RTCLIB_H_
	rtc.begin();
	if (! rtc.isrunning()) {
		#ifdef DEBUG
		this->outputln("RTC is NOT running!");
		#endif
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(__DATE__, __TIME__));
	}
	#endif
	
	#ifdef ADAFRUIT_CC3000_H
	this->connectCC3000(cc3000, client, wlan_ssid, wlan_pass, wlan_security, false);
	#endif
	
    #if defined(STM32F10X_MD) && defined(PubSubClient_h)
    this->sparkConnectMQTT(MQTT_Username, MQTT_Password);
    #endif
    
	#ifdef cc3000_PubSubClient_h
	this->connectMQTT(cc3000, client, mqttclient, MQTT_Username, MQTT_Password, server.ip);
	#endif
}

char *ARO_MicrOS::control(char *returnData, char *commandString) {
	/******* NOTES  
	* for control functions with an eeprom.write a delay before returning is needed otherwise no data is returned
	* returnData needs to be setup as follows
	* int counter = 0;
	* returnData[counter] = value;
	* counter++
	* the reason being it makes it easier to change
	*/

	String command = String(strtok(commandString, "/"));
	int counter;
	boolean check = false;
	char itoa_buffer[6];

	memset(returnData, 0, sizeof returnData);
	memset(itoa_buffer, 0, sizeof itoa_buffer);  

	#ifdef DEBUG
	this->output("Command Token: ");
	this->outputln(command);
	#endif
  
	/*
	identify
	*/
	#ifdef COMMAND_IDENTIFY_ENABLED
	if(command == "identify") {    
		counter = 0;

		for(byte j = ADDRESS_START+1; j < ADDRESS_END; j++) {
			returnData[counter] = this->eepromRead(j);
			counter++;
		}    
		returnData[counter] = '\0';
		delay(1);    
		return returnData;    
	}
	#endif
  
  /*
  Setting Arduino Pin modes
  */
  #ifdef COMMAND_PINMODE_ENABLED
  if(command == "pinmode") {
    char *charpin = strtok(NULL, "/");
    char *charmode = strtok(NULL, "/");
    byte pin = atoi(charpin);
    byte mode = atoi(charmode);
    
    this->pinModeSet(pin, mode);
    
    #ifdef DEBUG
		this->output("Pin Mode Pin: ");
		this->outputln(pin);
		this->output("Pin Mode: ");
		this->outputln(mode);      
    #endif
    
    counter = 0;

    if(charpin[0] != 0) {
      returnData[counter] = charpin[0];
      counter++;
    }
    if(charpin[1] != 0) {
      returnData[counter] = charpin[1];
      counter++;
    }    
    returnData[counter] = '/';
    counter++;
    returnData[counter] = *charmode;
    delay(1);    

    return returnData;    
  }
  #endif
  
  /*
  Set pin status
  */
  #ifdef COMMAND_SETPIN_ENABLED
  if(command == "setpin") {
    char *charpin = strtok(NULL, "/");
    char *charpinstatus = strtok(NULL, "/");
    byte pin = atoi(charpin);
    byte pinstatus = atoi(charpinstatus);

    setPinStatus(pin, pinstatus);

    counter = 0;
    check = false;	

    if(charpin[0] != 0) {
      returnData[counter] = charpin[0];
      counter++;
    }
    if(charpin[1] != 0) {
      returnData[counter] = charpin[1];
      counter++;
    }    
    returnData[counter] = '/';
    counter++;    
    if(charpinstatus[0] != 0) {
      returnData[counter] = charpinstatus[0];
      counter++;
    }
    if(charpinstatus[1] != 0) {
      returnData[counter] = charpinstatus[1];
      counter++;
    } else {
      check = true;
    }
    if(charpinstatus[2] != 0 && check != true) {
      returnData[counter] = charpinstatus[2];
      counter++;
    }
    delay(1);

    return returnData;
  }
  #endif  
  
  /*
  reset all the macros
  */
  #ifdef COMMAND_RESETMACROS_ENABLED
  if(command == "resetconfig") {
    this->resetMacros();
  }
  #endif	
  
  /* 
  analogread
  */
  #ifdef COMMAND_ANALOGPINREAD_ENABLED
  if(command == "analogpinread"){
    char *charwatch_pin = strtok(NULL, "/");
    int watch_pin = atoi(charwatch_pin);

    itoa(analogRead(watch_pin), itoa_buffer, 10);

    int counter = 0;
    boolean check = false;

    returnData[counter] = *charwatch_pin;
    counter++;
    returnData[counter] = '/';
    counter++;
    if(itoa_buffer[0] != 0) {
      returnData[counter] = itoa_buffer[0]; 
      counter++;
    } else {
      returnData[counter] = 48;
      counter++;
      check = true;
    }
    if(itoa_buffer[1] != 0 && check != true) {
      returnData[counter] = itoa_buffer[1]; 
      counter++;
    } else {
      check = true;
    }
    if(itoa_buffer[2] != 0 && check != true) {
      returnData[counter] = itoa_buffer[2]; 
      counter++;
    } else {
      check = true;
    }
    if(itoa_buffer[3] != 0 && check != true) {
      returnData[counter] = itoa_buffer[3]; 
      counter++;
    } 
    else {
      check = true;
    }
    delay(1);   

    return returnData;
  }
  #endif
  
  #ifdef COMMAND_DIGITALPINREAD_ENABLED
  //TODO - not returning an address and won't return correct pin for values over 9
  if(command == "digitalpinread"){
    char *char_watch_pin = strtok(NULL, "/");
    byte watch_pin = atoi(char_watch_pin);

    itoa(digitalRead(watch_pin), itoa_buffer, 10);

    counter = 0;
    check = false;

    if(char_watch_pin[0] != 0) {
      returnData[counter] = char_watch_pin[0];
      counter++;
    }
    if(char_watch_pin[1] != 0) {
      returnData[counter] = char_watch_pin[1];
      counter++;
    } 
    else {
      check = true;
    }
    returnData[counter] = '/';
    counter++;
    returnData[counter] = itoa_buffer[0]; 
    counter++;    
    delay(1);   

    return returnData;
  }
  #endif		

  #if defined(_RTCLIB_H_) && defined(COMMAND_SETDS1307_ENABLED)
	if(command == "setds1307"){	
	  char *charyear = strtok(NULL, "/");
	  char *charmonth = strtok(NULL, "/");      
	  char *charday = strtok(NULL, "/");
	  char *charhour = strtok(NULL, "/");
	  char *charminute = strtok(NULL, "/");
	  char *charsecond = strtok(NULL, "/");

	  byte hour = atoi(charhour);
	  byte minute = atoi(charminute);
	  byte second = atoi(charsecond);
	  byte day = atoi(charday);
	  byte month = atoi(charmonth);
	  int year = atoi(charyear);

	  this->DS1307SetTime(year, month, day, hour, minute, second);

	  int counter = 0;
	  boolean check_hour = false;
	  boolean check_minute = false;
	  boolean check_second = false;
	  boolean check_day = false;
	  boolean check_dow = false;
	  boolean check_month = false;
	  boolean check_year = false;

	  returnData[counter] = 'S'; 
	  counter++;
	  returnData[counter] = 'E'; 
	  counter++;
	  returnData[counter] = 'T'; 
	  counter++;
	  returnData[counter] = 'T'; 
	  counter++;
	  returnData[counter] = 'I'; 
	  counter++;
	  returnData[counter] = 'M'; 
	  counter++;
	  returnData[counter] = 'E'; 
	  counter++;                                    
	  delay(1);   

	  return returnData;
	}	
  #endif

  #if defined(_RTCLIB_H_) && defined(COMMAND_GETDS1307UNITTIME_ENABLED)
	if(command == "getds1307unixtime"){
		char unixtime[11]; 
		DateTime now = rtc.now();

		snprintf(unixtime, sizeof unixtime, "%lu", (unsigned long)now.unixtime());
		strcpy(returnData, unixtime);
		
		return returnData;
	}
  #endif
  
	#ifdef COMMAND_CONFIGUREISE_ENABLED
	if(command == "configureise"){
		char *charpin = strtok(NULL, "/");
		char *charvalue1 = strtok(NULL, "/");
		char *charvalue2 = strtok(NULL, "/");
		char *chartype = strtok(NULL, "/");		

		byte pin = atoi(charpin);

		return (char*)this->configureISE(pin, charvalue1, charvalue2, chartype);		
	}  
	#endif
  
	#ifdef COMMAND_READISE_ENABLED  
	if(command == "readise"){
		char *charpin_number = strtok(NULL, "/");
		byte pin_number = atoi(charpin_number);	  

		#ifdef DEBUG
		//this->output("readise: ");
		//this->outputln(this->readISE(configuration_number));
		#endif

		this->readISE(pin_number);
	}  	
	#endif

//TODO
#ifdef TLC5940_H
  if(commandToken == 11) {
    int tlc_pin_num = atoi(strtok(NULL, "/"));
    int tlc_action = atoi(strtok(NULL, "/"));
    
		#ifdef DEBUG
		this->output("tlc_pin_num ");
		this->outputln(tlc_pin_num);
		this->output("tlc_action ");
		this->outputln(tlc_action);
    #endif
   
    controlTLC5940(tlc_pin_num, tlc_action); 
    
    counter = 0;

    returnData[counter] = '{';
    counter++;    
    returnData[counter] = 'S';
    counter++;    
    returnData[counter] = 'E';
    counter++;    
    returnData[counter] = 'T';
    counter++;    
    returnData[counter] = 'T';
    counter++;    
    returnData[counter] = 'L';
    counter++;    
    returnData[counter] = 'C';
    counter++;    
    returnData[counter] = '5';
    counter++;    
    returnData[counter] = '9';
    counter++;    
    returnData[counter] = '4';
    counter++;    
    returnData[counter] = '0';
    counter++;    
    returnData[counter] = '}';
    counter++;       
    delay(1); 

    return returnData;    
  }
#endif

	#if defined(OneWire_h) && defined(COMMAND_DISCOVERONEWIREDEVICES_ENABLED)
  if(command == "discoveronewiredevices"){		
    int devices_found = this->discoverOneWireDevices();

    itoa(devices_found, itoa_buffer, 10);

    int counter = 0;
    returnData[counter] = itoa_buffer[0];
    counter++;
    delay(1);
    
    return returnData;
  }
	#endif

	#if defined(OneWire_h) && defined(COMMAND_GETDS18B20TEMP_ENABLED)
  if(command == "getds18b20temp"){
    char *chartemp_sensor_num = strtok(NULL, "/");
    int temp_sensor_num = atoi(chartemp_sensor_num);
    int temp_reading = getDS18B20Temp(temp_sensor_num);

    itoa(temp_reading, itoa_buffer, 10);

    int counter = 0;
    boolean check_temp_sensor_num = false;
    boolean check_itoa_buffer = false;

    if(chartemp_sensor_num[0] != 0) {
      returnData[counter] = chartemp_sensor_num[0];
      counter++;
    } 
    else {
      returnData[counter] = 48;
      counter++;
      check_temp_sensor_num = true;
    }
    if(chartemp_sensor_num[1] != 0 && check_temp_sensor_num != true) {
      returnData[counter] = chartemp_sensor_num[1];
      counter++;
    }
    returnData[counter] = '/';
    counter++;
    if(itoa_buffer[0] != 0) {
      returnData[counter] = itoa_buffer[0];
      counter++;
    } 
    else {
      returnData[counter] = 48;
      counter++;
      check_itoa_buffer = true;
    }
    if(itoa_buffer[1] != 0 && check_itoa_buffer != true) {
      returnData[counter] = itoa_buffer[1];
      counter++;
    } 
    else {
      check_itoa_buffer = true;
    }
    if(itoa_buffer[2] != 0 && check_itoa_buffer != true) {
      returnData[counter] = itoa_buffer[2];
      counter++;
    } 
    else {
      check_itoa_buffer = true;
    }
    if(itoa_buffer[3] != 0 && check_itoa_buffer != true) {
      returnData[counter] = itoa_buffer[3];
      counter++;
    } 
    else {
      check_itoa_buffer = true;
    }
    if(itoa_buffer[4] != 0 && check_itoa_buffer != true) {
      returnData[counter] = itoa_buffer[4];
      counter++;
    } 
    else {
      check_itoa_buffer = true;
    }
    delay(1);

    return returnData;		
  }
	#endif

	#if defined(OneWire_h) && defined(COMMAND_PUBLISHDS18B20TEMP_ENABLED)
  if(command == "publishds18b20temp"){
    char *chartemp_sensor_num = strtok(NULL, "/");
    int temp_sensor_num = atoi(chartemp_sensor_num);

		if((PUBLISH_ONEWIRE_TEMP_START + temp_sensor_num) <= PUBLISH_ONEWIRE_TEMP_END) {
			this->eepromWrite((PUBLISH_ONEWIRE_TEMP_START + temp_sensor_num), 1);
		}
  }	
	#endif

	#if defined(OneWire_h) && defined(COMMAND_PUBLISHDS18B20TEMP_ENABLED)
  if(command == "unpublishds18b20temp"){
    char *chartemp_sensor_num = strtok(NULL, "/");
    int temp_sensor_num = atoi(chartemp_sensor_num);

		if((PUBLISH_ONEWIRE_TEMP_START + temp_sensor_num) <= PUBLISH_ONEWIRE_TEMP_END) {
			this->eepromWrite((PUBLISH_ONEWIRE_TEMP_START + temp_sensor_num), 0);
		}
  }	
	#endif
	
	#ifdef COMMAND_EC_CONFIGURE
	if(command == "configure_ec"){
		char *char_configuration_num = strtok(NULL, "/");	
		char *char_enable_pin = strtok(NULL, "/");
		char *char_read_pin = strtok(NULL, "/");		
		//ppm, ec, salinity
		char *char_calibration_units = strtok(NULL, "/");
		// uS, SG
		char *char_calibration_scale = strtok(NULL, "/");	
		char *char_calibration_solution_1 = strtok(NULL, "/");
		char *char_calibration_solution_2 = strtok(NULL, "/");	
	
		byte configuration_num = atoi(char_configuration_num);
		byte enable_pin = atoi(char_enable_pin);
		byte read_pin = atoi(char_read_pin);				
	
		this->configureEC(configuration_num, enable_pin, read_pin, char_calibration_units, char_calibration_scale, char_calibration_solution_1, char_calibration_solution_2); 
	
	}
	#endif
	#ifdef COMMAND_READ_EC
	if(command == "readec"){
		char *char_configuration_num = strtok(NULL, "/");	

		byte configuration_num = atoi(char_configuration_num);
	
		this->readEC(configuration_num); 
	}
	#endif	
	

  #ifdef PHENABLED
  if(commandToken == 28){
    char *char_watch_pin = strtok(NULL, "/");
    int watch_pin = atoi(char_watch_pin);

    //float pHReturned = getPHValue(watch_pin) * 100;
    char pHChar[5];

    //itoa (pHReturned, pHChar, 10);

    return pHChar;
  }
  #endif

#ifdef MACROSENABLED
  if(commandToken == 31) {
    char *char_macro_number = strtok(NULL, "/");
    int macro_number = atoi(char_macro_number);

    int max_macros = ((macros_memend - macros_memstart) / macros_bytes) - 1;

    if(macro_number > max_macros) {
      int counter = 0;

      returnData[counter] = '"';
      counter++;      
      returnData[counter] = 'O';
      counter++;
      returnData[counter] = 'U';
      counter++;
      returnData[counter] = 'T';
      counter++;
      returnData[counter] = 'O';
      counter++;
      returnData[counter] = 'F';
      counter++;
      returnData[counter] = 'B';
      counter++;
      returnData[counter] = 'O';
      counter++;
      returnData[counter] = 'U';
      counter++;
      returnData[counter] = 'N';
      counter++;
      returnData[counter] = 'D';
      counter++;
      returnData[counter] = 'S';
      counter++;
      returnData[counter] = '"';
      counter++;      
    } 
    else {
      int counter = 0;
      boolean check_macro_number = false;
      int address_start = macros_memstart + (macros_bytes * macro_number);
      returnData[counter] = '{';
      counter++;
      returnData[counter] = '"';
      counter++;
      returnData[counter] = 'R';
      counter++;
      returnData[counter] = 'U';
      counter++;
      returnData[counter] = 'L';
      counter++;
      returnData[counter] = 'E';
      counter++;
      returnData[counter] = '"';
      counter++;      
      returnData[counter] = ':';
      counter++;      
      returnData[counter] = '"';
      counter++;
      if(char_macro_number[0] != NULL) {
        returnData[counter] = char_macro_number[0];
        counter++;
      } 
      else {
        returnData[counter] = 48;
        counter++;
        check_macro_number = true;
      }
      if(char_macro_number[1] != NULL && check_macro_number != true) {
        returnData[counter] = char_macro_number[1];
        counter++;
      } 
      else {
        check_macro_number = true;
      }
      returnData[counter] = ',';
      counter++;
      for(int i=0; i<macros_bytes; i++) {
        int address = address_start + i;
		    int eeprom_value = this->eepromRead(address);
        if(i < 15) {
          char itoa_buffer[5];
          itoa(eeprom_value, itoa_buffer, 10);
          boolean check_itoa_buffer = false;

          if(itoa_buffer[0] != NULL) {
            returnData[counter] = itoa_buffer[0];
            counter++;
          } 
          else {
            returnData[counter] = 48;
            counter++;
            check_itoa_buffer = true;
          }
          if(itoa_buffer[1] != NULL && check_itoa_buffer != true) {
            returnData[counter] = itoa_buffer[1];
            counter++;  
          } 
          else {
            check_itoa_buffer = true;
          }
          if(itoa_buffer[2] != NULL && check_itoa_buffer != true) {
            returnData[counter] = itoa_buffer[2];
            counter++;  
          } 
          else {
            check_itoa_buffer = true;
          }
          returnData[counter] = ',';
          counter++;          
        } 
        else {
          if(eeprom_value != 0) {
            returnData[counter] = eeprom_value;
            counter++;
          }
        }
      }
      returnData[counter] = '"';
      counter++;      
      returnData[counter] = '}';
      counter++;      
    }  
    delay(1);

    return returnData;
  }
#endif

#ifdef MACROSENABLED
  if(commandToken == 32) {
    char *char_macro_number = strtok(NULL, "/");
    int macro_number = atoi(char_macro_number);

    int address = macros_memstart + (macros_bytes * macro_number);

		this->eepromWrite(address, 0);

    int counter = 0;

    returnData[counter] = '"';
    counter++;
    returnData[counter] = 'D';
    counter++;
    returnData[counter] = 'E';
    counter++;
    returnData[counter] = 'L';
    counter++;
    returnData[counter] = 'E';
    counter++;
    returnData[counter] = 'T';
    counter++;
    returnData[counter] = 'E';
    counter++;
    returnData[counter] = 'M';
    counter++;
    returnData[counter] = 'A';
    counter++;
    returnData[counter] = 'C';
    counter++;
    returnData[counter] = 'R';
    counter++;
    returnData[counter] = 'O';
    counter++;
    returnData[counter] = '"';
    counter++;    
    delay(1);

    return returnData;
  }  
#endif

#ifdef MACROSENABLED
  if(commandToken == 5) {
    char *macro_name = strtok(NULL, "/");
    int arg1 = atoi(strtok(NULL, "/"));
    int arg2 = atoi(strtok(NULL, "/"));
    int arg3 = atoi(strtok(NULL, "/"));
    int arg4 = atoi(strtok(NULL, "/"));
    int arg5 = atoi(strtok(NULL, "/"));
    int arg6 = atoi(strtok(NULL, "/"));
    int arg7 = atoi(strtok(NULL, "/"));
    int arg8 = atoi(strtok(NULL, "/"));
    int arg9 = atoi(strtok(NULL, "/"));
    int arg10 = atoi(strtok(NULL, "/"));
    int arg11 = atoi(strtok(NULL, "/"));   

    macroSet(macro_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);

    counter = 0;

    returnData[counter] = '"';
    counter++;
    returnData[counter] = 'S';
    counter++;
    returnData[counter] = 'E';
    counter++;
    returnData[counter] = 'T';
    counter++;
    returnData[counter] = 'M';
    counter++;
    returnData[counter] = 'A';
    counter++;
    returnData[counter] = 'C';
    counter++;
    returnData[counter] = 'R';
    counter++;
    returnData[counter] = 'O';
    counter++;
    returnData[counter] = '"';
    counter++;     
    delay(1);

    return returnData;
  }
#endif

  #if defined(PCF8574AENABLED) && defined(TwoWire_h)
  if(commandToken == 6) {
    byte device = atoi(strtok(NULL, "/"));
    byte pin = atoi(strtok(NULL, "/"));
    byte action = atoi(strtok(NULL, "/"));

    //void controlPCF8574AA(byte device, byte pin_status, byte pin) {

    controlPCF8574A(device, action, pin);

    counter = 0;

    returnData[counter] = '{';
    counter++;  
    returnData[counter] = '"';
    counter++;
    returnData[counter] = 'S';
    counter++;
    returnData[counter] = 'E';
    counter++;
    returnData[counter] = 'T';
    counter++;
    returnData[counter] = 'P';
    counter++;
    returnData[counter] = 'C';
    counter++;
    returnData[counter] = 'F';
    counter++;
    returnData[counter] = '8';
    counter++;
    returnData[counter] = '5';
    counter++;
    returnData[counter] = '7';
    counter++;     
    returnData[counter] = '4';
    counter++;          
    returnData[counter] = '"';
    counter++;          
    returnData[counter] = ':';
    counter++;  
    returnData[counter] = '"';
    counter++;  
    returnData[counter] = 'Y';
    counter++;      
    returnData[counter] = '"';
    counter++;          
    returnData[counter] = '}';
    counter++;      
    delay(1);

    return returnData;
  }   
  #endif

  memset(commandString, 0, sizeof commandString);  	  
  return returnData;
}

float c2f(float cel) {
  return (cel * (9.0/5.0)) + (float)3200;
}

int ARO_MicrOS::highValue(unsigned int value) {
	if(value > 256) {
		return value / 256;
	} else {
		return 0;
	}
}

int ARO_MicrOS::lowValue(unsigned  value) {
	return value % 256; 
}

int ARO_MicrOS::combineValue(unsigned int lb, unsigned int hb) {
  return ((hb * 256) + lb);
}


void ARO_MicrOS::resetMacros(){
  for(int i = CONFIGURATION_START; i <= CONFIGURATION_END; i++){
		this->eepromWrite(i, 0);
  }
  #if OUTPUT == SERIAL
  this->outputln("Macros reset");
  #endif
}

void ARO_MicrOS::pinModeSet(byte pin, byte mode) {
	this->eepromWrite(DIGITAL_PIN_START+pin, mode);
	#if defined(ARDUINO) && ARDUINO >= 100
	pinMode(pin, mode);
	#endif
	#ifdef STM32F10X_MD
	if(mode == 0) {
		pinMode(pin, INPUT);
	} else if(mode == 1) {
		pinMode(pin, OUTPUT);
	}
	#endif
}

void ARO_MicrOS::setPinStatus(byte pin, byte pinstatus) {
  if(pinstatus <= 1) {
    digitalWrite(pin, pinstatus);
  }
  else if(pinstatus > 1) {
    analogWrite(pin, pinstatus);
  }
}

#if defined(PCF8574AENABLED) && defined(TwoWire_h)
void controlPCF8574A(byte device, byte pin_status, byte pin) {
  if(pin_status == 0) {
    pcf8574a_states[device] |= (1 << pin);
    Wire.beginTransmission(pcf8574a_addresses[device]);
    Wire.write(pcf8574a_states[device]);
    Wire.endTransmission();
  }
  else if(pin_status == 1) {
    pcf8574a_states[device] &= ~(1 << pin);
    Wire.beginTransmission(pcf8574a_addresses[device]);
    Wire.write(pcf8574a_states[device]);
    Wire.endTransmission();    
  }
}
#endif

//TODO - intelligently handle numbers > 255 and store them.
#ifdef MACROSENABLED
void macroSet(char *macro_name,  int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11){
  int arg12;
  int arg13;
  int arg14;
  int memstart;
  for(int i=0; i<((MACROS_END - MACROS_START)/MACRO_BYTE_LENGTH); i++) {
	#if defined(ARDUINO) && ARDUINO >= 100
	if(this->eepromRead((MACROS_START + (i * MACRO_BYTE_LENGTH))) == 0) {
    #endif
	#ifdef STM32F10X_MD
	if(I2_this->eepromRead((MACROS_START + (i * MACRO_BYTE_LENGTH))) == 0) {
	#endif            
      memstart = MACROS_START + (i * MACRO_BYTE_LENGTH);
      break;
    }
  }

  if(arg1 > 255) {
    arg14 = highValue(arg1);    
    arg1 = lowValue(arg1);
  }
  else if(arg2 > 255) {
    arg14 = highValue(arg2);    
    arg2 = lowValue(arg2);    
  }
  else if(arg3 > 255) {
    arg14 = highValue(arg3);    
    arg3 = lowValue(arg3);    
  }
  else if(arg4 > 255) {
    arg14 = highValue(arg4);    
    arg4 = lowValue(arg4);    
  }
  else if(arg5 > 255) {
    arg14 = highValue(arg5);    
    arg5 = lowValue(arg5);
  }
  else if(arg6 > 255) {
    arg14 = highValue(arg6);    
    arg6 = lowValue(arg6);    
  }
  else if(arg7 > 255) {
    arg14 = highValue(arg7);    
    arg7 = lowValue(arg7);    
  }
  else if(arg8 > 255) {
    arg14 = highValue(arg8);    
    arg8 = lowValue(arg8);
  }    

  this->eepromWrite(memstart, 1);
  this->eepromWrite(memstart + 1, arg1);  
  this->eepromWrite(memstart + 2, arg2);
  this->eepromWrite(memstart + 3, arg3);
  this->eepromWrite(memstart + 4, arg4);
  this->eepromWrite(memstart + 5, arg5);
  this->eepromWrite(memstart + 6, arg6);
  this->eepromWrite(memstart + 7, arg7);
  this->eepromWrite(memstart + 8, arg8);
  this->eepromWrite(memstart + 9, arg9);
  this->eepromWrite(memstart + 10, arg10);
  this->eepromWrite(memstart + 11, arg11);
  this->eepromWrite(memstart + 12, arg12);
  this->eepromWrite(memstart + 13, arg13);
  this->eepromWrite(memstart + 14, arg14);  

  int counter = 0;
  boolean check = false;
  for(int i=15; i<= 19; i++) {
    if(macro_name[counter] != NULL && check != true) {
      this->eepromWrite(memstart + i, macro_name[counter]);
      counter++;
    } 
    else if(macro_name[counter] == NULL && check == false) {
      this->eepromWrite(memstart + i, 0);      
      check = true;
    }
  } 
}
#endif

#ifdef DIGITALPINSENABLED
void runDigitalMacro(unsigned int mem_address){
  if(digitalRead(this->eepromRead(mem_address + 2)) == this->eepromRead(mem_address + 3)) {
    if(this->eepromRead(mem_address + 5) <= 1) {
      if(this->eepromRead(mem_address + 4) <= 13) {  
        digitalWrite(this->eepromRead(mem_address + 4), this->eepromRead(mem_address + 5));
      } 
      else if(this->eepromRead(mem_address + 4) == 254) { //pcf8574
        #ifdef PCF8574AENABLED && TwoWire_h
          controlPCF8574A(this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 5), this->eepromRead(mem_address + 7));
        #endif        
      }
    }
    else{
      analogWrite(this->eepromRead(mem_address + 4), this->eepromRead(mem_address + 5));		
    }
  }
}
#endif

#ifdef ANALOGENABLED
void runAnalogMacro(unsigned int mem_address){
  if(this->eepromRead(mem_address + 4) == 1) {
    if(analogRead(this->eepromRead(mem_address + 2)) < combineValue(this->eepromRead(mem_address + 3), this->eepromRead(mem_address + 14))) {
      if(this->eepromRead(mem_address + 6) <= 1) {
        if(this->eepromRead(mem_address + 5) <= 13) {
          digitalWrite(this->eepromRead(mem_address + 5), this->eepromRead(mem_address + 6));
        }
        else if(this->eepromRead(mem_address + 5) == 254) { //pcf8574
          #ifdef PCF8574AENABLED && TwoWire_h
            controlPCF8574A(this->eepromRead(mem_address + 7), this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 8));
          #endif          
        }
      }
      else if(this->eepromRead(mem_address + 6) > 1) {
        analogWrite(this->eepromRead(mem_address + 5), this->eepromRead(mem_address + 6));
      }
    }
  }
  else if(this->eepromRead(mem_address + 4) == 2) {
    if(analogRead(this->eepromRead(mem_address + 2)) > combineValue(this->eepromRead(mem_address + 3), this->eepromRead(mem_address + 14))) {
      if(this->eepromRead(mem_address + 6) <= 1) {
        if(this->eepromRead(mem_address + 5) <= 13) {
          digitalWrite(this->eepromRead(mem_address + 5), this->eepromRead(mem_address + 6));
        }
        else if(this->eepromRead(mem_address + 5) == 254) { //pcf8574
          #ifdef PCF8574AENABLED && TwoWire_h
            controlPCF8574A(this->eepromRead(mem_address + 7), this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 8));
          #endif          
        }
      }
      else if(this->eepromRead(mem_address + 6) > 1) {
        analogWrite(this->eepromRead(mem_address + 5), this->eepromRead(mem_address + 6));
      }
    }	
  }
  else if(this->eepromRead(mem_address + 4) == 3) {
    if(analogRead(this->eepromRead(mem_address + 2)) == combineValue(this->eepromRead(mem_address + 3), this->eepromRead(mem_address + 14))) {
      if(this->eepromRead(mem_address + 6) <= 1) {
        if(this->eepromRead(mem_address + 5) <= 13) {
          digitalWrite(this->eepromRead(mem_address + 5), this->eepromRead(mem_address + 6));
        }
        else if(this->eepromRead(mem_address + 5) == 254) { //pcf8574
          #ifdef PCF8574AENABLED && TwoWire_h
            controlPCF8574A(this->eepromRead(mem_address + 7), this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 8));
          #endif          
        }
      }
      else if(this->eepromRead(mem_address + 6) > 1) {
        analogWrite(this->eepromRead(mem_address + 5), this->eepromRead(mem_address + 6));
      }
    }
  }
}
#endif

#ifdef _RTCLIB_H_
void ARO_MicrOS::DS1307SetTime(int year, byte month, byte day, byte hour, byte minute, byte second) {
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
}
#endif


//TODO
// add in support for on state and off state
#ifdef DS1307_h
void runDS1307Macro(unsigned int mem_address){
  long time_start;
  long time_stop;
  long rtc_time;		

  rtc_time = (RTC.get(DS1307_HR,true) * 60 * 60) + (RTC.get(DS1307_MIN,true) * 60);
  time_start = (this->eepromRead(mem_address + 2) * 60 * 60) + (this->eepromRead(mem_address + 3) * 60);
  time_stop = (this->eepromRead(mem_address + 4) * 60 * 60) + (this->eepromRead(mem_address + 5) * 60);

  if(RTC.get(DS1307_DOW, false) == this->eepromRead(mem_address + 6) || this->eepromRead(mem_address + 6) == 0) {	
    if(rtc_time >= time_start && rtc_time <= time_stop) {
      if(this->eepromRead(mem_address + 8) <= 1) {
        if(this->eepromRead(mem_address + 7) <= 13) {
          digitalWrite(this->eepromRead(mem_address + 7), this->eepromRead(mem_address + 8));
        } else if(this->eepromRead(mem_address + 7) == 254) { //pcf8574
          #ifdef PCF8574AENABLED
            controlPCF8574A(this->eepromRead(mem_address + 10), this->eepromRead(mem_address + 8), this->eepromRead(mem_address + 11));
          #endif
        }
      } else {
        if(rtc_time < (time_start + this->eepromRead(mem_address + 9))) { //fade in
          if(this->eepromRead(mem_address + 7) <= 13) {
            int pwm_value = (rtc_time - time_start) * (this->eepromRead(mem_address + 8) / this->eepromRead(mem_address + 9));
            #ifdef DEBUG
						this->output("PWM Fade In: ");
						this->outputln(pwm_value);
            #endif
            analogWrite(this->eepromRead(mem_address + 7), pwm_value);
          }
        } else if(rtc_time > (time_stop - this->eepromRead(mem_address + 9))) { //fade out
          int pwm_value = (time_stop - rtc_time) * (this->eepromRead(mem_address + 8) / this->eepromRead(mem_address + 9));
          #ifdef DEBUG
					this->output("PWM Fade Out: ");        
					this->outputln(pwm_value);
          #endif        
          analogWrite(this->eepromRead(mem_address + 7), pwm_value);
        } else {
          #ifdef DEBUG
					this->output("PWM On at: ");
					this->outputln(this->eepromRead(mem_address + 8));
          #endif
          analogWrite(this->eepromRead(mem_address + 7), this->eepromRead(mem_address + 8));
        }

      }
    } else if(rtc_time > time_stop) {
      digitalWrite(this->eepromRead(mem_address + 7), 0);
      #ifdef PCF8574AENABLED
        controlPCF8574A(this->eepromRead(mem_address + 10), 0, this->eepromRead(mem_address + 11));    
      #endif
    }
  }
}
#endif

#if defined(OneWire_h) && defined(COMMAND_DISCOVERONEWIREDEVICES_ENABLED)
int ARO_MicrOS::discoverOneWireDevices() {
  OneWire ds(ONEWIRE_PIN);

  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  unsigned int d = 0;
  unsigned int e;

  while(ds.search(addr)) {
    e = ONEWIRE_ADDRESSES_START + (d * ONEWIRE_ADDRESSES_BYTES);
    
    for( i = 0; i < 8; i++) {
      this->eepromWrite((e + i), addr[i]);
      #ifdef DEBUG
      this->output(addr[i], HEX);
      #endif
    }
    #ifdef DEBUG
    this->outputln();
    #endif

    if ( OneWire::crc8( addr, 7) != addr[7]) {
      return 0;
    }
    d++;
  }
  this->eepromWrite(NUM_ONEWIRE_DEVICES, d);

  ds.reset_search();
  return d;
}
#endif

#if defined(OneWire_h) && defined(COMMAND_GETDS18B20TEMP_ENABLED)
int ARO_MicrOS::getDS18B20Temp(int device_num) {
  OneWire ds(ONEWIRE_PIN);

  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

  for(i = 0; i < 8; i++){
    addr[i] = this->eepromRead((ONEWIRE_ADDRESSES_START + (ONEWIRE_ADDRESSES_BYTES * device_num) + i)); 
  }

	#ifdef DEBUG
	if ( OneWire::crc8( addr, 7) != addr[7]) {
		this->output("CRC is not valid!\n");
	}
	#endif

  if ( addr[0] != 0x28) {
		#ifdef DEBUG
    this->output("Device is not a DS18S20 family device.\n");
    #endif
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);

#if ONEWIRE_PARASITIC_POWER_MODE == 1	
  delay(1000);
#endif

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

#ifndef CELSIUS
  Tc_100 = c2f(Tc_100);
#endif 

  return Tc_100;
}
#endif

#ifdef ORPENABLED
int getORPValue(unsigned int pin) {
  int analogreadings;
  int millivolts;
  unsigned int i;

  analogreadings = 0;
  for(i = 1; i < 50; i++){
    analogreadings += analogRead(pin);
    // give analog pin time to recover
    delay(10);
  }
  analogreadings /= i;

  //millivolts = ((analogreadings * ARDUINO_VOLTAGE) / 1024) * 1000;
  return millivolts; 
}
#endif

#if defined(OneWire_h) && defined(EEPROM_h)
void ARO_MicrOS::runDS18B20Macro(int mem_address) {
  int temp;

  temp = this->getDS18B20Temp(this->eepromRead(mem_address + 2));

  if(this->eepromRead(mem_address + 3) == 1) {
    if(temp < ((this->eepromRead(mem_address + 4) * 100) + this->eepromRead(mem_address + 5))) {
      if(this->eepromRead(mem_address + 7) <= 1) {
        if(this->eepromRead(mem_address + 6) <= 13) {
          digitalWrite(this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 7));
        }
        else if(this->eepromRead(mem_address + 6) == 254) { //pcf8574
          #if defined(PCF8574AENABLED) && defined(TwoWire_h)
            controlPCF8574A(this->eepromRead(mem_address + 8), this->eepromRead(mem_address + 7), this->eepromRead(mem_address + 9));
          #endif
        }
      } 
      else {
        analogWrite(this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 7));			
      }
    }
  } 
  else if(this->eepromRead(mem_address + 3) == 2) {
    if(temp > ((this->eepromRead(mem_address + 4) * 100) + this->eepromRead(mem_address + 5))) {
      if(this->eepromRead(mem_address + 7) <= 1) {
        if(this->eepromRead(mem_address + 6) <= 13) {
          digitalWrite(this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 7));
        }
        else if(this->eepromRead(mem_address + 6) == 254) { //pcf8574
          #if defined(PCF8574AENABLED) && defined(TwoWire_h)
            controlPCF8574A(this->eepromRead(mem_address + 8), this->eepromRead(mem_address + 7), this->eepromRead(mem_address + 9));
          #endif
        }
      } 
      else {
        analogWrite(this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 7));			
      }
    }	
  } 
  else if(this->eepromRead(mem_address + 3) == 3) {
    if(temp == ((this->eepromRead(mem_address + 4) * 100) + this->eepromRead(mem_address + 5))) {
      if(this->eepromRead(mem_address + 7) <= 1) {
        if(this->eepromRead(mem_address + 6) <= 13) {
          digitalWrite(this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 7));
        }
        else if(this->eepromRead(mem_address + 6) == 254) { //pcf8574
          #if defined(PCF8574AENABLED) && defined(TwoWire_h)
            controlPCF8574A(this->eepromRead(mem_address + 8), this->eepromRead(mem_address + 7), this->eepromRead(mem_address + 9));
          #endif
        }
      } 
      else {
        analogWrite(this->eepromRead(mem_address + 6), this->eepromRead(mem_address + 7));			
      }
    }	
  }
}
#endif  

#ifdef TLC5940_H
void controlTLC5940(int tlc_pin_num, int tlc_action) {
  Tlc.set(tlc_pin_num, tlc_action);
  Tlc.update();
  delay(100);
}
#endif

void ARO_MicrOS::runMacros() {
	#if defined(cc3000_PubSubClient_h) && defined(OneWire_h)
	for(int j = PUBLISH_ONEWIRE_TEMP_START; j <= PUBLISH_ONEWIRE_TEMP_END; j++) {
		if(this->eepromRead(j) == 1) {		
			if(mqttclient.connected()) {
				char chartemp[10];
				//PUBLISH_ONEWIRE_TEMP_END + j
				int temp = this->getDS18B20Temp(0);
				itoa(temp, chartemp, 10);
				
				char device[10];
				int deviceNum = j - PUBLISH_ONEWIRE_TEMP_START;
				itoa(deviceNum, device, 10);
				
				memset(mqttString, 0, sizeof mqttString); 
				strcat(mqttString, MQTT_Username);
				strcat(mqttString, "/"); 
				strcat(mqttString, this->deviceID());
				strcat(mqttString, "/publish/temp/");
				strcat(mqttString, device);
				mqttclient.publish(mqttString, chartemp);
			}
		}
	}
	#endif
/*  int macro_mem_address_start;
  for(unsigned int i = 0; i < ((MACROS_END - MACROS_START) / MACRO_BYTE_LENGTH); i++) {
    macro_mem_address_start = MACROS_START + (i * MACRO_BYTE_LENGTH);
    if(this->eepromRead(macro_mem_address_start) == 1) {    
      if(this->eepromRead(macro_mem_address_start + 1) == 1) {
        #ifdef DIGITALPINSENABLED
          runDigitalMacro(macro_mem_address_start);
        #endif
      }
      else if(this->eepromRead(macro_mem_address_start + 1) == 2) {
        #ifdef ANALOGENABLED
          runAnalogMacro(macro_mem_address_start);
        #endif  
      }
      else if(this->eepromRead(macro_mem_address_start + 1) == 3) {
#ifdef DS1307_h
        runDS1307Macro(macro_mem_address_start);
#endif
      }
      else if(this->eepromRead(macro_mem_address_start + 1) == 4) {
        #ifdef OneWire_hh && DS18B20ENABLED
          runDS18B20Macro(macro_mem_address_start);
        #endif
      }
    }
  }*/
}

/**************************************************************************/
/*!
 @brief  Tries to read the 6-byte MAC address of the CC3000 module
 */
/**************************************************************************/
/*void displayMACAddress(void)
{
  uint8_t macAddress[6];

  if(!cc3000.getMacAddress(macAddress))
  {
    this->outputln(F("Unable to retrieve MAC Address!\r\n"));
  }
  else
  {
    this->output(F("MAC Address : "));
    cc3000.printHex((byte*)&macAddress, 6);
  }
}*/

void ARO_MicrOS::setDeviceAddress() {
  #if defined(ARDUINO) && ARDUINO >= 100
  if(this->eepromRead(ADDRESS_START) != 253) {
    this->eepromWrite(ADDRESS_START, 253); 
    for(byte j = ADDRESS_START+1; j < ADDRESS_END; j++) {
      this->eepromWrite(j, random(65, 90));  
    }
  } else {
    for(byte j = ADDRESS_START+1; j < ADDRESS_END; j++) {
      this->_deviceID[j-1] = this->eepromRead(j);
	  this->_deviceID[j] = '\0';
    }
	#if OUTPUT == SERIAL
	this->output("Device ID: ");
	this->outputln(this->_deviceID);
	#endif   
  }
  #endif
  
  #ifdef STM32F10X_MD
  String myID = Spark.deviceID();      
  char charmyID[40];
  strcpy(charmyID, myID.c_str());  
  for(byte j = ADDRESS_START+1; j < ADDRESS_END; j++) {
    this->_deviceID[j-1] = charmyID[j];
    this->_deviceID[j] = '\0';
  }
  #ifdef DEBUG
  this->output("Device ID: ");
  this->outputln(this->_deviceID);
  #endif     
  #endif
  return;
}

char* ARO_MicrOS::deviceID() {
	return this->_deviceID;
}

void ARO_MicrOS::eepromWrite(unsigned int address, byte value) {
	#if defined(ARDUINO) && ARDUINO >= 100
		EEPROM.write(address, value); 
		return;
	#endif
}

byte ARO_MicrOS::eepromRead(unsigned int address) {
	byte value;
	#if defined(ARDUINO) && ARDUINO >= 100
		return EEPROM.read(address);
	#endif
}

/**************************************************************************/
/*!
 @brief  Tries to read the CC3000's internal firmware patch ID
 */
/**************************************************************************/
#ifdef ADAFRUIT_CC3000_H
uint16_t ARO_MicrOS::cc3000CheckFirmwareVersion(Adafruit_CC3000& cc3000) {
  uint8_t major, minor;
  uint16_t version;

  #ifndef CC3000_TINY_DRIVER  
  if(!cc3000.getFirmwareVersion(&major, &minor)) {
    version = 0;
  } 
  else {
    version = major; 
    version <<= 8; 
    version |= minor;
  }
  #endif
  return version;
}
#endif

/**************************************************************************/
/*!
 @brief  Tries to read the IP address and other connection details
 */
/**************************************************************************/
#ifdef ADAFRUIT_CC3000_H
bool ARO_MicrOS::displayConnectionDetails(Adafruit_CC3000& cc3000)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;

  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
  	#ifdef DEBUG
    this->outputln(F("Unable to retrieve the IP Address!\r\n"));
    #endif
    return false;
  }
  else
  {
  	#ifdef DEBUG
    this->output(F("\nIP Addr: ")); 
    cc3000.printIPdotsRev(ipAddress);
    this->output(F("\nNetmask: ")); 
    cc3000.printIPdotsRev(netmask);
    this->output(F("\nGateway: ")); 
    cc3000.printIPdotsRev(gateway);
    this->output(F("\nDHCPsrv: ")); 
    cc3000.printIPdotsRev(dhcpserv);
    this->output(F("\nDNSserv: ")); 
    cc3000.printIPdotsRev(dnsserv);
    this->outputln();
    #endif
    return true;
  }
}
#endif

#ifdef COMMAND_CONFIGUREISE_ENABLED
bool ARO_MicrOS::configureISE(byte pin, char *value1, char *value2, char *type) {
	byte x = 0;
	byte counter = 0;
	unsigned int address_start;
	unsigned int address_pointer;
	
	while(x == 0) {
		if(this->eepromRead(CONFIGURATION_START + (counter * 40)) == 0) {
			address_pointer = CONFIGURATION_START + (counter * 40);
			address_pointer = address_pointer;
			x = 1;
			#ifdef DEBUG
			this->outputln("Matched pin");
			#endif			
		} else {
			counter++;
			#ifdef DEBUG
			this->outputln(CONFIGURATION_START + 1 + (counter * 40));
			#endif
		}
	}
	
	if((address_start + CONFIGURATION_LENGTH) > CONFIGURATION_END) {
		#if OUTPUT == SERIAL
		this->outputln("Out of address space");
		#endif
		return false;
	}

	#if OUTPUT == SERIAL
	this->output("Pin: ");
	this->outputln(pin);
	this->output("Value 1: ");
	this->outputln(value1);
	this->output("Value 2: ");
	this->outputln(value2);			
	this->output("Type: ");
	this->outputln(type);				
	#endif

	byte matchCount;
	unsigned long reading = 0;
	int oldValue = 1;
	int newValue = 0;	

	this->eepromWrite(address_pointer, CONFIGURATION_TYPE_ISE);	
	address_pointer++;
	this->eepromWrite(address_pointer, pin);	
	address_pointer++;	
	while(*type) {
		this->eepromWrite(address_pointer, *type++);	
		address_pointer++;
	}	
	this->eepromWrite(address_pointer, 255);
	address_pointer++;		
	while(*value1) {
		this->eepromWrite(address_pointer, *value1++);	
		address_pointer++;
	}	
	this->eepromWrite(address_pointer, 255);
	address_pointer++;	
	while(*value2) {
		this->eepromWrite(address_pointer, *value2++);	
		address_pointer++;
	}	
	this->eepromWrite(address_pointer, 255);
	address_pointer++;		

	#if OUTPUT == SERIAL
	this->output("Configured match count: ");
	this->outputln(ISE_CONFIGURE_MATCHCOUNT);
	#endif

	matchCount = 0;
	reading = 0;
	while(matchCount < ISE_CONFIGURE_MATCHCOUNT) {
		oldValue = newValue;
		reading = 0;
		for(byte i=0; i<255; i++) {
			reading = reading + analogRead(pin);
			delay(1);
		}
		#if OUTPUT == SERIAL
		this->output("Match count: ");
		this->outputln(matchCount);
		#endif		
		newValue = reading/255;

		if(newValue == oldValue) {
			matchCount++;
		}
		#ifdef DEBUG
		this->output("Old value: ");
		this->outputln(oldValue);
		this->output("New value: ");
		this->outputln(newValue);    
		this->output("Match Count: ");
		this->outputln(matchCount);   
		#endif
		delay(500); 
	}
	matchCount = 0;

	this->eepromWrite(address_pointer, this->lowValue(newValue));
	address_pointer++;		  
	this->eepromWrite(address_pointer, this->highValue(newValue)); 
	address_pointer++;
	this->eepromWrite(address_pointer, 255); 

	#if OUTPUT == SERIAL
	this->outputln("Now configuring second solution. Please place probe in second solution. Waiting 30 seconds.");
	#endif
	
	delay(30000);
	
	oldValue = 1;
	newValue = 0;
	while(matchCount < ISE_CONFIGURE_MATCHCOUNT) {
		oldValue = newValue;
		reading = 0;
		#if OUTPUT == SERIAL
		this->output("Match count: ");
		this->outputln(matchCount);
		#endif
		for(byte i=0; i<255; i++) {
			reading = reading + analogRead(pin);
			delay(1);
		}
		newValue = reading/255;

		if(newValue == oldValue) {
			matchCount++;
		}
		#ifdef DEBUG
		this->output("Old value: ");
		this->outputln(oldValue);
		this->output("New value: ");
		this->outputln(newValue);    
		this->output("Match Count: ");
		this->outputln(matchCount);   
		#endif
		delay(500); 
	}
	matchCount = 0;

	this->eepromWrite(address_pointer, this->lowValue(newValue));
	address_pointer++;		  
	this->eepromWrite(address_pointer, this->highValue(newValue)); 
	address_pointer++;
	this->eepromWrite(address_pointer, 255);
	
	#if OUTPUT == SERIAL
	this->outputln("Completed ISE configuration");
	#endif	

	return true;  
}
#endif

#ifdef COMMAND_READISE_ENABLED
float ARO_MicrOS::readISE(byte pin) {
	byte x = 0;
	byte counter = 0;
	unsigned int address_pointer = CONFIGURATION_START;

	while(x == 0 && address_pointer <= CONFIGURATION_END) {
		#ifdef DEBUG
		this->output("Address pointer: ");
		this->outputln(address_pointer);
		#endif

		if(this->eepromRead(address_pointer) == CONFIGURATION_TYPE_ISE && this->eepromRead(address_pointer+1) == pin) {
			x = 1;
			#ifdef DEBUG
			this->outputln("Matched pin");
			#endif			
		} else {
			counter++;
			address_pointer = address_pointer + CONFIGURATION_LENGTH;
		}
	}

	float X1;
	float X2;
	unsigned int Y1;
	unsigned int Y2;	
	char buffer[10];
	char type[6];
	byte char_counter = 0;

	// add 2 because first 2 bytes are type and pin
	address_pointer += 2;

	while(this->eepromRead(address_pointer) != 255) {
		type[char_counter] = this->eepromRead(address_pointer);
		char_counter++;
		type[char_counter+1] = '\0';		
		address_pointer++;
	}
	address_pointer++;
	char_counter = 0;	
	while(this->eepromRead(address_pointer) != 255) {
		buffer[char_counter] = this->eepromRead(address_pointer);
		char_counter++;
		buffer[char_counter+1] = '\0';		
		address_pointer++;
	}
	address_pointer++;
	char_counter = 0;	
	X1 = atof(buffer);
	while(this->eepromRead(address_pointer) != 255) {
		buffer[char_counter] = this->eepromRead(address_pointer);
		char_counter++;
		buffer[char_counter+1] = '\0';		
		address_pointer++;		
	}
	address_pointer++;
	char_counter = 0;	
	X2 = atof(buffer);

	Y1 = this->combineValue(this->eepromRead(address_pointer), this->eepromRead(address_pointer + 1));
	address_pointer += 2;
	Y2 = this->combineValue(this->eepromRead(address_pointer), this->eepromRead(address_pointer + 1));

	#ifdef DEBUG
	this->output("Pin: ");
	this->outputln(pin);	
	this->output("type: ");
	this->outputln(type);
	this->output("X1: ");	
	this->outputln(X1);
	this->output("X2: ");	
	this->outputln(X2);	
	this->output("Y1: ");	
	this->outputln(Y1);
	this->output("Y2: ");	
	this->outputln(Y2);		
	#endif

	//work out m for y = mx + b
	float m = ((float)Y1 - (float)Y2) / ((float)X1 - (float)X2);
	//work out b for y = mx + 
	float b = (float)Y1 - ((float)m * (float)X1);

	#ifdef DEBUG
	this->output("X1: ");
	this->outputln(X1);
	this->output("X2: ");
	this->outputln(X2);  
	this->output("Y1: ");
	this->outputln(Y1);  
	this->output("Y2: ");
	this->outputln(Y2);  
	this->output("M: ");
	this->outputln(m);
	this->output("B: ");
	this->outputln(b);
	#endif

	long reading = 0;
	for(byte i=0; i<255; i++) {
	reading = reading + analogRead(pin);
	delay(10);
	}
	reading = reading/255;
	#ifdef DEBUG
	this->output("Analog: ");
	this->outputln(reading);
	#endif

	float y = (m*(float)reading) + b;

	#if OUTPUT == SERIAL
	this->output(type);
	this->output(" ");
	this->output(y);
	#endif
	
	return y;
}
#endif

#if defined(ADAFRUIT_CC3000_H)
bool ARO_MicrOS::connectCC3000(Adafruit_CC3000& cc3000, Adafruit_CC3000_Client& client, const char* wlan_ssid, const char* wlan_pass, uint8_t wlan_security, bool reconnect) {
	if(reconnect == true) {
		#ifdef DEBUG
		this->outputln("Rebooting cc3000");
		#endif
		cc3000.reboot();
		#ifdef DEBUG
		this->outputln("cc3000 Rebooted");
		#endif		
	}

  if (!cc3000.begin()) {
    #ifdef DEBUG
    this->outputln(F("Unable to initialise the CC3000! Check your wiring?"));
    #endif
    for(;;);
  }

  if ((this->cc3000CheckFirmwareVersion(cc3000) != 0x113) && (this->cc3000CheckFirmwareVersion(cc3000) != 0x118)) {
    //TODO have an error LED blink if wrong firmware
    #ifdef DEBUG
    this->outputln(F("Wrong firmware version!"));
    #endif
    for(;;);
  } else {
    #ifdef DEBUG
    this->output("Firmware V.");
    this->outputln(this->cc3000CheckFirmwareVersion(cc3000));
    #endif
  }

  #ifdef DEBUG
  this->outputln(F("\nDeleting old connection profiles"));
  #endif
  
  if (!cc3000.deleteProfiles()) {
    #ifdef DEBUG
    this->outputln(F("Failed to deleteProfiles()!"));
    #endif
    while(1);
  }

  #ifdef DEBUG
  this->output(F("\nAttempting to connect to ")); 
  this->outputln(wlan_ssid);
  #endif

  /* NOTE: Secure connections are not available in 'Tiny' mode! */
  if (!cc3000.connectToAP(wlan_ssid, wlan_pass, wlan_security)) {
    #ifdef DEBUG
    this->outputln(F("Failed to connect to SSID!"));
    #endif
    while(1);
  }

  #ifdef DEBUG
  this->outputln(F("Connected!"));
  /* Wait for DHCP to complete */
  this->outputln(F("Request DHCP"));
  #endif
  
  while (!cc3000.checkDHCP()) {
    delay(100); // ToDo: Insert a DHCP timeout!
  }

  /* Display the IP address DNS, Gateway, etc. */
  while (!this->displayConnectionDetails(cc3000)) {
    delay(1000);
  }
}
#endif

#if defined(cc3000_PubSubClient_h) && defined(ADAFRUIT_CC3000_H)
bool ARO_MicrOS::connectMQTT(Adafruit_CC3000& cc3000, Adafruit_CC3000_Client& client, cc3000_PubSubClient& mqttclient, char* MQTT_Username, char* MQTT_Password, uint32_t server) {
  // connect to the broker
  if (!client.connected()) {
    client = cc3000.connectTCP(server, 1883);
    #ifdef DEBUG
    this->outputln("cc3000.connectTCP Complete");
    #endif
  }
  
  // did that last thing work? sweet, let's do something
  if(client.connected()) {
    if (mqttclient.connect(this->deviceID(), MQTT_Username, MQTT_Password)) {
      memset(mqttString, 0, sizeof mqttString); 
      strcat(mqttString, MQTT_Username);
      strcat(mqttString, "/"); 
      strcat(mqttString, "checkin");
      mqttclient.publish(mqttString, this->deviceID());
      memset(mqttString, 0, sizeof mqttString); 
      strcat(mqttString, MQTT_Username);
      strcat(mqttString, "/");
      strcat(mqttString, this->deviceID());      
      strcat(mqttString, "/");      
      strcat(mqttString, "control");
      #ifdef DEBUG
      this->outputln(mqttString);
      #endif
      mqttclient.subscribe(mqttString);
      return true;
    }
  } else {
  	return false;
  } 
}
#endif

#ifdef STM32F10X_MD
bool ARO_MicrOS::sparkConnectMQTT(char* MQTT_Username, char* MQTT_Password) {
    #ifdef DEBUG
    this->outputln("mqttclient connecting");
    #endif    
	if (mqttclient.connect(this->deviceID(), MQTT_Username, MQTT_Password)) {
	    #ifdef DEBUG
	    this->outputln("mqttclient connected");
	    #endif
		memset(mqttString, 0, sizeof mqttString); 
		strcat(mqttString, MQTT_Username);
		strcat(mqttString, "/"); 
		strcat(mqttString, "checkin");
		mqttclient.publish(mqttString, this->deviceID());
		memset(mqttString, 0, sizeof mqttString); 
		strcat(mqttString, MQTT_Username);
		strcat(mqttString, "/");
		strcat(mqttString, this->deviceID());      
		strcat(mqttString, "/");      
		strcat(mqttString, "control");
		#ifdef DEBUG
		this->outputln(mqttString);
		#endif
		mqttclient.subscribe(mqttString);
		return true;
	}
	return false; 
}
#endif

void ARO_MicrOS::serialInterface() {
	char serialCommandString[BUFFERSIZE];
	char serialReturnData[BUFFERSIZE];
	memset(serialReturnData, 0, sizeof serialReturnData);
	memset(serialCommandString, 0, sizeof serialCommandString);	
	byte inByte;
	byte index = 0;

	while(Serial.available() > 0) {	
		inByte = Serial.read();
		serialCommandString[index] = inByte;
		#ifdef DEBUG
		this->output("Received: ");
		this->outputln(serialCommandString[index]);
		#endif
		delay(1);
		index++;
		serialCommandString[index] = '\0';
	}

	this->outputln(this->control(serialReturnData, serialCommandString));

	return;
}

#if defined(cc3000_PubSubClient_h) && defined(ADAFRUIT_CC3000_H) || defined(STM32F10X_MD)
void ARO_MicrOS::publishCheckin() {	
  this->checkinCurrentMillis = millis();
  if ((this->checkinCurrentMillis - this->checkinPreviousMillis) >= 30000){
    this->checkinPreviousMillis = this->checkinCurrentMillis;     
    memset(mqttString, 0, sizeof mqttString); 
    strcat(mqttString, MQTT_Username);
    strcat(mqttString, "/");
    strcat(mqttString, "checkin");
    if(mqttclient.connected()) {
      mqttclient.publish(mqttString, this->deviceID());
    }
  }
}
#endif

#if defined(cc3000_PubSubClient_h) || (defined(STM32F10X_MD) && defined(PubSubClient_h))
void ARO_MicrOS::mqttPublish(char* topic, char* payload) {
	#ifdef DEBUG
	this->outputln(topic);
	this->outputln(payload);
	#endif
	mqttclient.publish(topic, payload);
}
#endif

void ARO_MicrOS::loop() {
	// if we don't get a true back from mqttclient reconnect cc3000 and reconnect to mqtt server
	#ifdef STM32F10X_MD
	if(!mqttclient.loop()) {
	    #ifdef DEBUG
	    this->outputln("mqttclient not connected");
	    #endif
	    this->sparkConnectMQTT(MQTT_Username, MQTT_Password);
	}
	#endif
	
	#ifdef cc3000_PubSubClient_h
	if (!mqttclient.loop()) {
		#ifdef DEBUG
		this->outputln("mqttclient.loop() failed");
		#endif
		if(!this->connectCC3000(cc3000, client, wlan_ssid, wlan_pass, wlan_security, true)) {
			#ifdef DEBUG
			this->outputln("connectCC3000");
			#endif
			while(1);
		}
		if(!this->connectMQTT(cc3000, client, mqttclient, MQTT_Username, MQTT_Password, server.ip)) {
			#ifdef DEBUG
			this->outputln("connectMQTT");
			#endif
			while(1);
		}
	}
	#endif

	
	#if defined(ARDUINO) && ARDUINO >= 100 && OUTPUT == SERIAL
	if (Serial.available() > 0) {
		this->serialInterface();
	}
	#endif

	this->runMacros();

	#if defined(cc3000_PubSubClient_h) || defined(STM32F10X_MD)
	this->publishCheckin();
	#endif
}

void ARO_MicrOS::configureEC(byte configuration_num, byte enable_pin, byte read_pin, char *char_calibration_units, char *char_calibration_scale, char *char_calibration_solution_1, char *char_calibration_solution_2) {
	if((CONFIGURATION_START + (CONFIGURATION_LENGTH * configuration_num)) > CONFIGURATION_END) {
		this->output("Configuration number out of bounds!");
		return;
	}

	unsigned int address_start = CONFIGURATION_START + (CONFIGURATION_LENGTH * configuration_num);
	unsigned int address_pointer = 0;
	float calibration_solution_1 = atof(char_calibration_solution_1);
	float calibration_solution_2 = atof(char_calibration_solution_2);	
	
	address_pointer = address_start;
	this->eepromWrite(address_pointer, CONFIGURATION_TYPE_EC);
	address_pointer++;	
	this->eepromWrite(address_pointer, enable_pin);
	address_pointer++;	
	this->eepromWrite(address_pointer, read_pin);	
	address_pointer++;	
	while(*char_calibration_units) {
		this->eepromWrite(address_pointer, *char_calibration_units++);	
		address_pointer++;
	}	
	this->eepromWrite(address_pointer, 255);	
	address_pointer++;	
	while(*char_calibration_scale) {
		this->eepromWrite(address_pointer, *char_calibration_scale++);	
		address_pointer++;
	}	
	this->eepromWrite(address_pointer, 255);	
	address_pointer++;		
	while(*char_calibration_solution_1) {
		this->eepromWrite(address_pointer, *char_calibration_solution_1++);	
		address_pointer++;
	}	
	this->eepromWrite(address_pointer, 255);	
	address_pointer++;		
	while(*char_calibration_solution_2) {
		this->eepromWrite(address_pointer, *char_calibration_solution_2++);	
		address_pointer++;
	}	
	this->eepromWrite(address_pointer, 255);	
	address_pointer++;

	pinMode(enable_pin, OUTPUT);
	unsigned long oldValue = 0;
	unsigned int matchCount = 0;
	
	unsigned long freqhigh = 0;
	unsigned long freqlow = 0;
	unsigned long frequency = 0;
	
	while(matchCount < EC_MATCH_COUNT) {
		oldValue = frequency;
		frequency = 0;
		this->outputln("Not stable, carrying on");
		freqhigh = 0;
		freqlow = 0;
		digitalWrite(enable_pin, HIGH);
		for(unsigned int i=0; i<EC_SAMPLES; i++) {
			freqhigh+= pulseIn(read_pin, HIGH, 250000);
			freqlow+= pulseIn(read_pin, LOW, 250000);			
		}
		digitalWrite(enable_pin, LOW);
		frequency = 1000000 / ( (freqhigh / EC_SAMPLES) + (freqlow / EC_SAMPLES) );

		if(oldValue > (frequency - 10) && oldValue < (frequency + 10)) {
			matchCount++;
		}

		#ifdef DEBUG
		this->output("Frequency: ");
		this->outputln(frequency);
		this->output("Old value: ");
		this->outputln(oldValue);
		this->output("Match Count: ");
		this->outputln(matchCount);   
		#endif
	}
	this->eepromWrite(address_pointer, this->lowValue(frequency));	
	address_pointer++;			
	this->eepromWrite(address_pointer, this->highValue(frequency));	
	address_pointer++;			
	this->eepromWrite(address_pointer, 255);	
	address_pointer++;

	this->outputln("Waiting 30 seconds. Please place probe in second solution");	
	delay(30000);
	
	matchCount = 0;
	while(matchCount < EC_MATCH_COUNT) {
		oldValue = frequency;
		frequency = 0;
		this->outputln("Not stable, carrying on");
		freqhigh = 0;
		freqlow = 0;
		digitalWrite(enable_pin, HIGH);
		for(unsigned int i=0; i<EC_SAMPLES; i++) {
			freqhigh+= pulseIn(read_pin, HIGH, 250000);
			freqlow+= pulseIn(read_pin, LOW, 250000);			
		}
		digitalWrite(enable_pin, LOW);
		frequency = 1000000 / ( (freqhigh / EC_SAMPLES) + (freqlow / EC_SAMPLES) );

		if(oldValue > (frequency - 10) && oldValue < (frequency + 10)) {
			matchCount++;
		}

		#ifdef DEBUG
		this->output("Frequency: ");
		this->outputln(frequency);
		this->output("Old value: ");
		this->outputln(oldValue);
		this->output("Match Count: ");
		this->outputln(matchCount);   
		#endif
	}
	this->eepromWrite(address_pointer, this->lowValue(frequency));	
	address_pointer++;			
	this->eepromWrite(address_pointer, this->highValue(frequency));	
	address_pointer++;			
	this->eepromWrite(address_pointer, 255);	
	address_pointer++;
}

float ARO_MicrOS::readEC(byte configuration_num) {
	unsigned int address_start = CONFIGURATION_START + (CONFIGURATION_LENGTH * configuration_num);
	unsigned int address_pointer = 0;
	char calibration_units[10];
	char calibration_scale[10];
	char buffer[10];
	byte char_counter = 0;
	float X1;
	float X2;	
	unsigned long Y1;
	unsigned long Y2;
	unsigned long frequencyHigh = 0;
	unsigned long frequencyLow = 0;	
	unsigned long frequency = 0;
		
	address_pointer = address_start + 1;
	byte enable_pin = this->eepromRead(address_pointer);
	address_pointer++;
	byte read_pin = this->eepromRead(address_pointer); 
	address_pointer++;
	while(this->eepromRead(address_pointer) != 255) {
		calibration_units[char_counter] = this->eepromRead(address_pointer);
		char_counter++;
		calibration_units[char_counter+1] = '\0';		
		address_pointer++;		
	}
	address_pointer++;
	char_counter = 0;
	while(this->eepromRead(address_pointer) != 255) {
		calibration_scale[char_counter] = this->eepromRead(address_pointer);
		char_counter++;
		calibration_scale[char_counter+1] = '\0';		
		address_pointer++;		
	}
	address_pointer++;
	char_counter = 0;
	while(this->eepromRead(address_pointer) != 255) {
		buffer[char_counter] = this->eepromRead(address_pointer);
		char_counter++;
		buffer[char_counter+1] = '\0';		
		address_pointer++;		
	}
	address_pointer++;
	char_counter = 0;
	X1 = atof(buffer);
	while(this->eepromRead(address_pointer) != 255) {
		buffer[char_counter] = this->eepromRead(address_pointer);
		char_counter++;
		buffer[char_counter+1] = '\0';		
		address_pointer++;		
	}
	address_pointer++;
	char_counter = 0;
	X2 = atof(buffer);	
	Y1 = this->combineValue(this->eepromRead(address_pointer++), this->eepromRead(address_pointer++));
	address_pointer++;	
	Y2 = this->combineValue(this->eepromRead(address_pointer++), this->eepromRead(address_pointer++));
	address_pointer++;		
	
	#ifdef DEBUG
	this->outputln(enable_pin);	
	this->outputln(read_pin);	
	this->outputln(calibration_units);
	this->outputln(calibration_scale);	
	this->outputln(X1);
	this->outputln(X2);	
	this->outputln(Y1);
	this->outputln(Y2);	
	#endif	

	pinMode(enable_pin, OUTPUT);
	digitalWrite(enable_pin, HIGH);

	for(unsigned int j=0; j<EC_SAMPLES; j++) {
		frequencyHigh+= pulseIn(read_pin, HIGH, 250000);
		frequencyLow+= pulseIn(read_pin, LOW, 250000);
	}
	digitalWrite(enable_pin, LOW);  
 
	frequency = 1000000 / ( (frequencyHigh / EC_SAMPLES) + (frequencyLow / EC_SAMPLES) );
 
	#ifdef DEBUG
	this->output("frequencyHigh: ");
	this->outputln(frequencyHigh);
	this->output("frequencyLow: ");
	this->outputln(frequencyLow);  
	this->output("Frequency: ");
	this->outputln(frequency);
	#endif

	// now we need to work out all the values for this formula
	//y = mx + b
 
	float m = ((float)Y2 - (float)Y1) / ((float)X2 - (float)X1);

	#ifdef DEBUG
	this->output("m= ");
	this->outputln(m, 8);
	#endif

	float b = Y1 - (m * X1);

	#ifdef DEBUG 
	this->output("b= ");
	this->outputln(b, 8);
	#endif

	float x = ((float)frequency - b) / m;

	#ifdef DEBUG 
	this->output("x: ");
	this->outputln(x, 8);
	#endif
 

  //to temperature compensate this you'll need to get a temp reading and plug it in to this formula:
  //EC25 = ECf / (1 + x(t - 25)) change 25 to 18 if you want to compensate to 18.
  //see the top comments for more info

 
  //lets say it's 21 degrees C
  //float EC25 = x / (1 + (0.019 * (21 - 25)));
  //this->output("EC25 = ");
  //this->outputln(EC25, 8);
 

  
  return x;
}

void ARO_MicrOS::output(const char* string) {
	#if OUTPUT == SERIAL
	Serial.print(string);
	#endif
}
void ARO_MicrOS::outputln(const char* string) {
	#if OUTPUT == SERIAL
	Serial.print(string);
	#endif
}

void ARO_MicrOS::output(byte string) {
	#if OUTPUT == SERIAL
	Serial.print(string);
	#endif
}

void ARO_MicrOS::outputln(byte string) {
	#if OUTPUT == SERIAL
	Serial.print(string);
	#endif
}