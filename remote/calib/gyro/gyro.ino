#include <Arduino_BMI270_BMM150.h>

void setup() {
	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}
}

void loop() {
	float gx, gy, gz;

	if (IMU.gyroscopeAvailable()) {
		IMU.readGyroscope(gx, gy, gz);

		Serial.print(gx, 6);
		Serial.print(',');
		Serial.print(gy, 6);
		Serial.print(',');
		Serial.println(gz, 6);
	}

	delay(10);
}