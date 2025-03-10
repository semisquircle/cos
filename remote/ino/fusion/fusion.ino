#include "Arduino_BMI270_BMM150.h"
#include "MalaysianMadgwick.h"


MalaysianMadgwick fusion;
float ax, ay, az, gx, gy, gz, mx, my, mz;
float gxOffset = 0, gyOffset = 0, gzOffset = 0;
float deltat;
float yaw, pitch, roll;

const float LOCAL_G = 9.801815;		// m/s²
const float M_FIELD = 48.113397;	// μT
const float M_DEC = -11.415502;		// deg

const float M_HARD[3] = {-30.115516, 49.942568, -25.54026};
const float M_SOFT[3][3] = {
	{1.307718, 0.001586, 0.064617},
	{0.001586, 1.239569, -0.053351},
	{0.064617, -0.053351, 0.717372}
};


void applyMagIron(float& mx, float& my, float& mz) {
	float temp[3];

	temp[0] = (mx - M_HARD[0]);
	temp[1] = (my - M_HARD[1]);
	temp[2] = (mz - M_HARD[2]);

	mx = M_SOFT[0][0] * temp[0] + M_SOFT[0][1] * temp[1] + M_SOFT[0][2] * temp[2];
	my = M_SOFT[1][0] * temp[0] + M_SOFT[1][1] * temp[1] + M_SOFT[1][2] * temp[2];
	mz = M_SOFT[2][0] * temp[0] + M_SOFT[2][1] * temp[1] + M_SOFT[2][2] * temp[2]; 
}


void applyMagScaleFactor(float& mx, float& my, float& mz) {
	float currentMagnitude = sqrt(mx * mx + my * my + mz * mz);
	float scaleFactor = M_FIELD / currentMagnitude;

	mx *= scaleFactor;
	my *= scaleFactor;
	mz *= scaleFactor;
}


void applyMagDec(float& mx, float& my) {
	float decRad = M_DEC * DEG_TO_RAD;
	float mx_corrected = mx * cos(decRad) - my * sin(decRad);
	float my_corrected = mx * sin(decRad) + my * cos(decRad);

	mx = mx_corrected;
	my = my_corrected;
}


void calibrateGyro() {
	Serial.println("Calibrating gyroscope...");
	const int numSamples = 3000;
	float sumX = 0, sumY = 0, sumZ = 0;

	int actualSamples = 0;
	for (int i = 0; i < numSamples; i++) {
		if (IMU.gyroscopeAvailable()) { IMU.readGyroscope(gx, gy, gz); }
		else { continue; }
		sumX += gx;
		sumY += gy;
		sumZ += gz;
		delay(3);
		actualSamples++;
	}

	gxOffset = sumX / actualSamples;
	gyOffset = sumY / actualSamples;
	gzOffset = sumZ / actualSamples;

	Serial.println("Finished");
	Serial.println(actualSamples);
	Serial.println(gxOffset);
	Serial.println(gyOffset);
	Serial.println(gzOffset);
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
		IMU.readGyroscope(gx, gy, gz);
		IMU.readAcceleration(ax, ay, az);
		IMU.readMagneticField(mx, my, mz);

		// Apply gyroscope calibration
		gx -= gxOffset;
		gy -= gyOffset;
		gz -= gzOffset;

		// Convert gyroscope data to radians
		gx *= DEG_TO_RAD;
		gy *= DEG_TO_RAD;
		gz *= DEG_TO_RAD;

		// Convert accelerometer data to local gravity
		ax *= LOCAL_G;
		ay *= LOCAL_G;
		az *= LOCAL_G;

		// Apply magnetometer calibration
		applyMagIron(mx, my, mz);
		applyMagScaleFactor(mx, my, mz);
		applyMagDec(mx, my);

		// Update filter
		deltat = fusion.deltatUpdate();
		fusion.updateMARG(-gx, -gy, -gz, ax, ay, az, my, -mx, -mz, deltat);
		fusion.getEulerAngles(yaw, pitch, roll);

		// Convert angles back to degrees
		yaw *= RAD_TO_DEG;
		pitch *= RAD_TO_DEG;
		roll *= RAD_TO_DEG;

		String json =
		"{\"imu\":{\"yaw\":" + String(yaw) +
		",\"pitch\":" + String(pitch) +
		",\"roll\":" + String(roll) +
		"},\"btns\":{\"dpad\":{\"left\":false" +
		",\"up\":false" +
		",\"right\":false" +
		",\"down\":false" +
		"},\"select\":false" +
		",\"back\":false" +
		"}}";
		Serial.println(json);
	}
}
