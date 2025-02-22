#include <Arduino_BMI270_BMM150.h>
#include <MadgwickAHRS.h>

Madgwick filter;

void setup() {
	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}

	filter.begin(IMU.gyroscopeSampleRate());
}

void loop() {
	float ax, ay, az;
	float gx, gy, gz;

	if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
		IMU.readAcceleration(ax, ay, az);
		IMU.readGyroscope(gx, gy, gz);

		filter.updateIMU(gx, gy, gz, ax, ay, az);

		Serial.print(filter.getPitch());
		Serial.print(",");
		Serial.print(filter.getYaw());
		Serial.print(",");
		Serial.println(filter.getRoll());
	}
}