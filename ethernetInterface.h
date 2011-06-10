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
        char commandString[BUFFERSIZE];
        int num_spaces = 0;
        int counter = 0;
        
        for(int i=0; i<=BUFFERSIZE; i++) { 
          if(num_spaces == 1) {
            commandString[counter] = clientline[i];
            counter++;
          }  
          if(clientline[i] == 0x20) {
            num_spaces++;
          }     
        }     
        
        #if DEBUGETHERNETQUERYSTRING == 1
          Serial.println(commandString);
        #endif        
        
        ethernetReturnValue = control(commandString);

        delay(1);

        if(ethernetReturnValue){
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.print("{ Value : ");
          client.print(ethernetReturnValue);          
          client.println(" }");
        } else {
          //  error
          Serial.println("erroring");
          client.println("HTTP/1.1 404 Not Found");
          client.println("Content-Type: text/html");
          client.println();
          
        }
        break;
      }
    }
    
    // close the connection:
    client.stop();
  }
}
