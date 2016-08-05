#include <Arduino.h>

#include "Dancer.h"


#ifndef MOTOR_H
#define MOTOR_H
#include "Motor.h"
#endif

	Dancer::Dancer()
	{
		
		currDanceIx = 0;
		lastPlay =0 ;
		dance = new char[2*danceLen* sizeof(char)];
	}
	
	Dancer:: ~Dancer()
	{
		delete dance;
	}
	
	void Dancer::setup(Motor *newMotor)
	{
		motor = newMotor;
	}
	
	String Dancer::createDance()
	{
    String newDance = "";
    int prevAction=1;
    int prevDuration =4;
    int actionIx =1;
    char cvtChar;
  	int duration=4;
	  int currIx =0;
	  for(int i=0;i<danceLen-2;i++)
	  {
      prevAction = actionIx;
      while(prevAction== actionIx){
    		actionIx = rand() % actionLen+1;
			actionIx = actionIx ==2?1:actionIx; //avoid the collision State
      }
       while(prevDuration== duration){
        duration = rand()%4+1;
      }
       
      newDance = newDance + actionIx  + duration;
      currIx +=2;
     
   //   Serial.println(newDance);
	  }
   dance = const_cast<char*>(newDance.c_str());
	  return newDance;
	}
	
	void Dancer::performDance(char* newDance)
	{
		if(strlen(newDance)<actionLen){
			stop();
			return;
		}
		memcpy(dance,newDance,2*danceLen* sizeof(char));
		currDanceIx =0;
		currPause =0;
		dancing = true;
	}
	
	void Dancer::stop()
	{
		motor->setState(STATE_IDLE);
		currDanceIx= danceLen*2 +1;
		currPause =0;
		dancing= false;
	}

	
void Dancer::update(long currTime)
	{
	  if(  (currTime - lastPlay)>currPause)
	  {
			if( (currDanceIx +1)<(danceLen*2))
			{
				char action= dance[currDanceIx];
				int newMotorState = action=='1'?STATE_FORWARD
                                    :action=='2'?STATE_COLLISION
                                    :action=='3'?STATE_TURN_RIGHT
                                    :action=='4'?STATE_TURN_LEFT
                                    :action=='5'?STATE_BACKWARD
                                    :STATE_IDLE;
				motor->setState(newMotorState);
				char dTime= dance[currDanceIx+1];
				currPause = dTime =='1'?1000
				            :dTime=='2'
				            ?2000
				            :dTime=='3'
				            ?3000
				            :dTime=='4'
				            ?4000
				            :2500;
				lastPlay = currTime;
				
				currDanceIx = currDanceIx+2;
			 }
			 else if (dancing)
			 {
				stop();
			 }
	  }
	  motor->update(currTime);
	}
	
