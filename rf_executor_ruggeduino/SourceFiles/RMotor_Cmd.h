
// Enable (PWM) outputs
#define EN1_PIN 3
#define EN2_PIN 11

// Direction outputs
#define DIR1_PIN 12
#define DIR2_PIN 13
class RMotor_Cmd
{
public: 
	RMotor_Cmd();
	enum enumSpeed {STOP=0, FAST=255, HALFSPEED=125};
	 enum enumDir {IDLE=0, FORWARD, LEFT,RIGHT, BACKWARD,COLLISION};
	 int currOp;
	 int motorState;
	 int currSpeed;
	 
	 long collisionAvoidanceTime;
	 long state_last_change;
	 
	 bool motor1Dir ;
	 bool motor2Dir;
	 
	 void Right(int speed);
	 void Left(int speed);
	 void Forward(int speed);
	 void Backward(int speed);
	 void Stop();
	void ReleaseMotors();
	void update(long currTime);
	void setState(int  newState);
	
	protected:
	void Motor(int direction,bool forward1 ,bool forward2 ,int speed);
};