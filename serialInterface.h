void serialInterface() {
  char serialCommandString[BUFFERSIZE];
  int serialReturnedData;
  
  if(Serial.available() > 0){		
    unsigned int numSerialAvailable = Serial.available();
		
    for(unsigned int i = 0; i < numSerialAvailable; i++){
      serialCommandString[i] = Serial.read();
    }
    
    serialReturnedData = control(serialCommandString);
    Serial.print("{ Value : ");    
    Serial.print(serialReturnedData);
    Serial.println(" }");

  
    for(int i=0; i<=BUFFERSIZE; i++) {
      serialCommandString[i] = '\0'; 
    }    
    
    
  }
  Serial.flush();
}
