

//ORIGINAL
// Arduino pins for the shift register
#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8



 //ORIGINAL
// 8-bit bus after the 74HC595 shift register
// (not Arduino pins)
// These are used to set the direction of the bridge driver.
#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR3_A 5
#define MOTOR3_B 7
#define MOTOR4_A 0
#define MOTOR4_B 6


/* //CHANGED
// 16-bit bus ???? after the 74HC595 shift register
// (not Arduino pins)
// These are used to set the direction of the bridge driver.
#define MOTOR1_A 10
#define MOTOR1_B 11
#define MOTOR2_A 9
#define MOTOR2_B 12
#define MOTOR3_A 13
#define MOTOR3_B 15
#define MOTOR4_A 8
#define MOTOR4_B 14
*/



/* ORIGINAL
// Arduino pins for the PWM signals.
#define MOTOR1_PWM 11
#define MOTOR2_PWM 3
#define MOTOR3_PWM 6
#define MOTOR4_PWM 5
#define SERVO1_PWM 10
#define SERVO2_PWM 9
*/
// Arduino pins for the PWM signals.
#define MOTOR1_PWM 26
#define MOTOR2_PWM 27
#define MOTOR3_PWM 28
#define MOTOR4_PWM 29


// Codes for the motor function.
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4
#define LEFT 0
#define RIGHT 1

#define FAST 225
#define SLOW 50
#define HALF_SPEED 125

#define STATE_IDLE  0
#define STATE_FORWARD 1
#define STATE_COLLISION 2
#define STATE_TURN_RIGHT 3
#define STATE_TURN_LEFT 4
#define STATE_BACKWARD 5


#define EVASIVE_TIME 1000

#define WHEEL_STATE_FORWARD	1
#define WHEEL_STATE_FORWARD_RIGHT 2
#define WHEEL_STATE_FORWARD_LEFT 3
#define WHEEL_STATE_BACKWARD 4
#define WHEEL_STATE_BACKWARD_RIGHT 5
#define WHEEL_STATE_BACKWARD_LEFT 6

class Motor
{
	public:
		Motor();
	
		int direction;
		int motorState;
		int wheelState;
		int motorSpeed;
		long collisionAvoidanceTime;
	
		void Forward(int speed);
		void Backward(int speed);
		void Turn(bool rightTurn);
		void TurnLeft(long currTime);
		void TurnRight(long currTime);
		void Stop();
		void ReleaseMotors();

		void setMotorSpeed(int newMotorSpeed);
		void setState(int newState);
		void update(long currTime);

		
	private:
		long state_last_change;
		long wheel_last_change;
		long lastUpdate ;
		void setWheelState(int newState);
		
		void shiftWrite(int output, int high_low);
		void motor_output (int output, int high_low, int speed);
		void motor(int nMotor, int command, int speed);
		
};
