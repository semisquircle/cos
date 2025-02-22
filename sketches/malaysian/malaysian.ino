#include "Arduino_BMI270_BMM150.h"
#include "SensorFusion.h"

SF fusion;
float ax, ay, az, gx, gy, gz, mx, my, mz;
float gyroXOffset = 0, gyroYOffset = 0, gyroZOffset = 0;
float deltat;

// Expected magnetic field strength (uT)
float M_scale = 27.4584;

// Hard-iron offset
float M_B[3]
	{-24.42, -13.83, 31.12};

// Soft-iron scale correction
float M_Ainv[3][3] {
	{1.097, 0.039, -0.018},
	{0.039, 0.991, 0.022},
	{-0.018, -0.022, 0.922}
};

float declinationDeg = -9.5;

void updateMag(float& mx, float& my, float& mz) {
	float temp[3];

	temp[0] = (mx - M_B[0]);
	temp[1] = (my - M_B[1]);
	temp[2] = (mz - M_B[2]);

	mx = M_Ainv[0][0] * temp[0] + M_Ainv[0][1] * temp[1] + M_Ainv[0][2] * temp[2];
	my = M_Ainv[1][0] * temp[0] + M_Ainv[1][1] * temp[1] + M_Ainv[1][2] * temp[2];
	mz = M_Ainv[2][0] * temp[0] + M_Ainv[2][1] * temp[1] + M_Ainv[2][2] * temp[2]; 
}

void applyScalingFactor(float& mx, float& my, float& mz, float expectedFieldStrength) {
	float currentMagnitude = sqrt(mx * mx + my * my + mz * mz);
	float scaleFactor = expectedFieldStrength / currentMagnitude;

	mx *= scaleFactor;
	my *= scaleFactor;
	mz *= scaleFactor;
}

void applyMagneticDeclination(float& mx, float& my) {
	float declinationRad = declinationDeg * (PI / 180.0);
	float mx_corrected = mx * cos(declinationRad) - my * sin(declinationRad);
	float my_corrected = mx * sin(declinationRad) + my * cos(declinationRad);

	mx = mx_corrected;
	my = my_corrected;
}

void calibrateGyro() {
	Serial.println("Calibrating gyroscope...");
	const int numSamples = 3000;
	float sumX = 0, sumY = 0, sumZ = 0;

	int actualSample = 0;
	for (int i = 0; i < numSamples; i++) {
		if (IMU.gyroscopeAvailable()) { IMU.readGyroscope(gx, gy, gz); }            // deg/s
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
	while (!Serial);

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU!");
		while (1);
	}

	calibrateGyro();
	Serial.println("Started");
}

void loop() {
	if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable() && IMU.magneticFieldAvailable()) {
		IMU.readGyroscope(gx, gy, gz);     // deg/s
		IMU.readAcceleration(ax, ay, az);  // g
		IMU.readMagneticField(mx, my, mz); // uT

		gx -= gyroXOffset;
		gy -= gyroYOffset;
		gz -= gyroZOffset;

		updateMag(mx, my, mz);
		applyScalingFactor(mx, my, mz, M_scale);
		applyMagneticDeclination(mx, my);

		ax *= 9.80665; ay *= 9.80665; az *= 9.80665;          // m/s2
		gx *= PI / 180.0; gy *= PI / 180.0; gz *= PI / 180.0; // rad/s

		deltat = fusion.deltatUpdate();
		fusion.MadgwickUpdate(gx, gy, gz, ax, ay, az, mx, -my, -mz, deltat);

		Serial.print(fusion.getPitch());
		Serial.print(",");
		Serial.print(fusion.getYaw());
		Serial.print(",");
		Serial.println(fusion.getRoll());
	}
}