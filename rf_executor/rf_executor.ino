#include "SourceFiles\VirtualWire.h"
#include "SourceFiles\RobotCmd.h"
#include "SourceFiles\rfcomm.h"

RobotCmd robotcmd;
RFcomm rfcomm;
char* lastSentMessage;
char* currMsg;
char* receiveMessage;
int receiveMessageLength = VW_MAX_MESSAGE_LEN;

void setup()
{
  Serial.begin(9600);
  rfcomm.setupRF(22, //11 receive
                 23, //12 transmit
                 31, //receive Light
                 30, //transmit light
                 5000);//timeout);
  robotcmd.setup(33); //buzzerPin
  robotcmd.currState = RBT_IDLE;
  robotcmd.commandTimeout = 5000; //set it so we don't wait so long

  digitalWrite(30, HIGH);
  delay(1000);
  digitalWrite(30, LOW);
  delay(1000);
  digitalWrite(31, HIGH);
  delay(1000);
  digitalWrite(31, LOW);
  robotcmd.motor.motorState = STATE_IDLE;

  receiveMessage = new char[receiveMessageLength];
  currMsg = new char[receiveMessageLength];
   lastSentMessage = new char[receiveMessageLength];
  memset(currMsg, '\0', receiveMessageLength);
  memset(lastSentMessage, '\0', receiveMessageLength);
}


void loop()
{
  long waitForAcknowledgment=0;
  long currTime = millis();
  long timeoutMessageSending= 10000; //resend a message for 3 seconds
                  //unless we get a new message to send over top of it
                  //need this for fault tolerance incase we miss the first sending of the message
  while ( (currTime + 1000) > millis())
  {
    
   if(strlen(lastSentMessage)>0)
  {
    Serial.print(" sending: ");
    Serial.println(lastSentMessage);
  }
    rfcomm.update(millis(), lastSentMessage, receiveMessage, receiveMessageLength);

    if (strlen(receiveMessage) > 0)
    {
     sprintf(currMsg, "%s", receiveMessage);
     waitForAcknowledgment = false;
      Serial.print("recevied: ");
      Serial.println(currMsg);
    }
    robotcmd.update(millis(), receiveMessage, currMsg, receiveMessageLength);
    if (strlen(currMsg) > 0)
    {
      strcpy(lastSentMessage, currMsg);
      waitForAcknowledgment = millis()+timeoutMessageSending;
    }
    if(millis()>waitForAcknowledgment)
    {
      //stop sending the message 
      memset(lastSentMessage,'\0',receiveMessageLength);
    }

  }

}

