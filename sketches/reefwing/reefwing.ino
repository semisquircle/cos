#include <ReefwingAHRS.h>
#include <Arduino_BMI270_BMM150.h>

ReefwingAHRS ahrs;
SensorData data;

void setup() {
	ahrs.begin();
	ahrs.setFusionAlgorithm(SensorFusion::MAHONY);
	ahrs.setDeclination(-9.5);

	Serial.begin(115200);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU");
		while (true);
	}
}

void loop() {
	if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable() && IMU.magneticFieldAvailable()) {
		IMU.readGyroscope(data.gx, data.gy, data.gz);
		IMU.readAcceleration(data.ax, data.ay, data.az);
		IMU.readMagneticField(data.mx, data.my, data.mz);

		ahrs.setData(data);
		ahrs.update();

		Serial.print(ahrs.angles.pitch, 2);
		Serial.print(",");
		Serial.print(ahrs.angles.yaw, 2);
		Serial.print(",");
		Serial.println(ahrs.angles.roll, 2);
	}
}