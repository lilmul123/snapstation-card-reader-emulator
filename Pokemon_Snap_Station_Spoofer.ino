/*
    Sketch designed to receive and send the responses to simulate the Pokemon Snap Station smart card reader
    Copyright (C) March 2015 Steve Muller - lilmul123@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */
 
 // internal variables
 char incoming[40];
 int numOfCredits = 1;
 int numOfBytesRead = 0;
 int resetLoop = 0;
 String incomingStr = "0";
 
void setup() {
  // initialize serial
  Serial.begin(9600);
  Serial.setTimeout(2000); // two seconds per timeout should allow resync within 10 seconds
}

void loop() {
        
  numOfBytesRead = Serial.readBytesUntil(0x03,incoming,40); // wait for 60011273 or 6002170772
  incoming[numOfBytesRead] = '\0';
  incomingStr = incoming;
  
  if(incomingStr == "60011776") // status is requested
    {
    // NOTE: The blank space after each string is ETX, *not* a space.
    Serial.write("60070006070000000066"); // card is inserted
    Serial.readBytesUntil(0x03,incoming,40); // wait for 60011273
    Serial.write("6007003B00000000005C"); // send ack response
    Serial.readBytesUntil(0x03,incoming,40); // wait for 60061300B0000206C1
    Serial.write("60090046010017A830900031"); // write card serial number
    Serial.readBytesUntil(0x03,incoming,40); // wait for 60061300B2050802C8
    
    // if a credit is available...
    if(numOfCredits)
      Serial.write("60050000019000F4"); // write "card has one credit" message
    // if a credit is unavailable...
    else
      Serial.write("60050000009000F5"); // write "card has no credits" message
      
    Serial.readBytesUntil(0x03,incoming,40); // wait for 60061300B2050802C8
    
    // if a credit is available...
    if(numOfCredits)
      Serial.write("60050000019000F4"); // write "card has one credit" message again
    // if a credit is unavailable...
    else
      Serial.write("60050000009000F5"); // write "card has no credits" message again
    
    // this is where we check to see if the Snap Station is trying to accept the credit
    numOfBytesRead = Serial.readBytesUntil(0x03,incoming,40); // wait for 6002170772 or 60081400D20508020000A1
    incoming[numOfBytesRead] = '\0';
    incomingStr = incoming;
    
    // "Snap Station wishes to accept credit" message received
    if(incomingStr == "60081400D20508020000A1")
    {
      Serial.write("6003009000F3"); // write "credit has been deposited" message
      numOfCredits = 0; // credit has been deposited, need to send "empty card inserted" string
      Serial.readBytesUntil(0x03,incoming,40); // wait for 6002170772
    }
    
    Serial.write("60010061"); // write final ack message
    
    // if the credit was deposited, send that it is empty ten times, and then set the number of credits back to 1
    if(!numOfCredits)
    {
       if(resetLoop++ >= 10) 
       {
        numOfCredits = 1; 
        resetLoop = 0;
       }
    }
      
    }
    
  else if(incomingStr == "6002170772")   // message end/heartbeat was received
  {
    Serial.write("60010061"); // write ack message
  }
 

}








