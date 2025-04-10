#include <ArduinoBLE.h>
#include <Arduino_BMI270_BMM150.h>
#include <MalaysianMadgwick.h>
#include <mLink.h>
#include <ArduinoJson.h>

// BLE
BLEService bleService("19b10000-e8f2-537e-4f6c-d104768a1214");
BLEStringCharacteristic bleCharacteristic("19b10010-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify, 256);

// IMU/fusion
MalaysianMadgwick fusion;
float ax, ay, az, gx, gy, gz, mx, my, mz;
float gxOffset = 0, gyOffset = 0, gzOffset = 0;
float deltat;
float yaw, pitch, roll;

const float EARTH_G = 9.80665;		// m/s²
const float M_FIELD = 43.636392;	// μT
const float M_DEC = -8.661414;		// deg

const float M_HARD[3] = {-30.115516, 49.942568, -25.54026};
const float M_SOFT[3][3] = {
	{1.307718, 0.001586, 0.064617},
	{0.001586, 1.239569, -0.053351},
	{0.064617, -0.053351, 0.717372}
};

// mLink/buttons
mLink mLink;
#define I2C_ADD BPAD_I2C_ADD


// Calibration functions
void calibrateGyro() {
	Serial.println("Calibrating gyroscope...");
	const int numSamples = 2000;
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

	Serial.print("Finished, ");
	Serial.print(actualSamples);
	Serial.println(" samples");

	Serial.print("X: ");
	Serial.print(gxOffset);
	Serial.print(", Y: ");
	Serial.print(gyOffset);
	Serial.print(", Z: ");
	Serial.print(gzOffset);
	Serial.println();
}

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
	float mxCorrected = mx * cos(decRad) - my * sin(decRad);
	float myCorrected = mx * sin(decRad) + my * cos(decRad);

	mx = mxCorrected;
	my = myCorrected;
}


// The real meat and cheese to run once the remote is connected
String compileData() {
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

	// Convert accelerometer data to m/s²
	ax *= EARTH_G;
	ay *= EARTH_G;
	az *= EARTH_G;

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

	// Populate JSON document
	JsonDocument doc;
	JsonObject euler = doc.createNestedObject("euler");
	euler["yaw"] = yaw;
	euler["pitch"] = pitch;
	euler["roll"] = roll;

	JsonObject accel = doc.createNestedObject("accel");
	accel["x"] = ax;
	accel["y"] = ay;
	accel["z"] = az;

	// Get button states (assuming dpad is rotated -90 degrees)
	JsonObject btns = doc.createNestedObject("btns");
	JsonObject dpad = btns.createNestedObject("dpad");
	dpad["left"] = mLink.bPad_DownState(I2C_ADD);
	dpad["up"] = mLink.bPad_LeftState(I2C_ADD);
	dpad["right"] = mLink.bPad_UpState(I2C_ADD);
	dpad["down"] = mLink.bPad_RightState(I2C_ADD);
	btns["select"] = mLink.bPad_SelectState(I2C_ADD);
	btns["back"] = mLink.bPad_BackState(I2C_ADD);
	/* dpad["left"] = false;
	dpad["up"] = false;
	dpad["right"] = false;
	dpad["down"] = false;
	btns["select"] = false;
	btns["back"] = false; */

	// Return JSON as string
	String output;
	serializeJson(doc, output);
	return output;
}


void setup() {
	Serial.begin(115200);
	while (!Serial);

	if (!BLE.begin()) {
		Serial.println("Failed to initialize BLE!");
		while (1);
	}

	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU!");
		while (1);
	}

	BLE.setLocalName("IMU Remote");
	BLE.setAdvertisedService(bleService);
	bleService.addCharacteristic(bleCharacteristic);
	BLE.addService(bleService);
	bleCharacteristic.writeValue("0");
	BLE.advertise();

	calibrateGyro();

	mLink.init();

	Serial.println("Bluetooth device active, waiting for connections...");
}


void loop() {
	BLEDevice central = BLE.central();

	if (central) {
		Serial.print("Connected to central: ");
		Serial.println(central.address());

		while (central.connected()) {
			if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable() && IMU.magneticFieldAvailable()) {
				String json = compileData();
				Serial.println(json);
				bleCharacteristic.writeValue(json);
			}
		}

		Serial.print("Disconnected from central: ");
		Serial.println(central.address());
	}
}
