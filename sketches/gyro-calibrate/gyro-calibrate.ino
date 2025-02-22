#include <Arduino_BMI270_BMM150.h>
#include <MadgwickAHRS.h>

float ax, ay, az;
float gx, gy, gz;
float gyroXOffset = 0, gyroYOffset = 0, gyroZOffset = 0;
Madgwick filter;

void calibrateGyro() {
	Serial.println("Calibrating gyroscope...");
	const int numSamples = 3000;
	float sumX = 0, sumY = 0, sumZ = 0;

	int actualSample = 0;
	for (int i = 0; i < numSamples; i++) {
		if (IMU.gyroscopeAvailable()) { IMU.readGyroscope(gx, gy, gz); }
		else { continue; }
		sumX += gx; sumY += gy; sumZ += gz;
		delay(3);
		actualSample++;
	}

	gyroXOffset = sumX / actualSample;
	gyroYOffset = sumY / actualSample;
	gyroZOffset = sumZ / actualSample;

	Serial.println("Finished");
	Serial.println(actualSample);
	Serial.println(gyroXOffset);
	Serial.println(gyroYOffset);
	Serial.println(gyroZOffset);
}

void setup() {
	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}

	filter.begin(IMU.gyroscopeSampleRate());
	calibrateGyro();
	Serial.println("Started");
}

void loop() {
	if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
		IMU.readAcceleration(ax, ay, az);
		IMU.readGyroscope(gx, gy, gz);

		gx -= gyroXOffset;
		gy -= gyroYOffset;
		gz -= gyroZOffset;

		filter.updateIMU(gx, gy, gz, ax, ay, az);

		Serial.print(filter.getPitch());
		Serial.print(",");
		Serial.print(filter.getYaw());
		Serial.print(",");
		Serial.println(filter.getRoll());
	}
}