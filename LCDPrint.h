void printToLCD() {
  #if I2CLCDENABLED == 1
    I2CLCD lcd = I2CLCD(0x12, 20, 4);
  #endif
  unsigned int temp;
  unsigned int ph;
  unsigned int hour;
  unsigned int minute;
  unsigned int second;
  unsigned int day;
  unsigned int dayofweek;
  unsigned int month;
  unsigned int year;		
	
  #if DS1307ENABLED == 1
    if(EEPROM.read(i2clcd_time_display) == 1) {
      if(EEPROM.read(display_time_as) == 0) {
        lcd.setCursor(EEPROM.read(i2clcd_cursor_col), EEPROM.read(i2clcd_cursor_row));
        if(hour < 10) {
          lcd.write(" ");
          lcd.write(hour);
        } else {
          lcd.write(hour);
        }
        lcd.write(":");
        if(minute < 10){
          lcd.write("0");
          lcd.write(minute);
        } else {
          lcd.write(minute);
        }
        lcd.write(":");
        if(second < 10){
          lcd.write("0");
          lcd.write(second);
        } else {
          lcd.write(second);
        }
      } else if(EEPROM.read(display_time_as) == 1) {
        lcd.setCursor(EEPROM.read(i2clcd_cursor_col), EEPROM.read(i2clcd_cursor_row) - 2);
        if(hour < 10){
          lcd.write(" ");
          lcd.write(hour);
        } else if (hour >= 10 && hour <= 12) {
          lcd.write(hour);
        } else if(hour > 12) {
          hour = hour - 12;
          if(hour < 10) {
            lcd.write(" ");
          }
          lcd.write(hour);
        }
        lcd.write(":");
        if(minute < 10) {
          lcd.write("0");
          lcd.write(minute);
        } else {
          lcd.write(minute);
        }
        lcd.write(":");
        if(second < 10) {
          lcd.write("0");
          lcd.write(second);
        } else {
          lcd.write(second);
        }
        if(hour < 12) {
          lcd.write("AM");
        } else if(hour >= 12 && hour <= 23) {
          lcd.write("PM");
        }
      }
    }
  #endif
	
  #if DS18B20ENABLED == 1
    if(EEPROM.read(display_ds18b20_temp1) == 1) {
      lcd.setCursor(EEPROM.read(ds18b20_temp1_col), EEPROM.read(ds18b20_temp1_row));
      temp = getDS18B20Temp(EEPROM.read(ds18b20_temp1_sensor));
      lcd.write("T1=");
      lcd.write((temp / 100));
      lcd.write(".");
      if((temp % 100) < 10) {
        lcd.write("0");
      }    
      lcd.write((temp % 100));
      lcd.write(degree_symbol);
      #if CELSIUS == 1 
        lcd.write("C");
      #elseif
        lcd.write("F");
      #endif                      
    }
    
    if(EEPROM.read(display_ds18b20_temp2) == 1) {
      lcd.setCursor(EEPROM.read(ds18b20_temp2_col), EEPROM.read(ds18b20_temp2_row));
      temp = getDS18B20Temp(EEPROM.read(ds18b20_temp2_sensor));
      lcd.write("T2=");    
      lcd.write((temp / 100));
      lcd.write(".");
      if((temp % 100) < 10) {
        lcd.write("0");
      }    
      lcd.write((temp % 100));
      lcd.write(degree_symbol);
      #if CELSIUS == 1 
        lcd.write("C");
      #elseif
        lcd.write("F");
      #endif  
    }  

    if(EEPROM.read(display_ds18b20_temp3) == 1) {
      lcd.setCursor(EEPROM.read(ds18b20_temp3_col), EEPROM.read(ds18b20_temp3_row));
      temp = getDS18B20Temp(EEPROM.read(ds18b20_temp3_sensor));
      lcd.write("T3=");    
      lcd.write((temp / 100));
      lcd.write(".");
      if((temp % 100) < 10){
        lcd.write("0");
      }
      lcd.write((temp % 100)); 
      lcd.write(degree_symbol);
      #if CELSIUS == 1 
        lcd.write("C");
      #elseif
        lcd.write("F");
      #endif   
    }
  #endif
	
  #if PHENABLED
    if(EEPROM.read(display_ph) == 1) {
      lcd.setCursor(EEPROM.read(display_ph_col), EEPROM.read(display_ph_row));
      lcd.write("pH ");
      ph = getPHValue(PH_PIN) * 100;
      lcd.write(ph / 100);
      lcd.write(".");
      lcd.write(ph % 100);
    }
  #endif
}

