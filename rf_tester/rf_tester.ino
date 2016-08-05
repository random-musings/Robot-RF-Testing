#include "SourceFiles/VirtualWire.h"
#include "SourceFiles/rfcomm.h"
#include "SourceFiles/RobotCmd.h"


int testTimeout = 3000;
int testIx=0;
long testStart =0;
RFcomm rfcomm;

Singer singer;
Dancer dancer;

String currMessage;
String newMessage;
String expectedOutput;
String testPassed=" passed.";
String testFailed=" failed.";
char* receiveMessage;
int receiveMessageLength = VW_MAX_MESSAGE_LEN;


void setup()
{
  Serial.begin(9600);
  rfcomm.setupRF(11, //receive
      12, //transmit
      13, //receive Light
      8, //transmit light
      5000);//timeout);
      
      newMessage.reserve(200);
      newMessage="";
      expectedOutput.reserve(200);
      expectedOutput = "";
      currMessage.reserve(200);
      currMessage="";
      singer.setup(0);
      dancer.setup(NULL);
      testStart=0;
      
  receiveMessage = new char[receiveMessageLength];
}




String getTestMessage(int ix)
{
  Serial.print(F(" Test "));
  Serial.print(ix);
  String newSong;
  switch(ix)
  {
    case 0: 
      Serial.println(F(" Sending MARCO. Do we get POLO? "));
      return String(RobotCmd::MARCO);
    case 1:
    Serial.println(F(" Sending Sending Dance/Song. Do we get OK? "));
     newSong = singer.createSong(0,10);
     return String(RobotCmd::SONG+newSong);
    case 2:
    Serial.println(F("Wait for other robot to become idle. Do we get MARCO? "));
     return "";  
     case 3:
     Serial.println(F("Send back POLO. Do we get DANCE? "));
     return  String(RobotCmd::POLO);
     case 4:
    Serial.println(F("Send DANGER to other robot. Does it run? "));
     return String(RobotCmd::DANGER);       
  }
  return "";
}

String getTestAnswer(int ix)
{
  switch(ix)
  {
    case 0: return String(RobotCmd::POLO);
    case 1: return String(RobotCmd::OK);
    case 2: return String(RobotCmd::MARCO);
    case 3: return String(RobotCmd::SONG);  
    case 4: return String(RobotCmd::OK);  
  }
  return "";
}


void loop()
{

  bool passed = false;
  testStart=millis();
  rfcomm.sendState= RF_TRANSMIT;
   currMessage = getTestMessage(testIx);
   rfcomm.update(millis(),currMessage,receiveMessage, receiveMessageLength);
   expectedOutput =getTestAnswer(testIx);
   while( (testStart+testTimeout)> millis()&& !passed)
    {
        rfcomm.update(millis(),currMessage,receiveMessage, receiveMessageLength);
        if(strlen(receiveMessage)>0)
        {
          Serial.print("Expected ");
          Serial.print(expectedOutput);
          Serial.print(" Received:");
          Serial.println(receiveMessage);
         }
        passed = receiveMessage[0]==expectedOutput[0] || passed;
    }
   Serial.println( passed?testPassed:testFailed);
   testIx += passed?1:0;
   delay(100);
}

