#include <Arduino_BMI270_BMM150.h>
#include "NewMadgwick.h"
#include <mLink.h>

NewMadgwick filter;
mLink mLink;

#define I2C_ADD BPAD_I2C_ADD

void setup() {
	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}

	mLink.init();	
}

String boolToString(bool value) {
	return value ? "true" : "false";
}

void loop() {
	float gx, gy, gz;
	float ax, ay, az;
	float pitch, yaw, roll;

	if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable()) {
		IMU.readGyroscope(gx, gy, gz);
		IMU.readAcceleration(ax, ay, az);

		// Degrees to radians
		gx *= DEG_TO_RAD;
		gy *= DEG_TO_RAD;
		gz *= DEG_TO_RAD;

		// Filter
		filter.update(gx, gy, gz, ax, ay, az);
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
			"{\"pitch\":" + String(pitch) +
			",\"yaw\":" + String(yaw) +
			",\"roll\":" + String(roll) +
			",\"up\":" + boolToString(up) +
			",\"left\":" + boolToString(left) +
			",\"right\":" + boolToString(right) +
			",\"down\":" + boolToString(down) +
			",\"select\":" + boolToString(select) +
			",\"back\":" + boolToString(back) +
			"}";
		Serial.println(json);
	}
}