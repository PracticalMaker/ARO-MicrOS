/*
Andrew Oke - andrew@practicalmaker.com

V0.3 - Initial Release
V0.4 - 2011/06/07 - Added Support for Celsius
V0.4.1 2011/06/08 - Rewrote interface and wrote ethernet interface. Much easier to add new interfaces as all all you do is grab the command line and pass it to the control function

*/


#include <SPI.h>
#include <Client.h>
#include <Ethernet.h>
#include <Server.h>
#include <Udp.h>
#include <Wire.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <I2CLCD.h>
#include <DS1307.h>
#include <OneWire.h>


/*******************
 * 
 * Variables you can change
 * 
 *******************/
// NOTE THAT TURNING DEBUG ON WILL REQUIRE YOU TO TURN OFF ALL OTHER FUNCTIONS EXCEPT THE ONE YOUR DEBUGGING
// OTHERWISE WITHIN A FEW COMMANDS YOU'LL RUN OUT OF MEMORY
#define DEBUG 0

#define SPIINTERFACEON 1
#define SERIALINTERFACEON 1
#define ETHERNETWIZNETW5100INTERFACEON 0

#define KEYPADCONTROL 0
#define DS1307ENABLED 1
#define I2CLCDENABLED 1
#define ONEWIREENABLED 1
#define DS18B20ENABLED 1
#define DIGITALENABLED 1
#define ANALOGENABLED 1
#define PHENABLED 0
#define CELSIUS 0
#define BUFSIZE 35

#define ARDUINO_VOLTAGE 5.06
#define PH_PIN 2

unsigned int onewire_pin = 2;
unsigned int server_port_num = 80;
/*******************
 * 
 * end variables you can change
 * 
 ******************/

/********************
 * Variables you shouldn't change
 *********************/

#define PH_GAIN 9.6525
//  url buffer size
#define BUFSIZE 255

uint8_t degree_symbol = 0xDF;


/*
  MEMORY LOCATIONS
 */
 
unsigned int arduino_mem_addr = 0; 
unsigned int digital_pin_mem_start = 2;
unsigned int digital_pin_mem_end = 13;

unsigned int display_time_as = 16;
unsigned int i2clcd_time_display = 17;
unsigned int i2clcd_cursor_col = 18;
unsigned int i2clcd_cursor_row = 19;
unsigned int num_ds18b20_devices = 20;
unsigned int display_ds18b20_temp1 = 21;
unsigned int ds18b20_temp1_col = 22;
unsigned int ds18b20_temp1_row = 23;
unsigned int display_ds18b20_temp2 = 24;
unsigned int ds18b20_temp2_col = 25;
unsigned int ds18b20_temp2_row = 26;
unsigned int display_ds18b20_temp3 = 27;
unsigned int ds18b20_temp3_col = 28;
unsigned int ds18b20_temp3_row = 29;
unsigned int ds18b20_temp1_sensor = 30;
unsigned int ds18b20_temp2_sensor = 31;
unsigned int ds18b20_temp3_sensor = 32;
unsigned int display_ph = 33;
unsigned int display_ph_row = 34;
unsigned int display_ph_col = 35;

/* 
* macro memory locations
*/
unsigned int macros_memstart = 100;
unsigned int macros_memend = 400;
unsigned int macros_bytes = 10;
unsigned int num_macros = 30;
unsigned int onewire_addresses_memstart = 401;
unsigned int onewire_addresses_memend = 500;
unsigned int onewire_addresses_bytes = 8;


#if ETHERNETWIZNETW5100INTERFACEON == 1
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192,168,1,28 };
byte gateway[] = { 192,168,1,1};	
byte subnet[] = { 255, 255, 255, 0 };
#endif



/**************
 * Setup devices
 **************/
#if I2CLCDENABLED == 1
I2CLCD lcd = I2CLCD(0x12, 20, 4);
#endif

#if ONEWIREENABLED == 1
OneWire ds(onewire_pin);
#endif

#if ETHERNETWIZNETW5100INTERFACEON == 1
Server server(server_port_num);
#endif

