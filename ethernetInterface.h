void ethernetWiznetW5100Interface() {
  Server server(80);
  char clientline[BUFFERSIZE];
  int index = 0;
  int ethernetReturnValue;

  // listen for incoming clients
  Client client = server.available();
  if (client) {
    //  reset input buffer
    index = 0;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //  fill url the buffer
        if(c != '\n' && c != '\r'){
          clientline[index] = c;
          index++;

          //  if we run out of buffer, overwrite the end
          if(index >= BUFFERSIZE)
            index = BUFFERSIZE -1;

          continue;
        }         

        /*
        //  convert clientline into a proper
         //  string for further processing
         String urlString = String(clientline);
         
         //  extract the operation
         //String op = urlString.substring(0,urlString.indexOf(' '));
         
         //  we're only interested in the first part...
         urlString = urlString.substring(urlString.indexOf('/'), urlString.indexOf(' ', urlString.indexOf('/')));
         
         //  put what's left of the URL back in client line
         urlString.toCharArray(clientline, BUFFERSIZE);
         
         //  get the first two parameters
         //char *commandLine = strtok(clientline,"/");
         */
        char *commandString;
        char *jsonpCallback;
        int num_spaces = 0;
        int counter = 0;

        commandString = strtok(clientline, " ");
        //commandString = strtok(commandString, "?");
        commandString = strtok(NULL, "=");     
        jsonpCallback = strtok(NULL, " ");

        String jsonpCallbackString = String(jsonpCallback);
        

        #if DEBUGETHERNETQUERYSTRING == 1
                Serial.println(commandString);
                Serial.println(jsonpCallbackString);
        #endif        
        
        delay(1);
          client.println("HTTP/1.1 200 OK");
          //client.println("Content-type: text/json");
          //client.println("Content-type: application/json");
          client.println("Content-Type: text/javascript");          
          client.println();

        ethernetReturnValue = control(commandString);

        client.print(jsonpCallbackString);
        client.print("({\"value\":");
        client.print(ethernetReturnValue);
        client.println("})");
       
        break;

      }
    }

    // close the connection:
    client.stop();
  }
}

