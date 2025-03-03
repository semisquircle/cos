#ifndef NEW_MADGWICK_H
#define NEW_MADGWICK_H

#include <cmath>

class Madgwick2010 {
private:
	// System constants
	float deltat = 0.001f; // sampling period in seconds (shown as 1 ms)
	float gyroMeasError = 5.0f * (M_PI / 180.0f); // gyroscope measurement error in rad/s (shown as 5 deg/s)
	float gyroMeasDrift = 0.2f * (M_PI / 180.0f); // gyroscope measurement error in rad/s/s (shown as 0.2f deg/s/s)
	float beta = sqrt(3.0f / 4.0f) * gyroMeasError; // compute beta
	float zeta = sqrt(3.0f / 4.0f) * gyroMeasDrift; // compute zeta

public:
	// Global system variables
	float a_x, a_y, a_z; // accelerometer measurements
	float w_x, w_y, w_z; // gyroscope measurements in rad/s
	float m_x, m_y, m_z; // magnetometer measurements

	// Estimated orientation quaternion elements with initial conditions
	float SEq_1 = 1.0f,
		  SEq_2 = 0.0f,
		  SEq_3 = 0.0f,
		  SEq_4 = 0.0f;

	float b_x = 1, b_z = 0; // reference direction of flux in earth frame
	float w_bx = 0, w_by = 0, w_bz = 0;

	void setSamplingRate(float rate);
	void setGyroMeasError(float error);
	void setGyroDriftError(float error);

	void updateIMU(float w_x, float w_y, float w_z, float a_x, float a_y, float a_z);
	void updateMARG(float w_x, float w_y, float w_z, float a_x, float a_y, float a_z, float m_x, float m_y, float m_z);
	
	void getQuaternions(float &q1, float &q2, float &q3, float &q4);
	void getEulerAngles(float &pitch, float &yaw, float &roll);
};

#endif