void setup() {
#if SPION == 1
  SPI.begin();
#endif

#if ETHERNETWIZNETW5100INTERFACEON == 1
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();  
#endif

#if SERIALINTERFACEON == 1
  Serial.begin(9600);
#endif

#if I2CLCDENABLED == 1
  lcd.init();
  lcd.backlight(0);
  lcd.cursorOff();
  lcd.clear();
#endif

  /*
	set pinmodes from eeprom
   	*/
  for(unsigned int i = digital_pin_mem_start; i <= digital_pin_mem_end; i++){
    pinMode(i, EEPROM.read(i));
  }
}


void loop(){

	#if SERIALINTERFACEON == 1
		serialInterface();
	#endif
	
	#if DS1307ENABLED == 1
	  updateDS1307();
	#endif
	
	runMacros();
		
	#if I2CLCDENABLED == 1
	  printToLCD();
	#endif

	#if ETHERNETWIZNETW5100INTERFACEON == 1
		ethernetWiznetW5100Interface();
	#endif

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


/***********************************************************************
*
* PRINT TO LCD BLOCK
*
************************************************************************/
#if I2CLCDENABLED == 1
void printToLCD() {
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
#endif

/*********************************************
*
* END PRINT TO LCD BLOCK
*
*********************************************/

void serialInterface() {
	char serialCommandString[BUFSIZE];
	
	if(Serial.available() > 0){
		// reset char str
		for(unsigned int i = 0; i <= BUFSIZE; i++){
			serialCommandString[i] = '\0';
		}    
		
		unsigned int numSerialAvailable = Serial.available();
		
		for(unsigned int i = 0; i < numSerialAvailable; i++){
			serialCommandString[i] = Serial.read();
		}
	
		Serial.flush();
	}
	
	delay(50);
	
	control(serialCommandString);
  
}


#if ETHERNETWIZNETW5100INTERFACEON == 1
void ethernetWiznetW5100Interface() {
	char clientline[BUFSIZE];
	int index = 0;
	
	// listen for incoming clients
	Client client = server.available();
		
	if (client) {	
		//  reset input buffer
		index = 0;

		while (client.connected()) {
			if (client.available()) {
				char c = client.read();

				//  fill url the buffer
				if(c != '\n' && c != '\r') {
					clientline[index] = c;
					index++;

					//  if we run out of buffer, overwrite the end
					if(index >= BUFSIZE) {
						index = BUFSIZE -1;
					}
				
					continue;
				} 
			}

			//  convert clientline into a proper
			//  string for further processing
			String urlString = String(clientline);
			
			//  extract the operation
			String op = urlString.substring(0,urlString.indexOf(' '));
			
			//  we're only interested in the first part...
			urlString = urlString.substring(urlString.indexOf('/'), urlString.indexOf(' ', urlString.indexOf('/')));
			
			//  put what's left of the URL back in client line
			urlString.toCharArray(clientline, BUFSIZE);
			
			control(clientline);

			break;
		}
	
		// give the web browser time to receive the data
		delay(1);
	
		// close the connection:
		client.stop();
	}				
}
#endif

#if ETHERNETWIZNETW5100INTERFACEON == 1
void ethernetSendJSON(String key, String value) {
	String jsonOut = String();
	Client client = server.available();
	
	if(client) {
		//  return value
		client.println("HTTP/1.1 200 OK");
		client.println("Content-Type: text/html");
		client.println();
		
		client.print("{ ");
		client.print(key);
		client.print(" : ");
		client.print(value);
		client.println(" }");
		
	}
	delay(1);
	client.stop();
}
#endif

void serialSendJSON(String key, String value) {
	Serial.print("{ ");
	Serial.print(key);
	Serial.print(" : ");
	Serial.print(value);
	Serial.println(" }");
}

void control(char commandString[BUFSIZE]) {
	String key = String();
	String value = String();
	unsigned int macro_number;
	unsigned int macro_type;
	unsigned int watch_pin;
	unsigned int watch_state;
	unsigned int output_pin;
	unsigned int output_action;
	unsigned int output_time_on;
	unsigned int greater_less_equal;
	unsigned int hour_start;
	unsigned int minute_start;
	unsigned int hour_stop;
	unsigned int minute_stop;
	unsigned int sensor_num;	
unsigned int hour;
unsigned int minute;
unsigned int second;
unsigned int day;
unsigned int dayofweek;
unsigned int month;
unsigned int year;	
	
	char *commandToken = strtok(commandString, "/");

	if(strcmp(commandToken, "ARDUINOACK") == 0) {
		#if SERIALINTERFACEON == 1
			serialSendJSON("ARDUINOACK", (int)EEPROM.read(arduino_mem_addr));
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("ARDUINOACK", (int)EEPROM.read(arduino_mem_addr));
		#endif
	}	

	if(strcmp(commandToken, "ARDUINOADDR") == 0) {
		int address = atoi(strtok(NULL, "/"));
		EEPROM.write(arduino_mem_addr, address);

		#if SERIALINTERFACEON == 1
			serialSendJSON("ARDUINOADDR", address);
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("ARDUINOADDR", address);
		#endif		
	}
	
	if(strcmp(commandToken, "RESETMACROS") == 0) {
		resetMacros();
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("RESETMACROS", "1");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("RESETMACROS", "1");
		#endif		
	}	
	
	#if DIGITALENABLED == 1
	if(strcmp(commandToken, "PINMODE") == 0) {
		int pin = atoi(strtok(NULL, "/"));
		int mode = atoi(strtok(NULL, "/"));
		setPinMode(pin, mode);
	
		#if SERIALINTERFACEON == 1
			serialSendJSON(pin, mode);
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON(pin, mode);
		#endif		
	}
	#endif
	
	#if DIGITALENABLED == 1
	if(strcmp(commandToken, "SETPINSTATUS") == 0) {
		int pin = atoi(strtok(NULL, "/"));
		int pinstatus = atoi(strtok(NULL, "/"));
		setPinStatus(pin, pinstatus);

		#if SERIALINTERFACEON == 1
			serialSendJSON(pin, pinstatus);
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON(pin, pinstatus);
		#endif	
	}
	#endif

	#if DIGITALENABLED == 1
	if(strcmp(commandToken, "WDDOMACROSET") == 0) {
		macro_number = atoi(strtok(NULL, "/"));
		macro_type = atoi(strtok(NULL, "/"));
		watch_pin = atoi(strtok(NULL, "/"));
		watch_state = atoi(strtok(NULL, "/"));
		output_pin = atoi(strtok(NULL, "/"));
		output_action = atoi(strtok(NULL, "/"));
		output_time_on = atoi(strtok(NULL, "/"));
		watchDigitalPinDigitalPinOutputMacroSet(macro_number, macro_type, watch_pin, watch_state, output_pin, output_action, output_time_on);

		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif	
	}
	#endif
	
	#if ANALOGENABLED == 1
	if(strcmp(commandString, "WADOMACROSET") == 0){		
		macro_number = atoi(strtok(NULL, "/"));
		macro_type = atoi(strtok(NULL, "/"));
		watch_pin = atoi(strtok(NULL, "/"));
		watch_state = atoi(strtok(NULL, "/"));
		greater_less_equal = atoi(strtok(NULL, "/"));
		output_pin = atoi(strtok(NULL, "/"));
		output_action = atoi(strtok(NULL, "/"));
		output_time_on = atoi(strtok(NULL, "/"));
		watchAnalogPinDigitalPinOutputMacroSet(macro_number, macro_type, watch_pin, watch_state, output_pin, output_action, output_time_on, greater_less_equal);
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif		
	
	#if DIGITALENABLED == 1
	if(strcmp(commandString, "DPINVAL") == 0){
		watch_pin = atoi(strtok(NULL, "/"));
		
		#if SERIALINTERFACEON == 1
			serialSendJSON(watch_pin, digitalRead(watch_pin));
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON(watch_pin, digitalRead(watch_pin));
		#endif			
	}
	#endif	
	
	#if DIGITALENABLED == 1
	if(strcmp(commandString, "APINVAL") == 0){
		watch_pin = atoi(strtok(NULL, "/"));
		
		#if SERIALINTERFACEON == 1
			serialSendJSON(watch_pin, analogRead(watch_pin));
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON(watch_pin, analogRead(watch_pin));
		#endif			
	}
	#endif	
	
	#if DS1307ENABLED == 1
	if(strcmp(commandString, "DS1307SET") == 0){	
		hour = atoi(strtok(NULL, "/"));
		minute = atoi(strtok(NULL, "/"));
		second = atoi(strtok(NULL, "/"));
		day = atoi(strtok(NULL, "/"));
		dayofweek = atoi(strtok(NULL, "/"));
		month = atoi(strtok(NULL, "/"));
		year = atoi(strtok(NULL, "/"));
		
		DS1307SetTime(hour, minute, second, day, dayofweek, month, year);
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}	
	#endif
	
	#if DS1307ENABLED == 1 && SERIALINTERFACEON == 1
	if(strcmp(commandString, "DS1307TIME") == 0){
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
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif
	
	#if DS1307ENABLED == 1
	if(strcmp(commandString, "WDS1307DOMACROSET") == 0){	
		macro_number = atoi(strtok(NULL, "/"));
		macro_type = atoi(strtok(NULL, "/"));
		hour_start = atoi(strtok(NULL, "/"));
		minute_start = atoi(strtok(NULL, "/"));
		hour_stop = atoi(strtok(NULL, "/"));
		minute_stop = atoi(strtok(NULL, "/"));
		output_pin = atoi(strtok(NULL, "/"));
		output_action = atoi(strtok(NULL, "/"));
		watchDS1307DigitalOutputMacroSet(macro_number, macro_type, hour_start, minute_start, hour_stop, minute_stop, output_pin, output_action);

		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif
	}
	#endif

	#if I2CLCDENABLED == 1 && DS1307ENABLED == 1	
	if(strcmp(commandString, "TIMEDISPLAYAS") == 0){
		EEPROM.write(display_time_as, atoi(strtok(NULL, "/")));
		
		#if I2CLCDENABLED == 1
			lcd.clear();
		#endif
	}
	#endif
	
	#if ONEWIREENABLED == 1
	if(strcmp(commandString, "1WIREDEVICES") == 0){		
		discoverOneWireDevices();

		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif
	
	#if ONEWIREENABLED == 1	
	if(strcmp(commandString, "1WIREEEPROM") == 0){	
		unsigned int mem_addresses = (onewire_addresses_memend - onewire_addresses_memstart) / (num_ds18b20_devices * onewire_addresses_bytes);
		//TODO
		for(unsigned int i = 0; i < mem_addresses; i++){
			for(unsigned int j = 0; j < 8; j++){
				Serial.print(EEPROM.read((onewire_addresses_memstart + (i * onewire_addresses_bytes) + j)), HEX); 
				Serial.print(", ");
			}
			Serial.print("\r\n");
		} 
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif 
	
	#if DS18B20ENABLED == 1
	if(strcmp(commandString, "DS18B20TEMP") == 0){
		unsigned int temp_sensor_num = atoi(strtok(NULL, "/"));
		#if SERIALINTERFACEON == 1
			serialSendJSON(temp_sensor_num, getDS18B20Temp(temp_sensor_num));
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON(temp_sensor_num, getDS18B20Temp(temp_sensor_num));
		#endif			
	}
	#endif 
	
	#if I2CLCDENABLED == 1 && DS1307ENABLED == 1
	if(strcmp(commandString, "LCDTIMEDISP") == 0){
		EEPROM.write(i2clcd_time_display, atoi(strtok(NULL, "/")));
		
		#if I2CLCDENABLED == 1
			lcd.clear();
		#endif
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif
	
	#if I2CLCDENABLED == 1 && DS1307ENABLED == 1
	if(strcmp(commandString, "LCDTIMEPOS") == 0){
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
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif
	
	#if DS18B20ENABLED == 1
	if(strcmp(commandString, "TEMP1POS") == 0){
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
			
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif				
	}
	#endif

	#if DS18B20ENABLED == 1	
	if(strcmp(commandString, "TEMP1SENSOR") == 0){
		unsigned int sensor_num = atoi(strtok(NULL, "/"));
		
		#if DEBUG == 1 && SERIALINTERFACEON == 1
			Serial.print("Temp 1 Sensor: ");
			Serial.println(sensor_num);
		#endif
		EEPROM.write(ds18b20_temp1_sensor, sensor_num);

		#if I2CLCDENABLED == 1
			lcd.clear();
		#endif
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif		
	}
	#endif
		
	#if DS18B20ENABLED == 1
	if(strcmp(commandString, "TEMP1DISP") == 0){
		bool display_temp = atoi(strtok(NULL, "/"));
		
		#if DEBUG == 1 && SERIALINTERFACEON == 1
			Serial.print("Display TEMP1 ");
			Serial.println(display_temp);
		#endif
		EEPROM.write(display_ds18b20_temp1, display_temp);

		#if I2CLCDENABLED == 1
			lcd.clear();
		#endif
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif

	#if DS18B20ENABLED == 1
	if(strcmp(commandString, "TEMP2POS") == 0){
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
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif		
	}
	#endif

	#if DS18B20ENABLED == 1	
	if(strcmp(commandString, "TEMP2SENSOR") == 0){
		unsigned int sensor_num = atoi(strtok(NULL, "/"));
		
		#if DEBUG == 1 && SERIALINTERFACEON == 1
			Serial.print("Temp 2 Sensor: ");
			Serial.println(sensor_num);
		#endif
		EEPROM.write(ds18b20_temp2_sensor, sensor_num);

		#if I2CLCDENABLED == 1
			lcd.clear();
		#endif
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif
		
	#if DS18B20ENABLED == 1
	if(strcmp(commandString, "TEMP2DISP") == 0){
		unsigned int display = atoi(strtok(NULL, "/"));
		
		#if DEBUG == 1 && SERIALINTERFACEON == 1
			Serial.print("Display TEMP2 ");
			Serial.println(display);
		#endif
		EEPROM.write(display_ds18b20_temp2, display);

		#if I2CLCDENABLED == 1
			lcd.clear();
		#endif
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif					
	}
	#endif
	
	#if DS18B20ENABLED == 1
	if(strcmp(commandString, "TEMP3POS") == 0){
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
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif

	#if DS18B20ENABLED == 1	
	if(strcmp(commandString, "TEMP3SENSOR") == 0){
		unsigned int sensor_num = atoi(strtok(NULL, "/"));
		
		#if DEBUG == 1 && SERIALINTERFACEON == 1
			Serial.print("Temp 3 Sensor: ");
			Serial.println(sensor_num);
		#endif
		EEPROM.write(ds18b20_temp3_sensor, sensor_num);

		#if I2CLCDENABLED == 1
			lcd.clear();
		#endif
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif
		
	#if DS18B20ENABLED == 1
	if(strcmp(commandString, "TEMP3DISP") == 0){
		unsigned int display = atoi(strtok(NULL, "/"));
		
		#if DEBUG == 1 && SERIALINTERFACEON == 1
			Serial.print("Display TEMP3 ");
			Serial.println(display);
		#endif
		EEPROM.write(display_ds18b20_temp3, display);

		#if I2CLCDENABLED == 1
			lcd.clear();
		#endif
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif
	
	#if DS18B20ENABLED == 1
	if(strcmp(commandString, "DS18B20MACROSET") == 0){

		macro_number = atoi(strtok(NULL, "/"));
		macro_type = atoi(strtok(NULL, "/"));
		sensor_num = atoi(strtok(NULL, "/"));
		greater_less_equal = atoi(strtok(NULL, "/"));
		watch_state = atoi(strtok(NULL, "/"));
		output_pin = atoi(strtok(NULL, "/"));
		output_action = atoi(strtok(NULL, "/"));

		watchDS18B20DigitalOutputMacroSet(macro_number, macro_type, sensor_num, greater_less_equal, watch_state, output_pin, output_action);     
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	
	}
	#endif
	
	#if PHENABLED == 1
	if(strcmp(commandString, "PRINTPH") == 0){
		watch_pin = atoi(strtok(NULL, "/"));
		
                char phValue[5];
		#if SERIALINTERFACEON == 1
                        sprintf(phValue, "%f", getPHValue(watch_pin));
			serialSendJSON(watch_pin, phValue);
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
                        sprintf(phValue, "%f", getPHValue(watch_pin));
			ethernetSendJSON(watch_pin, phValue);
		#endif		
	}
	#endif

	#if PHENABLED == 1 && I2CLCDENABLED == 1	
	if(strcmp(commandString, "PHPOS") == 0){
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
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif
	
	#if PHENABLED == 1 && I2CLCDENABLED == 1
	if(strcmp(commandString, "PHDISP") == 0){
		unsigned int display = atoi(strtok(NULL, "/"));
		
		#if DEBUG == 1 && SERIALINTERFACEON == 1
			Serial.print("Display PH ");
			Serial.println(display);
		#endif
		EEPROM.write(display_ph, display);
		#if I2CLCDENABLED == 1
			lcd.clear();
		#endif 
		
		#if SERIALINTERFACEON == 1
			serialSendJSON("", "");
		#endif
		#if ETHERNETWIZNETW5100INTERFACEON == 1
			ethernetSendJSON("", "");
		#endif			
	}
	#endif		
	
	
	for(unsigned int i = 0; i <= BUFSIZE; i++) {
		commandString[i] = '\0';
	}
}



        
 
/***************************************************************
* 
* CONTROL FUNCTIONS
* 
* Control functions are functions that an interface can call (interfaces being serial, ethernet etc.)
* 
* A control function is a function that updates variables, writes to the eeprom, or changes the function of the code
* 
* Control functions are nested as follows
* 
* ie. If it is the main control function (as in the DS1307) than it is nested in the DS1307 control functions
* 
* however, if the interface is serial setting the RTC macros than it goes in the serial interface block
* 
* 
*****************************************************************/
#if DIGITALENABLED == 1         
void setPinMode(unsigned int pin, unsigned int mode){
	#if DEBUG == 1	&& SERIALINTERFACEON == 1
		Serial.print("Set Pin Mode Pin: ");
		Serial.println(pin);
		Serial.print("Mode: ");
		Serial.println(mode);
	#endif
	EEPROM.write(pin, mode);
	pinMode(pin, mode);
}
#endif

#if DIGITALENABLED == 1
void setPinStatus(unsigned int pin, unsigned int status) {
	#if DEBUG == 1	&& SERIALINTERFACEON == 1
		Serial.print("Set Pin Status Pin: ");
		Serial.println(pin);
		Serial.print("Status: ");
		Serial.println(status);
	#endif	

	if(status <= 1) {
		digitalWrite(pin, status);
	}else if(status > 1) {
		analogWrite(pin, status);
	}
}
#endif
    
#if DIGITALENABLED == 1    
void watchDigitalPinDigitalPinOutputMacroSet(unsigned int macro_number, unsigned int macro_type, unsigned int watch_pin, unsigned int watch_state, unsigned int output_pin, unsigned int output_action, unsigned int output_time_on){
	#if DEBUG == 1 && SERIALINTERFACEON == 1
		Serial.print("Macro Number: ");
		Serial.println(macro_number);
		Serial.print("Macro Type: ");
		Serial.println(macro_type);
		Serial.print("Watch Pin: ");
		Serial.println(watch_pin);
		Serial.print("Watch State: ");
		Serial.println(watch_state);
		Serial.print("Output Pin: ");
		Serial.println(output_pin);
		Serial.print("Output Action: ");
		Serial.println(output_action);
		Serial.print("Output Time On: ");
		Serial.println(output_time_on);
	#endif

	unsigned int memstart = macros_memstart + (macro_number * macros_bytes);
	unsigned int output_time_on_lb = lowValue(output_time_on);
	unsigned int output_time_on_hb = highValue(output_time_on);
	
	EEPROM.write(memstart, macro_type);
	EEPROM.write((memstart + 1), watch_pin);
	EEPROM.write((memstart + 2), watch_state);
	EEPROM.write((memstart + 3), output_pin);
	EEPROM.write((memstart + 4), output_action);  
	EEPROM.write((memstart + 5), output_time_on_lb);
	EEPROM.write((memstart + 6), output_time_on_hb);
}
#endif
               
void resetMacros(){
	for(unsigned int i = macros_memstart; i <= macros_memend; i++){
		EEPROM.write(i, 0);
		delay(5);
	}
	#if DEBUG == 1 && SERIALINTERFACEON == 1
		Serial.println("Reset Macros");
	#endif
}

#if DIGITALENABLED == 1
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
#endif

#if ANALOGENABLED == 1    
void watchAnalogPinDigitalPinOutputMacroSet(unsigned int macro_number, unsigned int macro_type, unsigned int watch_pin, unsigned int watch_state, unsigned int output_pin, unsigned int output_action, unsigned int output_time_on, unsigned int greater_less_equal){
	#if DEBUG == 1 && SERIALINTERFACEON == 1
		Serial.print("Macro Number: ");
		Serial.println(macro_number);
		Serial.print("Macro Type: ");
		Serial.println(macro_type);
		Serial.print("Watch Pin: ");
		Serial.println(watch_pin);
		Serial.print("Watch State: ");
		Serial.println(watch_state);
		Serial.print("<>=: ");
		Serial.println(greater_less_equal);		
		Serial.print("Output Pin: ");
		Serial.println(output_pin);
		Serial.print("Output Action: ");
		Serial.println(output_action);
		Serial.print("Output Time On: ");
		Serial.println(output_time_on);
	#endif

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
#endif

#if ANALOGENABLED == 1
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
#endif

#if DS1307ENABLED == 1
void DS1307SetTime(unsigned int hour, unsigned int minute, unsigned int second, unsigned int day, unsigned int dow, unsigned int month, unsigned int year){
	#if DEBUG == 1 && SERIALINTERFACEON == 1
		Serial.print("Hour: ");
		Serial.println(hour);
		Serial.print("Minute: ");
		Serial.println(minute);
		Serial.print("Second: ");
		Serial.println(second);
		Serial.print("Day: ");
		Serial.println(day);
		Serial.print("DOW: ");
		Serial.println(dow);
		Serial.print("Month: ");
		Serial.println(month);
		Serial.print("Year: ");
		Serial.println(year);
	#endif
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
#endif

#if DS1307ENABLED == 1
void updateDS1307() {
//TODO
unsigned int hour;
unsigned int minute;
unsigned int second;
unsigned int day;
unsigned int dayofweek;
unsigned int month;
unsigned int year;  
  
	hour = RTC.get(DS1307_HR,true);
	minute = RTC.get(DS1307_MIN,false);
	second = RTC.get(DS1307_SEC,false);
	dayofweek = RTC.get(DS1307_DOW,false);
	day = RTC.get(DS1307_DATE,false);
	month = RTC.get(DS1307_MTH,false);
	year = RTC.get(DS1307_YR,false);  
}
#endif

#if DS1307ENABLED == 1
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
#endif

#if DS1307ENABLED == 1
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
		}else{
			if(EEPROM.read(mem_address + 6) == 2 && RTC.get(DS1307_HR,false) == EEPROM.read(mem_address + 1)) {
				#if DEBUG == 1 && SERIALINTERFACEON == 1
					Serial.print("Fade In: ");
					Serial.println((minute * 4));
				#endif
				analogWrite(EEPROM.read(mem_address + 5), (RTC.get(DS1307_MIN,false) * 4));
			}else if(EEPROM.read(mem_address + 6) == 2 && (RTC.get(DS1307_HR,false) + 1) == EEPROM.read(mem_address + 3)) {
				#if DEBUG == 1 && SERIALINTERFACEON == 1
					Serial.print("Fade Out: ");
					Serial.println((255 - (minute * 4)));
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
#endif

#if PHENABLED
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
#endif

#if DS18B20ENABLED == 1
unsigned int getDS18B20Temp(int device_num) {
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
	if (SignBit) // negative
	{
		TReading = (TReading ^ 0xffff) + 1; // 2's comp
	}
	Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

	#if CELSIUS == 0
         	Tc_100 = c2f(Tc_100);
        #endif 
	
	return Tc_100;
}
#endif

#if ONEWIREENABLED == 1
void discoverOneWireDevices() {
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
#endif

#if DS18B20ENABLED == 1
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
#endif

#if DS18B20ENABLED == 1
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
#endif
/*********************
 * 
 * miscellaneous converting functions
 * 
 **********************/
#if CELSIUS == 0
float c2f(float cel) {
  return (cel * (9.0/5.0)) + (float)3200;
}
#endif

int highValue(int value) {
  return value / 256;
}

int lowValue(int value) {
  return value % 256; 
}

int combineValue(unsigned int lb, unsigned int hb) {
  return ((hb * 256) + lb);
}



