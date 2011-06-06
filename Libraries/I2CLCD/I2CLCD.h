/*
** Library for the ATTINY2313 I2C LCD/Keypad display driver developed by John Crouchley
*/

#ifndef I2CLCD_h
#define I2CLCD_h

#include <WProgram.h>
#include <inttypes.h>

#define BACKLIGHT_ON 0
#define BACKLIGHT_OFF 1
#define LCD_ON 1
#define LCD_OFF 0



class I2CLCD {
public:
  I2CLCD(uint8_t, uint8_t, uint8_t);
  void init();
  virtual void write(uint8_t value);
  virtual void write(const char* str);
  void command(uint8_t);
  void clear();
  void home();
  void setCursor(int, int);
  void backlight(uint8_t);
  void cursorOff();
  void cursorOn();
  void displayOff();
  void displayOn();
  void blinkOn();
  void blinkOff();
  void cursorLeft();
  void cursorRight();
  void progCharMode();
  void normalMode();
  uint8_t readEEPROM(uint8_t);
  void writeEEPROM(uint8_t, uint8_t);
  void zeroTermString(uint8_t);
  void resetEEPROM();
  void reset();
  uint8_t keysInBuffer();
  uint8_t getKey();
  uint8_t keyDown();
  void clearBuffer();
  uint8_t readKeysInBuffer(uint8_t);
  void interruptOn();
  void interruptOff();

private:
  uint8_t _adr;
  uint8_t _columns;
  uint8_t _rows; 

};

#endif
