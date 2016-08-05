#include "SourceFiles\VirtualWire.h"
#include "SourceFiles\RobotCmd.h"
#include "SourceFiles\rfcomm.h"

RobotCmd robotcmd;
RFcomm rfcomm;
String currMessage;
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

  currMessage = "";
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
  memset(currMsg, '\0', receiveMessageLength);
}


void loop()
{
  bool waitForAcknowledgment;
  long currTime = millis();
  while ( (currTime + 1000) > millis())
  {
    rfcomm.update(millis(), currMsg, receiveMessage, receiveMessageLength);

    if (strlen(receiveMessage) > 0)
    {
     sprintf(currMsg, "%s", receiveMessage);
     waitForAcknowledgment = false;
      Serial.print("current message: ");
      Serial.println(currMsg);
    }
    robotcmd.update(millis(), receiveMessage, currMsg, receiveMessageLength);
    if (strlen(currMsg) > 0)
    {
      Serial.print("send:  ");
      Serial.println(currMsg);
      waitForAcknowledgment = true;
    }

  }

}

