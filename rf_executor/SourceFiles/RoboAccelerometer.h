

class RoboAccelerometer
{
	public:
		RoboAccelerometer();
		void init(int sampleSize,
               int offsetX, int offsetY, int offsetZ,
               double newAttackTolerance, double newCollisionTolerance, double newMovingTolerance,
                long newAccelShakeTimeout);
		void setBaseMarkers(int sampleSize);
		void update(long currentTime);
		void calcAngle();
		void resetValues(long currTime);
		long lastReadShake;
		long accelShakeTimeout;
		Accelerometer acc;
		double aX, aY, aZ;
		double accel_center_x,accel_center_y,accel_center_z;
		double angle_z;
		bool collision;
		bool attacked;
		bool moving;
		bool accelAvail;
		double collisionTolerance;
		double movingTolerance;
		double attackTolerance;
};
