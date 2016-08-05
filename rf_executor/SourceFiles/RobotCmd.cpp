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
	dangerNear = 5000;
	dangerCautious = 9000;
	dangerOver = 19000;
	commandTimeout = 15000;

	stateChanged = 0;
	currTimeout = 0;
	safeFeeling=10;
	freeTime =0;
	lastCommandSent = 0; 
}


void RobotCmd::setup(int buzzerPin) {
	  accel.init(20,        // int sampleSize,
				 -7,-7,8,   // int offsetX, int offsetY, int offsetZ,
				 0.5,0.13,0.03,  // newAttackTolerance, newCollisionTolerance, double newMovingTolerance,
				 2000);     // long newAccelShakeTimeout);

	  singer.setup(buzzerPin);
	  dancer.setup(&motor);
	  singer.stop();
	  dancer.stop();
  }
  
  
  void RobotCmd::update(long currTime,char *incomingMessage,char* outgoingMessage ,int outgoingMessageLength)
  {
	if(motor.motorState != STATE_COLLISION)
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
  
  
  if(accel.collision){
   motor.setState(STATE_COLLISION);
   accel.collision = false;
   return;
  }
  
 if( processDanger(currTime))
 {
	return;
 }
 
  
  if(motor.motorState == STATE_COLLISION)
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
      return;
    }
 }

  if((currTime - lastCommandSent) >commandTimeout)
  {
		Serial.println(" currently idle and not sent MARCO");
      currState = RBT_IDLE;
      lastCommandSent = currTime;
      freeTime = rand() *500+currTime;
	  freeTime = freeTime<0?-1*freeTime:freeTime;
  }

}

bool RobotCmd::processDanger(long currTime)
{
  bool processed = false;
  if(currState== RBT_DANGER )
  {
	  if(motor.motorSpeed != STATE_COLLISION)
	  {
		if((currTime - dangerTime) >dangerNear
		&& motor.motorState == STATE_BACKWARD){ //1st stage after running away
		  motor.Stop();
		  motor.setState(STATE_IDLE);
		}
		if((currTime - dangerTime) >dangerCautious
			&& motor.motorState == STATE_IDLE){ //2nd stage creep back slowly
		  motor.Stop();
		  motor.setState(STATE_FORWARD);
		}
		if((currTime - dangerTime) > dangerOver){ //3rd stage being playing
		  motor.setState(STATE_IDLE);
		  motor.Stop();
		  currState = RBT_IDLE;
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
      singer.stop();
      dancer.stop();
      motor.motorSpeed =FAST;
      motor.setState(STATE_BACKWARD);
      currState = RBT_DANGER;
      dangerTime = currTime;
	  safeFeeling = 0;
	  lastCommandSent = currTime;	  
      return true;
    }
  }
  return false;
}
