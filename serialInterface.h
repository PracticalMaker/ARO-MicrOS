#ifndef SERIALINTERFACE
#define SERIALINTERFACE

void serialInterface() {
  char serialCommandString[128];
  char *serialReturnData;
  byte inByte;
  byte index = 0;
  
  while(Serial.available() > 0) {	
    inByte = Serial.read();
    serialCommandString[index] = inByte;
    #ifdef DEBUG_SERIAL_INTERFACE
      Serial.print("Received: ");
      Serial.println(serialCommandString[index]);
    #endif
    delay(1);
    index++;
    serialCommandString[index] = '\0';
  }

  serialReturnData = control(serialCommandString);
  Serial.println(serialReturnData);

  return;
}

#endif


