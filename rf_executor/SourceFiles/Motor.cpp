#include <Arduino.h>
//include <ServoTimer2.h>


#ifndef MOTOR_H
#define MOTOR_H
#include "Motor.h"
#endif


 Motor::Motor()
{
	direction = STATE_FORWARD;
	motorState= STATE_IDLE;
	wheelState = STATE_FORWARD ;
	state_last_change = 0;
	wheel_last_change  = 0;
	motorSpeed= FAST;
	lastUpdate =0;
	collisionAvoidanceTime = 3000;
}



void Motor::ReleaseMotors(){
  motor(1, RELEASE, 0);
  motor(2, RELEASE, 0);
  motor(4, RELEASE, 0);
  motor(3, RELEASE, 0);
  delay(5);
}



// ---------------------------------
// motor
//
// Select the motor (1-4), the command,
// and the speed (0-255).
// The commands are: FORWARD, BACKWARD, BRAKE, RELEASE.
//
void Motor::motor(int nMotor, int command, int speed)
{
  int motorA, motorB;

  if (nMotor >= 1 && nMotor <= 4)
  {  
    switch (nMotor)
    {
    case 1:
      motorA   = MOTOR1_A;
      motorB   = MOTOR1_B;
      break;
    case 2:
      motorA   = MOTOR2_A;
      motorB   = MOTOR2_B;
      break;
    case 3:
      motorA   = MOTOR3_A;
      motorB   = MOTOR3_B;
      break;
    case 4:
      motorA   = MOTOR4_A;
      motorB   = MOTOR4_B;
      break;
    default:
      break;
    }

    switch (command)
    {
    case FORWARD:
      motor_output (motorA, HIGH, speed);
      motor_output (motorB, LOW, -1);     // -1: no PWM set
      break;
    case BACKWARD:
      motor_output (motorA, LOW, speed);
      motor_output (motorB, HIGH, -1);    // -1: no PWM set
      break;
    case RELEASE:
      motor_output (motorA, LOW, 0);  // 0: output floating.
      motor_output (motorB, LOW, -1); // -1: no PWM set
      break;
    default:
      break;
    }
  }
}


// ---------------------------------
// motor_output
//
// The function motor_ouput uses the motor driver to
// drive normal outputs like lights, relays, solenoids,
// DC motors (but not in reverse).
//
// It is also used as an internal helper function
// for the motor() function.
//
// The high_low variable should be set 'HIGH'
// to drive lights, etc.
// It can be set 'LOW', to switch it off,
// but also a 'speed' of 0 will switch it off.
//
// The 'speed' sets the PWM for 0...255, and is for
// both pins of the motor output.
//   For example, if motor 3 side 'A' is used to for a
//   dimmed light at 50% (speed is 128), also the
//   motor 3 side 'B' output will be dimmed for 50%.
// Set to 0 for completelty off (high impedance).
// Set to 255 for fully on.
// Special settings for the PWM speed:
//    Set to -1 for not setting the PWM at all.
//
void Motor::motor_output (int output, int high_low, int speed)
{
  int motorPWM;

  switch (output)
  {
  case MOTOR1_A:
  case MOTOR1_B:
    motorPWM = MOTOR1_PWM;
    break;
  case MOTOR2_A:
  case MOTOR2_B:
    motorPWM = MOTOR2_PWM;
    break;
  case MOTOR3_A:
  case MOTOR3_B:
    motorPWM = MOTOR3_PWM;
    break;
  case MOTOR4_A:
  case MOTOR4_B:
    motorPWM = MOTOR4_PWM;
    break;
  default:
    // Use speed as error flag, -3333 = invalid output.
    speed = -3333;
    break;
  }

  if (speed != -3333)
  {

  

    /*
      Serial.print(" output =  ");
   Serial.println(output);
     Serial.print("  MOTOLATCH: ");
  Serial.print(MOTORLATCH);
  
  Serial.print("   MOTORENABLE: ");
  Serial.print(MOTORENABLE);
  Serial.print("    MOTORDATA: ");
  Serial.print(MOTORDATA);
  Serial.print("   MOTORCLK: ");
  Serial.println(MOTORCLK);  
    */
    // Set the direction with the shift register
    // on the MotorShield, even if the speed = -1.
    // In that case the direction will be set, but
    // not the PWM.
    shiftWrite(output, high_low);

    // set PWM only if it is valid
    if ( (speed >= 0 && speed <= 255) )	
    {
      analogWrite(motorPWM, speed);
    }
	
	
  }
}


