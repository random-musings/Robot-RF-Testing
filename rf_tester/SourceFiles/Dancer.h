#ifndef MOTOR_H
#define MOTOR_H
#include "Motor.h"
#endif

class Dancer
{

	public:
		Dancer();
		~Dancer();
		String createDance();
		void performDance(char* newDance);
		void setup(Motor *newMotor);
		void stop();
		void update(long currTime);
    const int danceLen = 7;
    bool dancing;
    char* dance;
    Motor* motor;
	const int actionLen =5;
		protected:
		const char* delim=",";
		const char* sep=":";
		



		long lastPlay;
		char *  strwrd(char *s, char *buf, size_t len, char *delim);	
		int currDanceIx;
		long currPause;

};
