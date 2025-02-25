#ifndef NEW_MADGWICK_H
#define NEW_MADGWICK_H

#include <cmath>

class NewMadgwick {
private:
	// System constants
	float deltat = 0.001f; // sampling period in seconds (shown as 1 ms)
	float gyroMeasError = 5.0f * (M_PI / 180.0f); // gyroscope measurement error in rad/s (shown as 5 deg/s)
	float beta = std::sqrt(3.0f / 4.0f) * gyroMeasError; // compute beta

public:
    // Global system variables
    float a_x, a_y, a_z; // accelerometer measurements
    float w_x, w_y, w_z; // gyroscope measurements in rad/s

	// Estimated orientation quaternion elements with initial conditions
    float SEq_1 = 1.0f,
		  SEq_2 = 0.0f,
		  SEq_3 = 0.0f,
		  SEq_4 = 0.0f;

	void setSamplingRate(float rate);
	void setGyroError(float error);
    void update(float w_x, float w_y, float w_z, float a_x, float a_y, float a_z);
	void getEulerAngles(float &pitch, float &yaw, float &roll);
};

#endif
