#include "Arduino.h"
#include "RobotCmd.h"

//messages that can be sent/processed
	const char RobotCmd::MARCO ='m';
	const char RobotCmd::POLO ='p';
	const char RobotCmd::DANGER ='d';
	const char RobotCmd::DANCE ='_';
	const char RobotCmd::SONG ='&';
	const char RobotCmd::OK ='o';
	
	
// *************************
// STRING PARSING FUNCTIONS
// *************************

  char *  strwrd(char *s, char *buf, size_t len, const char *delim)
  {
    s += strspn(s, delim);
    int n = strcspn(s, delim); 
    if (len-1 < n)           
      n = len-1;
    memcpy(buf, s, n);
    buf[n] = 0;
    s += n;
    return (*s == 0) ? NULL : s;
  }
  

RobotCmd::RobotCmd(){
	dangerNear = 3000;
	dangerCautious = 7000;
	dangerOver = 10000;
	commandTimeout = 15000;

	stateChanged = 0;
	currTimeout = 0;
	safeFeeling=10;
	freeTime =0;
	lastCommandSent = 0; 
}


void RobotCmd::setup(int buzzerPin) {
	//start motors so we get accurate base line
	motor.Forward(255);
	delay(100);
	  accel.init(200,        // int sampleSize,
				 -7,-7,8,   // int offsetX, int offsetY, int offsetZ,
				 0.5,0.25,0.03,  // newAttackTolerance, newCollisionTolerance, double newMovingTolerance,
				 2000);     // long newAccelShakeTimeout);
	motor.Stop();
	  singer.setup(buzzerPin);
	  dancer.setup(&motor);
	  singer.stop();
	  dancer.stop();
  }
  
  
  void RobotCmd::update(long currTime,char *incomingMessage,char* outgoingMessage ,int outgoingMessageLength)
  {
	accel.update(currTime);
	if(motor.motorState != motor.enumDir::COLLISION)
	{
		singer.update(currTime); 
		dancer.update(currTime);
	}
	motor.update(currTime);
	processMessage(currTime,incomingMessage,outgoingMessage,outgoingMessageLength); 
  }
  
  
  
void RobotCmd::processMessage(long currTime, char* incomingMessage,  char* outgoingMessage,int outgoingMessageLength)
{

	char* msg = incomingMessage;
	memset(outgoingMessage,'\0',outgoingMessageLength);
  
	if(detectNewDanger(msg,currTime)){
		outgoingMessage[0]= incomingMessage[0]== RobotCmd::DANGER 
			? RobotCmd::OK
			:RobotCmd::DANGER;
		return;
	}
  
  
  if(accel.collision ){
	//check to see if we actually hit something or accelerometer is settling
	double currentReading =  abs(accel.aY) + abs(accel.aZ);
	if(currentReading> collisionReading)
	{
		collisionReading = currentReading +1;
		Serial.println("COLLISION ");
	   motor.setState(motor.enumDir::COLLISION);
	   accel.collision = false;
   }
   return;
  }
  
 if( processDanger(currTime))
 {
	return;
 }
 
  
  if(motor.motorState == motor.enumDir::COLLISION)
  {
		//when encountering a collision get us out of a jam before resuming all activities
		return;
  }

  
  if(currState != RBT_DANCE && msg[0]==MARCO) //received marco
  {
	lastCommandSent = millis();
    Serial.println("process MARCO waiting for DANCE");
    currState = RBT_WAIT_DANCE;
	outgoingMessage[0]= POLO;
    return; //send back polo;
  }
  
  if(currState == RBT_WAIT_POLO && msg[0]==POLO) //recived polo
  {
	lastCommandSent = millis();
    Serial.println("process POLO");
    currState = RBT_DANCE;
    String newSong = singer.createSong(safeFeeling,safeFeeling+10);
	singer.playSong(const_cast<char*>(newSong.c_str()));
    String newDance = "";//dancer.createDance(); //disbaled dance cause we can't hear RF signals with it
	sprintf ( outgoingMessage,"%c%s",SONG,newSong.c_str());
    return;
  }
  
 
  if(currState == RBT_WAIT_DANCE && msg[0]==SONG)
  {
   Serial.println("process SONG");
	lastCommandSent =  millis();
	singer.playSong(const_cast<char*>(msg+1));
    currState = RBT_DANCE;
	outgoingMessage[0]= OK;
    return;
  }

  //we just finished dancing
  if(currState == RBT_DANCE)
  {
    Serial.println(" dancing/singing SONG");
	if( !dancer.dancing && !singer.singing)
	{
		currState = RBT_IDLE;
		freeTime = rand() *1000;//set the time before we connect to other robot
		freeTime = freeTime<2000?2000:freeTime;
		 
	}
	return;
  }

 if(currState == RBT_IDLE)
 {
    if((currTime - freeTime )>  commandTimeout)
    { //see if robot is responding
      currState = RBT_WAIT_POLO;
      lastCommandSent = currTime;
	  //determines the song we sing
	  safeFeeling = safeFeeling< (singer.maxSafeFeeling-10)?safeFeeling+10:safeFeeling;
	  outgoingMessage[0] = MARCO;
	  collisionReading = accel.accel_center_z+ accel.accel_center_y;
      return;
    }
 }

  if((currTime - lastCommandSent) >commandTimeout)
  {
		Serial.print(" returning to IDLE status");
      currState = RBT_IDLE;
      lastCommandSent = millis();
      freeTime = rand() *500+currTime;
	  freeTime = freeTime<0?-1*freeTime:freeTime;
	   collisionReading =accel.accel_center_z+ accel.accel_center_y;
	   
  }

}

bool RobotCmd::processDanger(long currTime)
{
  bool processed = false;
  
  if(currState== RBT_DANGER )
  {
	
	if(motor.motorState != motor.enumDir::COLLISION)
	{
		if((currTime - dangerTime) >dangerNear
		&& motor.motorState == motor.enumDir::BACKWARD){ //1st stage after running away
		  motor.Stop();
		  motor.setState(motor.enumDir::IDLE);
		}
		if((currTime - dangerTime) >dangerCautious
			&& motor.motorState == motor.enumDir::IDLE){ //2nd stage creep back slowly
		  motor.Stop();
		  motor.setState(motor.enumDir::FORWARD);
		  collisionReading =accel.accel_center_x+ accel.accel_center_y;
		}
		if((currTime - dangerTime) > dangerOver){ //3rd stage being playing
		  motor.setState(motor.enumDir::IDLE);
		  motor.Stop();
		  currState = RBT_IDLE;
		  collisionReading = accel.accel_center_x+ accel.accel_center_y;
		  //reset the wait for Marco - Polo to some random interval
		}
	}
    processed = true;
  }
  return processed;
}



bool RobotCmd::detectNewDanger(char* incomingMessage,long currTime)
{
  if(currState!= RBT_DANGER )
  {
    if(incomingMessage[0]== RobotCmd::DANGER || accel.attacked)
    {
		Serial.println("DANGER   ");
      singer.stop();
      dancer.stop();
      motor.currSpeed =motor.enumSpeed::FAST;
      motor.setState(motor.enumDir::BACKWARD);
      currState = RBT_DANGER;
      dangerTime = currTime;
	  safeFeeling = 0;
	  lastCommandSent = currTime;	  
	  //set this so that we do not have Flase collision readings when accelerometer settles
	  collisionReading =  abs(accel.aY) + abs(accel.aZ) +0.2 ;
      return true;
    }
  }
  return false;
}
