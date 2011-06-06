/*

Uses I2c Wires interface

Uses Analog pin 4 - SDA
Uses Analog pin 5 - SCL


Usage:

see the examples folder of this library distribution.
*/


#include <stdio.h>
#include <string.h>
#include "Wire.h"
#include "I2CLCD.h"


//*******************************************************************
//
//  Constructor and Initialisation
//
//*******************************************************************


I2CLCD::I2CLCD(uint8_t adr, uint8_t brows, uint8_t columns)
{
  _adr = adr;
  _columns = columns;	// not yet used
  _rows = brows;		// not yet used
}

void I2CLCD::init()
{
  Wire.begin();
  backlight(0x00);
  clear();
}

void I2CLCD::command(uint8_t value) {
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(value);
  Wire.endTransmission();
  delay(1);
}

void I2CLCD::clear()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x01);  // clear display, set cursor position to zero
  Wire.endTransmission();
  delay(2);
}


void I2CLCD::write(const char* b)
{
	Wire.beginTransmission(_adr);
	while (*b)
	{
		if (*b == 0xfe || *b == 0xff) Wire.send(0xfe);
			Wire.send(*b++); // sends one byte
	}
	Wire.endTransmission(); // stop transmitting
	delay(1);
}

void I2CLCD::write(uint8_t value)
{
	Wire.beginTransmission(_adr);
	if (value == 0xfe || value == 0xff) Wire.send(0xfe);
		Wire.send(value); // sends one byte
	Wire.endTransmission(); // stop transmitting
	delay(1);
}

void I2CLCD::setCursor(int row, int col)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  command(0x80 | (col + row_offsets[row]));
}

void I2CLCD::backlight(uint8_t value)
{
  Wire.beginTransmission(_adr);
  Wire.send(0xff);
  Wire.send(0x01);
  Wire.send(value);
  Wire.endTransmission();
  delay(1);
}

void I2CLCD::cursorOff()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x0c);
  Wire.endTransmission();
  delay(1);

}

void I2CLCD::cursorOn()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x0e);
  Wire.endTransmission();
  delay(1);

}

void I2CLCD::displayOff()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x08);
  Wire.endTransmission();
  delay(1);
}

void I2CLCD::displayOn()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x0c);
  Wire.endTransmission();
  delay(1);
}

void I2CLCD::blinkOn()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x0f);
  Wire.endTransmission();
  delay(1);
}

void I2CLCD::blinkOff()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x0e);
  Wire.endTransmission();
  delay(1);
}


void I2CLCD::cursorRight()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x14);
  Wire.endTransmission();
  delay(1);
}

/////////////////
//TODO
/////////////////
void I2CLCD::cursorLeft()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x14);
  Wire.endTransmission();
  delay(1);
}

void I2CLCD::progCharMode()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x40);
  Wire.endTransmission();
  delay(1);
}

void I2CLCD::normalMode()
{
  Wire.beginTransmission(_adr);
  Wire.send(0xfe);
  Wire.send(0x80);
  Wire.endTransmission();
  delay(1);
}

uint8_t I2CLCD::readEEPROM(uint8_t addr)
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x02);
	Wire.send(addr);
	Wire.endTransmission();
	delay(1);
	Wire.requestFrom((int)_adr,1); // ask for the byte
	return Wire.receive(); // and return it
}

void I2CLCD::writeEEPROM(uint8_t addr, uint8_t value)
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x03);
	Wire.send(addr);
	Wire.send(value);
	Wire.endTransmission();
	delay(1);
}

void I2CLCD::zeroTermString(uint8_t addr)
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x04);
	Wire.send(addr);
	Wire.endTransmission();
	delay(1);
}

void I2CLCD::resetEEPROM()
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0xf0);
	Wire.endTransmission();
	delay(1);
}

void I2CLCD::reset()
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0xf1);
	Wire.endTransmission();
	delay(1);
}

/***************
** for some reason this doesn't return a value. Possible firmware bug
****************/
uint8_t I2CLCD::keysInBuffer()
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x10);
	Wire.endTransmission();
	delay(50);
	Wire.requestFrom((int)_adr,1); // ask for the byte
	delay(50);
	return Wire.receive(); // and return it
}

uint8_t I2CLCD::getKey()
{
	uint8_t key = 0x00;
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x11);
	Wire.endTransmission();
	//needs a higher delay otherwise it won't reliably return the key
	delay(40);
	Wire.requestFrom((int)_adr, 1); // ask for the byte
	if(Wire.available()) {
		key = Wire.receive(); // and return it
	}
	if(key != 0x00){
		return key;
	}else{
		return 0x20;
	}
}

/***************
** for some reason this doesn't return a value. Possible firmware bug
****************/
uint8_t I2CLCD::keyDown()
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x12);
	Wire.endTransmission();
	delay(1);
	Wire.requestFrom((int)_adr,1); // ask for the byte
	return Wire.receive(); // and return it
}

void I2CLCD::clearBuffer()
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x13);
	Wire.endTransmission();
	delay(1);
}

uint8_t I2CLCD::readKeysInBuffer(uint8_t num_keys)
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x14);
	Wire.send(num_keys);
	Wire.endTransmission();
	delay(1);
	Wire.requestFrom((int)_adr,1); // ask for the byte
	return Wire.receive(); // and return it
}

void I2CLCD::interruptOn()
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x16);
	Wire.endTransmission();
	delay(1);
}

void I2CLCD::interruptOff()
{
	Wire.beginTransmission(_adr);
	Wire.send(0xff);
	Wire.send(0x16);
	Wire.endTransmission();
	delay(1);
}