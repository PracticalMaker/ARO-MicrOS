int control(char commandString[BUFFERSIZE]) {	

  #if I2CLCDENABLED == 1
    I2CLCD lcd = I2CLCD(0x12, 20, 4);
  #endif
  
  int commandToken = atoi(strtok(commandString, "/"));
  
  #if DEBUG == 1
    Serial.print("Token: ");
    Serial.println(commandToken);
  #endif
  
  #if DEBUG == 1
    Serial.print("Command: ");
    Serial.println(commandToken);
  #endif
  
  if(commandToken == 255) {
    return EEPROM.read(ARDUINO_MEM_ADDR);
  }	

  if(commandToken == 1) {
    int address = atoi(strtok(NULL, "/"));
    EEPROM.write(ARDUINO_MEM_ADDR, address);
    
    return address;
  }
	
  if(commandToken == 2) {
    resetMacros();
    
    return 1;
  }	
	
  #if DIGITALENABLED == 1
    if(commandToken == 3) {
      int pin = atoi(strtok(NULL, "/"));
      int mode = atoi(strtok(NULL, "/"));
      setPinMode(pin, mode);	
      
      return 1;
    }
  #endif
	
  #if DIGITALENABLED == 1
    if(commandToken == 4) {
      int pin = atoi(strtok(NULL, "/"));
      int pinstatus = atoi(strtok(NULL, "/"));
      setPinStatus(pin, pinstatus);	
      
      return 1;
    }
  #endif

  #if DIGITALENABLED == 1
    if(commandToken == 5) {
      byte macro_number = atoi(strtok(NULL, "/"));
      byte macro_type = atoi(strtok(NULL, "/"));
      byte watch_pin = atoi(strtok(NULL, "/"));
      byte watch_state = atoi(strtok(NULL, "/"));
      byte output_pin = atoi(strtok(NULL, "/"));
      byte output_action = atoi(strtok(NULL, "/"));
      byte output_time_on = atoi(strtok(NULL, "/"));
      watchDigitalPinDigitalPinOutputMacroSet(macro_number, macro_type, watch_pin, watch_state, output_pin, output_action, output_time_on);
    }
  #endif
	
  #if ANALOGENABLED == 1
    if(commandToken == 6){		
      int macro_number = atoi(strtok(NULL, "/"));
      int macro_type = atoi(strtok(NULL, "/"));
      int watch_pin = atoi(strtok(NULL, "/"));
      int watch_state = atoi(strtok(NULL, "/"));
      int greater_less_equal = atoi(strtok(NULL, "/"));
      int output_pin = atoi(strtok(NULL, "/"));
      int output_action = atoi(strtok(NULL, "/"));
      int output_time_on = atoi(strtok(NULL, "/"));
      watchAnalogPinDigitalPinOutputMacroSet(macro_number, macro_type, watch_pin, watch_state, output_pin, output_action, output_time_on, greater_less_equal);
		
      return 1;
    }
  #endif		
	
  #if DIGITALENABLED == 1
    if(commandToken == 7){
      int watch_pin = atoi(strtok(NULL, "/"));

      return digitalRead(watch_pin);
    }
  #endif		
	
	
  #if ANALOGENABLED == 1
    if(commandToken == 8){
      byte watch_pin = atoi(strtok(NULL, "/"));
		
      return analogRead(watch_pin);
    }
  #endif	
	
  #if DS1307ENABLED == 1
    if(commandToken == 9){	
      byte hour = atoi(strtok(NULL, "/"));
      byte minute = atoi(strtok(NULL, "/"));
      byte second = atoi(strtok(NULL, "/"));
      byte day = atoi(strtok(NULL, "/"));
      byte dayofweek = atoi(strtok(NULL, "/"));
      byte month = atoi(strtok(NULL, "/"));
      byte year = atoi(strtok(NULL, "/"));
		
      DS1307SetTime(hour, minute, second, day, dayofweek, month, year);
		
      return 1;
    }	
  #endif
	
  #if DS1307ENABLED == 1
    if(commandToken == 10){
      //TODO
      Serial.print(RTC.get(DS1307_HR,true));
      Serial.print(":");
      Serial.print(RTC.get(DS1307_MIN,false));
      Serial.print(":");
      Serial.print(RTC.get(DS1307_SEC,false));
      Serial.print("      ");                 
      Serial.print(RTC.get(DS1307_DATE,false));
      Serial.print("/");
      Serial.print(RTC.get(DS1307_MTH,false));
      Serial.print("/");
      Serial.println(RTC.get(DS1307_YR,false));
		
      return 1;
    }
  #endif
	
  #if DS1307ENABLED == 1
    if(commandToken == 11){	
      byte macro_number = atoi(strtok(NULL, "/"));
      byte macro_type = atoi(strtok(NULL, "/"));
      byte hour_start = atoi(strtok(NULL, "/"));
      byte minute_start = atoi(strtok(NULL, "/"));
      byte hour_stop = atoi(strtok(NULL, "/"));
      byte minute_stop = atoi(strtok(NULL, "/"));
      byte output_pin = atoi(strtok(NULL, "/"));
      unsigned int output_action = atoi(strtok(NULL, "/"));
      watchDS1307DigitalOutputMacroSet(macro_number, macro_type, hour_start, minute_start, hour_stop, minute_stop, output_pin, output_action);

      return 1;
    }
  #endif

  #if I2CLCDENABLED == 1 && DS1307ENABLED == 1	
    if(commandToken == 12){
      EEPROM.write(display_time_as, atoi(strtok(NULL, "/")));
		
      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif
    }
  #endif
	
  #if ONEWIREENABLED == 1
    if(commandToken == 13){		
      discoverOneWireDevices();
      
      return 1;
    }
  #endif
	
  #if ONEWIREENABLED == 1	
    if(commandToken == 14){	
      unsigned int mem_addresses = (onewire_addresses_memend - onewire_addresses_memstart) / (num_ds18b20_devices * onewire_addresses_bytes);
      //TODO
      for(unsigned int i = 0; i < mem_addresses; i++){
        for(unsigned int j = 0; j < 8; j++){
          Serial.print(EEPROM.read((onewire_addresses_memstart + (i * onewire_addresses_bytes) + j)), HEX); 
          Serial.print(", ");
        }
        Serial.print("\r\n");
      } 
		
		
    }
  #endif 
	
  #if DS18B20ENABLED == 1
    if(commandToken == 15){
      unsigned int temp_sensor_num = atoi(strtok(NULL, "/"));
      
      return getDS18B20Temp(temp_sensor_num);
      		
    }
  #endif 
	
  #if I2CLCDENABLED == 1 && DS1307ENABLED == 1
    if(commandToken == 16){
      EEPROM.write(i2clcd_time_display, atoi(strtok(NULL, "/")));
		
      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif

      return 1;		
    }
  #endif
	
  #if I2CLCDENABLED == 1 && DS1307ENABLED == 1
    if(commandToken == 17){
      unsigned int row = atoi(strtok(NULL, "/"));
      unsigned int col = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Column ");
        Serial.println(col);
      #endif

      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Row ");
        Serial.println(row);
      #endif

      EEPROM.write(i2clcd_cursor_col, col);
      EEPROM.write(i2clcd_cursor_row, row);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif
		
      return 1;
    }
  #endif
	
  #if DS18B20ENABLED == 1
    if(commandToken == 18){
      unsigned int row = atoi(strtok(NULL, "/"));
      unsigned int col = atoi(strtok(NULL, "/"));		
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Column ");
        Serial.println(col);
      #endif

      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Row ");
        Serial.println(row);
      #endif
      EEPROM.write(ds18b20_temp1_col, col);
      EEPROM.write(ds18b20_temp1_row, row);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif
			
      return 1;
    }
  #endif

  #if DS18B20ENABLED == 1	
    if(commandToken == 19){
      unsigned int sensor_num = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Temp 1 Sensor: ");
        Serial.println(sensor_num);
      #endif
      EEPROM.write(ds18b20_temp1_sensor, sensor_num);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif
		
      return 1;
    }
  #endif
		
  #if DS18B20ENABLED == 1
    if(commandToken == 20){
      bool display_temp = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Display TEMP1 ");
        Serial.println(display_temp);
      #endif
      EEPROM.write(display_ds18b20_temp1, display_temp);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif
	
      return 1;
    }
  #endif

  #if DS18B20ENABLED == 1
    if(commandToken == 21){
      unsigned int row = atoi(strtok(NULL, "/"));
      unsigned int col = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Column ");
        Serial.println(col);
      #endif

      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Row ");
        Serial.println(row);
      #endif
      EEPROM.write(ds18b20_temp2_col, col);
      EEPROM.write(ds18b20_temp2_row, row);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif
		
      return 1;
    }
  #endif

  #if DS18B20ENABLED == 1	
    if(commandToken == 22){
      unsigned int sensor_num = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Temp 2 Sensor: ");
        Serial.println(sensor_num);
      #endif
      EEPROM.write(ds18b20_temp2_sensor, sensor_num);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif
		
      return 1;
    }
  #endif
		
  #if DS18B20ENABLED == 1
    if(commandToken == 23){
      unsigned int display = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Display TEMP2 ");
        Serial.println(display);
      #endif
      EEPROM.write(display_ds18b20_temp2, display);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif

      return 1;
    }
  #endif
	
  #if DS18B20ENABLED == 1
    if(commandToken == 24){
      unsigned int col = atoi(strtok(NULL, "/"));
      unsigned int row = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Column ");
        Serial.println(col);
      #endif

      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Row ");
        Serial.println(row);
      #endif
      EEPROM.write(ds18b20_temp3_col, col);
      EEPROM.write(ds18b20_temp3_row, row);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif

      return 1;
    }
  #endif

  #if DS18B20ENABLED == 1	
    if(commandToken == 25){
      unsigned int sensor_num = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Temp 3 Sensor: ");
        Serial.println(sensor_num);
      #endif
      EEPROM.write(ds18b20_temp3_sensor, sensor_num);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif
	
      return 1;
    }
  #endif
		
  #if DS18B20ENABLED == 1
    if(commandToken == 26){
      unsigned int display = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Display TEMP3 ");
        Serial.println(display);
      #endif
      EEPROM.write(display_ds18b20_temp3, display);

      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif

      return 1;
    }
  #endif
	
  #if DS18B20ENABLED == 1
    if(commandToken == 27){
      int macro_number = atoi(strtok(NULL, "/"));
      int macro_type = atoi(strtok(NULL, "/"));
      int sensor_num = atoi(strtok(NULL, "/"));
      int greater_less_equal = atoi(strtok(NULL, "/"));
      int watch_state = atoi(strtok(NULL, "/"));
      int output_pin = atoi(strtok(NULL, "/"));
      int output_action = atoi(strtok(NULL, "/"));

      watchDS18B20DigitalOutputMacroSet(macro_number, macro_type, sensor_num, greater_less_equal, watch_state, output_pin, output_action);     

      return 1;		
    }
  #endif
	
  #if PHENABLED == 1
    if(commandToken == 28){
      int watch_pin = atoi(strtok(NULL, "/"));

      return (getPHValue(watch_pin) * 100);
    }
  #endif

  #if PHENABLED == 1 && I2CLCDENABLED == 1	
    if(commandToken == 29){
      unsigned int row = atoi(strtok(NULL, "/"));
      unsigned int col = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Column ");
        Serial.println(col);
      #endif

      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Row ");
        Serial.println(row);
      #endif
      EEPROM.write(display_ph_col, col);
      EEPROM.write(display_ph_row, row);
		
      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif     
		
      return 1;
    }
  #endif
	
  #if PHENABLED == 1 && I2CLCDENABLED == 1
    if(commandToken == 30){
      unsigned int display = atoi(strtok(NULL, "/"));
		
      #if DEBUG == 1 && SERIALINTERFACEON == 1
        Serial.print("Display PH ");
        Serial.println(display);
      #endif
      EEPROM.write(display_ph, display);
      #if I2CLCDENABLED == 1
        lcd.clear();
      #endif 
		
      return 1;
    }
  #endif

	//free(commandString);
	
  for(unsigned int i = 0; i <= BUFFERSIZE; i++) {
    commandString[i] = '\0';
  }
}
