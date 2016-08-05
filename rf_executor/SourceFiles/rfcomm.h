

#define RF_IDLE 0
#define RF_TRANSMIT 1
#define RF_RECEIVE 2



class RFcomm{


public:
	RFcomm();
	
	void setupRF(int receivePin, int transmitPin,int ledPin,int ledPin2 , int waitTime);
	void transmit(const char* sendMessage);
	void receive(char* received,int maxLength);
   void update(long currTime , String send ,char* received,int maxLength);
	
	int sendState;
	long sendTime;
	long sendTimeout;

protected:

	const char* currMessage;
	int led;
	int led2;
};
