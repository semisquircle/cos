#include "Arduino_BMI270_BMM150.h"

float SENSORS_RADS_TO_DPS = 57.29577793;

void setup() {
	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}
}

void loop() {
	float ax, ay, az;
	float gx, gy, gz;
	float mx, my, mz;

	if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() && IMU.magneticFieldAvailable()) {
		IMU.readAcceleration(ax, ay, az);
		IMU.readGyroscope(gx, gy, gz);
		IMU.readMagneticField(mx, my, mz);

		Serial.print("Raw:");
		Serial.print(int(ax * 8192 / 9.8)); Serial.print(",");
		Serial.print(int(ay * 8192 / 9.8)); Serial.print(",");
		Serial.print(int(az * 8192 / 9.8)); Serial.print(",");
		Serial.print(int(gx * SENSORS_RADS_TO_DPS * 16)); Serial.print(",");
		Serial.print(int(gy * SENSORS_RADS_TO_DPS * 16)); Serial.print(",");
		Serial.print(int(gz * SENSORS_RADS_TO_DPS * 16)); Serial.print(",");
		Serial.print(int(mx * 10)); Serial.print(",");
		Serial.print(int(my * 10)); Serial.print(",");
		Serial.print(int(mz * 10)); Serial.println("");

		Serial.print("Uni:");
		Serial.print(ax); Serial.print(",");
		Serial.print(ay); Serial.print(",");
		Serial.print(az); Serial.print(",");
		Serial.print(gx, 4); Serial.print(",");
		Serial.print(gy, 4); Serial.print(",");
		Serial.print(gz, 4); Serial.print(",");
		Serial.print(mx); Serial.print(",");
		Serial.print(my); Serial.print(",");
		Serial.print(mz); Serial.println("");
	}
}
