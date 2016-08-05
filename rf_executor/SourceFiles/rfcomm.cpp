#include "VirtualWire.h"
#include "rfcomm.h"

RFcomm::RFcomm()
{
  sendTime = 0;
  sendTimeout =23000;
}


void RFcomm::setupRF(int receivePin, int transmitPin,int ledPin,int ledPin2 , int waitTime)
{
	  
	 //transmitPin
	vw_set_tx_pin(transmitPin);
	vw_set_ptt_inverted(true); // Requir
	vw_setup(2000);

    //receivePin
  vw_set_rx_pin(receivePin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);  // Bits per sec
  vw_rx_start();  



	sendState = RF_IDLE;
	sendTimeout = waitTime;

	pinMode(ledPin,OUTPUT);
	led  = ledPin;
	led2 = ledPin2;
 digitalWrite(led,LOW);
 digitalWrite(led2,LOW);
}

void RFcomm::update(long currTime , String sendMessage
,char* received,int maxLength)
{
	if( currMessage[0]!= sendMessage[0] 
	&& sendMessage!="")
	{
			sendTime = currTime;
			sendState = RF_TRANSMIT;
			currMessage = sendMessage.c_str();
	}
 
  transmit(currMessage);

    receive(received, maxLength);

}

void RFcomm::transmit(const char* sendMessage)
{
	if(strlen(sendMessage)>0)	
	{
    digitalWrite(led2,LOW);
    delay(500);
    digitalWrite(led2,HIGH);
		Serial.print("sending:");
	Serial.println(sendMessage);
		vw_send((uint8_t *)currMessage, strlen(currMessage));
		vw_wait_tx(); // Wait until the whole message is gone

	}

	
}

void RFcomm::receive(char* received,int maxLength)
{
	
		uint8_t buf[VW_MAX_MESSAGE_LEN];
		uint8_t buflen = VW_MAX_MESSAGE_LEN;
		memset (received,'\0',maxLength); //initialize to null

	if (vw_get_message(buf, &buflen)) // Non-blocking
    {
      	buflen = buflen>=maxLength?maxLength-1:buflen;
	  	for (int i = 0; i < buflen; i++)
  		{
				received [i]= char(buf[i]);
				Serial.print(char(buf[i]));
  		}
		received[buflen]= '\0';
			
		digitalWrite(led,HIGH);
		delay(500);
		digitalWrite(led,LOW);
		Serial.print("received: ");
		Serial.println(received);
	}
}
