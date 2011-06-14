void serialInterface() {
  char serialCommandString[BUFFERSIZE];
  int serialReturnedData;
  byte inChar;
  byte index = 0;
  
  while(Serial.available() > 0) {	
    inChar = Serial.read();
    delay(1);
    if(inChar != '\n') {
      serialCommandString[index] = inChar;
      #if DEBUG == 1
        Serial.print("Received: ");
        Serial.println(serialCommandString[index]);
      #endif
      index++;
      serialCommandString[index] = '\0';
    } else { 
      serialReturnedData = control(serialCommandString);
  
      Serial.print("{ Value : ");        
      Serial.print(serialReturnedData);
      Serial.println(" }");
      
      Serial.flush();
    }
  }
}