// ---------------------------------
// shiftWrite
//
// The parameters are just like digitalWrite().
//
// The output is the pin 0...7 (the pin behind
// the shift register).
// The second parameter is HIGH or LOW.
//
// There is no initialization function.
// Initialization is automatically done at the first
// time it is used.
//
void Motor::shiftWrite(int output, int high_low)
{
  static int latch_copy;
  static int shift_register_initialized = false;


  // Do the initialization on the fly,
  // at the first time it is used.
  if (!shift_register_initialized)
  {
    // Set pins for shift register to output
    pinMode(MOTORLATCH, OUTPUT);
    pinMode(MOTORENABLE, OUTPUT);
    pinMode(MOTORDATA, OUTPUT);
    pinMode(MOTORCLK, OUTPUT);

    // Set pins for shift register to default value (low);
    digitalWrite(MOTORDATA, LOW);
    digitalWrite(MOTORLATCH, LOW);
    digitalWrite(MOTORCLK, LOW);
    // Enable the shift register, set Enable pin Low.
    digitalWrite(MOTORENABLE, LOW);

    // start with all outputs (of the shift register) low
    latch_copy = 0;

    shift_register_initialized = true;
  }

  // The defines HIGH and LOW are 1 and 0.
  // So this is valid.
  bitWrite(latch_copy, output, high_low);

  // Use the default Arduino 'shiftOut()' function to
  // shift the bits with the MOTORCLK as clock pulse.
  // The 74HC595 shiftregister wants the MSB first.
  // After that, generate a latch pulse with MOTORLATCH.
  shiftOut(MOTORDATA, MOTORCLK, MSBFIRST, latch_copy);
 // delayMicroseconds(5);    // For safety, not really needed.
  digitalWrite(MOTORLATCH, HIGH);
 // delayMicroseconds(5);    // For safety, not really needed.
  digitalWrite(MOTORLATCH, LOW);
  
}

void Motor::Stop(){
	ReleaseMotors();
}




void Motor::Forward(int speed){
	if(speed<1 || speed>255){
		return;
	}

	motor(1, FORWARD, speed);
	motor(2, FORWARD, speed);
	motor(4, FORWARD, speed);
	motor(3, FORWARD, speed);
}


void Motor::Backward(int speed){
	if(speed<1 || speed>255){
		return;
	}
	
	motor(1, BACKWARD, speed);
	motor(2, BACKWARD, speed);
	motor(4, BACKWARD, speed);
	motor(3, BACKWARD, speed);
}



void Motor::setState(int newState)
{
	motorState = newState;
	state_last_change = millis();
}

void Motor::setWheelState(int newState)
{
	wheelState = newState;
	wheel_last_change = millis();
}

void Motor::update(long currTime)
{

	if( (currTime - lastUpdate) >10)
	{
	
		if(motorState == STATE_COLLISION){
			motorState = ( currTime-state_last_change )> collisionAvoidanceTime
				? STATE_IDLE //auto set state to forward after 3 seconds
				: STATE_COLLISION;
			TurnRight(currTime);
		}

		if(motorState == STATE_TURN_RIGHT){
			TurnRight(currTime);
		}
		
		if( motorState == STATE_TURN_LEFT)
		{
			TurnLeft(currTime);
		}
		if(motorState == STATE_FORWARD)
		{
			Forward(FAST);
		}
		if(motorState == STATE_BACKWARD)
		{
			Backward(FAST);
		}
		if(motorState == STATE_IDLE)
		{
			Stop();
		}
		
		lastUpdate = currTime;
	}
    
 
}



void Motor::TurnLeft(long currTime)
{
	long delta = currTime - wheel_last_change;
	if(delta >EVASIVE_TIME)
	{
		if(direction == STATE_FORWARD
		||  wheelState ==WHEEL_STATE_FORWARD_LEFT)
		{
			Stop();
			motor(1, BACKWARD, FAST);
			motor(2, BACKWARD, FAST);
			motor(3, FORWARD, 0);
			motor(4, FORWARD, 0);
			direction = STATE_BACKWARD;
			setWheelState(WHEEL_STATE_BACKWARD_LEFT);
		}else 
		if(direction == STATE_BACKWARD
		|| wheelState == WHEEL_STATE_BACKWARD_LEFT)
		{
			Stop();
			motor(1, BACKWARD, 0);
			motor(2, BACKWARD, 0);
			motor(3, FORWARD, FAST);
			motor(4, FORWARD, FAST);
			direction = STATE_FORWARD;
			setWheelState(WHEEL_STATE_FORWARD_LEFT);
		}
	}
}


void Motor::TurnRight(long currTime)
{
  long delta = currTime - wheel_last_change;

	if(delta >EVASIVE_TIME)
	{
		if(direction == STATE_FORWARD
		||  wheelState ==WHEEL_STATE_FORWARD_RIGHT)
		{
			Stop();
			motor(1, FORWARD, 0);
			motor(2, FORWARD, 0);
			motor(4, BACKWARD, FAST);
			motor(3, BACKWARD, FAST);
			direction = STATE_BACKWARD;
			setWheelState(WHEEL_STATE_BACKWARD_RIGHT);
		} else
		if(direction == STATE_BACKWARD
		||  wheelState == WHEEL_STATE_BACKWARD_RIGHT)
		{
			Stop();
			motor(1, FORWARD, FAST);
			motor(2, FORWARD, FAST);
			motor(4, BACKWARD, 0);
			motor(3, BACKWARD, 0);
			direction = STATE_FORWARD;
			setWheelState(WHEEL_STATE_FORWARD_RIGHT);
		}

	}
}




void Motor::setMotorSpeed(int newMotorSpeed)
{
	motorSpeed = newMotorSpeed;
}

