#ifdef client_h
void ethernetWiznetW5100Interface() {
  EthernetServer server(80);
  server.begin();
  
  char ethernetCommandString[BUFFERSIZE];
  int index = 0;
  char *ethernetReturnData;

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    //  reset input buffer
    index = 0;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //  fill url the buffer
        if(c != '\n' && c != '\r'){
          ethernetCommandString[index] = c;
          index++;

          //  if we run out of buffer, overwrite the end
          if(index >= BUFFERSIZE)
            index = BUFFERSIZE -1;
            continue;
        }         

        char *commandString;
        char *jsonpCallback;
        int num_spaces = 0;
        int counter = 0;

        commandString = strtok(ethernetCommandString, " ");
        commandString = strtok(NULL, "?");
        jsonpCallback = strtok(NULL, "=");
        jsonpCallback = strtok(NULL, "&");        


        String jsonpCallbackString = String(jsonpCallback);
        

        #ifdef DEBUGETHERNETQUERYSTRING
                Serial.println(ethernetCommandString);
                Serial.println(commandString);
                Serial.println(jsonpCallbackString);
        #endif        
        
        delay(1);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/javascript");          
          client.println();

        ethernetReturnData = control(commandString);
        
        #ifdef DEBUGETHERNETRETURNDATA 
          Serial.print(jsonpCallbackString);
          Serial.print("(");
          Serial.print(ethernetReturnData);
          Serial.println(")");          
        #endif

        client.print(jsonpCallbackString);
        client.print("(");
        client.print(ethernetReturnData);
        client.println(");");
       
        break;

      }
    }

    // close the connection:
    client.stop();
  }
}
#endif
