#ifndef MalaysianMadgwick_h
#define MalaysianMadgwick_h

#include <cmath>
#include "Arduino.h"

class MalaysianMadgwick {
public:
	MalaysianMadgwick();
	void updateIMU(float gx, float gy, float gz, float ax, float ay, float az, float deltat);
	void updateMARG(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float deltat);
	bool initQuat(float ax, float ay, float az, float mx, float my, float mz);
	float deltatUpdate();
	void getQuaternions(float &qw, float &qx, float &qy, float &qz);
	void getEulerAngles(float &yaw, float &pitch, float &roll);

private:
	float beta;
	float q0, q1, q2, q3;
	float Now, lastUpdate, deltat;
	static float invSqrt(float x);
	void vectorCross(float A[3], float B[3], float cross[3]);
};

#endif
