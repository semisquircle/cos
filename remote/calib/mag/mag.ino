#include <Arduino_BMI270_BMM150.h>

void setup() {
	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}
}

void loop() {
	float mx, my, mz;

	if (IMU.magneticFieldAvailable()) {
		IMU.readMagneticField(mx, my, mz);

		Serial.print(mx, 6);
		Serial.print(',');
		Serial.print(my, 6);
		Serial.print(',');
		Serial.println(mz, 6);
	}

	delay(10);
}