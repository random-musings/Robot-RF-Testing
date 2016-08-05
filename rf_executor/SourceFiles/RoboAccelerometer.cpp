#include "arduino.h"
#include <Wire.h>
#include "ADXL345lib.h"
#include "RoboAccelerometer.h"


	RoboAccelerometer::RoboAccelerometer()
	{
		accelAvail = false;
		lastReadShake = 0;
		angle_z = 0;
		collision = false;
		moving = false;
	}
	

void RoboAccelerometer::init(int sampleSize,
								int offsetX, int offsetY, int offsetZ,
								double newAttackTolerance, 
								double newCollisionTolerance, 
								double newMovingTolerance,
								long newAccelShakeTimeout)
	{
	
		if (acc.begin(OSEPP_ACC_SW_ON) != 0){			return;		}
		acc.setSensitivity(ADXL345_RANGE_PM4G);
		acc.setOffsets(offsetX,offsetY,offsetZ);
		accelAvail = true;
		setBaseMarkers(sampleSize);
		collisionTolerance = newCollisionTolerance;
		movingTolerance = newMovingTolerance;
		attackTolerance  =  newAttackTolerance;
		accelShakeTimeout = newAccelShakeTimeout;
/*		Serial.println("Accelerometer baseline");
		Serial.print(accel_center_x);
		Serial.print(",");
		Serial.print(accel_center_y);
		Serial.print(",");
		Serial.print(accel_center_z);
		Serial.print("  ");
		Serial.println("______________");
		*/
	}
		
		
	void RoboAccelerometer::setBaseMarkers(int sampleSize)
	{
		if(!accelAvail){
			Serial.println("Accelerometer not available");
			return;
		}
   double aX,aY,aZ;
		accel_center_x = 0;
		accel_center_y = 0;
		accel_center_z = 0;
		angle_z=0;
		int ix=0;
		while(ix<sampleSize){
		  acc.readGs(&aX,&aY,&aZ);
		  ix++;
		  accel_center_x += aX;
		  accel_center_y += aY;
		  accel_center_z += aZ;
		  delay(10);
		}
		accel_center_x = accel_center_x/sampleSize;
		accel_center_y = accel_center_y/sampleSize;
		accel_center_z = accel_center_z/sampleSize;
	}
		
		
	void RoboAccelerometer::update(long currTime)
	{
      
		  bool acc_shake = false;
			acc.readGs(&aX, &aY, &aZ);
			Serial.print(aX);
			Serial.print(",");
			Serial.print(aY);
			Serial.print(",");
			Serial.print(aZ);
			Serial.print("  ");
			
			acc_shake = ( abs(abs(aY)- abs(accel_center_y)) >= movingTolerance
			  || abs(abs(aZ)- abs(accel_center_z)) >= movingTolerance 
			  || abs(abs(aX)- abs(accel_center_x)) >= movingTolerance);
			  
			collision =(abs((abs(aY)- abs(accel_center_y))) >= collisionTolerance
  			|| abs((abs(aX)- abs(accel_center_x))) >= collisionTolerance
	  		|| abs((abs(aZ)- abs(accel_center_z))) >= collisionTolerance);

       attacked = (abs((abs(aY)- abs(accel_center_y))) >= attackTolerance
        || abs((abs(aX)- abs(accel_center_x))) >= attackTolerance
        || abs((abs(aZ)- abs(accel_center_z))) >= attackTolerance);
		
	
		if(collision){ Serial.println(" Collision");}
        
			lastReadShake   = acc_shake? currTime:lastReadShake;
			moving  = acc_shake || (currTime -lastReadShake)< accelShakeTimeout;
			calcAngle();
	}
		
	void RoboAccelerometer::calcAngle()
	{
	   float  y_val, z_val, result;
	   double y2, z2; //24 bit
     angle_z =0;
	   y_val = (float)aY-(float)accel_center_y;
	   z_val = (float)aZ-(float)accel_center_z;
	   y2 = (  double)(y_val*y_val);
	   z2 = (  double)(z_val*z_val);
		if( abs(z2+y2 )> 0.02){
			  result=sqrt(z2+y2);
			  result=z_val/result;
			  angle_z= atan(result);
		}
	}
		
	void RoboAccelerometer::resetValues(long currentTime)
	{
		collision = false;
		moving = true;
		lastReadShake =currentTime;
	}
	

	