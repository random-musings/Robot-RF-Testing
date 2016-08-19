#include <Arduino.h>

#include "RMotor_Cmd.h"


RMotor_Cmd::RMotor_Cmd()
{
	currOp = (int) enumDir::IDLE;
	currSpeed = 0;
	motorState =  (int)enumDir::IDLE;
	motor1Dir = true;
	motor2Dir = true;
	collisionAvoidanceTime = 2000;
}
	
	
void RMotor_Cmd::ReleaseMotors(){
 
  analogWrite(EN1_PIN, 0);      // Motor 1 off
  analogWrite(EN2_PIN, 0);      // Motor 1 off
  delay(20);
}

void RMotor_Cmd::Motor(int direction,bool forward1 ,bool forward2 ,int speed)
{
	if(direction!= currOp || motorState == enumDir::IDLE )
	{
		ReleaseMotors();
		currOp = direction;
		return;
	}
	
	currSpeed = speed;
	motor1Dir = forward1;
	motor2Dir = forward2;
	digitalWrite(DIR2_PIN, forward1?HIGH:LOW);  // Set Motor 2 forward direction
	analogWrite(EN2_PIN, 255);    // Motor 2 on in forward direction
   digitalWrite(DIR1_PIN,  forward2?HIGH:LOW);  // Set Motor 1 forward direction
   analogWrite(EN1_PIN, 255);    // Motor 1 on in forward direction
}

void RMotor_Cmd::Forward(int speed){
	Motor(enumDir::FORWARD,true,true,speed);
}


void RMotor_Cmd::Backward(int speed){
	Motor(enumDir::BACKWARD,false,false,speed);
}


void RMotor_Cmd::Left(int speed){
	Motor(enumDir::LEFT,true,false,speed);
}


void RMotor_Cmd::Right(int speed){
	Motor(enumDir::RIGHT,false,true,speed);
}

void RMotor_Cmd::Stop(){
	ReleaseMotors();
}


void RMotor_Cmd::update(long currTime)
{

	if(motorState == enumDir::COLLISION){
		motorState = ( currTime - state_last_change )> collisionAvoidanceTime
			? enumDir::IDLE //auto set state to forward after 3 seconds
			: enumDir::COLLISION;
		
		motorState == enumDir::COLLISION?Right(currSpeed):Forward(currSpeed);
		return;
	}
	
	Motor(currOp,motor1Dir,motor2Dir,currSpeed);
}


void RMotor_Cmd::setState(int newState)
{
	if(newState!= motorState)
	{
		ReleaseMotors();
	}
	motor1Dir =  newState == enumDir::FORWARD || newState ==enumDir::LEFT;
	motor2Dir = newState == enumDir::FORWARD || newState ==enumDir::RIGHT;
	Serial.print("Setting motor State:");
	Serial.print(newState);
	Serial.print(" ");
	Serial.print(motor1Dir);
	Serial.print(",");
	Serial.println(motor2Dir);
	state_last_change  = millis();
	motorState = newState;
}