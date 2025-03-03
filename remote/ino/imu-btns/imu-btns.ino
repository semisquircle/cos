#include <Arduino_BMI270_BMM150.h>
#include "Madgwick2010.h"
#include <mLink.h>

float gx, gy, gz, ax, ay, az;
float gxOffset = 0, gyOffset = 0, gzOffset = 0;
float pitch, yaw, roll;
Madgwick2010 filter;
mLink mLink;

#define I2C_ADD BPAD_I2C_ADD

void setup() {
	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}

	filter.setSamplingRate(IMU.gyroscopeSampleRate());
	filter.setGyroMeasError(0.01 * DEG_TO_RAD);

	mLink.init();
	calibrateGyro();
}

void calibrateGyro() {
	Serial.println("Calibrating gyroscope...");

	const int numSamples = 2500;
	float sumX = 0, sumY = 0, sumZ = 0;

	int actualSample = 0;
	for (int i = 0; i < numSamples; i++) {
		IMU.readGyroscope(gx, gy, gz);
		sumX += gx;
		sumY += gy;
		sumZ += gz;
		actualSample++;
	}

	gxOffset = sumX / actualSample;
	gyOffset = sumY / actualSample;
	gzOffset = sumZ / actualSample;

	Serial.println("Finished calibration.");
}

String boolToString(bool value) {
	return value ? "true" : "false";
}

void loop() {
	if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable()) {
		IMU.readGyroscope(gx, gy, gz);
		IMU.readAcceleration(ax, ay, az);

		// Apply calibration
		gx -= gxOffset;
		gy -= gyOffset;
		gz -= gzOffset;

		// Degrees to radians
		gx *= DEG_TO_RAD;
		gy *= DEG_TO_RAD;
		gz *= DEG_TO_RAD;

		// Filter
		filter.updateIMU(gx, gy, gz, ax, ay, az);
		filter.getEulerAngles(pitch, yaw, roll);

		// Back to degrees
		pitch *= RAD_TO_DEG;
		yaw *= RAD_TO_DEG;
		roll *= RAD_TO_DEG;

		bool left = mLink.bPad_DownState(I2C_ADD);
		bool right = mLink.bPad_UpState(I2C_ADD);
		bool up = mLink.bPad_LeftState(I2C_ADD);
		bool down = mLink.bPad_RightState(I2C_ADD);
		bool select = mLink.bPad_SelectState(I2C_ADD);
		bool back = mLink.bPad_BackState(I2C_ADD);

		String json =
		"{\"imu\":{\"pitch\":" + String(pitch) +
		",\"yaw\":" + String(yaw) +
		",\"roll\":" + String(roll) +
		"},\"btns\":{\"dpad\":{\"left\":" + boolToString(left) +
		",\"up\":" + boolToString(up) +
		",\"right\":" + boolToString(right) +
		",\"down\":" + boolToString(down) +
		"},\"select\":" + boolToString(select) +
		",\"back\":" + boolToString(back) +
		"}}";
		Serial.println(json);
	}
}