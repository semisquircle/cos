#include <Arduino_BMI270_BMM150.h>

void setup() {
	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}
}

void loop() {
	float ax, ay, az;

	if (IMU.accelerationAvailable()) {
		IMU.readAcceleration(ax, ay, az);

		Serial.print(ax, 6);
		Serial.print(',');
		Serial.print(ay, 6);
		Serial.print(',');
		Serial.println(az, 6);
	}

	delay(10);
}