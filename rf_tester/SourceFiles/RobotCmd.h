#include "Dancer.h"
#include "Singer.h"
#include "ADXL345lib.h"
#include "RoboAccelerometer.h"


#ifndef MOTOR_H
#define MOTOR_H
#include "Motor.h"
#endif

//ROBOT STATES
#define RBT_IDLE 0
#define RBT_WAIT_POLO 1
#define RBT_WAIT_DANCE 2
#define RBT_DANCE 3
#define RBT_DANGER 6


class RobotCmd{
	public:
	RobotCmd();
	
	//MEssages
	static const char MARCO;
	static const char POLO;
	static const char DANGER;
	static const char DANCE;
	static const char SONG;
	static const char OK;
	
	
	void update(long currTime,char* incomingMessage, char* outGoingMessage,int outgoingMessageLength);
	void setup(int buzzerPin);
		
	//danger messages
	long dangerNear ; //the amount of time to spend running away
	long dangerCautious ;  //the amount of time to spend hiding
	long dangerOver ; //the amount of time before robot will sing and dance  after danger

	long commandTimeout; 	//amount of time to wait before we give up on hearing a response
	long currState = 0; 	//current status of robot
	long freeTime; // time we wait after finishing a dance before starting a new one
	long stateChanged; //the last time robot changed state
	long currTimeout; //the random timeout before we send Marco message out after danger
	int safeFeeling; //how safe the robot feels - determines the type of song it sings
	long lastCommandSent; //the time we sent the last command (like MARCO)
	long dangerTime;
	
	RoboAccelerometer accel;
	Singer singer; //pushes notes to piezo buzzer
	Dancer dancer; //changes motor state so the robot can dance
	Motor motor;

	void processMessage(long currTime,char* incomingMessage, char* outgoingMessage,int outgoingMessageLength);
	
	protected:
	char dir; //need to get rid of this once we figure out what to do when 
	

	bool detectNewDanger(char* incomingMessage,long currTime);
	bool processDanger(long currTime);
	
	
};


	
